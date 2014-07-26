use32

global _start, start
global sys_sname, sys_exit, sys_exec
global sys_close, sys_open, sys_read, sys_write
global _errno

extern main


start:
_start:
    sub esp, 16
    and esp, 0xfffffff0
    xor eax, eax
    mov [esp], eax
    mov [esp + 4], eax
    call main
    mov ecx, eax
    mov eax, 0x10
    int 0x30
    jmp $

_errno:
    dd 0

; ----------------------------------------------------------------------------
; ----------------------------------------------------------------------------
sys_sname:
    mov eax, 0x00
    jmp os_syscall_2

; ----------------------------------------------------------------------------
; ----------------------------------------------------------------------------
sys_exit:
    mov eax, 0x10
    jmp os_syscall_2

sys_exec:
    mov eax, 0x11
    jmp os_syscall_2

; ----------------------------------------------------------------------------
; ----------------------------------------------------------------------------
sys_close:
    mov eax, 0x20
    jmp os_syscall_2

sys_open:
    mov eax, 0x21
    jmp os_syscall_3

sys_read:
    xor eax, eax
    mov al, 0x22
    jmp os_syscall_4

sys_write:
    mov eax, 0x23
    jmp os_syscall_4

; ----------------------------------------------------------------------------
; ----------------------------------------------------------------------------
os_syscall_2:
    push ebp
    mov ebp, esp
    mov ecx, [ebp + 8]   ; arg1
    mov edx, [ebp + 12]  ; arg2
    int 0x30
    leave
    ret

os_syscall_3:
    push ebp
    mov ebp, esp
    push ebx
    mov ecx, [ebp + 8]   ; arg1
    mov edx, [ebp + 12]  ; arg2
    mov ebx, [ebp + 16]  ; arg3
    int 0x30
    pop ebx
    leave
    ret

os_syscall_4:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    mov ecx, [ebp + 8]   ; arg1
    mov edx, [ebp + 12]  ; arg2
    mov ebx, [ebp + 16]  ; arg3
    mov esi, [ebp + 20]  ; arg4
    int 0x30
    pop esi
    pop ebx
    leave
    ret

os_syscall_5:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi
    mov ecx, [ebp + 8]   ; arg1
    mov edx, [ebp + 12]  ; arg2
    mov ebx, [ebp + 16]  ; arg3
    mov esi, [ebp + 20]  ; arg4
    mov edi, [ebp + 24]  ; arg5
    int 0x30
    pop edi
    pop esi
    pop ebx
    leave
    ret

; ----------------------------------------------------------------------------
; ----------------------------------------------------------------------------
