// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/CondWriterAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Write some conditions objects for testing purposes.
 */


#ifndef DATAMODELTESTDATACOMMON_CONDWRITERALG_H
#define DATAMODELTESTDATACOMMON_CONDWRITERALG_H


#include "EventInfo/EventInfo.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"


namespace DMTest {


class CondWriterAlg
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param pSvcLocator The service locator.
   */
  CondWriterAlg (const std::string &name, ISvcLocator *pSvcLocator);


  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute_r (const EventContext& ctx) const override;


private:
  ServiceHandle<IIOVRegistrationSvc> m_regSvc;
  SG::ReadHandleKey<EventInfo> m_eventInfoKey;
  std::string m_attrListKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_CONDWRITERALG_H
