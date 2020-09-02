/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PtFromAlphaBeta.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtFromAlphaBeta::PtFromAlphaBeta(const std::string& type,
					       const std::string& name,
					       const IInterface*  parent):
  AthAlgTool(type, name, parent) 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtFromAlphaBeta::initialize()
{
  ATH_MSG_DEBUG(m_use_cscpt);
  // 
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::PtFromAlphaBeta::setMCFlag(BooleanProperty use_mcLUT,
					      const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc)
{
  m_use_mcLUT = use_mcLUT;
  m_ptEndcapLUT = ptEndcapLUTSvc->ptEndcapLUT();
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtFromAlphaBeta::setPt(TrigL2MuonSA::TrackPattern& trackPattern,
                                                TrigL2MuonSA::TgcFitResult& tgcFitResult)
{

  if (trackPattern.etaBin < -1) 
    return StatusCode::FAILURE;
  
  const float ZERO_LIMIT = 1e-5;
  
  // use the TGC PT if the MDT fit is not available
  if ( fabs(trackPattern.slope)<ZERO_LIMIT && fabs(trackPattern.intercept)<ZERO_LIMIT )
    return StatusCode::SUCCESS;
  
  const float tgcPt = tgcFitResult.tgcPT ;
  
  // MDT pT by alpha
  const int  side   = (trackPattern.etaMap <= 0.0) ? 0 : 1;
  const int  charge = (trackPattern.intercept * trackPattern.etaMap) < 0.0 ? 0 : 1;

  float mdtPt = (*m_ptEndcapLUT)->lookup(side, charge, PtEndcapLUT::ALPHAPOL2, trackPattern.etaBin,
				      trackPattern.phiBin, trackPattern.endcapAlpha) / 1000;

  if (charge == 0)  mdtPt = -mdtPt;
  trackPattern.ptEndcapAlpha = mdtPt;//pt calculated by alpha
  //
  const float ALPHA_TO_BETA_PT    = 10;
  const float ALPHA_TO_BETA_RATIO = 0.5;
  const float ALPHA_TO_CSC_RATIO = 0.3;
  const float ALPHA_TO_CSC_RATIO_PT = 0.025;

  // use MDT beta if condition allows
  if (fabs(mdtPt) > ALPHA_TO_BETA_PT && fabs(trackPattern.endcapBeta)>ZERO_LIMIT) {
    float betaPt = (*m_ptEndcapLUT)->lookup(side, charge, PtEndcapLUT::BETAPOL2, trackPattern.etaBin,
					 trackPattern.phiBin, trackPattern.endcapBeta) / 1000;

    if (charge == 0)  betaPt = -betaPt;
    trackPattern.ptEndcapBeta = betaPt;//pt calculated by beta

    const int outer = xAOD::L2MuonParameters::Chamber::EndcapOuter;
    if ( fabs((betaPt - mdtPt) / mdtPt) < ALPHA_TO_BETA_RATIO ) {
      mdtPt = betaPt;
    } else if ( fabs(trackPattern.superPoints[outer].Z) < ZERO_LIMIT) {
      if( fabs(betaPt) > fabs(mdtPt) || (fabs((tgcPt-mdtPt)/mdtPt) > fabs((tgcPt-betaPt)/betaPt)) ) mdtPt = betaPt;
    }
  }
  if (trackPattern.endcapRadius3P>0) {//calculate pt from radius
    ATH_MSG_DEBUG("calculate pt from Radius");
    const float invR = 1. / trackPattern.endcapRadius3P;

    if (trackPattern.etaBin<8){
      trackPattern.ptEndcapRadius =  (*m_ptEndcapLUT)->lookup(side, charge, PtEndcapLUT::INVRADIUSPOL2, 
                                          trackPattern.etaBin, trackPattern.phiBinEE, invR) / 1000;
    }
  }
  if ( fabs(trackPattern.cscGamma)>ZERO_LIMIT ){

    float cscPt = (*m_ptEndcapLUT)->lookup(side, charge, PtEndcapLUT::CSCPOL2, trackPattern.etaBin,
					trackPattern.phiBin, trackPattern.cscGamma) / 1000;
    if (charge == 0)  cscPt = -cscPt;
    trackPattern.ptCSC = cscPt;
  }
  if(mdtPt!=0.0) {
    trackPattern.pt     = fabs(mdtPt);
    trackPattern.charge = mdtPt / fabs(mdtPt);
  }

  if (trackPattern.ptEndcapRadius>0 && trackPattern.ptEndcapRadius<500)
      trackPattern.pt = trackPattern.ptEndcapRadius;//use pt calculated from endcap radius

  if(m_use_cscpt){
    const float &cscPt = trackPattern.ptCSC;
    const int &etabin = trackPattern.etaBin;
    const bool validrange = (20<=etabin && etabin<=27) || (etabin==20 && abs(side-charge)!=1);//side-charge==0 <=> Qeta==1
    const bool validchamber = !m_avoid_misaligned_cscs || (16!=trackPattern.hashID_CSC && 17!=trackPattern.hashID_CSC);
    if( etabin !=23 && etabin!=24 &&  validrange && validchamber){
      if(fabs(trackPattern.ptEndcapBeta)<ZERO_LIMIT && fabs(cscPt)>ZERO_LIMIT 
	 &&  fabs((cscPt - mdtPt) / mdtPt)<ALPHA_TO_CSC_RATIO && fabs(1./cscPt-1./mdtPt)<ALPHA_TO_CSC_RATIO_PT ){
	trackPattern.pt = fabs(cscPt);
	//trackPattern.charge = cscPt/fabs(cscPt);//not need 
      }
    }
  }//use pt calculated from CSC-gamma
  
  
  bool pTCB = false;
  if( pTCB ){
    double Co_APt = 0.;
    double Co_BPt = 0.;
    
    double tmpalphaPt = (*m_ptEndcapLUT)->lookup(side, charge, PtEndcapLUT::ALPHAPOL2, trackPattern.etaBin, trackPattern.phiBin, trackPattern.endcapAlpha) / 1000;
    trackPattern.ptEndcapAlpha = tmpalphaPt;
    
    double tmpbetaPt = 0.;
    if(fabs(trackPattern.endcapBeta)>ZERO_LIMIT) tmpbetaPt = (*m_ptEndcapLUT)->lookup(side, charge, PtEndcapLUT::BETAPOL2, trackPattern.etaBin, trackPattern.phiBin, trackPattern.endcapBeta) / 1000;
    trackPattern.ptEndcapBeta = tmpbetaPt;
    
    double tmp_pt = (*m_ptEndcapLUT)->ptcombined(trackPattern.etaBin, trackPattern.phiBin, tmpalphaPt,tmpbetaPt, Co_APt, Co_BPt);
    
    float final_pt = 0.;
    if(fabs(Co_APt)>ZERO_LIMIT && fabs(Co_BPt)>ZERO_LIMIT && fabs(Co_BPt-Co_APt)/fabs(Co_APt) < 0.5){
      final_pt = tmp_pt;
    }else{
      final_pt = Co_APt;
    }
    if(final_pt == 0.)final_pt = tgcPt;
  }

  ATH_MSG_DEBUG("pT determined from alpha and beta: endcapAlpha/endcapBeta/endcapRadius3P/cscGamma/pT/charge/s_address="
		<< trackPattern.endcapAlpha << "/" << trackPattern.endcapBeta << "/" << trackPattern.endcapRadius3P << "/" << trackPattern.cscGamma << "/" 
		<< trackPattern.pt
		<< "/" << trackPattern.charge << "/" << trackPattern.s_address);
  ATH_MSG_DEBUG("ptEndcapAlpha/ptEndcapBeta/tgcPt/ptEndcapRadius/ptCSC="
		<< trackPattern.ptEndcapAlpha << "/" << trackPattern.ptEndcapBeta << "/" 
		<< tgcPt << "/" << trackPattern.ptEndcapRadius << "/" << trackPattern.ptCSC);
		
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

