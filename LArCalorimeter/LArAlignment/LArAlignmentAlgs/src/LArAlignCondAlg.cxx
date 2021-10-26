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
  ATH_CHECK(m_readLArAlignKey.initialize());
  ATH_CHECK(m_writeGeoAlignKey.initialize());

  // Register Write Cond Handle
  if(m_condSvc->regHandle(this, m_writeGeoAlignKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeGeoAlignKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArAlignCondAlg::execute()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<GeoAlignmentStore> writeGeoAlignHandle{m_writeGeoAlignKey,ctx};
  if (writeGeoAlignHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }
  
  // ____________ Get Read Cond Object ____________
  SG::ReadCondHandle<DetCondKeyTrans> readLArAlignHandle{m_readLArAlignKey,ctx};
  ATH_CHECK(readLArAlignHandle.isValid());
  ATH_MSG_DEBUG("Retrieved DetCondKeyTrans object form the Condition Store");
  writeGeoAlignHandle.addDependency(readLArAlignHandle);

  // ____________ Construct new Write Cond Object and apply alignments  ____________
  std::unique_ptr<GeoAlignmentStore> writeCdo = std::make_unique<GeoAlignmentStore>();
  if(m_alignHelper.applyAlignments(detStore(),*readLArAlignHandle,writeCdo.get()).isFailure()) {
    ATH_MSG_ERROR("Failed to apply LAr alignments");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(writeGeoAlignHandle.record(std::move(writeCdo)));
  ATH_MSG_INFO("recorded new GeoAlignmentStore object for LAr with key " << writeGeoAlignHandle.key() 
	       << " and range " << writeGeoAlignHandle.getRange());

  return StatusCode::SUCCESS;
}
