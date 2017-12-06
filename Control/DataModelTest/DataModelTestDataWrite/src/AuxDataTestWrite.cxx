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
#include "AthContainersInterfaces/AuxDataOption.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"


#define CHECK_OPTION(ret)                       \
  do {                                          \
    if (!ret) {                                 \
      ATH_MSG_ERROR("setOption failed");        \
      return StatusCode::FAILURE;               \
    }                                           \
  } while(0)


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

  auto vec = std::make_unique<BAuxVec>();
  auto store = std::make_unique<SG::AuxStoreInternal>();
  vec->setStore (store.get());

  static BAux::Accessor<int> anInt1 ("anInt1");
  static BAux::Accessor<float> aFloat1 ("aFloat1");
  static BAux::Accessor<ElementLink<BAuxVec> > anEL ("anEL");
  static BAux::Accessor<DMTest::B> aB ("aB");
  static BAux::Decorator<float> dFloat1 ("dFloat1");

  static BAux::Accessor<unsigned int> pint ("pint");
  static BAux::Accessor<float> pfloat ("pfloat");
  static BAux::Accessor<std::vector<int> > pvint ("pvint");
  static BAux::Accessor<std::vector<float> > pvfloat ("pvfloat");

  CHECK_OPTION( vec->setOption ("pint", "nbits", 3) );
  CHECK_OPTION( vec->setOption ("pvfloat", SG::AuxDataOption ("nbits", 13)) );
  CHECK_OPTION( vec->setOption ("pvfloat", SG::AuxDataOption ("nmantissa",12)));

  for (int i=0; i < 10; i++) {
    vec->push_back (new BAux (i + m_count * 1000));
    BAux& b = *vec->back();
    anInt1(b) = m_count*1000 + i+100;
    aFloat1(b) = m_count*1000 + i+200 + 0.5;
    dFloat1(b) = m_count*1000 + i+400 + 0.5;
    aB(b).m_x = m_count*1000 + i+300;

    pint(b) = (m_count*2 + i) % 3;
    pfloat(b) = i + (float)m_count * 0.01;

    std::vector<int> pvi;
    for (int j=0; j<i; j++)
      pvi.push_back (j + i*10 + m_count*100 - 500);
    pvint(b) = pvi;

    std::vector<float> pvf;
    for (int j=0; j<i; j++)
      pvf.push_back ((float)j*0.1 + (float)i*0.01 + (float)m_count*0.001 - 0.5);
    pvfloat(b) = std::move (pvf);
  }

  CHECK_OPTION( vec->setOption ("pfloat", SG::AuxDataOption ("nbits", 17)) );
  CHECK_OPTION( vec->setOption ("pfloat", SG::AuxDataOption ("signed", 0)) );
  CHECK_OPTION( vec->setOption ("pfloat", SG::AuxDataOption ("nmantissa", 17)));
  CHECK_OPTION( vec->setOption ("pfloat", SG::AuxDataOption ("scale", 10)) );

  CHECK_OPTION( vec->setOption ("pvint", SG::AuxDataOption ("nbits", 13)) );

  BAuxVec& rvec = *vec;
  CHECK( evtStore()->record (std::move(vec), "bauxvec", false) );
  CHECK( evtStore()->record (std::move(store), "bauxvecAux.", false) );
  
  auto b = std::make_unique<BAuxStandalone> (m_count * 10000);
  b->makePrivateStore();
  anInt1(*b) = m_count*9999;
  aFloat1(*b) = m_count*9999 + 0.5;
  dFloat1(*b) = m_count*9999 + 200 + 0.5;
  aB(*b) = m_count*9999 + 100;
  anEL(*b).toIndexedElement (rvec, m_count % rvec.size());
  CHECK( evtStore()->record (std::move(b), "b", false) );

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

