/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/src/CaloBCIDLumiCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2020
 * @brief Conditions algorithm to create CaloBCIDLumi.
 */


#include "CaloBCIDLumiCondAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"


/**
 * @brief Gaudi initialize method.
 */
StatusCode CaloBCIDLumiCondAlg::initialize()
{
  ATH_CHECK( m_coeffsKey.initialize() );
  ATH_CHECK( m_outputLumiKey.initialize() );
  ATH_CHECK( m_bcDataKey.initialize(m_isMC) );
  ATH_CHECK( m_lumiDataKey.initialize(!m_isMC) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Execute the algorithm.
 * @param ctx Event context.
 */
StatusCode CaloBCIDLumiCondAlg::execute (const EventContext& ctx) const
{
  SG::WriteCondHandle<CaloBCIDLumi> outputLumi (m_outputLumiKey, ctx);
  SG::ReadCondHandle<CaloBCIDCoeffs>     coeffs     (m_coeffsKey, ctx);

  std::unique_ptr<CaloBCIDLumi> lumi;
  if (m_isMC) {
    if (outputLumi.isValid()) {
      ATH_MSG_DEBUG ("Found valid write handle");
      return StatusCode::SUCCESS;
    }

    SG::ReadCondHandle<BunchCrossingCondData> bccd (m_bcDataKey,ctx);
    lumi = std::make_unique<CaloBCIDLumi> (**coeffs, **bccd);
    outputLumi.addDependency (bccd);
  }
  else {
    SG::ReadCondHandle<LuminosityCondData> lcd (m_lumiDataKey,ctx);

    // Need to check start of range, since lumi might be extensible.
    EventIDRange range;
    if (outputLumi.isValid(range)) {
      EventIDBase start = range.start();
      EventIDBase coeffs_start = coeffs.getRange().start();
      EventIDBase lcd_start = lcd.getRange().start();
      if ((start.time_stamp() == coeffs_start.time_stamp() &&
           start.time_stamp_ns_offset() == coeffs_start.time_stamp_ns_offset()) ||
          (start.time_stamp() == lcd_start.time_stamp() &&
           start.time_stamp_ns_offset() == lcd_start.time_stamp_ns_offset()))
      {
        ATH_MSG_DEBUG ("Found valid write handle");
        return StatusCode::SUCCESS;
      }
    }

    lumi = std::make_unique<CaloBCIDLumi> (**coeffs, **lcd);
    outputLumi.addDependency (lcd);
  }

  outputLumi.addDependency (coeffs);

  ATH_CHECK( outputLumi.record (std::move (lumi)) );
  ATH_MSG_INFO( "recorded new " << outputLumi.key() << " with range " << outputLumi.getRange() );
  return StatusCode::SUCCESS;
}
