// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/src/xAODTestWriteCInfo.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief Algorithm to test writing xAOD classes with auxiliary data (cinfo).
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTWRITECINFO_H
#define DATAMODELTESTDATACOMMON_XAODTESTWRITECINFO_H


//#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventInfo.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm for creating test aux data.
 */
class xAODTestWriteCInfo
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestWriteCInfo (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute_r (const EventContext& ctx) const override;


  /**
   * @brief Algorithm finalization; called at the end of the job.
   */
  virtual StatusCode finalize() override;


private:
  int m_offset;
  //SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey;
  SG::ReadHandleKey<EventInfo> m_eventInfoKey;
  SG::ReadHandleKey<DMTest::CVec> m_cvecKey;
  SG::WriteHandleKey<DMTest::C> m_cinfoKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTWRITECINFO_H
