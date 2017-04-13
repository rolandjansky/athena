// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/CurrentEventStore.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Wrapper for CurrentEventStore, to do the correct thing
 *        for both Athena and standalone builds.
 */


#ifndef ATHCONTAINERS_CURRENTEVENTSTORE_H
#define ATHCONTAINERS_CURRENTEVENTSTORE_H


#ifdef XAOD_STANDALONE
#include "xAODRootAccessInterfaces/TVirtualEvent.h"
#include "xAODRootAccessInterfaces/TActiveEvent.h"

typedef xAOD::TVirtualEvent IProxyDict;
namespace SG {
class CurrentEventStore
{
public:
  static IProxyDict* store()
  {
    return xAOD::TActiveEvent::event();
  }
};
} // namespace SG
#else
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/IProxyDict.h"
#endif


#endif // not ATHCONTAINERS_CURRENTEVENTSTORE_H
