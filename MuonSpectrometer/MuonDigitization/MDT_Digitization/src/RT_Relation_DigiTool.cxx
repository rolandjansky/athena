/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "MDT_Digitization/MdtDigiToolInput.h"

#include "MDT_Digitization/RT_Relation_DigiTool.h"

#include "PathResolver/PathResolver.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"


#include <iostream>

using namespace MuonGM;
using namespace std;

#define RT_DATA "ArCO2.rt"

RT_Relation_DigiTool::RT_Relation_DigiTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent ) 
  : AlgTool(type,name,parent) 
{
  declareInterface<IMDT_DigitizationTool>(this);

  declareProperty("EffectiveRadius",  m_effRadius = 14.4275);

}

MdtDigiToolOutput RT_Relation_DigiTool::digitize(const MdtDigiToolInput& input)
{

  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Digitizing input " << endreq;

  if( isTubeEfficient( input.radius() ) ){
    MdtDigiToolOutput output(true,getDriftTime( input.radius() ), getAdcResponse() );
    return output;
  }
  MdtDigiToolOutput output(false,0.,0.);

  return output;

  
}
 
StatusCode RT_Relation_DigiTool::initialize()
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
	log << MSG::DEBUG
	    << "MuonGeoModelDetectorManager retrieved from StoreGate."<< endreq;
	//initialize the MdtIdHelper
	m_idHelper = m_muonGeoMgr->mdtIdHelper();
	log << MSG::DEBUG
	    << "MdtIdHelper: " << m_idHelper << endreq;
	if(!m_idHelper) return status;
      }
    }
  }

  initializeTube();

  return StatusCode::SUCCESS;
}

bool RT_Relation_DigiTool::initializeTube(){
  
  m_maxRadius = m_muonGeoMgr->getGenericMdtDescriptor()->innerRadius;
  
  MsgStream log(msgSvc(),name());
  if (rt.size() < 1) {
    std::string inputFile = RT_DATA;
    // Find the full path to inputFile:
    std::string file = PathResolver::find_file (inputFile, "DATAPATH");
    log << MSG::DEBUG << "Found file " << file << endreq;
    rt.push_back (new Rt_relation);
    std::ifstream rt_file(file.c_str());
    rt[0]->set_name(file.c_str());
    if (!rt_file.good()) {
      log << MSG::FATAL <<
	      "Could not open RT relation file " 
	      << rt[0]->name() << endreq;
      return StatusCode::FAILURE;
    }
    else {
      log << MSG::DEBUG << "RT relation file "<<
	      rt[0]->name() << " opened" << endreq;
    }
    rt[0]->read_rt(rt_file);
  }

  m_maxRadius = m_muonGeoMgr->getGenericMdtDescriptor()->innerRadius;
  
  log << MSG::DEBUG 
      << "Initialized Inner tube radius to " << m_maxRadius
      << endreq;
    
	return true;	 
}

