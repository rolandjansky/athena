// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file DataModelTestDataCommon/src/xAODTestReadSymlink.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Test reading via a symlink.
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTREADSYMLINK_H
#define DATAMODELTESTDATACOMMON_XAODTESTREADSYMLINK_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthContainers/AuxElement.h"


namespace DMTest {


class xAODTestReadSymlink
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestReadSymlink (const std::string &name, ISvcLocator *pSvcLocator);


  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute_r (const EventContext& ctx) const override;


private:
  SG::ReadHandleKey<SG::AuxElement> m_objKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTREADSYMLINK_H
