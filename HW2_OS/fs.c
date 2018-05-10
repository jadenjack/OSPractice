#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include "disk.h"
#include "fs.h"


#define SET '1'
#define RESET '0'

#define BITS_A_BYTE 8
#define DATA_REGION_START_INDEX 19

const int NUM_OF_BLOCKS = FS_DISK_CAPACITY/BLOCK_SIZE;
const int NUM_OF_BLOCKS_IN_INDIRECTBLOCK = BLOCK_SIZE/sizeof(int);

typedef enum _UPDATE_FLAG{
    ADD_DIR,
    ALOCATE_BLOCK,
    ALOCATE_INODE,
    FREE_BLOCK,
    FREE_INODE
}UPDATE_FLAG;


extern int fd;
const int NUMBER_OF_INODE = BLOCK_SIZE/sizeof(Inode);

FileDescTable* pFileDescTable = NULL;

void copyInode(int blkno, Inode* pInode, void (*fp)());
int initNewFile(int newInodeNum);
int addFileDir(char* target,int parentInodeNum);
int addNewFileBlock(int index, char* target, Inode* parentInode, int parentInodeNum, int* parentBlockNum, DirEntry* parentDE);
int getAvailableByte(int flow);
int getFileBlockNum(Inode* fInode, int offset);
void addBlockInInode(Inode* fInode,int inodeNum, int writeBlockNum);
int isOpened(int fileInodeNum);

int OpenFile(const char* szFileName, OpenFlag flag)
{
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    int* curInodeNum=(int *)malloc(sizeof(int));
    *curInodeNum = 0;
    int* parentBlockNum = (int *)malloc(sizeof(int));
    char target[MAX_NAME_LEN];
    memset(target,0,sizeof(target));

    if(flag == OPEN_FLAG_CREATE) {
        int parentInodeNum = getParentInode(szFileName, target, pInode, curInodeNum, parentBlockNum);
        int newInodeNum =addFileDir(target, parentInodeNum);

        if(newInodeNum==-1)
            return -1;
        return initNewFile(newInodeNum);
    }
    else if(flag==OPEN_FLAG_READWRITE){
        int parentInodeNum = getParentInode(szFileName, target, pInode, curInodeNum, parentBlockNum);
        return initNewFile(curInodeNum);
    }
    return -1;

}
int initNewFile(int newInodeNum){

    int i;
    for(i=0;i<MAX_FD_ENTRY_LEN;i++){
        if(pFileDescTable->file[i].bUsed==false){
            pFileDescTable->file[i].bUsed=true;
            pFileDescTable->file[i].inodeNum=newInodeNum;
            pFileDescTable->file[i].fileOffset=0;
            return i;
        }
    }
    return -1;
}

int addFileDir(char* target,int parentInodeNum){
    int i,j;
    int bnum;
    int newBlockNum;
    Inode* parentInode = (Inode*)malloc(BLOCK_SIZE);
    GetInode(parentInodeNum,parentInode);
    DirEntry de[NUM_OF_DIRENT_PER_BLOCK];

    if(isExistInDir(target, parentInode)==1)//there is same name file
        return -1;

    //travel direct pointers
    for(i=0;i<NUM_OF_DIRECT_BLOCK_PTR;i++){

        bnum = parentInode->dirBlockPtr[i];
        if(bnum==0){//add new direct pointer and new entry
            newBlockNum = addNewDirEntry(i,parentInode,parentInodeNum);
            parentInode->dirBlockPtr[i] = newBlockNum;
            parentInode->size+=BLOCK_SIZE;
            bnum = parentInode->dirBlockPtr[i];
            PutInode(parentInodeNum,parentInode);
        }
        DevReadBlock(bnum, de);

        //add new dir
        for(j=0;j<NUM_OF_DIRENT_PER_BLOCK;j++){//4
            if(strcmp(de[j].name,"")==0){
                int newInodeNum = addNewFileBlock(i,target,parentInode,parentInodeNum,bnum,de);
                return newInodeNum;
            }
        }
    }

    //--------indirect------

    //travel indirect block
    int indirectBlockNum = parentInode->indirBlockPtr;
    if(indirectBlockNum==0) {//does't allocate indirect Block yet
        //add new indirect Block
        newBlockNum = GetFreeBlockNum();
        updateFileSysInfo(ALOCATE_BLOCK);
        SetBlockBitmap(newBlockNum);
        indirectBlockNum = newBlockNum;
        bnum = newBlockNum;
        int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
        memset(indirectBlock,0,BLOCK_SIZE);
        parentInode->indirBlockPtr=indirectBlockNum;
        DevWriteBlock(bnum,indirectBlock);
        PutInode(parentInodeNum,parentInode);
    }

    int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
    DevReadBlock(indirectBlockNum,indirectBlock);

    for(i=0;i<NUM_OF_BLOCKS_IN_INDIRECTBLOCK;i++){
        bnum = indirectBlock[i];
        DirEntry de[NUM_OF_DIRENT_PER_BLOCK];

        if(bnum==0){//add new block for indirectBlock
            int newBlockNum = GetFreeBlockNum();
            SetBlockBitmap(newBlockNum);
            updateFileSysInfo(ALOCATE_BLOCK);
            parentInode->size+=BLOCK_SIZE;
            indirectBlock[i]=newBlockNum;
            DirEntry* newDirEntry = (DirEntry *)malloc(BLOCK_SIZE);
            memset(newDirEntry,0,BLOCK_SIZE);
            DevWriteBlock(newBlockNum,newDirEntry);
            DevWriteBlock(indirectBlockNum,indirectBlock);
        }
        bnum = indirectBlock[i];
        DevReadBlock(bnum, de);

        for(j=0;j<NUM_OF_DIRENT_PER_BLOCK;j++){
            if(strcmp(de[j].name,"")==0){//get empty dirent number
                int newInodeNum = addNewFileBlock(j,target,parentInode,parentInodeNum,bnum,de);
                DevWriteBlock(indirectBlockNum,indirectBlock);
                return newInodeNum;
            }
        }

    }
    return -1;
}

