/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENT_RINGERRINGSCONTAINER_H 
#define TRIGCALOEVENT_RINGERRINGSCONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TrigCaloEvent/RingerRings.h"
#include "AthenaKernel/BaseInfo.h"

class RingerRingsContainer : public DataVector<RingerRings> { 
};

CLASS_DEF( RingerRingsContainer , 1313785721 , 1 )

SG_BASE(RingerRingsContainer, DataVector<RingerRings>);

#endif
