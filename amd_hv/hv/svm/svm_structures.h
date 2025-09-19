#pragma once

#include "ia32.h"

#include <ntifs.h>
#include <windef.h>
#include <intrin.h>

// we disable this error to allow for more clean structures
#pragma warning(disable: 4201)

struct GENERAL_REGISTERS 
{
    UINT64  r15;
    UINT64  r14;
    UINT64  r13;
    UINT64  r12;
    UINT64  r11;
    UINT64  r10;
    UINT64  r9;
    UINT64  r8;
    UINT64  rdi;
    UINT64  rsi;
    UINT64  rbp;
    UINT64  rsp;
    UINT64  rbx;
    UINT64  rdx;
    UINT64  rcx;
    UINT64  rax;
};

enum AMD_MSR : UINT64
{
    vm_cr       = 0xC0010114,
    vm_hsave_pa = 0xC0010117,
};

// 3.1.7 Extended Feature Enable Register (EFER)
union AMD_EFER_MSR
{
    struct
    {
        UINT64 sce          : 1; // System Call Extensions
        UINT64 reserved1    : 7;
        UINT64 lme          : 1; // Long Mode Enable
        UINT64 reserved2    : 1;
        UINT64 lma          : 1; // Long Mode Active
        UINT64 nxe          : 1; // No-Execute Enable
        UINT64 svme         : 1; // Secure Virtual Machine Enable
        UINT64 lmsle        : 1; // Long Mode Segment Limit Enable
        UINT64 ffxse        : 1; // Fast FXSAVE/FXRSTOR
        UINT64 tce          : 1; // Translation Cache Extension
        UINT64 reserved     : 1;
        UINT64 mcommit      : 1; // Enable MCOMMIT instruction 
        UINT64 intwb        : 1; // Interruptible WBINVD/WBNOINVD enable
        UINT64 reserved3    : 1;
        UINT64 uaie         : 1; // Upper Address Ignore Enable
        UINT64 aibrse       : 1; // Automatic IBRS Enable 
        UINT64 reserved4    : 41;
    };
    UINT64 value;
};

// 15.30.1  VM_CR MSR 
union AMD_VM_CR_MSR
{
    struct
    {
        UINT64	dpd      : 1;   // DPD—Bit 0. If set, disables the external hardware debug port and certain internal debug features
        UINT64	r_init   : 1;   // R_INIT—Bit 1. If set, non-intercepted INIT signals are converted into an #SX exception. 
        UINT64	dis_a20m : 1;   // Bit 2. If set, disables A20 masking.
        UINT64	lock     : 1;   // Bit 3. When this bit is set, writes to LOCK and SVMDIS are silently ignored. When this
        // bit is clear, VM_CR bits 3 and 4 can be written.Once set, LOCK can only be cleared using the
        //     SVM_KEY MSR(See Section 15.31.) This bit is not affected by INIT or SKINIT.

        UINT64	svmdis   : 1;   // SVMDIS—Bit 4. When this bit is set, writes to EFER treat the SVME bit as MBZ. When this bit is
        // clear, EFER.SVME can be written normally.This bit does not prevent CPUID from reporting that
        // SVM is available.Setting SVMDIS while EFER.SVME is 1 generates a #GP fault, regardless of
        // the current state of VM_CR.LOCK.This bit is not affected by SKINIT.It is cleared by INIT when
        // LOCK is cleared to 0; otherwise, it is not affected.

        UINT64	reserved : 59;
    };
    UINT64 value;
};


// 14.1.3  Processor Initialization State 
// Table 14-2. Initial State of Segment-Register Attributes
union SEGMENT_ATTRIBUTE
{
    UINT16 value;
    struct
    {
        UINT16 type         : 4; 
        UINT16 system       : 1;    
        UINT16 dpl          : 2;       
        UINT16 present      : 1;   
        UINT16 avl          : 1;        
        UINT16 long_mode    : 1;  
        UINT16 default_bit  : 1;
        UINT16 granularity  : 1; 
        UINT16 reserved1    : 4;
    };
};