int addNewFileBlock(int index, char* target, Inode* parentInode, int parentInodeNum, int* parentBlockNum, DirEntry* parentDE){

    int newBlockNum = -1;
    int curBlockNum = parentInode->dirBlockPtr[index];
    int i;
    for(i=0;i<NUM_OF_DIRENT_PER_BLOCK;i++){
        if(strcmp(parentDE[i].name,"")==0){

            //save child DE
            int newInodeNum = GetFreeInodeNum();
            updateFileSysInfo(ALOCATE_INODE);
            SetInodeBitmap(newInodeNum);
            Inode* newInode = (Inode*)malloc(BLOCK_SIZE);
            newInode->type=FILE_TYPE_FILE;
            newInode->size=0;
            PutInode(newInodeNum,newInode);

            //save parent DE
            strcpy(parentDE[i].name,target);
            parentDE[i].inodeNum = newInodeNum;
            DevWriteBlock(parentBlockNum,parentDE);

            newBlockNum = GetFreeBlockNum();
            SetBlockBitmap(newBlockNum);
            updateFileSysInfo(ALOCATE_BLOCK);
            char* newFile = (char *)malloc(sizeof(BLOCK_SIZE));
            memset(newFile,0,sizeof(BLOCK_SIZE));

            DevWriteBlock(newBlockNum,newFile);
            DevWriteBlock(parentBlockNum,parentDE);

            newInode->dirBlockPtr[0]=newBlockNum;
            PutInode(newInodeNum,newInode);
            return newInodeNum;
        }
    }
    return -1;

}


int WriteFile(int fileDesc, char* pBuffer, int length)
{

    if(fileDesc<0||fileDesc>MAX_FD_ENTRY_LEN)
        return -1;
    if(pFileDescTable->file[fileDesc].bUsed==false)
        return -1;

    Inode* fInode = (Inode*)malloc(sizeof(Inode));
    int inodeNum = pFileDescTable->file[fileDesc].inodeNum;
    int offset = pFileDescTable->file[fileDesc].fileOffset;
    int stringLength = strlen(pBuffer);

    GetInode(inodeNum,fInode);
    char* writeBlock = (char *)malloc(BLOCK_SIZE);
    int writeBlockNum=0;
    int flow = 0;
    int totalWriteSize = 0;
    int i;
    int saveFileBlockNum;

    //TODO : File Write Link with blocks
    while(totalWriteSize<length) {
        flow = offset+totalWriteSize;
        if (flow % BLOCK_SIZE == 0) {//no data or full block
            writeBlockNum = GetFreeBlockNum();
            DevWriteBlock(writeBlockNum, writeBlock);
            updateFileSysInfo(ALOCATE_BLOCK);
            SetBlockBitmap(writeBlockNum);
            addBlockInInode(fInode, inodeNum, writeBlockNum);
        }
        saveFileBlockNum = getFileBlockNum(fInode,flow);
        char *saveBlock = (char *) malloc(BLOCK_SIZE);
        DevReadBlock(saveFileBlockNum, saveBlock);

        int availableByte = getAvailableByte(flow);
        int writeStartIndex = BLOCK_SIZE-availableByte;
        for(i=0;i<availableByte;i++){
            saveBlock[writeStartIndex]=pBuffer[totalWriteSize];
            writeStartIndex++;
             totalWriteSize++;
            if(totalWriteSize>=length||totalWriteSize>=stringLength) {
                DevWriteBlock(saveFileBlockNum,saveBlock);
                break;
            }
        }
        DevWriteBlock(saveFileBlockNum,saveBlock);
        if(totalWriteSize>=length||totalWriteSize>=stringLength)
            break;
    }
    pFileDescTable->file[fileDesc].fileOffset+=totalWriteSize;
    fInode->size+=totalWriteSize;
    PutInode(inodeNum,fInode);
    return totalWriteSize;
}
int getAvailableByte(int flow){
    int div = flow/BLOCK_SIZE;
    int totalBlockSize = (div+1)*BLOCK_SIZE;
    return totalBlockSize - flow;
}
int getFileBlockNum(Inode* fInode, int offset){
    int div = offset/BLOCK_SIZE;
    if(div==0)
        return fInode->dirBlockPtr[0];
    if(div==1)
        return fInode->dirBlockPtr[1];

    int i;
    div-=2;
    int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
    DevReadBlock(fInode->indirBlockPtr,indirectBlock);
    return indirectBlock[div];
}

