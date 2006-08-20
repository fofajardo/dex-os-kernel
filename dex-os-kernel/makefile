CC=gcc
CFLAGS=-w -nostdlib -fno-builtin -ffreestanding -c
NASMC=nasmw
NASMFLAGS=-f coff

dex-os-kernel: kernel32.o scheduler.o fat.o iso9660.o devfs.o iosched.o devmgr_error.o \
			   startup.o asmlib.o irqwrap.o
			   ld -T lscript.txt -Map mapfile.txt
			   gzip kernel32.bin

kernel32.o: kernel32.c
		  $(CC) $(CFLAGS) -o kernel32.o kernel32.c
               
scheduler.o: process/scheduler.c
			 $(CC) $(CFLAGS) -o scheduler.o process/scheduler.c

fat.o: filesystem/fat12.c
	   $(CC) $(CFLAGS) -o fat.o filesystem/fat12.c
       
iso9660.o: filesystem/iso9660.c
		   $(CC) $(CFLAGS) -o iso9660.o filesystem/iso9660.c
         
devfs.o: filesystem/devfs.c
		 $(CC) $(CFLAGS) -o devfs.o filesystem/devfs.c
       
iosched.o: iomgr/iosched.c
		   $(CC) $(CFLAGS) -o iomgr.o iomgr/iosched.c

devmgr_error.o: devmgr/devmgr_error.c
				$(CC) $(CFLAGS) -o devmgr_error.o devmgr/devmgr_error.c
         
startup.o: startup/startup.asm
		   $(NASMC)  $(NASMFLAGS) -o startup.o startup/startup.asm

asmlib.o: startup/asmlib.asm
		  $(NASMC) $(NASMFLAGS) -f coff -o asmlib.o startup/asmlib.asm

irqwrap.o: irqwrap.asm      
		   $(NASMC) $(NASMFLAGS) -f coff -o irqwrap.o irqwrap.asm

clean :
	  rm kernel32.bin.gz kernel32.o scheduler.o fat.o iso9660.o devfs.o \
	  iomgr.o devmgr_error.o startup.o asmlib.o irqwrap.o
