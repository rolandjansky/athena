// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/src/xAODTestThinCVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Thin CVec objects.
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTTHINCVEC_H
#define DATAMODELTESTDATACOMMON_XAODTESTTHINCVEC_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DataModelTestDataCommon/CVec.h"
#include "StoreGate/ThinningHandleKey.h"


namespace DMTest {


/**
 * @brief Thin CVec objects.
 */
class xAODTestThinCVec
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


private:
  SG::ThinningHandleKey<DMTest::CVec> m_cvecKey
  { this, "CVecKey", "cvec", "Object being thinned" };

  StringProperty m_stream
  { this, "Stream", "STREAM", "Stream for which to apply thinning" };

  UnsignedIntegerProperty m_mask
  { this, "Mask", 0, "Mask to apply to event number when thinning" };
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTTHINCVEC_H
