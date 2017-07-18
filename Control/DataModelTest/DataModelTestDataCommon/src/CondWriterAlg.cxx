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
#include "EventInfo/EventID.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaKernel/IOVTime.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param pSvcLocator The service locator.
 */
CondWriterAlg::CondWriterAlg (const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_regSvc ("IOVRegistrationSvc", name)
{
  declareProperty ("EventInfoKey", m_eventInfoKey = "McEventInfo");
  declareProperty ("AttrListKey",  m_attrListKey = "/DMTest/TestAttrList");
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode CondWriterAlg::initialize()
{
  ATH_CHECK( m_regSvc.retrieve() );
  ATH_CHECK( m_eventInfoKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode CondWriterAlg::execute_r (const EventContext& ctx) const
{
  SG::ReadHandle<EventInfo> eventInfo (m_eventInfoKey, ctx);
  unsigned int count = eventInfo->event_ID()->event_number() + 1;

  auto attrList = std::make_unique<AthenaAttributeList>();
  attrList->extend ("xint", "int");
  (*attrList)["xint"].setValue(static_cast<int> (count*10));

  ATH_CHECK( detStore()->overwrite (std::move (attrList), m_attrListKey) );

  ATH_CHECK( m_regSvc->registerIOV ("AthenaAttributeList",
                                    m_attrListKey,
                                    "tag AttrList_noTag",
                                    IOVTime::MINRUN, IOVTime::MAXRUN,
                                    count-1, count-1) );

  return StatusCode::SUCCESS;
}


} // namespace DMTest
