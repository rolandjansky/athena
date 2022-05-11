// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
// $Id$
/**
 * @file DataModelTestDataCommon/src/CondAlg2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief Testing conditions algorithm taking both RL and TS inputs
 *        and producing mixed output.
 */


#ifndef DATAMODELTESTDATACOMMON_CONDALG2_H
#define DATAMODELTESTDATACOMMON_CONDALG2_H

#include "DataModelTestDataCommon/S3Cond.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


namespace DMTest {


class CondAlg2
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param pSvcLocator The service locator.
   */
  CondAlg2 (const std::string &name, ISvcLocator *pSvcLocator);


  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  SG::ReadCondHandleKey<AthenaAttributeList> m_rltestKey;
  SG::ReadCondHandleKey<AthenaAttributeList> m_tstestKey;
  SG::WriteCondHandleKey<DMTest::S3> m_outKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_CONDALG2_H
