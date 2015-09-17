/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMenuAnalysis/Lvl1ConfigNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigConfL1Data/CTPConfig.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  Lvl1ConfigNtComponent::Lvl1ConfigNtComponent(NtupleAlgorithm *mainAlg,
					       NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters),
    m_configSvc(0) {
    m_eventInfoKey = "";
    m_npit = 0; 
    m_pit_name = 0;
    m_pit_number = 0;
    m_item_name = 0;
    m_item_ctpId = 0;
    m_runNumber = -1;
  }
  
  //---------------------------------------------------------------------

  Lvl1ConfigNtComponent::~Lvl1ConfigNtComponent() {
    delete m_configSvc;
  }

  //---------------------------------------------------------------------

  StatusCode Lvl1ConfigNtComponent::setupServices(void) {
    StatusCode sc;

    // Initialise base class services
    if((sc = NtupleComponent::setupServices()).isFailure()) return sc;

    // Create service handle
    //    m_configSvc = new ServiceHandle<TrigConf::ILVL1ConfigSvc>("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", m_mainAlg->name());
    m_configSvc = new ServiceHandle<TrigConf::ILVL1ConfigSvc>("TrigConf::TrigConfigSvc/TrigConfigSvc", m_mainAlg->name());
    // Connect to the LVL1ConfigSvc.
    if((sc = m_configSvc->retrieve()).isFailure()) {
      (*m_msg) << MSG::ERROR << "Could not connect to " 
	       << m_configSvc->typeAndName() << endreq;
      return sc;
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode Lvl1ConfigNtComponent::book(void) {

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_pit_name = new std::vector<std::string>();
    m_pit_number = new std::vector<uint16_t>();
    m_item_name = new std::vector<std::string>();
    m_item_ctpId = new std::vector<int>();
    m_tree->Branch("run_number", &m_runNumber,   "run_number/i");
    m_tree->Branch("lvl1_config_npit", &m_npit, "lvl1_config_npit/i");
    m_tree->Branch("lvl1_config_pit_name", &m_pit_name);
    m_tree->Branch("lvl1_config_pit_number", &m_pit_number);
    m_tree->Branch("lvl1_config_item_name", &m_item_name);
    m_tree->Branch("lvl1_config_item_ctpId", &m_item_ctpId);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode Lvl1ConfigNtComponent::fill(void) {

    // Clear ntuple parameters
    m_runNumber = 0;
    m_npit = 0;
    m_pit_name->clear();
    m_pit_number->clear();
    m_item_name->clear();
    m_item_ctpId->clear();

    const DataHandle<EventInfo> evt;
    const DataHandle<EventInfo> evtEnd;

    StatusCode sc=StatusCode::SUCCESS;
    sc=(*m_evtStore)->retrieve(evt, evtEnd);
    if( sc.isFailure()  || !evt ) {
      (*m_msg) << MSG::ERROR << "Error retrieving EventInfo!"<<endreq;
      return StatusCode::RECOVERABLE;
    }

    m_runNumber   = evt->event_ID()->run_number();

    const TrigConf::CTPConfig *ctpConfig = 0;

    ctpConfig = (*m_configSvc)->ctpConfig();
    if(!ctpConfig) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << "Error retrieving CTPConfig from LVL1ConfigSvc" << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    
   
    std::vector< TrigConf::PIT* >::const_iterator pit_itr = ctpConfig->menu().pitVector().begin();
    std::vector< TrigConf::PIT* >::const_iterator pit_itr_end = ctpConfig->menu().pitVector().end();
    
    //if(pit_itr==pit_itr_end) {
      //(*m_msg) << MSG::INFO << "pitVector is empty." << endreq;
    //}

    for(;pit_itr!=pit_itr_end;++pit_itr) {
      //(*m_msg) << MSG::INFO << " pit name=" << (*pit_itr)->thresholdName() << " " << endreq;
      m_pit_name->push_back((*pit_itr)->thresholdName());
      m_pit_number->push_back((*pit_itr)->pitNumber());
    }

    for(TrigConf::ItemContainer::iterator item = ctpConfig->menu().items().begin(); item != ctpConfig->menu().items().end(); ++item) {
      m_item_name->push_back((*item)->name());
      m_item_ctpId->push_back((*item)->ctpId());
      //(*m_msg) << MSG::INFO << "name = " << (*item_itr)->name() << " ctpId=" << (*item_itr)->ctpId() << endreq;
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
