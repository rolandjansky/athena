// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file src/xAODTestReadCView.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Algorithm to test reading xAOD data (CView)
 */


#ifndef DATAMODELTESTDATAREAD_XAODTESTREADCVIEW_H
#define DATAMODELTESTDATAREAD_XAODTESTREADCVIEW_H


#include "DataModelTestDataCommon/CView.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm to test reading xAOD data (CView).
 */
class xAODTestReadCView
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestReadCView (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute_r (const EventContext& ctx) const override;


  /**
   * @brief Algorithm finalization; called at the end of the job.
   */
  virtual StatusCode finalize() override;


private:
  SG::ReadHandleKey<DMTest::CView> m_cviewKey;
  SG::WriteHandleKey<DMTest::CView> m_writeKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_XAODTESTREADCVIEW_H
