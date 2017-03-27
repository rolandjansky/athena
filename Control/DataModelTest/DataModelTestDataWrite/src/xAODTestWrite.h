// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file src/xAODTestWrite.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Algorithm to test writing xAOD classes with auxiliary data.
 */


#ifndef DATAMODELTESTDATAWRITE_XAODTESTWRITE_H
#define DATAMODELTESTDATAWRITE_XAODTESTWRITE_H


//#include "DataModelTestDataWrite/HVec.h"
#include "DataModelTestDataCommon/CVec.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm for creating test aux data.
 */
class xAODTestWrite
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestWrite (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize();


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute(); 


  /**
   * @brief Algorithm finalization; called at the end of the job.
   */
  virtual StatusCode finalize();


private:
  /// Test writing container with additional data.
  StatusCode write_cvec_with_data();

  /// Test schema evolution involving view container.
  //StatusCode write_htest();

  /// Event counter.
  int m_count;

  SG::ReadHandleKey<DMTest::CVec> m_cvecKey;
  //SG::WriteHandleKey<DMTest::HVec> m_hvecKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_XAODTESTWRITE_H
