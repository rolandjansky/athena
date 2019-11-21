/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/src/xAODTestThinCVec.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Thin CVec objects.
 */


#include "xAODTestThinCVec.h"
#include "StoreGate/ThinningHandle.h"


namespace DMTest {


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestThinCVec::initialize()
{
  ATH_CHECK( m_cvecKey.initialize (m_stream) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestThinCVec::execute (const EventContext& ctx) const
{
  SG::ThinningHandle<DMTest::CVec> cvec (m_cvecKey, ctx);

  unsigned int evt = ctx.evt();

  cvec.keepAll();
  for (size_t i = 0; i < cvec->size(); i++) {
    if (((evt ^ m_mask) & (1 << (i%4))) == 0) {
      cvec.thin(i);
    }
  }

  return StatusCode::SUCCESS;
}


} // namespace DMTest