void addBlockInInode(Inode* fInode,int inodeNum, int writeBlockNum){
    int i;
    int newBlockNum;
    for(i=0;i<NUM_OF_DIRECT_BLOCK_PTR;i++){
        if(fInode->dirBlockPtr[i]==0){
            fInode->dirBlockPtr[i]=writeBlockNum;
            PutInode(inodeNum,fInode);
            return;
        }
    }

    int indirectBlockNum = fInode->indirBlockPtr;
    if(indirectBlockNum==0) {//does't allocate indirect Block yet
        //add new indirect Block
        newBlockNum = GetFreeBlockNum();
        updateFileSysInfo(ALOCATE_BLOCK);
        SetBlockBitmap(newBlockNum);
        indirectBlockNum = newBlockNum;
        int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
        memset(indirectBlock,0,BLOCK_SIZE);
        fInode->indirBlockPtr=indirectBlockNum;
        DevWriteBlock(indirectBlockNum,indirectBlock);
        PutInode(inodeNum,fInode);
    }
    int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
    int bnum;
    DevReadBlock(indirectBlockNum,indirectBlock);

    //link with indirect
    for(i=0;i<NUM_OF_BLOCKS_IN_INDIRECTBLOCK;i++){
        bnum = indirectBlock[i];

        if(bnum==0){//add new block for indirectBlock
            char* newFileBlock = (char *)malloc(BLOCK_SIZE);
            indirectBlock[i]=writeBlockNum;
            memset(newFileBlock,0,BLOCK_SIZE);
            DevWriteBlock(writeBlockNum,newFileBlock);
            DevWriteBlock(indirectBlockNum,indirectBlock);
            return;
        }

    }
    printf("OverFlow");
}

int ReadFile(int fileDesc, char* pBuffer, int length)
{
    Inode* fInode = (Inode*)malloc(sizeof(Inode));
    int inodeNum = pFileDescTable->file[fileDesc].inodeNum;
    int offset = pFileDescTable->file[fileDesc].fileOffset;
    //int offset = 0;
    GetInode(inodeNum,fInode);
    int size = fInode->size;

    char* readBlock = (char *)malloc(BLOCK_SIZE);
    int readBlockNum=0;
    int flow = 0;
    int totalReadSize = 0;
    int i;

    //TODO : File Write Link with blocks
    while(totalReadSize<length) {
        flow = offset+totalReadSize;
        if(flow>=size)
            break;
        readBlockNum = getFileBlockNum(fInode,flow);
        DevReadBlock(readBlockNum, readBlock);

        int availableByte = getAvailableByte(flow);
        int readStartIndex = BLOCK_SIZE-availableByte;
        char c;
        for(i=0;i<availableByte;i++){
            c =readBlock[readStartIndex];
            if(c==0)
                break;

            pBuffer[totalReadSize] = c;
            readStartIndex++;
            totalReadSize++;
            if(totalReadSize>=length) {
                break;
            }
        }
        if(totalReadSize>=length)
            break;
    }
    pFileDescTable->file[fileDesc].fileOffset+=totalReadSize;
    return totalReadSize;
}


int CloseFile(int fileDesc)
{
    //FileDesc targetFD = pFileDescTable->file[fileDesc];
    if(pFileDescTable->file[fileDesc].bUsed==true) {
        pFileDescTable->file[fileDesc].inodeNum = 0;
        pFileDescTable->file[fileDesc].fileOffset = 0;
        pFileDescTable->file[fileDesc].bUsed = false;
        return 0;
    }else
        return -1;

}

int RemoveFile(const char* szFileName)
{
    Inode* pInode = (Inode*)malloc(sizeof(Inode));

    int* curInodeNum=(int *)malloc(sizeof(int));
    *curInodeNum = 0;
    int* parentBlockNum = (int *)malloc(sizeof(int));
    char target[MAX_NAME_LEN];
    memset(target,0,sizeof(target));

    int parentInodeNum = getParentInode(szFileName,target,pInode,curInodeNum,parentBlockNum);
    int i;
    int bnum;

    if(*curInodeNum==-1){
        return -1;
    }

    //int fileInodeNum = getFileInodeNum(pInode,target);

    if(isOpened(*curInodeNum)==1)// file has been opened
        return -1;

    Inode* fInode = (Inode*)malloc(sizeof(Inode));
    GetInode(*curInodeNum,fInode);

    char* pBlock = (char *)malloc(BLOCK_SIZE);
    for(i=0;i<NUM_OF_DIRECT_BLOCK_PTR;i++){
        bnum = fInode->dirBlockPtr[i];
        if(bnum==0)
            break;
        DevReadBlock(bnum,pBlock);
        memset(pBlock,0,BLOCK_SIZE);
        DevWriteBlock(bnum,pBlock);
        ResetBlockBitmap(bnum);
        updateFileSysInfo(FREE_BLOCK);
    }
    bnum = fInode->indirBlockPtr;
    if(bnum!=0){
        int inbnum;
        int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
        DevReadBlock(bnum,indirectBlock);
        for(i=0;i<NUM_OF_BLOCKS_IN_INDIRECTBLOCK;i++){
            inbnum = indirectBlock[i];
            if(inbnum==0)
                break;
            DevReadBlock(inbnum,pBlock);
            memset(pBlock,0,BLOCK_SIZE);
            DevWriteBlock(inbnum,pBlock);
            ResetBlockBitmap(inbnum);
            updateFileSysInfo(FREE_BLOCK);
        }
        DevReadBlock(bnum,pBlock);
        memset(pBlock,0,BLOCK_SIZE);
        DevWriteBlock(bnum,pBlock);
        ResetBlockBitmap(bnum);
        updateFileSysInfo(FREE_BLOCK);
    }
    //free inode,
    ResetInodeBitmap(*curInodeNum);
    updateFileSysInfo(FREE_INODE);
    memset(pInode,0,BLOCK_SIZE);

    GetInode(*curInodeNum,pInode);
    disLinkWithParent(pInode,*curInodeNum,fInode);
    //dislink with parent
    return 0;
}
int isOpened(int fileInodeNum){
    int i;
    for(i=0;i<MAX_FD_ENTRY_LEN;i++){
        if(pFileDescTable->file[i].inodeNum == fileInodeNum)
            return 1;
    }
    return 0;
}

