/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFTrackIsolationAlgMT.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTrigMuon/L2IsoMuonAuxContainer.h"

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
  ATH_MSG_DEBUG("L2MuonParticles       : " << m_l2MuonContainerKey.key());
  ATH_MSG_DEBUG("requireCombinedMuon   : " << m_requireCombined);
  ATH_MSG_DEBUG("useVarIso             : " << m_useVarIso);
  ATH_MSG_DEBUG("IsoType               : " << m_isoType);

  if(m_isoType!=1 && m_isoType!=2){
    ATH_MSG_ERROR("Isolation type not supported");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_trackParticleKey.initialize() );
  ATH_CHECK( m_l2MuonContainerKey.initialize(m_isoType==2) );  // we don't need L2 muons in this mode, do disable the read handle key
  ATH_CHECK( m_l2MuonIsoContainerKey.initialize(m_isoType==2) );  // we don't record L2 isolation muons in this mode, do disable the write handle key
  ATH_CHECK( m_efMuonContainerKey.initialize(m_isoType==1) );
  ATH_CHECK( m_muonContainerKey.initialize(m_isoType==1) );



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
  const xAOD::L2CombinedMuonContainer *l2MuonContainer = nullptr;
  auto idTrackHandle = SG::makeHandle( m_trackParticleKey, ctx );
  if( !idTrackHandle.isValid() ) {
    ATH_MSG_ERROR("Failed to retrieve inner detector track particles");
    return StatusCode::FAILURE;
  }
  idTrackParticles = idTrackHandle.cptr();
  ATH_MSG_DEBUG("Inner detector track particles retrieved with size : " << idTrackHandle->size() << "...");

  if ( m_isoType == 1 ) { // isoType==1 -> ID+EF
    auto efMuonHandle = SG::makeHandle( m_efMuonContainerKey, ctx );
    if ( !efMuonHandle.isValid() ) {
      ATH_MSG_ERROR("Failed to retrieve EF Muon Container " << m_efMuonContainerKey.key());
      return StatusCode::FAILURE;
    } 
    efMuonContainer = efMuonHandle.cptr();
    ATH_MSG_DEBUG("EF Muons retrieved with size : " << efMuonHandle->size());
  }
  if ( m_isoType == 2 ) { // isoType==2 -> FTK+L2
    auto l2MuonHandle = SG::makeHandle( m_l2MuonContainerKey, ctx );
    if ( !l2MuonHandle.isValid() ) {
      ATH_MSG_ERROR("Failed to retrieve L2 Muon container");
      return StatusCode::FAILURE;
    }
    l2MuonContainer = l2MuonHandle.cptr();
    ATH_MSG_DEBUG("L2 Muons retrieved with size : " << l2MuonHandle->size());
  }

  // start calculation
  // prepare output
  xAOD::MuonContainer *muonContainer = nullptr;
  xAOD::L2IsoMuonContainer *l2MuonIsoContainer = nullptr;

  StatusCode result = StatusCode::SUCCESS;
  std::vector<double> isoResults;
  std::vector<double> dzvals; // for monitoring
  std::vector<double> drvals; // for monitoring
  std::vector<double> selfremoval;

  if( m_isoType==1){
    SG::WriteHandle<xAOD::MuonContainer> muonOutput(m_muonContainerKey);
    ATH_CHECK(muonOutput.record(std::make_unique<xAOD::MuonContainer>(), std::make_unique<xAOD::MuonAuxContainer>())); 
    ATH_MSG_DEBUG("Record EF isolation muon : " << m_muonContainerKey.key());
    muonContainer = muonOutput.ptr();

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
      result = m_onlineEfIsoTool->calcTrackIsolation( muon, idTrackParticles, m_coneSizes, isoResults, &dzvals, &drvals, false, &selfremoval );

      ini_trkdz.insert( ini_trkdz.begin(), dzvals.begin(), dzvals.end());
      ini_trkdr.insert( ini_trkdr.begin(), drvals.begin(), drvals.end());
      if ( selfremoval.size()==3 ) {
        ini_selfpt.push_back(selfremoval[0]*1e-3);     // convert to GeV
        ini_combinedpt.push_back(selfremoval[1]*1e-3); // convert to GeV
        ini_removedpt.push_back(selfremoval[2]*1e-3);  // convert to GeV
      }

      if ( result.isFailure() || isoResults.size()!=2 ) {
	ATH_MSG_WARNING("Isolation will not be set for this muon. result.isFailure: "<<result.isFailure()<<" isoResults.size: "<<isoResults.size());
      } 
      else { // isolation tool was ok - store results
      	const float ptcone20 = isoResults[0]; 	
        const float ptcone30 = isoResults[1]; 
	ini_cone2.push_back(ptcone20*1e-3); // convert to GeV
        ini_cone3.push_back(ptcone30*1e-3); // convert to GeV

        muonContainer->push_back( new xAOD::Muon(*muon) );
        xAOD::Muon* outputmuon = muonContainer->back();

        // set isolation info into output muon object
        if ( m_useVarIso ) {
          outputmuon->setIsolation( ptcone20, xAOD::Iso::ptvarcone20 );
          outputmuon->setIsolation( ptcone30, xAOD::Iso::ptvarcone30 );
        } else {
          outputmuon->setIsolation( ptcone20, xAOD::Iso::ptcone20 );
          outputmuon->setIsolation( ptcone30, xAOD::Iso::ptcone30 );
        }
      }
    }//loop over muons
  }
  if(m_isoType==2){
    SG::WriteHandle<xAOD::L2IsoMuonContainer> l2MuonIsoOutput(m_l2MuonIsoContainerKey);
    ATH_CHECK(l2MuonIsoOutput.record(std::make_unique<xAOD::L2IsoMuonContainer>(), std::make_unique<xAOD::L2IsoMuonAuxContainer>()));
    ATH_MSG_DEBUG("Record L2 isolation muon : " << m_muonContainerKey.key());
    l2MuonIsoContainer = l2MuonIsoOutput.ptr();

    for ( auto muon : *l2MuonContainer ) {

      result = m_onlineEfIsoTool->calcTrackIsolation( muon, idTrackParticles, m_coneSizes, isoResults, &dzvals, &drvals, true, &selfremoval );

      ini_trkdz.insert( ini_trkdz.begin(), dzvals.begin(), dzvals.end());
      ini_trkdr.insert( ini_trkdr.begin(), drvals.begin(), drvals.end());
      if ( selfremoval.size()==3 ) {
        ini_selfpt.push_back(selfremoval[0]*1e-3);     // convert to GeV
        ini_combinedpt.push_back(selfremoval[1]*1e-3); // convert to GeV
        ini_removedpt.push_back(selfremoval[2]*1e-3);  // convert to GeV
      }

      if ( result.isFailure() || isoResults.size()!=2 ) {
	ATH_MSG_WARNING("Isolation will not be set for this muon. result.isFailure: "<<result.isFailure()<<" isoResults.size: "<<isoResults.size());
      } else { // isolation tool was ok - store results

      	const float ptcone20 = isoResults[0]; 	
        const float ptcone30 = isoResults[1]; 
	ini_cone2.push_back(ptcone20*1e-3); // convert to GeV
        ini_cone3.push_back(ptcone30*1e-3); // convert to GeV

      	xAOD::L2IsoMuon *muonIso = new xAOD::L2IsoMuon();
        l2MuonIsoContainer->push_back(muonIso);
        muonIso->setPt(muon->pt());
        muonIso->setEta(muon->eta());
        muonIso->setPhi(muon->phi());
        muonIso->setCharge(muon->charge());
        muonIso->setSumPt02(ptcone20);
        muonIso->setSumPt03(ptcone30);
      }
    }
  }

  return StatusCode::SUCCESS;
}
