/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "LArRawConditions/LArDAC2uAMC.h"
#include "LArElecCalib/ILArMCSymTool.h"

#include <iostream> 
using namespace std ;

float LArDAC2uAMC::dummy = ERRORCODE;

LArDAC2uAMC::LArDAC2uAMC() :LArDAC2uAComplete(), m_larmcsym("LArMCSymTool")
{

}

StatusCode LArDAC2uAMC::initialize ()  
{

  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LArDAC2uAMC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endmsg; 
      return (StatusCode::FAILURE);
  }

   return (CONTAINER::initialize()) ;
}

LArDAC2uAMC::~LArDAC2uAMC() {}


/* retrieve DAC2uA ******************************************************
 */
const float& LArDAC2uAMC::DAC2UA(const HWIdentifier& CellID) const 
{ 
  //if(!m_larmcsym) initialize(); 
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  
  return LArDAC2uAComplete::DAC2UA(SymCellID); 
}

const float& LArDAC2uAMC::DAC2UA(const Identifier&  CellID) const
{
//  if(!m_larmcsym) initialize(); 
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);

  return LArDAC2uAComplete::DAC2UA(SymCellID); 

}