int getFileInodeNum(Inode* pInode,char *target){
    int i,j;
    int bnum;
    DirEntry de[NUM_OF_DIRENT_PER_BLOCK];
    for(i=0;i<NUM_OF_DIRECT_BLOCK_PTR;i++){
        bnum = pInode->dirBlockPtr[i];
        if(bnum!=0){
            DevReadBlock(bnum,de);
            for(j=0;j<NUM_OF_DIRENT_PER_BLOCK;j++){
                if(strcmp(de[j].name,target)==0){
                    return de[j].inodeNum;
                }
            }
        }
    }
    bnum = pInode->indirBlockPtr;
    int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
    int inbnum;
    DevReadBlock(bnum,indirectBlock);
    for(i=0;i<NUM_OF_BLOCKS_IN_INDIRECTBLOCK;i++){
        inbnum=indirectBlock[i];
        if(inbnum!=0){
            DevReadBlock(inbnum,de);
            for(j=0;j<NUM_OF_DIRENT_PER_BLOCK;j++){
                if(strcmp(de[j].name,target)==0){
                    return de[j].inodeNum;
                }
            }
        }
    }
    return -1;
}

void disLinkWithParent(Inode* parentInode,int parentInodeNum, int pInodeNum){
    int i,j;
    int bnum;

    //direct DE
    DirEntry de[NUM_OF_DIRENT_PER_BLOCK];
    for(i=0;i<NUM_OF_DIRECT_BLOCK_PTR;i++){
        bnum = parentInode->dirBlockPtr[i];
        DevReadBlock(bnum,de);
        for(j=0;j<NUM_OF_DIRENT_PER_BLOCK;j++){
            if(de[j].inodeNum==pInodeNum){
                de[j].inodeNum=0;
                memset(de[j].name,0,sizeof(de[j].name));
                //strcpy(de[j].name,"");
                DevWriteBlock(bnum,de);

                //if number of de equal 0, also dislink DE with parent;
                int numOfBlockInDE = getNumOfBlockInDE(de);
                if(numOfBlockInDE==0){
                    ResetBlockBitmap(bnum);
                    updateFileSysInfo(FREE_BLOCK);
                    parentInode->dirBlockPtr[i]=0;
                    parentInode->size-=BLOCK_SIZE;
                    PutInode(parentInodeNum,parentInode);
                }

                return;
            }
        }
    }
    int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
    int inbnum;
    bnum = parentInode->indirBlockPtr;
    DevReadBlock(bnum,indirectBlock);
    for(i=0;i<NUM_OF_BLOCKS_IN_INDIRECTBLOCK;i++){
        inbnum = indirectBlock[i];
        DevReadBlock(inbnum,de);
        for(j=0;j<NUM_OF_DIRENT_PER_BLOCK;j++){
            if(de[j].inodeNum==pInodeNum){
                de[j].inodeNum=0;
                strcpy(de[j].name,"");
                DevWriteBlock(inbnum,de);

                int numOfBlockInDE = getNumOfBlockInDE(de);
                //de has no child, free block
                if(numOfBlockInDE==0){
                    ResetBlockBitmap(inbnum);
                    updateFileSysInfo(FREE_BLOCK);
                    indirectBlock[i]=0;
                    parentInode->size-=BLOCK_SIZE;
                    DevWriteBlock(bnum,indirectBlock);
                }

                //dislink indirectBLOCK
                int numIndirectDE = getNumOfIndirectDE(indirectBlock);
                if(numIndirectDE==0){
                    ResetInodeBitmap(parentInode->indirBlockPtr);
                    updateFileSysInfo(FREE_INODE);
                    parentInode->indirBlockPtr=0;
                    PutInode(parentInodeNum,parentInode);
                }
                return;
            }
        }
    }
}
int getNumOfIndirectDE(int indirectBlock[]){
    int i;
    int count=0;
    for(i=0;i<NUM_OF_BLOCKS_IN_INDIRECTBLOCK;i++){
        if(indirectBlock[i]!=0){
            count++;
        }
    }
    return count;
}
int getNumOfBlockInDE(DirEntry* de){
    int i=0;
    int count=0;
    for(i=0;i<NUM_OF_DIRENT_PER_BLOCK;i++) {
        if (de[i].inodeNum == 0)
            count++;
    }
    return count;
}
int getParentInodeDir(char* szDirName,char* target, Inode* pInode, int* curInodeNum, int* parentBlockNum){

    //get root inode
    GetInode(0,pInode);
    int i;
    int nameIndex=0;

    memset(target,0,sizeof(target));
    //travel directory
    for(i=1;i<=strlen(szDirName);i++){
        if(i>=strlen(szDirName)){//end of string
            return *curInodeNum;
        }else if(szDirName[i] != '/'){
            target[nameIndex++] = szDirName[i];
        }else{// == '/'
            if(i==(strlen(szDirName)-1))//if last character equal '/'
                continue;
            *curInodeNum = goDownDir(target,pInode, curInodeNum,parentBlockNum);
            memset(target,0,MAX_NAME_LEN);
            nameIndex=0;
            if(curInodeNum==-1) {
                printf("Invalid Dir Name");
                return -1;
            }
        }
    }
}

