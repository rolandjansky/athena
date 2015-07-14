/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// V0CandidateFillerTool.cxx
// AUTHORS: Till Eifert, Roberto Di Nardo
// **********************************************************************

#include "V0CandidateFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "TrkV0Vertex/V0Hypothesis.h"
#include "TrkV0Vertex/V0Candidate.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"
#include "Particle/TrackParticle.h"


using CLHEP::Hep3Vector;


namespace D3PD {

V0CandidateFillerTool::V0CandidateFillerTool (const std::string& type,
					const std::string& name,
					const IInterface* parent)
  : BlockFillerTool<Trk::V0Candidate> (type, name, parent),
    m_iBeamCondSvc(0)
{
  declareProperty("BeamCondSvcName", m_beamCondSvcName = "BeamCondSvc");

  book().ignore(); // Avoid coverity warnings.
}

StatusCode V0CandidateFillerTool::initialize()
{
  // m_v0Tools = ToolHandle<Trk::V0Tools>("Trk::V0Tools");
  // Pick up the BeamConditionService
  StatusCode sc = service(m_beamCondSvcName, m_iBeamCondSvc);
  if (sc.isFailure() || m_iBeamCondSvc == 0) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not find BeamCondSvc: " <<  m_beamCondSvcName;
    REPORT_MESSAGE (MSG::WARNING) << "Will use nominal beamspot at (0,0,0)";
  }
  return sc;
}

StatusCode V0CandidateFillerTool::book()
{
 
  CHECK( addVariable  ("ksMass", m_ksMass) );
  CHECK( addVariable  ("lambda1Mass", m_lambda1Mass) );
  CHECK( addVariable  ("lambda2Mass", m_lambda2Mass) );
  CHECK( addVariable  ("vertexProb", m_vertexProb) );
  CHECK( addVariable  ("vertexChi2", m_vertexChi2) );
  CHECK( addVariable  ("ksPt", m_ksPt) );
  CHECK( addVariable  ("ksPx", m_ksPx) );
  CHECK( addVariable  ("ksPy", m_ksPy) );
  CHECK( addVariable  ("ksEta", m_ksEta) );
  CHECK( addVariable  ("ksPhi", m_ksPhi) );
  CHECK( addVariable  ("ksMomentum", m_ksMomentum) );

  CHECK( addVariable ("flightX", m_flightX) );
  CHECK( addVariable ("flightY", m_flightY) );
  CHECK( addVariable ("cosThetaPointing" , m_cosThetaPointing) );
  CHECK( addVariable ("totalFlightDistance" , m_totalFlightDistance) );
  CHECK( addVariable ("properDecayTime" ,  m_properDecayTime) );
  CHECK( addVariable ("properFlightDist" ,  m_properFlightDist) );

  CHECK( addVariable ("flightX_BS", m_flightX_BS) );
  CHECK( addVariable ("flightY_BS", m_flightY_BS) );
  CHECK( addVariable ("cosThetaPointing_BS" , m_cosThetaPointing_BS) );
  CHECK( addVariable ("totalFlightDistance_BS" , m_totalFlightDistance_BS) );
  CHECK( addVariable ("properDecayTime_BS" ,  m_properDecayTime_BS) );
  CHECK( addVariable ("properFlightDist_BS" ,  m_properFlightDist_BS) );

  CHECK( addVariable ("radius" , m_radius) );

  CHECK( addVariable  ("thetaPiPlus" , m_thetaPiPlus) );         
  CHECK( addVariable  ("thetaPiMinus", m_thetaPiMinus) );          
  CHECK( addVariable  ("trk_L" , m_trk_L) );     
  CHECK( addVariable  ("trk_T",  m_trk_T) );  

  CHECK( addVariable  ("thetaStarPiPlus",  m_thetaStarPiPlus) );
  CHECK( addVariable  ("thetaStarPiMinus", m_thetaStarPiMinus) );

  CHECK( addVariable  ("trkPt_SV" , m_trkPt_SV) );
  CHECK( addVariable  ("trkEta_SV" , m_trkEta_SV) );
  CHECK( addVariable  ("trkPhi_SV" , m_trkPhi_SV) );

  return StatusCode::SUCCESS;
}

