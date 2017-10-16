// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataRead/xAODTestReadCInfo.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2017
 * @brief Algorithm to test reading xAOD cinfo data.
 */


#ifndef DATAMODELTESTDATAREAD_XAODTESTREADCINFO_H
#define DATAMODELTESTDATAREAD_XAODTESTREADCINFO_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"


namespace DMTest {


/**
 * @brief Algorithm to test reading xAOD data.
 */
class xAODTestReadCInfo
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestReadCInfo (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize();


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute_r (const EventContext& ctx) const;


private:
  SG::ReadHandleKey<DMTest::C>  m_cinfoKey;
  SG::WriteHandleKey<DMTest::C> m_writeKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_XAODTESTREADCINFO_H
