// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoI.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_ENERGYSUMROI_H
#define XAODTRIGGER_ENERGYSUMROI_H

// Local include(s):
#include "xAODTrigger/versions/EnergySumRoI_v1.h"

namespace xAOD{
   typedef EnergySumRoI_v1 EnergySumRoI;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EnergySumRoI, 61465022, 1 )

#endif // XAODTRIGGER_ENERGYSUMROI_H
