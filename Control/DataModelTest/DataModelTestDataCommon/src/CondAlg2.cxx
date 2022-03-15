/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
// $Id$
/**
 * @file DataModelTestDataCommon/src/CondAlg2.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief Testing conditions algorithm taking both RL and TS inputs
 *        and producing mixed output.
 */


#include "CondAlg2.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param pSvcLocator The service locator.
 */
CondAlg2::CondAlg2 (const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_rltestKey ("/DMTest/RLTest"),
    m_tstestKey ("/DMTest/TSTest"),
    m_outKey ("scond3", "DMTest")
{
  declareProperty ("RLTestKey",  m_rltestKey);
  declareProperty ("TSTestKey",  m_tstestKey);
  declareProperty ("OutKey",     m_outKey);
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode CondAlg2::initialize()
{
  ATH_CHECK( m_rltestKey.initialize() );
  ATH_CHECK( m_tstestKey.initialize() );
  ATH_CHECK( m_outKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode CondAlg2::execute (const EventContext& ctx) const
{
  SG::ReadCondHandle<AthenaAttributeList> rltest (m_rltestKey, ctx);
  int xint_rl = (**rltest)["xint"].data<int>();

  SG::ReadCondHandle<AthenaAttributeList> tstest (m_tstestKey, ctx);
  int xint_ts = (**tstest)["xint"].data<int>();

  EventIDRange range_rl;
  ATH_CHECK( rltest.range(range_rl) );

  EventIDRange range_ts;
  ATH_CHECK( tstest.range(range_ts) );

  EventIDBase start (range_rl.start().run_number(),
                     range_rl.start().event_number(),
                     range_ts.start().time_stamp(),
                     range_ts.start().time_stamp_ns_offset(),
                     range_rl.start().lumi_block());
  EventIDBase stop (range_rl.stop().run_number(),
                    range_rl.stop().event_number(),
                    range_ts.stop().time_stamp(),
                    range_ts.stop().time_stamp_ns_offset(),
                    range_rl.stop().lumi_block());

  EventIDRange range (start, stop);

  SG::WriteCondHandle<DMTest::S3> out (m_outKey, ctx);
  auto s3 = std::make_unique<DMTest::S3> (xint_rl + xint_ts);
  ATH_CHECK( out.record (range, std::move(s3)) );

  ATH_MSG_DEBUG("Recorded '" << out.key() << "' with range " << range);

  return StatusCode::SUCCESS;
}


} // namespace DMTest
