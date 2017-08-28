/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigT2HistoPrmVtxBase.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2HistoPrmVtx
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticle.h"

#include "TrigT2HistoPrmVtxBase.h"
#include "TrigT2HistoPrmVtx/HistoVertexHelper.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "Particle/TrackParticleContainer.h"

#include "TMath.h"



TrigT2HistoPrmVtxBase::TrigT2HistoPrmVtxBase(MsgStream& log, unsigned int logLvl) :
  m_log(log),
  m_logLvl(logLvl),
  zPrmVtx(3),
  zPrmVtxSigmaAll(3),
  zPrmVtxSigma2Trk(3),
  nTrackVtx(3),
  nVtxFound(0),
  m_totTracks(0),
  m_totTracks_All(0),
  m_totSelTracks(0),
  m_totSelTracks_All(0),
  m_algo(0),
  m_nBins(0),
  m_useBeamSpot(false),
  m_useEtaPhiTrackSel(false),
  m_l2TrkSelChi2(0.0),
  m_l2TrkSelBLayer(0),
  m_l2TrkSelSiHits(0),
  m_l2TrkSelD0(0.0),
  m_l2TrkSelPt(0.0),
  m_efTrkSelChi2(0.0),
  m_efTrkSelBLayer(0),
  m_efTrkSelPixHits(0),
  m_efTrkSelSiHits(0),
  m_efTrkSelD0(0.0),
  m_efTrkSelPt(0.0),
  m_xBeamSpot(0.0), m_yBeamSpot(0.0), m_zBeamSpot(0.0),
  m_xBeamSpotTilt(0.0), m_yBeamSpotTilt(0.0),
  m_xBeamSpotSigma(0.0), m_yBeamSpotSigma(0.0), m_zBeamSpotSigma(0.0)
{

  m_hisVtx = new HistoVertexHelper(8000, -200, 200);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigT2HistoPrmVtxBase::TrigT2HistoPrmVtxBase(const TrigT2HistoPrmVtxBase &other) :
  m_log(other.m_log),
  m_logLvl(other.m_logLvl),
  zPrmVtx(other.zPrmVtx),
  zPrmVtxSigmaAll(other.zPrmVtxSigmaAll),
  zPrmVtxSigma2Trk(other.zPrmVtxSigma2Trk),
  nTrackVtx(other.nTrackVtx),
  nVtxFound(other.nVtxFound),
  m_totTracks(other.m_totTracks),
  m_totTracks_All(other.m_totTracks_All),
  m_totSelTracks(other.m_totSelTracks),
  m_totSelTracks_All(other.m_totSelTracks_All),
  m_algo(other.m_algo),
  m_nBins(other.m_nBins),
  m_useBeamSpot(other.m_useBeamSpot),
  m_useEtaPhiTrackSel(other.m_useEtaPhiTrackSel),
  m_l2TrkSelChi2(other.m_l2TrkSelChi2),
  m_l2TrkSelBLayer(other.m_l2TrkSelBLayer),
  m_l2TrkSelSiHits(other.m_l2TrkSelSiHits),
  m_l2TrkSelD0(other.m_l2TrkSelD0),
  m_l2TrkSelPt(other.m_l2TrkSelPt),
  m_efTrkSelChi2(other.m_efTrkSelChi2),
  m_efTrkSelBLayer(other.m_efTrkSelBLayer),
  m_efTrkSelPixHits(other.m_efTrkSelBLayer),
  m_efTrkSelSiHits(other.m_efTrkSelSiHits),
  m_efTrkSelD0(other.m_efTrkSelD0),
  m_efTrkSelPt(other.m_efTrkSelPt),
  m_xBeamSpot(other.m_xBeamSpot), 
  m_yBeamSpot(other.m_yBeamSpot), 
  m_zBeamSpot(other.m_zBeamSpot),
  m_xBeamSpotTilt(other.m_xBeamSpotTilt), 
  m_yBeamSpotTilt(other.m_yBeamSpotTilt), 
  m_xBeamSpotSigma(other.m_xBeamSpotSigma), 
  m_yBeamSpotSigma(other.m_yBeamSpotSigma), 
  m_zBeamSpotSigma(other.m_zBeamSpotSigma)
{

  this->m_hisVtx = new HistoVertexHelper(*other.m_hisVtx);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigT2HistoPrmVtxBase::~TrigT2HistoPrmVtxBase() {

  delete m_hisVtx;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigT2HistoPrmVtxBase& TrigT2HistoPrmVtxBase::operator=(const TrigT2HistoPrmVtxBase &rhs)
{
  if (this == &rhs)
    return *this;

  //this->m_log = rhs.m_log;
  this->m_logLvl = rhs.m_logLvl;
  this->zPrmVtx = rhs.zPrmVtx;
  this->zPrmVtxSigmaAll = rhs.zPrmVtxSigmaAll;
  this->zPrmVtxSigma2Trk = rhs.zPrmVtxSigma2Trk;
  this->nTrackVtx = rhs.nTrackVtx;
  this->nVtxFound = rhs.nVtxFound;
  this->m_totTracks = rhs.m_totTracks;
  this->m_totTracks_All = rhs.m_totTracks_All;
  this->m_totSelTracks = rhs.m_totSelTracks;
  this->m_totSelTracks_All = rhs.m_totSelTracks_All;
  this->m_algo = rhs.m_algo;
  this->m_nBins = rhs.m_nBins;
  this->m_useBeamSpot = rhs.m_useBeamSpot;
  this->m_useEtaPhiTrackSel = rhs.m_useEtaPhiTrackSel;
  this->m_l2TrkSelChi2 = rhs.m_l2TrkSelChi2;
  this->m_l2TrkSelBLayer = rhs.m_l2TrkSelBLayer;
  this->m_l2TrkSelSiHits = rhs.m_l2TrkSelSiHits;
  this->m_l2TrkSelD0 = rhs.m_l2TrkSelD0;
  this->m_l2TrkSelPt = rhs.m_l2TrkSelPt;
  this->m_efTrkSelChi2 = rhs.m_efTrkSelChi2;
  this->m_efTrkSelBLayer = rhs.m_efTrkSelBLayer;
  this->m_efTrkSelPixHits = rhs.m_efTrkSelBLayer;
  this->m_efTrkSelSiHits = rhs.m_efTrkSelSiHits;
  this->m_efTrkSelD0 = rhs.m_efTrkSelD0;
  this->m_efTrkSelPt = rhs.m_efTrkSelPt;
  this->m_xBeamSpot = rhs.m_xBeamSpot;
  this->m_yBeamSpot = rhs.m_yBeamSpot;
  this->m_zBeamSpot = rhs.m_zBeamSpot;
  this->m_xBeamSpotTilt = rhs.m_xBeamSpotTilt;
  this->m_yBeamSpotTilt = rhs.m_yBeamSpotTilt;
  this->m_xBeamSpotSigma = rhs.m_xBeamSpotSigma;
  this->m_yBeamSpotSigma = rhs.m_yBeamSpotSigma;
  this->m_zBeamSpotSigma = rhs.m_zBeamSpotSigma;

  if (this->m_hisVtx) delete m_hisVtx;
  this->m_hisVtx = new HistoVertexHelper(*rhs.m_hisVtx);

  return *this;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigT2HistoPrmVtxBase::phiCorr(float phi) {

  if(phi < -M_PI) phi += 2*M_PI;
  if(phi >  M_PI) phi -= 2*M_PI;

  return phi;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigT2HistoPrmVtxBase::IPCorr(float d0, float z0, float& d0c, float& z0c, float phi0, float eta, float pt) {

  double sn  = sin(phi0);
  double cs  = cos(phi0);
  double sd0 = (d0 != 0 ? d0/fabs(d0) : 1);
  double spt = (pt != 0 ? pt/fabs(pt) : 1);
    
  if (fabs(pt) >= 1*CLHEP::TeV) {
    d0c = d0 + m_xBeamSpot*sn - m_yBeamSpot*cs;
    
    if(m_xBeamSpot != 0 && m_yBeamSpot != 0 && m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << "d0 (shifted) = " << d0c << " , d0 (referred to origin) = " << d0 << endmsg;
  } else {

    double rc = fabs(pt)*15.0/(9.0*1.042);
    
    double xc = (fabs(d0)-spt*sd0*rc)*cos(phi0+M_PI/2*sd0) - m_xBeamSpot;
    double yc = (fabs(d0)-spt*sd0*rc)*sin(phi0+M_PI/2*sd0) - m_yBeamSpot;
    
    if(m_xBeamSpot != 0 && m_yBeamSpot != 0 && m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << "Coordinates of the circle center in transverse plane = (" << xc << "," << yc << ")" << endmsg;

    double newphi;
    double xd01,yd01,xd02,yd02;

    if (xc == 0) {
      xd01 = 0; yd01 = rc+yc;
      xd02 = 0; yd02 = -rc+yc; 
    } else {
      xd01 = xc+yc/xc*yc+sqrt(pow((xc+yc/xc*yc),2)-xc*xc-yc*yc+rc*rc); yd01 = yc/xc*xd01;
      xd02 = xc+yc/xc*yc-sqrt(pow((xc+yc/xc*yc),2)-xc*xc-yc*yc+rc*rc); yd02 = yc/xc*xd02;
    }

    double r1 = sqrt(xd01*xd01+yd01*yd01);
    double r2 = sqrt(xd02*xd02+yd02*yd02);

    double phiV;

    if (r1 < r2)
      phiV = atan2(yd01,xd01); 
    else
      phiV = atan2(yd02,xd02); 

    double phi1 = phiCorr(phiV+M_PI/2);
    double phi2 = phiCorr(phiV-M_PI/2);
    
    if (fabs(phiCorr(phi1-phi0))<=fabs(phiCorr(phi2-phi0)))
      newphi = phi1;
    else
      newphi = phi2;
      
    d0c = fabs(sqrt(xc*xc+yc*yc)-rc)*sin(phiV-newphi);

    double theta=2*atan2(exp(-eta),1);
    double theta_save=theta;
    theta = theta - (1+spt)/2*M_PI;
    if (theta>0) theta = theta_save;

    double deltaz0= -spt*rc/tan(theta)*phiCorr(phiCorr(newphi)-phiCorr(phi0));
    z0c = z0 + deltaz0;

    if(m_xBeamSpot != 0 && m_yBeamSpot != 0 && m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << "z0 (shifted) = " << z0c << " , z0 (referred to origin) = " << z0 << endmsg;
    if(m_xBeamSpot != 0 && m_yBeamSpot != 0 && m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << "d0 (shifted) = " << d0c << " , d0 (referred to origin) = " << d0 << endmsg;
  }

}

//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigT2HistoPrmVtxBase::efTrackSel(const xAOD::TrackParticle*& track, unsigned int i, float eta, float phi) {

  if(m_useEtaPhiTrackSel) {

    if (fabs(track->eta()-eta) > 0.2) {
      if (m_logLvl <= MSG::VERBOSE) {
	m_log << MSG::VERBOSE << "efTrackSel method" << endmsg;
	m_log << MSG::VERBOSE << "  Track number " << i+1 << " is not selected (eta matching)" << endmsg;
      }

      m_listCutApplied.push_back(0);
      return false;
    }

    if (fabs(phiCorr(phiCorr(track->phi())-phiCorr(phi))) > 0.2) {
      if (m_logLvl <= MSG::VERBOSE) {
	m_log << MSG::VERBOSE << "efTrackSel method" << endmsg;
	m_log << MSG::VERBOSE << "  Track number " << i+1 << " is not selected (phi matching)" << endmsg;
      }

      m_listCutApplied.push_back(1);
      return false;
    }
  }

  uint8_t nBlay = 0;
  uint8_t nPix  = 0;  
  uint8_t nSCT  = 0; 

  uint8_t expBL  = 0;
  track->summaryValue(expBL, xAOD::expectInnermostPixelLayerHit);

  track->summaryValue(nBlay, xAOD::numberOfInnermostPixelLayerHits);
  track->summaryValue(nPix,  xAOD::numberOfPixelHits);
  track->summaryValue(nSCT,  xAOD::numberOfSCTHits);


  //nBlay = nBlay*2;
  //nPix  = nPix*2; 
 
  float nSi = nPix+nSCT;
  float d0  = track->d0();
  float pT  = track->pt(); 

//   float z0  = track->z0();
//   if(m_useBeamSpot)
//     IPCorr(track->d0(),track->z0(),d0,z0,track->phi(),track->eta(), pT);

  if (m_logLvl <= MSG::VERBOSE) {
    m_log << MSG::VERBOSE << "efTrackSel method" << endmsg;
    m_log << MSG::VERBOSE << "  Track number " << i+1 << " to be selected must be:" << endmsg;
    m_log << MSG::VERBOSE << "    Pt    " << fabs(pT) << " >= " << m_efTrkSelPt << endmsg;
    m_log << MSG::VERBOSE << "    d0    " << fabs(d0) << " <= " << m_efTrkSelD0 << endmsg;
    m_log << MSG::VERBOSE << "    bLayer " << (int)nBlay << " >= " << m_efTrkSelBLayer << endmsg;
    m_log << MSG::VERBOSE << "    pixelHit " << (int)nPix << " >= " << m_efTrkSelPixHits << endmsg; 
    m_log << MSG::VERBOSE << "    SiHit " << nSi << " >= " << m_efTrkSelSiHits << endmsg;
    m_log << MSG::VERBOSE << "    Prob(chi2) " << TMath::Prob(track->chiSquared(), (int)nSi*3-5) << " > " << m_efTrkSelChi2 << endmsg;
  }

  if (fabs(pT) < m_efTrkSelPt) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (pT cut)" << endmsg;

    m_listCutApplied.push_back(2);
    return false;
  }
  if (fabs(d0) > m_efTrkSelD0) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (d0 cut)" << endmsg;

    m_listCutApplied.push_back(3);
    return false;
  }
  //  if (nBlay < m_efTrkSelBLayer) {
  if (expBL && (nBlay < m_efTrkSelBLayer) ){
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (missing b-layer hit)" << endmsg;

    m_listCutApplied.push_back(4);
    return false;
  }
  if (nPix < m_efTrkSelPixHits) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (too few pixel hits)" << endmsg;

    m_listCutApplied.push_back(5);
    return false;
  }
  if (nSi < m_efTrkSelSiHits) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (too few silicon hits)" << endmsg;

    m_listCutApplied.push_back(6);
    return false;
  }
  if (TMath::Prob(track->chiSquared(), (int)nSi*3-5) <= m_efTrkSelChi2) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (chi2 cut)" << endmsg;

    m_listCutApplied.push_back(7);
    return false;
  }
  
  if (m_logLvl <= MSG::VERBOSE)
    m_log << MSG::VERBOSE << "    track is selected" << endmsg;

  m_listCutApplied.push_back(8);
  return true;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigT2HistoPrmVtxBase::findPrmVtx() {

  // Finding primary vertex
  std::vector<int> maxVal(3), pos(3);
  std::vector<float> numMean(3), numSigma(3), den(3);

  int val=0; 
  nVtxFound=0;

  if (m_logLvl <= MSG::DEBUG)
    m_log << MSG::DEBUG << "Finding primary vertex candidates:" << " histogram with "  << m_hisVtx->getNBin() << " bins" << endmsg;      

  for (int j=0; j<3; j++) {
    val=0;

    for(int i=0; i<m_nBins; i++) 
      val += m_hisVtx->getN(i);

    maxVal[j] = val;

    for (int i=m_nBins; i<m_hisVtx->getNBin(); i++) {     
    
      if (val > maxVal[j]) {
	maxVal[j] = val;
	pos[j] = i - m_nBins;
      }

      val -= m_hisVtx->getN(i-m_nBins);
      val += m_hisVtx->getN(i);
    }

    if (m_logLvl <= MSG::VERBOSE) {
      m_log << MSG::VERBOSE << " maxVal[" << j << "] = " << maxVal[j] 
	       << " pos[" << j << "] = " << pos[j] << endmsg;
    }

    for (int i = pos[j]; i < (pos[j]+m_nBins); i++) {
      float nCont = m_hisVtx->getN(i);
      den[j] += nCont;
      numMean[j] += i*nCont;
    }

    if (den[j] != 0) {

      nVtxFound++;
      zPrmVtx.at(j) = m_hisVtx->getZ(numMean[j]/den[j]);   	 
      nTrackVtx.at(j) = m_hisVtx->getIntegral(pos[j], m_nBins);

      for (int i = pos[j]; i < (pos[j]+m_nBins); i++)
	for (int k=0; k<m_hisVtx->getN(i); k++)
	  numSigma[j] += (m_hisVtx->getZ(i)-zPrmVtx.at(j)) * (m_hisVtx->getZ(i)-zPrmVtx.at(j));

      if (den[j] == 1) {
	zPrmVtxSigmaAll.at(j)  = 0.025;
	zPrmVtxSigma2Trk.at(j) = 0.025;
      } else if (nTrackVtx.at(j)<20) {
	zPrmVtxSigmaAll.at(j)  = sqrt(numSigma[j]/(den[j]-1));
	zPrmVtxSigma2Trk.at(j) = sqrt(numSigma[j]/(den[j]-1));
      } else {
	zPrmVtxSigmaAll.at(j)  = sqrt(numSigma[j]/den[j]);
	zPrmVtxSigma2Trk.at(j) = sqrt(numSigma[j]/den[j]);
      }

      m_hisVtx->reset(pos[j], m_nBins);

    } else {

      zPrmVtx.at(j)          = -200;
      zPrmVtxSigmaAll.at(j)  = -200;
      zPrmVtxSigma2Trk.at(j) = -200;
      break;

    }
  }
} 

 

