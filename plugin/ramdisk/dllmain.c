/*
  Name: RamDisk Driver for dex-os
  Copyright: 
  Author: Joseph Emmanuel DL Dayo
  Date: 02/04/04 19:54
  Description: This module provides support for a ramdisk device.
  
  RamDisk driver for dex-os
  Copyright (C) 2004  Joseph Emmanuel DL Dayo

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
*/


#define _DEX_DEVICE_

#include "dexsdk.h"
#include "dll.h"
#include <dex\dextypes.h>
#include <dex\dex32_devmgr.h>


#define FAT12 1
#define FAT16 2
#define FAT32 3

#define BLOCK_SIZE 512
#define RAMDISK_MINBLOCKS 50
#define RAMDISK_MAXBLOCKS 1000000

typedef struct __attribute__((packed)) fat_bootsector			/* Warning: this struct must be packed */
{
   BYTE  jump[3];               /* 16-bit JMP to boot code, or 8-bit JMP + NOP */
   BYTE  oem_id[8];             /* e.g. 'MSWIN4.0' */
   WORD  bytes_per_sector;	/* usu. =512 */
   BYTE  sectors_per_cluster;
   WORD  num_boot_sectors;	/* usu. =1 */
   BYTE  num_fats;              /* usu. =2 */
   WORD num_root_dir_ents;
   WORD total_sectors;		/* 16-bit; 0 if num sectors > 65535 */
   BYTE  media_ID_byte;         /* usu. =0F0h */
   WORD sectors_per_fat;
   WORD sectors_per_track;
   WORD heads;
   DWORD hidden_sectors;	/* =LBA partition start */
   DWORD total_sectors_large;	/* 32-bit; 0 if num sectors < 65536 */
   BYTE  boot_code[474];
   BYTE  magic[2];              /* 55h, 0AAh */
} BPB;            

typedef struct __attribute__((packed)) fat_bootsector32			/* Warning: this struct must be packed */
{
    BYTE  jump[3];               /* 16-bit JMP to boot code, or 8-bit JMP + NOP */
    BYTE  oem_id[8];             /* e.g. 'MSWIN4.0' */
  	WORD  bytes_per_sector;	/* usu. =512 */
    BYTE  sectors_per_cluster;
  	WORD  num_boot_sectors;	/* usu. =1 */
    BYTE  num_fats;              /* usu. =2 */
  	WORD num_root_dir_ents;
	WORD total_sectors;		/* 16-bit; 0 if num sectors > 65535 */
    BYTE  media_ID_byte;         /* usu. =0F0h */
	WORD sectors_per_fat;
	WORD sectors_per_track;
	WORD heads;
	DWORD hidden_sectors;	/* =LBA partition start */
	DWORD total_sectors_large;	/* 32-bit; 0 if num sectors < 65536 */
	
	/*********************FAT12/16 only portion******************************/
	DWORD fatsz32;
	WORD  extflags;
	WORD  fsversion;
	DWORD rootcluster;
	WORD  fsinfo;
	WORD  bkbootsec;
	char  reserved[12];
	BYTE  drvnum;
    BYTE  boot_code[445];
    BYTE  magic[2];              /* 55h, 0AAh */
} BPB32;                              /* 512 bytes total */

typedef struct DSKSZTOSECPERCLUS {
	DWORD	DiskSize;
	BYTE	SecPerClusVal;
    } dsksizetosectorsperclus;

dsksizetosectorsperclus DskTableFAT16 [] = {
        {        8400,   0}, /* disks up to  4.1 MB, the 0 value for SecPerClusVal trips an error */
        {      32680,   2},  /* disks up to   16 MB,  1k cluster */
        {    262144,   4},   /* disks up to 128 MB,  2k cluster */
        {   524288,    8},   /* disks up to 256 MB,  4k cluster */
        { 1048576,  16},     /* disks up to 512 MB,  8k cluster */
        /* The entries after this point are not used unless FAT16 is forced */
        { 2097152,  32},     /* disks up to     1 GB, 16k cluster */
        { 4194304,  64},     /* disks up to     2 GB, 32k cluster */
        { 0xFFFFFFFF, 0} /* any disk greater than 2GB, 0 value for SecPerClusVal trips an error */
    };

