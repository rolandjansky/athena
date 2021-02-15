// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DataModelTestDataRead/src/xAODTestRead.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Algorithm to test reading xAOD data.
 */


#ifndef DATAMODELTESTDATAREAD_XAODTESTREAD_H
#define DATAMODELTESTDATAREAD_XAODTESTREAD_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DataModelTestDataRead/GVec.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CVecWithData.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm to test reading xAOD data.
 */
class xAODTestRead
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
  /// Test reading container with additional data.
  StatusCode read_cvec_with_data (const EventContext& ctx) const;

  /// Test reading GVec object.
  StatusCode read_gvec (const EventContext& ctx) const;

  /// Test reading view container.
  //StatusCode read_cview() const;

  SG::ReadHandleKey<DMTest::CVec> m_ctrigReadKey
  { this, "CTrigReadKey", "ctrig", "" };

  SG::ReadHandleKey<DMTest::GVec> m_gvecReadKey
  { this, "GVecReadKey", "gvec", "" };

  SG::ReadHandleKey<DMTest::CVecWithData> m_cvecWDReadKey
  { this, "CVecWDReadKey", "cvecWD", "" };

  SG::WriteHandleKey<DMTest::CVec> m_ctrigWriteKey
  { this, "CTrigWriteKey", "", "" };

  SG::WriteHandleKey<DMTest::GVec> m_gvecWriteKey
  { this, "GVecWriteKey", "", "" };

  SG::WriteHandleKey<DMTest::CVecWithData> m_cvecWDWriteKey
  { this, "CVecWDWriteKey", "", "" };
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_XAODTESTREAD_H
