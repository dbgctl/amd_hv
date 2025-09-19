PUBLIC send_hv_command

.code

send_hv_command proc frame
.endprolog

xor rax, rax

cpuid

cmp rax, 1		; check if its exactly 1

je return		; if it is, return it

xor rax, rax	; if its not, return 0

return:
ret

send_hv_command endp

END 