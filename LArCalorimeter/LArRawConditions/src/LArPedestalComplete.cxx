/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArPedestalComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"

#include <iostream> 
using namespace std ;

LArPedestalComplete::LArPedestalComplete()  {}

LArPedestalComplete::~LArPedestalComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArPedestalComplete::set(const HWIdentifier& CellID, const int gain,
			      const float Pedestal, const float PedestalRMS ) {
  
  LArPedestalP1 t(Pedestal,PedestalRMS); 
  this->setPdata(CellID,t,gain); 
}
