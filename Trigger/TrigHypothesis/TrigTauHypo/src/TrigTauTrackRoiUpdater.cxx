/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

#include "TrigTauTrackRoiUpdater.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "CxxUtils/phihelper.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "PathResolver/PathResolver.h"
#include "tauRecTools/HelperFunctions.h"

TrigTauTrackRoiUpdater::TrigTauTrackRoiUpdater(const std::string & name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

TrigTauTrackRoiUpdater::~TrigTauTrackRoiUpdater()
{
}

StatusCode TrigTauTrackRoiUpdater::initialize()
{
  ATH_MSG_INFO( "Initializing " << name() );
  ATH_MSG_INFO( "z0HalfWidth  " << m_z0HalfWidth );
  ATH_MSG_INFO( "etaHalfWidth " << m_etaHalfWidth );
  ATH_MSG_INFO( "phiHalfWidth " << m_phiHalfWidth );
  ATH_MSG_INFO( "nHitPix      " << m_nHitPix );
  ATH_MSG_INFO( "nSiHoles     " << m_nSiHoles );
  ATH_MSG_INFO( "useBDT       " << m_useBDT );

  if(m_z0HalfWidth<0. || m_etaHalfWidth<0. || m_phiHalfWidth<0.) {
    ATH_MSG_ERROR( "Incorrect parameters." );
    return StatusCode::FAILURE;
  }

  if(m_useBDT) {
    ATH_MSG_INFO( "BDTweights " << m_BDTweights );
    std::string inputWeightsPath = PathResolverFindCalibFile(m_BDTweights);
    ATH_MSG_INFO( "InputWeightsPath: " << inputWeightsPath );
    m_reader = std::make_unique<tauRecTools::BDTHelper>();
    ATH_CHECK( m_reader->initialize(inputWeightsPath) );
  }

  ATH_MSG_DEBUG( "Initialising HandleKeys" );
  CHECK( m_roIInputKey.initialize() );
  CHECK( m_tracksKey.initialize() );
  CHECK( m_roIOutputKey.initialize() );
  CHECK( m_tauJetKey.initialize(SG::AllowEmpty) );

  return StatusCode::SUCCESS;
}

StatusCode TrigTauTrackRoiUpdater::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "Running "<< name() );

  // Retrieve Input TrackCollection
  SG::ReadHandle< TrackCollection > TrackCollectionHandle = SG::makeHandle( m_tracksKey,ctx );
  CHECK( TrackCollectionHandle.isValid() );
  const TrackCollection *foundTracks = TrackCollectionHandle.get();

  if(foundTracks == nullptr) {
    ATH_MSG_ERROR( "No track container found, the track ROI updater should not be scheduled" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Found " << foundTracks->size() << " fast tracks, updating the RoI" );

  const Trk::Track *leadTrack = nullptr;
  const Trk::Perigee *trackPer = nullptr;
  const Trk::TrackSummary* summary = nullptr;
  double trkPtMax = 0.;
  
  // if useBDT=true, the track with the highest BDT score is used to define the updated ROI
  // else, the highest-pt track satisfying quality cuts is used
  // if no track is found, the input ROI is used
  if(!foundTracks->empty()) {

    // Find the track with the highest BDT score
    if(m_useBDT) {
      // retrieve TauJet from TrigTauRecCaloOnlyMVASequence
      SG::ReadHandle< xAOD::TauJetContainer > tauJetHandle = SG::makeHandle( m_tauJetKey,ctx );
      const xAOD::TauJetContainer *foundTaus = tauJetHandle.get();

      const Trk::Track *leadTrackBDT = nullptr;
      for(const Trk::Track* trk_it : *foundTracks) {
        if(trk_it->perigeeParameters()->pT() > trkPtMax) {
          trkPtMax = trk_it->perigeeParameters()->pT();
          leadTrackBDT = trk_it;
        }
      }
      double BDTMax = 0.;
      for(const Trk::Track* trk_it : *foundTracks) {
	double BDTscore = getBDTscore( foundTaus->at(0), trk_it, leadTrackBDT );
	if(BDTscore > BDTMax) {
	  BDTMax = BDTscore;
	  leadTrack = trk_it;
	}
      }
    }  
    // Find leading track passing quality cuts
    else {
      for (const Trk::Track* track : *foundTracks) {
        trackPer = track->perigeeParameters();
        summary = track->trackSummary();
        if(summary==nullptr) {
	  ATH_MSG_WARNING( "track summary not available in RoI updater " << name() );
	  continue;
	}
	float trackPt = trackPer->pT();
	if ( trackPt > trkPtMax ) {
	  int nPix  = summary->get(Trk::numberOfPixelHits);
	  if(nPix<0) nPix=0;
	  if(nPix < m_nHitPix) {
	    ATH_MSG_DEBUG( "Track rejected because of nHitPix " << nPix << " < " << m_nHitPix );
	    continue;
	  }
	  int nPixHole = summary->get(Trk::numberOfPixelHoles);
	  if (nPixHole < 0) nPixHole = 0;
	  int nSCTHole = summary->get(Trk::numberOfSCTHoles);
	  if (nSCTHole < 0) nSCTHole = 0;
	  if((nPixHole + nSCTHole) > m_nSiHoles) {
	    ATH_MSG_DEBUG( "Track rejected because of nSiHoles " << nPixHole+nSCTHole << " > " << m_nSiHoles );
	    continue;
	  }
	  leadTrack = track;
	  trkPtMax = trackPt;
	}
      }
    }
  }

  // get RoI descriptor
  SG::ReadHandle< TrigRoiDescriptorCollection > roisHandle = SG::makeHandle( m_roIInputKey, ctx );
  ATH_MSG_DEBUG("Size of roisHandle: "<<roisHandle->size());
  const TrigRoiDescriptor *roiDescriptor = roisHandle->at(0);

  // if a leading track is found, update all the ROI parameters (eta, etaMinus, etaPlus, phi, phiMinus, phiPlus, zed, zedMinus, zedPlus)
  // else, only update the eta/phi width (etaMinus, etaPlus, phiMinus, phiPlus)
  double zed = roiDescriptor->zed();
  double zedMinus = roiDescriptor->zedMinus();
  double zedPlus = roiDescriptor->zedPlus();
  double eta = roiDescriptor->eta();
  double phi = roiDescriptor->phi();  
  if( leadTrack ) {
    zed = leadTrack->perigeeParameters()->parameters()[Trk::z0];
    zedMinus = zed - m_z0HalfWidth;
    zedPlus = zed + m_z0HalfWidth;    
    eta = leadTrack->perigeeParameters()->eta();
    phi = leadTrack->perigeeParameters()->parameters()[Trk::phi0];
  }
  double etaMinus = eta - m_etaHalfWidth; 
  double etaPlus  = eta + m_etaHalfWidth;
  double phiMinus = CxxUtils::wrapToPi( phi - m_phiHalfWidth ); 
  double phiPlus  = CxxUtils::wrapToPi( phi + m_phiHalfWidth ); 

  // Prepare the new RoI
  TrigRoiDescriptor *outRoi = new TrigRoiDescriptor(roiDescriptor->roiWord(), roiDescriptor->l1Id(), roiDescriptor->roiId(),
						    eta, etaMinus, etaPlus,
						    phi, phiMinus, phiPlus,
						    zed, zedMinus, zedPlus);
  
  ATH_MSG_DEBUG("Input RoI " << *roiDescriptor);
  ATH_MSG_DEBUG("Output RoI " << *outRoi);

  auto roICollection = std::make_unique<TrigRoiDescriptorCollection>();
  roICollection->push_back(outRoi);

  // Save Outputs
  ATH_MSG_DEBUG( "Saving RoIs to be used as input to Fast Tracking -- TO BE CHANGED -- ::: " << m_roIOutputKey.key() );
  SG::WriteHandle< TrigRoiDescriptorCollection > outputRoiHandle = SG::makeHandle( m_roIOutputKey,ctx );
  CHECK( outputRoiHandle.record( std::move( roICollection ) ) );

  return StatusCode::SUCCESS;
}

double TrigTauTrackRoiUpdater::getBDTscore(const xAOD::TauJet* tau, const Trk::Track* track, const Trk::Track* leadtrack ) const
{
  std::map<TString, float*> BDTvars;
  BDTInputVariables vars;
    
  BDTvars = {
    {"log(Coretrack_pt)", &vars.logtrk_pt}, 
    {"fabs(Coretrack_z0)", &vars.abstrck_z0},
    {"fabs(Coretrack_d0)", &vars.abstrk_d0},
    {"Coretrack_nPiHits", &vars.trk_nPiHits},
    {"Coretrack_nSiHoles", &vars.trk_nSiHoles},
    {"log(Coretrack_ratioptCalo)", &vars.logtrk_ratiopt},
    {"Coretrack_dR", &vars.trk_dR},
    {"Coretrack_dRleadtrk", &vars.trk_dRtoleadtrk},
    {"Coretrack_CaloHadpt", &vars.CaloHad_pt},
    {"Coretrack_CaloEMpt", &vars.CaloEM_pt}
  };

  const Trk::Perigee* trkPerigee = track->perigeeParameters();
  const Trk::TrackSummary* trkSummary = track->trackSummary();

  int nPixHit = trkSummary->get(Trk::numberOfPixelHits);
  int nPixDead = trkSummary->get(Trk::numberOfPixelDeadSensors);

  int nPixHole = trkSummary->get(Trk::numberOfPixelHoles);
  int nSCTHole = trkSummary->get(Trk::numberOfSCTHoles);

  float ratio_pt = (tau->pt()>0.) ? trkPerigee->pT()/tau->pt() : 0.;

  float dEta = tau->eta() - trkPerigee->eta();
  float dPhi = CxxUtils::wrapToPi(tau->phi() - trkPerigee->parameters()[Trk::phi0]);
  float dR = std::sqrt(dEta*dEta + dPhi*dPhi);

  const Trk::Perigee* leadtrkPerigee = leadtrack->perigeeParameters();
  float dEta_leadtrk = trkPerigee->eta() - leadtrkPerigee->eta();
  float dPhi_leadtrk = CxxUtils::wrapToPi(trkPerigee->parameters()[Trk::phi0] - leadtrkPerigee->parameters()[Trk::phi0]);
  float dR_leadtrk = std::sqrt(dEta_leadtrk*dEta_leadtrk + dPhi_leadtrk*dPhi_leadtrk);

  float tau_emscale_ptEM = 0.;
  float tau_emscale_ptHad = 0.;
  if ( !tau->detail( xAOD::TauJetParameters::etEMAtEMScale, tau_emscale_ptEM ) ) {
    ATH_MSG_WARNING("Retrieval of tau etEMAtEMScale detail failed.");
  }
  if ( !tau->detail( xAOD::TauJetParameters::etHadAtEMScale, tau_emscale_ptHad ) ) {
    ATH_MSG_WARNING("Retrieval of tau etHadAtEMScale detail failed.");
  }

  vars.logtrk_pt = std::log( trkPerigee->pT() );
  vars.abstrck_z0 = std::abs( trkPerigee->parameters()[Trk::z0] );
  vars.abstrk_d0 = std::abs( trkPerigee->parameters()[Trk::d0] );
  vars.trk_nPiHits = nPixHit + nPixDead;
  vars.trk_nSiHoles = nPixHole + nSCTHole;
  vars.logtrk_ratiopt = std::log( ratio_pt );
  vars.trk_dR = dR;
  vars.trk_dRtoleadtrk = dR_leadtrk;
  vars.CaloHad_pt = tau_emscale_ptHad;
  vars.CaloEM_pt = tau_emscale_ptEM;

  double BDTval = m_reader->getResponse(BDTvars);

  return BDTval;
}
