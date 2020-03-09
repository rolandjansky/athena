// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file DataModelTestDataCommon/src/DummyDecisionWriter.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Make a dummy xAOD::TrigDecision to avoid warnings
 *        when writing bytestream.
 */


#ifndef DATAMODELTESTDATACOMMON_DUMMYDECISIONWRITER_H
#define DATAMODELTESTDATACOMMON_DUMMYDECISIONWRITER_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigger/TrigDecision.h"
#include "StoreGate/WriteHandleKey.h"


namespace DMTest {


/**
 * @brief Make a dummy xAOD::TrigDecision to avoid warnings
 *        when writing bytestream.
 */
class DummyDecisionWriter
  : public AthReentrantAlgorithm
{
public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;


  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  SG::WriteHandleKey<xAOD::TrigDecision> m_decisionKey
  { this, "DecisionKey", "decisions", "" };
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_DUMMYDECISIONWRITER_H
