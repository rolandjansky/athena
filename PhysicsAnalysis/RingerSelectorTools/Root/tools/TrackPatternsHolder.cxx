/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackPatternsHolder.cxx 693573 2015-09-07 19:15:49Z wsfreund $
#ifndef RINGER_STANDALONE
// Local includes:
#include "RingerSelectorTools/tools/TrackPatternsHolder.h"
#include "RingerSelectorTools/tools/cxx/RingerUseNewCppFeatures.h"

// STL includes:
#include <cmath>


// xAOD includes:
# include "xAODTracking/TrackParticle.h"
# include "xAODEgamma/EgammaEnums.h"
# include "xAODEgamma/Electron.h"

#if !defined(XAOD_STANDALONE) && !defined(RINGER_STANDALONE)
# include "TrkTrackSummary/TrackSummary.h"
#else
// XXX This tool should be implemented in AsgTool enviroment... 
namespace Trk {
namespace TrackSummary {
const int SummaryTypeNotSet = -1;
} // namespace TrackSummary;
} // namespace Trk
#endif

namespace Ringer {

#if !(RINGER_USE_NEW_CPP_FEATURES)
namespace {
float cutBinEta_TRT[] = {0.1, 0.625, 1.07, 1.304, 1.752, 2.0};
} // private namespace
const double TrackPatternsHolder::m_a0 = 33.14; 
const double TrackPatternsHolder::m_b0 = -129.1; 
const double TrackPatternsHolder::m_c0 = 1455.;
const double TrackPatternsHolder::m_a1 = 29.42; 
const double TrackPatternsHolder::m_b1 = 27.93; 
const double TrackPatternsHolder::m_c1 = -89.96; 
const double TrackPatternsHolder::m_d1 = 91.51;
const double TrackPatternsHolder::m_a2 = 196.3; 
const double TrackPatternsHolder::m_b2 = -403.; 
const double TrackPatternsHolder::m_c2 = 230.2;
const double TrackPatternsHolder::m_a3 = -10.59; 
const double TrackPatternsHolder::m_b3 = 37.29;
const double TrackPatternsHolder::m_a4 = -640.9; 
const double TrackPatternsHolder::m_b4 = 1323.; 
const double TrackPatternsHolder::m_c4 = -851.8; 
const double TrackPatternsHolder::m_d4 = 180.8;
const double TrackPatternsHolder::m_a5 = 159.8; 
const double TrackPatternsHolder::m_b5 = -70.9;
const std::vector<float> TrackPatternsHolder::m_cutBinEta_TRT = 
  std::vector<float>(cutBinEta_TRT, cutBinEta_TRT + sizeof(cutBinEta_TRT) / sizeof(float));
#else
constexpr std::array<float,6> TrackPatternsHolder::m_cutBinEta_TRT;
#endif

// =============================================================================
void TrackPatternsHolder::resetValues()
{
  m_nBL = 0;
  m_nBLOutliers = 0;
  m_nPi = 0;
  m_nPiOutliers = 0;
  m_nSCT = 0;
  m_nSCTOutliers = 0;
  m_nTRThigh          = 0;
  m_nTRThighOutliers  = 0;
  m_nTRT         = 0;
  m_nTRTOutliers = 0;
  m_nTRTXenonHits = 0;
  m_expectHitInBLayer = true;
  m_trackd0 = 0;
  m_deltaeta = 0;
  m_deltaphi = 0;
  m_ep = 0;

  m_energy = 0;
  m_eta = 0;
}

// =============================================================================
void TrackPatternsHolder::extractPatternsFrom(
    const xAOD::TrackParticle *track,
    const xAOD::Electron *el)
{

  // Reset values to default:
  resetValues();

  // Get cluster energy:
  m_energy =  el->caloCluster()->e();
  m_eta = std::fabs(el->caloCluster()->eta());

  bool allFound = true;

  allFound = allFound   && track->summaryValue(m_nBL,               xAOD::numberOfInnermostPixelLayerHits);
  if (m_useBLOutliers) {
    allFound = allFound && track->summaryValue(m_nBLOutliers,       xAOD::numberOfInnermostPixelLayerOutliers);
  }
  allFound = allFound   && track->summaryValue(m_nPi,               xAOD::numberOfPixelHits);
  if (m_usePIXOutliers){
    allFound = allFound && track->summaryValue(m_nPiOutliers,       xAOD::numberOfPixelOutliers);
  }
  allFound = allFound   && track->summaryValue(m_nSCT,              xAOD::numberOfSCTHits);
  if (m_useSCTOutliers){
    allFound = allFound && track->summaryValue(m_nSCTOutliers,      xAOD::numberOfSCTOutliers);
  }
  allFound = allFound   && track->summaryValue(m_nTRThigh,          xAOD::numberOfTRTHighThresholdHits);
  if (m_useTRTOutliers){
    allFound = allFound && track->summaryValue(m_nTRThighOutliers,  xAOD::numberOfTRTHighThresholdOutliers);
  }
  allFound = allFound   && track->summaryValue(m_nTRT,              xAOD::numberOfTRTHits);
  allFound = allFound   && track->summaryValue(m_nTRTOutliers,      xAOD::numberOfTRTOutliers);
  if (m_useTRTXenonHits){
    allFound = allFound && track->summaryValue(m_nTRTXenonHits,     xAOD::numberOfTRTXenonHits);
  }
  allFound = allFound   && track->summaryValue(m_expectHitInBLayer, xAOD::expectInnermostPixelLayerHit);

  m_trackd0 = fabsf(track->d0());

  allFound = allFound && el->trackCaloMatchValue(m_deltaeta,        xAOD::EgammaParameters::deltaEta1);
  allFound = allFound && el->trackCaloMatchValue(m_deltaphi,        xAOD::EgammaParameters::deltaPhi2);

  m_ep = m_energy * fabs(track->qOverP());

  if (!allFound) {
    // if object is bad then use the bit for "bad eta"
    ATH_MSG_WARNING("Have some variables missing.");
  }

}

// =============================================================================
void TrackPatternsHolder::exportPatternsTo(std::vector<float> &vec) const 
{

  // Copy originals:
  uint8_t nBL = m_nBL;
  uint8_t nPi = m_nPi;
  uint8_t nSi = m_nPi + m_nSCT;

  // Add outliers:
  if (m_useBLOutliers) {
    nBL += m_nBLOutliers;
  }

  if (m_usePIXOutliers) {
    nPi += m_nPiOutliers;
    nSi += m_nPiOutliers;
  }

  if (m_useSCTOutliers) {
    nSi += m_nSCTOutliers;
  }

  // Track quality cuts (CutID only dependent on eta)
  //@{
  // egammaPID::TrackBlayer_Electron (CutID uses min
  // value cut and check if modules was alive)
  vec.push_back(nBL);  // Pattern 0

  // We, instead, put it as a pattern
  vec.push_back(m_expectHitInBLayer); // Pattern 1

  // egammaPID::TrackPixel_Electron (CutID uses min value cut)
  vec.push_back(nPi); // Pattern 2

  // egammaPID::TrackSi_Electron (CutID uses min value cut)
  vec.push_back(nSi); // Pattern 3

  // egammaPID::TrackA0_Electron || egammaPID::TrackA0Tight_Electron (CutID
  // uses max value cut)
  vec.push_back(m_trackd0); // Pattern 4
  //@}

  // CutID dependent on eta & et
  //@{ 
  // egammaPID::TrackMatchEta_Electron (CutID uses max value cut)
  vec.push_back(m_deltaeta); // Pattern 5

  // egammaPID::TrackMatchPhi_Electron (CutID uses min & max value cuts)
  vec.push_back(m_deltaphi); // Pattern 6

  // egammaPID::TrackMatchEoverP_Electron (CutID uses min & max value cuts)
  vec.push_back(m_ep);  // Pattern 7
  //@}

  // Use of Transition Radiation Tracker
  //@{ 
  double rTRT(0);
  double trt_estimated_hits(0);
  getTRTVar(rTRT,trt_estimated_hits);

  // egammaPID::TrackTRTratio(90)_Electron (CutID min value):
  vec.push_back(rTRT); // Pattern 8

  // egammaPID::TrackTRThits_Electron (CutID min value)
  vec.push_back(trt_estimated_hits); // Pattern 9
  // @}
}

// =============================================================================
void TrackPatternsHolder::getTRTVar(double &rTRT, double &estHitsTRT) const
{
  int nTRTTotal(0);
  if (m_useTRTOutliers) {
    if (m_useTRTXenonHits /*&& 
        (static_cast<int>(m_nTRTXenonHits) != 
         Trk::TrackSummary::SummaryTypeNotSet)*/) //<- is it always true ? 
    {
      nTRTTotal= m_nTRTXenonHits;
    } else {
      nTRTTotal = m_nTRT+m_nTRTOutliers;
    }
    rTRT = (nTRTTotal) > 0 ?
      ((double) (m_nTRThigh+m_nTRThighOutliers)/(nTRTTotal) ) : 0.;
  } else {
    rTRT = (m_nTRT) > 0 ? ((double) (m_nTRThigh)/(m_nTRT) ) : 0.;
    nTRTTotal = m_nTRT;
  }
  // FIXME Why isn't this in a function available anywhere else?
  estHitsTRT = -1;
  int ibin_eta_TRT = -1;
  for (unsigned int ibinEta=0;ibinEta<m_cutBinEta_TRT.size();ibinEta++) {
    if ( ibinEta == 0 ){
      if ( m_eta < m_cutBinEta_TRT[ibinEta] ) {
        ibin_eta_TRT = ibinEta;
      }
    } else {
      if ( m_eta >= m_cutBinEta_TRT[ibinEta-1] &&
           m_eta < m_cutBinEta_TRT[ibinEta] ) {
        ibin_eta_TRT = ibinEta;
      }
    }
  }
  switch (ibin_eta_TRT) {
  case 0:
    estHitsTRT = nTRTTotal - 
      (m_a0 + m_b0*m_eta + m_c0*m_eta*m_eta);
    break;
  case 1:
    estHitsTRT = nTRTTotal - 
      (m_a1 + m_b1*m_eta + m_c1*m_eta*m_eta + m_d1*m_eta*m_eta*m_eta);
    break;
  case 2:
    estHitsTRT = nTRTTotal - 
      (m_a2 + m_b2*m_eta + m_c2*m_eta*m_eta) ;
    break;
  case 3:
    estHitsTRT = nTRTTotal - (m_a3 + m_b3*m_eta);
    break;
  case 4:
    estHitsTRT = nTRTTotal - 
      (m_a4 + m_b4*m_eta + m_c4*m_eta*m_eta + m_d4*m_eta*m_eta*m_eta);
    break;
  case 5:
    estHitsTRT = nTRTTotal - (m_a5 + m_b5*m_eta);
  }
}


} // namespace Ringer

#endif // RINGER_STANDALONE
