;This file handles all the wrappers for the interrupt handlers

[bits 32]

section .text

extern _time_handler
extern _kbd_irq
extern _pagefaulthandler
extern _fdchandler
extern _api_syscall
extern _GPFhandler
extern _CPUint
extern _nocoprocessor
extern _divide_error
extern _exc_invalidtss
extern _irq_activate

;this function handles the normal DEX32 system calls
global _syscallwrapper
_syscallwrapper:
push edi
push esi
push edx
push ecx
push ebx
push eax
call _api_syscall
add esp,24
iret



global _CPUintwrapper
_CPUintwrapper:
cli
 call _CPUint
sti
iret


global _fdcwrapper
_fdcwrapper:
cli
pusha
call _fdchandler
popa
sti
iret

global _copwrapper
_copwrapper:
cli
call _nocoprocessor
sti
iret

global _timerwrapper
_timerwrapper:
cli
push ebp
push gs
push fs
push es
push ss
push ds
pusha

call _time_handler

popa
pop ds
pop ss
pop es
pop fs
pop gs
pop ebp
sti
iret


global _kbdwrapper
_kbdwrapper:
push ebp
push gs
push fs
push es
push ss
push ds
pusha

call _kbd_irq

popa
pop ds
pop ss
pop es
pop fs
pop gs
pop ebp

iret

global _irq1wrapper
_irq1wrapper:
push ebp
push gs
push fs
push es
push ss
push ds
pusha

push 1
call _irq_activate
add esp,4

popa
pop ds
pop ss
pop es
pop fs
pop gs
pop ebp
iret


global _irq2wrapper
_irq2wrapper:
push ebp
push gs
push fs
push es
push ss
push ds
pusha

push 2
call _irq_activate
add esp,4

popa
pop ds
pop ss
pop es
pop fs
pop gs
pop ebp
iret

global _irq3wrapper
_irq3wrapper:
push ebp
push gs
push fs
push es
push ss
push ds
pusha

push 3
call _irq_activate
add esp,4

popa
pop ds
pop ss
pop es
pop fs
pop gs
pop ebp
iret

global _irq4wrapper
_irq4wrapper:
push ebp
push gs
push fs
push es
push ss
push ds
pusha

push 4
call _irq_activate
add esp,4


popa
pop ds
pop ss
pop es
pop fs
pop gs
pop ebp
iret

global _irq5wrapper
_irq5wrapper:
push ebp
push gs
push fs
push es
push ss
push ds
pusha

push 5
call _irq_activate
add esp,4


popa
pop ds
pop ss
pop es
pop fs
pop gs
pop ebp
iret

global _irq6wrapper
_irq6wrapper:
push ebp
push gs
push fs
push es
push ss
push ds
pusha

push 6
call _irq_activate
add esp,4

popa
pop ds
pop ss
pop es
pop fs
pop gs
pop ebp
iret

global _irq7wrapper
_irq7wrapper:
push ebp
push gs
push fs
push es
push ss
push ds
pusha

push 7
call _irq_activate
add esp,4

popa
pop ds
pop ss
pop es
pop fs
pop gs
pop ebp
iret

global _irq8wrapper
_irq8wrapper:
push ebp
push gs
push fs
push es
push ss
push ds
pusha

push 8
call _irq_activate
add esp,4


popa
pop ds
pop ss
pop es
pop fs
pop gs
pop ebp
iret

global _div_wrapper
_div_wrapper:
cli 
push ebp
mov ebp,esp
mov eax,[ebp+8]
push eax
call _divide_error
add esp,4
pop ebp
sti
iret

global _gpfwrapper
_gpfwrapper:
cli 
push ebp
mov ebp,esp
mov eax,[ebp+8]
push eax
call _GPFhandler
add esp,4
pop ebp
sti
iret

global _invalidtsswrapper
_invalidtsswrapper:
cli 
push ebp
mov ebp,esp
mov eax,[ebp+8]
push eax
call _exc_invalidtss
add esp,4
pop ebp
sti
iret


global _pfwrapper
_pfwrapper:
cli
push ebp
mov ebp,esp
mov eax,[ebp+8]
push eax
mov eax,cr2
push eax
call _pagefaulthandler
add esp,8
sti
iret

SYS_CODE_SEL2   equ     10000b
SYS_STACK_SEL   equ     11000b
SYS_DATA_SEL    equ    100000b
