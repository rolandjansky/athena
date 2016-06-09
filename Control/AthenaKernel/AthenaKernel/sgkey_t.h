// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaKernel/sgkey_t.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Define the type used for hashed StoreGate key+CLID pairs.
 *
 * These are managed by @c IProxyDict.  This definition was broken
 * out from there due to dependency issues.
 */


#ifndef ATHENAKERNEL_SGKEY_T_H
#define ATHENAKERNEL_SGKEY_T_H


#include <stdint.h>


namespace SG {


/// Type used for hashed StoreGate key+CLID pairs.
typedef uint32_t sgkey_t;


} // namespace SG


#endif // not ATHENAKERNEL_SGKEY_T_H