int getParentInode(char* szDirName,char* target, Inode* pInode, int* curInodeNum, int* parentBlockNum){

    //get root inode
    GetInode(0,pInode);
    int i;
    int nameIndex=0;

    memset(target,0,sizeof(target));
    //travel directory
    for(i=1;i<=strlen(szDirName);i++){
        if(i>=strlen(szDirName)){//end of string
            int parentInodeNum = *curInodeNum;
            *curInodeNum = goDownDir(target,pInode, curInodeNum,parentBlockNum);
            return parentInodeNum;
        }else if(szDirName[i] != '/'){
            target[nameIndex++] = szDirName[i];
        }else{// == '/'
            *curInodeNum = goDownDir(target,pInode, curInodeNum,parentBlockNum);
             memset(target,0,MAX_NAME_LEN);
            nameIndex=0;
            if(curInodeNum==-1) {
                printf("Invalid Dir Name");
                return -1;
            }
        }
    }
}

int MakeDir(const char* szDirName)
{
    Inode* pInode = (Inode*)malloc(sizeof(Inode));

    int* curInodeNum=(int *)malloc(sizeof(int));
    *curInodeNum = 0;
    int* parentBlockNum = (int *)malloc(sizeof(int));
    char target[MAX_NAME_LEN];
    memset(target,0,sizeof(target));

    int parentInodeNum = getParentInodeDir(szDirName,target,pInode,curInodeNum,parentBlockNum);
    return addNewDir(target,pInode,parentInodeNum);

}

int addNewDir(char* target, Inode* parentInode,int parentInodeNum){
    int i,j;
    int bnum;
    int newBlockNum = -1;
    int hasParent = -1;
    int freeInodeNum;
    DirEntry de[NUM_OF_DIRENT_PER_BLOCK];

    if(isExistInDir(target, parentInode)==1)
        return -1;

    //travel direct pointers
    for(i=0;i<NUM_OF_DIRECT_BLOCK_PTR;i++){

        bnum = parentInode->dirBlockPtr[i];
        if(bnum==0){//add new direct pointer and new entry
            int newBlockNum = addNewDirEntry(i,parentInode,parentInodeNum);
            parentInode->dirBlockPtr[i] = newBlockNum;
            parentInode->size+=BLOCK_SIZE;
            bnum = parentInode->dirBlockPtr[i];
            PutInode(parentInodeNum,parentInode);
        }
        DevReadBlock(bnum, de);

        //add new dir
        for(j=0;j<NUM_OF_DIRENT_PER_BLOCK;j++){//4
            if(strcmp(de[j].name,"")==0){
                newBlockNum = addNewDirBlock(i,target,parentInode,parentInodeNum,bnum,de);
                return 0;
            }
        }
    }

    //--------indirect------

    //travel indirect block
    int indirectBlockNum = parentInode->indirBlockPtr;
    if(indirectBlockNum==0) {//does't allocate indirect Block yet
        //add new indirect Block
        newBlockNum = GetFreeBlockNum();
        updateFileSysInfo(ALOCATE_BLOCK);
        SetBlockBitmap(newBlockNum);
        indirectBlockNum = newBlockNum;
        bnum = newBlockNum;
        int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
        memset(indirectBlock,0,BLOCK_SIZE);
        parentInode->indirBlockPtr=indirectBlockNum;
        DevWriteBlock(bnum,indirectBlock);
        PutInode(parentInodeNum,parentInode);
    }

    int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
    DevReadBlock(indirectBlockNum,indirectBlock);

    for(i=0;i<NUM_OF_BLOCKS_IN_INDIRECTBLOCK;i++){
        bnum = indirectBlock[i];
        DirEntry de[NUM_OF_DIRENT_PER_BLOCK];

        if(bnum==0){//add new block for indirectBlock
            int newBlockNum = GetFreeBlockNum();
            SetBlockBitmap(newBlockNum);
            updateFileSysInfo(ALOCATE_BLOCK);
            parentInode->size+=BLOCK_SIZE;
            indirectBlock[i]=newBlockNum;
            DirEntry* newDirEntry = (DirEntry *)malloc(BLOCK_SIZE);
            memset(newDirEntry,0,BLOCK_SIZE);
            DevWriteBlock(newBlockNum,newDirEntry);
            DevWriteBlock(indirectBlockNum,indirectBlock);
        }
        bnum = indirectBlock[i];
        DevReadBlock(bnum, de);

        for(j=0;j<NUM_OF_DIRENT_PER_BLOCK;j++){
            if(strcmp(de[j].name,"")==0){//get empty dirent number
                newBlockNum = addNewDirBlock(j,target,parentInode,parentInodeNum,bnum,de);
                DevWriteBlock(indirectBlockNum,indirectBlock);
                return 0;
            }
        }

    }
    return -1;
}

