del kernel32.bin
del kernel32.o
del fat.o

REM set this to the location of djgpp.env
set DJGPP=C:\gcc\djgpp.env

%1\gcc -fwritable-strings -nostdlib -fno-builtin -ffreestanding -c -o kernel32.o kernel32.c
pause
%1\gcc -w -fwritable-strings -nostdlib -fno-builtin -ffreestanding -c -o scheduler.o process\scheduler.c
%1\gcc -w -fwritable-strings -nostdlib -fno-builtin -ffreestanding -c -o fat.o filesystem\fat12.c
%1\gcc -w -fwritable-strings -nostdlib -fno-builtin -ffreestanding -c -o iso9660.o filesystem\iso9660.c
%1\gcc -w -fwritable-strings -nostdlib -fno-builtin -ffreestanding -c -o devfs.o filesystem\devfs.c
%1\gcc -w -fwritable-strings -nostdlib -fno-builtin -ffreestanding -c -o iomgr.o iomgr\iosched.c
%1\gcc -w -fwritable-strings -nostdlib -fno-builtin -ffreestanding -c -o devmgr_error.o devmgr\devmgr_error.c
%2\nasmw -f coff -o startup.o startup\startup.asm
%2\nasmw -f coff -o asmlib.o startup\asmlib.asm
%2\nasmw -f coff -o irqwrap.o irqwrap.asm
%1\ld -T lscript.txt -Map mapfile.txt
REM -Ttext 0x100000 --oformat binary -o kernel32.bin startup.o irqwrap.o asmlib.o kernel32.o scheduler.o
dir kernel32.bin
