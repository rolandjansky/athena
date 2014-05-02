/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArShape32MC.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"
#include "LArTools/LArMCSymTool.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "AthenaKernel/getMessageSvc.h"

#include "LArRawConditions/LArWaveHelper.h"
#include "LArRawConditions/LArWave.h"

#include "LArElecCalib/ILArMCSymTool.h"


#include <iostream> 
using namespace std ;

LArShape32MC::LArShape32MC() :CONTAINER() ,  m_larmcsym("LArMCSymTool")
{ } 


StatusCode LArShape32MC::initialize()
{
  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LArShape32MC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endreq; 
      return (StatusCode::FAILURE);
  }


  return (CONTAINER::initialize()); 
}

LArShape32MC::~LArShape32MC() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArShape32MC::set(const HWIdentifier& CellID, int gain,
                       const std::vector<float>& vShape,
                       const std::vector<float>& vShapeDer )
{
 
  LArShapeP1 t;
  t.m_vShape    = vShape;
  t.m_vShapeDer = vShapeDer;

  setPdata(CellID, t, gain); 
}

/* retrieve Shape ******************************************************
 */
LArShape32MC::ShapeRef_t
LArShape32MC::Shape(const HWIdentifier& CellID,
                    int gain,
                    int /*tbin*/,
                    int /*mode*/) const 
{ 
  
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  const LArShapeP1& t = get(SymCellID,gain) ; 
  return t.m_vShape;
    
}

LArShape32MC::ShapeRef_t
LArShape32MC::ShapeDer(const HWIdentifier& CellID,
                       int gain,
                       int /*tbin*/,
                       int /*mode*/) const 
{ 
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  const LArShapeP1& t = get(SymCellID,gain) ; 
  return t.m_vShapeDer;
}


LArShape32MC::ShapeRef_t
LArShape32MC::Shape(const Identifier&  CellID,
                    int gain,
                    int /*tbin*/,
                    int /*mode*/) const
{
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  const LArShapeP1& t = get(SymCellID,gain) ; 
  return t.m_vShape;
}

LArShape32MC::ShapeRef_t
LArShape32MC::ShapeDer(const Identifier&  CellID,
                       int gain,
                       int /*tbin*/,
                       int /*mode*/) const
{
  // symmetrize CellID for MC usage
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  const LArShapeP1& t = get(SymCellID,gain) ; 
  return t.m_vShapeDer;
}
