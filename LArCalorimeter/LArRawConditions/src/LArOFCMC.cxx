/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArOFCMC.h" 
#include "AthenaKernel/getMessageSvc.h"
#include "LArElecCalib/ILArMCSymTool.h"

#include <iostream> 
using namespace std ;

LArOFCMC::LArOFCMC() : LArOFCComplete() , m_larmcsym("LArMCSym")
{
} 
StatusCode LArOFCMC::initialize()  
{
  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LArOFCMC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endreq; 
      return (StatusCode::FAILURE);
  }

  return (LArOFCComplete::initialize()) ; 
}

LArOFCMC::~LArOFCMC() {}

/* retrieve OFC ******************************************************
 */

LArOFCMC::OFCRef_t
LArOFCMC::OFC_a(const HWIdentifier&  CellID,int gain,int tbin) const
{
//   if(!m_larmcsym) initialize(); 
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);

  return LArOFCComplete::OFC_a(SymCellID,gain,tbin);

}


  

LArOFCMC::OFCRef_t
LArOFCMC::OFC_b(const HWIdentifier&  CellID,int gain,int tbin) const
{
//   if(!m_larmcsym) initialize(); 
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);

  return LArOFCComplete::OFC_b(SymCellID,gain,tbin);

}


LArOFCMC::OFCRef_t
LArOFCMC::OFC_a(const Identifier&  CellID, int gain, int tbin) const
{
  return LArOFCComplete::OFC_a (CellID, gain, tbin);
}


LArOFCMC::OFCRef_t
LArOFCMC::OFC_b(const Identifier&  CellID, int gain, int tbin) const 
{
  return LArOFCComplete::OFC_b (CellID, gain, tbin);
}
