// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/src/xAODTestReadCLinks.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Read and dump CLinks/CLinksAOD objects.
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTREADCLINKS_H
#define DATAMODELTESTDATACOMMON_XAODTESTREADCLINKS_H


#include "DataModelTestDataCommon/CLinks.h"
#include "DataModelTestDataCommon/CLinksContainer.h"
#include "DataModelTestDataCommon/CLinksAOD.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"


namespace DMTest {


/**
 * @brief Read and dump CLinks/CLinksAOD objects.
 */
class xAODTestReadCLinks
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
  /**
   * @brief Dump a CLinks object.
   */
  StatusCode dumpCLinks (const CLinks& clinks) const;


  SG::ReadHandleKey<DMTest::CLinks> m_clinksKey
  { this, "CLinksKey", "clinks", "Standalone CLinks object key" };

  SG::ReadHandleKey<DMTest::CLinksContainer> m_clinksContainerKey
  { this, "CLinksContainerKey", "clinksContainer", "CLinks container key" };

  SG::ReadHandleKey<DMTest::CLinksAOD> m_clinksAODKey
  { this, "CLinksAODKey", "clinksAOD", "AOD CLinks container key" };
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTREADCLINKS_H
