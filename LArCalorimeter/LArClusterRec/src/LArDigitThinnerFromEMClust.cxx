/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArClusterRec/LArDigitThinnerFromEMClust.h"

#include <iostream>

#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArDigit.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCell.h"
#include "AthContainers/ConstDataVector.h"



LArDigitThinnerFromEMClust::LArDigitThinnerFromEMClust(const std::string& name,
				 ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
    , m_larCablingSvc("LArCablingService") 
    , m_onlineID(0)
    , m_inputContainerName("FREE")
    , m_outputContainerName("LArDigitContainer_EMClust")
    , m_nchannels(0)
    , m_nevent(0)
    , m_ncells(0)
{
  declareProperty("InputContainerName", m_inputContainerName);
  declareProperty("OutputContainerName",m_outputContainerName);
  declareProperty("ClusterContainer",m_clusterContainerName);
}

LArDigitThinnerFromEMClust::~LArDigitThinnerFromEMClust() { }


StatusCode LArDigitThinnerFromEMClust::initialize() {

  StatusCode sc;

  sc = m_larCablingSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve LArCablingService Tool" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = detStore()->retrieve(m_onlineID, "LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endreq;
    return sc;
  } 

  m_nchannels = m_onlineID->channelHashMax();

  m_listCells.resize(m_nchannels);

  msg(MSG::INFO) << " Number of LAr online channels " << m_nchannels << endreq;

  m_nevent=0;
  m_ncells=0;
  
  return StatusCode::SUCCESS;
}

StatusCode LArDigitThinnerFromEMClust::finalize()
{
  msg(MSG::INFO) << "LArDigitThinnerFromEMClust Finalize" << endreq;  
  msg(MSG::INFO) << " Number of events " << m_nevent << endreq;
  msg(MSG::INFO) << " Number of digits written " << m_ncells << endreq;

  return StatusCode::SUCCESS;
}


StatusCode LArDigitThinnerFromEMClust::execute() {
  StatusCode sc;

  // Create the new digit container
  ConstDataVector<LArDigitContainer>* outputContainer = new ConstDataVector<LArDigitContainer>(SG::VIEW_ELEMENTS);
  if (!outputContainer){
    msg() << MSG::WARNING << "Could not allocate a new LArDigitContainer" << endreq;
    return StatusCode::SUCCESS;	  
  }
  
  sc = evtStore()->record(outputContainer , m_outputContainerName);
  if (sc.isFailure()) {
    msg(MSG::WARNING) << "Could not record output LArDigitContainer with key " 
		      << m_outputContainerName << endreq;
    return StatusCode::SUCCESS;
  }

  const LArDigitContainer* inputContainer = 0;
  sc = evtStore()->retrieve(inputContainer, m_inputContainerName);
  
  if (sc.isFailure()) { 
    msg(MSG::WARNING) << "Input LArDigitContainer not found with key"
		      << m_inputContainerName << endreq;
    return StatusCode::SUCCESS;
  }


  sc = getCells(); 
  if (sc.isFailure()) { 
    return StatusCode::SUCCESS;
  }

  m_nevent++;

  for (LArDigitContainer::const_iterator chan = inputContainer->begin(); 
       chan != inputContainer->end(); ++chan) 
  {       

    HWIdentifier channelID = (*chan)->channelID();
    IdentifierHash idHash = m_onlineID->channel_Hash(channelID);
    size_t index = (size_t) (idHash);
 
    if (!m_listCells[index]) continue;

    outputContainer->push_back(*chan);
    m_ncells++;
  }

  
  ATH_MSG_DEBUG("Copied " << outputContainer->size() << " of " << inputContainer->size() << " digits.");
  
  return StatusCode::SUCCESS;
}
  



StatusCode  LArDigitThinnerFromEMClust::getCells()
{
  
  for (size_t i=0;i<m_nchannels;i++) m_listCells[i]=false;
    
  const xAOD::CaloClusterContainer* clusterCollection=0;
  StatusCode sc = evtStore()->retrieve(clusterCollection, m_clusterContainerName);
    
  if ( sc.isFailure() || !clusterCollection) {
    msg() << MSG::WARNING
	  << " Cluster collection not found "
	  << endreq;
    return StatusCode::SUCCESS;
  }

  xAOD::CaloClusterContainer::const_iterator clusB = clusterCollection->begin();
  xAOD::CaloClusterContainer::const_iterator clusE = clusterCollection->end();
  for ( ; clusB!= clusE; ++clusB) {
    const xAOD::CaloCluster* clus = (*clusB);
    // loop over cells
    xAOD::CaloCluster::const_cell_iterator cellIter    = clus->cell_begin();
    xAOD::CaloCluster::const_cell_iterator cellIterEnd = clus->cell_end();
    for( ;cellIter!=cellIterEnd;cellIter++) { 
      const CaloCell* cell = (*cellIter);
      if (cell) {
         Identifier id = cell->ID();
         HWIdentifier hwid = m_larCablingSvc->createSignalChannelID(id);
         IdentifierHash idHash =  m_onlineID->channel_Hash(hwid);
         size_t index = (size_t) (idHash);
         m_listCells[index]=true;
      }
    }
  }

  return StatusCode::SUCCESS; 

}
