// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/src/xAODTestFilterCVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep 2016
 * @brief Algorithm to test filtering xAOD data.
 */


#ifndef DATAMODELTESTDATAREAD_XAODTESTFILTERCVEC_H
#define DATAMODELTESTDATAREAD_XAODTESTFILTERCVEC_H


#include "DataModelTestDataCommon/CVec.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm to test filtering xAOD data.
 */
class xAODTestFilterCVec
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestFilterCVec (const std::string &name, ISvcLocator *pSvcLocator);
  

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
  SG::ReadHandleKey<DMTest::CVec> m_cvecKey;
  SG::WriteHandleKey<DMTest::CVec> m_writeKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_XAODTESTFILTERCVEC_H
