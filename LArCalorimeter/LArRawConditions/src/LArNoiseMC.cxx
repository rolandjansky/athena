/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "LArRawConditions/LArNoiseMC.h"
#include "LArElecCalib/ILArMCSymTool.h"

#include <iostream> 
using namespace std ;


LArNoiseMC::LArNoiseMC()  : LArNoiseComplete(), m_larmcsym("LArMCSymTool")
{} 

StatusCode LArNoiseMC::initialize()
{
  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LArNoiseMC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endreq; 
      return (StatusCode::FAILURE);
  }

   return (CONTAINER::initialize()) ;
}

LArNoiseMC::~LArNoiseMC() {}


/* retrieve Noise ******************************************************
 */
const float& LArNoiseMC::noise(const HWIdentifier& CellID, int gain) const 
{ 
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  
  return LArNoiseComplete::noise(SymCellID,gain); 

}

const float& LArNoiseMC::noise(const Identifier&  CellID, int gain) const
{
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);

  return LArNoiseComplete::noise(SymCellID,gain); 
}



  

