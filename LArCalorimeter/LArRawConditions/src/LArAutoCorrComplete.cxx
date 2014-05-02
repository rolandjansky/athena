/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArAutoCorrComplete.h" 


LArAutoCorrComplete::LArAutoCorrComplete():LArConditionsContainer<LArAutoCorrP1>() 
{

}

LArAutoCorrComplete::~LArAutoCorrComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArAutoCorrComplete::set(const HWIdentifier& CellID, int gain,
			 const std::vector<float>& vAutoCorr ) {

//  std::cout<<" cell id "<< CellID<<" gain "<< gain<<" size "<<
//   vAutoCorr.size()<<std::endl;

  LArAutoCorrP1 t;
  t.m_vAutoCorr.reserve(vAutoCorr.size());
  for (unsigned int i = 0; i < vAutoCorr.size(); ++i) {
      t.m_vAutoCorr.push_back(vAutoCorr[i]);
  }
  setPdata(CellID, t, (unsigned int)gain); 

}

/* retrieve AutoCorr ******************************************************
 */
ILArAutoCorr::AutoCorrRef_t LArAutoCorrComplete::autoCorr(const Identifier&  CellID, int gain) const 
{ 
  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID);
  const LArAutoCorrP1& t = get(OnId,gain) ;
  return t.m_vAutoCorr;
}
  

ILArAutoCorr::AutoCorrRef_t LArAutoCorrComplete::autoCorr(const HWIdentifier&  CellID, int gain) const 
{ 
  const LArAutoCorrP1& t = get(CellID,gain) ;
  return t.m_vAutoCorr;
}



  

