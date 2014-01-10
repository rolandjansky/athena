/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/SpLvl2NtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  SpLvl2NtComponent::SpLvl2NtComponent(NtupleAlgorithm *mainAlg,
				       NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  SpLvl2NtComponent::~SpLvl2NtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode SpLvl2NtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("TrigSpacePointCountsCollection"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_spacePointLvl2ContainerName = "HLT_spacepoints"; // The default when no key is found.
    }
    else {
      m_spacePointLvl2ContainerName = (*itr).second; // Use the setting.
    }

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_pixelClusEndcapC_contents = new std::vector<float>();
    m_pixelClusBarrel_contents = new std::vector<float>();
    m_pixelClusEndcapA_contents = new std::vector<float>();
    m_droppedPixelModuleIds = new std::vector<unsigned int>();
    m_droppedSctModuleIds = new std::vector<unsigned int>();

    m_tree->Branch("lvl2_sp_nElements", &m_nElements, "lvl2_sp_nElements/i");
    m_tree->Branch("lvl2_sp_hPixelClusTotBins", &m_hPixelClusTotBins, "lvl2_sp_hPixelClusTotBins/i");
    m_tree->Branch("lvl2_sp_hPixelClusTotMin", &m_hPixelClusTotMin, "lvl2_sp_hPixelClusTotMin/F");
    m_tree->Branch("lvl2_sp_hPixelClusTotMax", &m_hPixelClusTotMax, "lvl2_sp_hPixelClusTotMax/F");
    m_tree->Branch("lvl2_sp_hPixelClusSizeBins",&m_hPixelClusSizeBins, "lvl2_sp_hPixelClusSizeBins/i");
    m_tree->Branch("lvl2_sp_hPixelClusSizeMin",&m_hPixelClusSizeMin, "lvl2_sp_hPixelClusSizeMin/F");
    m_tree->Branch("lvl2_sp_hPixelClusSizeMax",&m_hPixelClusSizeMax, "lvl2_sp_hPixelClusSizeMax/F");
    m_tree->Branch("lvl2_sp_pixelClusEndcapC_contents", &m_pixelClusEndcapC_contents);
    m_tree->Branch("lvl2_sp_pixelClusBarrel_contents", &m_pixelClusBarrel_contents);
    m_tree->Branch("lvl2_sp_pixelClusEndcapA_contents", &m_pixelClusEndcapA_contents);
    m_tree->Branch("lvl2_sp_droppedPixelModuleIds", &m_droppedPixelModuleIds);

    m_tree->Branch("lvl2_sp_sctSpEndcapC", &m_sctSpEndcapC, "lvl2_sp_sctSpEndcapC/i");
    m_tree->Branch("lvl2_sp_sctSpBarrel", &m_sctSpBarrel, "lvl2_sp_sctSpBarrel/i");
    m_tree->Branch("lvl2_sp_sctSpEndcapA", &m_sctSpEndcapA, "lvl2_sp_sctSpEndcapA/i");
    m_tree->Branch("lvl2_sp_droppedSctModuleIds", &m_droppedSctModuleIds);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode SpLvl2NtComponent::fill(void) {
    StatusCode sc;
    const TrigSpacePointCountsCollection *spCountsCollection = 0;

    // Clear ntuple variables
    m_nElements = 0;
    m_hPixelClusTotBins = 0;
    m_hPixelClusTotMin = 0.;
    m_hPixelClusTotMax = 0.;
    m_hPixelClusSizeBins = 0;
    m_hPixelClusSizeMin = 0.;
    m_hPixelClusSizeMax = 0.;
    m_pixelClusEndcapC_contents->clear();
    m_pixelClusBarrel_contents->clear();
    m_pixelClusEndcapA_contents->clear(); 
    m_droppedPixelModuleIds->clear();
    m_sctSpEndcapC = 0;
    m_sctSpBarrel = 0;
    m_sctSpEndcapA = 0;
    m_droppedSctModuleIds->clear();

    // Container is present only if the level 1 trigger item that
    // seeds the level 2 trigger item has fired.
    sc=(*m_evtStore)->retrieve(spCountsCollection, m_spacePointLvl2ContainerName);
    if( sc.isFailure()  || !spCountsCollection ) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING <<    m_spacePointLvl2ContainerName <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    
    std::vector<Identifier> droppedIdentifiers;
    std::vector<Identifier>::iterator id_itr;
    std::vector<Identifier>::iterator id_itr_end;

    TrigHisto2D pixelClusEndcapC;
    TrigHisto2D pixelClusBarrel;
    TrigHisto2D pixelClusEndcapA;
   
    m_nElements = spCountsCollection->size();
    (*m_msg) << MSG::DEBUG << "There are " << m_nElements << " TrigSpacePointCounts entries." << endreq;
    
    TrigSpacePointCountsCollection::const_iterator itr = spCountsCollection->begin();
    TrigSpacePointCountsCollection::const_iterator itr_end = spCountsCollection->end();

    // The TE is full scan and not connected to an RoI.  All TEs
    // within a given event are therefore the same.  Just get the first TE.
    if(itr != itr_end) {

      (*m_msg) << MSG::DEBUG << "Dumping the first TrigSpacePointCounts entry." << endreq;

      // Get a copy of the TrigHisto objects
      pixelClusEndcapC = (*itr)->pixelClusEndcapC();
      pixelClusBarrel = (*itr)->pixelClusBarrel();
      pixelClusEndcapA = (*itr)->pixelClusEndcapA();
      
      // The TrigHisto parameters are the same for endcapC, barrel and endcapA
      m_hPixelClusTotBins = pixelClusEndcapC.nbins_x();
      m_hPixelClusTotMin = pixelClusEndcapC.min_x();
      m_hPixelClusTotMax = pixelClusEndcapC.max_x();
      m_hPixelClusSizeBins = pixelClusEndcapC.nbins_y();
      m_hPixelClusSizeMin = pixelClusEndcapC.min_y();
      m_hPixelClusSizeMax = pixelClusEndcapC.max_y();
      *m_pixelClusEndcapC_contents = pixelClusEndcapC.contents();
      *m_pixelClusBarrel_contents = pixelClusBarrel.contents();
      *m_pixelClusEndcapA_contents = pixelClusEndcapA.contents();
      
      droppedIdentifiers.clear();
      droppedIdentifiers = (*itr)->droppedPixelModules();
      id_itr = droppedIdentifiers.begin();
      id_itr_end = droppedIdentifiers.end();
      for(;id_itr != id_itr_end; ++id_itr) {
	m_droppedPixelModuleIds->push_back((*id_itr).get_identifier32().get_compact());
      }

      m_sctSpEndcapC = (*itr)->sctSpEndcapC();
      m_sctSpBarrel = (*itr)->sctSpBarrel();
      m_sctSpEndcapA  = (*itr)->sctSpEndcapA();
      
      droppedIdentifiers.clear();
      droppedIdentifiers = (*itr)->droppedSctModules();
      id_itr = droppedIdentifiers.begin();
      id_itr_end = droppedIdentifiers.end();
      for(;id_itr != id_itr_end; ++id_itr) {
	m_droppedSctModuleIds->push_back((*id_itr).get_identifier32().get_compact());
      }
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
