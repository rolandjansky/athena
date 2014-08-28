/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// PAUphotonCorrectionTool.cxx, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////////////////
// author: Pierre-Antoine Delsart 
///////////////////////////////////////////////////////////////////////////////

#include "PhotonAnalysisUtils/PAUphotonCorrectionTool.h"

#include "PhotonAnalysisUtils/PAUhggFitter.h"
#include "PhotonAnalysisUtils/PAUprimaryVertexFinder.h"
#include "egammaEvent/PhotonContainer.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "egammaEvent/PhotonContainer.h"
#include <math.h>

#include "AthenaKernel/errorcheck.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))


// constructor and destructor for PAUphotonCorrectionTool::corrected_set_t
//IPAUphotonCorrectionTool::corrected_set_t::corrected_set_t() :

// constructor
PAUphotonCorrectionTool::PAUphotonCorrectionTool(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p), 
  m_PrimaryVertex( "PAUprimaryVertexFinder" ) ,
  m_PAUhggFitterTool( "PAUhggFitter"           ) ,
  m_ConvTool     ( "PAUconversionFlagTool"  ) 
{
  declareProperty("PrimaryVertexFinder",m_PrimaryVertex);
  declareProperty("ConvTool", m_ConvTool);
  declareProperty("HggFitter", m_PAUhggFitterTool);
  declareInterface<IPAUphotonCorrectionTool>(this);
}

// ********************************************************************************
// Athena standard methods
// initialize
StatusCode PAUphotonCorrectionTool::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "... in PAUphotonCorrectionTool::initialize()" << endreq ;

  CHECK ( m_ConvTool.retrieve() );
  CHECK ( m_PrimaryVertex.retrieve() );
  CHECK ( m_PAUhggFitterTool.retrieve() );

  log <<  MSG::INFO  << name() <<" initialize() successful" << endreq;    
  return StatusCode::SUCCESS;
}
// ********************************************************************************

// finalize
StatusCode PAUphotonCorrectionTool::finalize() {
  MsgStream log(msgSvc(), name());
  log <<  MSG::VERBOSE  << "... in finalize() ..." << endreq ;
  clear();
  return StatusCode::SUCCESS;
}
// ********************************************************************************



// ********************************************************************************
// ********************************************************************************
void PAUphotonCorrectionTool::set_photons(photon_t *ph1, photon_t *ph2){
  if( (m_inPh1 == ph1) && (m_inPh1 ==ph2) ) return; // Nothing to be done in this case
  m_inPh1 = ph1;
  m_inPh2 = ph2;
  m_isConv1 = (ph1->convFlag() >0);
  m_isConv2 = (ph2->convFlag() >0);
  // clear stored corrections
  clear();
}

