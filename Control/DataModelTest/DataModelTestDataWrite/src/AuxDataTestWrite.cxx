/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  src/AuxDataTestWrite.cxx
 * @author snyder@bnl.gov
 * @date May 2014
 * @brief Algorithm to test writing @c DataVector with auxiliary data.
 */


#include "AuxDataTestWrite.h"
#include "DataModelTestDataCommon/B.h"
#include "DataModelTestDataCommon/BAux.h"
#include "DataModelTestDataCommon/BAuxStandalone.h"
#include "DataModelTestDataCommon/BAuxVec.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
AuxDataTestWrite::AuxDataTestWrite (const std::string &name,
                                    ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator),
    m_count(0)
{
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode AuxDataTestWrite::initialize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode AuxDataTestWrite::execute()
{
  ++m_count;

  BAuxVec* vec = new BAuxVec;
  SG::AuxStoreInternal* store = new SG::AuxStoreInternal;
  vec->setStore (store);

  static BAux::Accessor<int> anInt1 ("anInt1");
  static BAux::Accessor<float> aFloat1 ("aFloat1");
  static BAux::Accessor<ElementLink<BAuxVec> > anEL ("anEL");
  static BAux::Accessor<DMTest::B> aB ("aB");
  static BAux::Decorator<float> dFloat1 ("dFloat1");

  for (int i=0; i < 10; i++) {
    vec->push_back (new BAux (i + m_count * 1000));
    BAux& b = *vec->back();
    anInt1(b) = m_count*1000 + i+100;
    aFloat1(b) = m_count*1000 + i+200 + 0.5;
    dFloat1(b) = m_count*1000 + i+400 + 0.5;
    aB(b).m_x = m_count*1000 + i+300;
  }
  
  CHECK( evtStore()->record (vec, "bauxvec") );
  CHECK( evtStore()->record (store, "bauxvecAux.") );
  CHECK( evtStore()->setConst (vec) );
  CHECK( evtStore()->setConst (store) );
  
  BAuxStandalone* b = new BAuxStandalone (m_count * 10000);
  b->makePrivateStore();
  anInt1(*b) = m_count*9999;
  aFloat1(*b) = m_count*9999 + 0.5;
  dFloat1(*b) = m_count*9999 + 200 + 0.5;
  aB(*b) = m_count*9999 + 100;
  anEL(*b).toIndexedElement (*vec, m_count % vec->size());
  CHECK( evtStore()->record (b, "b") );
  CHECK( evtStore()->setConst (b) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode AuxDataTestWrite::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

