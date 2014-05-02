/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LAruA2MeVComplete.h" 

LAruA2MeVComplete::LAruA2MeVComplete()  {}
LAruA2MeVComplete::~LAruA2MeVComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LAruA2MeVComplete::set(const HWIdentifier& chid, const float uA2MeV ) {
  this->setPdata(chid,uA2MeV); 
}

/* retrieve uA2MeV ******************************************************
 */
const float& LAruA2MeVComplete::UA2MEV(const HWIdentifier& chid) const { 
  return this->get(chid).m_data; 
}

const float& LAruA2MeVComplete::UA2MEV(const Identifier& id) const{
  const HWIdentifier chid = larCablingSvc()->createSignalChannelID(id); 
  return UA2MEV(chid);
}


  
