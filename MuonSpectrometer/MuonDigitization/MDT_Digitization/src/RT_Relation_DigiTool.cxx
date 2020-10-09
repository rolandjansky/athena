/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_Digitization/MdtDigiToolInput.h"
#include "MDT_Digitization/RT_Relation_DigiTool.h"
#include "PathResolver/PathResolver.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <iostream>

using namespace MuonGM;

#define RT_DATA "ArCO2.rt"

RT_Relation_DigiTool::RT_Relation_DigiTool( const std::string& type,
					    const std::string& name,
					    const IInterface* parent )
  : AthAlgTool(type,name,parent)
  , m_maxRadius(0)
  , m_idHelper(0)
{
  declareInterface<IMDT_DigitizationTool>(this);
  declareProperty("EffectiveRadius",  m_effRadius = 14.4275);
}

MdtDigiToolOutput RT_Relation_DigiTool::digitize(const MdtDigiToolInput& input,CLHEP::HepRandomEngine *rndmEngine)
{

  ATH_MSG_DEBUG("Digitizing input ");

  if( isTubeEfficient( input.radius(), rndmEngine ) ){
    MdtDigiToolOutput output(true,getDriftTime( input.radius() ), getAdcResponse() );
    return output;
  }
  MdtDigiToolOutput output(false,0.,0.);

  return output;

}

StatusCode RT_Relation_DigiTool::initialize()
{
  const MuonGM::MuonDetectorManager* muDetMgr=nullptr;
  if(detStore()->contains<MuonDetectorManager>( "Muon" )){
      ATH_CHECK(detStore()->retrieve(muDetMgr));
      ATH_MSG_DEBUG("MuonGeoModelDetectorManager retrieved from StoreGate.");
      //initialize the MdtIdHelper
      m_idHelper = muDetMgr->mdtIdHelper();
      ATH_MSG_DEBUG("MdtIdHelper: " << m_idHelper );
      if(!m_idHelper) {
        ATH_MSG_ERROR("MdtIdHelper is nullptr");
        return StatusCode::FAILURE;
      }
  }

  initializeTube(muDetMgr);

  return StatusCode::SUCCESS;
}

bool RT_Relation_DigiTool::initializeTube(const MuonGM::MuonDetectorManager* detMgr){

  m_maxRadius = detMgr->getGenericMdtDescriptor()->innerRadius;

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

  ATH_MSG_DEBUG("Initialized Inner tube radius to " << m_maxRadius );

  return true;
}

