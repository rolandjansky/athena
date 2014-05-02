/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "LArRawConditions/LArMinBiasMC.h" 
#include "LArElecCalib/ILArMCSymTool.h"

#include <iostream> 
using namespace std ;

float LArMinBiasMC::dummy = ERRORCODE;

LArMinBiasMC::LArMinBiasMC()  :LArMinBiasComplete(),m_larmcsym("LArMCSymTool")
{}

StatusCode LArMinBiasMC::initialize() 
{
  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LArMinBiasMC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endreq; 
      return (StatusCode::FAILURE);
  }

   return (CONTAINER::initialize()) ;
}

LArMinBiasMC::~LArMinBiasMC() {}


/* retrieve RMS of E in Minimum Bias events ******************************
 */
const float& LArMinBiasMC::minBiasRMS(const HWIdentifier& CellID) const 
{ 

  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  
  return LArMinBiasComplete::minBiasRMS(SymCellID); 

}

const float& LArMinBiasMC::minBiasRMS(const Identifier&  CellID) const
{
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);

  return LArMinBiasComplete::minBiasRMS(SymCellID);


}
