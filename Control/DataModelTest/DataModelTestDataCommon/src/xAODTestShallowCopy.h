// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DataModelTestDataCommon/src/xAODTestShallowCopy.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov 2020, from earlier version Jun, 2014
 * @brief Algorithm to test shallow-copy of xAOD data.
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTSHALLOWCOPY_H
#define DATAMODELTESTDATACOMMON_XAODTESTSHALLOWCOPY_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CVecWithData.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ShallowCopyDecorDeps.h"


namespace DMTest {


/**
 * @brief Algorithm to do a shallow copy of CVec types.
 */
class xAODTestShallowCopy
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
  SG::ReadHandleKey<DMTest::CVec> m_cvecReadKey
  { this, "CVecReadKey", "cvec", "Key for cvec object read from SG" };
  SG::WriteHandleKey<DMTest::CVec> m_cvecWriteKey
  { this, "CVecWriteKey", "scopy_cvec", "Key for cvec object written to SG" };
  SG::WriteDecorHandleKey<DMTest::CVec> m_cvecAnInt10Key
  { this, "CVecAnInt10Key", "scopy_cvec.anInt10", "Key for cvec anInt10 decoration" };
  SG::ShallowCopyDecorDeps<DMTest::CVec> m_cvecDecorDeps
  { this, "CVecDecorHelper", { "dInt1" } };

  SG::ReadHandleKey<DMTest::CVecWithData> m_cvecWDReadKey
  { this, "CVecWDReadKey", "cvecWD", "Key for cvecWD object read from SG" };
  SG::WriteHandleKey<DMTest::CVecWithData> m_cvecWDWriteKey
  { this, "CVecWDWriteKey", "scopy_cvecWD", "Key for cvecWD object written to SG" };
  SG::WriteDecorHandleKey<DMTest::CVecWithData> m_cvecWDAnInt10Key
  { this, "CVecWDAnInt10Key", "scopy_cvecWD.anInt10", "Key for cvecWD anInt10 decoration" };
  SG::ShallowCopyDecorDeps<DMTest::CVecWithData> m_cvecWDDecorDeps
  { this, "CVecWDDecorHelper", { "dInt1" } };

  SG::ReadHandleKey<DMTest::C> m_cinfoReadKey
  { this, "CInfoReadKey", "cinfo", "Key for cinfo object read from SG" };
  SG::WriteHandleKey<DMTest::C> m_cinfoWriteKey
  { this, "CInfoWriteKey", "scopy_cinfo", "Key for cinfo object written to SG" };
  SG::WriteDecorHandleKey<DMTest::C> m_cinfoAnInt10Key
  { this, "CInfoAnInt10Key", "scopy_cinfo.anInt10", "Key for cinfo anInt10 decoration" };
  SG::ShallowCopyDecorDeps<DMTest::C> m_cinfoDecorDeps
  { this, "CInfoDecorHelper", { "dInt1" } };

  SG::ReadHandleKey<DMTest::CVec> m_ctrigReadKey
  { this, "CTrigReadKey", "ctrig", "Key for ctrig object read from SG" };
  SG::WriteHandleKey<DMTest::CVec> m_ctrigWriteKey
  { this, "CTrigWriteKey", "scopy_ctrig", "Key for ctrig object written to SG" };
  SG::WriteDecorHandleKey<DMTest::CVec> m_ctrigAnInt10Key
  { this, "CTrigAnInt10Key", "scopy_ctrig.anInt10", "Key for ctrig anInt10 decoration" };
  SG::ShallowCopyDecorDeps<DMTest::CVec> m_ctrigDecorDeps
  { this, "CTrigDecorHelper", { "dInt1" } };

};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTSHALLOWCOPY_H
