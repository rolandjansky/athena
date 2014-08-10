/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENT_TRIGT2MBTSBITSCONTAINER_H 
#define TRIGCALOEVENT_TRIGT2MBTSBITSCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "TrigCaloEvent/TrigT2MbtsBits.h"
#include "SGTools/BaseInfo.h"

class TrigT2MbtsBitsContainer : public DataVector<TrigT2MbtsBits> { }; 

CLASS_DEF( TrigT2MbtsBitsContainer , 1184644728 , 1 )

SG_BASE(TrigT2MbtsBitsContainer, DataVector<TrigT2MbtsBits>);

#endif
