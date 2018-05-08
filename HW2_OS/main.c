#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "fs.h"
#include "disk.h"

//TODO : extern fd, mount reload
int main() {

    Mount(MT_TYPE_FORMAT);

    int i;garamTest();

    return 0;
}

void mkandremove(){
    int i;
    DirEntryInfo* pDirEntryInfo = (DirEntryInfo*)malloc(BLOCK_SIZE * 10);
    EnumerateDirStatus("/tmp1",pDirEntryInfo, 10);
    for(i=0;i<10000;i++) {
//        printf("%d : %d\n",i, MakeDir("/tmp1"));
//        printf("%d\n", RemoveDir("/tmp1"));
        MakeDir("/tmp1");
        RemoveDir("/tmp1");
    }
}

void writeTest(){
    //100 bytes string
    char* str = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    MakeDir("/tmp");
    MakeDir("/tmp/writeTest");
    int myFd = OpenFile("/tmp/wrtieTest/file1",OPEN_FLAG_CREATE);

    WriteFile(myFd,str,sizeof(str));
}

void garamTest(){
    Mount(MT_TYPE_FORMAT);
    MakeDir("/tmp");
    MakeDir("/tmp/asd/");
    MakeDir("/test");
    MakeDir("/tmp/asd/cde");
    MakeDir("/tmp/bcd/");
    printInodeState();
    printBlockState();
    MakeDir("/tmp/fff/");
    MakeDir("/tmp/mon/");
    MakeDir("/tmp/tue/");
    MakeDir("/tmp/wed/");
    printInodeState();
    printBlockState();
    MakeDir("/tmp/thur/");
    MakeDir("/tmp/fri/");
    MakeDir("/tmp/sat/");
    MakeDir("/tmp/sun/");
    MakeDir("/tmp/jan/");
    MakeDir("/tmp/feb/");
    MakeDir("/tmp/mar/");
    MakeDir("/tmp/apr/");
    MakeDir("/tmp/may/");
    MakeDir("/tmp/jun/");
    MakeDir("/tmp/jul/");
    MakeDir("/tmp/aug/");
    MakeDir("/tmp/sep/");
    MakeDir("/tmp/oct/");
    MakeDir("/tmp/nov/");
    MakeDir("/tmp/dec/");
    MakeDir("/tmp/ddddd/");
}

void printFileSysInfo(){
    printf("\n↓↓↓↓↓↓↓↓↓FileSysInfo↓↓↓↓↓↓↓↓↓↓↓\n");
    printf("blocks : %d\n", pFileSysInfo->blocks);
    printf("rootInodeNum : %d\n", pFileSysInfo->rootInodeNum);
    printf("diskCapacity : %d\n", pFileSysInfo->diskCapacity);
    printf("numAllocBlocks : %d\n", pFileSysInfo->numAllocBlocks);
    printf("numFreeBlocks : %d\n", pFileSysInfo->numFreeBlocks);
    printf("numAllocInodes : %d\n", pFileSysInfo->numAllocInodes);
    printf("blockBitmapBlock : %d\n", pFileSysInfo->blockBitmapBlock);
    printf("inodeBitmapBlock : %d\n", pFileSysInfo->inodeBitmapBlock);
    printf("inodeListBlock : %d\n", pFileSysInfo->inodeListBlock);
    printf("dataRegionBlock : %d\n", pFileSysInfo->dataRegionBlock);
    printf("↑↑↑↑↑↑↑↑↑FileSysInfo↑↑↑↑↑↑↑↑↑↑↑\n");

}

void printInodeState(){
    int i,j;
    int inodeNum;
    Inode* inode = (Inode*)malloc(sizeof(Inode));
    //for(i=INODELIST_BLOCK_FIRST;i<INODELIST_BLOCK_FIRST+INODELIST_BLOCKS;i++){
    for(i=INODELIST_BLOCK_FIRST;i<INODELIST_BLOCK_FIRST+5;i++){
        for(j=0;j<4;j++){
            inodeNum = (i-INODELIST_BLOCK_FIRST)*4+j;
            GetInode(inodeNum,inode);
            printInode(inode,inodeNum);
        }
    }
}

void printInode(Inode* inode,int inodeNum){
    printf("\n === Inode %d ===\n",inodeNum);
    printf("size   : %d\n",inode->size);
    printf("type   : %d\n",inode->type);
    printf("dir[0] : %d\n",inode->dirBlockPtr[0]);
    printf("dir[1] : %d\n",inode->dirBlockPtr[1]);
    printf("indirct: %d\n",inode->indirBlockPtr);
    printf("===================\n");
}

void printBlockState(){
    int i,j;
    int inodeNum;
    Inode* inode = (Inode*)malloc(sizeof(Inode));
    for(i=19;i<24;i++){
        printBlock(i);
    }
}

void printBlock(int bnum){
    DirEntry de[4];
    int i;
    DevReadBlock(bnum, de);
    printf("\n === Block %d ===\n",bnum);
    for(i=0;i<4;i++){
        printf("%12s | %d\n",de[i].name,de[i].inodeNum);
    }
    printf("===================\n");
}