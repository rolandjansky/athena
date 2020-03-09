/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file DataModelTestDataCommon/src/DummyDecisionWriter.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Make a dummy xAOD::TrigDecision to avoid warnings
 *        when writing bytestream.
 */


#include "DummyDecisionWriter.h"
#include "xAODTrigger/TrigDecisionAuxInfo.h"
#include "StoreGate/WriteHandle.h"


namespace DMTest {


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode DummyDecisionWriter::initialize()
{
  ATH_CHECK( m_decisionKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode DummyDecisionWriter::execute (const EventContext& ctx) const
{
  SG::WriteHandle<xAOD::TrigDecision> decH (m_decisionKey, ctx);
  ATH_CHECK( decH.record (std::make_unique<xAOD::TrigDecision>(),
                          std::make_unique<xAOD::TrigDecisionAuxInfo>()) );

  return StatusCode::SUCCESS;
}


} // namespace DMTest