dsksizetosectorsperclus DskTableFAT32 [] = {
        {       66600,   0},  /* disks up to 32.5 MB, the 0 value for SecPerClusVal trips an error */
        {     532480,   1},   /* disks up to 260 MB,  .5k cluster */
        { 16777216,   8},     /* disks up to     8 GB,    4k cluster */
        { 33554432, 16},      /* disks up to   16 GB,    8k cluster */
        { 67108864, 32},      /* disks up to   32 GB,  16k cluster */
        { 0xFFFFFFFF, 64}/* disks greater than 32GB, 32k cluster */
    };

devmgr_stdlib stdlib;
devmgr_block_desc myinterface;

typedef struct _block {
char data[BLOCK_SIZE];
} block;

int ramdisk_handle;
DWORD ramdisk_size = 4000;
block *ramdiskmem;


/*Tools used by device drivers, this pointers to functions will
      be filled in by the dex device driver stub*/
      
void (*kfree)(void *ptr) = 0;
void *(*kmalloc)(size_t size) = 0;
int  (*kprintf)(const char *fmt, ...) = 0;
void *(*krealloc)(void *ptr, size_t size) = 0;

int  (*devmgr_register)(devmgr_generic *) = 0;
char *(*devmgr_identify)(int type,char *buf) = 0;
int  (*devmgr_finddevice)(const char *name) = 0;
int  (*devmgr_copyinterface)(const char *name,devmgr_generic *interface) = 0;
devmgr_generic (*devmgr_getdevice)(int deviceid) = 0;
void (*devmgr_disableints)() = 0;
void (*devmgr_enableints)() = 0;
int  (*extension_override)(devmgr_generic *ext, devmgr_generic **prev) = 0;



int read_block(int block,char *blockbuff,DWORD numblocks);
int write_block(int block,char *blockbuff,DWORD numblocks);

int fat_write_cluster(int cluster,int value,void *fat,int fat_type)
{
   WORD *temp;
   BYTE  *fat12;
   WORD  *fat16;
   DWORD *fat32;
   int x,index;

   if (fat == 0 || fat_type == FAT32)
   {
      DWORD start_of_fat = 32;   
      DWORD fat_sector = (cluster >> 7) + start_of_fat;
      DWORD handle;
      fat32 = (DWORD*)kmalloc(512);
      
      read_block(fat_sector,(char*) fat32,1);                
      
      fat32[cluster & 0x7f] = value & 0x0FFFFFFF;   

      write_block(fat_sector,(char*) fat32,1);
      
      kfree(fat32);
      return cluster ;
   };
   
   //determine the correct type of FAT
   if (fat_type == FAT12) fat12 =(BYTE*)fat;
       else
   if (fat_type == FAT16) fat16 =(WORD*)fat;
       else
   if (fat_type == FAT32) fat32 =(DWORD*)fat;
   
   if (fat_type == FAT12)
   {
      
      x=(cluster*3)/2; //index=cluster*3/2
      temp=(WORD*)(fat12+x);
      index=*temp;

      if (cluster & 1) //cluster is odd
      {
         value = value << 4;
         *temp= (*temp&0x000F) | value;
      }
        else
     {
      value =value & 0x0FFF;
      *temp=(*temp&0xF000) | value;
     };
   }
      else
   if (fat_type == FAT16)
        fat16[cluster] = value;
        
   return 0;
;};

int find_fat_type(DWORD num_sectors,DWORD *sect_per_clust)
{
        int i = 0;
        /*Find for the valid sector per cluster number and the
          correct FAT type*/
        kprintf("Identifying FAT type..\n");

        //FAT16
        while (DskTableFAT16[i].DiskSize!=0xFFFFFFFF)
        {
            if (num_sectors < DskTableFAT16[i].DiskSize)
               {
                   if ( DskTableFAT16[i].SecPerClusVal == 0 ) 
                           {
                           *sect_per_clust = 1;
                            return FAT12;
                           }
                   else
                           {
                           *sect_per_clust = DskTableFAT16[i].SecPerClusVal;
                           return FAT16;
                           };
               };
               i++;
        }; 
        
        i = 0;
        //FAT32
        while (DskTableFAT32[i].DiskSize!=0xFFFFFFFF)
        {
            if (num_sectors < DskTableFAT32[i].DiskSize)
               {
                   if ( DskTableFAT16[i].SecPerClusVal == 0 ) 
                           {
                           *sect_per_clust = 1;
                            return FAT16;
                           }
                   else
                           {
                           *sect_per_clust = DskTableFAT32[i].SecPerClusVal;
                           return FAT32;
                           };
               };
               i++;
        }; 
}; 

