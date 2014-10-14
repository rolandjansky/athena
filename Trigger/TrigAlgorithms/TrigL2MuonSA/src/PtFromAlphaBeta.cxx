/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/PtFromAlphaBeta.h"

#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtFromAlphaBeta::PtFromAlphaBeta(MsgStream* msg,
					       const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc): 
   m_msg(msg),
   m_ptEndcapLUT(ptEndcapLUTSvc->ptEndcapLUT())
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtFromAlphaBeta::~PtFromAlphaBeta() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtFromAlphaBeta::setPt(TrigL2MuonSA::TrackPattern& trackPattern)
{

  if (trackPattern.etaBin < -1) 
    return StatusCode::FAILURE;
  
  const float ZERO_LIMIT = 1e-5;
  
  // use the TGC PT if the MDT fit is not available
  if ( fabs(trackPattern.slope)<ZERO_LIMIT && fabs(trackPattern.intercept)<ZERO_LIMIT )
    return StatusCode::SUCCESS;
  
  float tgcPt = fabs(trackPattern.pt) * trackPattern.charge;
  
  // MDT pT by alpha
  int  side   = (trackPattern.etaMap <= 0.0) ? 0 : 1;
  int  charge = (trackPattern.intercept * trackPattern.etaMap) < 0.0 ? 0 : 1;

  float mdtPt = m_ptEndcapLUT->lookup(side, charge, PtEndcapLUT::ALPHAPOL2, 2,  trackPattern.etaBin,
				      trackPattern.phiBin, trackPattern.endcapAlpha) / 1000;//sector=2 All

  if (charge == 0)  mdtPt = -mdtPt;
  trackPattern.ptEndcapAlpha = mdtPt;//pt calculated by alpha
  //
  const float ALPHA_TO_BETA_PT    = 10;
  const float ALPHA_TO_BETA_RATIO = 0.5;
  
  // use MDT beta if condition allows
  if (fabs(mdtPt) > ALPHA_TO_BETA_PT && fabs(trackPattern.endcapBeta)>ZERO_LIMIT) {
    float betaPt = m_ptEndcapLUT->lookup(side, charge, PtEndcapLUT::BETAPOL2, 2, trackPattern.etaBin,
					 trackPattern.phiBin, trackPattern.endcapBeta) / 1000;//sector=2 All

    if (charge == 0)  betaPt = -betaPt;
    trackPattern.ptEndcapBeta = betaPt;//pt calculated by beta

    int outer = xAOD::L2MuonParameters::Chamber::EndcapOuter;
    if ( fabs((betaPt - mdtPt) / mdtPt) < ALPHA_TO_BETA_RATIO ) {
      mdtPt = betaPt;
    } else if ( fabs(trackPattern.superPoints[outer].Z) < ZERO_LIMIT) {
      if( fabs(betaPt) > fabs(mdtPt) || (fabs((tgcPt-mdtPt)/mdtPt) > fabs((tgcPt-betaPt)/betaPt)) ) mdtPt = betaPt;
    }
  }
  if (trackPattern.endcapRadius3P>0) {//calculate pt from radius
    msg() << MSG::DEBUG << "calculate pt from invR" << endreq;
    float invR = 1. / trackPattern.endcapRadius3P;

    if (trackPattern.smallLarge==0) 
      trackPattern.ptEndcapRadius =  m_ptEndcapLUT->lookup(side, charge, PtEndcapLUT::INVRADIUSPOL2, 0, trackPattern.etaBin, 
							   trackPattern.phiBin24, invR) / 1000;//sector=0 small
    if (trackPattern.smallLarge==1) 
      trackPattern.ptEndcapRadius =  m_ptEndcapLUT->lookup(side, charge, PtEndcapLUT::INVRADIUSPOL2, 1, trackPattern.etaBin, 
							   trackPattern.phiBin24, invR) / 1000;//sector=1 large
  }

  if(mdtPt!=0.0) {
    trackPattern.pt     = fabs(mdtPt);
    trackPattern.charge = mdtPt / fabs(mdtPt);
  }
  
  msg() << MSG::DEBUG << "pT determined from alpha and beta: endcapAlpha/endcapBeta/endcapRadius3P/pT/charge/s_address="
	<< trackPattern.endcapAlpha << "/" << trackPattern.endcapBeta << "/" << trackPattern.endcapRadius3P << "/" << trackPattern.pt
	<< "/" << trackPattern.charge << "/" << trackPattern.s_address << endreq;
  msg() << MSG::DEBUG << "ptEndcapAlpha/ptEndcapBeta/ptEndcapRadius="
	<< trackPattern.ptEndcapAlpha << "/" << trackPattern.ptEndcapBeta << "/" << trackPattern.ptEndcapRadius << endreq;

  return StatusCode::SUCCESS; 
}
