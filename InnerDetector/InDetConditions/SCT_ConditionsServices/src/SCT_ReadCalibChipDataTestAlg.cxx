/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibChipDataTestAlg.cxx Implementation file for SCT_ReadCalibChipDataTestAlg class
    @author: Per Johansson, based on Jorgen Dalmau TestReadSCT_CalibData example
    adapted first to test a tool and then a service
*/

// Include SCT_ReadCalibDataTestAlg and Svc
#include "SCT_ReadCalibChipDataTestAlg.h"
#include "SCT_ConditionsServices/ISCT_ReadCalibChipDataSvc.h"

// Include Athena stuff
#include "InDetIdentifier/SCT_ID.h"

// Include Gaudi stuff

// Include Read Handle
#include "StoreGate/ReadHandle.h"

// Include STL stuff
#include <vector>

//----------------------------------------------------------------------
SCT_ReadCalibChipDataTestAlg::SCT_ReadCalibChipDataTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_sc{0},
  m_id_sct{nullptr},
  m_currentEventKey{std::string("EventInfo")},
  m_moduleId{0},
  m_waferId{0},
  m_stripId{0},
  m_ReadCalibChipDataSvc{"SCT_ReadCalibChipDataSvc", name},
  m_doTestmyConditionsSummary{false},
  m_doTestmyDataSummary{false},
  m_moduleOfflinePosition{} {
    declareProperty("SCT_ReadCalibChipDataSvc",  m_ReadCalibChipDataSvc);
    declareProperty("DoTestmyConditionsSummary", m_doTestmyConditionsSummary = false, "Test return bool conditions summary?");
    declareProperty("DoTestmyDataSummary",       m_doTestmyDataSummary       = false, "Test return data summary?");
    declareProperty("ModuleOfflinePosition",     m_moduleOfflinePosition            , "Offline pos. as: B-EC,layer-disk,phi,eta");
  }

