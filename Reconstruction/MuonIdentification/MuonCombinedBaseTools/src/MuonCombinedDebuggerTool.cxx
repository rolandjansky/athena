/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCombinedDebuggerTool
//  AlgTool to dump ROOT TTree with debugging information to tune muon combined
//  fit preselection
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#include "MuonCombinedDebuggerTool.h"

#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "AthLinks/ElementLink.h"

#include <TTree.h>

namespace MuonCombined {
 
  MuonCombinedDebuggerTool::MuonCombinedDebuggerTool (const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_histSvc("THistSvc", name),
    m_recoTree(nullptr)
  {  
    declareInterface<MuonCombinedDebuggerTool>(this);
  }

  StatusCode MuonCombinedDebuggerTool::initialize() {
    ATH_CHECK(m_matchQuality.retrieve());
    ATH_CHECK(m_truthToTrack.retrieve());
    ATH_CHECK(m_histSvc.retrieve());
    return StatusCode::SUCCESS;
  }

  void MuonCombinedDebuggerTool::bookBranches() 
  {
    if(m_recoTree) {
      ATH_MSG_WARNING("bookBranches() must be called once only! No tree booked!");    
      return;
    }
    
    m_recoTree = new TTree("reco","reco");

    if(m_histSvc->regTree("/RECODEBUG/reco", m_recoTree).isFailure())
      ATH_MSG_WARNING("problems registering TTree to THistSvc!");    

    m_recoTree->Branch("eventNumber",  &m_eventNumber, "eventNumber/I");
    
    //m_recoTree->Branch("mstrack_truth_nprecLayers",  &m_mstrack_truth_nprecLayers);
    //m_recoTree->Branch("mstrack_nprecLayers",  &m_mstrack_nprecLayers);
    
    m_recoTree->Branch("mstrack_has_truth", &m_mstrack_has_truth);
    m_recoTree->Branch("mstrack_has_truth_par", &m_mstrack_has_truth_par);    
    m_recoTree->Branch("mstrack_has_sa", &m_mstrack_has_sa);

    m_recoTree->Branch("mstrack_truth_id", &m_mstrack_truth_id);
    m_recoTree->Branch("mstrack_truth_barcode", &m_mstrack_truth_barcode);
    m_recoTree->Branch("mstrack_truth_sur_x", &m_mstrack_truth_sur_x);
    m_recoTree->Branch("mstrack_truth_sur_y", &m_mstrack_truth_sur_y); 
    m_recoTree->Branch("mstrack_truth_sur_z", &m_mstrack_truth_sur_z);
    m_recoTree->Branch("mstrack_truth_d0", &m_mstrack_truth_d0);    
    m_recoTree->Branch("mstrack_truth_z0", &m_mstrack_truth_z0);    
    m_recoTree->Branch("mstrack_truth_phi0", &m_mstrack_truth_phi0);    
    m_recoTree->Branch("mstrack_truth_theta", &m_mstrack_truth_theta);    
    m_recoTree->Branch("mstrack_truth_qOverP", &m_mstrack_truth_qOverP);    

    m_recoTree->Branch("mstrack_sur_x", &m_mstrack_sur_x);
    m_recoTree->Branch("mstrack_sur_y", &m_mstrack_sur_y); 
    m_recoTree->Branch("mstrack_sur_z", &m_mstrack_sur_z);
    m_recoTree->Branch("mstrack_d0", &m_mstrack_d0);
    m_recoTree->Branch("mstrack_z0", &m_mstrack_z0);     	
    m_recoTree->Branch("mstrack_phi0", &m_mstrack_phi0);     	
    m_recoTree->Branch("mstrack_theta", &m_mstrack_theta);      
    m_recoTree->Branch("mstrack_qOverP", &m_mstrack_qOverP);     
    m_recoTree->Branch("mstrack_cov_d0", &m_mstrack_cov_d0);     
    m_recoTree->Branch("mstrack_cov_z0", &m_mstrack_cov_z0);      
    m_recoTree->Branch("mstrack_cov_phi0", &m_mstrack_cov_phi0);     
    m_recoTree->Branch("mstrack_cov_theta", &m_mstrack_cov_theta);      
    m_recoTree->Branch("mstrack_cov_qOverP", &m_mstrack_cov_qOverP);

    m_recoTree->Branch("mstrack_sa_sur_x", &m_mstrack_sa_sur_x);
    m_recoTree->Branch("mstrack_sa_sur_y", &m_mstrack_sa_sur_y); 
    m_recoTree->Branch("mstrack_sa_sur_z", &m_mstrack_sa_sur_z);
    m_recoTree->Branch("mstrack_sa_d0", &m_mstrack_sa_d0);
    m_recoTree->Branch("mstrack_sa_z0", &m_mstrack_sa_z0);     	
    m_recoTree->Branch("mstrack_sa_phi0", &m_mstrack_sa_phi0);     	
    m_recoTree->Branch("mstrack_sa_theta", &m_mstrack_sa_theta);      
    m_recoTree->Branch("mstrack_sa_qOverP", &m_mstrack_sa_qOverP);     
    m_recoTree->Branch("mstrack_sa_cov_d0", &m_mstrack_sa_cov_d0);     
    m_recoTree->Branch("mstrack_sa_cov_z0", &m_mstrack_sa_cov_z0);      
    m_recoTree->Branch("mstrack_sa_cov_phi0", &m_mstrack_sa_cov_phi0);     
    m_recoTree->Branch("mstrack_sa_cov_theta", &m_mstrack_sa_cov_theta);      
    m_recoTree->Branch("mstrack_sa_cov_qOverP", &m_mstrack_sa_cov_qOverP);

    m_recoTree->Branch("idtrack_truth_id", &m_idtrack_truth_id);    
    m_recoTree->Branch("idtrack_truth_barcode", &m_idtrack_truth_barcode);    
    m_recoTree->Branch("idtrack_has_truth", &m_idtrack_has_truth);    
    m_recoTree->Branch("idtrack_has_truth_par", &m_idtrack_has_truth_par);    
    m_recoTree->Branch("idtrack_truth_sur_x", &m_idtrack_truth_sur_x);
    m_recoTree->Branch("idtrack_truth_sur_y", &m_idtrack_truth_sur_y); 
    m_recoTree->Branch("idtrack_truth_sur_z", &m_idtrack_truth_sur_z);
    m_recoTree->Branch("idtrack_truth_d0", &m_idtrack_truth_d0);    
    m_recoTree->Branch("idtrack_truth_z0", &m_idtrack_truth_z0);    
    m_recoTree->Branch("idtrack_truth_phi0", &m_idtrack_truth_phi0);    
    m_recoTree->Branch("idtrack_truth_theta", &m_idtrack_truth_theta);    
    m_recoTree->Branch("idtrack_truth_qOverP", &m_idtrack_truth_qOverP);    

    m_recoTree->Branch("idtrack_sur_x", &m_idtrack_sur_x);
    m_recoTree->Branch("idtrack_sur_y", &m_idtrack_sur_y); 
    m_recoTree->Branch("idtrack_sur_z", &m_idtrack_sur_z);
    m_recoTree->Branch("idtrack_d0", &m_idtrack_d0);
    m_recoTree->Branch("idtrack_z0", &m_idtrack_z0);     	
    m_recoTree->Branch("idtrack_phi0", &m_idtrack_phi0);     	
    m_recoTree->Branch("idtrack_theta", &m_idtrack_theta);      
    m_recoTree->Branch("idtrack_qOverP", &m_idtrack_qOverP);     
    m_recoTree->Branch("idtrack_cov_d0", &m_idtrack_cov_d0);     
    m_recoTree->Branch("idtrack_cov_z0", &m_idtrack_cov_z0);      
    m_recoTree->Branch("idtrack_cov_phi0", &m_idtrack_cov_phi0);     
    m_recoTree->Branch("idtrack_cov_theta", &m_idtrack_cov_theta);      
    m_recoTree->Branch("idtrack_cov_qOverP", &m_idtrack_cov_qOverP);

    m_recoTree->Branch("ms_id_ichi2", &m_ms_id_ichi2);
    m_recoTree->Branch("ms_id_indf", &m_ms_id_indf);
    m_recoTree->Branch("ms_id_iprob", &m_ms_id_iprob);
    m_recoTree->Branch("ms_id_ochi2", &m_ms_id_ochi2);
    m_recoTree->Branch("ms_id_ondf", &m_ms_id_ondf);
    m_recoTree->Branch("ms_id_oprob", &m_ms_id_oprob);
  }
  
  void MuonCombinedDebuggerTool::fillBranches(const MuonCandidateCollection& muonCandidates, 
					      const InDetCandidateCollection& inDetCandidates) const { 
    fillMsBranches(muonCandidates);
    fillIdBranches(inDetCandidates);
    fillMsIdBranches(muonCandidates, inDetCandidates);
    if (m_recoTree->Fill() == -1) {
      ATH_MSG_WARNING("problems writing reco debugging ttree!");
    }
  }

  void MuonCombinedDebuggerTool::fillMsIdBranches(const MuonCandidateCollection& muonCandidates, 
						  const InDetCandidateCollection& inDetCandidates) const {
    m_ms_id_ochi2.clear();
    m_ms_id_ondf.clear();
    m_ms_id_oprob.clear();
    m_ms_id_ichi2.clear();
    m_ms_id_indf.clear();
    m_ms_id_iprob.clear();
    m_ms_id_ochi2.resize(muonCandidates.size());
    m_ms_id_ondf.resize(muonCandidates.size());
    m_ms_id_oprob.resize(muonCandidates.size());
    m_ms_id_ichi2.resize(muonCandidates.size());
    m_ms_id_indf.resize(muonCandidates.size());
    m_ms_id_iprob.resize(muonCandidates.size());

    unsigned int imu=0;
    for(auto muonCandidate : muonCandidates) {
      bool hasExtr = muonCandidate->extrapolatedTrack();

      for(auto inDetCandidate : inDetCandidates) {

	// matching chi2s
	double outerMatchChi2 = m_matchQuality->outerMatchChi2(*inDetCandidate->indetTrackParticle().track(), muonCandidate->muonSpectrometerTrack());
	int outerMatchDoF     = m_matchQuality->outerMatchDOF(*inDetCandidate->indetTrackParticle().track(), muonCandidate->muonSpectrometerTrack());
	double outerMatchProb = m_matchQuality->outerMatchProbability(*inDetCandidate->indetTrackParticle().track(), muonCandidate->muonSpectrometerTrack());      
	double innerMatchChi2 = -1;
	int innerMatchDoF     = -1;
	double innerMatchProb = -1;
	if(hasExtr) {
	  innerMatchChi2 = m_matchQuality->innerMatchChi2(*inDetCandidate->indetTrackParticle().track(), *muonCandidate->extrapolatedTrack());
	  innerMatchDoF  = m_matchQuality->innerMatchDOF(*inDetCandidate->indetTrackParticle().track(), *muonCandidate->extrapolatedTrack());
	  innerMatchProb = m_matchQuality->innerMatchProbability(*inDetCandidate->indetTrackParticle().track(), *muonCandidate->extrapolatedTrack());      		
	}
            
	m_ms_id_ochi2[imu].push_back(outerMatchChi2);
	m_ms_id_ondf[imu].push_back(outerMatchDoF);
	m_ms_id_oprob[imu].push_back(outerMatchProb);
	if(hasExtr) {
	  m_ms_id_ichi2[imu].push_back(innerMatchChi2);
	  m_ms_id_indf[imu].push_back(innerMatchDoF);
	  m_ms_id_iprob[imu].push_back(innerMatchProb);
	}else{
	  m_ms_id_ichi2[imu].push_back(-1);
	  m_ms_id_indf[imu].push_back(-1);
	  m_ms_id_iprob[imu].push_back(-1);	
	}
      
      }
      imu++;
    }
  }

  void MuonCombinedDebuggerTool::fillIdBranches(const InDetCandidateCollection& inDetCandidates) const {
    // truth
    m_idtrack_truth_id.clear();
    m_idtrack_truth_barcode.clear();
    m_idtrack_has_truth.clear();
    m_idtrack_has_truth_par.clear();
    m_idtrack_truth_sur_x.clear();
    m_idtrack_truth_sur_y.clear(); 
    m_idtrack_truth_sur_z.clear();
    m_idtrack_truth_d0.clear();
    m_idtrack_truth_z0.clear();
    m_idtrack_truth_phi0.clear();
    m_idtrack_truth_theta.clear();
    m_idtrack_truth_qOverP.clear();
  
    // id track
    m_idtrack_sur_x.clear();
    m_idtrack_sur_y.clear(); 
    m_idtrack_sur_z.clear();
    m_idtrack_d0.clear();
    m_idtrack_z0.clear();
    m_idtrack_phi0.clear();
    m_idtrack_theta.clear();
    m_idtrack_qOverP.clear();
    m_idtrack_cov_d0.clear();
    m_idtrack_cov_z0.clear();
    m_idtrack_cov_phi0.clear();
    m_idtrack_cov_theta.clear();
    m_idtrack_cov_qOverP.clear();

    for(auto inDetCandidate : inDetCandidates) {
    
      // id track
      m_idtrack_sur_x.push_back( inDetCandidate->indetTrackParticle().track()->perigeeParameters()->associatedSurface().center().x() );
      m_idtrack_sur_y.push_back( inDetCandidate->indetTrackParticle().track()->perigeeParameters()->associatedSurface().center().y() );
      m_idtrack_sur_z.push_back( inDetCandidate->indetTrackParticle().track()->perigeeParameters()->associatedSurface().center().z() );
      m_idtrack_d0.push_back( inDetCandidate->indetTrackParticle().track()->perigeeParameters()->parameters()[Trk::d0] );
      m_idtrack_z0.push_back( inDetCandidate->indetTrackParticle().track()->perigeeParameters()->parameters()[Trk::z0] );
      m_idtrack_phi0.push_back( inDetCandidate->indetTrackParticle().track()->perigeeParameters()->parameters()[Trk::phi0] );
      m_idtrack_theta.push_back( inDetCandidate->indetTrackParticle().track()->perigeeParameters()->parameters()[Trk::theta] );
      m_idtrack_qOverP.push_back( inDetCandidate->indetTrackParticle().track()->perigeeParameters()->parameters()[Trk::qOverP] );
      const AmgSymMatrix(5) * covid = inDetCandidate->indetTrackParticle().track()->perigeeParameters()->covariance();
      if(covid) {
	m_idtrack_cov_d0.push_back( (*covid)(Trk::d0,Trk::d0) );
	m_idtrack_cov_z0.push_back( (*covid)(Trk::z0,Trk::z0) );
	m_idtrack_cov_phi0.push_back( (*covid)(Trk::phi0,Trk::phi0) );
	m_idtrack_cov_theta.push_back( (*covid)(Trk::theta,Trk::theta) );
	m_idtrack_cov_qOverP.push_back( (*covid)(Trk::qOverP,Trk::qOverP) );
      }else{
	m_idtrack_cov_d0.push_back( -1 );
	m_idtrack_cov_z0.push_back( -1 );
	m_idtrack_cov_phi0.push_back( -1 );
	m_idtrack_cov_theta.push_back( -1 );
	m_idtrack_cov_qOverP.push_back( -1 );      
      }
    
    
      // truth
      if(inDetCandidate->indetTrackParticle().isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
	ElementLink<xAOD::TruthParticleContainer> truthLink = inDetCandidate->indetTrackParticle().auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
	if(truthLink.isValid()) {
	  m_idtrack_has_truth.push_back(1);
	  m_idtrack_truth_id.push_back((*truthLink)->pdgId());
	  m_idtrack_truth_barcode.push_back((*truthLink)->barcode());
	  const Trk::TrackParameters *perigee = m_truthToTrack->makePerigeeParameters(*truthLink);
	  if(perigee) {
	    m_idtrack_has_truth_par.push_back(1);
	    m_idtrack_truth_sur_x.push_back( perigee->associatedSurface().center().x() );
	    m_idtrack_truth_sur_y.push_back( perigee->associatedSurface().center().y() );
	    m_idtrack_truth_sur_z.push_back( perigee->associatedSurface().center().z() );
	    m_idtrack_truth_d0.push_back(perigee->parameters()[Trk::d0]);
	    m_idtrack_truth_z0.push_back(perigee->parameters()[Trk::z0]);
	    m_idtrack_truth_phi0.push_back(perigee->parameters()[Trk::phi0]);
	    m_idtrack_truth_theta.push_back(perigee->parameters()[Trk::theta]);
	    m_idtrack_truth_qOverP.push_back(perigee->parameters()[Trk::qOverP]);
	  }else{
	    m_idtrack_has_truth_par.push_back(0);
	    m_idtrack_truth_sur_x.push_back( -1 );
	    m_idtrack_truth_sur_y.push_back( -1 );
	    m_idtrack_truth_sur_z.push_back( -1 );
	    m_idtrack_truth_d0.push_back(-1);
	    m_idtrack_truth_z0.push_back(-1);
	    m_idtrack_truth_phi0.push_back(-1);
	    m_idtrack_truth_theta.push_back(-1);
	    m_idtrack_truth_qOverP.push_back(-1);	
	  }
	}else{
	  m_idtrack_has_truth.push_back(0);
	  m_idtrack_has_truth_par.push_back(0);
	  m_idtrack_truth_id.push_back(-1);
	  m_idtrack_truth_barcode.push_back(-1);
	  m_idtrack_truth_sur_x.push_back( -1 );
	  m_idtrack_truth_sur_y.push_back( -1 );
	  m_idtrack_truth_sur_z.push_back( -1 );
	  m_idtrack_truth_d0.push_back(-1);
	  m_idtrack_truth_z0.push_back(-1);
	  m_idtrack_truth_phi0.push_back(-1);
	  m_idtrack_truth_theta.push_back(-1);
	  m_idtrack_truth_qOverP.push_back(-1);	
	}
      }else{
	m_idtrack_has_truth.push_back(0);
	m_idtrack_has_truth_par.push_back(0);
	m_idtrack_truth_id.push_back(-1);
	m_idtrack_truth_barcode.push_back(-1);
	m_idtrack_truth_sur_x.push_back( -1 );
	m_idtrack_truth_sur_y.push_back( -1 );
	m_idtrack_truth_sur_z.push_back( -1 );
	m_idtrack_truth_d0.push_back(-1);
	m_idtrack_truth_z0.push_back(-1);
	m_idtrack_truth_phi0.push_back(-1);
	m_idtrack_truth_theta.push_back(-1);
	m_idtrack_truth_qOverP.push_back(-1);	
      }      
    }
  }

  void MuonCombinedDebuggerTool::fillMsBranches(const MuonCandidateCollection& muonCandidates) const {
    // ms track
    m_mstrack_sur_x.clear();
    m_mstrack_sur_y.clear(); 
    m_mstrack_sur_z.clear();
    m_mstrack_d0.clear();
    m_mstrack_z0.clear();
    m_mstrack_phi0.clear();
    m_mstrack_theta.clear();
    m_mstrack_qOverP.clear();
    m_mstrack_cov_d0.clear();
    m_mstrack_cov_z0.clear();
    m_mstrack_cov_phi0.clear();
    m_mstrack_cov_theta.clear();
    m_mstrack_cov_qOverP.clear();

    // sa track
    m_mstrack_has_sa.clear();
    m_mstrack_sa_sur_x.clear();
    m_mstrack_sa_sur_y.clear(); 
    m_mstrack_sa_sur_z.clear();
    m_mstrack_sa_d0.clear();
    m_mstrack_sa_z0.clear();
    m_mstrack_sa_phi0.clear();
    m_mstrack_sa_theta.clear();
    m_mstrack_sa_qOverP.clear();
    m_mstrack_sa_cov_d0.clear();
    m_mstrack_sa_cov_z0.clear();
    m_mstrack_sa_cov_phi0.clear();
    m_mstrack_sa_cov_theta.clear();
    m_mstrack_sa_cov_qOverP.clear();

    // truth
    m_mstrack_has_truth.clear();
    m_mstrack_has_truth_par.clear();
    m_mstrack_truth_id.clear();
    m_mstrack_truth_barcode.clear();
    m_mstrack_truth_sur_x.clear();
    m_mstrack_truth_sur_y.clear(); 
    m_mstrack_truth_sur_z.clear();
    m_mstrack_truth_d0.clear();
    m_mstrack_truth_z0.clear();
    m_mstrack_truth_phi0.clear();
    m_mstrack_truth_theta.clear();
    m_mstrack_truth_qOverP.clear();

    for(auto muonCandidate : muonCandidates) {

      bool hasExtr = muonCandidate->extrapolatedTrack();
      const Trk::TrackParameters* muonPars = hasExtr ? 
	muonCandidate->extrapolatedTrack()->perigeeParameters() : muonCandidate->muonSpectrometerTrack().perigeeParameters();
      if( !muonPars ){
	ATH_MSG_WARNING("MuonCandidate without Perigee, skipping");
	return;
      }
    
      // ms track
      m_mstrack_sur_x.push_back( muonCandidate->muonSpectrometerTrack().perigeeParameters()->associatedSurface().center().x() );
      m_mstrack_sur_y.push_back( muonCandidate->muonSpectrometerTrack().perigeeParameters()->associatedSurface().center().y() );
      m_mstrack_sur_z.push_back( muonCandidate->muonSpectrometerTrack().perigeeParameters()->associatedSurface().center().z() );
      m_mstrack_d0.push_back( muonCandidate->muonSpectrometerTrack().perigeeParameters()->parameters()[Trk::d0] );
      m_mstrack_z0.push_back( muonCandidate->muonSpectrometerTrack().perigeeParameters()->parameters()[Trk::z0] );
      m_mstrack_phi0.push_back( muonCandidate->muonSpectrometerTrack().perigeeParameters()->parameters()[Trk::phi0] );
      m_mstrack_theta.push_back( muonCandidate->muonSpectrometerTrack().perigeeParameters()->parameters()[Trk::theta] );
      m_mstrack_qOverP.push_back( muonCandidate->muonSpectrometerTrack().perigeeParameters()->parameters()[Trk::qOverP] );
      const AmgSymMatrix(5) * cov = muonCandidate->muonSpectrometerTrack().perigeeParameters()->covariance();
      if(cov) {
	m_mstrack_cov_d0.push_back( (*cov)(Trk::d0,Trk::d0) );
	m_mstrack_cov_z0.push_back( (*cov)(Trk::z0,Trk::z0) );
	m_mstrack_cov_phi0.push_back( (*cov)(Trk::phi0,Trk::phi0) );
	m_mstrack_cov_theta.push_back( (*cov)(Trk::theta,Trk::theta) );
	m_mstrack_cov_qOverP.push_back( (*cov)(Trk::qOverP,Trk::qOverP) );
      }else{
	m_mstrack_cov_d0.push_back( -1 );
	m_mstrack_cov_z0.push_back( -1 );
	m_mstrack_cov_phi0.push_back( -1 );
	m_mstrack_cov_theta.push_back( -1 );
	m_mstrack_cov_qOverP.push_back( -1 );    
      }

      // sa track
      m_mstrack_has_sa.push_back(hasExtr);
      if (hasExtr) {      
	m_mstrack_sa_sur_x.push_back( muonPars->associatedSurface().center().x() );
	m_mstrack_sa_sur_y.push_back( muonPars->associatedSurface().center().y() );
	m_mstrack_sa_sur_z.push_back( muonPars->associatedSurface().center().z() );
	m_mstrack_sa_d0.push_back( muonPars->parameters()[Trk::d0] );
	m_mstrack_sa_z0.push_back( muonPars->parameters()[Trk::z0] );
	m_mstrack_sa_phi0.push_back( muonPars->parameters()[Trk::phi0] );
	m_mstrack_sa_theta.push_back( muonPars->parameters()[Trk::theta] );
	m_mstrack_sa_qOverP.push_back( muonPars->parameters()[Trk::qOverP] );
	const AmgSymMatrix(5) * cov = muonPars->covariance();
	if(cov) {
	  m_mstrack_sa_cov_d0.push_back( (*cov)(Trk::d0,Trk::d0) );
	  m_mstrack_sa_cov_z0.push_back( (*cov)(Trk::z0,Trk::z0) );
	  m_mstrack_sa_cov_phi0.push_back( (*cov)(Trk::phi0,Trk::phi0) );
	  m_mstrack_sa_cov_theta.push_back( (*cov)(Trk::theta,Trk::theta) );
	  m_mstrack_sa_cov_qOverP.push_back( (*cov)(Trk::qOverP,Trk::qOverP) );
	}
      }else{
	m_mstrack_sa_sur_x.push_back( -1 );
	m_mstrack_sa_sur_y.push_back( -1 );
	m_mstrack_sa_sur_z.push_back( -1 );
	m_mstrack_sa_d0.push_back( -1 );
	m_mstrack_sa_z0.push_back( -1 );
	m_mstrack_sa_phi0.push_back( -1 );
	m_mstrack_sa_theta.push_back( -1 );
	m_mstrack_sa_qOverP.push_back( -1 );
	m_mstrack_sa_cov_d0.push_back( -1 );
	m_mstrack_sa_cov_z0.push_back( -1 );
	m_mstrack_sa_cov_phi0.push_back( -1 );
	m_mstrack_sa_cov_theta.push_back( -1 );
	m_mstrack_sa_cov_qOverP.push_back( -1 );
      }
    
      // truth
      if((*muonCandidate->muonSpectrometerTrackLink())->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
	ElementLink<xAOD::TruthParticleContainer> truthLink = (*muonCandidate->muonSpectrometerTrackLink())->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
	if(truthLink.isValid()) {
	  m_mstrack_has_truth.push_back(1);
	  m_mstrack_truth_id.push_back((*truthLink)->pdgId());
	  m_mstrack_truth_barcode.push_back((*truthLink)->barcode());
	  const Trk::TrackParameters *perigee = m_truthToTrack->makePerigeeParameters(*truthLink);
	  if(perigee) {
	    m_mstrack_has_truth_par.push_back(1);
	    m_mstrack_truth_sur_x.push_back( perigee->associatedSurface().center().x() );
	    m_mstrack_truth_sur_y.push_back( perigee->associatedSurface().center().y() );
	    m_mstrack_truth_sur_z.push_back( perigee->associatedSurface().center().z() );
	    m_mstrack_truth_d0.push_back(perigee->parameters()[Trk::d0]);
	    m_mstrack_truth_z0.push_back(perigee->parameters()[Trk::z0]);
	    m_mstrack_truth_phi0.push_back(perigee->parameters()[Trk::phi0]);
	    m_mstrack_truth_theta.push_back(perigee->parameters()[Trk::theta]);
	    m_mstrack_truth_qOverP.push_back(perigee->parameters()[Trk::qOverP]);
	  }else{
	    m_mstrack_has_truth_par.push_back(0);
	    m_mstrack_truth_sur_x.push_back( -1 );
	    m_mstrack_truth_sur_y.push_back( -1 );
	    m_mstrack_truth_sur_z.push_back( -1 );
	    m_mstrack_truth_d0.push_back(-1);
	    m_mstrack_truth_z0.push_back(-1);
	    m_mstrack_truth_phi0.push_back(-1);
	    m_mstrack_truth_theta.push_back(-1);
	    m_mstrack_truth_qOverP.push_back(-1);	
	  }
	  //truth hit info
	  //m_mstrack_truth_nprecLayers.push_back( (*truthLink)->isAvailable<uint8_t>("nprecLayers") ? -1 : (*truthLink)->auxdata<uint8_t>("nprecLayers") );
	}else{ // no truth link available
	  m_mstrack_has_truth.push_back(0);
	  m_mstrack_has_truth_par.push_back(0);
	  m_mstrack_truth_id.push_back(0);
	  m_mstrack_truth_barcode.push_back(0);
	  m_mstrack_truth_sur_x.push_back( -1 );
	  m_mstrack_truth_sur_y.push_back( -1 );
	  m_mstrack_truth_sur_z.push_back( -1 );
	  m_mstrack_truth_d0.push_back(-1);
	  m_mstrack_truth_z0.push_back(-1);
	  m_mstrack_truth_phi0.push_back(-1);
	  m_mstrack_truth_theta.push_back(-1);
	  m_mstrack_truth_qOverP.push_back(-1);	
	}
      }else{ // no truth link available
	m_mstrack_has_truth.push_back(0);
	m_mstrack_has_truth_par.push_back(0);
	m_mstrack_truth_id.push_back(0);
	m_mstrack_truth_barcode.push_back(0);
	m_mstrack_truth_sur_x.push_back( -1 );
	m_mstrack_truth_sur_y.push_back( -1 );
	m_mstrack_truth_sur_z.push_back( -1 );
	m_mstrack_truth_d0.push_back(-1);
	m_mstrack_truth_z0.push_back(-1);
	m_mstrack_truth_phi0.push_back(-1);
	m_mstrack_truth_theta.push_back(-1);
	m_mstrack_truth_qOverP.push_back(-1);		  
      }
    }
  }

}	// end of namespace