// ********************************************************************************
PAUphotonCorrectionTool::correction_id PAUphotonCorrectionTool::preferred_corrections(){
  // Define a default correction (using PVF, not correcting angle of converted photon)
  PAUphotonCorrectionTool::correction_id id(0,0,1,0);
  // Choose a correction type depending if there is a conversion
  if(m_isConv1 || m_isConv2) id.angle_correction = PAUphotonCorrectionTool::PAUhggFitter_Conv;
  else id.angle_correction = PAUphotonCorrectionTool::PAUhggFitter_Calo;
  
  return id;
}
// ********************************************************************************
PAUphotonCorrectionTool::corrected_set_t PAUphotonCorrectionTool::compute_single_correction(correction_id id){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " compute_single_correction "<< id.angle_correction << endreq;


  // First look if correction id has already been computed :
  corrections_store_t::iterator it= m_corrections.find(id);
  if ( it!=m_corrections.end() && (*it).first==id ) {
    log << MSG::DEBUG << " Found a correction !!\t angle_corr=" << (*it).first.angle_correction 
	<< "\t energy_corr=" << (*it).first.energy_correction 
	<< "\t do_pvf=" << (*it).first.do_pvf
	<< "\t do_eta_correct_convphoton=" << (*it).first.do_eta_correct_convphoton
	<< endreq;
    return (*it).second;
    //     log << MSG::DEBUG << " req. correction was  \t angle_corr=" << id.angle_correction 
    // 	<< "\t energy_corr=" << id.energy_correction 
    // 	<< "\t do_pvf=" << id.do_pvf
    // 	<< "\t do_eta_correct_convphoton=" << id.do_eta_correct_convphoton
    // 	<< endreq;
    //     if ( id == (*it).first ) return (*it).second;
    //     else log << MSG::DEBUG << " ... but they are DIFFERENT!" << endreq;
  }

  // reset prim vertex to false
  m_PAUhggFitterTool->SetPrimVertexInfo(false, 0, 0);

  // prepare a new correction
  corrected_set_t theCorr;
  init_correction(theCorr);  

  // compute angle corrections
  do_angle_corrections(id,theCorr);


  log << MSG::DEBUG << " compute_single_correction. Corrected angle . z (before pvF)="<< theCorr.vtx_beforePVF_z << endreq;
  // Use primary vertex if requested
  if ( id.do_pvf ) theCorr.isCorrected = correct_withPrimaryVertex(theCorr, id.do_eta_correct_convphoton);
  log << MSG::DEBUG << " compute_single_correction. Corrected with PVF = "<< id.do_pvf<< "  is corrected = "<< theCorr.isCorrected << endreq;

  // compute energy corrections
  do_energy_corrections(id,theCorr);

  // compute corrections to the Higgs
  finalize_correction(theCorr);
  log << MSG::DEBUG << " compute_single_correction. finalized correction  " << endreq;


  // store corrected quantities
  m_corrections[id] = theCorr;

  log << MSG::DEBUG << " compute_single_correction. returning  " << theCorr.Ph1 <<"  "<< theCorr.Ph2 << "   "<< theCorr.Higgs << endreq;

  return theCorr;
}
// ********************************************************************************



// ********************************************************************************
bool PAUphotonCorrectionTool::retrieve_corrections(correction_id id, corrected_set_t &corrSet){
  corrections_store_t::iterator it= m_corrections.find(id);
  if(it != m_corrections.end()) {
    corrSet =  (*it).second;
    return true;
  } else{
    return false;
  }  
}

Analysis::Photon * PAUphotonCorrectionTool::retrieve_corrected_ph1(correction_id id){
  corrections_store_t::iterator it= m_corrections.find(id);
  if(it != m_corrections.end()) return ((*it).second).Ph1;
  else return 0;
}
Analysis::Photon * PAUphotonCorrectionTool::retrieve_corrected_ph2(correction_id id){
  corrections_store_t::iterator it= m_corrections.find(id);
  if(it != m_corrections.end()) return ((*it).second).Ph2;
  else return 0;
}
CompositeParticle * PAUphotonCorrectionTool::retrieve_corrected_higgs(correction_id id){
  corrections_store_t::iterator it= m_corrections.find(id);
  if(it != m_corrections.end()) return ((*it).second).Higgs;
  else return 0;
}
// ********************************************************************************



// ********************************************************************************
// Internal functions
// ********************************************************************************


// ********************************************************************************
bool PAUphotonCorrectionTool::do_energy_corrections(correction_id id, corrected_set_t &/*corrSet*/){
  switch(id.angle_correction) {    
  default:
    break;
    //    return false;
  }
  // nothing ready yet
  return true;
}