struct INTERCEPT_VECTOR_0
{
    // Intercept reads of CR0–15, respectively
    union
    {
        struct
        {
            UINT16 cr0  : 1;
            UINT16 cr1  : 1;
            UINT16 cr2  : 1;
            UINT16 cr3  : 1;
            UINT16 cr4  : 1;
            UINT16 cr5  : 1;
            UINT16 cr6  : 1;
            UINT16 cr7  : 1;
            UINT16 cr8  : 1;
            UINT16 cr9  : 1;
            UINT16 cr10 : 1;
            UINT16 cr11 : 1;
            UINT16 cr12 : 1;
            UINT16 cr13 : 1;
            UINT16 cr14 : 1;
            UINT16 cr15 : 1;
        };
    } read; 

    // Intercept writes of CR0–15, respectively
    union
    {
        struct
        {
            UINT16 cr0  : 1;
            UINT16 cr1  : 1;
            UINT16 cr2  : 1;
            UINT16 cr3  : 1;
            UINT16 cr4  : 1;
            UINT16 cr5  : 1;
            UINT16 cr6  : 1;
            UINT16 cr7  : 1;
            UINT16 cr8  : 1;
            UINT16 cr9  : 1;
            UINT16 cr10 : 1;
            UINT16 cr11 : 1;
            UINT16 cr12 : 1;
            UINT16 cr13 : 1;
            UINT16 cr14 : 1;
            UINT16 cr15 : 1;
        };
    } write; 
};

union INTERCEPT_VECTOR_1
{
    // Intercept reads of DR0–15, respectively
    union
    {
        struct
        {
            UINT16 dr0  : 1;
            UINT16 dr1  : 1;
            UINT16 dr2  : 1;
            UINT16 dr3  : 1;
            UINT16 dr4  : 1;
            UINT16 dr5  : 1;
            UINT16 dr6  : 1;
            UINT16 dr7  : 1;
            UINT16 dr8  : 1;
            UINT16 dr9  : 1;
            UINT16 dr10 : 1;
            UINT16 dr11 : 1;
            UINT16 dr12 : 1;
            UINT16 dr13 : 1;
            UINT16 dr14 : 1;
            UINT16 dr15 : 1;
        };
    } read;

    // Intercept writes of DR0–15, respectively
    union
    {
        struct
        {
            UINT16 dr0  : 1;
            UINT16 dr1  : 1;
            UINT16 dr2  : 1;
            UINT16 dr3  : 1;
            UINT16 dr4  : 1;
            UINT16 dr5  : 1;
            UINT16 dr6  : 1;
            UINT16 dr7  : 1;
            UINT16 dr8  : 1;
            UINT16 dr9  : 1;
            UINT16 dr10 : 1;
            UINT16 dr11 : 1;
            UINT16 dr12 : 1;
            UINT16 dr13 : 1;
            UINT16 dr14 : 1;
            UINT16 dr15 : 1;
        };
    } write;
};

// Intercept exception vectors 0–31, respectively
// AMD64 Manual Volume 2: 15.12 Exception Intercepts
union INTERCEPT_VECTOR_2
{
    struct
    {
        UINT32 de : 1;          // #DE (Divide By Zero)
        UINT32 db : 1;          // #DB (Debug)
        UINT32 reserved1 : 1;   // Vector 2 (Reserved) This intercept bit is not implemented; use the NMI intercept (Section 15.13.2) instead.
        UINT32 bp : 1;          // #BP (Breakpoint)
        UINT32 of : 1;          // #OF (Overflow)
        UINT32 br : 1;          // #BR (Bound-Range)
        UINT32 ud : 1;          // #UD (Invalid Opcode)
        UINT32 nm : 1;          // #NM (Device-Not-Available)
        UINT32 df : 1;          // #DF (Double Fault)
        UINT32 reserved2 : 1;   // Vector 9 (Reserved) This intercept is not implemented.
        UINT32 ts : 1;          // #TS (Invalid TSS)
        UINT32 np : 1;          // #NP (Segment Not Present)
        UINT32 ss : 1;          // #SS (Stack Fault)
        UINT32 gp : 1;          // #GP (General Protection)
        UINT32 pf : 1;          // #PF (Page Fault)
        UINT32 mf : 1;          // #MF (X87 Floating Point)
        UINT32 ac : 1;          // #AC (Alignment Check)
        UINT32 mc : 1;          // #MC (Machine Check)
        UINT32 xf : 1;          // #XF (SIMD Floating Point)
        UINT32 sx : 1;          // #SX (Security Exception)
        UINT32 cp : 1;          // #CP (Control Protection)

