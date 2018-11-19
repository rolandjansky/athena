/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArfSamplComplete.h" 


LArfSamplComplete::LArfSamplComplete()  {}
LArfSamplComplete::~LArfSamplComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArfSamplComplete::set(const HWIdentifier& CellID, float fSampl ) {
  LArfSamplP t; 
  t.m_fSampl = fSampl; 
  setPdata(CellID,t) ;
}

// retrieve fSampl ***************************************************
const float& LArfSamplComplete::FSAMPL(const HWIdentifier& CellID) const { 
  const LArfSamplP& t = get(CellID) ;
  return ( t.m_fSampl );

}

const float& LArfSamplComplete::FSAMPL(const Identifier&) const {
  //Not implemented (on purpose) for the *Complete-class.
  //Only implemeneted in the LArfSamplSym class
  std::abort();
}


  
