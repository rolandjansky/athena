/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "LArRawConditions/LAruA2MeVMC.h" 
#include "LArElecCalib/ILArMCSymTool.h"

#include <iostream> 
using namespace std ;

LAruA2MeVMC::LAruA2MeVMC() : LAruA2MeVComplete(), m_larmcsym("LArMCSymTool")
{ }

StatusCode LAruA2MeVMC::initialize()
{
  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LAruA2MeVMC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endmsg; 
      return (StatusCode::FAILURE);
  }


   return (CONTAINER::initialize()); 
}

LAruA2MeVMC::~LAruA2MeVMC() {}


/* retrieve uA2MeV ******************************************************
 */
const float& LAruA2MeVMC::UA2MEV(const HWIdentifier& CellID) const 
{ 
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  
  return LAruA2MeVComplete::UA2MEV(SymCellID) ; 
}

const float& LAruA2MeVMC::UA2MEV(const Identifier&  CellID) const
{
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  
  return LAruA2MeVComplete::UA2MEV(SymCellID) ; 

 
}
