// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*
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


#include "xAODEventInfo/EventInfo.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"


namespace DMTest {


class CondWriterAlg
  : public AthAlgorithm
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
  virtual StatusCode execute() override;


private:
  StatusCode writeSCond (unsigned int count);
  StatusCode writeRLTest (unsigned int count);
  StatusCode writeTSTest (unsigned int count);

  ServiceHandle<IIOVRegistrationSvc> m_regSvc;
  ToolHandle<IAthenaOutputStreamTool> m_streamer;
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EvtInfo", "EventInfo", "EventInfo name"};
  std::string m_attrListKey;
  std::string m_s2Key;
  std::string m_rltestKey;
  std::string m_tstestKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_CONDWRITERALG_H
