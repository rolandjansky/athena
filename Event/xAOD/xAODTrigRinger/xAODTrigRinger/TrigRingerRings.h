// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_TRIGRINGERRINGS_H
#define XAODTRIGRINGER_TRIGRINGERRINGS_H

// Local include(s):
#include "xAODTrigRinger/versions/TrigRingerRings_v2.h"
//#include "xAODTrigRinger/versions/TrigRingerRings_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the TrigRingerRings class
   typedef TrigRingerRings_v2 TrigRingerRings;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigRingerRings,70014748, 1 )

#endif // XAODTRIGRINGER_TRIGRINGERRINGS_H
