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
//#include "AthContainers/PackedElement.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/StrFormat.h"
#include "GaudiKernel/System.h"
#include <map>
#include <memory>
#include <sstream>


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


void dumpAuxItem (std::ostream& ost, SG::auxid_t auxid, const SG::AuxVectorData& c, size_t i)
{
  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  const std::type_info* ti = r.getType(auxid);
  std::string head = r.getName(auxid) + ": ";
  if (ti == &typeid(int))
    ost << head << c.getData<int> (auxid, i) << "; ";
  else if (ti == &typeid(float))
    ost << head << CxxUtils::strformat ("%.3f", c.getData<float> (auxid, i)) << "; ";
  else if (ti == &typeid(ElementLink<DMTest::BAuxVec>)) {
    const ElementLink<DMTest::BAuxVec>& el =
      c.getData<ElementLink<DMTest::BAuxVec> > (auxid, i);
    ost << head << el.dataID() << "[" << el.index() << "]; ";
  }
  else if (ti == &typeid(DMTest::B)) {
    ost << head << c.getData<B>(auxid, i).m_x << "; ";
  }
#if 0
  else if (ti == &typeid(SG::PackedElement<unsigned int>))
    ost << head << c.getData<SG::PackedElement<unsigned int> > (auxid, i) << "; ";
#endif
  else if (ti == &typeid(unsigned int))
    ost << head << c.getData<unsigned int> (auxid, i) << "; ";
#if 0
  else if (ti == &typeid(SG::PackedElement<float>))
    ost << head << c.getData<SG::PackedElement<float> > (auxid, i) << "; ";
  else if (ti == &typeid(SG::PackedElement<std::vector<unsigned int> >)) {
    ost << "\n    " << head << "[";
    for (auto ii : c.getData<SG::PackedElement<std::vector<unsigned int> > > (auxid, i))
      ost << ii << " ";
    ost << "]; ";
  }
  else if (ti == &typeid(SG::PackedElement<std::vector<int> >)) {
    ost << "\n    " << head << "[";
    for (auto ii : c.getData<SG::PackedElement<std::vector<int> > > (auxid, i))
      ost << ii << " ";
    ost << "]; ";
  }
  else if (ti == &typeid(SG::PackedElement<std::vector<float> >)) {
    ost << "\n    " << head << "[";
    for (auto ii : c.getData<SG::PackedElement<std::vector<float> > > (auxid, i))
      ost << CxxUtils::strformat ("%.3f", ii) << " ";
    ost << "]; ";
  }
#endif
  else if (ti == &typeid(std::vector<int>)) {
    ost << "\n    " << head << "[";
    for (auto ii : c.getData<std::vector<int> > (auxid, i))
      ost << ii << " ";
    ost << "]; ";
  }
  else if (ti == &typeid(std::vector<float>) ||
           strcmp (ti->name(), typeid(std::vector<float>).name()) == 0)
  {
    ost << "\n    " << head << "[";
    for (auto ii : c.getData<std::vector<float> > (auxid, i))
      ost << CxxUtils::strformat ("%.3f", ii) << " ";
    ost << "]; ";
  }
  else
    ost << head << "xxx " << ti->name() << "; ";
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
    std::ostringstream ss;
    ss << "  ";
    for (const auto& m : auxid_map)
      dumpAuxItem (ss, m.second, *vec, i);
    std::cout << ss.str() << "\n";
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
  std::ostringstream ss;
  for (const auto& m : bauxid_map)
    dumpAuxItem (ss, m.second, *cont, 0);
  std::cout << ss.str() << "\n";

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

