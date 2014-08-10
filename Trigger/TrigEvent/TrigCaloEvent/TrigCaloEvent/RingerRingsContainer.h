/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENT_RINGERRINGSCONTAINER_H 
#define TRIGCALOEVENT_RINGERRINGSCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "TrigCaloEvent/RingerRings.h"
#include "SGTools/BaseInfo.h"

class RingerRingsContainer : public DataVector<RingerRings> { 
};

CLASS_DEF( RingerRingsContainer , 1313785721 , 1 )

SG_BASE(RingerRingsContainer, DataVector<RingerRings>);

#endif
