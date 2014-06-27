/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "TrackFilter.h"
#include "Particle/TrackParticle.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "Particle/TrackParticleContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/egammaPIDdefs.h"
#include "DataModel/ConstDataVector.h"
#include "muonEvent/MuonContainer.h"

#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "D3PDMakerInterfaces/IObjGetterTool.h"

#include "ParticleEvent/TrackParticleAssocs.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
 
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/errorcheck.h"

#include "StoreGate/StoreGateSvc.h" 

#define PI 2.*asin(1.)

namespace D3PD{
TrackFilter::TrackFilter(const std::string& name, 
			 ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator), 
  m_inputTracksName("TrackParticleCandidate"),
  m_outputTracksName("SelectedTracks"),
  m_ptCut(10000),
  m_etaCut(2.5),
  m_deltaRCut(.6),
  m_photonIsEM(egammaPID::PhotonLoose),
  m_electronIsEM(egammaPID::ElectronTight),
  m_storePerpTrk(true),
  m_storeGate(0)
{ 
  declareProperty("InputTrackCollectionName" ,m_inputTracksName);
  declareProperty("OutputTrackCollectionName",m_outputTracksName);
  declareProperty("photonGetter"             ,m_ph_getter);
  declareProperty("electronGetter"           ,m_el_getter);
  declareProperty("muonGetter"               ,m_mu_getter);
  declareProperty("CheckGSFTracks"           ,m_check_gsf=false);
  declareProperty("GSFTrackParticleGetter"   ,m_gsf_getter);
  declareProperty("GSFAssocGetter"           ,m_gsf_assoc_getter);
  declareProperty("ptCut"                    ,m_ptCut);
  declareProperty("etaCut"                   ,m_etaCut);
  declareProperty("deltaRCut"                ,m_deltaRCut);
  declareProperty("photonIsEM"               ,m_photonIsEM);
  declareProperty("electronIsEM"             ,m_electronIsEM);
  declareProperty("storePerpendicularTracks" ,m_storePerpTrk);
  declareProperty("FilterTracksForMET"       ,m_filterTracksForMET=false);
}

TrackFilter::~TrackFilter()
{ }

StatusCode TrackFilter::initialize()
{

  MsgStream log(msgSvc(), name());


  StatusCode sc;
   
   // get StoreGate 
  sc = service("StoreGateSvc", m_storeGate);
  if ( sc == StatusCode::FAILURE ) {
    log<<MSG::ERROR << "   could not access StoreGate " << endreq;
    return StatusCode::FAILURE;
  }
 
  // Retrieve the getter tool and configure it, saying that we want
  // to get @c PAU::egamma objects from it.
  CHECK( m_ph_getter.retrieve() );
  CHECK( m_ph_getter->configureD3PD<Analysis::Photon>() );

  // Retrieve the getter tool and configure it, saying that we want
  // to get @c PAU::egamma objects from it.
  CHECK( m_el_getter.retrieve() );
  CHECK( m_el_getter->configureD3PD<Analysis::Electron>() );

  // Retrieve the getter tool and configure it, saying that we want
  // to get @c Analysis::Muon objects from it.
  CHECK( m_mu_getter.retrieve() );
  CHECK( m_mu_getter->configureD3PD<Analysis::Muon>() );

  if(m_check_gsf){
    CHECK( m_gsf_getter.retrieve() );
    CHECK( m_gsf_getter->configureD3PD<Rec::TrackParticle>() );
    
    CHECK( m_gsf_assoc_getter.retrieve() );
    CHECK( m_gsf_assoc_getter->configureD3PD<TrackParticleAssocs> () );
  }

  return StatusCode::SUCCESS;
    
}


StatusCode TrackFilter::finalize()
{

 return StatusCode::SUCCESS;
}

///////////////
StatusCode TrackFilter::execute()
{
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << " in execute" << endreq;

  // Get the input tracks container
  const Rec::TrackParticleContainer* trkcoll ;
  StatusCode sc = m_storeGate->retrieve(trkcoll, m_inputTracksName) ; 
  if(StatusCode::SUCCESS != sc ){
    log<<MSG::ERROR<<" unable to retrieve TrackParticle container: "<< m_inputTracksName <<endreq; 
    return sc;
  }

  // Record the output tracks container
  ConstDataVector<Rec::TrackParticleContainer>* outputCont =
    new ConstDataVector<Rec::TrackParticleContainer>(SG::VIEW_ELEMENTS) ;
  sc = m_storeGate->record(outputCont, m_outputTracksName) ; 
  if(StatusCode::SUCCESS != sc ) 
    {
      log<<MSG::ERROR<<" unable to record Tracks container: "<< m_outputTracksName <<endreq; 
      return sc;
    }

  // clean vectors
  etas.clear();
  phis.clear();

  // a vector to keep track of the tracks from electrons, muons and photons that pass our basic selection
  std::vector<const Rec::TrackParticle*> electron_muon_photon_tracks;

  // Get the photons
  if (m_ph_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  // now, loop over all the photons, store the eta and phi positions of qualifying photons
  //const Analysis::Photon* eout = 0;
  while ( const Analysis::Photon* obj = m_ph_getter->next<Analysis::Photon>()) {

    // keep all tracks corresponding to conversions.
    int nConversions = obj->nConversions();
    for(int conv_iter=0; conv_iter<nConversions; conv_iter++){
      const Trk::VxCandidate* vtx = obj->conversion(conv_iter);
      if(vtx){
	const std::vector<Trk::VxTrackAtVertex*>* trklist = vtx->vxTrackAtVertex();
	for(unsigned int trkiter = 0; trkiter < trklist->size(); trkiter++){
	  Trk::VxTrackAtVertex* trk = (*trklist)[trkiter];
	  Trk::ITrackLink* link = trk->trackOrParticleLink();
	  const Trk::LinkToTrackParticleBase* tr_part = dynamic_cast<const Trk::LinkToTrackParticleBase*>(link);
	  if(!tr_part || !(*tr_part)) continue;
	  const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(**tr_part);
	  if(tp) electron_muon_photon_tracks.push_back(tp);
	}
      }
    }

    // only keep the rest of the tracks if they're nearby an interesting photon
    if(fabs(obj->cluster()->etaBE(2))>m_etaCut) continue;
    if(obj->cluster()->pt() < m_ptCut) continue;
    if(obj->isem(m_photonIsEM)!=0) continue;

    fillDirection((egamma*)obj);
  }

  // Get the electrons
  if (m_el_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  // now, loop over all the electrons, store the eta and phi positions of qualifying electrons
  //const Analysis::Electron* eout = 0;
  while ( const Analysis::Electron* obj = m_el_getter->next<Analysis::Electron>()) {

    // keep all tracks corresponding to electrons.
    if(obj->trackParticle()){
      electron_muon_photon_tracks.push_back(obj->trackParticle());
    }

    // only keep the rest of the tracks if they're nearby an interesting electron
    if(fabs(obj->cluster()->etaBE(2))>m_etaCut) continue;
    if(obj->cluster()->pt() < m_ptCut) continue;
    if(obj->isem(m_electronIsEM)!=0) continue;

    fillDirection((egamma*)obj);
  }

  // Get the muons
  if (m_mu_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  // now, loop over all the muons, store the eta and phi positions of qualifying muons
  //const Analysis::Muon* eout = 0;
  while ( const Analysis::Muon* obj = m_mu_getter->next<Analysis::Muon>()) {

    // keep all tracks corresponding to muons.
    if(obj->track()){
      electron_muon_photon_tracks.push_back(obj->track());
    }

    // only keep the rest of the tracks if they're nearby an interesting muons
    if(obj->pt() < m_ptCut) continue;

    fillDirection((Analysis::Muon*)obj);
  }

  if(m_check_gsf){
    // Get the tracks
    if (m_gsf_getter->reset().isFailure()) {
      REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
      return 0;
    }

    // finally, check the GSF-refitted particles, so we can keep the original tracks
    // from the refits
    const TrackParticleAssocs* assocs = m_gsf_assoc_getter->get<TrackParticleAssocs>();
    if (assocs) {
      while ( const Rec::TrackParticle* track = m_gsf_getter->next<Rec::TrackParticle>()){

	if (track->info().trackFitter() != Trk::TrackInfo::GaussianSumFilter){
	  REPORT_MESSAGE (MSG::VERBOSE) << "Not a GSF track.  Skipping.";
	  break;
	}
      
	TrackParticleAssocs::asso_iterator beg = assocs->beginAssociation (track);
	TrackParticleAssocs::asso_iterator end = assocs->endAssociation (track);
	const Rec::TrackParticle* ret = 0;
	if (beg != end && beg.isValid())
	  ret = dynamic_cast<const Rec::TrackParticle*> (*beg);
	if (ret){
	  electron_muon_photon_tracks.push_back(ret);
	}
	else{
	  REPORT_MESSAGE (MSG::INFO) << "No original track.";
	}
      }

      m_gsf_assoc_getter->releaseObject (assocs);
    }
    else{
      REPORT_MESSAGE (MSG::INFO) << "No GSF track assocs.";
    }
  }
  else{
    REPORT_MESSAGE (MSG::INFO) << "No GSF tracks.";
  }

  // if there are no qualifying photons/electrons/muons, no need to look any further
  if(etas.size() == 0 && electron_muon_photon_tracks.size() == 0) return sc;

  // now loop over all the tracks, and keep the ones that are within m_deltaRCut of at least one of the qualifying photons
  Rec::TrackParticleContainer::const_iterator f_trk = trkcoll->begin();
  Rec::TrackParticleContainer::const_iterator l_trk = trkcoll->end();
  
  for ( ; f_trk!=l_trk; ++f_trk) {
    const Rec::TrackParticle* track = (*f_trk);

    if (m_filterTracksForMET) {

      // Following harcoded cuts from Claire and Rachid, to get tracks to be used for TrackMET
      double trk_theta = track->measuredPerigee()->parameters()[Trk::theta];
      double trk_qOverP = track->measuredPerigee()->parameters()[Trk::qOverP];
      double trk_pt = fabs(1.0/trk_qOverP) * sin(trk_theta) ;
      double trk_eta = track->eta();
      int trk_nPixHits = track->trackSummary()->get(Trk::numberOfPixelHits);
      int trk_nSCTHits = track->trackSummary()->get(Trk::numberOfSCTHits);

      if ( trk_pt>500 && fabs(trk_eta)<2.5 && trk_nPixHits>=1 && trk_nSCTHits>=6 ) {
	outputCont->push_back(const_cast<Rec::TrackParticle*>(track));	
      } else {
	// let's make sure we store the tracks that correspond to the electron, muon or the converted photon
	bool foundtrack=false;
	for(unsigned int epiter = 0; epiter < electron_muon_photon_tracks.size(); epiter++){
	  const Rec::TrackParticle* eptrack = electron_muon_photon_tracks[epiter];
	  
	  if(track->hasSameAthenaBarCodeExceptVersion(*eptrack) || track==eptrack){
	    outputCont->push_back(const_cast<Rec::TrackParticle*>(track));
	    foundtrack=true;
	    break;
	  }
	}
	if(foundtrack) continue;
	
	for(unsigned int i=0; i<etas.size(); i++){
	  float deltaR = sqrt( pow(etas[i]-track->eta()            ,2.) +
			       pow(acos(cos(phis[i]-track->phi() )),2.) );
	  if( deltaR < m_deltaRCut ){
	    // FIXME: cast needed for copying between containers
	    outputCont->push_back(const_cast<Rec::TrackParticle*> (track)); 
	    break;
	  }
	}     
      }
      
    } else {
      // let's make sure we store the tracks that correspond to the electron, muon or the converted photon
      bool foundtrack=false;
      for(unsigned int epiter = 0; epiter < electron_muon_photon_tracks.size(); epiter++){
	const Rec::TrackParticle* eptrack = electron_muon_photon_tracks[epiter];
	
	if(track->hasSameAthenaBarCodeExceptVersion(*eptrack) || track==eptrack){
          outputCont->push_back(track); 
	  foundtrack=true;
	  break;
	}
      }
      if(foundtrack){
	continue;
      }
      
      for(unsigned int i=0; i<etas.size(); i++){
	float deltaR = sqrt( pow(etas[i]-track->eta()            ,2.) +
			     pow(acos(cos(phis[i]-track->phi() )),2.) );
	if( deltaR < m_deltaRCut ){
	  // FIXME: cast needed for copying between containers
	  outputCont->push_back(const_cast<Rec::TrackParticle*> (track)); 
	  break;
	}
      }     
    }

  }

  return sc;
}

void TrackFilter::fillDirection(egamma* obj)
{  
  float eta = obj->eta();
  float phi = obj->phi();
  float phi_perp1 = correct_phi(phi+PI/2.);
  float phi_perp2 = correct_phi(phi-PI/2.);
  
  etas.push_back(eta);
  phis.push_back(phi);
  
  if (m_storePerpTrk) { //store tracks close to the egamma's perpendicular region (perp. in phi)
    etas.push_back(eta);
    phis.push_back(phi_perp1);
    etas.push_back(eta);
    phis.push_back(phi_perp2);
  }
  
  // consider both eta and phi directions, the egamma's object and the cluster ones
  float eta_clus = obj->cluster()->eta();
  float phi_clus = obj->cluster()->phi();
  float phi_clus_perp1 = correct_phi(phi_clus+PI/2.);
  float phi_clus_perp2 = correct_phi(phi_clus-PI/2.);
  
  etas.push_back(eta_clus);
  phis.push_back(phi_clus);
  
  if (m_storePerpTrk) { //store tracks close to the egamma's perpendicular region (perp. in phi)
    etas.push_back(eta_clus);
    phis.push_back(phi_clus_perp1);
    etas.push_back(eta_clus);
    phis.push_back(phi_clus_perp2);
  }

}

void TrackFilter::fillDirection(Analysis::Muon* obj)
{  
  float eta = obj->eta();
  float phi = obj->phi();
  float phi_perp1 = correct_phi(phi+PI/2.);
  float phi_perp2 = correct_phi(phi-PI/2.);
  
  etas.push_back(eta);
  phis.push_back(phi);
  
  if (m_storePerpTrk) { //store tracks close to the muons' perpendicular region (perp. in phi)
    etas.push_back(eta);
    phis.push_back(phi_perp1);
    etas.push_back(eta);
    phis.push_back(phi_perp2);
  }
  
}

float TrackFilter::correct_phi(float phi)
{
  if (phi>    PI) return phi-2.*PI;
  if (phi<-1.*PI) return phi+2.*PI;
  return phi;
}

}
