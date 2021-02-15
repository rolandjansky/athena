// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DataModelTestDataRead/src/xAODTestShallowCopyHVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov 2020, from earlier version Jun, 2014
 * @brief Algorithm to test shallow-copy of xAOD data.
 */


#ifndef DATAMODELTESTDATAREAD_XAODTESTSHALLOWCOPYHVEC_H
#define DATAMODELTESTDATAREAD_XAODTESTSHALLOWCOPYHVEC_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DataModelTestDataRead/HVec.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ShallowCopyDecorDeps.h"


namespace DMTest {


/**
 * @brief Algorithm to do a shallow copy of HVec types.
 */
class xAODTestShallowCopyHVec
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
  SG::ReadHandleKey<DMTest::HVec> m_readKey
  { this, "ReadKey", "hvec", "Key for object read from SG" };

  SG::WriteHandleKey<DMTest::HVec> m_writeKey
  { this, "WriteKey", "scopy_hvec", "Key for object written to SG" };

  SG::WriteDecorHandleKey<DMTest::HVec> m_aFloat20Key
  { this, "AFloat20Key", "scopy_hvec.aFloat20", "Key for aFloat20 decoration" };
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_XAODTESTSHALLOWCOPYHVEC_H