        UINT32 reserved3 : 11;
    };
    UINT32 value;
};

union INTERCEPT_VECTOR_3
{
    struct
    {
        UINT32 intr         : 1;    // Intercept INTR (physical maskable interrupt)
        UINT32 nmi          : 1;
        UINT32 smi          : 1;
        UINT32 init         : 1;
        UINT32 vintr        : 1;
        UINT32 cr0_misc     : 1;    // Intercept CR0 writes that change bits other than CR0.TS or CR0.MP
        UINT32 read_idtr    : 1;
        UINT32 read_gdtr    : 1;
        UINT32 read_ldtr    : 1;
        UINT32 read_tr      : 1;
        UINT32 write_idtr   : 1;
        UINT32 write_gdtr   : 1;
        UINT32 write_ldtr   : 1;
        UINT32 write_tr     : 1;
        UINT32 rdtsc        : 1;
        UINT32 rdpmc        : 1;
        UINT32 pushf        : 1;
        UINT32 popf         : 1;
        UINT32 cpuid        : 1;
        UINT32 rsm          : 1;
        UINT32 iret         : 1;
        UINT32 intn         : 1;
        UINT32 invd         : 1;
        UINT32 pause        : 1;
        UINT32 hlt          : 1;
        UINT32 invlpg       : 1;
        UINT32 invlpga      : 1;
        UINT32 ioio_prot    : 1;    // Intercept IN/OUT accesses to selected ports 
        UINT32 msr_prot     : 1;    // Intercept RDMSR or WRMSR accesses to selected MSRs
        UINT32 taskswitch   : 1;    // Intercept task switches (jmp, call, exceptions, etc)
        UINT32 ferr_freeze  : 1;
        UINT32 shutdown     : 1;    // Intercept shutdown events
    };
    UINT32 value;
};

union INTERCEPT_VECTOR_4
{
    struct
    {
        UINT32 vmrun        : 1;
        UINT32 vmmcall      : 1;
        UINT32 vmload       : 1;
        UINT32 vmsave       : 1;
        UINT32 stgi         : 1;
        UINT32 clgi         : 1;
        UINT32 skinit       : 1;
        UINT32 rdtscp       : 1;
        UINT32 icebp        : 1;
        UINT32 wbinvd       : 1;    // Intercept WBINVD and WBNOINVD instructions 
        UINT32 monitor      : 1;    // Intercept MONITOR/MONITORX instruction 
        UINT32 mwait_uncond : 1;    // Intercept MWAIT/MWAITX instruction unconditionally 
        UINT32 mwait_cond   : 1;    // Intercept MWAIT/MWAITX instruction if monitor hardware is armed 
        UINT32 xsetbv       : 1;
        UINT32 rdpru        : 1;
        UINT32 efer         : 1;    // Intercept writes of EFER (occurs after guest instruction finishes) 
        UINT32 crnwrite     : 16;   // Intercept writes of CR0-15 (occurs after guest instruction finishes) 
    };
    UINT32 value;
};

union INTERCEPT_VECTOR_5
{
    struct
    {
        UINT32 invlpgb          : 1;    // Intercept all INVLPGB instructions
        UINT32 invlpgb_illegal  : 1;    // Intercept only illegally specified INVLPGB instructions
        UINT32 invpcid          : 1;
        UINT32 mcommit          : 1;
        UINT32 tlbsync          : 1;    // Intercept TLBSYNC instruction. Presence of this bit is indicated by CPUID Fn8000_000A, EDX[24] = 1. 
        UINT32 buslockzero      : 1;    // Intercept bus lock operations when Bus Lock Threshold Counter is 0 (occurs before guest instruction executes).
        UINT32 hlt              : 1;    // Intercept HLT instruction if a virtual interrupt is not pending.
        UINT32 reserved         : 25;
    };
    UINT32 value;
};

