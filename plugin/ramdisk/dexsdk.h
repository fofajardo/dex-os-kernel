/*
  Name: DEX Application Development Kit Library version 1.0
  Copyright: 
  Author: Joseph Emmanuel DL Dayo
  Date: 28/02/04 07:12
  Description: This module provides POSIX functons that you can use
  when building an application for DEX. This compiles under DEV-C++/MINGW
  and Borland C++. There is a possibility for it to be compiled under other
  compilers as well.
  
  Replaces: stdio.h stdlib.h time.h
*/

#ifndef _DEXSDK_H
#define _DEXSDK_H

#define _DEXSDK_VERSION_ 1
#include <stdarg.h>


extern int syscall(int function_num,int p1,int p2,int p3,int p4,int p5);
extern int main2(int argc, char *argv[]);

/*=============defines SYSTEM CALL constants in DEX==================*/
#define FXN_DEQUEUECHAR 1
#define FXN_FREAD 0x39
#define FXN_FSEEK 0x41
#define FXN_SBRK 0x9
#define FXN_DPUTC 0x6
#define FXN_EXIT 3
#define FXN_SLEEP 0x54
#define FXN_GETPARAMS 0x50
#define FXN_OPENFILE 0x4
#define FXN_TIME 0x55
#define FXN_FSTAT 0x58
#define FXN_CLOSEFILE 0x5
#define FXN_DELFILE 0x59
#define FXN_WRITE 0x45
#define FXN_GETPID 2
#define FXN_GETPPID 27
#define FXN_FINDPROC 28
#define FXN_LOADLIB 29
#define FXN_LIBGETFUNC 30
#define FXN_SETERROR 31
#define FXN_GETERROR 32
#define FXN_SEARCHNAME 33
#define FXN_STAT 36


/*=============defines color constants, as used by stdio.h===========*/
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGRAY 7
#define DARKGRAY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15
#define BLINK 128


#define	SPC	01
#define	STP	02

/***************** tables (used by stdlib*************************/
static char _sctab[256] = {
	0,0,0,0,0,0,0,0,
	0,SPC,SPC,SPC,SPC,SPC,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	SPC,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};

static long double powten[] =
{
  1e1L, 1e2L, 1e4L, 1e8L, 1e16L, 1e32L, 1e64L, 1e128L, 1e256L,
  1e512L, 1e1024L, 1e2048L, 1e4096L
};

typedef struct {
  int quot;
  int rem;
} div_t;

typedef struct {
  long quot;
  long rem;
} ldiv_t;

/*===========constants defined in errno.h==============*/
extern int errno;

#define EDOM		1
#define ERANGE		2



#define E2BIG		3
#define EACCES		4
#define EAGAIN		5
#define EBADF		6
#define EBUSY		7
#define ECHILD		8
#define EDEADLK		9
#define EEXIST		10
#define EFAULT		11
#define EFBIG		12
#define EINTR		13
#define EINVAL		14
#define EIO		15
#define EISDIR		16
#define EMFILE		17
#define EMLINK		18
#define ENAMETOOLONG	19
#define ENFILE		20
#define ENODEV		21
#define ENOENT		22
#define ENOEXEC		23
#define ENOLCK		24
#define ENOMEM		25
#define ENOSPC		26
#define ENOSYS		27
#define ENOTDIR		28
#define ENOTEMPTY	29
#define ENOTTY		30
#define ENXIO		31
#define EPERM		32
#define EPIPE		33
#define EROFS		34
#define ESPIPE		35
#define ESRCH		36
#define EXDEV		37


/*============files.h constants=========*/

/*ftell, fseek constants*/
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define FILE unsigned int
#define EOF (-1)                /* End of file indicator */

extern FILE *stdout, *stdin, *stderr;

/*============DEX constants for files===========*/
#define FILE_READ 0
#define FILE_WRITE 1
#define FILE_READWRITE 2
#define FILE_APPEND 3


#define unconst(__v, __t) __extension__ ({union { const __t __cp; __t __p; } __q; __q.__cp = __v; __q.__p;})
/*============constants defined in limits.h============*/
#define CHAR_BIT 8
#define CHAR_MAX 127
#define CHAR_MIN (-128)
#define INT_MAX 2147483647
#define INT_MIN (-2147483647-1)
#define LONG_MAX 2147483647L
#define LONG_MIN (-2147483647L-1L)
#define MB_LEN_MAX 5
#define SCHAR_MAX 127
#define SCHAR_MIN (-128)
#define SHRT_MAX 32767
#define SHRT_MIN (-32768)
#define UCHAR_MAX 255
#define UINT_MAX 4294967295U
#define ULONG_MAX 4294967295UL
#define USHRT_MAX 65535
#define WCHAR_MIN 0
#define WCHAR_MAX 127
#define WINT_MIN 0
#define WINT_MAX 32767

