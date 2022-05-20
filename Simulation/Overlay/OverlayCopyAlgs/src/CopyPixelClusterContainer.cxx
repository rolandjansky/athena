/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyPixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetIdentifier/PixelID.h"

CopyPixelClusterContainer::CopyPixelClusterContainer(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { }

StatusCode CopyPixelClusterContainer::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_inputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  ATH_CHECK(detStore()->retrieve(m_idHelper,"PixelID"));

  return StatusCode::SUCCESS;
}

StatusCode CopyPixelClusterContainer::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");
  // Reading the input container
  ATH_MSG_VERBOSE("Retrieving input container");

  SG::ReadHandle<InDet::PixelClusterContainer> inputContainer(m_inputKey, ctx);
  if (!inputContainer.isValid()) {
    ATH_MSG_ERROR("Could not get pileup PixelClusterContainer " << inputContainer.name() << " from store " << inputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found pileup PixelClusterContainer " << inputContainer.name() << " in store " << inputContainer.store());

  // Creating output RDO container
  SG::WriteHandle<InDet::PixelClusterContainer> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<InDet::PixelClusterContainer>(m_idHelper->wafer_hash_max())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output PixelClusterContainer " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output PixelClusterContainer container " << outputContainer.name() << " in store " << outputContainer.store());

  for(const InDet::PixelClusterCollection* col : *inputContainer){
    InDet::PixelClusterCollection* newCol=new InDet::PixelClusterCollection(col->identifyHash());
    for(const InDet::PixelCluster* clus : *col){
      newCol->push_back(std::make_unique<InDet::PixelCluster>(*clus));
    }
    ATH_CHECK(outputContainer->addCollection(newCol,newCol->identifyHash()));
  }
  return StatusCode::SUCCESS;
}
