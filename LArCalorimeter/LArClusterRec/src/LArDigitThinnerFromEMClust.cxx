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

  ATH_CHECK( m_larCablingSvc.retrieve() );
  ATH_CHECK(  detStore()->retrieve(m_onlineID, "LArOnlineID") );

  m_nchannels = m_onlineID->channelHashMax();

  m_listCells.resize(m_nchannels);

  ATH_MSG_INFO( " Number of LAr online channels " << m_nchannels  );

  m_nevent=0;
  m_ncells=0;
  
  return StatusCode::SUCCESS;
}

StatusCode LArDigitThinnerFromEMClust::finalize()
{
  ATH_MSG_INFO( "LArDigitThinnerFromEMClust Finalize"  );
  ATH_MSG_INFO( " Number of events " << m_nevent  );
  ATH_MSG_INFO( " Number of digits written " << m_ncells  );

  return StatusCode::SUCCESS;
}


StatusCode LArDigitThinnerFromEMClust::execute() {
  StatusCode sc;

  // Create the new digit container
  ConstDataVector<LArDigitContainer>* outputContainer = new ConstDataVector<LArDigitContainer>(SG::VIEW_ELEMENTS);
  if (!outputContainer){
    ATH_MSG_WARNING( "Could not allocate a new LArDigitContainer"  );
    return StatusCode::SUCCESS;	  
  }
  
  sc = evtStore()->record(outputContainer , m_outputContainerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Could not record output LArDigitContainer with key " 
                     << m_outputContainerName  );
    return StatusCode::SUCCESS;
  }

  const LArDigitContainer* inputContainer = 0;
  sc = evtStore()->retrieve(inputContainer, m_inputContainerName);
  
  if (sc.isFailure()) { 
    ATH_MSG_WARNING( "Input LArDigitContainer not found with key"
                     << m_inputContainerName  );
    return StatusCode::SUCCESS;
  }


  ATH_CHECK( getCells() );

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
    ATH_MSG_WARNING( " Cluster collection not found " );
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
