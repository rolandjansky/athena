// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file DataModelTestDataCommon/src/xAODTestAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2017
 * @brief Algorithm used to test SG access from tools.
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTALG_H
#define DATAMODELTESTDATACOMMON_XAODTESTALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DataModelTestDataCommon/ITestTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace DMTest {


class xAODTestAlg : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestAlg (const std::string &name, ISvcLocator *pSvcLocator);


  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute_r (const EventContext& ctx) const override;


private:
  ToolHandleArray<ITestTool> m_tools;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTALG_H
