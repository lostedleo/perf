; =============================================================================
;  tea.asm
; -----------------------------------------------------------------------------
; 
; 
; Revision History:
; -----------------------------------------------------------------------------
; 
; 
; =============================================================================

%include "nasmBase.inc"

bits 64

; =============================================================================
; sections

%define Delta        0x9E3779B9
%define Sum32        0xC6EF3720
%define Sum16        0xE3779B90


; -----------------------------------------------------------------------------
section data align=16
align 16
  delta		times 4	dd	Delta	
  delta32	times 4 dd	Sum32
  delta16	times 4 dd	Sum16

; -----------------------------------------------------------------------------
section .text
align 16

; =============================================================================
; macros

; =============================================================================
; ENCRYPT_32_BYTES
; n0 += (n1<<4 + key0)^(n1 + sum)^(n1>>5 + key1)
; n1 += (n0<<4 + key2)^(n0 + sum)^(n0>>5 + key3)
%macro ENCRYPT_32_BYTES		4

	pxor			xmm0, xmm0				; sum = 0
	movdqu			xmm1, [%1      ]		; n3, n2, n1, n0
	movdqu			xmm3, [%1 +  16]		; n7, n6, n5, n4
	pshufd			xmm1, xmm1, 0xd8		; n3, n1, n2, n0
	pshufd			xmm3, xmm3, 0xd8		; n7, n5, n6, n4
	movdqa			xmm2, xmm1				; n3, n1, n2, n0
	punpckldq		xmm1, xmm3				; n6, n2, n4, n0
	punpckhdq		xmm2, xmm3				; n7, n3, n5, n1
	
.L_encrypt_32:
	paddd			xmm0, xmm7				; sum += delta 
	movdqa			xmm3, xmm2				; n7, n3, n5, n1
	movdqa			xmm4, xmm2				; n7, n3, n5, n1
	movdqa			xmm5, xmm2				; n7, n3, n5, n1
	pslld			xmm3, 4					; n1<<4...
	paddd			xmm3, [%3]				; n1<<4 + key0
	paddd			xmm4, xmm0				; n1 + sum
	psrld			xmm5, 5					; n1>>5...
	paddd			xmm5, [%3 +  16]		; n1>>5 + key1
	pxor			xmm3, xmm4				; (n1<<4 + key0)^(n1 + sum)
	pxor			xmm3, xmm5				; (n1<<4 + key0)^(n1 + sum)^(n1>>5 + key1)
	paddd			xmm1, xmm3				; n0 += (n1<<4 + key0)^(n1 + sum)^(n1>>5 + key1)
	movdqa			xmm3, xmm1				; n0...
	movdqa			xmm4, xmm1				; n0...
	movdqa			xmm5, xmm1				; n0...
	pslld			xmm3, 4					; n0<<4...
	paddd			xmm3, [%3 +  32]		; n0<<4 + key2
	paddd			xmm4, xmm0				; n0 + sum
	psrld			xmm5, 5					; n0>>5...
	paddd			xmm5, [%3 +  48]		; n0>>5 + key3
	pxor			xmm3, xmm4				; (n0<<4 + key2)^(n0 + sum)
	pxor			xmm3, xmm5				; (n0<<4 + key2)^(n0 + sum)^(n0>>5 + key3)
	paddd			xmm2, xmm3				; n1 += (n0<<4 + key2)^(n0 + sum)^(n0>>5 + key3)
	
	dec				%4
    jnz				.L_encrypt_32
	
	pshufd			xmm1, xmm1, 0xd8		; n6, n4, n2, n0
	pshufd			xmm2, xmm2, 0xd8		; n7, n5, n3, n1
	movdqa			xmm3, xmm1				; n6, n4, n2, n0
	punpckldq		xmm1, xmm2				; n3, n2, n1, n0
	punpckhdq		xmm3, xmm2				; n7, n6, n5, n4
	movdqu			[%2], xmm1				; store n3...n0
	movdqu			[%2+16], xmm3			; store n7...n4
	
%endmacro

; =============================================================================
; decrypt_32_bytes
; n1 -= (n0<<4 + key2)^(n0 + sum)^(n0>>5 + key3)
; n0 -= (n1<<4 + key0)^(n1 + sum)^(n1>>5 + key1)
%macro DECRYPT_32_BYTES		5

	movdqu			xmm0, [%5]				; sum
	movdqu			xmm1, [%1      ]		; n3, n2, n1, n0
	movdqu			xmm3, [%1 +  16]		; n7, n6, n5, n4
	pshufd			xmm1, xmm1, 0xd8		; n3, n1, n2, n0
	pshufd			xmm3, xmm3, 0xd8		; n7, n5, n6, n4
	movdqa			xmm2, xmm1				; n3, n1, n2, n0
	punpckldq		xmm1, xmm3				; n6, n2, n4, n0
	punpckhdq		xmm2, xmm3				; n7, n3, n5, n1
	