// 15.16.1 TLB Flush
// "The VMRUN instruction reads, but does not change, thevalue of the TLB_CONTROL field."
// be sure to clear the tlb control field upon the start of the vmexit to restore it
enum TLB_CONTROL
{
    dont_flush       = 0,
    flush_entire_tlb = 1,   // flushes all entries AND all ASID-s, its better to use the one below this
    flush_tlb        = 3,   // flushes only tlb entries only in guest
    flush_tlb_local  = 7,   // flushes only non global tlb entries only in guest
};

union VIRTUAL_CONTROL
{
    struct
    {
        UINT64 v_tpr            : 8;  // The virtual TPR for the guest. Bits 3:0 are used for a 4 - bit virtual TPR value; bits 7:4 are SBZ
        UINT64 v_irq            : 1;  // If nonzero, virtual INTR is pending  This field is ignored on VMRUN when AVIC is enabled.
        UINT64 vgif_state       : 1;  // 15.33.2 Virtual GIF (VGIF) value (0 – Virtual interrupts are masked, 1 – Virtual Interrupts are unmasked)
        UINT64 v_nmi            : 1;  // 15.21.10 NMI Virtualization - If nonzero, virtual NMI is pending
        UINT64 v_nmi_mask       : 1;  // 15.21.10 NMI Virtualization - if nonzero, virtual NMI is masked
        UINT64 reserved1        : 3;
        UINT64 v_intr_prio      : 4;  // Priority for virtual interrupt This field is ignored on VMRUN when AVIC is enabled.
        UINT64 v_ign_tr         : 1;  // If nonzero, the current virtual interrupt ignores the(virtual) TPR This field is ignored on VMRUN when AVIC is enabled.
        UINT64 reserved2        : 3;
        UINT64 v_intr_masking   : 1;  // Virtualize masking of INTR interrupts
        UINT64 v_gif_enable     : 1;  // 15.33.2 Virtual GIF (VGIF) - Virtual GIF enable for this guest (0 - Disabled, 1 - Enabled) 
        UINT64 v_nmi_enable     : 1;  // 15.21.10 NMI Virtualization - NMI Virtualization Enable
        UINT64 reserved3        : 3;
        UINT64 x2avic_enable    : 1;  // 15.29.10 x2AVIC 
        UINT64 avic_enable      : 1;
        UINT64 v_intr_vector    : 8;
        UINT64 reserved4        : 24;
    };
    UINT64 value;
};

// 15.15 VMCB State Caching
// VMCB state caching allows the processor to cache certain guest register values in hardware between a
// #VMEXIT and subsequent VMRUN instructions and use the cached values to improve context - switch
// performance.
// 
// For forward compatibility, if the hypervisor has
// not modified the VMCB, the hypervisor may write FFFF_FFFFh to the VMCB Clean Field to indicate
// that it has not changed any VMCB contents other than the fields described below as explicitly
// uncached.
union VMCB_CLEAN
{
    struct
    {
        UINT64 i    : 1;    // all the intercept vectors, TSC offset, Pause Filte Count
        UINT64 iopm : 1;    // IOMSRPM: IOPM_BASE, MSRPM_BASE 
        UINT64 asid : 1;
        UINT64 tpr  : 1;    // V_TPR, V_IRQ, V_INTR_PRIO, V_IGN_TPR, V_INTR_MASKING, V_INTR_VECTOR (Offset 60h–67h)
        UINT64 np   : 1;    // Nested Paging: NCR3, G_PAT 
        UINT64 crx  : 1;    // CR0, CR3, CR4, EFER 
        UINT64 drx  : 1;    // DR6, DR7 
        UINT64 dt   : 1;    // GDT/IDT Limit and Base 
        UINT64 seg  : 1;    // CS/DS/SS/ES Sel/Base/Limit/Attr, CPL 
        UINT64 cr2  : 1;     
        UINT64 lbr  : 1;    // DebugCtl MSR, br_from/to, lastint_from/to 
        UINT64 avic : 1;    // AVIC APIC_BAR; AVIC APIC_BACKING_PAGE, AVIC PHYSICAL_TABLE and AVIC LOGICAL_TABLE Pointers 
        UINT64 cet  : 1;    // S_CET, SSP, ISST_ADDR 

