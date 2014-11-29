// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRings.h 563319 2014-10-17 joaoVictor $
#ifndef XAODTRIGRINGER_TRIGRINGERRINGS_H
#define XAODTRIGRINGER_TRIGRINGERRINGS_H

// Local include(s):
#include "xAODTrigRinger/versions/TrigRingerRings_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the RingerRings class
   typedef TrigRingerRings_v1 TrigRingerRings;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigRingerRings,70014748, 1 )

#endif // XAODTIRGRINGER_TRIGRINGERRINGS_H
