/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*
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
    m_streamer ("AthenaOutputStreamTool/CondStream", this)
{
  declareProperty ("Streamer",     m_streamer);
  declareProperty ("AttrListKey",  m_attrListKey = "/DMTest/TestAttrList");
  declareProperty ("S2Key",        m_s2Key       = "/DMTest/S2");
  declareProperty ("RLTestKey",    m_rltestKey   = "/DMTest/RLTest");
  declareProperty ("TSTestKey",    m_tstestKey   = "/DMTest/TSTest");
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

  // Each IOV for this folder is two LBs long.
  ATH_CHECK( m_regSvc->registerIOV ("DMTest::S2",
                                    m_s2Key,
                                    "S2_noTag",
                                    0, 0,
                                    count, count+2) );

  return StatusCode::SUCCESS;
}


StatusCode CondWriterAlg::writeRLTest (unsigned int count)
{
  // Bound in LBN.
  static const unsigned int bounds[] = { 0, 1, 3, 4, 7, 8, 9 };

  unsigned int lbn = count / 3;
  const unsigned int* pos = std::find (std::begin(bounds),
                                       std::end(bounds),
                                       lbn);
  if (pos != std::end(bounds)) {
    unsigned int niov = pos - std::begin(bounds) + 1;

    auto attrList = std::make_unique<AthenaAttributeList>();
    attrList->extend ("xint", "int");
    (*attrList)["xint"].setValue(static_cast<int> (niov));
    ATH_CHECK( detStore()->overwrite (std::move (attrList), m_rltestKey) );

    ATH_CHECK( m_regSvc->registerIOV ("AthenaAttributeList",
                                      m_rltestKey,
                                      "RL_noTag",
                                      0, 0,
                                      lbn, IOVTime::MAXEVENT) );
  }

  return StatusCode::SUCCESS;
}


StatusCode CondWriterAlg::writeTSTest (unsigned int count)
{
  // Bound in count.
  static const unsigned int bounds[] = { 0, 3, 6, 7, 10, 17, 19, 22, 26 };

  const unsigned int* pos = std::find (std::begin(bounds),
                                       std::end(bounds),
                                       count);
  if (pos != std::end(bounds)) {
    unsigned int niov = pos - std::begin(bounds) + 1;

    auto attrList = std::make_unique<AthenaAttributeList>();
    attrList->extend ("xint", "int");
    (*attrList)["xint"].setValue(static_cast<int> (niov * 100));
    ATH_CHECK( detStore()->overwrite (std::move (attrList), m_tstestKey) );

    ATH_CHECK( m_regSvc->registerIOV ("AthenaAttributeList",
                                      m_tstestKey,
                                      "TS_noTag",
                                      static_cast<uint64_t>(count) * 1000000000,
                                      IOVTime::MAXTIMESTAMP) );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode CondWriterAlg::execute()
{
  SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey);
  unsigned int count = eventInfo->eventNumber();

  auto attrList = std::make_unique<AthenaAttributeList>();
  attrList->extend ("xint", "int");
  (*attrList)["xint"].setValue(static_cast<int> ((count+1)*10));

  ATH_CHECK( detStore()->overwrite (std::move (attrList), m_attrListKey) );

  ATH_CHECK( m_regSvc->registerIOV ("AthenaAttributeList",
                                    m_attrListKey,
                                    "AttrList_noTag",
                                    0, IOVTime::MAXRUN,
                                    count, IOVTime::MAXEVENT) );

  if (count%2 == 0) {
    ATH_CHECK( writeSCond (count) );
  }

  ATH_CHECK( writeRLTest (count) );
  ATH_CHECK( writeTSTest (count) );

  return StatusCode::SUCCESS;
}


} // namespace DMTest
