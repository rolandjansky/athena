// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file DataModelTestDataCommon/src/xAODTestReadCVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief Algorithm to test reading xAOD data (CVec)
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTREADCVEC_H
#define DATAMODELTESTDATACOMMON_XAODTESTREADCVEC_H


#include "DataModelTestDataCommon/CVec.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm to test reading xAOD data (CVec).
 */
class xAODTestReadCVec
  : public AthReentrantAlgorithm
{
public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;
  

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
  SG::ReadHandleKey<DMTest::CVec> m_cvecKey
  { this, "CVecKey", "cvec", "" };

  SG::WriteHandleKey<DMTest::CVec> m_writeKey
  { this, "WriteKey", "", "" };

  BooleanProperty m_brief
  { this, "Brief", false, "" };
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTREADCVEC_H
