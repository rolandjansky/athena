// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RootUtils/TBranchElementClang.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Include TBranchElement.h, suppressing clang warnings.
 */


#ifndef ROOTUTILS_TBRANCHELEMENTCLANG_H
#define ROOTUTILS_TBRANCHELEMENTCLANG_H


#if defined(__clang__)
# pragma clang diagnostic push
# if __has_warning("-Wformat-pedantic")
#  pragma clang diagnostic ignored "-Wformat-pedantic"
# endif
#endif
#include "TBranchElement.h"
#if defined(__clang__)
# pragma clang diagnostic pop
#endif


#endif // not ROOTUTILS_TBRANCHELEMENTCLANG_H
