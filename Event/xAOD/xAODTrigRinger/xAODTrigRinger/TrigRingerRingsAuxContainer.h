// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_TRIGRINGERRINGSAUXCONTAINER_H
#define XAODTRIGRINGER_TRIGRINGERRINGSAUXCONTAINER_H

// Local include(s):
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v2.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the TrigRingerRings class
   typedef TrigRingerRingsAuxContainer_v2 TrigRingerRingsAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigRingerRingsAuxContainer , 1085112341 , 1 )

#endif // XAODTRIGRINGER_TRIGRINGERRINGSAUXCONTAINER_H
