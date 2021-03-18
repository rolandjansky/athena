/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "TrigT2HistoPrmVtxBaseMT.h"
#include "HistoVertexHelper.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "Particle/TrackParticleContainer.h"

#include "TMath.h"
#include "CxxUtils/phihelper.h"


TrigT2HistoPrmVtxBaseMT::TrigT2HistoPrmVtxBaseMT( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm( name,pSvcLocator ),
  m_zPrmVtx(3),
  m_zPrmVtxSigmaAll(3),
  m_zPrmVtxSigma2Trk(3),
  m_nTrackVtx(3),
  m_nVtxFound(0),
  m_totTracks(0),
  m_totTracks_All(0),
  m_totSelTracks(0),
  m_totSelTracks_All(0) {}


//** ----------------------------------------------------------------------------------------------------------------- **//

StatusCode TrigT2HistoPrmVtxBaseMT::initialize() {

  ATH_MSG_DEBUG( "declareProperty review:"  );
  ATH_MSG_DEBUG( " AlgoId = "              << m_algo  );
  ATH_MSG_DEBUG( " Numbins = "             << m_nBins  );
  ATH_MSG_DEBUG( " UseBeamSpot = "         << m_useBeamSpot  );
  ATH_MSG_DEBUG( " UseEtaPhiTrackSel = "   << m_useEtaPhiTrackSel  );
  
  ATH_MSG_DEBUG( " HLTTrkSel_Chi2 = "     << m_hltTrkSelChi2  );
  ATH_MSG_DEBUG( " HLTTrkSel_BLayer = "   << m_hltTrkSelBLayer  );
  ATH_MSG_DEBUG( " HLTTrkSel_PixHits = "  << m_hltTrkSelPixHits  );
  ATH_MSG_DEBUG( " HLTTrkSel_SiHits = "   << m_hltTrkSelSiHits  );
  ATH_MSG_DEBUG( " HLTTrkSel_D0 = "       << m_hltTrkSelD0  );
  ATH_MSG_DEBUG( " HLTTrkSel_Pt = "       << m_hltTrkSelPt  );    

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigT2HistoPrmVtxBaseMT::trackSel(const xAOD::TrackParticle* track, unsigned int i, float eta, float phi) {

  if(m_useEtaPhiTrackSel) {

    if (fabs(track->eta()-eta) > 0.2) {
      ATH_MSG_VERBOSE( "efTrackSel method" );
      ATH_MSG_VERBOSE( "  Track number " << i+1 << " is not selected (eta matching)" );

      m_listCutApplied.push_back(0);
      return false;
    }

    if (fabs(CxxUtils::wrapToPi(CxxUtils::wrapToPi(track->phi())-CxxUtils::wrapToPi(phi))) > 0.2) {
      ATH_MSG_VERBOSE( "efTrackSel method" );
      ATH_MSG_VERBOSE( "  Track number " << i+1 << " is not selected (phi matching)" );

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

  ATH_MSG_VERBOSE( "efTrackSel method" );
  ATH_MSG_VERBOSE( "  Track number " << i+1 << " to be selected must be:" );
  ATH_MSG_VERBOSE( "    Pt    " << fabs(pT) << " >= " << m_hltTrkSelPt );
  ATH_MSG_VERBOSE( "    d0    " << fabs(d0) << " <= " << m_hltTrkSelD0 );
  ATH_MSG_VERBOSE( "    bLayer " << (int)nBlay << " >= " << m_hltTrkSelBLayer );
  ATH_MSG_VERBOSE( "    pixelHit " << (int)nPix << " >= " << m_hltTrkSelPixHits );
  ATH_MSG_VERBOSE( "    SiHit " << nSi << " >= " << m_hltTrkSelSiHits );
  ATH_MSG_VERBOSE( "    Prob(chi2) " << TMath::Prob(track->chiSquared(), (int)nSi*3-5) << " > " << m_hltTrkSelChi2 );

  if (fabs(pT) < m_hltTrkSelPt) {
    ATH_MSG_VERBOSE( "    track is not selected (pT cut)" );

    m_listCutApplied.push_back(2);
    return false;
  }
  if (fabs(d0) > m_hltTrkSelD0) {
    ATH_MSG_VERBOSE( "    track is not selected (d0 cut)" );

    m_listCutApplied.push_back(3);
    return false;
  }
  if (expBL && (nBlay < m_hltTrkSelBLayer) ){
    ATH_MSG_VERBOSE( "    track is not selected (missing b-layer hit)" );

    m_listCutApplied.push_back(4);
    return false;
  }
  if (nPix < m_hltTrkSelPixHits) {
    ATH_MSG_VERBOSE( "    track is not selected (too few pixel hits)" );

    m_listCutApplied.push_back(5);
    return false;
  }
  if (nSi < m_hltTrkSelSiHits) {
    ATH_MSG_VERBOSE( "    track is not selected (too few silicon hits)" );

    m_listCutApplied.push_back(6);
    return false;
  }
  if (TMath::Prob(track->chiSquared(), (int)nSi*3-5) <= m_hltTrkSelChi2) {
    ATH_MSG_VERBOSE( "    track is not selected (chi2 cut)" );

    m_listCutApplied.push_back(7);
    return false;
  }
  
  ATH_MSG_VERBOSE( "    track is selected" );

  m_listCutApplied.push_back(8);
  return true;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigT2HistoPrmVtxBaseMT::findPrmVtx( std::unique_ptr< HistoVertexHelper >& hisVtx ) {

  // Finding primary vertex
  std::vector< int > maxVal(3), pos(3);
  std::vector< float > numMean(3), numSigma(3), den(3);

  int val=0; 
  m_nVtxFound=0;

  ATH_MSG_DEBUG( "Finding primary vertex candidates:" << " histogram with "  << hisVtx->getNBin() << " bins" );

  for (int j=0; j<3; j++) {
    val=0;

    for(int i=0; i<m_nBins; i++) 
      val += hisVtx->getN(i);

    maxVal[j] = val;

    for (int i=m_nBins; i<hisVtx->getNBin(); i++) {     
    
      if (val > maxVal[j]) {
	maxVal[j] = val;
	pos[j] = i - m_nBins;
      }

      val -= hisVtx->getN(i-m_nBins);
      val += hisVtx->getN(i);
    }

    ATH_MSG_VERBOSE( " maxVal[" << j << "] = " << maxVal[j]
		     << " pos[" << j << "] = " << pos[j] );

    for (int i = pos[j]; i < (pos[j]+m_nBins); i++) {
      float nCont = hisVtx->getN(i);
      den[j] += nCont;
      numMean[j] += i*nCont;
    }

    if (den[j] != 0) {

      m_nVtxFound++;
      m_zPrmVtx.at(j) = hisVtx->getZ(numMean[j]/den[j]);   	 
      m_nTrackVtx.at(j) = hisVtx->getIntegral(pos[j], m_nBins);

      for (int i = pos[j]; i < (pos[j]+m_nBins); i++)
	for (int k=0; k<hisVtx->getN(i); k++)
	  numSigma[j] += (hisVtx->getZ(i)-m_zPrmVtx.at(j)) * (hisVtx->getZ(i)-m_zPrmVtx.at(j));

      if (den[j] == 1) {
	m_zPrmVtxSigmaAll.at(j)  = 0.025;
	m_zPrmVtxSigma2Trk.at(j) = 0.025;
      } else if (m_nTrackVtx.at(j)<20) {
	m_zPrmVtxSigmaAll.at(j)  = sqrt(numSigma[j]/(den[j]-1));
	m_zPrmVtxSigma2Trk.at(j) = sqrt(numSigma[j]/(den[j]-1));
      } else {
	m_zPrmVtxSigmaAll.at(j)  = sqrt(numSigma[j]/den[j]);
	m_zPrmVtxSigma2Trk.at(j) = sqrt(numSigma[j]/den[j]);
      }

      hisVtx->reset(pos[j], m_nBins);

    } else {

      m_zPrmVtx.at(j)          = -200;
      m_zPrmVtxSigmaAll.at(j)  = -200;
      m_zPrmVtxSigma2Trk.at(j) = -200;
      break;

    }
  }
} 

 

