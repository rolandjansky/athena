/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFTrackIsolationAlgMT.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "StoreGate/WriteDecorHandle.h"

TrigMuonEFTrackIsolationAlgMT::TrigMuonEFTrackIsolationAlgMT( const std::string& name, 
                                                              ISvcLocator* pSvcLocator )
  :AthAlgorithm(name, pSvcLocator),
   m_coneSizes()
{
  // cone sizes are hard-coded to ensure the correct result goes to the edm
  m_coneSizes.push_back(0.2);
  m_coneSizes.push_back(0.3);
}


StatusCode TrigMuonEFTrackIsolationAlgMT::initialize()
{
  ATH_MSG_DEBUG("Initialization");

  ATH_MSG_DEBUG("package version = " << PACKAGE_VERSION);
  ATH_MSG_DEBUG("*** Properties set as follows ***");
  ATH_MSG_DEBUG("Track isolation tool  : " << m_onlineEfIsoTool);
  ATH_MSG_DEBUG("TrackParticles        : " << m_trackParticleKey.key());
  ATH_MSG_DEBUG("EFMuonParticles       : " << m_efMuonContainerKey.key());
  ATH_MSG_DEBUG("requireCombinedMuon   : " << m_requireCombined);
  ATH_MSG_DEBUG("useVarIso             : " << m_useVarIso);

  ATH_CHECK( m_trackParticleKey.initialize() );
  ATH_CHECK( m_efMuonContainerKey.initialize() );
  ATH_CHECK( m_muonContainerKey.initialize() );
  ATH_CHECK( m_muonIso20Key.initialize() );
  ATH_CHECK( m_muonIso30Key.initialize() );



  ATH_CHECK( m_onlineEfIsoTool.retrieve() );

  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
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

  // variables to initialize and keep values for monitoring variables
  std::vector<double> ini_cone2(0);
  std::vector<double> ini_cone3(0);
  std::vector<double> ini_trkdz(0);
  std::vector<double> ini_trkdr(0);
  std::vector<double> ini_selfpt(0);
  std::vector<double> ini_removedpt(0);
  std::vector<double> ini_combinedpt(0);
  std::vector<int> ini_ntrks(0);

  // Monitoring histogram
  auto trkptiso_cone2  = Monitored::Collection("trks_trkptiso_cone2", ini_cone2);
  auto trkptiso_cone3  = Monitored::Collection("trks_trkptiso_cone3", ini_cone3);
  auto trkdz           = Monitored::Collection("trks_trkdz", ini_trkdz);
  auto trkdr           = Monitored::Collection("trks_trksdr", ini_trkdr);
  auto muon_selfpt     = Monitored::Collection("muon_selfpt", ini_selfpt);
  auto muon_removedpt  = Monitored::Collection("muon_removedpt", ini_removedpt);
  auto muon_combinedpt = Monitored::Collection("muon_combinedpt", ini_combinedpt);
  auto n_trks          = Monitored::Collection("n_trks", ini_ntrks);

  auto monitorIt = Monitored::Group( m_monTool, trkptiso_cone2, trkptiso_cone3, trkdz, trkdr, 
                                            muon_selfpt, muon_removedpt, muon_combinedpt, n_trks );

  // get input objects
  const xAOD::TrackParticleContainer *idTrackParticles = nullptr;
  const xAOD::MuonContainer *efMuonContainer = nullptr;

  auto idTrackHandle = SG::makeHandle( m_trackParticleKey, ctx );
  if( !idTrackHandle.isValid() ) {
    ATH_MSG_ERROR("Failed to retrieve inner detector track particles");
    return StatusCode::FAILURE;
  }
  idTrackParticles = idTrackHandle.cptr();
  ATH_MSG_DEBUG("Inner detector track particles retrieved with size : " << idTrackHandle->size() << "...");

  auto efMuonHandle = SG::makeHandle( m_efMuonContainerKey, ctx );
  if ( !efMuonHandle.isValid() ) {
    ATH_MSG_ERROR("Failed to retrieve EF Muon Container " << m_efMuonContainerKey.key());
    return StatusCode::FAILURE;
  } 
  efMuonContainer = efMuonHandle.cptr();
  ATH_MSG_DEBUG("EF Muons retrieved with size : " << efMuonHandle->size());
  

  // start calculation
  // prepare output
  xAOD::MuonContainer *muonContainer = nullptr;

  StatusCode result = StatusCode::SUCCESS;
  std::vector<double> isoResults;
  std::vector<double> dzvals; // for monitoring
  std::vector<double> drvals; // for monitoring
  std::vector<double> selfremoval;
  
  SG::WriteHandle<xAOD::MuonContainer> muonOutput(m_muonContainerKey);
  ATH_CHECK(muonOutput.record(std::make_unique<xAOD::MuonContainer>(), std::make_unique<xAOD::MuonAuxContainer>())); 
  ATH_MSG_DEBUG("Record EF isolation muon : " << m_muonContainerKey.key());
  muonContainer = muonOutput.ptr();

  SG::WriteDecorHandle<xAOD::MuonContainer, double> muonptCone20(m_muonIso20Key);
  SG::WriteDecorHandle<xAOD::MuonContainer, double> muonptCone30(m_muonIso30Key);

  for ( auto muon : *efMuonContainer ) {
    const xAOD::Muon::MuonType muonType = muon->muonType();
    if ( muonType!=xAOD::Muon::MuonType::Combined) {
      if ( m_requireCombined ) {
	ATH_MSG_DEBUG("Not a combined or muon & requiredCombined = true, so ignore this muon");
	continue;
      } else {
	if ( muonType!=xAOD::Muon::MuonType::MuonStandAlone ) {
	  ATH_MSG_DEBUG("EF muon hsa neither combined, segment tagged, nor standalone muon");
	  continue;
	}
      }
    }
    result = m_onlineEfIsoTool->calcTrackIsolation( muon, idTrackParticles, m_coneSizes, isoResults, &dzvals, &drvals, &selfremoval );

    ini_trkdz.insert( ini_trkdz.begin(), dzvals.begin(), dzvals.end());
    ini_trkdr.insert( ini_trkdr.begin(), drvals.begin(), drvals.end());
    if ( selfremoval.size()==2 ) {
      ini_selfpt.push_back(selfremoval[0]*1e-3);     // convert to GeV
      ini_combinedpt.push_back(selfremoval[1]*1e-3); // convert to GeV
    }

    if ( result.isFailure() || isoResults.size()!=2 ) {
      ATH_MSG_WARNING("Isolation will not be set for this muon. result.isFailure: "<<result.isFailure()<<" isoResults.size: "<<isoResults.size());
    } 
    else { // isolation tool was ok - store results
      muonContainer->push_back( new xAOD::Muon(*muon) );
      xAOD::Muon* outputmuon = muonContainer->back();
      muonptCone20(*outputmuon) = isoResults[0]; 	
      muonptCone30(*outputmuon) = isoResults[1]; 
      ini_cone2.push_back(isoResults[0]*1e-3); // convert to GeV
      ini_cone3.push_back(isoResults[1]*1e-3); // convert to GeV
    }
  }//loop over muons
  

  return StatusCode::SUCCESS;
}