bool PAUphotonCorrectionTool::do_angle_corrections(correction_id id, corrected_set_t &corrSet){

  
  switch(id.angle_correction) {    
  case angle_none:
    corrSet.isCorrected = true ;
    break ;
  case PAUhggFitter_Calo:
    {
      corrSet.isCorrected = correct_hggFitterCaloOnly(corrSet, id.do_eta_correct_convphoton);      
    }
    break;
  case PAUhggFitter_Conv:
    {
      if( !m_isConv2 && !m_isConv1) break; // No conversion !
      corrSet.isCorrected = correct_hggFitterConv(corrSet, id.do_eta_correct_convphoton);
    }
    break;
  case PAUhggFitter_Track:
    {
      if( !m_isConv2 && !m_isConv1) break; // No conversion !
      corrSet.isCorrected = correct_hggFitterTrack(corrSet, id.do_eta_correct_convphoton);
    }
    break;
  case PAUhggFitter_ConvTrack:
    {
      corrSet.isCorrected = correct_hggFitterConvTrack(corrSet, id.do_eta_correct_convphoton);
    }
    break;
  case Track:
    {
      // Use the track of electron to estimate Z of vertex.
      if( !m_isConv2 && !m_isConv1) break; // No conversion !
      id.do_pvf = true ; // this is mandatory
      float z0_track1=0, z0_track2=0 ;
      float err1_z0 = 0, err2_z0=0;
      if(m_isConv1){
	z0_track1=m_inPh1->trackParticle()->measuredPerigee()->parameters()[Trk::z0];	
	err1_z0 = (*(m_inPh1->trackParticle()->measuredPerigee()->covariance()))(Trk::z0,Trk::z0);
      }
      if(m_isConv2){
	z0_track2=m_inPh2->trackParticle()->measuredPerigee()->parameters()[Trk::z0];	
	err2_z0 = (*(m_inPh2->trackParticle()->measuredPerigee()->covariance()))(Trk::z0,Trk::z0);
      }
      if( !m_isConv2 || !m_isConv1){ // only 1 conv
	// One of the quantity==0 so we can summ them
	corrSet.vtx_beforePVF_z    = z0_track2+z0_track1;
	corrSet.vtx_beforePVF_zres = err1_z0+err2_z0 ; 
      }else{ // 2 conv
	corrSet.vtx_beforePVF_z    = (z0_track2+z0_track1)/2;
	corrSet.vtx_beforePVF_zres = sqrt(err1_z0*err1_z0 + err2_z0*err2_z0);
      }
      corrSet.isCorrected = true;
    }
  default:
    break;
    return false;
  }

  return true;

}
// ********************************************************************************



// ********************************************************************************
bool PAUphotonCorrectionTool::correct_hggFitterCaloOnly(PAUphotonCorrectionTool::corrected_set_t &corrSet, bool correct_eta_conv){

  // Purely calorimetric :
  m_PAUhggFitterTool->SetEtas(m_inPh1->cluster(), m_inPh2->cluster());
  //m_PAUhggFitterTool->SetConversion(1, 0) ;
  //m_PAUhggFitterTool->SetConversion(2, 0) ;
  //m_PAUhggFitterTool->SetTrack(1, 0) ;
  //m_PAUhggFitterTool->SetTrack(2, 0) ;
  
  return correct_hggFitter_generic(corrSet, correct_eta_conv);

}