/*format ramdisk to conform to the FAT12/16/32 specification*/
int init_device()
{
    BPB diskBPB;
    BPB32 diskBPB32;
    DWORD sect_per_clust;
    int i,i2;
    char *FAT;
    char temp[255];
    for (i=0;i<ramdisk_size;i++)     
    memset(ramdiskmem[i].data,0,BLOCK_SIZE);
    

    if (find_fat_type(ramdisk_size,&sect_per_clust) == FAT12 )
    {
           /* For FAT12 formats*/
            memset(&diskBPB,0,sizeof(diskBPB));
            diskBPB.jump[0] = 0xEB; diskBPB.jump[1] = 0x00;diskBPB.jump[2] = 0x90 ;
            memcpy(diskBPB.oem_id,"MSWIN4.1",8);
            kprintf("Using FAT12..\n");
            diskBPB.bytes_per_sector = 512;
            diskBPB.sectors_per_cluster = 1;
            diskBPB.num_boot_sectors=1;
            diskBPB.num_fats=2;
            diskBPB.num_root_dir_ents=224;
            diskBPB.total_sectors = ramdisk_size;
            diskBPB.media_ID_byte=0xF0;
            diskBPB.sectors_per_fat   = 0x9;
            diskBPB.sectors_per_track = 0x12;
            diskBPB.heads= 2;
            diskBPB.magic[0]=0x55;
            diskBPB.magic[1]=0xaa;
            write_block(0,(char*)&diskBPB,1);
            
            kprintf("allocating FAT..\n");
            FAT = (char*)kmalloc(512*(diskBPB.sectors_per_fat+1));
            memset(FAT,0,512*diskBPB.sectors_per_fat);
            kprintf("writing to FAT..");
            for (i=0;i<13;i++)
             {
                  fat_write_cluster(i,i+1,FAT,FAT12);
             };
            fat_write_cluster(13,0x0FF8,FAT,FAT12);
            kprintf("formatting..\n");
            //write FAT to disk
            write_block(diskBPB.num_boot_sectors,(void*)FAT,diskBPB.sectors_per_fat);
            write_block(diskBPB.num_boot_sectors+diskBPB.sectors_per_fat,
                                                 (void*)FAT,diskBPB.sectors_per_fat);
            kprintf("done.\n");
            kfree(FAT);
    }
        else
    if (find_fat_type(ramdisk_size,&sect_per_clust) == FAT16 )
    {
            /* For FAT16 formats*/
            memset(&diskBPB,0,sizeof(diskBPB));
            diskBPB.jump[0] = 0xEB; diskBPB.jump[1] = 0x00;diskBPB.jump[2] = 0x90 ;
            memcpy(diskBPB.oem_id,"MSWIN4.1",8);
            kprintf("Using FAT16..\n");
            diskBPB.bytes_per_sector = 512;
            diskBPB.sectors_per_cluster = sect_per_clust;
            diskBPB.num_boot_sectors=1;
            diskBPB.num_fats=2;
            diskBPB.num_root_dir_ents=512;
            diskBPB.total_sectors = ramdisk_size;
            diskBPB.media_ID_byte=0xF0;
            diskBPB.sectors_per_track = 0x12;
            diskBPB.heads= 2;
            diskBPB.magic[0]=0x55;
            diskBPB.magic[1]=0xaa;
            
            /*Compute for the FAT Size*/
            int RootDirSectors = ((diskBPB.num_root_dir_ents * 32) + 
                                  (diskBPB.bytes_per_sector - 1)) / diskBPB.bytes_per_sector;
                                  
            int TmpVal1 = ramdisk_size - (diskBPB.num_boot_sectors + RootDirSectors);
            int TmpVal2 = (256 * diskBPB.sectors_per_cluster) + diskBPB.num_fats;
            int FATSz;
            
            FATSz = (TmpVal1 + (TmpVal2 - 1)) / TmpVal2;
            diskBPB.sectors_per_fat = FATSz;
            write_block(0,(char*)&diskBPB,1);
            
            kprintf("allocating FAT..\n");
            FAT = (char*)kmalloc(512*(diskBPB.sectors_per_fat+1));
            memset(FAT,0,512*diskBPB.sectors_per_fat);
            kprintf("writing to FAT..");
            for (i=0;i<13;i++)
             {
                  fat_write_cluster(i, i+1, FAT, FAT16);
             };
            fat_write_cluster(13, 0xFFF8, FAT, FAT16);
            kprintf("formatting..\n");
            //write FAT to disk
            write_block(diskBPB.num_boot_sectors,(void*)FAT,diskBPB.sectors_per_fat);
            write_block(diskBPB.num_boot_sectors+diskBPB.sectors_per_fat,
                                                 (void*)FAT,diskBPB.sectors_per_fat);
            kprintf("done.\n");
            kfree(FAT);
    }
        else
    if (find_fat_type(ramdisk_size,&sect_per_clust) == FAT32)
    {
            memset(&diskBPB32,0,sizeof(diskBPB32));
            diskBPB32.jump[0] = 0xEB; 
            diskBPB32.jump[1] = 0x00;
            diskBPB32.jump[2] = 0x90;
            memcpy(diskBPB32.oem_id,"MSWIN4.1",8);
            kprintf("Using FAT32..\n");
            diskBPB32.bytes_per_sector = 512;
            diskBPB32.sectors_per_cluster = sect_per_clust;
            diskBPB32.num_boot_sectors=32;
            diskBPB32.num_fats=2;
            diskBPB32.num_root_dir_ents = 0;
            diskBPB32.total_sectors = 0;
            diskBPB32.total_sectors_large = ramdisk_size;
            diskBPB32.media_ID_byte = 0xF0;
            diskBPB32.sectors_per_fat   = 0x9;
            diskBPB32.sectors_per_track = 0x12;
            diskBPB32.heads= 2;
            diskBPB32.magic[0]=0x55;
            diskBPB32.magic[1]=0xaa;
            diskBPB32.rootcluster = 2;
            
            /*Compute for the FAT Size*/
            int RootDirSectors = ((diskBPB32.num_root_dir_ents * 32) + 
                                  (diskBPB32.bytes_per_sector - 1)) / diskBPB32.bytes_per_sector;
                                  
            int TmpVal1 = ramdisk_size - (diskBPB32.num_boot_sectors + RootDirSectors);
            int TmpVal2 = (256 * diskBPB32.sectors_per_cluster) + diskBPB32.num_fats;
            int FATSz;
            
            TmpVal2 = TmpVal2 / 2;
            FATSz = (TmpVal1 + (TmpVal2 - 1)) / TmpVal2;
            diskBPB32.sectors_per_fat = 0;
            diskBPB32.fatsz32 = FATSz;
            write_block(0,(char*)&diskBPB32,1);
           
    }; 
      
    
    
    
    

    
    return 1;
};

