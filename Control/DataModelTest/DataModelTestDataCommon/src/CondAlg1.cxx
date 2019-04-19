/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
// $Id$
/**
 * @file DataModelTestDataCommon/src/CondAlg1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Testing conditions algorithms.
 */


#include "CondAlg1.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param pSvcLocator The service locator.
 */
CondAlg1::CondAlg1 (const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_attrListKey ("/DMTest/TestAttrList"),
    m_scondKey ("scond", "DMTest")
{
  declareProperty ("AttrListKey",  m_attrListKey);
  declareProperty ("SCondKey",  m_scondKey);
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode CondAlg1::initialize()
{
  ATH_CHECK( m_attrListKey.initialize() );
  ATH_CHECK( m_scondKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode CondAlg1::execute (const EventContext& ctx) const
{
  SG::ReadCondHandle<AthenaAttributeList> attrList (m_attrListKey, ctx);
  int xint = (**attrList)["xint"].data<int>();

  EventIDRange range;
  ATH_CHECK( attrList.range(range) );

  SG::WriteCondHandle<DMTest::S2> scond (m_scondKey, ctx);
  auto s2 = std::make_unique<DMTest::S2> (xint*100);
  ATH_CHECK( scond.record (range, std::move(s2) ) );

  return StatusCode::SUCCESS;
}


} // namespace DMTest