StatusCode V0CandidateFillerTool::fill (const Trk::V0Candidate& /*theV0Candidate*/ )
{

  this->clearData();

  //const double piMass        = 139.57018;   // MeV
  const double ksMassPDG     = 497.648;     // MeV
  //const double protonMassPDG = 938.272013;  // MeV
  const double c_speed       = 299792458.0; // meters per sec

  //const Trk::V0Hypothesis* myV0Hypothesis =  0; //m_v0Tools->v0Unconstr( &theV0Candidate );
 
  CLHEP::Hep3Vector ksMomentumVector ;//= m_v0Tools->V04Momentum( (const Trk::ExtendedVxCandidate*)myV0Hypothesis, ksMassPDG ).vect();

  //K and lambda masses
  *m_ksMass = 0;//m_v0Tools->invariantMass( (const Trk::ExtendedVxCandidate*)myV0Hypothesis, piMass, piMass );
  *m_lambda1Mass= 0;//m_v0Tools->invariantMass( (const Trk::ExtendedVxCandidate*)myV0Hypothesis, protonMassPDG, piMass ) ;
  *m_lambda2Mass= 0;//m_v0Tools->invariantMass( (const Trk::ExtendedVxCandidate*)myV0Hypothesis, piMass, protonMassPDG ) ;
  *m_vertexProb=  0;//m_v0Tools->vertexProbability( (const Trk::ExtendedVxCandidate*)myV0Hypothesis ) ;
  *m_vertexChi2=  0;//m_v0Tools->chisq( (const Trk::ExtendedVxCandidate*)myV0Hypothesis ) ;

  //K prop
  *m_ksPt       =  ksMomentumVector.perp() ;
  *m_ksMomentum =  ksMomentumVector.mag()  ;
  *m_ksPx       =  ksMomentumVector.x() ;
  *m_ksPy       =  ksMomentumVector.y() ;
  *m_ksEta      =  0;//m_v0Tools->V04Momentum((const Trk::ExtendedVxCandidate*)myV0Hypothesis,ksMassPDG).pseudoRapidity() ;
  *m_ksPhi      =  0;//m_v0Tools->V04Momentum((const Trk::ExtendedVxCandidate*)myV0Hypothesis,ksMassPDG).phi() ;

  const VxContainer* vertices = 0;
  StatusCode scv = evtStore()->retrieve(vertices,"VxPrimaryCandidate");
  if (scv.isFailure()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No VxPrimaryCandidate Collection found in StoreGate" << endreq;
    return StatusCode::SUCCESS;
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Collection with name VxPrimaryCandidate found in StoreGate" << endreq;
  }
 
  const Trk::RecVertex *primaryVertex=&((*(vertices->begin()))->recVertex());
 
  //double transverseFlightDistance;
  CLHEP::Hep3Vector flightVector;
  if(primaryVertex!=NULL) {
    //m_v0Tools->lxy((const Trk::ExtendedVxCandidate*)myV0Hypothesis,*primaryVertex);
    flightVector = CLHEP::Hep3Vector(); //m_v0Tools->vtx((const Trk::ExtendedVxCandidate*)myV0Hypothesis)-primaryVertex->position();
  }
  else {
    //m_v0Tools->rxy((const Trk::ExtendedVxCandidate*)myV0Hypothesis);
    flightVector = CLHEP::Hep3Vector(); //m_v0Tools->vtx((const Trk::ExtendedVxCandidate*)myV0Hypothesis);
  }
  float tp = ( ( ksMomentumVector.x()*flightVector.x()+
		 ksMomentumVector.y()*flightVector.y() )/
	       sqrt( ksMomentumVector.x()*ksMomentumVector.x()+
		     ksMomentumVector.y()*ksMomentumVector.y() )/
	       sqrt( flightVector.x()*flightVector.x()+
		     flightVector.y()*flightVector.y() ) );

  *m_cosThetaPointing = tp;
  *m_flightX= flightVector.x() ;
  *m_flightY= flightVector.y() ;
 
  if (tp > 0.) {         // put sign on flight distance
    *m_totalFlightDistance = flightVector.mag() ;
  }else{
    *m_totalFlightDistance =( -1.*flightVector.mag() );
  }

  *m_properDecayTime      = ( (1.0e12 / (1000 * c_speed)) * ksMassPDG / ksMomentumVector.mag() * (*m_totalFlightDistance));
  *m_properFlightDist     = ( ksMassPDG /  ksMomentumVector.mag() * (*m_totalFlightDistance) ); // mm


  //Flight Dist, decay Time costhetapoint wrt BS

  /// Pick up the beamspot
  Amg::Vector3D beamSpot(0,0,0);
  //Trk::RecVertex* bsVertex;
  if (m_iBeamCondSvc) {
    //bsVertex=new Trk::RecVertex(m_iBeamCondSvc->beamVtx());
    beamSpot = m_iBeamCondSvc->beamVtx().position();
  }else{
    ATH_MSG_WARNING( " Cannot get beamSpot center from iBeamCondSvc. Using (0,0,0)... " );
    //bsVertex=new Trk::RecVertex(Amg::Vector3D(0,0,0));
  }
  
  

  // *primaryVertex=&((m_iBeamCondSvc->beamVtx()));
 
  
  
  if(primaryVertex!=NULL) {
    //m_v0Tools->lxy((const Trk::ExtendedVxCandidate*)myV0Hypothesis,*bsVertex);
    flightVector = CLHEP::Hep3Vector();//m_v0Tools->vtx((const Trk::ExtendedVxCandidate*)myV0Hypothesis)-beamSpot;
  }
  else {
    //m_v0Tools->rxy((const Trk::ExtendedVxCandidate*)myV0Hypothesis);
    flightVector = CLHEP::Hep3Vector();//m_v0Tools->vtx((const Trk::ExtendedVxCandidate*)myV0Hypothesis);
  }
  tp = ( ( ksMomentumVector.x()*flightVector.x()+
	   ksMomentumVector.y()*flightVector.y() )/
         sqrt( ksMomentumVector.x()*ksMomentumVector.x()+
               ksMomentumVector.y()*ksMomentumVector.y() )/
         sqrt( flightVector.x()*flightVector.x()+
               flightVector.y()*flightVector.y() ) );

  *m_cosThetaPointing_BS= tp;
  *m_flightX_BS= flightVector.x() ;
  *m_flightY_BS= flightVector.y() ;
 
  if (tp > 0.) {         // put sign on flight distance
    *m_totalFlightDistance_BS = flightVector.mag() ;
  }else{
    *m_totalFlightDistance_BS =( -1.*flightVector.mag() );
  }

  *m_properDecayTime_BS      = ( (1.0e12 / 1000 / c_speed) * ksMassPDG / ksMomentumVector.mag() * (*m_totalFlightDistance_BS) );
  *m_properFlightDist_BS     = ( ksMassPDG /  ksMomentumVector.mag() * (*m_totalFlightDistance_BS) ); // mm




  // V0 position
  *m_radius = 0;//m_v0Tools->rxy((const Trk::ExtendedVxCandidate*)myV0Hypothesis);


  // V0 kinematics - pi ipot

  CLHEP::HepLorentzVector v1 ;//= m_v0Tools->positiveTrack4Momentum((const Trk::ExtendedVxCandidate*)myV0Hypothesis,piMass);
  CLHEP::HepLorentzVector v2 ;//= m_v0Tools->negativeTrack4Momentum((const Trk::ExtendedVxCandidate*)myV0Hypothesis,piMass);
  CLHEP::HepLorentzVector v0 = v1 + v2;

  *m_thetaPiPlus  = ( v0.angle( v1.vect() ));
  *m_thetaPiMinus = ( v0.angle( v2.vect() ));

  // get longitudinal and transverse components of both track 3D-vectors wrt V0 3D-vector
  CLHEP::Hep3Vector  trk1vec3   =  v1.vect();
  CLHEP::Hep3Vector  trk2vec3   =  v2.vect();
  CLHEP::Hep3Vector  v0UnitVec3 =  (v0.vect()).unit();
  float trkPos_L = ( trk1vec3.dot( v0UnitVec3 ) );
  float trkPos_T = ( trk1vec3.perp( v0UnitVec3 ) );
  float trkNeg_L = ( trk2vec3.dot( v0UnitVec3 ) );
  float trkNeg_T = ( trk2vec3.perp( v0UnitVec3 ) );

  CLHEP::Hep3Vector  boost =  v0.boostVector(); //v0.boostVector();
  boost *= -1.0; // change direction of boost (to get into rest-frame)
  v1.boost( boost );
  v2.boost( boost );
 
  *m_thetaStarPiPlus =  ( v0.angle( v1.vect() ));
  *m_thetaStarPiMinus = ( v0.angle( v2.vect() ));

  float posTrkPt_SV = ( v1.perp() );
  float posTrkEta_SV = ( v1.pseudoRapidity() );
  float posTrkPhi_SV = ( v1.phi() );
 
  float negTrkPt_SV = ( v2.perp() );
  float negTrkEta_SV = ( v2.pseudoRapidity() );
  float negTrkPhi_SV = ( v2.phi() );

  int pos_idx = 0;
  int neg_idx = 1;

  m_trk_L->at(pos_idx) = trkPos_L;
  m_trk_T->at(pos_idx) = trkPos_T;

  m_trk_L->at(neg_idx) = trkNeg_L;
  m_trk_T->at(neg_idx) = trkNeg_T;

  m_trkPt_SV->at(pos_idx) = posTrkPt_SV;
  m_trkEta_SV->at(pos_idx) = posTrkEta_SV;
  m_trkPhi_SV->at(pos_idx) = posTrkPhi_SV;

  m_trkPt_SV->at(neg_idx) = negTrkPt_SV;
  m_trkEta_SV->at(neg_idx) = negTrkEta_SV;
  m_trkPhi_SV->at(neg_idx) = negTrkPhi_SV;

  return StatusCode::SUCCESS;
}