        UINT64 reserved : 19;
        UINT64 unused   : 32;
    };
    UINT64 value;

    __forceinline void reset()
    {
        value = ULONG_MAX;
        return;
    }
};

struct CURRENT_INSTRUCTION
{
    UINT8 bytes_amt;
    UINT8 instruction_bytes[15];
};

// 15.20 Event Injection
// "The encoding matches that of the EXITINTINFO field."
// we will use this union for exit int info field as well
union EVENT_INJECTION
{
    struct
    {
        UINT64 vector       : 8;    // VECTOR—Bits 7:0. The 8-bit IDT vector of the interrupt or exception. If TYPE is 2 (NMI), the VECTOR field is ignored.
        UINT64 type         : 3;    // TYPE—Bits 10:8. Qualifies the guest exception or interrupt to generate
        UINT64 ev           : 1;    // EV (Error Code Valid)—Bit 11. Set to 1 if the exception should push an error code onto the stack
        UINT64 reserved     : 19;
        UINT64 valid        : 1;    // V (Valid)—Bit 31. Set to 1 if an event is to be injected into the guest; clear to 0 otherwise.
        UINT64 error_code   : 32;
    };
    UINT64 value;
};

// these are used for hardening the hv, mostly security features
union HARDENING_CONTROL
{
    struct
    {
        UINT64 np_enable        : 1;    // Enable nested paging.
        UINT64 sev_enable       : 1;    // Enable Secure Encrypted Virtualization
        UINT64 sev_es_enable    : 1;    // Enable Encrypted State for Secure Encrypted Virtualization
        UINT64 get_enable       : 1;    // Enable Guest Mode Execute Trap 
        UINT64 sss_check_enable : 1;    // Enable supervisor shadow stack restrictions in nested page tables.
        UINT64 vt_encrypt       : 1;    // Enable Virtual Transparent Encryption.
        UINT64 ro_gpt           : 1;    // Enable Read Only Guest Page Tables. See “Nested Table Walk” on page 550
        UINT64 invlpgb_tlbsync  : 1;    // Enable INVLPGB/TLBSYNC.
        UINT64 reserved         : 56;
    };
    UINT64 value;
};

union VIRTUAL_CONTROL_2
{
    struct
    {
        UINT64 lbr_virt             : 1;    // LBR Virtualization Enable
        UINT64 vmsave_vmload_virt   : 1;    // VMSAVE/VMLOAD Virtualization Enable
        UINT64 ibs_virt             : 1;    // IBS Virtualization Enable
        UINT64 pmc_virt             : 1;    // PMC Virtualization Enable
        UINT64 reserved             : 60;
    };
    UINT64 value;
};

struct x86segment {
    SEGMENT_SELECTOR    Selector;  
    SEGMENT_ATTRIBUTE   Attrib;       
    UINT32              Limit;
    UINT64              Base;      

    __forceinline void setup(unsigned short seg, SEGMENT_ATTRIBUTE attribute) {

        Limit           = __segmentlimit(seg);
        Selector.AsUInt = seg;
        Attrib          = attribute;

        // in x64 mode, the base of these arent needed
        Base = 0;
         
        return;
    }

};

struct x64segment {
    char    pad[4];
    UINT32  limit;     
    UINT64  base;      

    __forceinline void setup(SEGMENT_DESCRIPTOR_REGISTER_64& SDR) {

        limit   = SDR.Limit;
        base    = SDR.BaseAddress;
    
        return;
    }
};

struct fullsegment {
    SEGMENT_SELECTOR    selector; 
    SEGMENT_ATTRIBUTE   attribute;   
    UINT32              limit;      
    UINT64              base;       

    __forceinline void setup(unsigned short seg, SEGMENT_ATTRIBUTE attributein, UINT64 basein) {

        limit           = __segmentlimit(seg);
        selector.AsUInt = seg;
        attribute       = attributein;
        base            = basein;

        return;
    }
};

