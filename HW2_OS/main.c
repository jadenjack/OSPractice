#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "fs.h"
#include "disk.h"

int main() {

    char* a =(char *)malloc(BLOCK_SIZE);
    free(a);
    Mount(MT_TYPE_FORMAT);

    printf("%d\n",MakeDir("/tmp1"));
    printf("%d\n",MakeDir("/tmp1/abs"));
    printf("%d\n",RemoveDir("/tmp1/abs"));
    printf("%d\n",RemoveDir("/tmp1"));
    printf("%d\n",MakeDir("/tmp1"));
    printf("%d\n",MakeDir("/tmp1/ab1"));
    printf("%d\n",MakeDir("/tmp1/ab2"));
    printf("%d\n",MakeDir("/tmp1/ab3"));
    printf("%d\n",MakeDir("/tmp1/ab4"));
    printf("%d\n",MakeDir("/tmp1/ab5"));
    printf("%d\n",MakeDir("/tmp1/ab6"));
    printf("%d\n",MakeDir("/tmp1/ab7"));
    printf("%d\n",MakeDir("/tmp1/ab8"));
    printf("%d\n",MakeDir("/tmp1/ab9"));
    printf("%d\n",MakeDir("/tmp1/ab10"));
    printf("%d\n",MakeDir("/tmp1/ab1"));
    printf("%d\n",MakeDir("/tmp1/ab10"));


    DirEntryInfo* pDirEntryInfo = (DirEntryInfo*)malloc(BLOCK_SIZE * 10);
    EnumerateDirStatus("/tmp1",pDirEntryInfo, 10);
    int i;
    for (i = 0;i < 10;i++)
        printf("directory entry:%s, type:%d, inode number:%d\n", pDirEntryInfo[i].name, pDirEntryInfo[i].type, pDirEntryInfo[i].inodeNum);

    int myFD = OpenFile("/tmp1/ab1/asd",OPEN_FLAG_CREATE);
    printf("Open File : %d\n",myFD);
    myFD = OpenFile("/tmp1/ab1/asd",OPEN_FLAG_CREATE);
    printf("Open File : %d\n",myFD);
    myFD = OpenFile("/tmp1/ab1/asd1",OPEN_FLAG_CREATE);
    printf("Open File : %d\n",myFD);
    myFD = OpenFile("/tmp1/ab1/asd2",OPEN_FLAG_CREATE);
    printf("Open File : %d\n",myFD);
    myFD = OpenFile("/tmp1/ab1/asd3",OPEN_FLAG_CREATE);
    printf("Open File : %d\n",myFD);
    myFD = OpenFile("/tmp1/ab1/asd4",OPEN_FLAG_CREATE);
    printf("Open File : %d\n",myFD);
    myFD = OpenFile("/tmp1/ab1/asd4",OPEN_FLAG_CREATE);
    printf("Open File : %d\n",myFD);
    myFD = OpenFile("/tmp1/ab1/asd4",OPEN_FLAG_CREATE);
    printf("Open File : %d\n",myFD);

    char* pBuf1 = (char *)malloc(BLOCK_SIZE);
    strcpy(pBuf1,"helloOS");
    char* pBuf2 = (char *)malloc(BLOCK_SIZE);
    strcpy(pBuf2,"01234567890123456789012345678901234567890123456789012345678901235");
    WriteFile(myFD,pBuf1,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf1,BLOCK_SIZE);
    RemoveFile("/tmp1/asda");
    CloseFile(myFD);
    RemoveFile("/tmp1/asd");
    myFD = OpenFile("/tmp1/asd",OPEN_FLAG_CREATE);

    WriteFile(myFD,pBuf1,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf2,BLOCK_SIZE);
    WriteFile(myFD,pBuf1,BLOCK_SIZE);


    CloseFile(myFD);
    RemoveFile("/tmp1/asd");

    printf("1: %d\n",pFileSysInfo->numAllocBlocks);
    Unmount();
    Mount(MT_TYPE_READWRITE);
    printf("2 : %d\n",pFileSysInfo->numAllocBlocks);
    return 0;
}