/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyTRT_DriftCircleContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetIdentifier/TRT_ID.h"

CopyTRT_DriftCircleContainer::CopyTRT_DriftCircleContainer(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { }

StatusCode CopyTRT_DriftCircleContainer::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_inputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  ATH_CHECK(detStore()->retrieve(m_idHelper,"TRT_ID"));

  return StatusCode::SUCCESS;
}

StatusCode CopyTRT_DriftCircleContainer::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");
  // Reading the input container
  ATH_MSG_VERBOSE("Retrieving input container");

  SG::ReadHandle<InDet::TRT_DriftCircleContainer> inputContainer(m_inputKey, ctx);
  if (!inputContainer.isValid()) {
    ATH_MSG_ERROR("Could not get pileup TRT_DriftCircleContainer " << inputContainer.name() << " from store " << inputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found pileup TRT_DriftCircleContainer " << inputContainer.name() << " in store " << inputContainer.store());

  // Creating output RDO container
  SG::WriteHandle<InDet::TRT_DriftCircleContainer> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<InDet::TRT_DriftCircleContainer>(m_idHelper->straw_layer_hash_max())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output TRT_DriftCircleContainer " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output TRT_DriftCircleContainer container " << outputContainer.name() << " in store " << outputContainer.store());

  for(const InDet::TRT_DriftCircleCollection* col : *inputContainer){
    InDet::TRT_DriftCircleCollection* newCol=new InDet::TRT_DriftCircleCollection(col->identifyHash());
    for(const InDet::TRT_DriftCircle* clus : *col){
      newCol->push_back(std::make_unique<InDet::TRT_DriftCircle>(*clus));
    }
    ATH_CHECK(outputContainer->addCollection(newCol,newCol->identifyHash()));
  }
  return StatusCode::SUCCESS;
}
