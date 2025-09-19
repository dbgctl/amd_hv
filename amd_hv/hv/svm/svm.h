#pragma once

#include "ia32.h"
#include "svm_structures.h"
#include "../asm/asm.h"

namespace svm 
{
    bool check_cpu_support();

    void enable_svm();
};
