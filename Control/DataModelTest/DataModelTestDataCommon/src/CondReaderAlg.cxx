/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DataModelTestDataCommon/src/CondReaderAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Write some conditions objects for testing purposes.
 */


#include "CondReaderAlg.h"
#include "EventInfo/EventID.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "GaudiKernel/Chrono.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param pSvcLocator The service locator.
 */
CondReaderAlg::CondReaderAlg (const std::string& name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_chronoSvc ("ChronoStatSvc", name),
    m_attrListKey ("/DMTest/TestAttrList"),
    m_scondKey ("scond", "DMTest"),
    m_s2Key ("/DMTest/S2"),
    m_s3Key(""),
    m_rltestKey (""),
    m_tstestKey ("")
{
  declareProperty ("ChronoSvc",    m_chronoSvc);
  declareProperty ("EventInfoKey", m_eventInfoKey = "McEventInfo");
  declareProperty ("AttrListKey",  m_attrListKey);
  declareProperty ("SCondKey",     m_scondKey);
  declareProperty ("S2Key",        m_s2Key);
  declareProperty ("S3Key",        m_s3Key);
  declareProperty ("RLTestKey",    m_rltestKey);
  declareProperty ("TSTestKey",    m_tstestKey);
  declareProperty ("Spins",        m_spins = 0);
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode CondReaderAlg::initialize()
{
  ATH_CHECK( m_chronoSvc.retrieve() );
  ATH_CHECK( m_eventInfoKey.initialize() );
  ATH_CHECK( m_attrListKey.initialize() );
  ATH_CHECK( m_scondKey.initialize() );

  // Allow running without POOL payload
  if ( !m_s2Key.key().empty()) ATH_CHECK( m_s2Key.initialize() );

  if ( !m_s3Key.key().empty())     ATH_CHECK( m_s3Key.initialize() );
  if ( !m_rltestKey.key().empty()) ATH_CHECK( m_rltestKey.initialize() );
  if ( !m_tstestKey.key().empty()) ATH_CHECK( m_tstestKey.initialize() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode CondReaderAlg::execute (const EventContext& ctx) const
{
  SG::ReadHandle<EventInfo> eventInfo (m_eventInfoKey, ctx);

  ATH_MSG_INFO ("Event " << eventInfo->event_ID()->event_number() <<
                " LBN " << eventInfo->event_ID()->lumi_block());

  SG::ReadCondHandle<AthenaAttributeList> attrList (m_attrListKey, ctx);
  ATH_MSG_INFO ("  xint " << (**attrList)["xint"]);

  SG::ReadCondHandle<DMTest::S1> s1 (m_scondKey, ctx);
  ATH_MSG_INFO ("  scond " << s1->m_x );

  if (!m_s2Key.key().empty()) {
    SG::ReadCondHandle<DMTest::S1> s2 (m_s2Key, ctx);
    ATH_MSG_INFO ("  s2 " << s2->m_x );
  }

  if (!m_rltestKey.key().empty()) {
    SG::ReadCondHandle<AthenaAttributeList> rl (m_rltestKey, ctx);
    ATH_MSG_INFO ("  rl " << (**rl)["xint"]);
  }

  if (!m_tstestKey.key().empty()) {
    SG::ReadCondHandle<AthenaAttributeList> ts (m_tstestKey, ctx);
    ATH_MSG_INFO ("  ts " << (**ts)["xint"]);
  }

  if (!m_s3Key.key().empty()) {
    SG::ReadCondHandle<DMTest::S3> s3 (m_s3Key, ctx);
    ATH_MSG_INFO ("  s3 " << s3->m_x );
  }

  {
    std::string xint = "xint";
    Chrono chrono (&*m_chronoSvc, "spin time");
    int xx = 0;
    for (size_t i = 0; i < m_spins; i++) {
      {
        SG::ReadCondHandle<AthenaAttributeList> attrList (m_attrListKey, ctx);
        // cppcheck-suppress internalAstError;  work around cppcheck 2.0 parse failure
        xx += (**attrList)[xint].template data<int>();
      }
      {
        SG::ReadCondHandle<DMTest::S1> s1 (m_scondKey, ctx);
        xx += s1->m_x;
      }
      {
        SG::ReadCondHandle<DMTest::S1> s2 (m_s2Key, ctx);
        xx += s2->m_x;
      }
    }
  }

  return StatusCode::SUCCESS;
}


} // namespace DMTest
