/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

#include "MDT_Digitization/MdtDigiToolInput.h"
#include "MDT_Digitization/MDT_Response_DigiTool.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <iostream>

using namespace MuonGM;
using namespace std;

MDT_Response_DigiTool::MDT_Response_DigiTool( const std::string& type,
					      const std::string& name,
					      const IInterface* parent )
  : AthAlgTool(type,name,parent)
	 , m_muonGeoMgr(0)
	 , m_idHelper(0)
{
  declareInterface<IMDT_DigitizationTool>(this);

  declareProperty("ClusterDensity",   m_clusterDensity = 8.5);
  declareProperty("Threshold",        m_threshold = 20.);
  declareProperty("AttenuationLength",m_attenuationLength = 16000);
  declareProperty("DoQballGamma",     m_DoQballGamma = false);

}

MdtDigiToolOutput
MDT_Response_DigiTool::digitize(const MdtDigiToolInput& input, CLHEP::HepRandomEngine *rndmEngine)
{

  m_tube.SetSegment(input.radius(),input.positionAlongWire());
  ATH_MSG_DEBUG("Digitizing input ");
  if(m_DoQballGamma==true){
	double ParticleCharge=input.electriccharge();
	double ParticleGamma=input.gamma();
	if(ParticleGamma>0.){

          if(m_tube.GetSignal(ParticleCharge,ParticleGamma, rndmEngine)){
			MdtDigiToolOutput output(true, m_tube.DriftTime(), m_tube.AdcResponse());
			return output;
		}
	}else{
		if(m_tube.GetSignal(rndmEngine)){
			MdtDigiToolOutput output(true, m_tube.DriftTime(), m_tube.AdcResponse());
			return output;
		}
	}

  }else{
	if(m_tube.GetSignal(rndmEngine)){

		MdtDigiToolOutput output(true, m_tube.DriftTime(), m_tube.AdcResponse());
		return output;
	}
  }
  MdtDigiToolOutput output(false,0.,0.);
  return output;
}

StatusCode MDT_Response_DigiTool::initialize()
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

bool MDT_Response_DigiTool::initializeTube(){

  // initialize MDT_Response
  double innerR(m_muonGeoMgr->getGenericMdtDescriptor()->innerRadius);

  ATH_MSG_DEBUG("INITIALIZED Inner tube radius to " << innerR );
  m_tube.SetTubeRadius(innerR);
  m_tube.SetClusterDensity(m_clusterDensity);
  m_tube.SetAttLength(m_attenuationLength);
  m_tube.SetTriggerElectron(m_threshold);
  return true;
}
