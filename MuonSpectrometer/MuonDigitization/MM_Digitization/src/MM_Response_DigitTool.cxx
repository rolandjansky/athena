/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "MM_Digitization/MmDigitToolInput.h"
#include "MM_Digitization/MM_Response_DigitTool.h"


#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

#include <iostream>
#include <vector>

using namespace MuonGM;
using namespace std;
/*******************************************************************************/ 
MM_Response_DigitTool::MM_Response_DigitTool( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : AlgTool(type,name,parent)
  , m_rndmEngine(0)
  , m_rndmEngineName("MuonDigitization")
  , m_rndmSvc("AtRndmGenSvc", name )
{
  declareInterface<IMM_DigitizationTool>(this); 	
  declareProperty("RndmSvc",    m_rndmSvc,         "Random Number Service used in Muon digitization" );
  declareProperty("RndmEngine", m_rndmEngineName,  "Random engine name");
}
/*******************************************************************************/
MmDigitToolOutput MM_Response_DigitTool::digitize( /*const MmDigitToolInput& input*/ )
{	
  vector<float> a, b;
  vector<int> c;  
  MmDigitToolOutput output(false, c, b, a, 1, 1);
  return output;
}
/*******************************************************************************/ 
StatusCode MM_Response_DigitTool::initialize()
{
  MsgStream log(msgSvc(),name());

  StoreGateSvc* detStore=0;
  StatusCode status = serviceLocator()->service("DetectorStore", detStore);

  if (status.isSuccess()) {
    if(detStore->contains<MuonDetectorManager>( "Muon" )){
      status = detStore->retrieve(m_muonGeoMgr);
      if (status.isFailure()) {
        if (log.level()<=MSG::FATAL) log << MSG::FATAL << "Could not retrieve MuonGeoModelDetectorManager!" << endreq;
        return status;
      }
      else {
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "MuonGeoModelDetectorManager retrieved from StoreGate."<< endreq;
        //initialize the MdtIdHelper
        m_idHelper = m_muonGeoMgr->mmIdHelper();
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "MdtIdHelper: " << m_idHelper << endreq;
        if(!m_idHelper) return status;
      }
    }
  }

  if (!m_rndmSvc.retrieve().isSuccess())
    {
      if (log.level()<=MSG::FATAL) log << MSG::FATAL << " Could not initialize Random Number Service" << endreq;
      return StatusCode::FAILURE;
    }     
	   
  // getting our random numbers stream
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "Getting random number engine : <" << m_rndmEngineName << ">" << endreq;
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    if (log.level()<=MSG::FATAL) log << MSG::FATAL << "Could not find RndmEngine : " << m_rndmEngineName << endreq;
    return StatusCode::FAILURE;
  }
	
  
  initializeStrip();
	
  return StatusCode::SUCCESS;
}
/*******************************************************************************/	
bool MM_Response_DigitTool::initializeStrip(){	  
  return true;
}
/*******************************************************************************/
 
