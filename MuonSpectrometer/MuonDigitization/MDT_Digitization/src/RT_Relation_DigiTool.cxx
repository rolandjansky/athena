/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
  : AthAlgTool(type,name,parent)
  , m_maxRadius(0)
  , m_muonGeoMgr(0)
  , m_idHelper(0)
{
  declareInterface<IMDT_DigitizationTool>(this);
  declareProperty("EffectiveRadius",  m_effRadius = 14.4275);
}

MdtDigiToolOutput RT_Relation_DigiTool::digitize(const MdtDigiToolInput& input)
{

  ATH_MSG_DEBUG("Digitizing input ");

  if( isTubeEfficient( input.radius() ) ){
    MdtDigiToolOutput output(true,getDriftTime( input.radius() ), getAdcResponse() );
    return output;
  }
  MdtDigiToolOutput output(false,0.,0.);

  return output;

}

StatusCode RT_Relation_DigiTool::initialize()
{

  StoreGateSvc* detStore=0;
  StatusCode status = serviceLocator()->service("DetectorStore", detStore);

  if (status.isSuccess()) {
    if(detStore->contains<MuonDetectorManager>( "Muon" )){
      status = detStore->retrieve(m_muonGeoMgr);
      if (status.isFailure()) {
	ATH_MSG_FATAL("Could not retrieve MuonGeoModelDetectorManager!");
	return status;
      }
      else {
	ATH_MSG_DEBUG("MuonGeoModelDetectorManager retrieved from StoreGate.");
	//initialize the MdtIdHelper
	m_idHelper = m_muonGeoMgr->mdtIdHelper();
	ATH_MSG_DEBUG("MdtIdHelper: " << m_idHelper );
	if(!m_idHelper) return status;
      }
    }
  }

  initializeTube();

  return StatusCode::SUCCESS;
}

bool RT_Relation_DigiTool::initializeTube(){

  m_maxRadius = m_muonGeoMgr->getGenericMdtDescriptor()->innerRadius;

  if (m_rt.size() < 1) {
    std::string inputFile = RT_DATA;
    // Find the full path to inputFile:
    std::string file = PathResolver::find_file (inputFile, "DATAPATH");
    ATH_MSG_DEBUG("Found file " << file );
    m_rt.push_back (new Rt_relation);
    std::ifstream rt_file(file.c_str());
    m_rt[0]->set_name(file.c_str());
    if (!rt_file.good()) {
      ATH_MSG_FATAL("Could not open RT relation file " << m_rt[0]->name() );
      return false;
    }
    else {
      ATH_MSG_DEBUG("RT relation file " << m_rt[0]->name() << " opened" );
    }
    m_rt[0]->read_rt(rt_file);
  }

  m_maxRadius = m_muonGeoMgr->getGenericMdtDescriptor()->innerRadius;

  ATH_MSG_DEBUG("Initialized Inner tube radius to " << m_maxRadius );

  return true;
}

