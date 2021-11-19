/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/src/arm_helpers.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2021
 * @brief Some helpers specific to aarch64.
 */


#if defined(__aarch64__) && defined(__linux)


#include "arm_helpers.h"
#include "CxxUtils/SealDebug.h"
#include <iterator>
#include <unistd.h>
#include <stdio.h>


namespace {


// ESR decoding logic adapted from the linux kernel ---
//   arch/arm64/kernel/traps.c
//   arch/arm64/mm/fault.c
//   arch/arm64/include/asm/esr.h

static constexpr uint32_t ESR_ELx_EC_SHIFT = 26;
static constexpr uint32_t ESR_ELx_EC_MASK = 0x3Ful << ESR_ELx_EC_SHIFT;
inline uint32_t ESR_ELx_EC (uint32_t esr) {
  return (esr & ESR_ELx_EC_MASK) >> ESR_ELx_EC_SHIFT;
}

static constexpr uint32_t ESR_ELx_IL_SHIFT = 25;
static constexpr uint32_t ESR_ELx_IL = 1ul << ESR_ELx_IL_SHIFT;
static constexpr uint32_t ESR_ELx_ISS_MASK = ESR_ELx_IL - 1;

// Shared ISS field definitions for Data/Instruction aborts.
static constexpr uint32_t ESR_ELx_SET_SHIFT   = 11;
static constexpr uint32_t ESR_ELx_SET_MASK    = 3ul << ESR_ELx_SET_SHIFT;
static constexpr uint32_t ESR_ELx_FnV_SHIFT   = 10;
static constexpr uint32_t ESR_ELx_FnV         = 1ul << ESR_ELx_FnV_SHIFT;
static constexpr uint32_t ESR_ELx_EA_SHIFT    = 9;
static constexpr uint32_t ESR_ELx_EA          = 1ul << ESR_ELx_EA_SHIFT;
static constexpr uint32_t ESR_ELx_S1PTW_SHIFT = 7;
static constexpr uint32_t ESR_ELx_S1PTW       = 1ul << ESR_ELx_S1PTW_SHIFT;

// ISS field definitions for Data Aborts.
static constexpr uint32_t ESR_ELx_ISV_SHIFT = 24;
static constexpr uint32_t ESR_ELx_ISV       = 1ul << ESR_ELx_ISV_SHIFT;
static constexpr uint32_t ESR_ELx_SAS_SHIFT = 22;
static constexpr uint32_t ESR_ELx_SAS       = 3ul << ESR_ELx_SAS_SHIFT;
static constexpr uint32_t ESR_ELx_SSE_SHIFT = 21;
static constexpr uint32_t ESR_ELx_SSE       = 1ul << ESR_ELx_SSE_SHIFT;
static constexpr uint32_t ESR_ELx_SRT_SHIFT = 16;
static constexpr uint32_t ESR_ELx_SRT_MASK  = 0x1Ful << ESR_ELx_SRT_SHIFT;
static constexpr uint32_t ESR_ELx_SF_SHIFT  = 15;
static constexpr uint32_t ESR_ELx_SF        = 1u << ESR_ELx_SF_SHIFT;
static constexpr uint32_t ESR_ELx_AR_SHIFT  = 14;
static constexpr uint32_t ESR_ELx_AR        = 1u << ESR_ELx_AR_SHIFT;
static constexpr uint32_t ESR_ELx_CM_SHIFT  = 8;
static constexpr uint32_t ESR_ELx_CM        = 1u << ESR_ELx_CM_SHIFT;

// ISS field definitions shared by different classes.
static constexpr uint32_t ESR_ELx_WNR_SHIFT = 6;
static constexpr uint32_t ESR_ELx_WNR       = 1u << ESR_ELx_WNR_SHIFT;


static const char*  const esr_class_str[] =
{
 "Unknown/Uncategorized",      // 0x00: ESR_ELx_EC_UNKNOWN
 "WFI/WFE",                    // 0x01: ESR_ELx_EC_WFx
 "UNRECOGNIZED EC",            // 0x02
 "CP15 MCR/MRC",               // 0x03: SR_ELx_EC_CP15_32
 "CP15 MCRR/MRRC",             // 0x04: ESR_ELx_EC_CP15_64
 "CP14 MCR/MRC",               // 0x05: ESR_ELx_EC_CP14_MR
 "CP14 LDC/STC",               // 0x06: ESR_ELx_EC_CP14_LS
 "ASIMD",                      // 0x07: ESR_ELx_EC_FP_ASIMD
 "CP10 MRC/VMRS",              // 0x08: ESR_ELx_EC_CP10_ID
 "UNRECOGNIZED EC",            // 0x09
 "UNRECOGNIZED EC",            // 0x0a
 "UNRECOGNIZED EC",            // 0x0b
 "CP14 MCRR/MRRC",             // 0x0c: ESR_ELx_EC_CP14_64
 "UNRECOGNIZED EC",            // 0x0d
 "PSTATE.IL",                  // 0x0e: ESR_ELx_EC_ILL
 "UNRECOGNIZED EC",            // 0x0f
 "UNRECOGNIZED EC",            // 0x10
 "SVC (AArch32)",              // 0x11: ESR_ELx_EC_SVC32
 "HVC (AArch32)",              // 0x12: ESR_ELx_EC_HVC32
 "SMC (AArch32)",              // 0x13: ESR_ELx_EC_SMC32
 "UNRECOGNIZED EC",            // 0x14
 "SVC (AArch64)",              // 0x15: ESR_ELx_EC_SVC64
 "HVC (AArch64)",              // 0x16: ESR_ELx_EC_HVC64
 "SMC (AArch64)",              // 0x17: ESR_ELx_EC_SMC64
 "MSR/MRS (AArch64)",          // 0x18: ESR_ELx_EC_SYS64
 "SVE",                        // 0x19: ESR_ELx_EC_SVE
 "UNRECOGNIZED EC",            // 0x1a
 "UNRECOGNIZED EC",            // 0x1b
 "UNRECOGNIZED EC",            // 0x1c
 "UNRECOGNIZED EC",            // 0x1d
 "UNRECOGNIZED EC",            // 0x1e
 "EL3 IMP DEF",                // 0x1f: ESR_ELx_EC_IMP_DEF
 "IABT (lower EL)",            // 0x20: ESR_ELx_EC_IABT_LOW
 "IABT (current EL)",          // 0x21: ESR_ELx_EC_IABT_CUR
 "PC Alignment",               // 0x22: ESR_ELx_EC_PC_ALIGN
 "UNRECOGNIZED EC",            // 0x23
 "DABT (lower EL)",            // 0x24: ESR_ELx_EC_DABT_LOW
 "DABT (current EL)",          // 0x25: ESR_ELx_EC_DABT_CUR
 "SP Alignment",               // 0x26: ESR_ELx_EC_SP_ALIGN
 "UNRECOGNIZED EC",            // 0x27
 "FP (AArch32)",               // 0x28: ESR_ELx_EC_FP_EXC32
 "UNRECOGNIZED EC",            // 0x29
 "UNRECOGNIZED EC",            // 0x2a
 "UNRECOGNIZED EC",            // 0x2b
 "FP (AArch64)",               // 0x2c: ESR_ELx_EC_FP_EXC64
 "UNRECOGNIZED EC",            // 0x2d
 "UNRECOGNIZED EC",            // 0x2e
 "SError",                     // 0x2f: ESR_ELx_EC_SERROR
 "Breakpoint (lower EL)",      // 0x30: ESR_ELx_EC_BREAKPT_LOW
 "Breakpoint (current EL)",    // 0x31: ESR_ELx_EC_BREAKPT_CUR
 "Software Step (lower EL)",   // 0x32: ESR_ELx_EC_SOFTSTP_LOW
 "Software Step (current EL)", // 0x33: ESR_ELx_EC_SOFTSTP_CUR
 "Watchpoint (lower EL)",      // 0x34: ESR_ELx_EC_WATCHPT_LOW
 "Watchpoint (current EL)",    // 0x35: ESR_ELx_EC_WATCHPT_CUR
 "UNRECOGNIZED EC",            // 0x36
 "UNRECOGNIZED EC",            // 0x37
 "BKPT (AArch32)",             // 0x38: ESR_ELx_EC_BKPT32
 "UNRECOGNIZED EC",            // 0x39
 "Vector catch (AArch32)",     // 0x3a: ESR_ELx_EC_VECTOR32
 "UNRECOGNIZED EC",            // 0x3b
 "BRK (AArch64)",              // 0x3c: ESR_ELx_EC_BRK64
};

static constexpr uint32_t ESR_ELx_EC_DABT_LOW = 0x24;
static constexpr uint32_t ESR_ELx_EC_DABT_CUR = 0x25;



const char* esr_get_class_string (uint32_t esr)
{
  uint32_t code = ESR_ELx_EC(esr);
  if (code >= std::size (esr_class_str))
    return esr_class_str[2]; // UNRECOGNIZED EC
  return esr_class_str[code];
}


inline bool esr_is_data_abort(uint32_t esr)
{
  const uint32_t ec = ESR_ELx_EC(esr);
  return ec == ESR_ELx_EC_DABT_LOW || ec == ESR_ELx_EC_DABT_CUR;
}


void esr_data_abort_decode (IOFD fd,
                            char* buf, unsigned int buf_size,
                            uint32_t esr)
{
  if (esr & ESR_ELx_ISV) {
    MYWRITE (fd, buf, snprintf (buf, buf_size,
                                "\n  Access size = %u byte(s)",
                                1u << ((esr & ESR_ELx_SAS) >> ESR_ELx_SAS_SHIFT)));
    MYWRITE (fd, buf, snprintf (buf, buf_size,
                                "\n  SSE = %u, SRT = %u",
                                (esr & ESR_ELx_SSE) >> ESR_ELx_SSE_SHIFT,
                                (esr & ESR_ELx_SRT_MASK) >> ESR_ELx_SRT_SHIFT));
    MYWRITE (fd, buf, snprintf (buf, buf_size,
                                "\n  SF = %u, AR = %u",
                                (esr & ESR_ELx_SF) >> ESR_ELx_SF_SHIFT,
                                (esr & ESR_ELx_AR) >> ESR_ELx_AR_SHIFT));
  } else {
    MYWRITE (fd, buf, snprintf (buf, buf_size,
                                "\n  ISV = 0, ISS = 0x%08x",
                                esr & ESR_ELx_ISS_MASK));
  }

  MYWRITE (fd, buf, snprintf (buf, buf_size,
                              "\n  CM = %u, WnR = %u",
                              (esr & ESR_ELx_CM) >> ESR_ELx_CM_SHIFT,
                              (esr & ESR_ELx_WNR) >> ESR_ELx_WNR_SHIFT));
}


} // anonymous namespace


