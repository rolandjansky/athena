/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteCInfoTool.cxx
 * @author snyder@bnl.gov
 * @date Jun 2017
 * @brief Tool to test writing xAOD classes with auxiliary data (cinfo).
 */


#include "xAODTestWriteCInfoTool.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "AthLinks/ElementLink.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param type The tool type.
 * @param name The tool name.
 * @param parent The tool's parent.
 */
xAODTestWriteCInfoTool::xAODTestWriteCInfoTool (const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent)

  : base_class (type, name, parent),
    m_cvecKey ("cvec"),
    m_cinfoKey ("cinfo")
{
  declareProperty ("CVecKey", m_cvecKey);
  declareProperty ("CInfoKey", m_cinfoKey);
}
  

/**
 * @brief Tool initialization; called at the beginning of the job.
 */
StatusCode xAODTestWriteCInfoTool::initialize()
{
  ATH_CHECK( m_cvecKey.initialize() );
  ATH_CHECK( m_cinfoKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Tool event processing.
 */
StatusCode xAODTestWriteCInfoTool::doit (const EventContext& ctx) const
{
  unsigned int count = ctx.eventID().event_number() + 1;

  SG::ReadHandle<DMTest::CVec> cvec (m_cvecKey, ctx);

  auto cinfo = std::make_unique<DMTest::C>();
  auto info_store = std::make_unique<DMTest::CInfoAuxContainer>();
  cinfo->setStore (info_store.get());

  static C::Accessor<int> anInt2 ("anInt2");
  static C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");

  cinfo->setAnInt (count * 1000);
  cinfo->setAFloat ((float)count * 0.1);
  anInt2(*cinfo) = count * 2000;

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


} // namespace DMTest

