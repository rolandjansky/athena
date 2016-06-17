// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoIAuxInfo.h 743093 2016-04-26 21:46:56Z watsona $
#ifndef XAODTRIGGER_ENERGYSUMROIAUXINFO_H
#define XAODTRIGGER_ENERGYSUMROIAUXINFO_H

// Local include(s):
#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v1.h"
#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v2.h"

namespace xAOD{
   /// Declare the most recent version of the energy-sum auxiliary info
   typedef EnergySumRoIAuxInfo_v2 EnergySumRoIAuxInfo;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EnergySumRoIAuxInfo, 263007227, 1 )

#endif // XAODTRIGGER_ENERGYSUMROIAUXINFO_H
