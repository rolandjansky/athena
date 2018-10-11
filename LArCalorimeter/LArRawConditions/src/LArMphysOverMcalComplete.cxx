/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMphysOverMcalComplete.h" 


LArMphysOverMcalComplete::LArMphysOverMcalComplete()  {}

LArMphysOverMcalComplete::~LArMphysOverMcalComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArMphysOverMcalComplete::set(const HWIdentifier& chid, int gain, float MphysOverMcal ) {
   this->setPdata(chid,MphysOverMcal,gain);
}
