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


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param pSvcLocator The service locator.
 */
CondReaderAlg::CondReaderAlg (const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_attrListKey ("/DMTest/TestAttrList")
{
  declareProperty ("EventInfoKey", m_eventInfoKey = "McEventInfo");
  declareProperty ("AttrListKey",  m_attrListKey);
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode CondReaderAlg::initialize()
{
  ATH_CHECK( m_eventInfoKey.initialize() );
  ATH_CHECK( m_attrListKey.initialize() );
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

  return StatusCode::SUCCESS;
}


} // namespace DMTest
