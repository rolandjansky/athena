/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibChipDataTestAlg.cxx Implementation file for SCT_ReadCalibChipDataTestAlg class
    @author: Per Johansson, based on Jorgen Dalmau TestReadSCT_CalibData example
    adapted first to test a tool and the a service
*/

// Include SCT_ReadCalibDataTestAlg and Svc
#include "SCT_ReadCalibChipDataTestAlg.h"
#include "SCT_ConditionsServices/ISCT_ReadCalibChipDataSvc.h"

// Include Event Info 
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Include Athena stuff
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"

// Include Gaudi stuff
#include "GaudiKernel/StatusCode.h"

// Include STL stuff
#include <vector>
#include <string>
using namespace std;

//----------------------------------------------------------------------
SCT_ReadCalibChipDataTestAlg::SCT_ReadCalibChipDataTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_sc(0),
  m_id_sct(0),
  m_currentEvent(0),
  m_moduleId(0),
  m_waferId(0),
  m_stripId(0),
  m_ReadCalibChipDataSvc("SCT_ReadCalibChipDataSvc",name),
  m_doTestmyConditionsSummary(false),
  m_doTestmyDataSummary(false),
  m_moduleOfflinePosition()
{
  declareProperty("SCT_ReadCalibChipDataSvc", m_ReadCalibChipDataSvc);
  declareProperty("DoTestmyConditionsSummary",   m_doTestmyConditionsSummary   = false, "Test return bool conditions summary?");
  declareProperty("DoTestmyDataSummary",         m_doTestmyDataSummary         = false, "Test return data summary?");
  declareProperty("ModuleOfflinePosition",       m_moduleOfflinePosition              , "Offline pos. as: B-EC,layer-disk,phi,eta");
}