int isExistInDir(char* target, Inode* parentInode){
    int i,j;
    int bnum;
    int count;
    DirEntry de[NUM_OF_DIRENT_PER_BLOCK];

    for(i=0;i<NUM_OF_DIRECT_BLOCK_PTR;i++){
        bnum = parentInode->dirBlockPtr[i];
        if(bnum==0)
            continue;
        DevReadBlock(bnum,de);
        if(isExistInDE(target,de)==1)
            return 1;
    }
    int inbnum = parentInode->indirBlockPtr;
    int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
    DevReadBlock(inbnum,indirectBlock);
    for(i=0;i<NUM_OF_BLOCKS_IN_INDIRECTBLOCK;i++){
        bnum = indirectBlock[i];
        if(bnum==0)
            continue;
        DevReadBlock(bnum,de);
        if(isExistInDE(target,de)==1)
            return 1;
    }
    return 0;
}

int isExistInDE(char* target, DirEntry* de){
    int i;
    for(i=0;i<NUM_OF_DIRENT_PER_BLOCK;i++){
        if(strcmp(de[i].name,target)==0)
            return 1;
    }
    return 0;
}

int addNewDirEntry(int index, Inode* parentInode, int parentInodeNum){

    int newBlockNum = GetFreeBlockNum();
    updateFileSysInfo(ALOCATE_BLOCK);
    SetBlockBitmap(newBlockNum);

    //init new Entry Block
    DirEntry newDirEntry[NUM_OF_DIRENT_PER_BLOCK];
    memset(newDirEntry,0,BLOCK_SIZE);

    //link with parent Block
    parentInode->dirBlockPtr[index] = newBlockNum;//ex) 19

    DevWriteBlock(newBlockNum,newDirEntry);


    return newBlockNum;

}

int addNewDirBlock(int index, char* target, Inode* parentInode, int parentInodeNum, int* parentBlockNum, DirEntry* parentDE){

    int newBlockNum = -1;
    int curBlockNum = parentInode->dirBlockPtr[index];
    int i;
    for(i=0;i<NUM_OF_DIRENT_PER_BLOCK;i++){
        if(strcmp(parentDE[i].name,"")==0){

            int newInodeNum = GetFreeInodeNum();
            updateFileSysInfo(ALOCATE_INODE);
            SetInodeBitmap(newInodeNum);
            Inode* newInode = (Inode*)malloc(BLOCK_SIZE);
            newInode->type=FILE_TYPE_DIR;
            newInode->size=0;
            PutInode(newInodeNum,newInode);

            strcpy(parentDE[i].name,target);
            parentDE[i].inodeNum = newInodeNum;
            DevWriteBlock(parentBlockNum,parentDE);

            newBlockNum = GetFreeBlockNum();
            SetBlockBitmap(newBlockNum);
            updateFileSysInfo(ALOCATE_BLOCK);
            DirEntry* newDE = (DirEntry *)malloc(sizeof(BLOCK_SIZE));
            strcpy(newDE[0].name,".");
            newDE[0].inodeNum=newInodeNum;
            strcpy(newDE[1].name,"..");
            newDE[1].inodeNum=parentInodeNum;
            DevWriteBlock(newBlockNum,newDE);
            DevWriteBlock(parentBlockNum,parentDE);

            newInode->dirBlockPtr[0]=newBlockNum;
            PutInode(newInodeNum,newInode);
            return newBlockNum;
        }
    }
    return -1;

}

int goDownDir(char* target, Inode* curInode, int* curInodeNum, int* bnum){
    int i,j;

    //just check direct block
    for(i=0;i<NUM_OF_DIRECT_BLOCK_PTR;i++){
        *bnum = curInode->dirBlockPtr[i];
        DirEntry de[NUM_OF_DIRENT_PER_BLOCK];
        DevReadBlock(*bnum,de);
        for(j=0;j<NUM_OF_DIRENT_PER_BLOCK;j++){
            if(strcmp(de[j].name,target)==0){
                GetInode(de[j].inodeNum,curInode);
                *bnum = curInode->dirBlockPtr[i];
                *curInodeNum = de[j].inodeNum;
                return *curInodeNum;
            }
        }
    }
    return -1;

}

