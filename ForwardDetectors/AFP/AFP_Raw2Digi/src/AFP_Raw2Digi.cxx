/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"


// AFP_Raw2Digi includes
#include "AFP_Raw2Digi/AFP_Raw2Digi.h"
#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitAuxContainer.h"

#include "TBranch.h"

//////////////////////////
//// constructor
//////////////////////////

AFP_Raw2Digi::AFP_Raw2Digi(const std::string &name, ISvcLocator *pSvcLocator)
    : AthAlgorithm(name, pSvcLocator) {



m_SiHitContainerName = "AFPSiHitContainer"; 
  declareProperty("SiHitContainerName", m_SiHitContainerName, "Name of the xAOD container");
}

//////////////////////////
//// destructor
//////////////////////////

AFP_Raw2Digi::~AFP_Raw2Digi() {}

//////////////////////////
//// initialize
//////////////////////////

StatusCode AFP_Raw2Digi::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

//////////////////////////
//// finalize
//////////////////////////

StatusCode AFP_Raw2Digi::finalize() {
  ATH_MSG_INFO("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

//////////////////////////
//// execute
//////////////////////////

StatusCode AFP_Raw2Digi::execute() {

  ATH_MSG_DEBUG("Executing " << name() << "...");

  const AFP_RawDataContainer *container = 0;
  StatusCode sc_read = evtStore()->retrieve(container, m_AFP_RawDataCollectionKey);
  if (sc_read.isFailure()) {
    ATH_MSG_WARNING("AFP_Raw2Digi: Could not find raw data container");
    return StatusCode::SUCCESS;
  }

  else {
    ATH_MSG_DEBUG("AFP_Raw2Digi: Raw data container retrieved");
  }


  xAOD::AFPSiHitContainer* siHitContainer = new xAOD::AFPSiHitContainer();
  CHECK( evtStore()->record(siHitContainer, m_SiHitContainerName) );
  xAOD::AFPSiHitAuxContainer* siHitAuxContainer = new xAOD::AFPSiHitAuxContainer();
  CHECK( evtStore()->record(siHitAuxContainer, m_SiHitContainerName + "Aux.") );
  siHitContainer->setStore(siHitAuxContainer);


  AFP_RawDataContainer::const_iterator RawData_Collection_Beg =
      container->begin();
  AFP_RawDataContainer::const_iterator RawData_Collection_End =
      container->end();

  ATH_MSG_DEBUG("Reading raw data container");

  for (; RawData_Collection_Beg != RawData_Collection_End;
       ++RawData_Collection_Beg) {

    AFP_RawDataCollection::const_iterator p_RawData_Beg =
        (*RawData_Collection_Beg)->begin();
    AFP_RawDataCollection::const_iterator p_RawData_End =
        (*RawData_Collection_Beg)->end();

      for (; p_RawData_Beg != p_RawData_End; ++p_RawData_Beg) {

      const uint32_t hitLink = (*p_RawData_Beg)->Get_link();

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

      xAODSiHit->setPixelColIDChip( (*p_RawData_Beg)->Get_column() );
      xAODSiHit->setPixelRowIDChip( (*p_RawData_Beg)->Get_row() );
      
      const unsigned int ToT_value = (*p_RawData_Beg)->Get_ToT();
      xAODSiHit->setTimeOverThreshold(ToT_value);
      xAODSiHit->setDepositedCharge(1909 + ToT_value * 363 + ToT_value * ToT_value * 141);

      ATH_MSG_DEBUG("Filled xAOD::AFPSiHit");

      }// end of for over RawData_Collection_Beg using p_RawData_Beg
    
  } // end of for over container using RawData_Collection_Beg

  return StatusCode::SUCCESS;
}
