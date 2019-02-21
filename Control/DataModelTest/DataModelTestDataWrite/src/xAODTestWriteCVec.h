// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteCVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief Algorithm to test writing xAOD classes with auxiliary data (cvec).
 */


#ifndef DATAMODELTESTDATAWRITE_XAODTESTWRITECVEC_H
#define DATAMODELTESTDATAWRITE_XAODTESTWRITECVEC_H


#include "DataModelTestDataCommon/CVec.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm for creating test aux data.
 */
class xAODTestWriteCVec
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestWriteCVec (const std::string &name, ISvcLocator *pSvcLocator);
  

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
  SG::WriteHandleKey<DMTest::CVec> m_cvecKey;
  SG::WriteDecorHandleKey<DMTest::CVec> m_cvecDecorKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_XAODTESTWRITECVEC_H