bool PAUphotonCorrectionTool::correct_hggFitterConv(PAUphotonCorrectionTool::corrected_set_t &corrSet, bool correct_eta_conv){
  MsgStream log(msgSvc(), name());

  // Purely calorimetric :
  m_PAUhggFitterTool->SetEtas(m_inPh1->cluster(), m_inPh2->cluster());

  if ( m_inPh1->convVx() ) {
    if ( m_inPh1->convVx()->vxTrackAtVertex() && m_inPh1->convVx()->vxTrackAtVertex()->size()==1) {
      log << MSG::DEBUG << "Photon 1 : Single track conversion" << endreq;
      if (m_inPh1->trackParticle()) {
	if ( !(m_inPh1->trackParticle()->trackSummary() && (m_inPh1->trackParticle()->trackSummary()->get(Trk::numberOfPixelHits)+m_inPh1->trackParticle()->trackSummary()->get(Trk::numberOfSCTHits))<4 ) ) {
	  log << MSG::DEBUG << "Photon 1 : photon->trackParticle() found ---> use it" << endreq;
	  m_PAUhggFitterTool->SetConversion(1, m_inPh1->convVx()) ;
	} else {
	  log << MSG::DEBUG << "Photon 1 : photon->trackParticle() found BUT <4 SCT+pixels hits ---> don't use any track" << endreq;
	}
      } else {
	log << MSG::DEBUG << "Photon 1 : No photon->trackParticle() found ---> use conversion track instead" << endreq;
	m_PAUhggFitterTool->SetConversion(1, m_inPh1->convVx()) ;
      }
    } else if ( m_inPh1->convVx()->vxTrackAtVertex() && m_inPh1->convVx()->vxTrackAtVertex()->size()==2) {
      log << MSG::DEBUG << "Photon 1 : Double track conversion" << endreq;
      if (m_inPh1->trackParticle()) {
	log << MSG::DEBUG << "Photon 1 : photon->trackParticle() found ---> use it" << endreq;
	m_PAUhggFitterTool->SetConversion(1, m_inPh1->convVx()) ;
      } else {
	log << MSG::DEBUG << "Photon 1 : No photon->trackParticle() found ---> don't use any track (should not happen...)" << endreq;
      }
    } else {
      if (m_inPh1->convVx()->vxTrackAtVertex()) {
	log << MSG::DEBUG << "Photon 1 : photon->vxTrackAtVertex() found, but size==0 or >2" << endreq;
      } else {
	log << MSG::DEBUG << "Photon 1 : No photon->vxTrackAtVertex() found ---> don't use any track" << endreq;
      }
    }
  }
  

  if ( m_inPh2->convVx() ) {
    if ( m_inPh2->convVx()->vxTrackAtVertex() && m_inPh2->convVx()->vxTrackAtVertex()->size()==1) {
      log << MSG::DEBUG << "Photon 2 : Single track conversion" << endreq;
      if (m_inPh2->trackParticle()) {
	if ( !(m_inPh2->trackParticle()->trackSummary() && (m_inPh2->trackParticle()->trackSummary()->get(Trk::numberOfPixelHits)+m_inPh2->trackParticle()->trackSummary()->get(Trk::numberOfSCTHits))<4 ) ) {
	  log << MSG::DEBUG << "Photon 2 : photon->trackParticle() found ---> use it" << endreq;
	  m_PAUhggFitterTool->SetConversion(2, m_inPh2->convVx()) ;
	} else {
	  log << MSG::DEBUG << "Photon 2 : photon->trackParticle() found BUT <4 SCT+pixels hits ---> don't use any track" << endreq;
	}
      } else {
	log << MSG::DEBUG << "Photon 2 : No photon->trackParticle() found ---> use conversion track instead" << endreq;
	m_PAUhggFitterTool->SetConversion(2, m_inPh2->convVx()) ;
      }
    } else if ( m_inPh2->convVx()->vxTrackAtVertex() && m_inPh2->convVx()->vxTrackAtVertex()->size()==2) {
      log << MSG::DEBUG << "Photon 2 : Double track conversion" << endreq;
      if (m_inPh2->trackParticle()) {
	log << MSG::DEBUG << "Photon 2 : photon->trackParticle() found ---> use it" << endreq;
	m_PAUhggFitterTool->SetConversion(2, m_inPh2->convVx()) ;
      } else {
	log << MSG::DEBUG << "Photon 2 : No photon->trackParticle() found ---> don't use any track (should not happen...)" << endreq;
      }
    } else {
      if (m_inPh2->convVx()->vxTrackAtVertex()) {
	log << MSG::DEBUG << "Photon 2 : photon->vxTrackAtVertex() found, but size==0 or >2" << endreq;
      } else {
	log << MSG::DEBUG << "Photon 2 : No photon->vxTrackAtVertex() found ---> don't use any track" << endreq;
      }
    }
  }
  //m_PAUhggFitterTool->SetTrack(1, 0) ;
  //m_PAUhggFitterTool->SetTrack(2, 0) ;
  
  return correct_hggFitter_generic(corrSet, correct_eta_conv);
}

bool PAUphotonCorrectionTool::correct_hggFitterTrack(PAUphotonCorrectionTool::corrected_set_t &corrSet, bool correct_eta_conv){

  // Purely calorimetric :
  m_PAUhggFitterTool->SetEtas(m_inPh1->cluster(), m_inPh2->cluster());
  m_PAUhggFitterTool->SetConversion(1, 0) ;
  m_PAUhggFitterTool->SetConversion(2, 0) ;
  m_PAUhggFitterTool->SetTrack(1, m_inPh1->trackParticle()) ;
  m_PAUhggFitterTool->SetTrack(2, m_inPh2->trackParticle()) ;
  
  return correct_hggFitter_generic(corrSet, correct_eta_conv);

}

