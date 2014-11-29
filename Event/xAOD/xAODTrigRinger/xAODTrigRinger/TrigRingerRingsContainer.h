// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRings.h 563319 2014-10-17 joaoVictor $
#ifndef XAODTRIGRINGER_TRIGRINGERRINGSCONTAINER_H
#define XAODTRIGRINGER_TRIGRINGERRINGSCONTAINER_H

// Local include(s):
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the RingerRingsContainer class
   typedef TrigRingerRingsContainer_v1 TrigRingerRingsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigRingerRingsContainer, 1302123242, 1 )

#endif // XAODTRIGRINGER_TRIGRINGERRINGSCONTAINER_H
