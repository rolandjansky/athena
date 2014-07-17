/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  src/xAODTestTypelessRead.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test reading @c DataVector with auxiliary data,
 *        without compile-time typing of aux data.
 */


#include "xAODTestTypelessRead.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "DataModelTestDataCommon/CTrigAuxContainer.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/StrFormat.h"
#include "GaudiKernel/System.h"
#include <memory>


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestTypelessRead::xAODTestTypelessRead (const std::string &name,
                                            ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator),
    m_count(0)
{
  declareProperty ("WritePrefix", m_writePrefix);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestTypelessRead::initialize()
{
  return StatusCode::SUCCESS;
}


namespace {


void dumpAuxItem (SG::auxid_t auxid, const SG::AuxVectorData& c, size_t i)
{
  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  const std::type_info* ti = r.getType(auxid);
  std::cout << r.getName(auxid) << ": ";
  if (ti == &typeid(int))
    std::cout << c.getData<int> (auxid, i) << "; ";
  else if (ti == &typeid(float))
    std::cout << CxxUtils::strformat ("%.1f", c.getData<float> (auxid, i)) << "; ";
  else if (ti == &typeid(ElementLink<DMTest::CVec>)) {
    const ElementLink<DMTest::CVec>& el =
      c.getData<ElementLink<DMTest::CVec> > (auxid, i);
    std::cout << el.dataID() << "[" << el.index() << "]; ";
  }
  else
    std::cout << "xxx; ";
}


} // anonymous namespace


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestTypelessRead::execute()
{
  ++m_count;
  std::cout << m_count << "\n";

  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  const CVec* vec = 0;
  CHECK( evtStore()->retrieve (vec, "cvec") );

  // Sort auxids in name order.
  std::map<std::string, SG::auxid_t> auxid_map;
  for (SG::auxid_t auxid : vec->getAuxIDs())
    auxid_map[r.getName(auxid)] = auxid;

  std::cout << "cvec types: ";
  for (const auto& m : auxid_map)
    std::cout << r.getName(m.second) << "/" 
              << System::typeinfoName (*r.getType(m.second)) << " ";
  std::cout << "\n";
  for (size_t i = 0; i < vec->size(); i++) {
    std::cout << "  ";
    for (const auto& m : auxid_map)
      dumpAuxItem (m.second, *vec, i);
    std::cout << "\n";
  }

  const C* c = 0;
  CHECK( evtStore()->retrieve (c, "cinfo") );
  const SG::AuxVectorData* cont = c->container();

  std::map<std::string, SG::auxid_t> cauxid_map;
  for (SG::auxid_t auxid : cont->getAuxIDs())
    cauxid_map[r.getName(auxid)] = auxid;

  std::cout << "cinfo types: ";
  for (const auto& m : cauxid_map)
    std::cout << r.getName(m.second) << "/" 
              << System::typeinfoName (*r.getType(m.second)) << " ";
  std::cout << "\n";
  for (const auto& m : cauxid_map)
    dumpAuxItem (m.second, *cont, 0);
  std::cout << "\n";

  const CVec* ctrig = 0;
  CHECK( evtStore()->retrieve (ctrig, "ctrig") );

  // Sort auxids in name order.
  std::map<std::string, SG::auxid_t> trig_auxid_map;
  for (SG::auxid_t auxid : ctrig->getAuxIDs())
    trig_auxid_map[r.getName(auxid)] = auxid;

  std::cout << "ctrig types: ";
  for (const auto& m : trig_auxid_map)
    std::cout << r.getName(m.second) << "/" 
              << System::typeinfoName (*r.getType(m.second)) << " ";
  std::cout << "\n";
  for (size_t i = 0; i < ctrig->size(); i++) {
    std::cout << "  ";
    for (const auto& m : trig_auxid_map)
      dumpAuxItem (m.second, *ctrig, i);
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
    *cnew = *c;

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
StatusCode xAODTestTypelessRead::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