bool PAUphotonCorrectionTool::correct_hggFitterConvTrack(PAUphotonCorrectionTool::corrected_set_t &corrSet, bool correct_eta_conv){
  MsgStream log(msgSvc(), name());

  // Purely calorimetric :
  m_PAUhggFitterTool->SetEtas(m_inPh1->cluster(), m_inPh2->cluster());

  
  if ( m_inPh1->convVx() ) {
    if ( m_inPh1->convVx()->vxTrackAtVertex() && m_inPh1->convVx()->vxTrackAtVertex()->size()==1) {
      log << MSG::DEBUG << "Photon 1 : Single track conversion" << endreq;
      if (m_inPh1->trackParticle()) {
	if ( !(m_inPh1->trackParticle()->trackSummary() && (m_inPh1->trackParticle()->trackSummary()->get(Trk::numberOfPixelHits)+m_inPh1->trackParticle()->trackSummary()->get(Trk::numberOfSCTHits))<4 ) ) {
	  log << MSG::DEBUG << "Photon 1 : photon->trackParticle() found ---> use it" << endreq;
	  m_PAUhggFitterTool->SetTrack(1, m_inPh1->trackParticle()) ;
	} else {
	  log << MSG::DEBUG << "Photon 1 : photon->trackParticle() found BUT <4 SCT+pixels hits ---> don't use any track" << endreq;
	}
      } else {
	Trk::VxTrackAtVertex* trk = (*m_inPh1->convVx()->vxTrackAtVertex())[0]; 
	Trk::ITrackLink* link = trk->trackOrParticleLink();
	const Trk::LinkToTrackParticleBase* tr_part = dynamic_cast<const Trk::LinkToTrackParticleBase*>(link);
	if(tr_part && *tr_part){
	  const Rec::TrackParticle* track = dynamic_cast<const Rec::TrackParticle*>(**tr_part); 
	  log << MSG::DEBUG << "Photon 1 : No photon->trackParticle() found ---> use conversion track instead" << endreq;
	  m_PAUhggFitterTool->SetTrack(1, track) ;
	}
      }
    } else if ( m_inPh1->convVx()->vxTrackAtVertex() && m_inPh1->convVx()->vxTrackAtVertex()->size()==2) {
      log << MSG::DEBUG << "Photon 1 : Double track conversion" << endreq;
      if (m_inPh1->trackParticle()) {
	log << MSG::DEBUG << "Photon 1 : photon->trackParticle() found ---> use it" << endreq;
	m_PAUhggFitterTool->SetTrack(1, m_inPh1->trackParticle()) ;
      } else {
	m_PAUhggFitterTool->SetTrack(1, 0) ;
	log << MSG::DEBUG << "Photon 1 : No photon->trackParticle() found ---> don't use any track (should not happen...)" << endreq;
      }
    } else {
      if (m_inPh1->convVx()->vxTrackAtVertex()) {
	log << MSG::DEBUG << "Photon 1 : photon->vxTrackAtVertex() found, but size==0 or >2" << endreq;
	m_PAUhggFitterTool->SetTrack(1, 0) ;
      } else {
	log << MSG::DEBUG << "Photon 1 : No photon->vxTrackAtVertex() found ---> don't use any track" << endreq;
	m_PAUhggFitterTool->SetTrack(1, 0) ;
      }
    }
  }
  

  if ( m_inPh2->convVx() ) {
    if ( m_inPh2->convVx()->vxTrackAtVertex() && m_inPh2->convVx()->vxTrackAtVertex()->size()==1) {
      log << MSG::DEBUG << "Photon 2 : Single track conversion" << endreq;
      if (m_inPh2->trackParticle()) {
	if ( !(m_inPh2->trackParticle()->trackSummary() && (m_inPh2->trackParticle()->trackSummary()->get(Trk::numberOfPixelHits)+m_inPh2->trackParticle()->trackSummary()->get(Trk::numberOfSCTHits))<4 ) ) {
	  log << MSG::DEBUG << "Photon 2 : photon->trackParticle() found ---> use it" << endreq;
	  m_PAUhggFitterTool->SetTrack(2, m_inPh2->trackParticle()) ;
	} else {
	  log << MSG::DEBUG << "Photon 2 : photon->trackParticle() found BUT <4 SCT+pixels hits ---> don't use any track" << endreq;
	}
      } else {
	Trk::VxTrackAtVertex* trk = (*m_inPh2->convVx()->vxTrackAtVertex())[0]; 
	Trk::ITrackLink* link = trk->trackOrParticleLink();
	const Trk::LinkToTrackParticleBase* tr_part = dynamic_cast<const Trk::LinkToTrackParticleBase*>(link);
	const Rec::TrackParticle* track = dynamic_cast<const Rec::TrackParticle*>(**tr_part); 
	log << MSG::DEBUG << "Photon 2 : No photon->trackParticle() found ---> use conversion track instead" << endreq;
	m_PAUhggFitterTool->SetTrack(2, track) ;
      }
    } else if ( m_inPh2->convVx()->vxTrackAtVertex() && m_inPh2->convVx()->vxTrackAtVertex()->size()==2) {
      log << MSG::DEBUG << "Photon 2 : Double track conversion" << endreq;
      if (m_inPh2->trackParticle()) {
	log << MSG::DEBUG << "Photon 2 : photon->trackParticle() found ---> use it" << endreq;
	m_PAUhggFitterTool->SetTrack(2, m_inPh2->trackParticle()) ;
      } else {
	m_PAUhggFitterTool->SetTrack(2, 0) ;
	log << MSG::DEBUG << "Photon 2 : No photon->trackParticle() found ---> don't use any track (should not happen...)" << endreq;
      }
    } else {
      if (m_inPh2->convVx()->vxTrackAtVertex()) {
	log << MSG::DEBUG << "Photon 2 : photon->vxTrackAtVertex() found, but size==0 or >2" << endreq;
	m_PAUhggFitterTool->SetTrack(2, 0) ;
      } else {
	log << MSG::DEBUG << "Photon 2 : No photon->vxTrackAtVertex() found ---> don't use any track" << endreq;
	m_PAUhggFitterTool->SetTrack(2, 0) ;
      }
    }
  }
  
  return correct_hggFitter_generic(corrSet, correct_eta_conv);

}

