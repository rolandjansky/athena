/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
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
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArIdentifier/LArOnlineID.h"


/**
 * @brief Gaudi initialize method.
 */
StatusCode CaloBCIDCoeffsCondAlg::initialize()
{
  if ( !m_isSC ) ATH_CHECK( m_mcSymKey.initialize() );
  ATH_CHECK( m_ofcKey.initialize() );
  ATH_CHECK( m_shapeKey.initialize() );
  ATH_CHECK( m_minBiasAvgKey.initialize() );
  ATH_CHECK( m_outputCoeffsKey.initialize() );

  if ( m_isSC ) {
	const LArOnline_SuperCellID* ll = nullptr;
	ATH_CHECK(detStore()->retrieve(ll,"LArOnline_SuperCellID"));
	m_laronline_id = (const LArOnlineID_Base*)ll;
  }
  else {
	const LArOnlineID* ll = nullptr;
	ATH_CHECK(detStore()->retrieve(ll,"LArOnlineID"));
	m_laronline_id = (const LArOnlineID_Base*)ll;
  }

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

  std::vector<HWIdentifier> hwids ;
  if (!m_isSC) {
	SG::ReadCondHandle<LArMCSym>           mcsym      (m_mcSymKey, ctx);
	hwids = mcsym->symIds();
  }
  else {
     for(long long unsigned int i=0;i<m_laronline_id->channelHashMax();i++){
 	hwids.push_back( m_laronline_id->channel_Id((IdentifierHash)i) );
     }
  }
  SG::ReadCondHandle<ILArOFC>            ofcs       (m_ofcKey,   ctx);
  SG::ReadCondHandle<ILArShape>          shapes     (m_shapeKey, ctx);
  SG::ReadCondHandle<ILArMinBiasAverage> minBiasAvg (m_minBiasAvgKey, ctx);

  auto coeffs = std::make_unique<CaloBCIDCoeffs> (hwids,
                                                  *((const LArOnlineID_Base*)m_laronline_id),
                                                  **ofcs,
                                                  **shapes,
                                                  **minBiasAvg);

  outputCoeffs.addDependency (ofcs, shapes, minBiasAvg);
  ATH_CHECK( outputCoeffs.record (std::move (coeffs)) );
  ATH_MSG_INFO( "recorded new " << outputCoeffs.key() << " with range " << outputCoeffs.getRange() );
  return StatusCode::SUCCESS;
}