/*============other constants================*/
#define NULL 0

/*POSIX typedefs*/
typedef unsigned int mode_t,dev_t,gid_t,ino_t,nlink_t,off_t,uid_t,clock_t,size_t;
typedef long int time_t;
typedef int (*fnptr_t)(unsigned c, void **helper,FILE *f);
typedef void (*sighandler_t)(int signum);

/*CRT structures*/
struct text_info {
   unsigned char winleft;        /* left window coordinate */
   unsigned char wintop;         /* top window coordinate */
   unsigned char winright;       /* right window coordinate */
   unsigned char winbottom;      /* bottom window coordinate */
   unsigned char attribute;      /* text attribute */
   unsigned char normattr;       /* normal attribute */
   unsigned char currmode;       /* BW40, BW80, C40, C80, or C4350 */
   unsigned char screenheight;   /* text screen's height */

   unsigned char screenwidth;    /* text screen's width */
   unsigned char curx;           /* x-coordinate in current window */
   unsigned char cury;           /* y-coordinate in current window */
};

/*****************structs defined in time.h********************/
struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
    long int tm_gmtoff;
    const char *tm_zone;
};

/*************************************Prototype definitions here********************************/

double atof(const char *ascii);
int atoi(const char *str);
void charputc(char c);
int chdir (const char *filename);
int closefile(FILE* fhandle);
void  clrscr();
int cprintf(const char *fmt, ...);
void dexsdk_crt();
int dexsdk_systemcall(int function_num,int p1,int p2,int p3,int p4,int p5);
double difftime (time_t time1, time_t time0);
void directputchar(char x,char y,char c,char color);
int do_printf(const char *fmt, va_list args, fnptr_t fn,FILE *f, void *ptr);
int dex_exit(int val);
int fclose(FILE *stream);
int fgetc (FILE *stream);
char *fgets(char *s, int n, FILE* f);
int fflush (FILE *stream);
FILE *fopen(const char *filename,const char *s);
int fprintf(FILE *f,const char *fmt, ...);
char fputc(char c,FILE *f);
int fputs (const char *s, FILE *stream);
int fread(const void *buf,int itemsize,int noitems,FILE* fhandle);
int fscanf(FILE *f, const char *fmt, ...);
char *fseek(FILE* f,long x,int y);
long int ftell(FILE *stream);
int fwrite(const void *buf,int itemsize,int noitems,FILE* fhandle);
void free(void *ptr);
char getch();
int getchar();
char *getcwd (char *buffer, size_t size);
char * getenv (const char *name);
void getparameters(char *buf);
char *gets(char *buf);
int  gettext(int left, int top, int right, int bottom, void *destin);
void gettextinfo(struct text_info *r);
int getx();
int gety();
int kb_deq(int *code);
int kbhit(void);
int isalnum(int ci);
int	isalpha(int ci);
int	isdigit(int ci);
int	isspace(int ci);
int	isupper(int ci);
ldiv_t ldiv(long num, long denom);
struct tm * localtime (const time_t *time);
void *malloc(size_t size);
void *memchr(const void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void * memcpy (void * dst, const void * src,unsigned int count);
void * memset (void *dst,int val,unsigned int count);
int mkdir (const char *filename, mode_t mode);
int movetext(int left, int top, int right, int bottom, int destleft, int desttop);
void normvideo(void);
FILE *openfile(const char *filename,int mode);
void outc(char x);
int printf(const char *fmt, ...);
int putc (int c, FILE *stream);
int puts (const char *s); 
int puttext(int left, int top, int right, int bottom, void *source);
void *realloc(void *ptr, size_t size);
int remove(char *filename);
void  *sbrk(int amt);
int scanf(const char *fmt, ...);
int sscanf(const char *str, const char *fmt, ...);
void setx(int x);
void sety(int y);
int sprintf(char *buffer, const char *fmt, ...);
char *strcat(char *s, const char *append);
char *strchr(const char *s, int c);
int strcmp(const char *s1, const char *s2);
int strcoll(const char *s1, const char *s2);
char *strcpy(char *to, const char *from);
char *strncpy(char *dst, const char *src, size_t n);
size_t strcspn(const char *s1, const char *s2);
size_t strlen(const char *str);
char *strncat(char *dst, const char *src, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
char *strrchr(const char *s, int c);
size_t strspn(const char *s1, const char *s2);
char *strstr(const char *s, const char *find);
char *strtok(char *s, const char *delim);
void textattr(int newattr);
void textcolor(char val);
void textbackground(char val);
void textmode(int newmode);
time_t time (time_t *result);
char tolower(char c);
char toupper(char c);
int ungetc(int c, FILE *stream);
int wherex(void);
int wherey(void);

#endif