//----------------------------------------------------------------------
SCT_ReadCalibChipDataTestAlg::~SCT_ReadCalibChipDataTestAlg() {
}

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibChipDataTestAlg::initialize() {
  // Print where you are
  ATH_MSG_DEBUG("in initialize()");
  
  // Get SCT ID helper
  m_sc = detStore()->retrieve(m_id_sct, "SCT_ID");
  if (m_sc.isFailure()) {
    ATH_MSG_FATAL("Failed to get SCT ID helper");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Found SCT detector manager");
  }
  
  // Process jobOption properties
  m_sc = processProperties();
  if (m_sc.isFailure()) {
    ATH_MSG_ERROR("Failed to process jobOpt properties");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Processed jobOpt properties");
  }

  // Get the SCT_ReadCaliChipDataSvc
  m_sc = m_ReadCalibChipDataSvc.retrieve();
  if (m_sc.isFailure()) {
    ATH_MSG_FATAL("Cannot locate CalibChipData service");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("CalibChipData Service located ");
  }

  // Read Handle
  ATH_CHECK(m_currentEventKey.initialize());

  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTestAlg::initialize()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibChipDataTestAlg::processProperties()
{
  // Print where you are
  ATH_MSG_DEBUG("in processProperties()");
  
  // Get module position from jobOpt property
  std::vector<int>::const_iterator itLoop{m_moduleOfflinePosition.value().begin()};
  int offlineBarrelEC{*itLoop};  ++itLoop;
  int offlineLayerDisk{*itLoop}; ++itLoop;
  int offlineEta{*itLoop};       ++itLoop;
  int offlinePhi{*itLoop};       ++itLoop;
  int offlineSide{*itLoop};      ++itLoop;
  int offlineStrip{*itLoop};     ++itLoop;
  
  ATH_MSG_DEBUG("Module positions from jobOpt property:");
  ATH_MSG_DEBUG("B-EC/layer-disk/eta/phi: "
                << offlineBarrelEC  << "/"
                << offlineLayerDisk << "/"
                << offlineEta       << "/"
                << offlinePhi       << "/"
                << offlineSide      << "/"
                << offlineStrip);
  
  // Create offline Identifier for this module position, wafer,chip and strip
  m_moduleId = m_id_sct->module_id(offlineBarrelEC, offlineLayerDisk, offlinePhi, offlineEta);
  m_waferId = m_id_sct->wafer_id(offlineBarrelEC, offlineLayerDisk, offlinePhi, offlineEta, offlineSide);
  m_stripId = m_id_sct->strip_id(offlineBarrelEC, offlineLayerDisk, offlinePhi, offlineEta, offlineSide, offlineStrip);

  // Debug output
  ATH_MSG_DEBUG("id-getString : " << m_moduleId.getString());   // hex format
  ATH_MSG_DEBUG("id-getCompact: " << m_moduleId.get_compact()); // dec format
  ATH_MSG_DEBUG("id-getCompact2: " << m_stripId.get_compact()); // dec format
  ATH_MSG_DEBUG("Module Id: " << m_id_sct->print_to_string(m_moduleId));
  ATH_MSG_DEBUG("Strip Id: " << m_id_sct->print_to_string(m_stripId));
  
  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTestAlg::processProperties()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibChipDataTestAlg::execute() {
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  
  // Print where you are
  ATH_MSG_DEBUG("in execute()");
  
  // Get the current event
  SG::ReadHandle<xAOD::EventInfo> currentEvent{m_currentEventKey};
  if (not currentEvent.isValid()) {
    ATH_MSG_ERROR("Could not get event info");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Current Run.Event,Time: "
                << "[" << currentEvent->runNumber()
                << "." << currentEvent->eventNumber()
                << "," << currentEvent->timeStamp()
                << "]");
  
  //Make sure data was filled
  bool CalibChipDataFilled{m_ReadCalibChipDataSvc->filled()};
  if(CalibChipDataFilled) {
    //Test Chip Data ConditionsSummary
    if (m_doTestmyConditionsSummary) {
      // Test summmary, ask status of strip in module
      Identifier IdM{m_moduleId};
      Identifier IdS{m_waferId};
      bool Sok{m_ReadCalibChipDataSvc->isGood(IdS, InDetConditions::SCT_SIDE)};
      ATH_MSG_INFO("Side " << IdS << " on module " << IdM << " is " << (Sok ? "good" : "bad"));
    }
  
    //Test data summary
    if (m_doTestmyDataSummary) {
      // Test to get some data from the NPtGain or NoiseOccupancy
      Identifier Id{m_moduleId}; 
      const int side{1}; 
      
      // Try to get some NPtGain data
      // GainByChip, GainRMSByChip, NoiseByChip, NoiseRMSByChip, OffsetByChip, OffsetRMSByChip
      std::string whatNPdata{"GainByChip"};
      std::vector<float> NPdata{m_ReadCalibChipDataSvc->getNPtGainData(Id, side, whatNPdata)};
      for (unsigned int i{0}; i<NPdata.size(); i++) {
        ATH_MSG_INFO("The " << whatNPdata << " for chip number " << i << " on side " << side << " is: " << NPdata[i]);
      }

      // Try to get some NO data
      // occupancy, occupancyRMS, noise, offset
      std::string whatNOdata{"OccupancyRMSByChip"};
      std::vector<float> NOdata{m_ReadCalibChipDataSvc->getNoiseOccupancyData(Id, side, whatNOdata)};
      ATH_MSG_INFO("Size of returned data: "<<NOdata.size());
      for (unsigned int i{0}; i<NOdata.size(); i++) {
        ATH_MSG_INFO("The " << whatNOdata << " for chip number " << i << " on side " << side << " is: " << NOdata[i]);
      }
      
      // Try to get some INVALID NPtGain data
      // GainByChip, GainRMSByChip, NoiseByChip, NoiseRMSByChip, OffsetByChip, OffsetRMSByChip
      Identifier invalidId;//constructor forms invalid Id
      ATH_MSG_INFO("Trying to retrieve invalid data");
      std::vector<float> nvNPdata{m_ReadCalibChipDataSvc->getNPtGainData(invalidId, 0, whatNPdata)};
      const long unsigned int sizeOfInvalidNPData{nvNPdata.size()};
      ATH_MSG_INFO("Size of returned data: " << sizeOfInvalidNPData);
      for (long unsigned int i{0}; i!=sizeOfInvalidNPData; ++i) {
        ATH_MSG_INFO("The " << whatNPdata << " for chip number " << i << " on side " << side << " is: " << nvNPdata[i]);
      }
    }
  }
  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibChipDataTestAlg::finalize() {
  // Print where you are
  ATH_MSG_DEBUG("in finalize()");
  
  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTestAlg::finalize()

//----------------------------------------------------------------------
