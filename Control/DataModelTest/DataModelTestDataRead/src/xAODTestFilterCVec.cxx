/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  DataModelTestDataRead/src/xAODTestFilterCVec.cxx
 * @author snyder@bnl.gov
 * @date Sep 2016
 * @brief Algorithm to test filtering xAOD data.
 */


#include "xAODTestFilterCVec.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include <memory>


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestFilterCVec::xAODTestFilterCVec (const std::string &name,
                                        ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_cvecKey ("cvec_renamed"),
    m_writeKey ("cvec")
{
  declareProperty ("CVecKey", m_cvecKey);
  declareProperty ("WriteKey", m_writeKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestFilterCVec::initialize()
{
  ATH_CHECK( m_cvecKey.initialize() );
  if (!m_writeKey.key().empty())
    ATH_CHECK( m_writeKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestFilterCVec::execute_r (const EventContext& ctx) const
{
  SG::ReadHandle<DMTest::CVec> cvec (m_cvecKey, ctx);
  const static C::Accessor<ElementLink<DMTest::CVec> > cEL ("cEL");


  auto vecnew = std::make_unique<CVec>();
  auto store = std::make_unique<CAuxContainer>();
  vecnew->setStore (store.get());
  for (size_t i = 0; i < cvec->size(); i+=2) {
    vecnew->push_back (new C);
    *vecnew->back() = *(*cvec)[i];
  }

  size_t i = 0;
  for (C* c : *vecnew) {
    cEL(*c).toIndexedElement (*vecnew, vecnew->size()-1-i);
    ++i;
  }
  SG::WriteHandle<DMTest::CVec> writevec (m_writeKey, ctx);
  ATH_CHECK( writevec.record(std::move(vecnew), std::move(store)) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestFilterCVec::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

