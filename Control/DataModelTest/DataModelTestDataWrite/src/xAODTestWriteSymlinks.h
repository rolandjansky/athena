// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteSymlinks.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Testing making symlinks/aliases through handles.
 */


#ifndef DATAMODELTESTDATAWRITE_XAODTESTWRITESYMLINKS_H
#define DATAMODELTESTDATAWRITE_XAODTESTWRITESYMLINKS_H


#include "DataModelTestDataCommon/S1.h"
#include "DataModelTestDataCommon/S2.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm testing making symlinks/aliases through handles.
 */
class xAODTestWriteSymlinks
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestWriteSymlinks (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute (const EventContext& ctx) const override; 


private:
  /// Main output.
  SG::WriteHandleKey<DMTest::S2> m_s2Key;

  /// Symlink.
  SG::WriteHandleKey<DMTest::S1> m_s1Key;

  /// Alias.
  SG::WriteHandleKey<DMTest::S2> m_aliasKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_XAODTESTWRITESYMLINKS_H