// Table B-1. VMCB Layout, Control Area
struct VMCB_CONTROL_AREA
{
    INTERCEPT_VECTOR_0 intercept_cr;            // 0x0
    INTERCEPT_VECTOR_1 intercept_dr;            // 0x4
    INTERCEPT_VECTOR_2 intercept_exceptions;    // 0x8
    INTERCEPT_VECTOR_3 intercept_instructions1; // 0xc
    INTERCEPT_VECTOR_4 intercept_instructions2; // 0x10
    INTERCEPT_VECTOR_5 intercept_instructions3; // 0x14
    UINT8  reserved1[0x24];                     // 0x18
    UINT16 pause_filter_threshold;              // 0x3c
    UINT16 pause_filter_count;                  // 0x3e
    UINT64 iopm_base_phys;                      // 0x40
    UINT64 msrpm_base_phys;                     // 0x48
    UINT64 tsc_offset;                          // 0x50
    UINT32 guest_asid;                          // 0x58
    UINT32 tlb_ctl;                             // 0x5c
    VIRTUAL_CONTROL     v_ctl;                  // 0x60
    UINT64 interrupt_shadow;                    // 0x68
    UINT64 exit_code;                           // 0x70
    UINT64 exit_info1;                          // 0x78
    UINT64 exit_info2;                          // 0x80
    EVENT_INJECTION     exit_int_info;          // 0x88
    HARDENING_CONTROL   security_ctl;           // 0x90 
    UINT64              avic_apic_bar;          // 0x98
    UINT64              ghcb_guest_phys;        // 0xa0
    EVENT_INJECTION     event_inject;           // 0xa8
    UINT64              ncr3;                   // 0xb0
    VIRTUAL_CONTROL_2   v_ctl2;                 // 0xb8 
    VMCB_CLEAN          vmcb_clean;             // 0xc0
    UINT64              nrip;                   // 0xc8
    CURRENT_INSTRUCTION cur_instr;              // 0xd0
    UINT64 avic_apic_backing_page;              // 0xe0
    UINT8  reserved2[0x8];                      // 0xe8
    UINT64 avic_logical_table;                  // 0xf0
    UINT64 avic_physical_table;                 // 0xf8
    UINT8  reserved3[0x8];                      // 0x100
    UINT64 vmsa_ptr;                            // 0x108
    UINT64 vmgexit_rax;                         // 0x110
    UINT64 vmgexit_cpl;                         // 0x118
    UINT64 bus_lock_threshold_counter;          // 0x120
    UINT8  reserved4[0xC];                      // 0x128 
    UINT32 update_irr;                          // 0x134
    UINT64 sev_features;                        // 0x138
    UINT64 guest_sev_features;                  // 0x140
    UINT8  reserved5[0x8];                      // 0x148
    UINT8  requested_irr[0x20];                 // 0x150
    UINT8  reserved6[0x400 - 0x170];            // 0x170
};

// Table B-2. VMCB Layout, State Save Area 
struct VMCB_STATE_SAVE_AREA
{
    x86segment  es;                     // 0x0
    x86segment  cs;                     // 0x10
    x86segment  ss;                     // 0x20
    x86segment  ds;                     // 0x30
    fullsegment fs;                     // 0x40
    fullsegment gs;                     // 0x50
    x64segment  gdtr;                   // 0x60
    fullsegment ldtr;                   // 0x70
    x64segment  idtr;                   // 0x80
    fullsegment tr;                     // 0x90
    UINT8       reserved1[0x2B];        // 0xa0
    UINT8       cpl;                    // 0xcb
    UINT32      reserved2;              // 0xcc
    AMD_EFER_MSR efer;                  // 0xd0
    UINT8       reserved3[0x70];        // 0xd8
    CR4         cr4;                    // 0x148
    CR3         cr3;                    // 0x150
    CR0         cr0;                    // 0x158
    DR7         dr7;                    // 0x160
    DR6         dr6;                    // 0x168
    RFLAGS      rflags;                 // 0x170
    UINT64      rip;                    // 0x178
    UINT8       reserved4[0x58];        // 0x180
    UINT64      rsp;                    // 0x1d8
    UINT8       reserved5[0x18];        // 0x1e0
    UINT64      rax;                    // 0x1f8
    UINT64      star;                   // 0x200
    UINT64      lstar;                  // 0x208
    UINT64      cstar;                  // 0x210
    UINT64      sfmask;                 // 0x218
    UINT64      kernel_gs_base;         // 0x220
    UINT64      sysenter_cs;            // 0x228
    UINT64      sysenter_esp;           // 0x230
    UINT64      sysenter_eip;           // 0x238
    UINT64      cr2;                    // 0x240
    UINT8       reserved6[0x20];        // 0x248
    UINT64      g_pat;                  // 0x268
    IA32_DEBUGCTL_REGISTER debug_ctl;   // 0x270
    UINT64      br_from;                // 0x278
    UINT64      br_to;                  // 0x280
    UINT64      last_excp_from;         // 0x288
    UINT64      last_excp_to;           // 0x290
    UINT8       reserved7[0x530];       // 0x298


