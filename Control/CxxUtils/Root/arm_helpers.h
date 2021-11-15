// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/src/arm_helpers.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2021
 * @brief Some helpers specific to aarch64.
 */


#ifndef CXXUTILS_ARM_HELPERS_H
#define CXXUTILS_ARM_HELPERS_H


#if defined(__aarch64__) && defined(__linux)


#include "CxxUtils/SealCommon.h"
#if HAVE_POSIX_SIGNALS
#include <ucontext.h>
#endif


namespace CxxUtils {


/**
 * @brief Dump out aarch64 registers.
 * @param fd File descriptor to which to write.
 * @param buf Temporary buffer for string formatting.
 * @param buf_size Size of buf.
 * @param mc HW context structure.
 */
void aarch64_dump_registers (IOFD fd,
                             char* buf, unsigned int buf_size,
                             const mcontext_t& mc);


} // namespace CxxUtils


#endif // __aarch64__ && __linux


#endif // not CXXUTILS_ARM_HELPERS_H
