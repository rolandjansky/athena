/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/MbtsLvl2NtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  MbtsLvl2NtComponent::MbtsLvl2NtComponent(NtupleAlgorithm *mainAlg,
					   NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters),
    m_nElements(0),
    m_trigger_nenergies(0),
    m_triggerEnergies(0),
    m_trigger_ntimes(0),
    m_triggerTimes(0) {
  }
  
  //---------------------------------------------------------------------

  MbtsLvl2NtComponent::~MbtsLvl2NtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode MbtsLvl2NtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("TrigT2MbtsBitsContainer"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_mbtsLvl2ContainerName = "HLT_T2Mbts"; // The default when no key is found.
    }
    else {
      m_mbtsLvl2ContainerName = (*itr).second; // Use the setting.
    }

    m_triggerEnergies = new std::vector<float>();
    m_triggerTimes = new std::vector<float>();

    m_tree->Branch("lvl2_mbts_nElements", &m_nElements, "lvl2_mbts_nElements/i");
    m_tree->Branch("lvl2_mbts_nenergies", &m_trigger_nenergies, "lvl2_mbts_nenergies/i");
    m_tree->Branch("lvl2_mbts_energies", &m_triggerEnergies);
    m_tree->Branch("lvl2_mbts_ntimes", &m_trigger_ntimes, "lvl2_mbts_ntimes/i"); 
    m_tree->Branch("lvl2_mbts_times", &m_triggerTimes); 
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode MbtsLvl2NtComponent::fill(void) {
    StatusCode sc;
    const TrigT2MbtsBitsContainer *t2MbtsBits = 0;

    // Clear the ntuple variables
    m_nElements = 0;
    m_trigger_nenergies = 0;
    m_triggerEnergies->clear();
    m_trigger_ntimes = 0;
    m_triggerTimes->clear();

    // Container is present only if the level 1 trigger item that
    // seeds the level 2 trigger item has fired.
    sc=(*m_evtStore)->retrieve(t2MbtsBits, m_mbtsLvl2ContainerName);
    if( sc.isFailure()  || !t2MbtsBits ) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << "Container " << m_mbtsLvl2ContainerName << " not present but requested, perhaps L2 Mbts trigger didn't run?"  << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    
    m_nElements = t2MbtsBits->size();
    (*m_msg) << MSG::DEBUG << "There are " << m_nElements << " TrigT2MbtsBits entries." << endreq;
    
    // There should only be one element.
    TrigT2MbtsBitsContainer::const_iterator itr = t2MbtsBits->begin();
    TrigT2MbtsBitsContainer::const_iterator itr_end = t2MbtsBits->end();

    // The TE is full scan and not connected to an RoI.  All TEs
    // within a given event are therefore the same.  Just get the first TE.
    if(itr != itr_end) {
      (*m_msg) << MSG::DEBUG << "Dumping the first TrigT2MbtsBits entry." << endreq;

      *m_triggerEnergies = (*itr)->triggerEnergies();
      m_trigger_nenergies = m_triggerEnergies->size();
      *m_triggerTimes = (*itr)->triggerTimes();
      m_trigger_ntimes = m_triggerTimes->size();
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