bool PAUphotonCorrectionTool::correct_hggFitter_generic(PAUphotonCorrectionTool::corrected_set_t &corrSet, bool correct_eta_conv){
  // Do the fit :
  float eta1, eta2;
  bool status = m_PAUhggFitterTool->FitPAU(corrSet.vtx_beforePVF_z, corrSet.vtx_beforePVF_zres, eta1, eta2);
  if(! status) return false;
  
  // Correct position :
  if(correct_eta_conv || !m_isConv1) corrSet.Ph1->setEta(eta1);
  if(correct_eta_conv || !m_isConv2) corrSet.Ph2->setEta(eta2);

  return true;
}

bool PAUphotonCorrectionTool::correct_withPrimaryVertex(PAUphotonCorrectionTool::corrected_set_t &corrSet, bool correct_eta_conv ){
  if( ! corrSet.isCorrected ) return false ; // something went wrong. exit

  std::vector<double> results = m_PrimaryVertex->PrimaryVxFinder(corrSet.vtx_beforePVF_z, corrSet.vtx_beforePVF_zres) ;
  if ( results.size() == 0 ) return false;
  if ( results.size()>=3 && results[1]==0 && results[2]==0 ) return false ;  // PrimaryVxFinder failed (vertex inefficiency!)

  m_PAUhggFitterTool->SetPrimVertexInfo( (float)results[0] , 0.1 ) ;

  corrSet.vtx_afterPVF_z = (float)results[0];
  corrSet.vtx_afterPVF_zlike = (float)results[1];

  float eta1, eta2;
  float z,zres;
  bool status = m_PAUhggFitterTool->FitPAU( z, zres, eta1, eta2 ) ;
  if(! status) return false;

  corrSet.vtx_afterPVF_zres = zres ;

  // Correct position :
  if(correct_eta_conv || !m_isConv1) corrSet.Ph1->setEta(eta1);
  if(correct_eta_conv || !m_isConv2) corrSet.Ph2->setEta(eta2);
  
  return true;
}