namespace CxxUtils {


void aarch64_dump_fpsimd (IOFD fd,
                          char* buf, unsigned int buf_size,
                          const fpsimd_context& ctx)
{
  MYWRITE (fd, buf, snprintf (buf, buf_size,
                              "\n"
                              "\n  fpsr: %08x                 fpcr:  %08x",
                              ctx.fpsr, ctx.fpcr));
  for (int i = 0; i < 32; ++i) {
    union {
      __uint128_t u128;
      uint32_t  u32[4];
    } c;
    c.u128 = ctx.vregs[i];
    MYWRITE (fd, buf, snprintf (buf, buf_size,
                                "\n  v%d:%s [%08x %08x %08x %08x]",
                                i,
                                i < 10 ? " " : "",
                                c.u32[0], c.u32[1], c.u32[2], c.u32[3]));
  }
}


void aarch64_dump_esr (IOFD fd,
                       char* buf, unsigned int buf_size,
                       const esr_context& ctx)
{
  MYWRITE (fd, buf, snprintf (buf, buf_size,
                              "\n\n  Mem abort info --- ESR:  %016llx",
                              ctx.esr));
  MYWRITE (fd, buf, snprintf (buf, buf_size,
                              "\n  Exception class = %s, IL = %u bits",
                              esr_get_class_string (ctx.esr),
                              (ctx.esr & ESR_ELx_IL) ? 32 : 16));
  MYWRITE (fd, buf, snprintf (buf, buf_size,
                              "\n  SET = %llu, FnV = %llu",
                              (ctx.esr & ESR_ELx_SET_MASK) >> ESR_ELx_SET_SHIFT,
                              (ctx.esr & ESR_ELx_FnV) >> ESR_ELx_FnV_SHIFT));
  MYWRITE (fd, buf, snprintf (buf, buf_size,
                              "\n  EA = %llu, S1PTW = %llu",
                              (ctx.esr & ESR_ELx_EA) >> ESR_ELx_EA_SHIFT,
                              (ctx.esr & ESR_ELx_S1PTW) >> ESR_ELx_S1PTW_SHIFT));

  if (esr_is_data_abort (ctx.esr)) {
    esr_data_abort_decode (fd, buf, buf_size, ctx.esr);
  }
}


/**
 * @brief Dump out aarch64 registers.
 * @param fd File descriptor to which to write.
 * @param buf Temporary buffer for string formatting.
 * @param buf_size Size of buf.
 * @param mc HW context structure.
 */
void aarch64_dump_registers (IOFD fd,
                             char* buf, unsigned int buf_size,
                             const mcontext_t& mc)
{
  MYWRITE (fd, buf, snprintf (buf, buf_size,
                              "\n  pc:  %016llx          pstate: %016llx",
                              mc.pc, mc.pstate));
  for (int i = 0; i < 30; i += 2) {
    MYWRITE (fd, buf, snprintf (buf, buf_size,
                                "\n  x%d:%s %016llx          x%d:%s %016llx",
                                i, i < 10 ? " " : "", mc.regs[i],
                                i+1, i+1  < 10 ? " " : "", mc.regs[i+1]));
  }
  MYWRITE (fd, buf, snprintf (buf, buf_size,
                              "\n  x30: %016llx          sp:  %016llx",
                              mc.regs[30], mc.sp));

  int ipos = 0;
  while (ipos + sizeof(_aarch64_ctx) <= sizeof (mc.__reserved)) {
    const _aarch64_ctx* ctx = reinterpret_cast<const _aarch64_ctx*> (&mc.__reserved[ipos]);
    if (ctx->magic == 0 || ipos + ctx->size > sizeof (mc.__reserved)) {
      break;
    }
    if (ctx->magic == FPSIMD_MAGIC) {
      aarch64_dump_fpsimd (fd, buf, buf_size, *reinterpret_cast<const fpsimd_context*>(ctx));
    }
    else if (ctx->magic == ESR_MAGIC) {
      aarch64_dump_esr (fd, buf, buf_size, *reinterpret_cast<const esr_context*>(ctx));
    }
    else if (ctx->magic == EXTRA_MAGIC) {
      MYWRITE (fd, buf, snprintf (buf, buf_size, "\n\n[extra dump not implemented]\n"));
    }
    else if (ctx->magic == SVE_MAGIC) {
      MYWRITE (fd, buf, snprintf (buf, buf_size, "\n\n[SVE dump not implemented]\n"));
    }
    ipos += ctx->size;
  }
}


} // namespace CxxUtils


#endif // __aarch64__ && __linux
