/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/src/CondWriterAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Write some conditions objects for testing purposes.
 */


#include "CondWriterAlg.h"
#include "DataModelTestDataCommon/S2Cond.h"
#include "EventInfo/EventID.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param pSvcLocator The service locator.
 */
CondWriterAlg::CondWriterAlg (const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator),
    m_regSvc ("IOVRegistrationSvc", name),
    m_streamer ("AthenaPoolOutputStreamTool/CondStream", this)
{
  declareProperty ("Streamer",     m_streamer);
  declareProperty ("EventInfoKey", m_eventInfoKey = "McEventInfo");
  declareProperty ("AttrListKey",  m_attrListKey = "/DMTest/TestAttrList");
  declareProperty ("S2Key",        m_s2Key       = "/DMTest/S2");
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode CondWriterAlg::initialize()
{
  ATH_CHECK( m_regSvc.retrieve() );
  ATH_CHECK( m_streamer.retrieve() );
  ATH_CHECK( m_eventInfoKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode CondWriterAlg::writeSCond (unsigned int count)
{
  auto s2 = std::make_unique<DMTest::S2> (count * 50);
  ATH_CHECK( detStore()->overwrite (std::move (s2), m_s2Key) );

  IAthenaOutputStreamTool::TypeKeyPairs typeKeys {{"DMTest::S2", m_s2Key}};

  ATH_CHECK( m_streamer->connectOutput() );
  ATH_CHECK( m_streamer->streamObjects (typeKeys) );
  ATH_CHECK( m_streamer->commitOutput() );

  ATH_CHECK( m_regSvc->registerIOV ("DMTest::S2",
                                    m_s2Key,
                                    "tag S2_noTag",
                                    IOVTime::MINRUN, IOVTime::MAXRUN,
                                    count, count+1) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode CondWriterAlg::execute()
{
  SG::ReadHandle<EventInfo> eventInfo (m_eventInfoKey);
  unsigned int count = eventInfo->event_ID()->event_number();

  auto attrList = std::make_unique<AthenaAttributeList>();
  attrList->extend ("xint", "int");
  (*attrList)["xint"].setValue(static_cast<int> ((count+1)*10));

  ATH_CHECK( detStore()->overwrite (std::move (attrList), m_attrListKey) );

  ATH_CHECK( m_regSvc->registerIOV ("AthenaAttributeList",
                                    m_attrListKey,
                                    "tag AttrList_noTag",
                                    IOVTime::MINRUN, IOVTime::MAXRUN,
                                    count, count) );

  if (count%2 == 0) {
    ATH_CHECK( writeSCond (count) );
  }

  return StatusCode::SUCCESS;
}


} // namespace DMTest