bool PAUphotonCorrectionTool::correct_withGivenVertex(float z, float zres, corrected_set_t &corrSet, bool correct_eta_conv ){

  corrSet.vtx_afterPVF_z = z;
  corrSet.vtx_afterPVF_zlike = zres;

  float eta1, eta2;

  bool status = m_PAUhggFitterTool->FitPAU( z, zres, eta1, eta2 ) ;
  if(! status) return false;

  // Correct position :
  if(correct_eta_conv || !m_isConv1) corrSet.Ph1->setEta(eta1);
  if(correct_eta_conv || !m_isConv2) corrSet.Ph2->setEta(eta2);
  
  return true;
}
// ********************************************************************************


// ********************************************************************************
void PAUphotonCorrectionTool::finalize_correction(PAUphotonCorrectionTool::corrected_set_t &corrSet){
  
  if(corrSet.Higgs) delete corrSet.Higgs;
  corrSet.Higgs = new CompositeParticle();
  MsgStream log(msgSvc(), name());
  IParticleContainer* cps;
  if (evtStore()->contains<IParticleContainer>("myPhotons")==false) {
    cps = new IParticleContainer;
    if ( !evtStore()->record(cps, "myPhotons").isSuccess() ) {
      log << MSG::ERROR << "Could not record IParticleContainer at [myPhotons] !" << endreq;
    }
    else log << MSG::DEBUG << "Record IParticleContainer at [myPhotons]" << endreq;
  }
  else {
    StatusCode sc=evtStore()->retrieve(cps, "myPhotons");
    if( sc.isFailure()  ||  !cps ) {
      log << MSG::ERROR << "No [myPhotons] container yet... has to be created first !" << endreq ;
    }
    if(!cps)
      return;
  }

  if (cps->size()!=0) {
    log << MSG::VERBOSE << "[myPhotons] size = " << cps->size() << endreq;
    cps->erase(cps->begin(),cps->end());
    log << MSG::VERBOSE << "Erase OK" << endreq;
  }
  else {
    log << MSG::VERBOSE << "No need to erase" << endreq;
  }

  cps->push_back(corrSet.Ph1);
  cps->push_back(corrSet.Ph2);

  INav4MomLink part1 = INav4MomLink( *cps, 0 ) ;
  INav4MomLink part2 = INav4MomLink( *cps, 1 ) ;


  if (cps->size()==2) {
    corrSet.Higgs->add(part1,part2);
  }

}




void PAUphotonCorrectionTool::clear(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " Clearing PAUphotonCorrectionTool "<< endreq;
#if 0 // unused
  corrections_store_t::iterator it = m_corrections.begin();
  corrections_store_t::iterator itE = m_corrections.end();
  for( ; it != itE;++it){
    //PAUphotonCorrectionTool::corrected_set_t s = (*it).second;
    //delete s.Ph1;
    //delete s.Ph2;
    //delete s.Higgs;
    s.Ph1=NULL;
    s.Ph2=NULL;
    s.Higgs=NULL;
  }
#endif
  m_corrections.clear();
}

void PAUphotonCorrectionTool::init_correction(PAUphotonCorrectionTool::corrected_set_t &corrSet){
  Analysis::Photon* ph1 = new Analysis::Photon();
  ph1->setE(m_inPh1->e());
  ph1->setEta(m_inPh1->eta());
  ph1->setPhi(m_inPh1->phi());
  ph1->setM(m_inPh1->m());
  
  Analysis::Photon* ph2 = new Analysis::Photon();
  ph2->setE(m_inPh2->e());
  ph2->setEta(m_inPh2->eta());
  ph2->setPhi(m_inPh2->phi());
  ph2->setM(m_inPh2->m());

  corrSet.Ph1 = ph1;
  corrSet.Ph2 = ph2;

  corrSet.isCorrected = false;
}