void updateFileSysInfo(UPDATE_FLAG flag){
    if(pFileSysInfo==NULL){
        loadFileSysInfo();
    }
    switch(flag){
        case ADD_DIR:
            pFileSysInfo->numAllocBlocks++;
            pFileSysInfo->numFreeBlocks--;
            pFileSysInfo->numAllocInodes++;
            break;
        case ALOCATE_BLOCK:
            pFileSysInfo->numAllocBlocks++;
            pFileSysInfo->numFreeBlocks--;
            break;
        case ALOCATE_INODE:
            pFileSysInfo->numAllocInodes++;
            break;
        case FREE_BLOCK:
            pFileSysInfo->numAllocBlocks--;
            pFileSysInfo->numFreeBlocks++;
            break;
        case FREE_INODE:
            pFileSysInfo->numAllocInodes--;
            break;
    }
    DevWriteBlock(FILESYS_INFO_BLOCK,pFileSysInfo);
}
int RemoveDir(const char* szDirName)
{
    Inode* pInode = (Inode*)malloc(sizeof(Inode));

    int* curInodeNum=(int *)malloc(sizeof(int));
    *curInodeNum = 0;
    int* parentBlockNum = (int *)malloc(sizeof(int));
    char target[MAX_NAME_LEN];
    memset(target,0,sizeof(target));

    int parentInodeNum = getParentInode(szDirName,target,pInode,curInodeNum,parentBlockNum);
    if(curInodeNum==-1)//no such directory
        return -1;
    int i;
    int bnum;

    char* pBlock = (char *)malloc(BLOCK_SIZE);
    if(countFile(pInode)>2)//if countFile is bigger than 3, there is any file in directory (include ".", ".."),
        return -1;
    //free inode,

    ResetBlockBitmap(pInode->dirBlockPtr[0]);
    ResetInodeBitmap(*curInodeNum);
    updateFileSysInfo(FREE_INODE);
    updateFileSysInfo(FREE_BLOCK);

    Inode* parentInode =(Inode*)malloc(BLOCK_SIZE);
    GetInode(parentInodeNum,parentInode);
    disLinkWithParent(parentInode,parentInodeNum,*curInodeNum);
    memset(pInode,0,BLOCK_SIZE);
    return 0;
}

int countFile(Inode* pInode){
    int count=0;
    int bnum;
    int i,j;
    DirEntry de[NUM_OF_DIRENT_PER_BLOCK];
    for(i=0;i<NUM_OF_DIRECT_BLOCK_PTR;i++){
        bnum = pInode->dirBlockPtr[i];
        if(bnum==0)
            continue;
        DevReadBlock(bnum,de);
        count+=countFileInBlock(de);
    }
    int inbnum = pInode->indirBlockPtr;
    int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];

    if(inbnum==0)
        return count;

    DevReadBlock(inbnum,indirectBlock);
    for(i=0;i<NUM_OF_BLOCKS_IN_INDIRECTBLOCK;i++){
        bnum = indirectBlock[i];
        DevReadBlock(bnum,de);
        count+=countFileInBlock(de);
    }
    return count;
}

int countFileInBlock(DirEntry de[NUM_OF_DIRENT_PER_BLOCK]){
    int i,count=0;
    for(i=0;i<NUM_OF_DIRENT_PER_BLOCK;i++){
        if(de[i].inodeNum!=0)
            count++;
    }
    return count;
}

int EnumerateDirStatus(const char* szDirName, DirEntryInfo* pDirEntry, int dirEntrys )
{
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    int* curInodeNum=(int *)malloc(sizeof(int));
    *curInodeNum = 0;
    int* parentBlockNum = (int *)malloc(sizeof(int));
    char target[MAX_NAME_LEN];
    memset(target,0,sizeof(target));

    int parentInodeNum = getParentInode(szDirName, target, pInode, curInodeNum, parentBlockNum);

    if(parentInodeNum==-1)
        return -1;

    Inode* parentInode = (Inode*)malloc(sizeof(Inode));
    GetInode(parentInodeNum,parentInode);
    int i,j,count=0;
    int bnum;

    //direct
    DirEntry de[NUM_OF_DIRENT_PER_BLOCK];
    for(i=0;i<NUM_OF_DIRECT_BLOCK_PTR;i++){
        bnum = pInode->dirBlockPtr[i];
        if(bnum==0)
            continue;
        DevReadBlock(bnum,de);
        count = EnumerateBlocks(de,pDirEntry,dirEntrys,count);
        if(count>=dirEntrys)
            return count;
    }

    //indirect
    int inbnum = pInode->indirBlockPtr;
    if(inbnum!=0) {
        int indirectBlock[NUM_OF_BLOCKS_IN_INDIRECTBLOCK];
        DevReadBlock(inbnum, indirectBlock);
        for (i = 0; i < NUM_OF_BLOCKS_IN_INDIRECTBLOCK; i++) {
            bnum = indirectBlock[i];
            if (bnum == 0)
                continue;
            DevReadBlock(bnum, de);
            count = EnumerateBlocks(de, pDirEntry, dirEntrys, count);
            if (count >= dirEntrys)
                return count;
        }
    }

    return count;
}

int EnumerateBlocks(DirEntry de[NUM_OF_DIRENT_PER_BLOCK],DirEntryInfo* pDirEntry, int dirEntrys, int count){
    int i;
    Inode* tempInode =(Inode*)malloc(sizeof(Inode));

    for(i=0;i<NUM_OF_DIRENT_PER_BLOCK;i++){
        if(count>=dirEntrys)
            break;
        if(de->inodeNum!=0) {//copy Directory Information
            if(strcmp(de[i].name,".")==0||strcmp(de[i].name,"..")==0)
                continue;
            if(de[i].inodeNum==0 || strcmp(de[i].name,"")==0)
                continue;
            pDirEntry[count].inodeNum=de[i].inodeNum;
            strcpy(pDirEntry[count].name,de[i].name);
            GetInode(de[i].inodeNum,tempInode);
            pDirEntry[count].type = tempInode->type;
            count++;
        }
    }
    return count;
}

