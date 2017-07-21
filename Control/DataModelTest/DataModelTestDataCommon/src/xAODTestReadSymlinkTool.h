// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file DataModelTestDataCommon/src/xAODTestReadSymlinkTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2017
 * @brief Test reading via a symlink, as a tool.
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTREADSYMLINKTOOL_H
#define DATAMODELTESTDATACOMMON_XAODTESTREADSYMLINKTOOL_H


#include "DataModelTestDataCommon/ITestTool.h"
#include "DataModelTestDataCommon/S1.h"
#include "DataModelTestDataCommon/S2.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthContainers/AuxElement.h"


namespace DMTest {


class xAODTestReadSymlinkTool
  : public extends<AthAlgTool, ITestTool>
{
public:
  /**
   * @brief Constructor.
   * @param type The tool type.
   * @param name The tool name.
   * @param parent The tool's parent.
   */
  xAODTestReadSymlinkTool (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);


  /**
   * @brief Tool initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Tool event processing.
   */
  virtual StatusCode doit (const EventContext& ctx) const override;


private:
  SG::ReadHandleKey<SG::AuxElement> m_objKey;

  SG::ReadHandleKey<DMTest::S1> m_s1Key;
  SG::ReadHandleKey<DMTest::S2> m_s2Key;
  SG::ReadHandleKey<DMTest::S2> m_aliasKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTREADSYMLINKTOOL_H
