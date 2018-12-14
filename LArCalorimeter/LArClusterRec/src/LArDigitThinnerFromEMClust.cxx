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
#include "LArCabling/LArOnOffIdMapping.h"


LArDigitThinnerFromEMClust::LArDigitThinnerFromEMClust(const std::string& name,
				 ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator)
    , m_onlineID(0)
    , m_inputContainerKey("FREE")
    , m_outputContainerKey("LArDigitContainer_EMClust")
    , m_larCablingKey("LArOnOffIdMap") 
{
  declareProperty("InputContainerName", m_inputContainerKey);
  declareProperty("OutputContainerName",m_outputContainerKey);
  declareProperty("ClusterContainer",m_clusterContainerKey);
  declareProperty("LArOnOffMap",m_larCablingKey);
}

LArDigitThinnerFromEMClust::~LArDigitThinnerFromEMClust() { }


StatusCode LArDigitThinnerFromEMClust::initialize() {
  ATH_CHECK( detStore()->retrieve(m_onlineID, "LArOnlineID") );
  ATH_CHECK( m_inputContainerKey.initialize());
  ATH_CHECK( m_outputContainerKey.initialize());
  ATH_CHECK( m_clusterContainerKey.initialize());
  ATH_CHECK( m_larCablingKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode LArDigitThinnerFromEMClust::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode LArDigitThinnerFromEMClust::execute(const EventContext& ctx) const {

  //Get inputs from read handles:
  SG::ReadHandle<LArDigitContainer> inputContainer(m_inputContainerKey,ctx);
  SG::ReadHandle<xAOD::CaloClusterContainer> clusterContainer(m_clusterContainerKey,ctx); 

  //Write output via write handle
  SG::WriteHandle<ConstDataVector<LArDigitContainer> >outputContainer(m_outputContainerKey,ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<ConstDataVector<LArDigitContainer> >(SG::VIEW_ELEMENTS)));

  //Get cable map via read conditions handle
  SG::ReadCondHandle<LArOnOffIdMapping> larCablingHdl(m_larCablingKey,ctx);
  const LArOnOffIdMapping* larCabling=*larCablingHdl;

  std::bitset<200000> clusteredDigits;

  //Loop over Clusters:
  for (const xAOD::CaloCluster* clus : *clusterContainer) {

    //Loop over cells in cluster:
    xAOD::CaloCluster::const_cell_iterator cellIter    = clus->cell_begin();
    xAOD::CaloCluster::const_cell_iterator cellIterEnd = clus->cell_end();
    for( ;cellIter!=cellIterEnd;cellIter++) { 
      const CaloCell* cell = (*cellIter);
      if (cell) {
	Identifier id = cell->ID();
	HWIdentifier hwid = larCabling->createSignalChannelID(id);
	IdentifierHash idHash =  m_onlineID->channel_Hash(hwid);
	size_t index = (size_t) (idHash);
	clusteredDigits.set(index);
      }
    }//end loop over cells in cluster
  }//end loop over cluster


  for (const LArDigit* dig : *inputContainer) {
    HWIdentifier channelID = dig->channelID();
    IdentifierHash idHash = m_onlineID->channel_Hash(channelID);
    size_t index = (size_t) (idHash);
    if (clusteredDigits.test(index)) outputContainer->push_back(dig);
  }

  ATH_MSG_DEBUG("Copied " << outputContainer->size() << " of " << inputContainer->size() << " digits.");
  
  return StatusCode::SUCCESS;
}
