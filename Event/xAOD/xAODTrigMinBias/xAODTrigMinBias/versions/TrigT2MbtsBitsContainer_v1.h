/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigT2MbtsBitsCONTAINER_V1_H
#define xAODTrigMinBias_TrigT2MbtsBitsCONTAINER_V1_H

#include "AthContainers/DataVector.h"
#include "xAODTrigMinBias/versions/TrigT2MbtsBits_v1.h"

namespace xAOD {
  typedef DataVector<xAOD::TrigT2MbtsBits_v1> TrigT2MbtsBitsContainer_v1;

}

// Setup a CLID for the container... not sure what this is for.
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigT2MbtsBitsContainer_v1 , 1119932133 , 1 )

#endif

#endif
