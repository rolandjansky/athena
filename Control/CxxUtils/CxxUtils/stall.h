// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file CxxUtils/stall.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief Emit stall instruction for use in a spin loop.
 */


#ifndef CXXUTILS_STALL_H
#define CXXUTILS_STALL_H


namespace CxxUtils {


/**
 * @brief Emit stall instruction for use in a spin loop.
 *
 * For best performance of a spin loop (as used for a spinlock or other
 * MT consistency operations), Intel recommends adding a `pause' instruction
 * within the loop on x86(_64) targets.  This function provides a portable
 * way of doing this.
 *
 * Currently a no-op for anything other than x86(_64).  Support for other
 * architectures can be added if it is useful.
 */
inline
void stall()
{
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
  __builtin_ia32_pause();
#endif
}


} // namespace CxxUtils


#endif // not CXXUTILS_STALL_H
