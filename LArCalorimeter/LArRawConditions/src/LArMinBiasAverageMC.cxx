/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "LArRawConditions/LArMinBiasAverageMC.h" 
#include "LArElecCalib/ILArMCSymTool.h"

#include <iostream> 
using namespace std ;

float LArMinBiasAverageMC::dummy = ERRORCODE;

LArMinBiasAverageMC::LArMinBiasAverageMC()  :LArMinBiasAverageComplete(),m_larmcsym("LArMCSymTool")
{}

StatusCode LArMinBiasAverageMC::initialize() 
{
  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LArMinBiasAverageMC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endreq; 
      return (StatusCode::FAILURE);
  }

   return (CONTAINER::initialize()) ;
}

LArMinBiasAverageMC::~LArMinBiasAverageMC() {}


/* retrieve Average of E in Minimum Bias events ******************************
 */
const float& LArMinBiasAverageMC::minBiasAverage(const HWIdentifier& CellID) const 
{ 

  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  
  return LArMinBiasAverageComplete::minBiasAverage(SymCellID); 

}

const float& LArMinBiasAverageMC::minBiasAverage(const Identifier&  CellID) const
{
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);

  return LArMinBiasAverageComplete::minBiasAverage(SymCellID);


}
