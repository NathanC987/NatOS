# Declare constants for the multiboot header.
.set ALIGN,    1<<0                     # align loaded modules on page boundaries
.set MEMINFO,  1<<1                     # provide memory map
.set VIDEO,    1<<2
.set FLAGS,    ALIGN | MEMINFO | VIDEO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002               # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)         # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
/* Padding for address fields */
.long 0
.long 0
.long 0
.long 0
.long 0
/* Graphics mode preference fields */
.long 0    # 0 = Linear graphics mode
.long 1024 # Preferred width (pixels)
.long 768  # Preferred height (pixels)
.long 32   # Preferred depth (Bits Per Pixel)

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	# Save multiboot arguments to be later passed to kernel_main
	movl %eax, %esi
	movl %ebx, %edi

	# Call the global constructors.
	call _init

    # Pad the stack by 8 bytes to maintain 16-byte alignment at the call point
    sub $8, %esp

    # Push Multiboot arguments to kernel_main(magic, info_structure)
    push %edi
    push %esi

	# Transfer control to the main kernel.
	call kernel_main

    add $16, %esp

	# Hang if kernel_main unexpectedly returns.
	cli
1:  hlt
	jmp 1b
.size _start, . - _start
