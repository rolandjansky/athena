// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/src/CondAlg1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Testing conditions algorithms.
 */


#ifndef DATAMODELTESTDATACOMMON_CONDALG1_H
#define DATAMODELTESTDATACOMMON_CONDALG1_H


#include "DataModelTestDataCommon/S2Cond.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


namespace DMTest {


class CondAlg1
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param pSvcLocator The service locator.
   */
  CondAlg1 (const std::string &name, ISvcLocator *pSvcLocator);


  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute_r (const EventContext& ctx) const override;


private:
  SG::ReadCondHandleKey<AthenaAttributeList> m_attrListKey;
  SG::WriteCondHandleKey<DMTest::S2> m_scondKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_CONDALG1_H
