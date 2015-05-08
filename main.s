	.text
	.file	"main.c"
	.section	.rodata.cst8,"aM",@progbits,8
	.align	8
.LCPI0_0:
	.quad	4613303441197561744     # double 2.71828
	.text
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:
	push	rbp
.Ltmp0:
	.cfi_def_cfa_offset 16
.Ltmp1:
	.cfi_offset rbp, -16
	mov	rbp, rsp
.Ltmp2:
	.cfi_def_cfa_register rbp
	sub	rsp, 288
	movsd	xmm0, qword ptr [.LCPI0_0]
	movabs	rax, .L.str
	mov	dword ptr [rbp - 4], 0
	mov	dword ptr [rbp - 8], edi
	mov	qword ptr [rbp - 16], rsi
	mov	qword ptr [rbp - 40], rax
	mov	qword ptr [rbp - 72], 0
	mov	qword ptr [rbp - 80], 255
	mov	qword ptr [rbp - 88], 100
	movsd	qword ptr [rbp - 96], xmm0
	mov	qword ptr [rbp - 104], 0
	mov	qword ptr [rbp - 112], 0
	mov	dword ptr [rbp - 116], 0
	cmp	dword ptr [rbp - 8], 1
	jle	.LBB0_27
# BB#1:
	mov	rax, qword ptr [rbp - 16]
	mov	rdi, qword ptr [rax + 8]
	mov	ecx, .L.str1
	mov	esi, ecx
	call	strcmp
	cmp	eax, 0
	jne	.LBB0_3
# BB#2:
	mov	rax, qword ptr [stdin]
	mov	qword ptr [rbp - 104], rax
	jmp	.LBB0_26
.LBB0_3:
	jmp	.LBB0_4
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movabs	rdx, .L.str2
	mov	edi, dword ptr [rbp - 8]
	mov	rsi, qword ptr [rbp - 16]
	call	getopt
	mov	cl, al
	mov	byte ptr [rbp - 17], cl
	movsx	eax, cl
	xor	eax, 4294967295
	cmp	eax, 0
	je	.LBB0_25
# BB#5:                                 #   in Loop: Header=BB0_4 Depth=1
	movsx	eax, byte ptr [rbp - 17]
	mov	ecx, eax
	sub	ecx, 96
	mov	dword ptr [rbp - 148], eax # 4-byte Spill
	mov	dword ptr [rbp - 152], ecx # 4-byte Spill
	jg	.LBB0_41
	jmp	.LBB0_40
.LBB0_40:                               #   in Loop: Header=BB0_4 Depth=1
	mov	eax, dword ptr [rbp - 148] # 4-byte Reload
	sub	eax, 63
	mov	dword ptr [rbp - 156], eax # 4-byte Spill
	je	.LBB0_17
	jmp	.LBB0_24
.LBB0_41:                               #   in Loop: Header=BB0_4 Depth=1
	mov	eax, dword ptr [rbp - 148] # 4-byte Reload
	sub	eax, 117
	mov	dword ptr [rbp - 160], eax # 4-byte Spill
	jg	.LBB0_43
	jmp	.LBB0_42
.LBB0_42:                               #   in Loop: Header=BB0_4 Depth=1
	mov	eax, dword ptr [rbp - 148] # 4-byte Reload
	add	eax, -97
	mov	ecx, eax
	sub	eax, 8
	mov	qword ptr [rbp - 168], rcx # 8-byte Spill
	mov	dword ptr [rbp - 172], eax # 4-byte Spill
	ja	.LBB0_24
# BB#45:                                #   in Loop: Header=BB0_4 Depth=1
	mov	rax, qword ptr [rbp - 168] # 8-byte Reload
	mov	rcx, qword ptr [8*rax + .LJTI0_0]
	jmp	rcx
.LBB0_43:                               #   in Loop: Header=BB0_4 Depth=1
	mov	eax, dword ptr [rbp - 148] # 4-byte Reload
	sub	eax, 120
	mov	dword ptr [rbp - 176], eax # 4-byte Spill
	je	.LBB0_12
	jmp	.LBB0_44
