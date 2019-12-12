
.text
.global spin_lock
.global spin_unlock

spin_lock:
    push %rbp
    mov %rsp,%rbp
	mov %rdi, -0x8(%rbp) # get first parameter
    mov -0x8(%rbp), %rax
loop:
    LOCK BTS $0x0, (%rax)  #test and set first bit
    JC loop
    pop %rbp
	ret

spin_unlock:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, -0x8(%rbp)
    mov -0x8(%rbp), %rax
    movl $0x0, (%rax)
    pop %rbp
	ret
