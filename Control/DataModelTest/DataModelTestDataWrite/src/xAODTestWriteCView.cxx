/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/xAODTestWriteCView.cxx
 * @author snyder@bnl.gov
 * @date Apr 2016
 * @brief Algorithm to test writing an xAOD view vector (cview)
 */


#include "xAODTestWriteCView.h"
#include "xAODTestWriteHelper.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CView.h"
#include "AthContainers/ConstDataVector.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestWriteCView::xAODTestWriteCView (const std::string &name,
                                        ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_cvecKey ("cvec"),
    m_cviewKey ("cview")
{
  declareProperty ("CVecKey", m_cvecKey);
  declareProperty ("CViewKey", m_cviewKey);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWriteCView::initialize()
{
  ATH_CHECK( m_cvecKey.initialize() );
  ATH_CHECK( m_cviewKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestWriteCView::execute_r (const EventContext& ctx) const
{
  SG::ReadHandle<DMTest::CVec> cvec (m_cvecKey, ctx);
  auto cview = std::make_unique<ConstDataVector<DMTest::CView> >(SG::VIEW_ELEMENTS);
  for (int i = cvec->size()-1; i >= 0; --i)
    cview->push_back ((*cvec)[i]);

  SG::WriteHandle<ConstDataVector<DMTest::CView> > cviewH (m_cviewKey, ctx);
  CHECK( DMTest::recordView1 (cviewH, std::move(cview)) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestWriteCView::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

