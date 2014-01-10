/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "MDT_Digitization/MdtDigiToolInput.h"
#include "MDT_Digitization/MDT_Response_DigiTool.h"


#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

#include <iostream>

using namespace MuonGM;
using namespace std;
 
MDT_Response_DigiTool::MDT_Response_DigiTool( const std::string& type, 
					      const std::string& name, 
					      const IInterface* parent ) 
  : AlgTool(type,name,parent) 
	 , m_rndmEngine(0)
	 , m_rndmEngineName("MuonDigitization")
	 , m_rndmSvc("AtRndmGenSvc", name )
{
  declareInterface<IMDT_DigitizationTool>(this);

  declareProperty("ClusterDensity",   m_clusterDensity = 8.5);
  declareProperty("Threshold",        m_threshold = 20.);
  declareProperty("AttenuationLength",m_attenuationLength = 16000);
  declareProperty("RndmSvc", 			  m_rndmSvc, "Random Number Service used in Muon digitization" );
  declareProperty("RndmEngine",       m_rndmEngineName,       "Random engine name");
  declareProperty("DoQballGamma",     m_DoQballGamma = false);

}

MdtDigiToolOutput 
MDT_Response_DigiTool::digitize(const MdtDigiToolInput& input)
{

  m_tube.SetSegment(input.radius(),input.positionAlongWire());
  MsgStream log(msgSvc(),name());
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "Digitizing input " << endreq;
  if(m_DoQballGamma==true){
	double ParticleCharge=input.electriccharge(); 
	double ParticleGamma=input.gamma();
	if(ParticleGamma>0.){

		if(m_tube.GetSignal(ParticleCharge,ParticleGamma)){
			MdtDigiToolOutput output(true, m_tube.DriftTime(), m_tube.AdcResponse());
			return output;
		}
	}else{
		if(m_tube.GetSignal()){
			MdtDigiToolOutput output(true, m_tube.DriftTime(), m_tube.AdcResponse());
			return output;
		}	
	}
	
  }else{
	if(m_tube.GetSignal()){
	
		MdtDigiToolOutput output(true, m_tube.DriftTime(), m_tube.AdcResponse());
		return output;	
	}
  }
  MdtDigiToolOutput output(false,0.,0.);
  return output;
}
 
StatusCode MDT_Response_DigiTool::initialize()
{
  MsgStream log(msgSvc(),name());

  StoreGateSvc* detStore=0;
  StatusCode status = serviceLocator()->service("DetectorStore", detStore);

  if (status.isSuccess()) {
    if(detStore->contains<MuonDetectorManager>( "Muon" )){
      status = detStore->retrieve(m_muonGeoMgr);
      if (status.isFailure()) {
	log << MSG::FATAL << "Could not retrieve MuonGeoModelDetectorManager!" << endreq;
	return status;
      }
      else {
	if (log.level()<=MSG::DEBUG) log << MSG::DEBUG
	    << "MuonGeoModelDetectorManager retrieved from StoreGate."<< endreq;
	//initialize the MdtIdHelper
	m_idHelper = m_muonGeoMgr->mdtIdHelper();
	if (log.level()<=MSG::DEBUG) log << MSG::DEBUG
	    << "MdtIdHelper: " << m_idHelper << endreq;
	if(!m_idHelper) return status;
      }
    }
  }

    if (!m_rndmSvc.retrieve().isSuccess())
      {
			log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
      }      
    
    // getting our random numbers stream
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "Getting random number engine : <" << m_rndmEngineName << ">" << endreq;
    m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
    if (m_rndmEngine==0) {
    log << MSG::ERROR << "Could not find RndmEngine : " << m_rndmEngineName << endreq;
    return StatusCode::FAILURE;
    }

//  IAtRndmGenSvc* p_AtRndmGenSvc = 0;

//  static const bool CREATEIFNOTTHERE(true);
//  StatusCode RndmStatus = service("AtRndmGenSvc", p_AtRndmGenSvc, CREATEIFNOTTHERE);
//  if (!RndmStatus.isSuccess() || 0 == p_AtRndmGenSvc)
//    {
//      log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
//      return RndmStatus;
//    }      
  
//  // getting our random numbers stream
//  p_engine  =       p_AtRndmGenSvc->GetEngine("MDTRESPONSE");
  
  initializeTube();

  return StatusCode::SUCCESS;
}

bool MDT_Response_DigiTool::initializeTube(){
  
  // initialize MDT_Response
  double innerR(m_muonGeoMgr->getGenericMdtDescriptor()->innerRadius);
    
  MsgStream log(msgSvc(),name());
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG 
      << "INITIALIZED Inner tube radius to " << innerR
      << endreq;
  m_tube.SetTubeRadius(innerR);
  m_tube.SetClusterDensity(m_clusterDensity);
  m_tube.SetAttLength(m_attenuationLength);
  m_tube.SetTriggerElectron(m_threshold);
  m_tube.SetRandomEngine(m_rndmEngine, false);
  return true;
}
