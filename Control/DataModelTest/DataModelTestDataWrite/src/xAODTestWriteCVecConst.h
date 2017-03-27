// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteCVecConst.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2016
 * @brief Test recording a const aux obj via WriteHandle
 */


#ifndef DATAMODELTESTDATAWRITE_XAODTESTWRITECVECCONST_H
#define DATAMODELTESTDATAWRITE_XAODTESTWRITECVECCONST_H


#include "DataModelTestDataCommon/CVec.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"


namespace DMTest {


/**
 * @brief Test recording a const aux obj via WriteHandle
 */
class xAODTestWriteCVecConst
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestWriteCVecConst (const std::string &name, ISvcLocator *pSvcLocator);
  

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
  SG::WriteHandleKey<DMTest::CVec> m_cvecKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_XAODTESTWRITECVECCONST_H
