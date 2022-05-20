/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CopySCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetIdentifier/SCT_ID.h"

CopySCT_ClusterContainer::CopySCT_ClusterContainer(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { }

StatusCode CopySCT_ClusterContainer::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_inputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  ATH_CHECK(detStore()->retrieve(m_idHelper,"SCT_ID"));

  return StatusCode::SUCCESS;
}

StatusCode CopySCT_ClusterContainer::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");
  // Reading the input container
  ATH_MSG_VERBOSE("Retrieving input container");

  SG::ReadHandle<InDet::SCT_ClusterContainer> inputContainer(m_inputKey, ctx);
  if (!inputContainer.isValid()) {
    ATH_MSG_ERROR("Could not get pileup SCT_ClusterContainer " << inputContainer.name() << " from store " << inputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found pileup SCT_ClusterContainer " << inputContainer.name() << " in store " << inputContainer.store());

  // Creating output RDO container
  SG::WriteHandle<InDet::SCT_ClusterContainer> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<InDet::SCT_ClusterContainer>(m_idHelper->wafer_hash_max())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output SCT_ClusterContainer " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output SCT_ClusterContainer container " << outputContainer.name() << " in store " << outputContainer.store());

  for(const InDet::SCT_ClusterCollection* col : *inputContainer){
    InDet::SCT_ClusterCollection* newCol=new InDet::SCT_ClusterCollection(col->identifyHash());
    for(const InDet::SCT_Cluster* clus : *col){
      newCol->push_back(std::make_unique<InDet::SCT_Cluster>(*clus));
    }
    ATH_CHECK(outputContainer->addCollection(newCol,newCol->identifyHash()));
  }
  return StatusCode::SUCCESS;
}
