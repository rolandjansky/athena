/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "LArRawConditions/LArfSamplMC.h" 
#include "LArElecCalib/ILArMCSymTool.h"

#include <iostream> 
using namespace std ;

float LArfSamplMC::dummy = ERRORCODE;

LArfSamplMC::LArfSamplMC() : LArfSamplComplete(), m_larmcsym("LArMCSymTool") 
{}


StatusCode LArfSamplMC::initialize() 
{
  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LArfSamplMC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endmsg; 
      return (StatusCode::FAILURE);
  }

   return (CONTAINER::initialize());   
}

LArfSamplMC::~LArfSamplMC() {}


/* retrieve fSampl ******************************************************
 */
const float& LArfSamplMC::FSAMPL(const HWIdentifier& CellID) const 
{ 
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  
  return LArfSamplComplete::FSAMPL(SymCellID)  ;
}

const float& LArfSamplMC::FSAMPL(const Identifier&  CellID) const
{

  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  
  return LArfSamplComplete::FSAMPL(SymCellID)  ;

}

