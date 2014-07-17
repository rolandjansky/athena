/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DMTestRead.cxx,v 1.6 2008-11-21 06:29:06 ssnyder Exp $
/**
 * @file  src/DMTestRead.cxx
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Algorithm to test reading @c DataVector data.
 *
 *        We read information using all four types,
 *        @c BVec, @c DVec, @c BDer, @c DDer.
 *        When we run, we have the inheritance relationship set up
 *        between @c BVec and @c DVec.
 */

#undef NDEBUG

#include "DMTestRead.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"
#include "DataModelTestDataRead/BVec.h"
#include "DataModelTestDataRead/BDer.h"
#include "DataModelTestDataRead/DVec.h"
#include "DataModelTestDataRead/DDer.h"
#include "DataModelTestDataRead/ELVec.h"
#include "AthContainers/ClassName.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/errorcheck.h"
#include <iostream>
#include <cassert>


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
DMTestRead::DMTestRead (const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm (name, pSvcLocator)
{
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode DMTestRead::initialize()
{
  return StatusCode::SUCCESS;
}


namespace {


/**
 * @brief Print out one of our test objects of type @c VEC from storegate.
 * @param sg The @c StoreGateSvc object.
 * @param key The storegate key to use.
 */
template <class VEC>
StatusCode print_vec (StoreGateSvc* sg,
                      const std::string& key,
                      const std::string& context)
{
  if (!sg->contains<VEC> (key))
  {
    std::cout << key << " not in SG; ignored.\n";
    return StatusCode::SUCCESS;
  }

  const VEC* vec;
  CHECK_WITH_CONTEXT( sg->retrieve (vec, key), context );
  std::cout << key << " as " << ClassName<VEC>::name() << ": ";
  for (unsigned i=0; i < vec->size(); i++)
    std::cout << (*vec)[i]->m_x << " ";
  std::cout << "\n";

  return StatusCode::SUCCESS;
}


StatusCode print_elvec (StoreGateSvc* sg,
                        const std::string& key,
                        const std::string& context)
{
  const ELVec* vec;
  CHECK_WITH_CONTEXT( sg->retrieve (vec, key), context );
  ELVec* mvec = const_cast<ELVec*> (vec);
  std::cout << key << ": ";
  for (size_t i = 0; i < vec->m_el.size(); i++) {
    const DMTest::B* b = *vec->m_el[i];
    mvec->m_el[i].toPersistent();
    std::cout << b->m_x << " ";
  }
  std::cout << "\n";
  return StatusCode::SUCCESS;
}


StatusCode remap_test (StoreGateSvc* sg, MsgStream& log)
{
  const ELVec* vec;
  CHECK_WITH_CONTEXT( sg->retrieve (vec, "elv_remap"), "remap_test" );
  ELVec* mvec = const_cast<ELVec*> (vec);

  ElementLinkCnv_p3<ElementLink<BVec> > elcnv;
  mvec->m_el2.resize (mvec->m_el2_p.size());
  for (size_t i=0; i < mvec->m_el2_p.size(); i++)
    elcnv.persToTrans (&mvec->m_el2_p[i], &mvec->m_el2[i], log);

  std::cout << "elv_remap: ";
  for (size_t i = 0; i < vec->m_el2.size(); i++) {
    const DMTest::B* b = *vec->m_el2[i];
    std::cout << b->m_x << " ";
  }
  std::cout << "\n";

  ElementLinkVectorCnv_p1<ElementLinkVector<BVec> > elvcnv;
  elvcnv.persToTrans (&vec->m_elv2_p, &const_cast<ELVec*>(vec)->m_elv2, log);

  std::cout << "elv_remap v2: ";
  for (size_t i = 0; i < vec->m_elv2.size(); i++) {
    const DMTest::B* b = *vec->m_elv2[i];
    std::cout << b->m_x << " ";
  }
  std::cout << "\n";

  DataLinkCnv_p1<DataLink<BVec> > dlcnv;
  mvec->m_dl2.resize (mvec->m_dl2_p.size());
  for (size_t i=0; i < mvec->m_dl2_p.size(); i++)
    dlcnv.persToTrans (&mvec->m_dl2_p[i], &mvec->m_dl2[i], log);

  const BVec* b3;
  CHECK_WITH_CONTEXT( sg->retrieve (b3, "b3"), "remap_test" );
  assert (vec->m_dl2[0].cptr() == b3);
  assert (vec->m_dl2[1].cptr() == b3);

  return StatusCode::SUCCESS;
}


} // anonymous namespace


/**
 * @brief Algorithm event processing.
 */
StatusCode DMTestRead::execute()
{
  MsgStream log(msgSvc(), name());

  StoreGateSvc* sg = &*evtStore();

  // Test reading our four types.
  CHECK( print_vec<BVec> (sg, "bvec", name()) );
  CHECK( print_vec<BDer> (sg, "bder", name()) );
  CHECK( print_vec<DVec> (sg, "dvec", name()) );
  CHECK( print_vec<DDer> (sg, "dder", name()) );

  // Test using implicit symlinks.
  CHECK( print_vec<BVec> (sg, "bder", name()) );
  CHECK( print_vec<BVec> (sg, "dvec", name()) );
  CHECK( print_vec<BVec> (sg, "dder", name()) );
  CHECK( print_vec<DVec> (sg, "dder", name()) );

  CHECK( print_elvec (sg, "elvec", name()) );

  CHECK( remap_test (sg, log) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode DMTestRead::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest
