/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/EventInfoNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"


namespace NtComponent {
  
  //---------------------------------------------------------------------

  EventInfoNtComponent::EventInfoNtComponent(NtupleAlgorithm *mainAlg,
					     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  EventInfoNtComponent::~EventInfoNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode EventInfoNtComponent::book(void) {

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_level1TriggerInfo = new std::vector<unsigned int>();
    m_level2TriggerInfo = new std::vector<unsigned int>();
    m_eventFilterInfo = new std::vector<unsigned int>();

    // Event ID
    m_tree->Branch("event_id_run_number", &m_runNumber,   "event_id_run_number/i");
    m_tree->Branch("event_id_evt_number", &m_eventNumber, "event_id_evt_number/i");
    m_tree->Branch("event_id_time_stamp", &m_time_stamp, "event_id_time_stamp/i");
    m_tree->Branch("event_id_time_stamp_ns_offset", &m_time_stamp_ns_offset, "event_id_time_stamp_ns_off/i");
    m_tree->Branch("event_id_lumiBlock", &m_lumiBlock, "event_id_lumiBlock/i");
    m_tree->Branch("event_id_bc_id", &m_bunch_crossing_id, "event_id_bc_id/i");
    m_tree->Branch("event_id_det_mask0", &m_detector_mask0, "event_id_det_mask0/i");
    m_tree->Branch("event_id_det_mask1", &m_detector_mask1, "event_id_det_mask1/i");

    // Event type, e.g. simulation, testbeam, etc
    // <Add here>

    // Trigger information
    m_tree->Branch("trigger_info_present", &m_trigger_info, "trigger_info_present/O");
    m_tree->Branch("trigger_info_status_element", &m_statusElement, "trigger_info_status_element/i");
    m_tree->Branch("trigger_info_extended_lvl1_id", &m_extendedLevel1ID, "trigger_info_extended_lvl1_id/i");
    m_tree->Branch("trigger_info_lvl1_trig_type", &m_level1TriggerType, "trigger_info_lvl1_trig_type/i");
    m_tree->Branch("trigger_info_nlvl1_trig_info", &m_nlevel1TriggerInfo, "trigger_info_nlvl1_trig_info/i");
    m_tree->Branch("trigger_info_lvl1_trig_info", &m_level1TriggerInfo);
    m_tree->Branch("trigger_info_nlvl2_trig_info", &m_nlevel2TriggerInfo, "trigger_info_nlvl2_trig_info/i");
    m_tree->Branch("trigger_info_lvl2_trig_info", &m_level2TriggerInfo);
    m_tree->Branch("trigger_info_nevt_filter_info", &m_neventFilterInfo, "trigger_info_nevt_filter_info/i");
    m_tree->Branch("trigger_info_evt_filter_info", &m_eventFilterInfo);
    

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode EventInfoNtComponent::fill(void) {
    StatusCode sc;
    const DataHandle<EventInfo> evt;
    const DataHandle<EventInfo> evtEnd;

    // Clear the ntuple variables
    m_trigger_info = false;
    m_statusElement = 0;
    m_extendedLevel1ID = 0;
    m_level1TriggerType = 0;
    m_nlevel1TriggerInfo = 0;
    m_level1TriggerInfo->clear();
    m_nlevel2TriggerInfo = 0;
    m_level2TriggerInfo->clear();
    m_neventFilterInfo = 0;
    m_eventFilterInfo->clear();

    sc=(*m_evtStore)->retrieve(evt, evtEnd);
    if( sc.isFailure()  || !evt ) {
      (*m_msg) << MSG::ERROR << "Error retrieving EventInfo!"<<endreq;
      return StatusCode::RECOVERABLE;
    }

    // Fill event id info
    m_eventNumber = evt->event_ID()->event_number();
    m_runNumber   = evt->event_ID()->run_number();
    m_time_stamp = evt->event_ID()->time_stamp();
    m_time_stamp_ns_offset = evt->event_ID()->time_stamp_ns_offset();
    m_lumiBlock = evt->event_ID()->lumi_block();
    m_bunch_crossing_id = evt->event_ID()->bunch_crossing_id();
    m_detector_mask0 = evt->event_ID()->detector_mask0();
    m_detector_mask1 = evt->event_ID()->detector_mask1();
    
    // Check if the trigger info is present
    if(!evt->trigger_info()) {
      m_trigger_info = false;
    }
    else {
      m_trigger_info = true;
      m_statusElement = evt->trigger_info()->statusElement();
      m_extendedLevel1ID = evt->trigger_info()->extendedLevel1ID();
      m_level1TriggerType = evt->trigger_info()->level1TriggerType();
      *m_level1TriggerInfo = evt->trigger_info()->level1TriggerInfo();
      m_nlevel1TriggerInfo = m_level1TriggerInfo->size();
      *m_level2TriggerInfo = evt->trigger_info()->level2TriggerInfo();
      m_nlevel2TriggerInfo = m_level2TriggerInfo->size();
      *m_eventFilterInfo = evt->trigger_info()->eventFilterInfo();
      m_neventFilterInfo = m_eventFilterInfo->size();
    }
      

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
