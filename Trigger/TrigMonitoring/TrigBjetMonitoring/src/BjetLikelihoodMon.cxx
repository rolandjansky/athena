/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     BjetLikelihoodMon.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrigBjetMonitoring/BjetLikelihoodMon.h"
#include "TrigBjetMonitoring/TaggerHelper.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "Particle/TrackParticleContainer.h"

#include "TMath.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


BjetLikelihoodMon::BjetLikelihoodMon(HLTBjetMonTool* bjetMon, MsgStream* log) :
  m_bjetMon(bjetMon),
  m_log(log),
  m_IP1D(0),
  m_IP2D(0),
  m_totTracks(0),
  m_totSelTracks(0),
  m_totVertices(0),
  m_xPrmVtx(0),
  m_yPrmVtx(0),
  m_zPrmVtx(0)
{
  
  m_taggerHelper = new TaggerHelper(log);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


BjetLikelihoodMon::~BjetLikelihoodMon() {

  if (m_taggerHelper) delete m_taggerHelper;
}


//** ----------------------------------------------------------------------------------------------------------------- **//




bool BjetLikelihoodMon::efTrackSel(const Rec::TrackParticle*& track, unsigned int i, float zv) {

  if(m_bjetMon->m_useEtaPhiTrackSel) {

    if (fabs(track->eta()-m_etaRoI) > 0.2) {

      *m_log << MSG::VERBOSE << "efTrackSel method" << endreq;
      *m_log << MSG::VERBOSE << "  Track number " << i+1 << " is not selected (eta matching)" << endreq;

      m_bjetMon->m_listCutAppliedLikelihood.push_back(2);
      return false;
    }

    if (fabs(m_taggerHelper->phiCorr(m_taggerHelper->phiCorr(track->phi())-m_taggerHelper->phiCorr(m_phiRoI))) > 0.2) {

      *m_log << MSG::VERBOSE << "efTrackSel method" << endreq;
      *m_log << MSG::VERBOSE << "  Track number " << i+1 << " is not selected (phi matching)" << endreq;

      m_bjetMon->m_listCutAppliedLikelihood.push_back(3);
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
			 d0,z0,track->phi(), track->eta(), pT, m_xBeamSpot, m_yBeamSpot);

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

    m_bjetMon->m_listCutAppliedLikelihood.push_back(4);
    return false;
  }
  if (fabs(d0) > m_bjetMon->m_efTrkSelD0) {

    *m_log << MSG::VERBOSE << "    track is not selected (d0 cut)" << endreq;

    m_bjetMon->m_listCutAppliedLikelihood.push_back(5);
    return false;
  }
  if (fabs(z0-zv) > m_bjetMon->m_efTrkSelZ0) {

    *m_log << MSG::VERBOSE << "    track is not selected (z0 cut)" << endreq;

    m_bjetMon->m_listCutAppliedLikelihood.push_back(6);
    return false;
  }
  if (summary->get(Trk::numberOfBLayerHits) < m_bjetMon->m_efTrkSelBLayer) {

    *m_log << MSG::VERBOSE << "    track is not selected (missing b-layer hit)" << endreq;

    m_bjetMon->m_listCutAppliedLikelihood.push_back(7);
    return false;
  }
  if (summary->get(Trk::numberOfPixelHits) < m_bjetMon->m_efTrkSelPixHits) {

    *m_log << MSG::VERBOSE << "    track is not selected (too few pixel hits)" << endreq;

    m_bjetMon->m_listCutAppliedLikelihood.push_back(8);
    return false;
  }
  if (numberOfSiHits < m_bjetMon->m_efTrkSelSiHits) {

    *m_log << MSG::VERBOSE << "    track is not selected (too few silicon hits)" << endreq;

    m_bjetMon->m_listCutAppliedLikelihood.push_back(9);
    return false;
  }
  if (TMath::Prob(quality->chiSquared(), (int)numberOfSiHits*3-5) <= m_bjetMon->m_efTrkSelChi2) {

    *m_log << MSG::VERBOSE << "    track is not selected (chi2 cut)" << endreq;

    m_bjetMon->m_listCutAppliedLikelihood.push_back(10);
    return false;
  }
  
  *m_log << MSG::VERBOSE << "    track is selected" << endreq;

  m_bjetMon->m_listCutAppliedLikelihood.push_back(11);
  return true;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetLikelihoodMon::getIP1DErr(float param) {
  
  float eta = fabs(param);
  float sd0 = 0.173 + 8.43e-3*std::pow(eta,4);

  return sd0;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetLikelihoodMon::getIP2DErr(float param) {

  float pt = fabs(param);
  float p0 = 0.023, p1=27, p2=1.48;
  float sd014 = sqrt(p0*p0 + std::pow((float)(p1/14000.),p2));   
  
  float sd0 = sqrt(p0*p0 + std::pow((float)(p1/pt),p2));
  
  if (pt>14000)
    sd0 = sd014;
  
  return sd0*1.5;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void BjetLikelihoodMon::getLikelihoodTag() {

  //* Initialize m_taggersXMap (taggers string <-> float X) *//
  m_taggersXMap["IP1D"] = -0.4; m_taggersXMap["IP2D"] = -0.4; m_taggersXMap["IP3D"] = -0.4;

  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//




void BjetLikelihoodMon::getLikelihoodTag(const Rec::TrackParticleContainer*& pointerToEFTrackCollections,
					 float m_prmvtx,
					 const ToolHandle<ITrigTrackJetFinderTool> m_trackJetFinderTool) {

  *m_log << MSG::DEBUG << "Executing BjetLikelihoodMon::getLikelihoodTag for taggers based on impact parameters" << endreq;

  m_xPrmVtx = 0; m_yPrmVtx = 0; m_zPrmVtx = 0;

  //* Initialize m_taggersWMap (tagger string <-> float W) *//
  m_taggersWMap["IP1D"] = 1; m_taggersWMap["IP2D"] = 1; m_taggersWMap["IP3D"] = 1;

  //* Initialize m_taggersXMap (tagger string <-> float X) *//
  m_taggersXMap["IP1D"] = -1; m_taggersXMap["IP2D"] = -1; m_taggersXMap["IP3D"] = -1;

  //* TrigTrackJetFinderTool *//
  m_trackJetFinderTool->clear();

  //* Get tracks number *//
  m_totTracks = m_taggerHelper->getTrackNumber(pointerToEFTrackCollections);

  m_zPrmVtx = m_prmvtx;

  //* Beam spot shift *//
  m_xBeamSpot = 0, m_yBeamSpot = 0;

  m_xBeamSpot = m_bjetMon->m_xBeamSpot;  
  m_yBeamSpot = m_bjetMon->m_yBeamSpot;

  //* Perform track selection *//
  std::vector<bool> isSelected(m_totTracks);

  for (unsigned int j = 0; j < m_totTracks; j++) {

    const Rec::TrackParticle* track = (*pointerToEFTrackCollections)[j];
    m_taggerHelper->showParam(track, j);

    //* for monitoring purposes *//
    m_bjetMon->m_mon_ef_trk_a0.push_back(track->measuredPerigee()->parameters()[Trk::d0]);
    m_bjetMon->m_mon_ef_trk_z0.push_back(track->measuredPerigee()->parameters()[Trk::z0]);

    if (efTrackSel(track, j, m_zPrmVtx)) isSelected[j] = true;  

    //* for monitoring purposes *//
    m_bjetMon->m_mon_ef_trk_a0_sel.push_back(track->measuredPerigee()->parameters()[Trk::d0]);
    m_bjetMon->m_mon_ef_trk_z0_sel.push_back(track->measuredPerigee()->parameters()[Trk::z0]);
    m_bjetMon->m_mon_ef_trk_z0_sel_PV.push_back(track->measuredPerigee()->parameters()[Trk::z0]-m_zPrmVtx);

  }

  if (m_bjetMon->m_jetDirection) {

    *m_log << MSG::DEBUG << "Computing jet direction in BjetLikelihoodMon at EF." << endreq;

    //* Primary vertex in TrigTrackJetFinderTool *//
    m_trackJetFinderTool->inputPrimaryVertexZ(m_zPrmVtx);

    //* Loop on tracks to add tracks to TrigTrackJetFinderTool *//
    for (unsigned int j = 0; j < m_totTracks; j++) {
      const Rec::TrackParticle* track = (*pointerToEFTrackCollections)[j];

      if (!isSelected[j]) continue;

      m_trackJetFinderTool->addTrack(track, j);
    }

    std::vector<int> tracksTrackJet;
    float etaTrackJet, phiTrackJet;

    //* Find jet direction *//
    m_trackJetFinderTool->findJet(tracksTrackJet, etaTrackJet, phiTrackJet);

    if (etaTrackJet != -99 && phiTrackJet != -99) {
      
      *m_log << MSG::DEBUG << "eta RoI = " << m_etaRoI << " ; eta Jet at EF = " << etaTrackJet << endreq;
      *m_log << MSG::DEBUG << "phi RoI = " << m_phiRoI << " ; phi Jet at EF = " << phiTrackJet << endreq;
      
      m_etaRoI = etaTrackJet; m_phiRoI = phiTrackJet;
      
    } else 
      *m_log << MSG::DEBUG << "eta Jet = eta RoI" << endreq;
  }

  std::vector<std::string>::iterator pTagger = m_bjetMon->m_taggers.begin();
  std::vector<std::string>::iterator lastTagger = m_bjetMon->m_taggers.end();

  m_totSelTracks = 0;

  //* Loop over tracks to calculate W *//
  for (unsigned int j = 0; j < m_totTracks; j++) {
    
    const Rec::TrackParticle* track = (*pointerToEFTrackCollections)[j];

    if (!isSelected[j]) continue;
    
    m_totSelTracks++;
    
    float w=1;
    
    float errIP1D, errIP2D;
    float z0, z0Sign, d0, d0Sign, phiTrack=0, etaTrack=0;
    
    if (m_bjetMon->m_useErrIPParam) {
      errIP1D = getIP1DErr(track->eta());
      errIP2D = getIP2DErr(track->pt());   
    } else {
      errIP1D = Amg::error(*(track->measuredPerigee()->covariance()),Trk::z0);
      errIP2D = Amg::error(*(track->measuredPerigee()->covariance()),Trk::d0);
    }
    
    d0 = track->measuredPerigee()->parameters()[Trk::d0];
    z0 = track->measuredPerigee()->parameters()[Trk::z0];
    phiTrack = track->phi();
    etaTrack = track->eta();

    float theta = track->measuredPerigee()->parameters()[Trk::theta];
    float qOverPt = track->measuredPerigee()->parameters()[Trk::qOverP]/sin(theta);
    float pT = (1/qOverPt);
    m_taggerHelper->IPCorr(track->measuredPerigee()->parameters()[Trk::d0], track->measuredPerigee()->parameters()[Trk::z0], d0, z0,
			   phiTrack, etaTrack, pT, m_xBeamSpot, m_yBeamSpot);

    z0 = z0 - m_zPrmVtx;
    d0Sign = m_taggerHelper->signedD0(d0, phiTrack, m_phiRoI);
    z0Sign = m_taggerHelper->signedZ0(z0, etaTrack, m_etaRoI);

    float m_sigmaBeamSpot;
    m_sigmaBeamSpot = (m_bjetMon->m_sigmaX+m_bjetMon->m_sigmaY)/2;

    pTagger = m_bjetMon->m_taggers.begin();

    for ( ; pTagger != lastTagger; pTagger++) {
      
      if ((*pTagger).find("IP") == std::string::npos)
	continue;

      if (!j) 
	*m_log << MSG::DEBUG << "Calculating " << (*pTagger) << " likelihood weight" << endreq;

      if ((*pTagger) == "IP1D") {
	
	m_IP1D = z0Sign/sqrt(errIP1D*errIP1D);
	w = getW("EF_IP1D", scaleVar(m_IP1D));
	
      } else if ((*pTagger) == "IP2D") {
	
	m_IP2D = d0Sign/sqrt(errIP2D*errIP2D + m_sigmaBeamSpot*m_sigmaBeamSpot);
	w = getW("EF_IP2D", scaleVar(m_IP2D));

      } else if ((*pTagger) == "IP3D") { 
	
	m_IP1D = z0Sign/sqrt(errIP1D*errIP1D);
	m_IP2D = d0Sign/sqrt(errIP2D*errIP2D + m_sigmaBeamSpot*m_sigmaBeamSpot);
	w = getW("EF_IP3D", scaleVar(m_IP2D), scaleVar(m_IP1D));

	//* for monitoring purposes *//
	m_bjetMon->m_mon_ef_trk_Sa0_sel.push_back(m_IP1D);
	m_bjetMon->m_mon_ef_trk_Sz0_sel.push_back(m_IP2D);

      }

      m_taggersWMap[(*pTagger)]*= w;

      *m_log << MSG::VERBOSE << "---> w(" << (*pTagger) << ") = " << w
	     << ";   W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)] << endreq;  
    }
  }
  
  pTagger = m_bjetMon->m_taggers.begin();

  if (!m_totSelTracks) {

    for ( ; pTagger != lastTagger; pTagger++)
      m_taggersXMap[(*pTagger)] = -0.3;
    
  } else {

    for ( ; pTagger != lastTagger; pTagger++)
      m_taggersXMap[(*pTagger)] = getX(m_taggersWMap[(*pTagger)]);

  }

  return;
}

//** ----------------------------------------------------------------------------------------------------------------- **//


void BjetLikelihoodMon::getBestLikelihoodTag() {
  
  if (m_taggersWMap["IP3D"] != 1 && m_taggersWMap["SVTX"] != 1)
    m_taggersWMap["COMB"] = m_taggersWMap["IP3D"]*m_taggersWMap["SVTX"];
  else if (m_taggersWMap["IP3D"] != 0)
    m_taggersWMap["COMB"] = m_taggersWMap["IP3D"];
  else
    m_taggersWMap["COMB"] = 1;

  m_taggersXMap["COMB"] = getX(m_taggersWMap["COMB"]);

  return; 
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetLikelihoodMon::getW(const std::string tagger, float val) {

  float min, max, num;
  float b, u, w = 1;

  int index;

  min = *m_likelihoodMap[tagger]->getMinXLikelihood();
  max = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  num = *m_likelihoodMap[tagger]->getNumXLikelihood();

  index = (int)floor(((val - min)/(max - min))*num);

  if (index > (num - 1)) index = (int)num - 1;
  else if (index < 0)    index = 0;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) 
    w = b/u; 
  else if (b != 0)
    w=1000.;

  return w;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetLikelihoodMon::getW(const std::string tagger, float valX, float valY) {
    
  float minX, maxX, numX;
  float minY, maxY, numY;
  float b, u, w = 1;

  int indexX, indexY, index;

  minX = *m_likelihoodMap[tagger]->getMinXLikelihood();
  maxX = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  numX = *m_likelihoodMap[tagger]->getNumXLikelihood();

  minY = *m_likelihoodMap[tagger]->getMinYLikelihood();
  maxY = *m_likelihoodMap[tagger]->getMaxYLikelihood();
  numY = *m_likelihoodMap[tagger]->getNumYLikelihood();

  indexX = (int)floor(((valX - minX)/(maxX - minX))*numX);
  indexY = (int)floor(((valY - minY)/(maxY - minY))*numY);

  if (indexX > (numX - 1)) indexX = (int)numX - 1;
  else if (indexX < 0)     indexX = 0;

  if (indexY > (numY - 1)) indexY = (int)numY - 1;
  else if (indexY < 0)     indexY = 0;

  index = indexX*(int)numY + indexY;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) 
    w = b/u; 
  else if (b != 0)
    w=1000.;
 
  return w;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetLikelihoodMon::getW(const std::string tagger, float valX, float valY, float valZ) {
    
  float minX, maxX, numX;
  float minY, maxY, numY;
  float minZ, maxZ, numZ;
  float b, u, w = 1;

  int indexX, indexY, indexZ, index;

  minX = *m_likelihoodMap[tagger]->getMinXLikelihood();
  maxX = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  numX = *m_likelihoodMap[tagger]->getNumXLikelihood();

  minY = *m_likelihoodMap[tagger]->getMinYLikelihood();
  maxY = *m_likelihoodMap[tagger]->getMaxYLikelihood();
  numY = *m_likelihoodMap[tagger]->getNumYLikelihood();

  minZ = *m_likelihoodMap[tagger]->getMinZLikelihood();
  maxZ = *m_likelihoodMap[tagger]->getMaxZLikelihood();
  numZ = *m_likelihoodMap[tagger]->getNumZLikelihood();

  indexX = (int)floor(((valX - minX)/(maxX - minX))*numX);
  indexY = (int)floor(((valY - minY)/(maxY - minY))*numY);
  indexZ = (int)floor(((valZ - minZ)/(maxZ - minZ))*numZ);

  if (indexX > (numX - 1)) indexX = (int)numX - 1;
  else if (indexX < 0)     indexX = 0;

  if (indexY > (numY - 1)) indexY = (int)numY - 1;
  else if (indexY < 0)     indexY = 0;

  if (indexZ > (numZ - 1)) indexZ = (int)numZ - 1;
  else if (indexZ < 0)     indexZ = 0;

  index = indexX*(int)(numZ*numY) + indexY*(int)numZ + indexZ;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) 
    w = b/u; 
  else if (b != 0)
    w=1000.;

  return w;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetLikelihoodMon::getX(float w) {
  
  if (w == 1) 
    return 0;

  return w/(1+w);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float BjetLikelihoodMon::scaleVar(const float& val) {

  if (!val)
    return val;

  float fval,sval;
  fval = fabs(val);      
  sval = (int) (val/fval);

  return log(1+fval)*sval;
}





