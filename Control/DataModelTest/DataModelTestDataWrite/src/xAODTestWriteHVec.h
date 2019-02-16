// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteHVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Algorithm to test writing xAOD classes for schema evolution (hvec/hview).
 */


#ifndef DATAMODELTESTDATAWRITE_XAODTESTWRITEHVEC_H
#define DATAMODELTESTDATAWRITE_XAODTESTWRITEHVEC_H


#include "DataModelTestDataWrite/HVec.h"
#include "DataModelTestDataWrite/HView.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm to test writing xAOD classes for schema evolution (hvec/hview).
 */
class xAODTestWriteHVec
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestWriteHVec (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute (const EventContext& ctx) const override;


  /**
   * @brief Algorithm finalization; called at the end of the job.
   */
  virtual StatusCode finalize() override;


private:
  SG::WriteHandleKey<DMTest::HVec> m_hvecKey;
  SG::WriteHandleKey<DMTest::HView> m_hviewKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_XAODTESTWRITEHVEC_H
