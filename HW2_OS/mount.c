#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "fs.h"
#include <unistd.h>


#define DATA_REGION_START_INDEX 19;

FileSysInfo* pFileSysInfo = NULL;
const char ROOT_DIR_NAME[] = "/";
extern int fd;

void Mount(MountType type)
{
    if(type == MT_TYPE_FORMAT) {
        FileSysInit();
        //ppt 10p
        int freeBlockNum = GetFreeBlockNum();//Data region start at block num 19, 0~18 blocks are system info
        int freeInodeNum = GetFreeInodeNum();//it must be 0
        DirEntry de[NUM_OF_DIRENT_PER_BLOCK];
        de[0].inodeNum = freeInodeNum;//first inodeNum of root dir must be 0
        strcpy(de[0].name, ".");//there is only "." file

        DevWriteBlock(freeBlockNum, de);

        pFileSysInfo = (FileSysInfo * )malloc(BLOCK_SIZE);

        //start init pFileSysInfo
        pFileSysInfo->blocks = 64 * 8;//num of block bit map * bits a byte
        pFileSysInfo->rootInodeNum = freeInodeNum;
        pFileSysInfo->diskCapacity = FS_DISK_CAPACITY; // 32KB
        pFileSysInfo->numAllocBlocks = 0;
        pFileSysInfo->numFreeBlocks = pFileSysInfo->blocks;
        pFileSysInfo->numAllocInodes = 0;
        pFileSysInfo->blockBitmapBlock = BLOCK_BITMAP_BLOCK_NUM;     // block bitmap의 시작 블록 번호
        pFileSysInfo->inodeBitmapBlock = INODE_BITMAP_BLOCK_NUM;     // inode bitmap의 시작 블록 번호
        pFileSysInfo->inodeListBlock = INODELIST_BLOCK_FIRST;	// inode list의 시작 블록 주소
        pFileSysInfo->dataRegionBlock = DATA_REGION_START_INDEX; // 0~18 is already used for metadata
        //end init pFileSysinfo

        pFileSysInfo->numFreeBlocks--;//used for root
        pFileSysInfo->numAllocBlocks++;//used for root
        pFileSysInfo->numAllocInodes++;//used for root

        DevWriteBlock(FILESYS_INFO_BLOCK,pFileSysInfo);
        SetBlockBitmap(freeBlockNum);
        SetInodeBitmap(freeInodeNum);

        //ppt 12p
        Inode* pInode = (Inode*)malloc(sizeof(Inode));
        GetInode(0, pInode);
        pInode->type=1;
        pInode->dirBlockPtr[0] = DATA_REGION_START_INDEX;
        PutInode(0, pInode);

        free(pInode);

    }else if(type == MT_TYPE_READWRITE){
        DevOpenDisk();
        if(pFileDescTable==NULL) {//FDT init
            pFileDescTable = (FileDescTable *) malloc(sizeof(FileDescTable));
            memset(pFileDescTable,0,sizeof(FileDescTable));
        }
        if(pFileSysInfo==NULL){
            char* infoBlock = (char*)malloc(BLOCK_SIZE);
            DevReadBlock(FILESYS_INFO_BLOCK,infoBlock);
            pFileSysInfo = (FileSysInfo * )malloc(BLOCK_SIZE);
            memcpy(pFileSysInfo,infoBlock,BLOCK_SIZE);
            free(infoBlock);
        }
    }
}

void Unmount(void)
{
    DevWriteBlock(0,pFileSysInfo);
    free(pFileSysInfo);
    pFileSysInfo = NULL;
    free(pFileDescTable);
    pFileDescTable = NULL;
    close(fd);
}

