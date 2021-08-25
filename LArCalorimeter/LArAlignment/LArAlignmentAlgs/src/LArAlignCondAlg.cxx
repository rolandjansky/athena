/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArAlignCondAlg.h"

#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

#include <memory>

StatusCode LArAlignCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_writeKey.initialize());

  // Register Write Cond Handle
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArAlignCondAlg::execute(const EventContext& ctx) const
{
  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<GeoAlignmentStore> writeHandle{m_writeKey,ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }
  
  // ____________ Get Read Cond Object ____________
  SG::ReadCondHandle<DetCondKeyTrans> readHandle{m_readKey,ctx};
  const DetCondKeyTrans* readCdo{*readHandle};
  if(!readCdo) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved DetCondKeyTrans object form the Condition Store");

  writeHandle.addDependency(readHandle);

  // ____________ Construct new Write Cond Object and apply alignments  ____________
  std::unique_ptr<GeoAlignmentStore> writeCdo = std::make_unique<GeoAlignmentStore>();
  if(m_alignHelper.applyAlignments(detStore(),readCdo,writeCdo.get()).isFailure()) {
    ATH_MSG_ERROR("Failed to apply LAr alignments");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(writeHandle.record(std::move(writeCdo)));
  ATH_MSG_INFO("recorded new GeoAlignmentStore object for LAr with key " << writeHandle.key() 
	       << " and range " << writeHandle.getRange());

  return StatusCode::SUCCESS;
}