void SetBit(char* InodeBlock, int inodeNum, int BitMap, char bit){
    DevReadBlock(BitMap, InodeBlock);

    int charIndex = inodeNum/BITS_A_BYTE;
    int bitIndex = inodeNum - charIndex * BITS_A_BYTE;

    if(bit==SET) {
        InodeBlock[charIndex] |= (128 >> bitIndex);

    }
    else {
        InodeBlock[charIndex] &= ~(128 >> bitIndex);
    }
    DevWriteBlock(BitMap, InodeBlock);
}



void FileSysInit(void)
{
    DevCreateDisk();
    DevOpenDisk();
    int i;

    //char** blankBuf = (char **)malloc(BLOCK_SIZE * NUM_OF_BLK_PTR);
    char* blankBuf = (char *)malloc(BLOCK_SIZE);;

    for(i=0;i<NUM_OF_BLOCKS;i++){
        memset(blankBuf,0,sizeof(blankBuf));
        DevWriteBlock(i,blankBuf);
    }

    if(pFileDescTable==NULL) {//FDT init
        pFileDescTable = (FileDescTable *) malloc(sizeof(FileDescTable));
        memset(pFileDescTable,0,sizeof(FileDescTable));
    }

}


void loadFileSysInfo(){
    pFileSysInfo = (FileSysInfo * )malloc(sizeof(FileSysInfo));
    DevReadBlock(FILESYS_INFO_BLOCK,pFileSysInfo);
}

void SetInodeBitmap(int inodeno)
{
    char* InodeBlock = (char *)malloc(BLOCK_SIZE);
    SetBit(InodeBlock,inodeno,INODE_BITMAP_BLOCK_NUM,SET);
}


void ResetInodeBitmap(int inodeno) {
    char *InodeBlock = (char *) malloc(BLOCK_SIZE);
    SetBit(InodeBlock,inodeno,INODE_BITMAP_BLOCK_NUM,RESET);
}



void SetBlockBitmap(int blkno)
{
    char* InodeBlock = (char *)malloc(BLOCK_SIZE);
    SetBit(InodeBlock,blkno,BLOCK_BITMAP_BLOCK_NUM,SET);
}


void ResetBlockBitmap(int blkno)
{
    char* InodeBlock = (char *)malloc(BLOCK_SIZE);
    SetBit(InodeBlock,blkno,BLOCK_BITMAP_BLOCK_NUM,RESET);
}

void GetInode(int blkno, Inode* pInode)
{

    void (*fp)() = GetInode;
    copyInode(blkno, pInode, fp);

}

void PutInode(int blkno, Inode* pInode)
{

    void (*fp)() = PutInode;
    copyInode(blkno, pInode, fp);

}

void copyInode(int blkno, Inode* pInode, void (*fp)()) {

    char *InodeBlock = (char *) malloc(BLOCK_SIZE);
    int targetBlockNum = INODELIST_BLOCK_FIRST + blkno / NUMBER_OF_INODE;
    int InodeIndex = blkno % NUMBER_OF_INODE;

    DevReadBlock(targetBlockNum, InodeBlock);

    if (fp == GetInode)
        memcpy(pInode,InodeBlock+sizeof(Inode)*InodeIndex,sizeof(Inode));

    if (fp == PutInode) {
        memcpy(InodeBlock + sizeof(Inode) * InodeIndex, pInode, sizeof(Inode));
        DevWriteBlock(targetBlockNum, InodeBlock);
    }

}

int GetFreeInodeNum(void)
{

    char* pBuf = (char *)malloc(BLOCK_SIZE);
    DevReadBlock(INODE_BITMAP_BLOCK_NUM,pBuf);
    return getEmptyInodeBitIndex(pBuf);
}


int GetFreeBlockNum(void)
{

    char* pBuf = (char *)malloc(BLOCK_SIZE);
    DevReadBlock(BLOCK_BITMAP_BLOCK_NUM,pBuf);
    return getEmptyBlockBitIndex(pBuf);

}

int getEmptyInodeBitIndex(char* pBuf){

    int i=0;
    int j=0;

    for(i=0;i<BLOCK_SIZE;i++){
        for(j=BITS_A_BYTE-1;j>=0;j--){

            if(pBuf[i]>>j & 1)
                continue;
            else
                return i * BITS_A_BYTE + (BITS_A_BYTE-1 -j);
        }
    }

    return -1;
}
int getEmptyBlockBitIndex(char* pBuf){

    int i=0;
    int j=0;
    int index=0;

    for(i=0;i<BLOCK_SIZE;i++){
        for(j=BITS_A_BYTE-1;j>=0;j--){

            if(index<DATA_REGION_START_INDEX){//less than 19
                index++;
                continue;
            }

            if(pBuf[i]>>j & 1)
                continue;
            else
                return i * BITS_A_BYTE + (BITS_A_BYTE-1 -j);
        }
    }

    return -1;
}