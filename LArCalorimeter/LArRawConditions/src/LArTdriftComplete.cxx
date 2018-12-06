/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArTdriftComplete.h" 


LArTdriftComplete::LArTdriftComplete()  {}

LArTdriftComplete::~LArTdriftComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArTdriftComplete::set(const HWIdentifier& CellID, float tdrift ) {

  LArTdriftP t;   
  t.m_Tdrift       = tdrift ;

  setPdata(CellID,t); 

}


/* retrieve Tdrift ******************************************************
 */
const float& LArTdriftComplete::Tdrift(const HWIdentifier& CellID) const 
{ 
  const LArTdriftP& t = get(CellID); 
  return (t.m_Tdrift);
}
