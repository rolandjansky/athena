/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENT_TRIGT2ZDCSIGNALSCONTAINER_H 
#define TRIGCALOEVENT_TRIGT2ZDCSIGNALSCONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TrigCaloEvent/TrigT2ZdcSignals.h"
#include "AthenaKernel/BaseInfo.h"

class TrigT2ZdcSignalsContainer : public DataVector<TrigT2ZdcSignals> { }; 

CLASS_DEF( TrigT2ZdcSignalsContainer , 1093332728 , 1 )

SG_BASE(TrigT2ZdcSignalsContainer, DataVector<TrigT2ZdcSignals>);

#endif