void V0CandidateFillerTool::clearData(){

  *m_ksMass = 0;
  *m_lambda1Mass = 0;
  *m_lambda2Mass = 0;
  *m_vertexProb = 0;
  *m_vertexChi2 = 0;
  *m_ksPt = 0;
  *m_ksPx = 0;
  *m_ksPy = 0;
  *m_ksEta = 0;
  *m_ksPhi = 0;
  *m_ksMomentum = 0;

  *m_flightX = 0;
  *m_flightY = 0;
  *m_cosThetaPointing = 0;
  *m_totalFlightDistance = 0;
  *m_properDecayTime = 0;
  *m_properFlightDist = 0;

  *m_flightX_BS = 0;
  *m_flightY_BS = 0;
  *m_cosThetaPointing_BS = 0;
  *m_totalFlightDistance_BS = 0;
  *m_properDecayTime_BS = 0;
  *m_properFlightDist_BS = 0;

  *m_radius = 0;

  *m_thetaPiPlus = 0;
  *m_thetaPiMinus = 0;
  
  *m_thetaStarPiPlus = 0;
  *m_thetaStarPiMinus = 0;
  
  m_trk_L->clear();
  m_trk_T->clear();
  m_trkEta_SV->clear();
  m_trkPhi_SV->clear();

  m_trk_L->push_back(0);
  m_trk_L->push_back(0);
  m_trk_T->push_back(0);
  m_trk_T->push_back(0);
  m_trkPt_SV->push_back(0);
  m_trkPt_SV->push_back(0);
  m_trkEta_SV->push_back(0);
  m_trkEta_SV->push_back(0);
  m_trkPhi_SV->push_back(0);
  m_trkPhi_SV->push_back(0);
}


} // namespace D3PD
