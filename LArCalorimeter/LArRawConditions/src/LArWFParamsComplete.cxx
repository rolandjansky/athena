/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArWFParamsComplete.h" 


LArWFParamsComplete::LArWFParamsComplete()  
{

}

LArWFParamsComplete::~LArWFParamsComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArWFParamsComplete::set(const HWIdentifier& CellID, int gain, const LArWFParams& theParams) {
  setPdata(CellID, theParams, gain);
}

/* retrieve WF params using online ID ****************************
 */
const LArWFParams& LArWFParamsComplete::getParams(const HWIdentifier& CellID, int gain) const { 
  return get(CellID,gain); 
}