//----------------------------------------------------------------------
SCT_ReadCalibChipDataTestAlg::~SCT_ReadCalibChipDataTestAlg()
{ }

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibChipDataTestAlg::initialize()
{
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in initialize()" << endreq;
  
  // Get SCT ID helper
  m_sc = detStore()->retrieve(m_id_sct, "SCT_ID");
  if (m_sc.isFailure()) {
    msg(MSG::FATAL) << "Failed to get SCT ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found SCT detector manager" << endreq;
  }
  
  // Process jobOption properties
  m_sc = processProperties();
  if (m_sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to process jobOpt properties" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Processed jobOpt properties" << endreq;
  }

  // Get the SCT_ReadCaliChipDataSvc
  m_sc = m_ReadCalibChipDataSvc.retrieve();
  if (m_sc.isFailure()) {
    msg(MSG::FATAL) << "Cannot locate CalibChipData service" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CalibChipData Service located " << endreq;
  }

  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTestAlg::initialize()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibChipDataTestAlg::processProperties()
{
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in processProperties()" << endreq;
  
  // Get module position from jobOpt property
  std::vector<int>::const_iterator itLoop = m_moduleOfflinePosition.value().begin();
  int offlineBarrelEC  = (*itLoop); ++itLoop;
  int offlineLayerDisk = (*itLoop); ++itLoop;
  int offlineEta       = (*itLoop); ++itLoop;
  int offlinePhi       = (*itLoop); ++itLoop;
  int offlineSide      = (*itLoop); ++itLoop;
  int offlineStrip     = (*itLoop); ++itLoop;
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Module positions from jobOpt property:" << endreq;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "B-EC/layer-disk/eta/phi: "
      << offlineBarrelEC  << "/"
      << offlineLayerDisk << "/"
      << offlineEta       << "/"
      << offlinePhi       << "/"
      << offlineSide      << "/"
      << offlineStrip     << endreq;
  
  // Create offline Identifier for this module position, wafer,chip and strip
  m_moduleId = m_id_sct->module_id(offlineBarrelEC, offlineLayerDisk, offlinePhi, offlineEta);
  m_waferId = m_id_sct->wafer_id(offlineBarrelEC, offlineLayerDisk, offlinePhi, offlineEta, offlineSide);
  m_stripId = m_id_sct->strip_id(offlineBarrelEC, offlineLayerDisk, offlinePhi, offlineEta, offlineSide, offlineStrip);

  // Debug output
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "id-getString : " << m_moduleId.getString() << endreq;   // hex format
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "id-getCompact: " << m_moduleId.get_compact() << endreq; // dec format
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "id-getCompact2: " << m_stripId.get_compact() << endreq; // dec format
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Module Id: " << m_id_sct->print_to_string(m_moduleId) << endreq;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Strip Id: " << m_id_sct->print_to_string(m_stripId) << endreq;
  
  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTestAlg::processProperties()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibChipDataTestAlg::execute()
{
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in execute()" << endreq;
  
  // Get the current event
  m_sc = evtStore()->retrieve(m_currentEvent);
  if ( m_sc.isFailure() ) {
    msg(MSG::ERROR) << "Could not get event info" << endreq;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Current Run.Event,Time: "
      << "[" << m_currentEvent->event_ID()->run_number()
      << "." << m_currentEvent->event_ID()->event_number()
      << "," << m_currentEvent->event_ID()->time_stamp()
      << "]" << endreq;
  
  //Make sure data was filled
  bool CalibChipDataFilled = m_ReadCalibChipDataSvc->filled();  
  if( CalibChipDataFilled ){
       
    //Test Chip Data ConditionsSummary
    if (m_doTestmyConditionsSummary) {
      // Test summmary, ask status of strip in module
      Identifier IdM = m_moduleId;
      Identifier IdS = m_waferId;
      bool Sok = m_ReadCalibChipDataSvc->isGood(IdS, InDetConditions::SCT_SIDE);
      msg(MSG::INFO) << "Side " << IdS << " on module " << IdM << " is " << (Sok?"good":"bad") << endreq;
    }
  
    //Test data summary
    if (m_doTestmyDataSummary) {
      // Test to get some data from the NPtGain or NoiseOccupancy
      Identifier Id = m_moduleId; 
      const int side = 1; 
      
      // Try to get some NPtGain data
      // GainByChip, GainRMSByChip, NoiseByChip, NoiseRMSByChip, OffsetByChip, OffsetRMSByChip
      std::string whatNPdata = "GainByChip";
      std::vector<float> NPdata = m_ReadCalibChipDataSvc->getNPtGainData(Id, side, whatNPdata);
      for ( unsigned int i = 0;  i < NPdata.size(); i++ ){
        msg(MSG::INFO) << "The " << whatNPdata << " for chip number " << i << " on side " << side << " is: " << NPdata[i] << endreq;
      }

      // Try to get some NO data
      // occupancy, occupancyRMS, noise, offset
      std::string whatNOdata = "OccupancyRMSByChip";
      std::vector<float> NOdata = m_ReadCalibChipDataSvc->getNoiseOccupancyData(Id, side, whatNOdata);
      msg(MSG::INFO)<<"Size of returned data: "<<NOdata.size()<<endreq;
      for ( unsigned int i = 0;  i < NOdata.size(); i++ ){
       msg(MSG::INFO) << "The " << whatNOdata << " for chip number " << i << " on side " << side << " is: " << NOdata[i] << endreq;
      }
      
        // Try to get some INVALID NPtGain data
        // GainByChip, GainRMSByChip, NoiseByChip, NoiseRMSByChip, OffsetByChip, OffsetRMSByChip
      Identifier invalidId;//constructor forms invalid Id
      msg(MSG::INFO)<<"Trying to retrieve invalid data"<<endreq;
      std::vector<float> nvNPdata = m_ReadCalibChipDataSvc->getNPtGainData(invalidId, 0, whatNPdata);
      msg(MSG::INFO)<<"Size of returned data: "<<nvNPdata.size()<<endreq;
      for ( unsigned int i = 0;  i < nvNPdata.size(); i++ ){
        msg(MSG::INFO) << "The " << whatNPdata << " for chip number " << i << " on side " << side << " is: " << NPdata[i] << endreq;
      }
      
    }
    
  }

  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibChipDataTestAlg::finalize()
{
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in finalize()" << endreq;
  
  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTestAlg::finalize()

//----------------------------------------------------------------------

