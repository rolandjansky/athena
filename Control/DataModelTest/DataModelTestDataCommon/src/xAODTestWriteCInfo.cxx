/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCOMMON/src/xAODTestWriteCInfo.cxx
 * @author snyder@bnl.gov
 * @date Apr 2016
 * @brief Algorithm to test writing xAOD classes with auxiliary data (cinfo).
 */


#include "xAODTestWriteCInfo.h"
#include "EventInfo/EventID.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "AthLinks/ElementLink.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestWriteCInfo::xAODTestWriteCInfo (const std::string &name,
                                        ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_eventInfoKey ("McEventInfo"),
    m_cvecKey ("cvec"),
    m_cinfoKey ("cinfo")
{
  declareProperty ("Offset", m_offset = 0);
  declareProperty ("EventInfoKey", m_eventInfoKey);
  declareProperty ("CVecKey", m_cvecKey);
  declareProperty ("CInfoKey", m_cinfoKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWriteCInfo::initialize()
{
  ATH_CHECK( m_eventInfoKey.initialize() );
  ATH_CHECK( m_cvecKey.initialize() );
  ATH_CHECK( m_cinfoKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestWriteCInfo::execute_r (const EventContext& ctx) const
{
  //SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, ctx);
  //unsigned int count = eventInfo->eventNumber()  + 1;
  SG::ReadHandle<EventInfo> eventInfo (m_eventInfoKey, ctx);
  unsigned int count = eventInfo->event_ID()->event_number() + 1;

  SG::ReadHandle<DMTest::CVec> cvec (m_cvecKey, ctx);

  auto cinfo = std::make_unique<DMTest::C>();
  auto info_store = std::make_unique<DMTest::CInfoAuxContainer>();
  cinfo->setStore (info_store.get());

  static C::Accessor<int> anInt2 ("anInt2");
  static C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");

  cinfo->setAnInt (count * 1000 + m_offset);
  cinfo->setAFloat ((float)count * 0.1 + m_offset);
  anInt2(*cinfo) = count * 2000 + m_offset;

  if (!cvec->empty()) {
    cEL(*cinfo).toIndexedElement (*cvec, count % cvec->size());
  }

  SG::WriteHandle<DMTest::C> cinfoH (m_cinfoKey, ctx);
  std::unique_ptr<const DMTest::C> cinfo_c (std::move (cinfo));
  std::unique_ptr<const DMTest::CInfoAuxContainer> info_store_c (std::move (info_store));
  if (cinfoH.put (std::move(cinfo_c), std::move(info_store_c)) == nullptr) {
    ATH_MSG_ERROR ("Can't record CInfo");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestWriteCInfo::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

