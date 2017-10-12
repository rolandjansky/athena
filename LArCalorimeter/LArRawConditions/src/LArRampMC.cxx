/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArRampMC.h" 
#include "AthenaKernel/getMessageSvc.h"
#include "LArElecCalib/ILArMCSymTool.h"

#include <iostream> 
using namespace std ;

LArRampMC::LArRampMC() : LArRampComplete() , m_larmcsym("LArMCSymTool")
{
} 
StatusCode LArRampMC::initialize()  
{
  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LArRampMC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endmsg; 
      return (StatusCode::FAILURE);
  }

  return (LArRampCompleteBase::initialize()) ; 
}

LArRampMC::~LArRampMC() {}

/* retrieve Ramp ******************************************************
 */
LArRampMC::RampRef_t LArRampMC::ADC2DAC(const HWIdentifier& CellID,int gain) const 
{ 
//  if(!m_larmcsym) initialize(); 
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);  

  return LArRampComplete::ADC2DAC(SymCellID,gain); 

}

LArRampMC::RampRef_t LArRampMC::ADC2DAC(const Identifier&  CellID,int gain) const
{
//   if(!m_larmcsym) initialize(); 
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);

  return LArRampComplete::ADC2DAC(SymCellID,gain);

}


  

