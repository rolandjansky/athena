/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArDAC2uAComplete.h" 


LArDAC2uAComplete::LArDAC2uAComplete()  {}
LArDAC2uAComplete::~LArDAC2uAComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArDAC2uAComplete::set(const HWIdentifier& chid, const float DAC2uA ) {
    this->setPdata(chid,DAC2uA); 
}

/* retrieve DAC2uA ******************************************************
 */
const float& LArDAC2uAComplete::DAC2UA(const HWIdentifier& chid) const {  
  return this->get(chid).m_data;
}

const float& LArDAC2uAComplete::DAC2UA(const Identifier&  id) const {
  const HWIdentifier chid = larCablingSvc()->createSignalChannelID(id); 
  return DAC2UA(chid);
}


  

