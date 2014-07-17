/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DMTestWrite.cxx,v 1.8 2008-11-22 02:03:44 ssnyder Exp $
/**
 * @file  src/DMTestWrite.cxx
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Algorithm to test writing @c DataVector data.
 *
 *        We write information using all four types,
 *        @c BVec, @c DVec, @c BDer, @c DDer.
 *        In the data we write, there's no inheritance relationship
 *        between @c BVec and @c DVec.
 *
 * This line'll work around a cmt depcy bug by making the quote count even.
 */

#include "DMTestWrite.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"
#include "DataModelTestDataWrite/BVec.h"
#include "DataModelTestDataWrite/BDer.h"
#include "DataModelTestDataWrite/DVec.h"
#include "DataModelTestDataWrite/DDer.h"
#include "DataModelTestDataWrite/ELVec.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
DMTestWrite::DMTestWrite (const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm (name, pSvcLocator)
{
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode DMTestWrite::initialize()
{
  return StatusCode::SUCCESS;
}


namespace {


/**
 * @brief Create a container of class @c VEC and register it in storegate.
 * @param log The logging object, for error reporting.
 * @param sg The @c StoreGateSvc object.
 * @param n The number of entries with in container to create.
 * @param istart The number of the first entry to create.
 * @param key The storegate key to use.
 */
template <class VEC>
StatusCode make_vec (StoreGateSvc* sg,
                     int n,
                     int istart,
                     const std::string& key,
                     const std::string& context)
{
  VEC* vec = new VEC;
  for (int i=0; i < n; i++)
    vec->push_back (new typename VEC::base_value_type (i + istart));
  CHECK_WITH_CONTEXT( sg->record (vec, key), context);
  return StatusCode::SUCCESS;
}


StatusCode make_elvec (StoreGateSvc* sg,
                       const std::string& key,
                       const std::string& context)
{
  DMTest::ELVec* vec = new DMTest::ELVec;
  for (int i = 0; i < 10; i += 3) {
    vec->m_el.push_back (ElementLink<BVec> ("bvec", i));
    vec->m_el.back().toPersistent();
  }
  for (int i = 1; i < 10; i += 3) {
    vec->m_el.push_back (ElementLink<BVec> ("bder", i));
    vec->m_el.back().toPersistent();
  }
  CHECK_WITH_CONTEXT( sg->record (vec, key), context);
  return StatusCode::SUCCESS;
}


StatusCode remap_test (StoreGateSvc* sg, int istart, MsgStream& log)
{
  ELVec* elv = new ELVec;
  CHECK_WITH_CONTEXT( sg->record (elv, "elv_remap"), "remap_test");

  BVec* b1 = new BVec;
  CHECK_WITH_CONTEXT( sg->record (b1, "b1"), "remap_test");
  for (int i=0; i < 10; i++) {
    b1->push_back (new B (i + istart + 100));
    elv->m_el2.push_back (ElementLink<BVec> ("b1", i));
    if ((i%2) == 0)
      elv->m_elv2.push_back (ElementLink<BVec> ("b1", i));
    else
      elv->m_elv2.push_back (ElementLink<BVec> (*b1, i));
  }

  BVec* b2 = new BVec;
  CHECK_WITH_CONTEXT( sg->record (b2, "b2"), "remap_test");
  for (int i=0; i < 10; i++) {
    b2->push_back (new B (i + istart + 200));
    elv->m_el2.push_back (ElementLink<BVec> ("b2", i));
    elv->m_elv2.push_back (ElementLink<BVec> ("b2", i));
  }

  elv->m_dl2.push_back (DataLink<BVec> ("b1"));
  elv->m_dl2.push_back (DataLink<BVec> ("b2"));

  BVec* b3 = new BVec (SG::VIEW_ELEMENTS);
  CHECK_WITH_CONTEXT( sg->record (b3, "b3"), "remap_test");
  b3->insert (b3->end(), b1->begin(), b1->end());
  b3->insert (b3->end(), b2->begin(), b2->end());

  sg->remap (ClassID_traits<BVec>::ID(), "b1", "b3", 0);
  sg->remap (ClassID_traits<BVec>::ID(), "b2", "b3", b1->size());

  ElementLinkVectorCnv_p1<ElementLinkVector<BVec> > elvcnv;
  elvcnv.transToPers (&elv->m_elv2, &elv->m_elv2_p, log);

  ElementLinkCnv_p3<ElementLink<BVec> > elcnv;
  elv->m_el2_p.resize (elv->m_el2.size());
  for (size_t i=0; i < elv->m_el2.size(); i++)
    elcnv.transToPers (&elv->m_el2[i], &elv->m_el2_p[i], log);

  DataLinkCnv_p1<DataLink<BVec> > dlcnv;
  elv->m_dl2_p.resize (elv->m_dl2.size());
  for (size_t i=0; i < elv->m_dl2.size(); i++)
    dlcnv.transToPers (&elv->m_dl2[i], &elv->m_dl2_p[i], log);

  return StatusCode::SUCCESS;
}


} // anonymous namespace


/**
 * @brief Algorithm event processing.
 */
StatusCode DMTestWrite::execute()
{
  // Make one instance of each container.
  MsgStream log(msgSvc(), name());
  static int i = 0;
  StoreGateSvc* sg = &*evtStore();
  CHECK( make_vec<BVec> (sg, 10,   0+i, "bvec", name()) );
  CHECK( make_vec<BDer> (sg, 10, 100+i, "bder", name()) );
  CHECK( make_vec<DVec> (sg, 10, 200+i, "dvec", name()) );
  CHECK( make_vec<DDer> (sg, 10, 300+i, "dder", name()) );
  CHECK( make_elvec (sg, "elvec", name()) );
  CHECK( remap_test (sg, i, log) );
  ++i;
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode DMTestWrite::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest
