/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  src/xAODTestRead.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test reading xAOD data.
 */


#include "xAODTestRead.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "DataModelTestDataCommon/CTrigAuxContainer.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"
#include <memory>


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestRead::xAODTestRead (const std::string &name,
                            ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator),
    m_count(0)
{
  declareProperty ("ReadPrefix",  m_readPrefix);
  declareProperty ("WritePrefix", m_writePrefix);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestRead::initialize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestRead::execute()
{
  ++m_count;
  std::cout << m_count << "\n";

  const CVec* vec = 0;
  CHECK( evtStore()->retrieve (vec, m_readPrefix + "cvec") );

  static C::Accessor<int> anInt2 ("anInt2");
  static C::Accessor<int> anInt10 ("anInt10");
  static C::Accessor<int> dInt1 ("dInt1");
  static C::Accessor<int> dInt100 ("dInt100");
  static C::Accessor<int> dInt150 ("dInt150");
  static C::Accessor<int> dInt200 ("dInt200");
  static C::Accessor<int> dInt250 ("dInt250");
  static C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");

  // Ordering of auxid is not reliable.  Sort by name.
  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  std::vector<std::string> names;
  for (SG::auxid_t auxid : vec->getAuxIDs())
    names.push_back (r.getName(auxid));
  std::sort (names.begin(), names.end());
  std::cout << "cvec aux items: ";
  for (const std::string& n : names)
    std::cout << n << " ";
  std::cout << "\n";

  for (const C* c : *vec) {
    std::cout << " anInt1 " << c->anInt()
              << " aFloat: " << c->aFloat()
              << " anInt2: " << anInt2(*c)
              << " dInt1: " << dInt1(*c);
      if (dInt100.isAvailable(*c))
        std::cout << " dInt100: " << dInt100(*c);
      if (dInt150.isAvailable(*c))
        std::cout << " dInt150: " << dInt150(*c);
      if (dInt200.isAvailable(*c))
        std::cout << " dInt200: " << dInt200(*c);
      if (dInt250.isAvailable(*c))
        std::cout << " dInt250: " << dInt250(*c);
      if (anInt10.isAvailable(*c))
        std::cout << " anInt10: " << anInt10(*c);
      if (cEL.isAvailable(*c))
        std::cout << " cEL: " << cEL(*c).dataID()
                  << "[" << cEL(*c).index() << "]";
    std::cout << "\n";
  }

  const C* cinfo = 0;
  CHECK( evtStore()->retrieve (cinfo, m_readPrefix + "cinfo") );

  names.clear();
  for (SG::auxid_t auxid : cinfo->getAuxIDs())
    names.push_back (r.getName(auxid));
  std::sort (names.begin(), names.end());
  std::cout << "cinfo aux items: ";
  for (const std::string& n : names)
    std::cout << n << " ";
  std::cout << "\n";

  std::cout << "cinfo "
            << " anInt1 " << cinfo->anInt()
            << " aFloat: " << cinfo->aFloat()
            << " anInt2: " << anInt2(*cinfo)
            << " dInt1: " << dInt1(*cinfo)
            << " cEL: " << cEL(*cinfo).dataID()
            << "[" << cEL(*cinfo).index() << "]";
  if (dInt100.isAvailable(*cinfo))
    std::cout << " dInt100: " << dInt100(*cinfo);
  if (dInt150.isAvailable(*cinfo))
    std::cout << " dInt150: " << dInt150(*cinfo);
  if (dInt200.isAvailable(*cinfo))
    std::cout << " dInt200: " << dInt200(*cinfo);
  if (dInt250.isAvailable(*cinfo))
    std::cout << " dInt250: " << dInt250(*cinfo);
  if (anInt10.isAvailable(*cinfo))
    std::cout << " anInt10: " << anInt10(*cinfo);
  std::cout << "\n";

  const CVec* ctrig = 0;
  CHECK( evtStore()->retrieve (ctrig, m_readPrefix + "ctrig") );

  names.clear();
  for (SG::auxid_t auxid : ctrig->getAuxIDs())
    names.push_back (r.getName(auxid));
  std::sort (names.begin(), names.end());
  std::cout << "ctrig aux items: ";
  for (const std::string& n : names)
    std::cout << n << " ";
  std::cout << "\n";

  for (const C* c : *ctrig) {
    std::cout << " anInt1 " << c->anInt()
              << " aFloat: " << c->aFloat()
              << " anInt2: " << anInt2(*c)
              << " dInt1: " << dInt1(*c);
      if (dInt100.isAvailable(*c))
        std::cout << " dInt100: " << dInt100(*c);
      if (dInt150.isAvailable(*c))
        std::cout << " dInt150: " << dInt150(*c);
      if (dInt200.isAvailable(*c))
        std::cout << " dInt200: " << dInt200(*c);
      if (anInt10.isAvailable(*c))
        std::cout << " anInt10: " << anInt10(*c);
    std::cout << "\n";
  }

  if (!m_writePrefix.empty()) {
    // Passing this as the third arg of record will make the object const.
    bool LOCKED = false;

    std::unique_ptr<CVec> vecnew (new CVec);
    std::unique_ptr<CAuxContainer> store (new CAuxContainer);
    vecnew->setStore (store.get());
    for (size_t i = 0; i < vec->size(); i++) {
      vecnew->push_back (new C);
      *vecnew->back() = *(*vec)[i];
    }
    CHECK (evtStore()->record (std::move(vecnew), m_writePrefix + "cvec", LOCKED));
    CHECK (evtStore()->record (std::move(store), m_writePrefix + "cvecAux.", LOCKED));

    std::unique_ptr<C> cnew (new C);
    std::unique_ptr<CInfoAuxContainer> info_store (new CInfoAuxContainer);
    cnew->setStore (info_store.get());
    *cnew = *cinfo;

    CHECK (evtStore()->record (std::move(cnew), m_writePrefix + "cinfo", LOCKED));
    CHECK (evtStore()->record (std::move(info_store), m_writePrefix + "cinfoAux.", LOCKED));

    std::unique_ptr<CVec> ctrignew (new CVec);
    std::unique_ptr<CTrigAuxContainer> trig_store (new CTrigAuxContainer);
    ctrignew->setStore (trig_store.get());
    for (size_t i = 0; i < ctrig->size(); i++) {
      ctrignew->push_back (new C);
      *ctrignew->back() = *(*ctrig)[i];
    }
    CHECK (evtStore()->record (std::move(ctrignew), m_writePrefix + "ctrig", LOCKED));
    CHECK (evtStore()->record (std::move(trig_store), m_writePrefix + "ctrigAux.", LOCKED));
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestRead::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

