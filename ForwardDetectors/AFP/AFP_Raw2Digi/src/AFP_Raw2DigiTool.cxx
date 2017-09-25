/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Raw2DigiTool.h"
#include "AFP_RawEv/AFP_RawDataContainer.h"
#include "AFP_RawEv/AFP_ROBID.h"
#include "xAODForward/AFPStationID.h"
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
	
  for (const AFP_RawDataCollection* const collection: *container) {
	  
    for (const AFP_RawData* const data : *collection) {
      const uint32_t hitLink = data->Get_link();

      if (hitLink == 12 || hitLink == 13) {
	// fill time-of-flight information
      }
      else {
	// fill tracking information
	xAOD::AFPSiHit* xAODSiHit = new xAOD::AFPSiHit();
	siHitContainer->push_back(xAODSiHit);
	 
	// decoding according to Outlink numbers in: https://twiki.cern.ch/twiki/bin/view/Atlas/AFPRunning#Tracker
	if (hitLink <=3) {	// not checking if larger or equal 0, because this is uint, which is always positive
	  // set near station ID selecting side based on ROB
	  if (collection->Get_robID() == AFP_ROBID::sideA)
	    xAODSiHit->setStationID(xAOD::AFPStationID::nearA);
	  else if (collection->Get_robID() == AFP_ROBID::sideC)
	    xAODSiHit->setStationID(xAOD::AFPStationID::nearC);
	  else  {
	    ATH_MSG_WARNING("Unrecognised robID: in dec="<<std::dec<<collection->Get_robID()<<",  in hex=0x"<<std::hex<<collection->Get_robID()<<std::dec);
	    xAODSiHit->setStationID(-1);
	  }
	  
	  // set pixel layer ID
	  xAODSiHit->setPixelLayerID(hitLink%4);

	} else if (hitLink >= 8 && hitLink <= 11) {
	  // set far station ID selecting side based on ROB
	  if (collection->Get_robID() == AFP_ROBID::sideA)
	    xAODSiHit->setStationID(xAOD::AFPStationID::farA);
	  else if (collection->Get_robID() == AFP_ROBID::sideC)
	    xAODSiHit->setStationID(xAOD::AFPStationID::farC);
	  else  {
	    ATH_MSG_WARNING("Unrecognised robID: in dec="<<std::dec<<collection->Get_robID()<<",  in hex=0x"<<std::hex<<collection->Get_robID()<<std::dec);
	    xAODSiHit->setStationID(-1);
	  }

	  // set pixel layer ID
	  xAODSiHit->setPixelLayerID(hitLink%4);

	} else {
	  ATH_MSG_WARNING("Unrecognised value in hitLink = " << hitLink);
	  xAODSiHit->setStationID(-1);
	  xAODSiHit->setPixelLayerID(-1);
	} // end of switch

	ATH_MSG_DEBUG("Filled information about station and detector in xAODSiHit object");

	xAODSiHit->setPixelColIDChip( data->Get_column() );
	xAODSiHit->setPixelRowIDChip( data->Get_row() );

	const unsigned int ToT_value = data->Get_ToT();
	xAODSiHit->setTimeOverThreshold(ToT_value);
	xAODSiHit->setDepositedCharge(1909 + ToT_value * 363 + ToT_value * ToT_value * 141);

	ATH_MSG_DEBUG("Filled xAOD::AFPSiHit");
      }	// end of if else (hitLink == 12 || hitLink == 13)
      
    } // end of for over RawData_Collection_Beg using p_RawData_Beg

  } // end of for over container using RawData_Collection_Beg

  return StatusCode::SUCCESS;
}

StatusCode AFP_Raw2DigiTool::finalize(){
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}
