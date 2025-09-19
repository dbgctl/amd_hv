#pragma once

extern "C" 
{
    void __sgdt(void* descriptor);

    unsigned short __str();

    unsigned short __sldt();

    bool start_hv(struct vcpu* vcpu);

    bool send_hv_command(unsigned long long key, unsigned long long command);
}