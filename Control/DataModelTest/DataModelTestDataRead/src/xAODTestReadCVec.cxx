/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  DataModelTestDataRead/src/xAODTestReadCVec.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test reading xAOD data (CVec).
 */


#include "xAODTestReadCVec.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/StrFormat.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/System.h"
#include <memory>


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestReadCVec::xAODTestReadCVec (const std::string &name,
                                    ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_cvecKey ("cvec"),
    m_writeKey ("")
{
  declareProperty ("CVecKey", m_cvecKey);
  declareProperty ("WriteKey", m_writeKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestReadCVec::initialize()
{
  ATH_CHECK( m_cvecKey.initialize() );
  if (!m_writeKey.key().empty())
    ATH_CHECK( m_writeKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestReadCVec::execute_r (const EventContext& ctx) const
{
  SG::ReadHandle<DMTest::CVec> cvec (m_cvecKey, ctx);

  const static C::Accessor<int> anInt2 ("anInt2");
  const static C::Accessor<int> anInt10 ("anInt10");
  const static C::Accessor<int> dInt1 ("dInt1");
  const static C::Accessor<unsigned int> dpInt1 ("dpInt1");
  const static C::Accessor<std::vector<float> > dpvFloat ("dpvFloat");
  const static C::Accessor<int> dInt100 ("dInt100");
  const static C::Accessor<int> dInt150 ("dInt150");
  const static C::Accessor<int> dInt200 ("dInt200");
  const static C::Accessor<int> dInt250 ("dInt250");
  const static C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");

  // Ordering of auxid is not reliable.  Sort by name.
  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  std::vector<std::string> names;
  for (SG::auxid_t auxid : cvec->getAuxIDs())
    names.push_back (r.getName(auxid));
  std::sort (names.begin(), names.end());
  std::cout << "cvec aux items: ";
  for (const std::string& n : names)
    std::cout << n << " ";
  std::cout << "\n";
  std::cout << "Type of aux store: "
            << System::typeinfoName (typeid (*cvec->getConstStore())) << "\n";

  for (const C* c : *cvec) {
    std::cout << " anInt1 " << c->anInt()
              << " aFloat: " << c->aFloat()
              << " pInt: " << c->pInt()
              << " pFloat: " << CxxUtils::strformat ("%.2f", c->pFloat());
    if (anInt2.isAvailable(*c))
      std::cout << " anInt2: " << anInt2(*c);
    if (dInt1.isAvailable(*c))
      std::cout << " dInt1: " << dInt1(*c);
    if (dpInt1.isAvailable(*c))
      std::cout << " dpInt1: " << dpInt1(*c);
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
    
    {
      const std::vector<int>& pvi = c->pvInt();
      std::cout << "  pvInt: [";
      for (auto ii : pvi)
        std::cout << ii << " ";
      std::cout << "]\n";
    }

    {
      const std::vector<float>& pvf = c->pvFloat();
      std::cout << "  pvFloat: [";
      for (auto ii : pvf)
        std::cout << CxxUtils::strformat ("%.3f", ii) << " ";
      std::cout << "]\n";
    }

    if (dpvFloat.isAvailable(*c))
    {
      const std::vector<float>& pvf = dpvFloat(*c);
      std::cout << "  dpvFloat: [";
      for (auto ii : pvf)
        std::cout << CxxUtils::strformat ("%.3f", ii) << " ";
      std::cout << "]\n";
    }
  }

  if (!m_writeKey.key().empty()) {
    auto vecnew = CxxUtils::make_unique<CVec>();
    auto store = CxxUtils::make_unique<CAuxContainer>();
    vecnew->setStore (store.get());
    for (size_t i = 0; i < cvec->size(); i++) {
      vecnew->push_back (new C);
      *vecnew->back() = *(*cvec)[i];
    }
    SG::WriteHandle<DMTest::CVec> writevec (m_writeKey, ctx);
    ATH_CHECK( writevec.record(std::move(vecnew), std::move(store)) );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestReadCVec::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

