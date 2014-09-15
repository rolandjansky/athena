/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyBFTool.h"

//----------------------------------------------------------------//
#include "MboyCoreEvt/MuonboyBFAbstract.h"
#include "MboyEDMTool/MuonboyBFAth.h"

MboyBFTool::MboyBFTool(const std::string& t, 
                       const std::string& n,
                       const IInterface*  p ):
  AthAlgTool(t,n,p),
  m_fieldServiceHandle("AtlasFieldSvc",n)
{

  declareInterface<IMboyBFTool>(this);

  declareProperty("MagFieldSvc"        , m_fieldServiceHandle);
  
  p_MuonboyBFAth = 0 ;
  m_UpdatedToolStamp  =-1 ;
  
}

MboyBFTool::~MboyBFTool(){
 delete p_MuonboyBFAth;
}

// Initialize
StatusCode MboyBFTool::initialize(){

  
//Create BF
  p_MuonboyBFAth = new MuonboyBFAth;

  ATH_CHECK(m_fieldServiceHandle.retrieve());

  return StatusCode::SUCCESS;

}

StatusCode MboyBFTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyBFTool::GetMuonboyBFAbstract(
               MuonboyBFAbstract*& pMuonboyBFAbstract
){

  p_MuonboyBFAth->m_fieldService = &*m_fieldServiceHandle;
  p_MuonboyBFAth->SetReady() ;

  pMuonboyBFAbstract = p_MuonboyBFAth ;

  return StatusCode::SUCCESS;

}

StatusCode MboyBFTool::UpdatedTool(IOVSVC_CALLBACK_ARGS)
{
  return StatusCode::SUCCESS;
}

int MboyBFTool::UpdatedToolStamp()  { return m_UpdatedToolStamp  ; }
int MboyBFTool::getToroidBarrelOn() { return m_fieldServiceHandle->toroidOn() ; }
int MboyBFTool::getToroidECTAOn()   { return m_fieldServiceHandle->toroidOn()   ; }
int MboyBFTool::getToroidECTCOn()   { return m_fieldServiceHandle->toroidOn()   ; }
int MboyBFTool::getSolenoidOn()     { return m_fieldServiceHandle->solenoidOn()     ; }
int MboyBFTool::getAllToroidOn()    { return m_fieldServiceHandle->toroidOn()    ; }
int MboyBFTool::getFieldStatusOn()  { return m_fieldServiceHandle->solenoidOn() && m_fieldServiceHandle->toroidOn()   ; }
