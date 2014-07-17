/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  src/AuxDataTestTypelessRead.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test reading @c DataVector with auxiliary data,
 *        without compile-time typing of aux data.
 */


#include "AuxDataTestTypelessRead.h"
#include "DataModelTestDataCommon/B.h"
#include "DataModelTestDataCommon/BAux.h"
#include "DataModelTestDataCommon/BAuxStandalone.h"
#include "DataModelTestDataCommon/BAuxVec.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/StrFormat.h"
#include "GaudiKernel/System.h"
#include <map>
#include <memory>


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
AuxDataTestTypelessRead::AuxDataTestTypelessRead (const std::string &name,
                                                  ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator),
    m_count(0)
{
  declareProperty ("WritePrefix", m_writePrefix);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode AuxDataTestTypelessRead::initialize()
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
  else if (ti == &typeid(ElementLink<DMTest::BAuxVec>)) {
    const ElementLink<DMTest::BAuxVec>& el =
      c.getData<ElementLink<DMTest::BAuxVec> > (auxid, i);
    std::cout << el.dataID() << "[" << el.index() << "]; ";
  }
  else if (ti == &typeid(DMTest::B)) {
    std::cout << c.getData<B>(auxid, i).m_x << "; ";
  }
  else
    std::cout << "xxx; ";
}


} // anonymous namespace


/**
 * @brief Algorithm event processing.
 */
StatusCode AuxDataTestTypelessRead::execute()
{
  ++m_count;
  std::cout << m_count << "\n";

  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  const BAuxVec* vec = 0;
  CHECK( evtStore()->retrieve (vec, "bauxvec") );

  // Sort auxids in name order.
  std::map<std::string, SG::auxid_t> auxid_map;
  for (SG::auxid_t auxid : vec->getAuxIDs())
    auxid_map[r.getName(auxid)] = auxid;

  std::cout << "bvec types: ";
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

  const BAux* b = 0;
  CHECK( evtStore()->retrieve (b, "b") );
  const SG::AuxVectorData* cont = b->container();

  std::map<std::string, SG::auxid_t> bauxid_map;
  for (SG::auxid_t auxid : cont->getAuxIDs())
    bauxid_map[r.getName(auxid)] = auxid;

  std::cout << "b types: ";
  for (const auto& m : bauxid_map)
    std::cout << r.getName(m.second) << "/" 
              << System::typeinfoName (*r.getType(m.second)) << " ";
  std::cout << "\n";
  for (const auto& m : bauxid_map)
    dumpAuxItem (m.second, *cont, 0);
  std::cout << "\n";

  if (!m_writePrefix.empty()) {
    // Passing this as the third arg of record will make the object const.
    bool LOCKED = false;

    std::unique_ptr<BAuxVec> vecnew (new BAuxVec);
    std::unique_ptr<SG::AuxStoreInternal> store (new SG::AuxStoreInternal);
    vecnew->setStore (store.get());
    for (size_t i = 0; i < vec->size(); i++) {
      vecnew->push_back (new BAux);
      *vecnew->back() = *(*vec)[i];
    }
    CHECK (evtStore()->record (std::move(vecnew), m_writePrefix + "bauxvec", LOCKED));
    CHECK (evtStore()->record (std::move(store), m_writePrefix + "bauxvecAux.", LOCKED));

    std::unique_ptr<BAuxStandalone> bnew (new BAuxStandalone);
    bnew->makePrivateStore (*b);
    CHECK (evtStore()->record (std::move(bnew), m_writePrefix + "b", LOCKED));
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode AuxDataTestTypelessRead::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

