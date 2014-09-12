// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/ClassID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Standalone helpers.
 */


#ifndef ATHCONTAINERS_CLASSID_H
#define ATHCONTAINERS_CLASSID_H


#ifdef XAOD_STANDALONE

typedef unsigned int CLID;
static const CLID CLID_NULL = 0;
namespace SG {
template <class T> CLID safe_clid() { return 0; }
}


#else


#include "GaudiKernel/ClassID.h"
#include "SGTools/safe_clid.h"


#endif


#endif // not ATHCONTAINERS_CLASSID_H