.L_decrypt_32:
	movdqa			xmm3, xmm1				; n0...
	movdqa			xmm4, xmm1				; n0...
	movdqa			xmm5, xmm1				; n0...
	pslld			xmm3, 4					; n0<<4...
	paddd			xmm3, [%3 +  32]		; n0<<4 + key2
	paddd			xmm4, xmm0				; n0 + sum
	psrld			xmm5, 5					; n0>>5...
	paddd			xmm5, [%3 +  48]		; n0>>5 + key3
	pxor			xmm3, xmm4				; (n0<<4 + key2)^(n0 + sum)
	pxor			xmm3, xmm5				; (n0<<4 + key2)^(n0 + sum)^(n0>>5 + key3)
	psubd			xmm2, xmm3				; n1 -= (n0<<4 + key2)^(n0 + sum)^(n0>>5 + key3)
	movdqa			xmm3, xmm2				; n1, n5, n3, n7
	movdqa			xmm4, xmm2				; n1, n5, n3, n7
	movdqa			xmm5, xmm2				; n1, n5, n3, n7
	pslld			xmm3, 4					; n1<<4...
	paddd			xmm3, [%3      ]		; n1<<4 + key0
	paddd			xmm4, xmm0				; n1 + sum
	psrld			xmm5, 5					; n1>>5...
	paddd			xmm5, [%3 +  16]		; n1>>5 + key1
	pxor			xmm3, xmm4				; (n1<<4 + key0)^(n1 + sum)
	pxor			xmm3, xmm5				; (n1<<4 + key0)^(n1 + sum)^(n1>>5 + key1)
	psubd			xmm1, xmm3				; n0 -= (n1<<4 + key0)^(n1 + sum)^(n1>>5 + key1)
	psubd			xmm0, xmm7				; sum -= delta
	
	dec				%4
    jnz				.L_decrypt_32
	
	pshufd			xmm1, xmm1, 0xd8		; n6, n4, n2, n0
	pshufd			xmm2, xmm2, 0xd8		; n7, n5, n3, n1
	movdqa			xmm3, xmm1				; n6, n4, n2, n0
	punpckldq		xmm1, xmm2				; n3, n2, n1, n0
	punpckhdq		xmm3, xmm2				; n7, n6, n5, n4
	movdqu			[%2], xmm1				; store n3...n0
	movdqu			[%2+16], xmm3			; store n7...n4
	
%endmacro

 ; ----------------------------------------------------------------------------
; c_style declare:
; void tea_encrypt_sse_x64(uint32_t *pIn, uint32_t *pOut, uint32_t uiLen, uint32_t *pKey, uint32_t *pDelta, uint32_t round)
; ----------------------------------------------------------------------------
; rdi, rsi, rdx, rcx, r8, r9
align 16
cglobal tea_encrypt_sse
tea_encrypt_sse:
 ;{
	movdqu			xmm7, [r8]				; xmm7 <== delta
	
	mov 			rax, r9
	sar				rdx, 5					; uiLen/32
	
.L_encrypt:
	ENCRYPT_32_BYTES	rdi, rsi, rcx, r9
	
	mov 			r9, rax
	add				rdi, 32
	add				rsi, 32
	dec				rdx
	jnz				.L_encrypt
	
.L_End:

	ret
 ;}
 
 ; ----------------------------------------------------------------------------
; c_style declare:
; void tea_decrypt_sse_x64(uint32_t *pIn, uint32_t *pOut, uint32_t uiLen, uint32_t *pKey, uint32_t *pDelta, uint32_t *pSum, uint32_t round)
; ----------------------------------------------------------------------------
; rdi, rsi, rdx, rcx, r8, r9
align 16
cglobal tea_decrypt_sse
tea_decrypt_sse:
 ;{
	push			rbp
	mov				rbp, rsp
	push  			rbx
 
	mov				rax, [rbp + 16]			; round
	sal				rax, 32
	sar				rax, 32
	mov      		rbx, rax
	movdqu			xmm7, [r8]				; xmm7 <== delta
	
	sar				rdx, 5					; uiLen/32
	
.L_decrypt:
	DECRYPT_32_BYTES	rdi, rsi, rcx, rax, r9
	
	mov 			rax, rbx
	add				rdi, 32
	add				rsi, 32
    dec				rdx
	jnz				.L_decrypt
	
.L_End:
	
	pop 			rbx
	pop				rbp
	emms
	ret
 ;}
