//  -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_TRIGGERTYPES_H
#define NSWL1_TRIGGERTYPES_H
#include <memory>
#include <stdint.h>
#include<vector>
//S.I 20.08.18
//Merged tdr_typedefs.h and This file(TriggerTypes.h) inside the common namespace NSWL1
//to avoid namespace inflation.

namespace NSWL1 {
    /** @defgroup TriggerTypes
    *  Define a few types to be used in the NSWL1 trigger simulation
    */
    
    class PadData;
    class PadOfflineData;
    class PadTrigger;
    class SingleWedgePadTrigger;
    class SectorTriggerCandidate;
    
    class StripData;
    class StripClusterData;
    class StripOfflineData;
}
#endif
