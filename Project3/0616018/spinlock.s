
.text
.global spin_lock
.global spin_unlock

spin_lock:
  xorl %ecx, %ecx
  incl %ecx
  loop:
    xorl %eax, %eax
    lock cmpxchgl %ecx, (%rdi)
    jnz loop
	ret

spin_unlock:
  movl $0, (%rdi)
	ret
