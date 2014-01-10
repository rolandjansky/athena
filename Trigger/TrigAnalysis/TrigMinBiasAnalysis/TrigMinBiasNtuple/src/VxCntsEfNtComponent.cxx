/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/VxCntsEfNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigInDetEvent/TrigVertexCountsCollection.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  VxCntsEfNtComponent::VxCntsEfNtComponent(NtupleAlgorithm *mainAlg,
					   NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  VxCntsEfNtComponent::~VxCntsEfNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode VxCntsEfNtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("TrigVertexCountsCollection"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_vertexCountsEfContainerName = "HLT_vertexcounts"; // The default when no key is found.
    }
    else {
      m_vertexCountsEfContainerName = (*itr).second; // Use the setting.
    }

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_vtxNtrks = new std::vector<unsigned int>();
    m_vtxTrkPtSqSum = new std::vector<float>();
    
    m_tree->Branch("ef_vxcnts_nElements", &m_nElements, "lvl2_sp_nElements/i");
    m_tree->Branch("ef_vxcnts_vtxNtrks", &m_vtxNtrks);
    m_tree->Branch("ef_vxcnts_vtxTrkPtSqSum", &m_vtxTrkPtSqSum);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode VxCntsEfNtComponent::fill(void) {
    StatusCode sc;
    const TrigVertexCountsCollection *vxCountsCollection = 0;

    // Clear ntuple variables
    m_nElements = 0;
    m_vtxNtrks->clear();
    m_vtxTrkPtSqSum->clear();

    // Container is present only if the level 1 trigger item that
    // seeds the level 2 trigger item has fired.
    sc=(*m_evtStore)->retrieve(vxCountsCollection, m_vertexCountsEfContainerName);
    if( sc.isFailure()  || !vxCountsCollection ) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << m_vertexCountsEfContainerName <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
   
    m_nElements = vxCountsCollection->size();
    (*m_msg) << MSG::DEBUG << "There are " << m_nElements << " TrigVertexCounts entries." << endreq;
    
    TrigVertexCountsCollection::const_iterator itr = vxCountsCollection->begin();
    TrigVertexCountsCollection::const_iterator itr_end = vxCountsCollection->end();

    // The TE is full scan and not connected to an RoI.  All TEs
    // within a given event are therefore the same.  Just get the first TE.
    if(itr != itr_end) {

      (*m_msg) << MSG::DEBUG << "Dumping the first TrigVertexCounts entry." << endreq;

      // Fill the branch variables.
      *m_vtxNtrks = (*itr)->vtxNtrks();
      *m_vtxTrkPtSqSum = (*itr)->vtxTrkPtSqSum();
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
