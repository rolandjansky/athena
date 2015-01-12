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
  m_totSelTracks_All(0)
{

  m_hisVtx = new HistoVertexHelper(8000, -200, 200);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigT2HistoPrmVtxBase::~TrigT2HistoPrmVtxBase() {

  delete m_hisVtx;
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
      m_log << MSG::DEBUG << "d0 (shifted) = " << d0c << " , d0 (referred to origin) = " << d0 << endreq;
  } else {

    double rc = fabs(pt)*15.0/(9.0*1.042);
    
    double xc = (fabs(d0)-spt*sd0*rc)*cos(phi0+M_PI/2*sd0) - m_xBeamSpot;
    double yc = (fabs(d0)-spt*sd0*rc)*sin(phi0+M_PI/2*sd0) - m_yBeamSpot;
    
    if(m_xBeamSpot != 0 && m_yBeamSpot != 0 && m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << "Coordinates of the circle center in transverse plane = (" << xc << "," << yc << ")" << endreq;

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
      m_log << MSG::DEBUG << "z0 (shifted) = " << z0c << " , z0 (referred to origin) = " << z0 << endreq;
    if(m_xBeamSpot != 0 && m_yBeamSpot != 0 && m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << "d0 (shifted) = " << d0c << " , d0 (referred to origin) = " << d0 << endreq;
  }

}

//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigT2HistoPrmVtxBase::efTrackSel(const xAOD::TrackParticle*& track, unsigned int i, float eta, float phi) {

  if(m_useEtaPhiTrackSel) {

    if (fabs(track->eta()-eta) > 0.2) {
      if (m_logLvl <= MSG::VERBOSE) {
	m_log << MSG::VERBOSE << "efTrackSel method" << endreq;
	m_log << MSG::VERBOSE << "  Track number " << i+1 << " is not selected (eta matching)" << endreq;
      }

      m_listCutApplied.push_back(0);
      return false;
    }

    if (fabs(phiCorr(phiCorr(track->phi())-phiCorr(phi))) > 0.2) {
      if (m_logLvl <= MSG::VERBOSE) {
	m_log << MSG::VERBOSE << "efTrackSel method" << endreq;
	m_log << MSG::VERBOSE << "  Track number " << i+1 << " is not selected (phi matching)" << endreq;
      }

      m_listCutApplied.push_back(1);
      return false;
    }
  }

  uint8_t nBlay = 0;
  uint8_t nPix  = 0;  
  uint8_t nSCT  = 0; 

  track->summaryValue(nBlay, xAOD::numberOfBLayerHits);
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
    m_log << MSG::VERBOSE << "efTrackSel method" << endreq;
    m_log << MSG::VERBOSE << "  Track number " << i+1 << " to be selected must be:" << endreq;
    m_log << MSG::VERBOSE << "    Pt    " << fabs(pT) << " >= " << m_efTrkSelPt << endreq;
    m_log << MSG::VERBOSE << "    d0    " << fabs(d0) << " <= " << m_efTrkSelD0 << endreq;
    m_log << MSG::VERBOSE << "    bLayer " << (int)nBlay << " >= " << m_efTrkSelBLayer << endreq;
    m_log << MSG::VERBOSE << "    pixelHit " << (int)nPix << " >= " << m_efTrkSelPixHits << endreq; 
    m_log << MSG::VERBOSE << "    SiHit " << nSi << " >= " << m_efTrkSelSiHits << endreq;
    m_log << MSG::VERBOSE << "    Prob(chi2) " << TMath::Prob(track->chiSquared(), (int)nSi*3-5) << " > " << m_efTrkSelChi2 << endreq;
  }

  if (fabs(pT) < m_efTrkSelPt) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (pT cut)" << endreq;

    m_listCutApplied.push_back(2);
    return false;
  }
  if (fabs(d0) > m_efTrkSelD0) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (d0 cut)" << endreq;

    m_listCutApplied.push_back(3);
    return false;
  }
  if (nBlay < m_efTrkSelBLayer) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (missing b-layer hit)" << endreq;

    m_listCutApplied.push_back(4);
    return false;
  }
  if (nPix < m_efTrkSelPixHits) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (too few pixel hits)" << endreq;

    m_listCutApplied.push_back(5);
    return false;
  }
  if (nSi < m_efTrkSelSiHits) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (too few silicon hits)" << endreq;

    m_listCutApplied.push_back(6);
    return false;
  }
  if (TMath::Prob(track->chiSquared(), (int)nSi*3-5) <= m_efTrkSelChi2) {
    if (m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "    track is not selected (chi2 cut)" << endreq;

    m_listCutApplied.push_back(7);
    return false;
  }
  
  if (m_logLvl <= MSG::VERBOSE)
    m_log << MSG::VERBOSE << "    track is selected" << endreq;

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
    m_log << MSG::DEBUG << "Finding primary vertex candidates:" << " histogram with "  << m_hisVtx->getNBin() << " bins" << endreq;      

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
	       << " pos[" << j << "] = " << pos[j] << endreq;
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

 

