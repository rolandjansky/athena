// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/src/xAODTestWriteCLinks.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Create CLinks/CLinksAOD objects.
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTWRITECLINKS_H
#define DATAMODELTESTDATACOMMON_XAODTESTWRITECLINKS_H


#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CLinks.h"
#include "DataModelTestDataCommon/CLinksContainer.h"
#include "DataModelTestDataCommon/CLinksAOD.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm for creating test aux data.
 */
class xAODTestWriteCLinks
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
  StatusCode fillCLinks (const std::string& key,
                         const CVec& cvec,
                         size_t ndx,
                         CLinks& clinks) const;
  StatusCode fillCLinksAOD (const std::string& key,
                            const CVec& cvec,
                            CLinksAOD& clinks) const;

  SG::ReadHandleKey<DMTest::CVec> m_cvecKey
  { this, "CVecKey", "cvec", "CVec object to target by links" };

  SG::WriteHandleKey<DMTest::CLinks> m_clinksKey
  { this, "CLinksKey", "clinks", "Standalone CLinks object to create" };

  SG::WriteHandleKey<DMTest::CLinksContainer> m_clinksContainerKey
  { this, "CLinksContainerKey", "clinksContainer", "CLinks container object to create" };

  SG::WriteHandleKey<DMTest::CLinksAOD> m_clinksAODKey
  { this, "CLinksAODKey", "clinksAOD", "AOD CLinks object to create" };
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTWRITECLINKS_H
