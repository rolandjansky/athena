/*
 *  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataRead/src/xAODTestReadCInfo.h
 * @author snyder@bnl.gov
 * @date Sep, 2017
 * @brief Algorithm to test reading xAOD cinfo data.
 */


#include "xAODTestReadCInfo.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"
#include <sstream>


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestReadCInfo::xAODTestReadCInfo (const std::string &name,
                                      ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator)
{
  declareProperty ("CInfoKey", m_cinfoKey = "cinfo");
  declareProperty ("WriteKey", m_writeKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestReadCInfo::initialize()
{
  ATH_CHECK( m_cinfoKey.initialize() );
  if (!m_writeKey.key().empty())
    ATH_CHECK( m_writeKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestReadCInfo::execute (const EventContext& ctx) const
{
  static const C::Accessor<int> anInt2 ("anInt2");
  static const C::Accessor<int> anInt10 ("anInt10");
  static const C::Accessor<int> dInt1 ("dInt1");
  static const C::Accessor<unsigned int> dpInt1 ("dpInt1");
  static const C::Accessor<std::vector<float> > dpvFloat ("dpvFloat");
  static const C::Accessor<int> dInt100 ("dInt100");
  static const C::Accessor<int> dInt150 ("dInt150");
  static const C::Accessor<int> dInt200 ("dInt200");
  static const C::Accessor<int> dInt250 ("dInt250");
  static const C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");

  // Ordering of auxid is not reliable.  Sort by name.
  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  std::vector<std::string> names;

  SG::ReadHandle<DMTest::C> cinfo (m_cinfoKey, ctx);

  names.clear();
  for (SG::auxid_t auxid : cinfo->getAuxIDs())
    names.push_back (r.getName(auxid));
  std::sort (names.begin(), names.end());
  std::ostringstream ost1;
  ost1 << "cinfo aux items: ";
  for (const std::string& n : names)
    ost1 << n << " ";
  ATH_MSG_INFO (ost1.str());

  std::ostringstream ost2;
  ost2 << "cinfo "
       << " anInt1 " << cinfo->anInt()
       << " aFloat: " << cinfo->aFloat()
       << " anInt2: " << anInt2(*cinfo)
       << " dInt1: " << dInt1(*cinfo)
       << " cEL: " << cEL(*cinfo).dataID()
       << "[" << cEL(*cinfo).index() << "]";
  if (dInt100.isAvailable(*cinfo))
    ost2 << " dInt100: " << dInt100(*cinfo);
  if (dInt150.isAvailable(*cinfo))
    ost2 << " dInt150: " << dInt150(*cinfo);
  if (dInt200.isAvailable(*cinfo))
    ost2 << " dInt200: " << dInt200(*cinfo);
  if (dInt250.isAvailable(*cinfo))
    ost2 << " dInt250: " << dInt250(*cinfo);
  if (anInt10.isAvailable(*cinfo))
    ost2 << " anInt10: " << anInt10(*cinfo);
  ATH_MSG_INFO (ost2.str());

  if (!m_writeKey.key().empty()) {
    auto cnew = std::make_unique<DMTest::C>();
    auto info_store = std::make_unique<DMTest::CInfoAuxContainer>();
    cnew->setStore (info_store.get());
    *cnew = *cinfo;

    SG::WriteHandle<DMTest::C> writevec (m_writeKey, ctx);
    ATH_CHECK( writevec.record(std::move(cnew), std::move(info_store)) );
  }

  return StatusCode::SUCCESS;
}


} // namespace DMTest

