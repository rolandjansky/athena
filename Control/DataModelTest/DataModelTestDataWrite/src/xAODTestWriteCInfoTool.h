// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteCInfoTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2017
 * @brief Tool to test writing xAOD classes with auxiliary data (cinfo).
 */


#ifndef DATAMODELTESTDATAWRITE_XAODTESTWRITECINFOTOOL_H
#define DATAMODELTESTDATAWRITE_XAODTESTWRITECINFOTOOL_H


#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/ITestTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm for creating test aux data.
 */
class xAODTestWriteCInfoTool
  : public extends<AthAlgTool, ITestTool>
{
public:
  /**
   * @brief Constructor.
   * @param type The tool type.
   * @param name The tool name.
   * @param parent The tool's parent.
   */
  xAODTestWriteCInfoTool (const std::string& type,
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
  SG::ReadHandleKey<DMTest::CVec> m_cvecKey;
  SG::WriteHandleKey<DMTest::C> m_cinfoKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_XAODTESTWRITECINFOTOOL_H
