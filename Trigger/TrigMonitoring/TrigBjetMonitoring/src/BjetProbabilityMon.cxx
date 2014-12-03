/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************************
//
// NAME:     BjetProbabilityMon.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Ariel Schwartzman / Andrea Coccaro
// EMAIL:    sch@slac.stanford.edu / Andrea.Coccaro@ge.infn.it
// 
// *************************************************************

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrigBjetMonitoring/BjetProbabilityMon.h"
#include "TrigBjetMonitoring/TaggerHelper.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "Particle/TrackParticleContainer.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


BjetProbabilityMon::BjetProbabilityMon(HLTBjetMonTool* bjetMon, MsgStream* log) :
  m_bjetMon(bjetMon),
  m_log(log),
  m_totTracks(0),
  m_totSelTracks(0),
  m_totVertices(0),
  m_zPrmVtx(0),
  m_maxIP(15.0),
  m_jetPhi(-999.9)
{

  m_taggerHelper = new TaggerHelper(log);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


BjetProbabilityMon::~BjetProbabilityMon() {

  if (m_taggerHelper) delete m_taggerHelper;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetProbabilityMon::f_ip(float x, int algo, int iclass) {

  float* p = 0;

  if (algo != 6) *m_log << MSG::INFO << " BjetProbabilityMon::f_ip algo " << algo << " doesn't corresponf to EF tracks ! " << endreq;

  if (iclass==0){
    p = &m_bjetMon->m_par_EF0[0];
  } else if (iclass==1){
    p = &m_bjetMon->m_par_EF1[0];
  }

  float y1  = p[2]*p[3]*erfc(fabs(x)/(sqrt(2)*p[3]));
  float y2  = p[0]*p[1]*erfc(fabs(x)/(sqrt(2)*p[1]));
  float y01 = p[2]*p[3];
  float y02 = p[0]*p[1];
  float res=(y1+y2)/(y01+y02);

  return res;
}


//** ----------------------------------------------------------------------------------------------------------------- **//




bool BjetProbabilityMon::efTrackSel(const Rec::TrackParticle*& track, unsigned int i, float zv) {

  if(m_bjetMon->m_useEtaPhiTrackSel) {

    if (fabs(track->eta()-m_etaRoI) > 0.2) {

      *m_log << MSG::VERBOSE << "efTrackSel method" << endreq;
      *m_log << MSG::VERBOSE << "  Track number " << i+1 << " is not selected (eta matching)" << endreq;

      m_bjetMon->m_listCutAppliedProbability.push_back(2);
      return false;
    }

    if (fabs(m_taggerHelper->phiCorr(m_taggerHelper->phiCorr(track->phi())-m_taggerHelper->phiCorr(m_phiRoI))) > 0.2) {

      *m_log << MSG::VERBOSE << "efTrackSel method" << endreq;
      *m_log << MSG::VERBOSE << "  Track number " << i+1 << " is not selected (phi matching)" << endreq;

      m_bjetMon->m_listCutAppliedProbability.push_back(3);
      return false;
    }
  }

  const Trk::TrackSummary *summary = track->trackSummary();
  const Trk::FitQuality *quality   = track->fitQuality();

  int numberOfSiHits = summary->get(Trk::numberOfPixelHits)+summary->get(Trk::numberOfSCTHits);
  float theta = track->measuredPerigee()->parameters()[Trk::theta];
  float qOverPt = track->measuredPerigee()->parameters()[Trk::qOverP]/sin(theta);
  float pT = (1/qOverPt);

  float d0 = track->measuredPerigee()->parameters()[Trk::d0];
  float z0 = track->measuredPerigee()->parameters()[Trk::z0];
  m_taggerHelper->IPCorr(track->measuredPerigee()->parameters()[Trk::d0],
			 track->measuredPerigee()->parameters()[Trk::z0],
			 d0,z0, track->phi(),  track->eta(), pT, m_xBeamSpot, m_yBeamSpot);

  *m_log << MSG::VERBOSE << "efTrackSel method" << endreq;
  *m_log << MSG::VERBOSE << "  Track number " << i+1 << " to be selected must be:" << endreq;
  *m_log << MSG::VERBOSE << "    Pt " << fabs(pT) << " >= " << m_bjetMon->m_efTrkSelPt << endreq;
  *m_log << MSG::VERBOSE << "    d0 " << fabs(d0) << " <= " << m_bjetMon->m_efTrkSelD0 << endreq;
  *m_log << MSG::VERBOSE << "    z0 " << fabs(z0-zv) << " <= " << m_bjetMon->m_efTrkSelZ0 << endreq;
  *m_log << MSG::VERBOSE << "    bLayer " << summary->get(Trk::numberOfBLayerHits) << " >= " << m_bjetMon->m_efTrkSelBLayer << endreq;
  *m_log << MSG::VERBOSE << "    pixelHit " << summary->get(Trk::numberOfPixelHits) << " >= " << m_bjetMon->m_efTrkSelPixHits << endreq;
  *m_log << MSG::VERBOSE << "    SiHit " << (summary->get(Trk::numberOfPixelHits)+ summary->get(Trk::numberOfSCTHits)) << " >= " 
	 << m_bjetMon->m_efTrkSelSiHits << endreq;
  *m_log << MSG::VERBOSE << "    Prob(chi2) " << TMath::Prob(quality->chiSquared(), (int)numberOfSiHits*3-5) << " > " << m_bjetMon->m_efTrkSelChi2 << endreq;

  if (fabs(pT) < m_bjetMon->m_efTrkSelPt) {

    *m_log << MSG::VERBOSE << "    track is not selected (pT cut)" << endreq;
    
    m_bjetMon->m_listCutAppliedProbability.push_back(4);
    return false;
  }
  if (fabs(d0) > m_bjetMon->m_efTrkSelD0) {

    *m_log << MSG::VERBOSE << "    track is not selected (d0 cut)" << endreq;

    m_bjetMon->m_listCutAppliedProbability.push_back(5);
    return false;
  }
  if (fabs(z0-zv) > m_bjetMon->m_efTrkSelZ0) {

    *m_log << MSG::VERBOSE << "    track is not selected (z0 cut)" << endreq;

    m_bjetMon->m_listCutAppliedProbability.push_back(6);
    return false;
  }
  if (summary->get(Trk::numberOfBLayerHits) < m_bjetMon->m_efTrkSelBLayer) {
    
    *m_log << MSG::VERBOSE << "    track is not selected (missing b-layer hit)" << endreq;

    m_bjetMon->m_listCutAppliedProbability.push_back(7);
    return false;
  }
  if (summary->get(Trk::numberOfPixelHits) < m_bjetMon->m_efTrkSelPixHits) {

    *m_log << MSG::VERBOSE << "    track is not selected (too few pixel hits)" << endreq;

    m_bjetMon->m_listCutAppliedProbability.push_back(8);
    return false;
  }
  if (numberOfSiHits < m_bjetMon->m_efTrkSelSiHits) {

    *m_log << MSG::VERBOSE << "    track is not selected (too few silicon hits)" << endreq;

    m_bjetMon->m_listCutAppliedProbability.push_back(9);
    return false;
  }
  if (TMath::Prob(quality->chiSquared(), (int)numberOfSiHits*3-5) <= m_bjetMon->m_efTrkSelChi2) {

    *m_log << MSG::VERBOSE << "    track is not selected (chi2 cut)" << endreq;

    m_bjetMon->m_listCutAppliedProbability.push_back(10);
    return false;
  }

  *m_log << MSG::VERBOSE << "    track is selected" << endreq;

  m_bjetMon->m_listCutAppliedProbability.push_back(11);
  return true;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void BjetProbabilityMon::getProbabilityTag() {

  //* Initialize m_taggersXMap (tagger string <-> float X) *//
  m_taggersXMap["CHI2"] = -0.4;

  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//




void BjetProbabilityMon::getProbabilityTag(const Rec::TrackParticleContainer*& pointerToEFTrackCollections,
					   //const TrigVertexCollection*& pointerToPrmVtxCollections,
					   float m_prmvtx,
					   const ToolHandle<ITrigTrackJetFinderTool> m_trackJetFinderTool) {

  *m_log << MSG::DEBUG << "Executing BjetProbabilityMon::getProbabilityTag" << endreq;

  //* Initialize m_taggersXMap (tagger string <-> float X) *//
  m_taggersXMap["CHI2"] = -1;

  //* TrigTrackJetFinderTool *//
  m_trackJetFinderTool->clear();

  //* Get tracks number *//
  m_totTracks = m_taggerHelper->getTrackNumber(pointerToEFTrackCollections);

  m_zPrmVtx = m_prmvtx;
  m_trackJetFinderTool->inputPrimaryVertexZ(m_zPrmVtx);

  //* Beam spot shift *//
  m_xBeamSpot = 0, m_yBeamSpot = 0;

  m_xBeamSpot = m_bjetMon->m_xBeamSpot;  
  m_yBeamSpot = m_bjetMon->m_yBeamSpot;

  m_totSelTracks = 0;

  //* Loop on tracks to add tracks to TrigTrackJetFinderTool *//
  for (unsigned int j = 0; j < m_totTracks; j++) {
    const Rec::TrackParticle* track = (*pointerToEFTrackCollections)[j];

    m_taggerHelper->showParam(track, j);

    //m_bjetMon->m_mon_ef_trk_a0.push_back(track->param()->a0());
    //m_bjetMon->m_mon_ef_trk_z0.push_back(track->param()->z0());

    if (!efTrackSel(track, j, m_zPrmVtx)) continue;

    //m_bjetMon->m_mon_ef_trk_a0_sel.push_back(track->param()->a0());
    //m_bjetMon->m_mon_ef_trk_z0_sel.push_back(track->param()->z0());
    //m_bjetMon->m_mon_ef_trk_z0_sel_PV.push_back(track->param()->z0()-m_zPrmVtx);

    m_totSelTracks++;

    m_trackJetFinderTool->addTrack(track, j);
  }

  std::vector<int> tracksTrackJet;
  float etaTrackJet, phiTrackJet;

  //* Find jet direction *//
  m_trackJetFinderTool->findJet(tracksTrackJet, etaTrackJet, phiTrackJet);

  setJetPhi(phiTrackJet);

  for(unsigned int i=0; i<tracksTrackJet.size(); i++) {
    int id = tracksTrackJet[i];
    const Rec::TrackParticle* track = (*pointerToEFTrackCollections)[id];
    addTrack(track);
  }

  float prob;

  if(tracksTrackJet.size()<1) { 
    prob=1.0;
  } else {
    int n=0; 
    getPositiveJetProb(prob,n);
    if (n==0) prob=1.0;
  }

  float val = 1.-prob;
  if (val>=1) val = 0.999;
  if (val<=0) val = 0.001;

  if (!m_totSelTracks)
    m_taggersXMap["CHI2"] = -0.3; 
  else 
    m_taggersXMap["CHI2"] = val;

  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void BjetProbabilityMon::setJetPhi(float jetPhi) {

  m_n_pos = 0;
  m_n_neg = 0;

  m_PI_pos = 1.0; 
  m_PI_neg = 1.0; 

  m_jetPhi = jetPhi;
}


//** ----------------------------------------------------------------------------------------------------------------- **//




void BjetProbabilityMon::addTrack(const Rec::TrackParticle*& track) {

  float d0,z0;
  float theta = track->measuredPerigee()->parameters()[Trk::theta];
  float qOverPt = track->measuredPerigee()->parameters()[Trk::qOverP]/sin(theta);
  float pT = (1/qOverPt);
  m_taggerHelper->IPCorr(track->measuredPerigee()->parameters()[Trk::d0],track->measuredPerigee()->parameters()[Trk::z0],d0,z0,
			 track->phi(),track->eta(), pT, m_xBeamSpot, m_yBeamSpot);

  float phi0 = track->measuredPerigee()->parameters()[Trk::phi0];
  float signed_d0 = m_taggerHelper->signedD0(d0, phi0, m_jetPhi); 
  float sign =  signed_d0/TMath::Abs(d0);

  float p = getTrackProbability(track);

  //* for monitoring purposes *//
  m_bjetMon->m_mon_ef_trk_prob.push_back(p);

  float dphi = phi0 - m_jetPhi;
  if(dphi>TMath::Pi()) dphi -= TMath::TwoPi();
  if(dphi<-TMath::Pi()) dphi += TMath::TwoPi();

  if(sign>=0 && p>=0) {m_PI_pos *= p; m_n_pos++;}
  if(sign<0 && p>=0)  {m_PI_neg *= p; m_n_neg++;}
}




//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetProbabilityMon::getTrackProbability(const Rec::TrackParticle*& track) {

  float p = -1.0;

  float d0,z0;
  float theta = track->measuredPerigee()->parameters()[Trk::theta];
  float qOverPt = track->measuredPerigee()->parameters()[Trk::qOverP]/sin(theta);
  //float charge = track->measuredPerigee()->charge();
  float pT = (1/qOverPt);
  m_taggerHelper->IPCorr(track->measuredPerigee()->parameters()[Trk::d0], track->measuredPerigee()->parameters()[Trk::z0],
			 d0,z0,  track->phi(), track->eta(), pT, m_xBeamSpot, m_yBeamSpot);

  float phi0 = track->measuredPerigee()->parameters()[Trk::phi0];
  float signed_d0 = m_taggerHelper->signedD0(d0, phi0, m_jetPhi); 

  float pscat = pScat(track);
  int nSiHits = track->trackSummary()->get(Trk::numberOfPixelHits)+track->trackSummary()->get(Trk::numberOfSCTHits);

  float d0_error;
  if (m_bjetMon->m_useErrIPParam)
    d0_error = d0Error(pscat, nSiHits);
  else
    d0_error = Amg::error(*(track->measuredPerigee()->covariance()),Trk::d0);

  float m_sigmaBeamSpot;
  m_sigmaBeamSpot = (m_bjetMon->m_sigmaX+m_bjetMon->m_sigmaY)/2;

  float ip = TMath::Abs(signed_d0/sqrt(d0_error*d0_error + m_sigmaBeamSpot*m_sigmaBeamSpot));
  if(ip <= m_maxIP) {
    if(nSiHits < 7)  p = f_ip(-ip,6,0);
    if(nSiHits >= 7) p = f_ip(-ip,6,1);
  } else p = 0.0001;

  return p;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void BjetProbabilityMon::getPositiveJetProb(float& prob, int& n) {

  n  = m_n_pos;
  prob = -1;
  
  float SUM = 0.0; 
  
  if(m_n_pos>=1 && m_PI_pos > 0) {
    for(int i=0; i<m_n_pos; i++)
      SUM += TMath::Power(-TMath::Log(m_PI_pos),i)/TMath::Gamma(i+1);
    prob = m_PI_pos*SUM;
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void BjetProbabilityMon::getNegativeJetProb(float& prob, int& n) {

  n = m_n_neg;
  prob = -1;
  
  float SUM = 0.0; 

  if(m_n_neg >= 1 && m_PI_neg > 0) {
    for(int i=0; i<m_n_neg; i++)
      SUM += TMath::Power(-TMath::Log(m_PI_neg),i)/TMath::Gamma(i+1);
    prob  = m_PI_neg*SUM;
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetProbabilityMon::d0Error(float& pscat, int nSiHits) {

 if(nSiHits<7) return 0.09392/pscat + 0.0293;
 else return 0.1067/pscat + 0.0233;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetProbabilityMon::pScat(const TrigInDetTrack*& track) {

  float pt  = track->param()->pT()/1000.;
  float phi = track->param()->phi0();
  float eta = track->param()->eta();
  float theta =  2*TMath::ATan(TMath::Exp(-eta));

  float px = pt * TMath::Cos(phi);
  float py = pt * TMath::Sin(phi);
  float pz = pt/TMath::Tan(2.0*TMath::ATan(TMath::Exp(-eta)));
  float P = sqrt(px*px+py*py+pz*pz);

  return P*TMath::Power(TMath::Sin(theta),3/2.0);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetProbabilityMon::pScat(const Rec::TrackParticle*& track) {

  float pt  = track->pt()/1000.;
  float phi = track->measuredPerigee()->parameters()[Trk::phi0];
  float eta = track->measuredPerigee()->parameters()[Trk::theta];
  float theta =  2*TMath::ATan(TMath::Exp(-eta));

  float px = pt * TMath::Cos(phi);
  float py = pt * TMath::Sin(phi);
  float pz = pt/TMath::Tan(2.0*TMath::ATan(TMath::Exp(-eta)));
  float P = sqrt(px*px+py*py+pz*pz);

  return P*TMath::Power(TMath::Sin(theta),3/2.0);
}


