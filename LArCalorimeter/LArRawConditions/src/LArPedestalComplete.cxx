/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArPedestalComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"

#include <iostream> 
using namespace std ;

LArPedestalComplete::LArPedestalComplete()  
{

}

LArPedestalComplete::~LArPedestalComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArPedestalComplete::set(const HWIdentifier& CellID, const int gain,
			      const float Pedestal, const float PedestalRMS ) {
  
  LArPedestalP1 t(Pedestal,PedestalRMS); 

  this->setPdata(CellID,t,gain); 
}

float LArPedestalComplete::pedestal(const Identifier&  CellID, int gain) const {

  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID);
  const LArPedestalP1& t = get(OnId, gain);
  /* 
     Alternative: 
     Access LArConditionsContainer via offline hash. That causes the underlying LArConditionsContainer to
     cache a hash-indexed vector of pointers to the payload object

     const IdentifierHash hash = offlineHelper()->calo_cell_hash(CellID);
     const LArPedestalP1& t = get(hash, gain);
     return ( t.m_vPedestal);    
  */
  return (t.m_Pedestal);
}

float LArPedestalComplete::pedestalRMS(const Identifier&  CellID, int gain) const
{

  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID);
  const LArPedestalP1& t = get(OnId, gain);
  /* 
     Alternative: 
     Access LArConditionsContainer via offline hash. That causes the underlying LArConditionsContainer to
     cache a hash-indexed vector of pointers to the payload object

     const IdentifierHash hash = offlineHelper()->calo_cell_hash(CellID);
     const LArPedestalP1& t = get(hash, gain);
     return ( t.m_vPedestalRMS);    
  */
  return (t.m_PedestalRMS);
}



  

