// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file src/xAODTestReadHVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Algorithm to test reading xAOD data with schema evolution (HVec/HView)
 */


#ifndef DATAMODELTESTDATAREAD_XAODTESTREADHVEC_H
#define DATAMODELTESTDATAREAD_XAODTESTREADHVEC_H


#include "DataModelTestDataRead/HVec.h"
#include "DataModelTestDataRead/HView.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm to test reading xAOD data with schema evolution (HVec/HView)
 */
class xAODTestReadHVec
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestReadHVec (const std::string &name, ISvcLocator *pSvcLocator);
  

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
  SG::ReadHandleKey<DMTest::HVec> m_hvecKey;
  SG::ReadHandleKey<DMTest::HVec> m_hviewKey;
  SG::WriteHandleKey<DMTest::HVec> m_vecWriteKey;
  SG::WriteHandleKey<DMTest::HView> m_viewWriteKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_XAODTESTREADHVEC_H
