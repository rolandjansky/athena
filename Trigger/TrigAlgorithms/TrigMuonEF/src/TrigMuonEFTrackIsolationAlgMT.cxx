/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFTrackIsolationAlgMT.h"
#include "AthenaMonitoring/Monitored.h"


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
  ATH_MSG_DEBUG("IdTrackParticles      : " << m_idTrackParticleKey.key());
  ATH_MSG_DEBUG("FTKTrackParticles     : " << m_ftkTrackParticleKey.key());
  ATH_MSG_DEBUG("EFMuonParticles       : " << m_efMuonContainerKey.key());
  ATH_MSG_DEBUG("L2MuonParticles       : " << m_l2MuonContainerKey.key());
  ATH_MSG_DEBUG("requireCombinedMuon   : " << m_requireCombined);
  ATH_MSG_DEBUG("useVarIso             : " << m_useVarIso);
  ATH_MSG_DEBUG("IsoType               : " << m_isoType);

  if ( m_isoType == 1 ) {
    ATH_CHECK( m_ftkTrackParticleKey.initialize(false) );  // we don't need FTK tracks in this mode, do disable the read handle key
    ATH_CHECK( m_l2MuonContainerKey.initialize(false) );  // we don't need L2 muons in this mode, do disable the read handle key
    ATH_CHECK( m_l2MuonIsoContainerKey.initialize(false) );  // we don't record L2 isolation muons in this mode, do disable the write handle key
    ATH_CHECK( m_idTrackParticleKey.initialize() );
    ATH_CHECK( m_efMuonContainerKey.initialize() );
    ATH_CHECK( m_muonContainerKey.initialize() );
  } else if ( m_isoType == 2 ) {
    ATH_CHECK( m_idTrackParticleKey.initialize(false) );  // we don't need IS tracks in this mode, do disable the read handle key
    ATH_CHECK( m_efMuonContainerKey.initialize(false) );  // we don't need EF muons in this mode, do disable the read handle key
    ATH_CHECK( m_muonContainerKey.initialize(false) );    // we don't record EF muons in this mode, do disable the write handle key
    ATH_CHECK( m_ftkTrackParticleKey.initialize() );
    ATH_CHECK( m_l2MuonContainerKey.initialize() );
    ATH_CHECK( m_l2MuonIsoContainerKey.initialize() );
  } else {
    ATH_MSG_WARNING("Not initialize DataHandling keys due to wrong iso type");
  }


  if ( m_useOnlineTriggerTool ) {
    ATH_CHECK( m_onlineEfIsoTool.retrieve() );
  } else {
    ATH_MSG_WARNING("Not configurate muon isolation tool");
  }

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
  if ( m_isoType == 1 ) { // isoType==1 -> ID+EF
    auto idTrackHandle = SG::makeHandle( m_idTrackParticleKey, ctx );
    if( !idTrackHandle.isValid() ) {
      ATH_MSG_WARNING("Failed to retrieve inner detector track particles");
      return StatusCode::FAILURE;
    }
    idTrackParticles = idTrackHandle.cptr();
    ATH_MSG_DEBUG("Inner detector track particles retrieved with size : " << idTrackHandle->size() << "...");

    auto efMuonHandle = SG::makeHandle( m_efMuonContainerKey, ctx );
    if ( !efMuonHandle.isValid() ) {
      ATH_MSG_WARNING("Failed to retrieve EF Muon Container " << m_efMuonContainerKey.key());
      return StatusCode::FAILURE;
    } 
    efMuonContainer = efMuonHandle.cptr();
    ATH_MSG_DEBUG("EF Muons retrieved with size : " << efMuonHandle->size());

  } else {
    ATH_MSG_DEBUG("No IDTrk / EF Muon isolation required");
  }

  const xAOD::TrackParticleContainer *ftkTrackParticles = nullptr;
  const xAOD::L2CombinedMuonContainer *l2MuonContainer = nullptr;  
  if ( m_isoType == 2 ) { // isoType==2 -> FTK+L2
    auto ftkTrackHandle = SG::makeHandle( m_ftkTrackParticleKey, ctx );
    if ( !ftkTrackHandle.isValid() ) {
      ATH_MSG_WARNING("Failed to retrieve FTK track particles");
      return StatusCode::FAILURE;
    }
    ftkTrackParticles = ftkTrackHandle.cptr();
    ATH_MSG_DEBUG("FTK track particles retrieved with size : " << ftkTrackHandle->size());

    auto l2MuonHandle = SG::makeHandle( m_l2MuonContainerKey, ctx );
    if ( !l2MuonHandle.isValid() ) {
      ATH_MSG_WARNING("Failed to retrieve L2 Muon container");
      return StatusCode::FAILURE;
    }
    l2MuonContainer = l2MuonHandle.cptr();
    ATH_MSG_DEBUG("L2 Muons retrieved with size : " << l2MuonHandle->size());

  } else {
    ATH_MSG_DEBUG("No FTK / L2 Muon isolation required");
  }

  // start calculation
  // prepare output for isoType==1
  auto muonContainer = std::make_unique<xAOD::MuonContainer>();
  auto efaux = std::make_unique<xAOD::MuonAuxContainer>();
  muonContainer->setStore(efaux.get());

  if ( m_isoType==1 ) { // isoType==1 -> ID+EF
    // loop on EF Muons
    for ( auto muon : *efMuonContainer ) {
      ATH_MSG_DEBUG("Processing newxt EF muon w/ ID Track Isolation " << muon);
      const xAOD::Muon::MuonType muonType = muon->muonType();
      if ( muonType==xAOD::Muon::MuonType::Combined || muonType==xAOD::Muon::MuonType::SegmentTagged ) {
        ATH_MSG_DEBUG("EF muon has combined or segment tagged muon");
      } else {
        if ( m_requireCombined ) {
          ATH_MSG_DEBUG("Not a combined or segment tagged muon & requiredCombined = true, so ignore this muon");
          continue;
        } else {
          if ( muonType==xAOD::Muon::MuonType::MuonStandAlone ) {
            ATH_MSG_DEBUG("EF muon has standalone muon");
          } else {
            ATH_MSG_DEBUG("EF muon hsa neither combined, segment tagged, nor standalone muon");
            continue;
          }
        }
      } // if ( muonType==xAOD::Muon::MuonType::Combined || .... )

      std::vector<double> isoResults;
      std::vector<double> dzvals; // for monitoring
      std::vector<double> drvals; // for monitoring
      std::vector<double> selfremoval;

      ATH_MSG_DEBUG("Runing ID Tracks now");
      StatusCode result = StatusCode::FAILURE;
      if ( m_useOnlineTriggerTool ) {
        result = m_onlineEfIsoTool->calcTrackIsolation( muon, idTrackParticles, m_coneSizes, isoResults, &dzvals, &drvals, false, &selfremoval );
      } else {
        ATH_MSG_WARNING("Not configurate offline isolation tool");
      }

      ini_trkdz.insert( ini_trkdz.begin(), dzvals.begin(), dzvals.end());
      ini_trkdr.insert( ini_trkdr.begin(), drvals.begin(), drvals.end());

      if ( selfremoval.size()==3 ) {
        ini_selfpt.push_back(selfremoval[0]*1e-3);     // convert to GeV
        ini_combinedpt.push_back(selfremoval[1]*1e-3); // convert to GeV
        ini_removedpt.push_back(selfremoval[2]*1e-3);  // convert to GeV
      } else {
        ATH_MSG_WARNING("Muon pt not stored correctly - histogram have not been filled for this muon");
      }
     
      if ( result.isFailure() || isoResults.size()!=2 ) {
         if ( result.isFailure() ) {
           ATH_MSG_WARNING("Isolation tool failed for this muon - isolation will not be set for this muon");
         } else if ( isoResults.size()!=2 ) {
           ATH_MSG_WARNING("Wrong number of isolation results - isolation will not be set for this muon");
         }
      } else { // isolation tool was ok - store results

	const float ptcone20 = isoResults[0]; 	
        const float ptcone30 = isoResults[1]; 

        ini_cone2.push_back(ptcone20*1e-3); // convert to GeV
        ini_cone3.push_back(ptcone30*1e-3); // convert to GeV

        // deep copy muon (since otherwise we risk overwriting isolation results from other algos)
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
    } // EF Muon loop
  } // done isoTypei==1

  // prepare output for isoType==1
  auto l2MuonIsoContainer = std::make_unique<xAOD::L2IsoMuonContainer>();
  auto l2isoaux = std::make_unique<xAOD::L2IsoMuonAuxContainer>();
  muonContainer->setStore(l2isoaux.get());

  if ( m_isoType==2 ) {
    // loop on L2 muons
    for ( auto muon : *l2MuonContainer ) {
      ATH_MSG_DEBUG("Running L2 Muons and FTK Tracks now");

      std::vector<double> isoResultsFTK;
      std::vector<double> dzvalsFTK;
      std::vector<double> drvalsFTK;
      std::vector<double> selfremovalFTK;   

      // FTK tracks
      StatusCode resultFTK = StatusCode::FAILURE;
      if ( m_useOnlineTriggerTool ) {
        resultFTK = m_onlineEfIsoTool->calcTrackIsolation( muon, ftkTrackParticles, m_coneSizes, isoResultsFTK, &dzvalsFTK, &drvalsFTK, true, &selfremovalFTK );
      } else {
        ATH_MSG_WARNING("Not configurate offline isolation tool");
      }

      ini_trkdz.insert( ini_trkdz.begin(), dzvalsFTK.begin(), dzvalsFTK.end());
      ini_trkdr.insert( ini_trkdr.begin(), drvalsFTK.begin(), drvalsFTK.end());

      if ( selfremovalFTK.size()==3 ) {
        ini_selfpt.push_back(selfremovalFTK[0]*1e-3);     // convert to GeV
        ini_combinedpt.push_back(selfremovalFTK[1]*1e-3); // convert to GeV
        ini_removedpt.push_back(selfremovalFTK[2]*1e-3);  // convert to GeV
      } else {
        ATH_MSG_WARNING("Muon pt not stored correctly - histogram have not been filled for this muon");
      }
 
      if ( resultFTK.isFailure() || isoResultsFTK.size()!=2 ) {
         if ( resultFTK.isFailure() ) {
           ATH_MSG_WARNING("Isolation tool failed for this muon (FTK) - isolation will not be set for this muon");
         } else if ( isoResultsFTK.size()!=2 ) {
           ATH_MSG_WARNING("Wrong number of FTK isolation results - isolation will not be set for this muon");
         }
      } else { // isolation tool was ok - store results

	const float ptcone20FTK = isoResultsFTK[0]; 	
        const float ptcone30FTK = isoResultsFTK[1]; 

        ini_cone2.push_back(ptcone20FTK*1e-3); // convert to GeV
        ini_cone3.push_back(ptcone30FTK*1e-3); // convert to GeV

        xAOD::L2IsoMuon *muonIso = new xAOD::L2IsoMuon();
        l2MuonIsoContainer->push_back(muonIso);
        muonIso->setPt(muon->pt());
        muonIso->setEta(muon->eta());
        muonIso->setPhi(muon->phi());
        muonIso->setCharge(muon->charge());
        muonIso->setSumPt02(ptcone20FTK);
        muonIso->setSumPt03(ptcone30FTK);

      } 
    } // L2 muon loop
  } // done isoType==2

  if ( m_isoType==1 ) {

     ini_ntrks.push_back(idTrackParticles->size());

    // record output objects for EF
    const size_t noutputMuons = muonContainer->size();
    auto muonOutput = SG::makeHandle( m_muonContainerKey, ctx );
    ATH_CHECK( muonOutput.record( std::move(muonContainer), std::move(efaux) ));
    ATH_MSG_DEBUG("Successfully record EF isolation muon : " << m_muonContainerKey.key() << " with size = " << noutputMuons);

  }
  if ( m_isoType ==2 ) {
    ini_ntrks.push_back(ftkTrackParticles->size());

    // record output objects for L2
    const size_t noutputMuons = l2MuonIsoContainer->size();
    auto l2MuonIsoOutput = SG::makeHandle( m_l2MuonIsoContainerKey, ctx );
    ATH_CHECK( l2MuonIsoOutput.record( std::move(l2MuonIsoContainer), std::move(l2isoaux) ));
    ATH_MSG_DEBUG("Successfully record L2 isolation muon : " << m_muonContainerKey.key() << " with size = " << noutputMuons);

  }

  return StatusCode::SUCCESS;
}