    __forceinline void setup_x86_segment(x86segment* seg, UINT16 segment_selector) 
    {
        seg->setup(segment_selector, get_segment_attributes(segment_selector));
        return;
    }

    // we have a custom baseaddress option for this incase you want to give TR segment a custom baseaddress 
    // (useful for type 2 hypervisors to store vcpu in there or other crutial memory pointers)
    __forceinline void setup_full_segment(fullsegment* seg, UINT16 segment_selector, UINT64 base = 0) 
    {
        seg->setup(segment_selector, get_segment_attributes(segment_selector), base ? base : get_segment_base(segment_selector) );
        return;
    }

    __forceinline void setup_x64_segment(x64segment* seg, SEGMENT_DESCRIPTOR_REGISTER_64& SDR) 
    {
        seg->setup(SDR);
        return;
    }

    __forceinline SEGMENT_ATTRIBUTE get_segment_attributes(UINT16 segment_selector)
    {
        SEGMENT_SELECTOR selector{ .AsUInt = segment_selector };

        SEGMENT_DESCRIPTOR_64 SegDescriptor = *reinterpret_cast<SEGMENT_DESCRIPTOR_64*>(gdtr.base + static_cast<UINT64>(selector.Index) * 8);

        SEGMENT_ATTRIBUTE attribute{ .value = 0 };
        attribute.type        = SegDescriptor.Type;
        attribute.system      = SegDescriptor.DescriptorType;
        attribute.dpl         = SegDescriptor.DescriptorPrivilegeLevel;
        attribute.present     = SegDescriptor.Present;
        attribute.avl         = SegDescriptor.System;
        attribute.long_mode   = SegDescriptor.LongMode;
        attribute.default_bit = SegDescriptor.DefaultBig;
        attribute.granularity = SegDescriptor.Granularity;
        attribute.reserved1   = 0;

        return attribute;
    }

    UINT64 get_segment_base(UINT16 segment_selector)
    {
        SEGMENT_SELECTOR selector{ .AsUInt = segment_selector };

        if (selector.Index == 0) 
            return 0;

        ULONG_PTR Selector = (ULONG_PTR)segment_selector;
        SEGMENT_DESCRIPTOR_64* SegDescriptor = reinterpret_cast<SEGMENT_DESCRIPTOR_64*>(gdtr.base + (Selector >> 3) * 8);

        uintptr_t Base = SegDescriptor->BaseAddressLow | SegDescriptor->BaseAddressMiddle << 16 | SegDescriptor->BaseAddressHigh << 24;

        if (!(SegDescriptor->DescriptorType))
            Base |= (UINT64(SegDescriptor->BaseAddressUpper) << 32);

        return Base;
    }
};

// must be PAGE_SIZE aligned
__declspec(align(PAGE_SIZE)) struct VMCB
{
    // AMD64 Manual Volume 2: Appendix B VMCB Layout

    VMCB_CONTROL_AREA control_area;
    VMCB_STATE_SAVE_AREA state_save_area;

    VMCB_CONTROL_AREA& get_control_area()
    {
        return control_area;
    }

    VMCB_STATE_SAVE_AREA& get_state_save_area()
    {
        return state_save_area;
    }

};

// AMD64 Manual Volume 2: Appendix C SVM Intercept Exit Codes
enum SVMEXIT : UINT64
{
    INVALID              = -1,
    BUSY                 = -2,
    VMEXIT_IDLE_REQUIRED = -3,
    VMEXIT_INVALID_PMC   = -4,