.LBB0_44:                               #   in Loop: Header=BB0_4 Depth=1
	mov	eax, dword ptr [rbp - 148] # 4-byte Reload
	sub	eax, 118
	mov	dword ptr [rbp - 180], eax # 4-byte Spill
	jne	.LBB0_24
	jmp	.LBB0_6
.LBB0_6:                                #   in Loop: Header=BB0_4 Depth=1
	mov	dword ptr [rbp - 116], 1
	jmp	.LBB0_24
.LBB0_7:                                #   in Loop: Header=BB0_4 Depth=1
	xor	eax, eax
	mov	esi, eax
	mov	edx, 10
	mov	rdi, qword ptr [optarg]
	call	strtol
	mov	qword ptr [rbp - 80], rax
	jmp	.LBB0_24
.LBB0_8:                                #   in Loop: Header=BB0_4 Depth=1
	xor	eax, eax
	mov	esi, eax
	mov	edx, 10
	mov	rdi, qword ptr [optarg]
	call	strtol
	mov	qword ptr [rbp - 88], rax
	mov	rax, qword ptr [rbp - 88]
	xor	rax, -1
	cmp	rax, 0
	je	.LBB0_10
# BB#9:                                 #   in Loop: Header=BB0_4 Depth=1
	mov	rax, qword ptr [rbp - 88]
	mov	qword ptr [rbp - 192], rax # 8-byte Spill
	jmp	.LBB0_11
.LBB0_10:                               #   in Loop: Header=BB0_4 Depth=1
	mov	eax, 9999999
	mov	ecx, eax
	mov	qword ptr [rbp - 192], rcx # 8-byte Spill
	jmp	.LBB0_11
.LBB0_11:                               #   in Loop: Header=BB0_4 Depth=1
	mov	rax, qword ptr [rbp - 192] # 8-byte Reload
	mov	qword ptr [rbp - 88], rax
	jmp	.LBB0_24
.LBB0_12:                               #   in Loop: Header=BB0_4 Depth=1
	xor	eax, eax
	mov	esi, eax
	mov	rdi, qword ptr [optarg]
	call	strtof
	cvtss2sd	xmm0, xmm0
	movsd	qword ptr [rbp - 96], xmm0
	jmp	.LBB0_24
.LBB0_13:                               #   in Loop: Header=BB0_4 Depth=1
	mov	rdi, qword ptr [optarg]
	call	strlen
	add	rax, 1
	mov	qword ptr [rbp - 72], rax
	mov	rax, qword ptr [rbp - 72]
	shl	rax, 0
	add	rax, 1
	mov	rdi, rax
	call	malloc
	mov	qword ptr [rbp - 64], rax
	mov	rdi, qword ptr [rbp - 64]
	mov	rsi, qword ptr [optarg]
	mov	rdx, qword ptr [rbp - 72]
	call	memcpy
	jmp	.LBB0_24
.LBB0_14:                               #   in Loop: Header=BB0_4 Depth=1
	movabs	rsi, .L.str3
	mov	rdi, qword ptr [optarg]
	call	fopen
	xor	ecx, ecx
	mov	esi, ecx
	mov	qword ptr [rbp - 104], rax
	cmp	rsi, qword ptr [rbp - 104]
	jne	.LBB0_16
# BB#15:
	mov	rdi, qword ptr [stderr]
	mov	rdx, qword ptr [optarg]
	mov	qword ptr [rbp - 200], rdi # 8-byte Spill
	mov	qword ptr [rbp - 208], rdx # 8-byte Spill
	call	__errno_location
	mov	edi, dword ptr [rax]
	call	strerror
	movabs	rsi, .L.str4
	mov	rdi, qword ptr [rbp - 200] # 8-byte Reload
	mov	rdx, qword ptr [rbp - 208] # 8-byte Reload
	mov	rcx, rax
	mov	al, 0
	call	fprintf
	mov	edi, 1
	mov	dword ptr [rbp - 212], eax # 4-byte Spill
	call	exit
.LBB0_16:                               #   in Loop: Header=BB0_4 Depth=1
	jmp	.LBB0_24
.LBB0_17:
	cmp	dword ptr [optopt], 105
	jne	.LBB0_19
