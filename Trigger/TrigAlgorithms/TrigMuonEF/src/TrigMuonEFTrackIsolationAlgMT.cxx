/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFTrackIsolationAlgMT.h"

TrigMuonEFTrackIsolationAlgMT::TrigMuonEFTrackIsolationAlgMT( const std::string& name, 
                                                              ISvcLocator* pSvcLocator )
  :AthAlgorithm(name, pSvcLocator),
   m_coneSizes(),
   m_timerSvc("TrigTimerSvc", name),
   m_dataPrepTime(0),
   m_calcTime(0),
   m_dataOutputTime(0)
{
  // cone sizes are hard-coded to ensure the correct result goes to the edm
  m_coneSizes.push_back(0.2);
  m_coneSizes.push_back(0.3);
}


StatusCode TrigMuonEFTrackIsolationAlgMT::initialize()
{
  ATH_MSG_DEBUG("Initialization");

  if ( m_isoType == 1 ) {
    //ATH_CHECK( m_idTrackParticleKey.initialize() );
    ATH_CHECK( m_efMuonContainerKey.initialize() );
  } else if ( m_isoType == 2 ) {
    //ATH_CHECK( m_ftkTrackParticleKey.initialize() );
    //ATH_CHECK( m_l2MuonContainerKey.initialize() );
    ATH_MSG_WARNING("Not initialize DataHandling keys due to wrong iso type");
  } else {
    ATH_MSG_WARNING("Not initialize DataHandling keys due to wrong iso type");
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigMuonEFTrackIsolationAlgMT::finalize()
{
  ATH_MSG_DEBUG("Finalization");

  return StatusCode::SUCCESS;
}


StatusCode TrigMuonEFTrackIsolationAlgMT::execute()
{
  ATH_MSG_DEBUG("Execution");

  auto ctx = getContext();

  const xAOD::TrackParticleContainer *idTrackParticles = 0;
  const xAOD::MuonContainer *efMuonContainer(0);
  if ( m_isoType == 1 ) { // isoType==1 -> ID+EF
    //auto idTrackHandle = SG::makeHandle( m_idTrackParticleKey, ctx );
    //if( !idTrackHandle.isValid() ) {
    //  ATH_MSG_WARNING("Failed to retrieve inner detector track particles");
    //  return StatusCode::FAILURE;
    //}
    //idTrackParticles = idTrackHandle.cptr();
    //ATH_MSG_DEBUG("Inner detector track particles retrieved with size : " << idTrackHandle->size() << "...");

    auto efMuonHandle = SG::makeHandle( m_efMuonContainerKey, ctx );
    if ( !efMuonHandle.isValid() ) {
      ATH_MSG_WARNING("Failed to retrieve EF Muon Container " << m_efMuonContainerKey.key());
      return StatusCode::FAILURE;
    } 
    //efMuonContainer = efMuonHandle.cptr();
    ATH_MSG_DEBUG("EF Muons retrieved with size : " << efMuonHandle->size());

  } else {
    ATH_MSG_DEBUG("No IDTrk / EF Muon isolation required");
  }

  //const xAOD::TrackParticleContainer *ftkTrackParticles =0;
  //const xAOD::L2CombinedMuonContainer *l2MuonContainer(0);  
  //if ( m_isoType == 2 ) { // isoType==2 -> FTK+L2
  //  auto ftkTrackHandle = SG::makeHandle( m_ftkTrackParticleKey, ctx );
  //  if ( !ftkTrackHandle.isValid() ) {
  //    ATH_MSG_WARNING("Failed to retrieve FTK track particles");
  //    return StatusCode::FAILURE;
  //  }
  //  ftkTrackParticles = ftkTrackHandle.cptr();
  //  ATH_MSG_DEBUG("FTK track particles retrieved with size : " << ftkTrackHandle->size() );

  //  auto l2MuonHandle = SG::makeHandle( m_l2MuonContainerKey, ctx );
  //  if ( !l2MuonHandle.isValid() ) {
  //    ATH_MSG_WARNING("Failed to retrieve L2 Muon container");
  //    return StatusCode::FAILURE;
  //  }
  //  l2MuonContainer = l2MuonHandle.cptr();
  //  ATH_MSG_DEBUG("L2 Muons retrieved with size : " << l2MuonHandle->size() );

  //} else {
  //  ATH_MSG_DEBUG("No FTK / L2 Muon isolation required");
  //}

  return StatusCode::SUCCESS;
}



