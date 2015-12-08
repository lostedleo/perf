; =============================================================================
; cpuid.asm
; -----------------------------------------------------------------------------
; 
; 
; Revision History:
; -----------------------------------------------------------------------------
; 
; 
; =============================================================================
bits 32

%include "nasmBase.inc"

; -----------------------------------------------------------------------------
%define CPU_MMX   0x01
%define CPU_SSE   0x02
%define CPU_SSE2  0x04
%define CPU_SSE3  0x08
%define CPU_SSSE3 0x10
%define CPU_SSE41 0x20
%define CPU_SSE42 0x40

; feature flag bit
%define BIT_MMX   0x00800000    ; bit 23 in EDX
%define BIT_SSE   0x02000000    ; bit 25 in EDX
%define BIT_SSE2  0x04000000    ; bit 26 in EDX
%define BIT_SSE3  0x00000001    ; bit  1 in ECX
%define BIT_SSSE3 0x00000200    ; bit 10 in ECX
%define BIT_SSE41 0x00080000    ; bit 20 in ECX
%define BIT_SSE42 0x00100000    ; bit 21 in ECX

section .text
align 16

; -----------------------------------------------------------------------------
; check cpu feature, result is in ebp (ebx is used as a temp register)
; %1 -- register, %2 -- bit to be checked, %3 -- bit to be setted
%macro CHECK 3
    mov     ebx, %2
    and     ebx, %1
    neg     ebx                 ; negation, if DEST = 0 then CF = 0 else CF = 1, DEST = -DEST
    sbb     ebx, ebx            ; integer subtraction with borrow, DEST = DEST - (SRC + CF)
    and     ebx, %3
    or      ebp, ebx            ; save result
%endmacro


; -----------------------------------------------------------------------------
; c-style declare:
;   int32_t detect_simd_ability(void);
; -----------------------------------------------------------------------------
cglobal detect_simd_ability
detect_simd_ability:
;{
    push    ebx
    push    ebp
    pushfd

    xor     ebp, ebp            ; clear ebp

    ; support cpuid instruction ?
    pushfd
    pop     eax
    xor     eax, 0x200000
    mov     ecx, eax
    push    eax
    popfd
    pushfd
    pop     eax
    popfd
    cmp     eax, ecx
    jnz near .quit

    ; cpuid
    mov     eax, 1                    ; request for feature flag
    cpuid                             ; cpuid instruction

    CHECK   edx, BIT_MMX,  CPU_MMX    ; support MMX ?
    CHECK   edx, BIT_SSE,  CPU_SSE    ; support SSE ?
    CHECK   edx, BIT_SSE2, CPU_SSE2   ; support SSE2?
    CHECK   ecx, BIT_SSE3, CPU_SSE3   ; support SSE3?
    CHECK   ecx, BIT_SSSE3, CPU_SSSE3 ; support SSSE3?
    CHECK   ecx, BIT_SSE41, CPU_SSE41 ; support SSE4.1?
    CHECK   ecx, BIT_SSE42, CPU_SSE42 ; support SSE4.2?

.quit:
    mov     eax, ebp                  ; return code
    pop     ebp
    pop     ebx
    ret
;}