# BB#18:
	movabs	rsi, .L.str5
	mov	rdi, qword ptr [stderr]
	mov	edx, dword ptr [optopt]
	mov	al, 0
	call	fprintf
	mov	dword ptr [rbp - 216], eax # 4-byte Spill
	jmp	.LBB0_23
.LBB0_19:
	movsxd	rax, dword ptr [optopt]
	mov	qword ptr [rbp - 224], rax # 8-byte Spill
	call	__ctype_b_loc
	mov	rax, qword ptr [rax]
	mov	rcx, qword ptr [rbp - 224] # 8-byte Reload
	movzx	edx, word ptr [rax + 2*rcx]
	and	edx, 16384
	cmp	edx, 0
	je	.LBB0_21
# BB#20:
	movabs	rsi, .L.str6
	mov	rdi, qword ptr [stderr]
	mov	edx, dword ptr [optopt]
	mov	al, 0
	call	fprintf
	mov	dword ptr [rbp - 228], eax # 4-byte Spill
	jmp	.LBB0_22
.LBB0_21:
	movabs	rsi, .L.str7
	mov	rdi, qword ptr [stderr]
	mov	edx, dword ptr [optopt]
	mov	al, 0
	call	fprintf
	mov	dword ptr [rbp - 232], eax # 4-byte Spill
.LBB0_22:
	jmp	.LBB0_23
.LBB0_23:
	mov	dword ptr [rbp - 4], 1
	jmp	.LBB0_39
.LBB0_24:                               #   in Loop: Header=BB0_4 Depth=1
	jmp	.LBB0_4
.LBB0_25:
	jmp	.LBB0_26
.LBB0_26:
	jmp	.LBB0_28
.LBB0_27:
	mov	rax, qword ptr [stdin]
	mov	qword ptr [rbp - 104], rax
.LBB0_28:
	movabs	rsi, .L.str8
	mov	rdi, qword ptr [rbp - 40]
	call	fopen
	mov	qword ptr [rbp - 112], rax
	cmp	qword ptr [rbp - 72], 0
	jne	.LBB0_33
# BB#29:
	jmp	.LBB0_30
.LBB0_30:                               # =>This Inner Loop Header: Depth=1
	mov	rdi, qword ptr [rbp - 104]
	call	fgetc
	mov	ecx, 4294967295
	mov	dl, al
	mov	byte ptr [rbp - 41], dl
	movsx	eax, dl
	cmp	ecx, eax
	je	.LBB0_32
# BB#31:                                #   in Loop: Header=BB0_30 Depth=1
	mov	eax, 1
	mov	ecx, eax
	lea	rdi, [rbp - 41]
	mov	rdx, qword ptr [rbp - 112]
	mov	rsi, rcx
	mov	qword ptr [rbp - 240], rdx # 8-byte Spill
	mov	rdx, rcx
	mov	rcx, qword ptr [rbp - 240] # 8-byte Reload
	call	fwrite
	add	rax, qword ptr [rbp - 72]
	mov	qword ptr [rbp - 72], rax
	jmp	.LBB0_30
.LBB0_32:
	jmp	.LBB0_34
.LBB0_33:
	mov	eax, 1
	mov	esi, eax
	mov	rdi, qword ptr [rbp - 64]
	mov	rdx, qword ptr [rbp - 72]
	mov	rcx, qword ptr [rbp - 112]
	call	fwrite
	mov	rdi, qword ptr [rbp - 64]
	mov	qword ptr [rbp - 248], rax # 8-byte Spill
	call	free
.LBB0_34:
	mov	rdi, qword ptr [rbp - 112]
	call	fclose
	cmp	qword ptr [rbp - 104], 0
	mov	dword ptr [rbp - 252], eax # 4-byte Spill
	je	.LBB0_36
# BB#35:
	mov	rdi, qword ptr [rbp - 104]
	call	fclose
	mov	dword ptr [rbp - 256], eax # 4-byte Spill
