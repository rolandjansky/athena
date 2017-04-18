/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Raw2DigiTool.h"
#include "AFP_RawEv/AFP_RawDataContainer.h"
#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitAuxContainer.h"

AFP_Raw2DigiTool::AFP_Raw2DigiTool(const std::string &type,
				   const std::string &name,
				   const IInterface *parent)
  : AthAlgTool(type, name, parent){
  declareInterface<IAFP_Raw2DigiTool>(this);

  declareProperty( "rawDataContainerName", m_rawDataContainerName = "AFP_RawData");
  declareProperty( "AFPSiHitsContainerName", m_AFPSiHitsContainerName = "AFPSiHitContainer" );
}		

AFP_Raw2DigiTool::~AFP_Raw2DigiTool() {}

StatusCode AFP_Raw2DigiTool::initialize(){
  ATH_MSG_INFO("Initializing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode AFP_Raw2DigiTool::recoSiHits(){
  ATH_MSG_DEBUG("AFP_Raw2DigiTool recoSiHits ");

  // create output containers
  xAOD::AFPSiHitContainer* siHitContainer = new xAOD::AFPSiHitContainer();
  CHECK( evtStore()->record(siHitContainer, m_AFPSiHitsContainerName) );
  xAOD::AFPSiHitAuxContainer* siHitAuxContainer = new xAOD::AFPSiHitAuxContainer();
  CHECK( evtStore()->record(siHitAuxContainer, m_AFPSiHitsContainerName + "Aux.") );
  siHitContainer->setStore(siHitAuxContainer);

  // retrieve raw data
  const AFP_RawDataContainer *container = 0;
  StatusCode sc_read = evtStore()->retrieve(container, m_rawDataContainerName);
  if (sc_read.isFailure()) {
    ATH_MSG_WARNING("AFP_Raw2DigiTool: Could not find raw data container");
    return StatusCode::SUCCESS;
  }
  else
    ATH_MSG_DEBUG("AFP_Raw2DigiTool: Raw data container retrieved");
	
  ATH_MSG_DEBUG("Reading raw data container");
  for (const AFP_RawDataCollection* const collection: *container) {
	  
    for (const AFP_RawData* const data : *collection) {

      const uint32_t hitLink = data->Get_link();
	    
      xAOD::AFPSiHit* xAODSiHit = new xAOD::AFPSiHit();
      ATH_MSG_DEBUG("Dynamically created xAODSiHit object");
      siHitContainer->push_back(xAODSiHit);
	    
      ATH_MSG_DEBUG("Added xAOD::AFPSiHit to a xAOD::AFPSiHitContainer");
	    
      switch (hitLink) {
      case 0:       
	xAODSiHit->setStationID(2);
	xAODSiHit->setPixelLayerID(0);
	break;
      case 1:
	xAODSiHit->setStationID(2);
	xAODSiHit->setPixelLayerID(1);
	break;
      case 2:
	xAODSiHit->setStationID(2);
	xAODSiHit->setPixelLayerID(2);
	break;
      case 3:
	xAODSiHit->setStationID(2);
	xAODSiHit->setPixelLayerID(3);
	break;
      case 8:
	xAODSiHit->setStationID(3);
	xAODSiHit->setPixelLayerID(0);
	break;
      case 9:
	xAODSiHit->setStationID(3);
	xAODSiHit->setPixelLayerID(1);
	break;
      case 10:
	xAODSiHit->setStationID(3);
	xAODSiHit->setPixelLayerID(2);
	break;
      case 11:
	xAODSiHit->setStationID(3);
	xAODSiHit->setPixelLayerID(3);
	break;
      default:
	ATH_MSG_WARNING("Unrecognised value in hitLink = " << hitLink);
	delete xAODSiHit;
	return StatusCode::SUCCESS;
      } // end of switch

      ATH_MSG_DEBUG("Filled information about station and detector in xAODSiHit object");

      xAODSiHit->setPixelColIDChip( data->Get_column() );
      xAODSiHit->setPixelRowIDChip( data->Get_row() );

      const unsigned int ToT_value = data->Get_ToT();
      xAODSiHit->setTimeOverThreshold(ToT_value);
      xAODSiHit->setDepositedCharge(1909 + ToT_value * 363 + ToT_value * ToT_value * 141);

      ATH_MSG_DEBUG("Filled xAOD::AFPSiHit");

    }// end of for over RawData_Collection_Beg using p_RawData_Beg

  } // end of for over container using RawData_Collection_Beg

  return StatusCode::SUCCESS;
}

StatusCode AFP_Raw2DigiTool::finalize(){
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}