int getcache(char *buf,DWORD block,DWORD numblocks)
{
    read_block(block,buf,numblocks);
    return 1;
};

int putcache(char *buf,DWORD block,DWORD numblocks)
{
    write_block(block,buf,numblocks);
    return 1;
};

int read_block(int block,char *blockbuff,DWORD numblocks)
{
int i,ofs=0,res;

        
for (i=0;i<numblocks;i++)
{
    if (block<ramdisk_size)
        memcpy(blockbuff + ofs,&ramdiskmem[ block + i ].data , BLOCK_SIZE);
    else
    return -1;
    
    ofs+=BLOCK_SIZE;
};    
return 1;
};

int write_block(int block,char *blockbuff,DWORD numblocks)
{
int i,res;
DWORD ofs;
ofs = 0;
for (i=0;i<numblocks;i++)
{
    res = 0;
    if (block + i <ramdisk_size)
       {
         memcpy(&ramdiskmem[ block + i ].data, blockbuff + ofs, BLOCK_SIZE);
       } 
      else
    return -1;
    
    ofs+=BLOCK_SIZE;
};
return 1;
};

int total_blocks()
{
    return ramdisk_size;
};

int get_block_size()
{
    return BLOCK_SIZE;
};


/*Inititalizes the ramdisk and interprets the parameters given to it*/
int device_init( devmgr_interface *devmgr,int c,char *args[])
{
    char temp[256];
    char name[256];
    char *p;
    int i;
    
    strcpy(name,"ramdisk");
    
    for (i=2; i < c ;i++)
    {
        if (strcmp(args[i],"-name")==0)
          {
                if (i + 1 < c)    
                {
                strcpy(name,args[i + 1]);
                i++;
                };
          }
        else  
        if (strcmp(args[i],"-blocks")==0)
          {
              if (i + 1 < c)    
               {
                 ramdisk_size = atoi( args[i+1] );
                 if (ramdisk_size < RAMDISK_MINBLOCKS || ramdisk_size > RAMDISK_MAXBLOCKS)
                 {
                     kprintf("RamDisk: (error) Invalid number of blocks, range should be (%d-%d).\n",
                        RAMDISK_MINBLOCKS, RAMDISK_MAXBLOCKS);
                     return -1;
                 };
                 i++;
               };
          }
        else
        {
                kprintf("RamDisk: (error) unidentified parameter \"%s\".\n",args[i]);
                return -1;
        };
    };
    
    
    
   
    kprintf("DEX Ramdisk Driver 1.03\n");
    
    if (devmgr->devmgr_finddevice(name)!=-1)
    { 
        kprintf("RamDisk was unable to register itself to the\n");
        kprintf("device manager. A device with the same name (%s)\n",name);
        kprintf("might be present, use the -name parameter to\n");
        kprintf("assign your own name.\n");
        return -1;
    };
    
    ramdiskmem = kmalloc(ramdisk_size*512);
    
    if (ramdiskmem == 0)
    {
        kprintf("RamDisk: (error) Error allocating memory or out of memory.\n");
        return -1;
    };
    
    kprintf("%d blocks alloated \n", ramdisk_size);
    
    
    //allocate 2MB of data for the ramdisk
    init_device();
    
    memset(&myinterface,0,sizeof(devmgr_block_desc));
    myinterface.hdr.size=sizeof(devmgr_block_desc);
    kprintf("Installing Ramdisk as %s\n",name);
    strcpy(myinterface.hdr.name,name);
    strcpy(myinterface.hdr.description,"Virtual Block Device");
    myinterface.hdr.type = DEVMGR_BLOCK;
    myinterface.read_block = read_block;
    myinterface.write_block =write_block;
    myinterface.total_blocks= total_blocks;
    myinterface.get_block_size= get_block_size;
    myinterface.getcache = getcache;
    myinterface.putcache = putcache;
    ramdisk_handle = devmgr_register((devmgr_generic*)&myinterface);
    
    if (ramdisk_handle != -1)
    {
        kprintf("Initialization successful!\n");
        kprintf("Driver was assigned handle %d\n",ramdisk_handle);
        
    }
        else
    {
        kprintf("RamDisk: There was an error registering.\n");
        kfree(ramdiskmem);
        return -1;
    };
    return 1;
};

DLLIMPORT int dex32_libmain(devmgr_interface *devmgr,char *parameter)
{
   char *s;
   int c=0;
   char *p[100];
   char params[500];
   
   devmgr_stdlib stdlib;
   devmgr_block_desc myinterface;
   
   strcpy(params,parameter);
   s=strtok(params," ");
   do {
    p[c]=s;
    c++;
    s=strtok(0," ");
   } while (s!=0);
   
   
   stdlib.hdr.size=sizeof(devmgr_stdlib);
   devmgr->devmgr_copyinterface("stdlib",(devmgr_generic*) &stdlib);
   
   kfree = stdlib.free;
   kmalloc = stdlib.malloc;
   kprintf = stdlib.printf;
   krealloc = stdlib.realloc;
   
   devmgr_register = devmgr->devmgr_register;
   devmgr_copyinterface = devmgr->devmgr_copyinterface;
   
   return device_init(devmgr,c,p);
};



int main2(int argc, char *argv[])
{
    printf("Ramdisk v 1.03 - use loadmod to install\n");
};