.LBB0_36:
	movabs	rsi, .L.str3
	mov	rdi, qword ptr [rbp - 40]
	call	fopen
	mov	qword ptr [rbp - 112], rax
	mov	rax, qword ptr [rbp - 72]
	shl	rax, 0
	add	rax, 1
	mov	rdi, rax
	call	malloc
	mov	ecx, 1
	mov	esi, ecx
	mov	qword ptr [rbp - 32], rax
	mov	rdi, qword ptr [rbp - 32]
	mov	rdx, qword ptr [rbp - 72]
	mov	rcx, qword ptr [rbp - 112]
	call	fread
	mov	rcx, qword ptr [rbp - 72]
	shl	rcx, 0
	mov	rdx, qword ptr [rbp - 32]
	mov	byte ptr [rdx + rcx], 0
	mov	rdi, qword ptr [rbp - 112]
	mov	qword ptr [rbp - 264], rax # 8-byte Spill
	call	fclose
	mov	rdi, qword ptr [rbp - 40]
	mov	dword ptr [rbp - 268], eax # 4-byte Spill
	call	remove
	xor	r8d, r8d
	mov	edx, r8d
	mov	rdi, qword ptr [rbp - 80]
	mov	rsi, qword ptr [rbp - 88]
	movsd	xmm0, qword ptr [rbp - 96]
	mov	dword ptr [rbp - 272], eax # 4-byte Spill
	call	new_arena_options
	mov	qword ptr [rbp - 128], rax
	mov	rdi, qword ptr [rbp - 32]
	mov	rsi, qword ptr [rbp - 128]
	mov	edx, dword ptr [rbp - 116]
	call	tokenize3
	mov	qword ptr [rbp - 136], rax
	mov	rdi, qword ptr [rbp - 136]
	call	normalize
	mov	rdi, qword ptr [rbp - 136]
	mov	qword ptr [rbp - 280], rax # 8-byte Spill
	call	analyze
	mov	qword ptr [rbp - 144], rax
	cmp	dword ptr [rbp - 116], 0
	je	.LBB0_38
# BB#37:
	mov	rax, qword ptr [rbp - 136]
	mov	rdi, qword ptr [rax]
	call	print_tokens
.LBB0_38:
	mov	rdi, qword ptr [rbp - 136]
	call	free_tokens
	mov	rdi, qword ptr [rbp - 128]
	call	free
	mov	rdi, qword ptr [rbp - 32]
	call	free
	mov	dword ptr [rbp - 4], 0
.LBB0_39:
	mov	eax, dword ptr [rbp - 4]
	add	rsp, 288
	pop	rbp
	ret
.Ltmp3:
	.size	main, .Ltmp3-main
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.align	8
.LJTI0_0:
	.quad	.LBB0_8
	.quad	.LBB0_7
	.quad	.LBB0_24
	.quad	.LBB0_24
	.quad	.LBB0_13
	.quad	.LBB0_24
	.quad	.LBB0_24
	.quad	.LBB0_24
	.quad	.LBB0_14

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	".dlqtemp"
	.size	.L.str, 9

	.type	.L.str1,@object         # @.str1
.L.str1:
	.asciz	"-"
	.size	.L.str1, 2

	.type	.L.str2,@object         # @.str2
.L.str2:
	.asciz	"vb:a:x:e:i:"
	.size	.L.str2, 12

	.type	.L.str3,@object         # @.str3
.L.str3:
	.asciz	"r"
	.size	.L.str3, 2

	.type	.L.str4,@object         # @.str4
.L.str4:
	.asciz	"Unable to open '%s': %s\n"
	.size	.L.str4, 25

	.type	.L.str5,@object         # @.str5
.L.str5:
	.asciz	"Option -%c requires an argument.\n"
	.size	.L.str5, 34

	.type	.L.str6,@object         # @.str6
.L.str6:
	.asciz	"Unknown option `-%c'.\n"
	.size	.L.str6, 23

	.type	.L.str7,@object         # @.str7
.L.str7:
	.asciz	"Unknown option character `\\x%x'.\n"
	.size	.L.str7, 34

	.type	.L.str8,@object         # @.str8
.L.str8:
	.asciz	"w"
	.size	.L.str8, 2


	.ident	"clang version 3.6.0 (tags/RELEASE_360/final)"
	.section	".note.GNU-stack","",@progbits
