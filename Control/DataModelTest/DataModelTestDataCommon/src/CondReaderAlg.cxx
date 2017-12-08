/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
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
    m_s2Key ("/DMTest/S2")
{
  declareProperty ("ChronoSvc",    m_chronoSvc);
  declareProperty ("EventInfoKey", m_eventInfoKey = "McEventInfo");
  declareProperty ("AttrListKey",  m_attrListKey);
  declareProperty ("SCondKey",     m_scondKey);
  declareProperty ("S2Key",        m_s2Key);
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
  ATH_CHECK( m_s2Key.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode CondReaderAlg::execute_r (const EventContext& ctx) const
{
  SG::ReadHandle<EventInfo> eventInfo (m_eventInfoKey, ctx);

  ATH_MSG_INFO ("Event " << eventInfo->event_ID()->event_number() <<
                " LBN " << eventInfo->event_ID()->lumi_block());

  SG::ReadCondHandle<AthenaAttributeList> attrList (m_attrListKey, ctx);
  ATH_MSG_INFO ("  xint " << (**attrList)["xint"]);

  SG::ReadCondHandle<DMTest::S1> s1 (m_scondKey, ctx);
  ATH_MSG_INFO ("  scond " << s1->m_x );

  SG::ReadCondHandle<DMTest::S1> s2 (m_s2Key, ctx);
  ATH_MSG_INFO ("  s2 " << s2->m_x );

  {
    Chrono chrono (&*m_chronoSvc, "spin time");
    int xx = 0;
    for (size_t i = 0; i < m_spins; i++) {
      {
        SG::ReadCondHandle<AthenaAttributeList> attrList (m_attrListKey, ctx);
        xx += (**attrList)["xint"].template data<int>();
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
