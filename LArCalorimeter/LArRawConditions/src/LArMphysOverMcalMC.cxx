/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "LArRawConditions/LArMphysOverMcalMC.h" 
#include "LArElecCalib/ILArMCSymTool.h"

#include <iostream> 
using namespace std ;

LArMphysOverMcalMC::LArMphysOverMcalMC()  
:LArMphysOverMcalComplete(), m_larmcsym("LArMCSymTool")
{

} 

StatusCode LArMphysOverMcalMC::initialize()
{ 

  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LArPhysOverMcalMC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endreq; 
      return (StatusCode::FAILURE);
  }

   return (CONTAINER::initialize()) ;
}

LArMphysOverMcalMC::~LArMphysOverMcalMC() {}


/* retrieve MphysOverMcal ******************************************************
 */
const float& LArMphysOverMcalMC::MphysOverMcal(const HWIdentifier& CellID, int gain) const 
{ 
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);  
  return LArMphysOverMcalComplete::MphysOverMcal(SymCellID, gain);
}

const float& LArMphysOverMcalMC::MphysOverMcal(const Identifier&  CellID, int gain) const
{
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  return LArMphysOverMcalComplete::MphysOverMcal(SymCellID, gain);

}
