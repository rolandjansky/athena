// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/src/CondReaderAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Testing conditions handling.
 */


#ifndef DATAMODELTESTDATACOMMON_CONDREADERALG_H
#define DATAMODELTESTDATACOMMON_CONDREADERALG_H


#include "DataModelTestDataCommon/S1Cond.h"
#include "EventInfo/EventInfo.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"


namespace DMTest {


class CondReaderAlg
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param pSvcLocator The service locator.
   */
  CondReaderAlg (const std::string &name, ISvcLocator *pSvcLocator);


  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute_r (const EventContext& ctx) const override;


private:
  ServiceHandle<IChronoStatSvc> m_chronoSvc;
  SG::ReadHandleKey<EventInfo> m_eventInfoKey;
  SG::ReadCondHandleKey<AthenaAttributeList> m_attrListKey;
  SG::ReadCondHandleKey<DMTest::S1> m_scondKey;
  SG::ReadCondHandleKey<DMTest::S1> m_s2Key;
  size_t m_spins;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_CONDREADERALG_H
