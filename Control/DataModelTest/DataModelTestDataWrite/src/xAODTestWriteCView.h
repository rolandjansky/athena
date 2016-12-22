// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteCView.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief Algorithm to test writing an xAOD view vector (cview)
 */


#ifndef DATAMODELTESTDATAWRITE_XAODTESTWRITECVIEW_H
#define DATAMODELTESTDATAWRITE_XAODTESTWRITECVIEW_H


#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CView.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthContainers/ConstDataVector.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm to test writing an xAOD view vector (cview)
 */
class xAODTestWriteCView
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestWriteCView (const std::string &name, ISvcLocator *pSvcLocator);
  

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
  SG::ReadHandleKey<DMTest::CVec> m_cvecKey;
  SG::WriteHandleKey<ConstDataVector<DMTest::CView> > m_cviewKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_XAODTESTWRITECVIEW_H
