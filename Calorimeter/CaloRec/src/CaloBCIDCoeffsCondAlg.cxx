/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/src/CaloBCIDCoeffsCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2020
 * @brief Conditions algorithm to create CaloBCIDCoeffs.
 */


#include "CaloBCIDCoeffsCondAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "GaudiKernel/ICondSvc.h"


/**
 * @brief Gaudi initialize method.
 */
StatusCode CaloBCIDCoeffsCondAlg::initialize()
{
  ATH_CHECK( m_mcSymKey.initialize() );
  ATH_CHECK( m_ofcKey.initialize() );
  ATH_CHECK( m_shapeKey.initialize() );
  ATH_CHECK( m_minBiasAvgKey.initialize() );
  ATH_CHECK( m_outputCoeffsKey.initialize() );

  ServiceHandle<ICondSvc> condSvc ("CondSvc", name());
  ATH_CHECK( condSvc.retrieve() );
  ATH_CHECK( condSvc->regHandle (this, m_outputCoeffsKey) );

  ATH_CHECK( detStore()->retrieve (m_laronline_id, "LArOnlineID") );
  return StatusCode::SUCCESS;
}


/**
 * @brief Execute the algorithm.
 * @param ctx Event context.
 */
StatusCode CaloBCIDCoeffsCondAlg::execute (const EventContext& ctx) const
{
  SG::WriteCondHandle<CaloBCIDCoeffs> outputCoeffs (m_outputCoeffsKey, ctx);
  if (outputCoeffs.isValid()) {
    ATH_MSG_DEBUG ("Found valid write handle");
    return StatusCode::SUCCESS;
  }

  SG::ReadCondHandle<LArMCSym>           mcsym      (m_mcSymKey, ctx);
  SG::ReadCondHandle<ILArOFC>            ofcs       (m_ofcKey,   ctx);
  SG::ReadCondHandle<ILArShape>          shapes     (m_shapeKey, ctx);
  SG::ReadCondHandle<ILArMinBiasAverage> minBiasAvg (m_minBiasAvgKey, ctx);

  auto coeffs = std::make_unique<CaloBCIDCoeffs> (mcsym->symIds(),
                                                  *m_laronline_id,
                                                  **ofcs,
                                                  **shapes,
                                                  **minBiasAvg);

  outputCoeffs.addDependency (mcsym, ofcs, shapes, minBiasAvg);
  ATH_CHECK( outputCoeffs.record (std::move (coeffs)) );
  ATH_MSG_INFO( "recorded new " << outputCoeffs.key() << " with range " << outputCoeffs.getRange() );
  return StatusCode::SUCCESS;
}
