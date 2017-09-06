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
#include "DataModelTestDataCommon/CVecWithData.h"
#include "DataModelTestDataCommon/CView.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "DataModelTestDataCommon/CTrigAuxContainer.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "DataModelTestDataRead/H.h"
#include "DataModelTestDataRead/HAuxContainer.h"
#include "DataModelTestDataRead/HVec.h"
#include "DataModelTestDataRead/HView.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/StrFormat.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/System.h"
#include <memory>
#include <sstream>


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


void dumpAuxItem (std::ostream& ost,
                  SG::auxid_t auxid,
                  const SG::AuxVectorData& c, size_t i)
{
  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  const std::type_info* ti = r.getType(auxid);
  std::string head = r.getName(auxid) + ": ";
  if (ti == &typeid(int))
    ost << head << c.getData<int> (auxid, i) << "; ";
  else if (ti == &typeid(unsigned int))
    ost << head << c.getData<unsigned int> (auxid, i) << "; ";
  else if (ti == &typeid(float))
    ost << head << CxxUtils::strformat ("%.3f", c.getData<float> (auxid, i)) << "; ";
  else if (ti == &typeid(ElementLink<DMTest::CVec>)) {
    const ElementLink<DMTest::CVec>& el =
      c.getData<ElementLink<DMTest::CVec> > (auxid, i);
    ost << head << el.dataID() << "[" << el.index() << "]; ";
  }
#if 0
  else if (ti == &typeid(SG::PackedElement<unsigned int>))
    ost << head << c.getData<SG::PackedElement<unsigned int> > (auxid, i) << "; ";
  else if (ti == &typeid(SG::PackedElement<float>))
    ost << head << c.getData<SG::PackedElement<float> > (auxid, i) << "; ";
#endif
  else if (ti == &typeid(std::vector<unsigned int>)) {
    ost << "\n    " << head << "[";
    for (auto ii : c.getData<std::vector<unsigned int> > (auxid, i))
      ost << ii << " ";
    ost << "]; ";
  }
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
#if 0
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
  else
    ost << head << "xxx " << ti->name() << "; ";
}


std::map<std::string, SG::auxid_t> get_map (const SG::AuxVectorData* vec)
{
  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  // Sort auxids in name order.
  std::map<std::string, SG::auxid_t> auxid_map;
  for (SG::auxid_t auxid : vec->getAuxIDs())
    auxid_map[r.getName(auxid)] = auxid;

  return auxid_map;
}


std::map<std::string, SG::auxid_t> get_map (const SG::AuxElement* elt)
{
  return get_map (elt->container());
}


void dumpelt (std::ostream& ost,
              const SG::AuxVectorData* cont,
              size_t index,
              const std::map<std::string, SG::auxid_t>& auxid_map)
{
  for (const auto& m : auxid_map)
    dumpAuxItem (ost, m.second, *cont, index);
  ost << "\n";
}


template <class OBJ>
void dumpobj (std::ostream& ost,
              const OBJ* obj,
              const std::map<std::string, SG::auxid_t>& auxid_map)
{
  for (size_t i = 0; i < obj->size(); i++) {
    ost << "  ";
    if (!obj->hasStore()) {
      // Handle view container.
      const auto* elt = obj->at(i);
      dumpelt (ost, elt->container(), elt->index(), auxid_map);
    }
    else
      dumpelt (ost, obj, i, auxid_map);
  }
}


void dumpobj (std::ostream& ost,
              const DMTest::C* obj,
              const std::map<std::string, SG::auxid_t>& auxid_map)
{
  const SG::AuxVectorData* cont = obj->container();
  dumpelt (ost, cont, 0, auxid_map);
}


void copy (DMTest::CVec& to, const DMTest::CVec& from)
{
  for (size_t i = 0; i < from.size(); i++) {
    to.push_back (new C);
    *to.back() = *from[i];
  }
}


void copy (DMTest::HVec& to, const DMTest::HVec& from)
{
  for (size_t i = 0; i < from.size(); i++) {
    to.push_back (new H);
    *to.back() = *from[i];
  }
}


void copy (DMTest::CVecWithData& to, const DMTest::CVecWithData& from)
{
  to.meta1 = from.meta1;
  copy (static_cast<DMTest::CVec&>(to),
        static_cast<const DMTest::CVec&>(from));
}


void copy (DMTest::C& to, const DMTest::C& from)
{
  to = from;
}


} // anonymous namespace


template <class OBJ, class AUX>
StatusCode
xAODTestTypelessRead::testit (const char* key)
{
  const OBJ* obj = nullptr;
  CHECK( evtStore()->retrieve (obj, key) );

  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  std::map<std::string, SG::auxid_t> auxid_map = get_map (obj);
  std::cout << key << " types: ";
  for (const auto& m : auxid_map)
    std::cout << r.getName(m.second) << "/" 
              << System::typeinfoName (*r.getType(m.second)) << " ";
  std::cout << "\n";

  std::ostringstream ost;
  dumpobj (ost, obj, auxid_map);
  std::cout << ost.str();

  if (!m_writePrefix.empty()) {
    // Passing this as the third arg of record will make the object const.
    bool LOCKED = false;

    auto objnew = CxxUtils::make_unique<OBJ>();
    auto store  = CxxUtils::make_unique<AUX>();
    objnew->setStore (store.get());
    copy (*objnew, *obj);
    CHECK (evtStore()->record (std::move(objnew), m_writePrefix + key, LOCKED));
    CHECK (evtStore()->record (std::move(store), m_writePrefix + key + "Aux.", LOCKED));
  }
  return StatusCode::SUCCESS;
}


template <class OBJ>
StatusCode
xAODTestTypelessRead::testit_view (const char* key)
{
  const OBJ* obj = nullptr;
  CHECK( evtStore()->retrieve (obj, key) );

  if (!obj->empty()) {
    std::map<std::string, SG::auxid_t> auxid_map = get_map (obj->front());
    std::cout << key << "\n";
    std::ostringstream ost;
    dumpobj (ost, obj, auxid_map);
    std::cout << ost.str();
  }

  if (!m_writePrefix.empty()) {
    CHECK (evtStore()->record (CxxUtils::make_unique<OBJ>(*obj),
                               m_writePrefix + key, false));
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestTypelessRead::execute()
{
  ++m_count;
  std::cout << m_count << "\n";

  CHECK(( testit<CVec, CAuxContainer>     ("cvec") ));
  CHECK(( testit<C,    CInfoAuxContainer> ("cinfo") ));
  CHECK(( testit<CVec, CTrigAuxContainer> ("ctrig") ));
  CHECK(( testit<CVecWithData, CAuxContainer> ("cvecWD") ));
  CHECK(( testit_view<CView> ("cview") ));
  CHECK(( testit<HVec, HAuxContainer>     ("hvec") ));
  CHECK(( testit_view<HView> ("hview") ));

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

