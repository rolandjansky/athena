/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/TrtRawNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "InDetRawData/TRT_RDORawData.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/TRT_RDO_Collection.h"

#include "InDetIdentifier/TRT_ID.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  TrtRawNtComponent::TrtRawNtComponent(NtupleAlgorithm *mainAlg,
				       NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  TrtRawNtComponent::~TrtRawNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode TrtRawNtComponent::setupServices(void) {
    StatusCode sc;
    
    if((sc = NtupleComponent::setupServices()).isFailure()) return sc;
    
    // Get a TRT identifier helper
    sc = (*m_detStore)->retrieve(m_trtHelper, "TRT_ID");
    if(sc.isFailure()) {
      (*m_msg) << MSG::ERROR << "Can't get " << m_trtHelper << endreq;
      return StatusCode::FAILURE;
    }

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("TRT_RDO_Container"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_trtRDO_ContKey = "TRT_RDOs"; // The default when no key is found.
    }
    else {
      m_trtRDO_ContKey = (*itr).second; // Use the setting.
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode TrtRawNtComponent::book(void) {

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_raw_tot = new std::vector<float>();
    m_raw_barrel_ec = new std::vector<short>();
    m_raw_phi_module = new std::vector<short>();
    m_raw_layer_or_wheel = new std::vector<short>();
    m_raw_straw_layer = new std::vector<short>();
    m_raw_straw = new std::vector<short>();
    
    m_tree->Branch("trt_nraw", &m_nraw, "trt_raw/i");
    m_tree->Branch("trt_raw_tot", &m_raw_tot);
    m_tree->Branch("trt_raw_barrel_ec", &m_raw_barrel_ec);
    m_tree->Branch("trt_raw_phi_module", &m_raw_phi_module);
    m_tree->Branch("trt_raw_layer_or_wheel", &m_raw_layer_or_wheel);
    m_tree->Branch("trt_raw_straw_layer", &m_raw_straw_layer);
    m_tree->Branch("trt_raw_straw", &m_raw_straw);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode TrtRawNtComponent::fill(void) {
    StatusCode sc;

    // Clear ntuple variables
    m_nraw = 0;
    m_raw_tot->clear();
    m_raw_barrel_ec->clear();
    m_raw_phi_module->clear();
    m_raw_layer_or_wheel->clear();
    m_raw_straw_layer->clear();
    m_raw_straw->clear();

    const TRT_RDO_Container *trtRDO_container=0;    
    sc = (*m_evtStore)->retrieve(trtRDO_container, m_trtRDO_ContKey);
    if(sc.isFailure() || !trtRDO_container) {

      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING <<  m_trtRDO_ContKey  <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    
    
    TRT_RDO_Container::const_iterator itrTrt = trtRDO_container->begin();
    TRT_RDO_Container::const_iterator itrTrt_end = trtRDO_container->end();
    
    // Loop over all system raw data objects (RDO)
    for( ; itrTrt!=itrTrt_end; ++itrTrt){
      InDetRawDataCollection<TRT_RDORawData>::const_iterator itrRawData = (*itrTrt)->begin();
      InDetRawDataCollection<TRT_RDORawData>::const_iterator itrRawData_end = (*itrTrt)->end();
      
      // Loop over TRT raw data objects
      for( ; itrRawData != itrRawData_end; ++itrRawData) {
	Identifier trt_id = (*itrRawData)->identify();
	m_raw_tot->push_back((float)(*itrRawData)->timeOverThreshold());
	m_raw_barrel_ec->push_back(m_trtHelper->barrel_ec(trt_id));
	m_raw_phi_module->push_back(m_trtHelper->phi_module(trt_id));
	m_raw_layer_or_wheel->push_back(m_trtHelper->layer_or_wheel(trt_id));
	m_raw_straw_layer->push_back(m_trtHelper->straw_layer(trt_id));
	m_raw_straw->push_back(m_trtHelper->straw(trt_id));
	m_nraw++;
      }
    }
    
    return StatusCode::SUCCESS;
  }
  
  //---------------------------------------------------------------------
}