    UNUSED = 0xF000000,

    CR0_READ = 0x00,
    CR2_READ = 0x02,
    CR3_READ = 0x03,
    CR4_READ = 0x04,
    CR8_READ = 0x08,

    CR0_WRITE = 0x10,
    CR2_WRITE = 0x12,
    CR3_WRITE = 0x13,
    CR4_WRITE = 0x14,
    CR8_WRITE = 0x18,

    DR0_READ = 0x20,
    DR1_READ = 0x21,
    DR2_READ = 0x22,
    DR3_READ = 0x23,
    DR4_READ = 0x24,  // may be alias to dr6, CR4.DE == 1
    DR5_READ = 0x25,  // may be alias to dr7
    DR6_READ = 0x26,
    DR7_READ = 0x27,


    DR0_WRITE = 0x30,
    DR1_WRITE = 0x31,
    DR2_WRITE = 0x32,
    DR3_WRITE = 0x33,
    DR7_WRITE = 0x37,

    // faults
    DE = 0x40,
    DB = 0x41,
    BP = 0x43,
    UD = 0x46,
    GP = 0x4D,
    PF = 0x4E,

    // interrupts
    INTR    = 0x60, // Physical INTR (maskable interrupt).
    NMI     = 0x61,
    SMI     = 0x62,
    INIT    = 0x63,
    VINTR   = 0x64,

    //cr0 fault?
    CR0_SEL_WRITE = 0x65, // Write of CR0 changed bits other than CR0.TS or CR0.MP

    // instruction traps
    IDTR_READ   = 0x66,
    GDTR_READ   = 0x67,
    LDTR_READ   = 0x68,
    TR_READ     = 0x69,

    IDTR_WRITE  = 0x6A,
    GDTR_WRITE  = 0x6B,
    LDTR_WRITE  = 0x6C,
    TR_WRITE    = 0x6D,

    // instructions
    RDTSC   = 0x6E,
    RDPMC   = 0x6F,
    PUSHF   = 0x70,
    POPF    = 0x71,
    CPUID   = 0x72,
    RSM     = 0x73,
    IRET    = 0x74,
    SWINT   = 0x75,
    INVD    = 0x76,
    PAUSE   = 0x77,
    HLT     = 0x78,
    INVLPG  = 0x79,
    INVLPGA = 0x7A,
    IOIO    = 0x7B,
    MSR     = 0x7C,

    // interrupts
    TASK_SWITCH = 0x7D,
    FERR_FREEZE = 0x7E,
    SHUTDOWN    = 0x7F,

    // AMD instructions
    VMRUN   = 0x80,
    VMMCALL = 0x81,
    VMLOAD  = 0x82,
    VMSAVE  = 0x83,
    STGI    = 0x84,
    CLGI    = 0x85,
    SKINIT  = 0x86,

    // instructions
    RDTSCP  = 0x87,
    ICEBP   = 0x88,
    WBINVD  = 0x89,
    MONITOR = 0x8A,
    MWAIT   = 0x8B,

    // AMD instructions
    MWAIT_CONDITIONAL   = 0x8C,
    RDPRU               = 0x8D,

    // instructions
    XSETBV = 0x8E,

    // write trap
    EFER_WRITE_TRAP = 0x8F,

    CR0_WRITE_TRAP  = 0x90,
    CR2_WRITE_TRAP  = 0x92,
    CR3_WRITE_TRAP  = 0x93,
    CR4_WRITE_TRAP  = 0x94,
    CR8_WRITE_TRAP  = 0x98,

    // instructions
    INVLPGB  = 0xA0,
    INVLPGB_ILLEGAL = 0xA1,
    INVPCID  = 0xA2,
    MCOMMIT  = 0xA3,
    TLBSYNC  = 0xA4,
    BUSLOCK  = 0xA5,
    IDLE_HLT = 0xA6,

    NPF                 = 0x400,
    AVIC_INCOMPLETE_IPI = 0x401,
    AVIC_NOACCEL        = 0x402,
    VMGEXIT             = 0x403,
};