// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/likely.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Define likely/unlikely macros for branch prediction.
 *
 * Duplicated here for standalone builds.
 */


#ifndef ATHCONTAINERS_LIKELY_H
#define ATHCONTAINERS_LIKELY_H


#if __GNUC__ >= 4 || defined(__clang__)
# define ATHCONTAINERS_LIKELY(x)       __builtin_expect(!!(x), 1)
# define ATHCONTAINERS_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
# define ATHCONTAINERS_LIKELY(x)   (x)
# define ATHCONTAINERS_UNLIKELY(x) (x)
#endif


#endif // not ATHCONTAINERS_LIKELY_H
