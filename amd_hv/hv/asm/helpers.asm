vcpu STRUCT
    guest_vmcb        BYTE 1000h DUP(?)
    host_vmcb         BYTE 1000h DUP(?)
    host_save_area    BYTE 1000h DUP(?)
    host_stack_base   QWORD ?
    host_stack        QWORD ?
    guest_vmcb_phys   QWORD ?
    host_vmcb_phys    QWORD ?
    regs              QWORD ?
vcpu ENDS

.code

PUSHAQ macro
        push    rax
        push    rcx
        push    rdx
        push    rbx
        push    rbx
        push    rbp
        push    rsi
        push    rdi
        push    r8
        push    r9
        push    r10
        push    r11
        push    r12
        push    r13
        push    r14
        push    r15
        endm
POPAQ macro
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     rdi
        pop     rsi
        pop     rbp
        pop     rbx    
        pop     rbx
        pop     rdx
        pop     rcx
        pop     rax
endm


send_hv_command proc frame
.endprolog

xor rax, rax    ; clear rax to have accurate results

cpuid           ; we intercept this instruction

cmp rax, 1		; check if its exactly 1, if its a valid command it will set rax to 1

je return		; if it is, return it

xor rax, rax	; if its not, return 0

return:
ret

send_hv_command endp


__sgdt proc frame
.endprolog

sgdt [rcx] 
ret

__sgdt endp

__str proc frame
.endprolog

str ax
ret

__str endp

__sldt proc frame
.endprolog

sldt ax
ret

__sldt endp


; extern their cpp symbol so the compiler can link it
extern ?handle_vmexit@hv@@YA_NPEAUvcpu@@@Z : proc
extern ?cleanup@hv@@YAXPEAUvcpu@@@Z : proc

start_hv proc frame
.endprolog


mov [rcx + 400h + 1d8h], rsp ; rsp inside guest vmcb

lea rax, OFFSET vmentry      ; we will move this label into guest vmcb so it will continue execution there
mov [rcx + 400h + 178h], rax ; rip inside guest vmcb

mov rsp, [rcx + vcpu.host_stack] ; load host stack
mov [rsp], rcx ; set vcpu ptr into stack to be used later

mov rax, [rcx + vcpu.guest_vmcb_phys] ; prepare guest physical vmcb to be loaded

jmp vmloop

vmentry:
ret ; we just return here, rax has been set to 1 inside guest vmcb already in the vcpu setup function

vmloop:

vmload rax  ; load more guest state
vmrun  rax  ; run the guest
vmsave rax  ; after vmexit, save guest state

push rax    ; backup guest state

mov rax, [rsp + 8h] ; put current vcpu into rax
mov rax, [rax + vcpu.host_vmcb_phys] ; load the host into rax
vmload rax  ; load host state

pop rax     ; restore guest state

PUSHAQ

mov rcx, [rsp + 8 * 16]	; move vcpu into rcx
mov [rcx + vcpu.regs], rsp ; put the ptr of the registers into its place

sub rsp, 30h  ; make space for the function call
call ?handle_vmexit@hv@@YA_NPEAUvcpu@@@Z
add rsp, 30h

test al, al ; test returned char 

POPAQ


jz vmloop ; if the returned char is zero, it means continue executing

; we will dirty rax here, but thats about it, other regs will be preserved

mov rax, [rsp] ; vcpu into rax
mov rsp, [rax + 400h + 1d8h] ; restore rsp from guest vmcb

push rax ; vcpu onto stack

PUSHAQ ; backup all the registers

mov rcx, [rsp + 8 * 16]	; move vcpu into rcx
mov [rcx + vcpu.regs], rsp ; put the ptr of the registers into its place

sub rsp, 30h  ; make space for the function call
call ?cleanup@hv@@YAXPEAUvcpu@@@Z ; we call the cleanup on the guest stack so we can free the host stack allocation
add rsp, 30h

POPAQ ; restore all registers

ret ; restore rip, the value on the stack here is overwritten by our cleanup function

start_hv endp


END