/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/PtFromAlphaBeta.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_PtFromAlphaBeta("IID_PtFromAlphaBeta", 1, 0);

const InterfaceID& TrigL2MuonSA::PtFromAlphaBeta::interfaceID() { return IID_PtFromAlphaBeta; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtFromAlphaBeta::PtFromAlphaBeta(const std::string& type,
					       const std::string& name,
					       const IInterface*  parent):
  AthAlgTool(type, name, parent), 
  m_ptEndcapLUT(0)
{
  declareInterface<TrigL2MuonSA::PtFromAlphaBeta>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtFromAlphaBeta::~PtFromAlphaBeta() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtFromAlphaBeta::initialize()
{
  ATH_MSG_DEBUG("Initializing PtFromAlphaBeta - package version " << PACKAGE_VERSION) ;
   
  StatusCode sc;
  sc = AthAlgTool::initialize();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
    return sc;
  }

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

  return;
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
  
  float tgcPt = tgcFitResult.tgcPT ;
  
  // MDT pT by alpha
  int  side   = (trackPattern.etaMap <= 0.0) ? 0 : 1;
  int  charge = (trackPattern.intercept * trackPattern.etaMap) < 0.0 ? 0 : 1;

  float mdtPt = (*m_ptEndcapLUT)->lookup(side, charge, PtEndcapLUT::ALPHAPOL2, trackPattern.etaBin,
				      trackPattern.phiBin, trackPattern.endcapAlpha) / 1000;

  if (charge == 0)  mdtPt = -mdtPt;
  trackPattern.ptEndcapAlpha = mdtPt;//pt calculated by alpha
  //
  const float ALPHA_TO_BETA_PT    = 10;
  const float ALPHA_TO_BETA_RATIO = 0.5;
  
  // use MDT beta if condition allows
  if (fabs(mdtPt) > ALPHA_TO_BETA_PT && fabs(trackPattern.endcapBeta)>ZERO_LIMIT) {
    float betaPt = (*m_ptEndcapLUT)->lookup(side, charge, PtEndcapLUT::BETAPOL2, trackPattern.etaBin,
					 trackPattern.phiBin, trackPattern.endcapBeta) / 1000;

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
    ATH_MSG_DEBUG("calculate pt from Radius");
    float invR = 1. / trackPattern.endcapRadius3P;

    if (trackPattern.etaBin<8){
      trackPattern.ptEndcapRadius =  (*m_ptEndcapLUT)->lookup(side, charge, PtEndcapLUT::INVRADIUSPOL2, 
                                          trackPattern.etaBin, trackPattern.phiBinEE, invR) / 1000;
    }
  }

  if(mdtPt!=0.0) {
    trackPattern.pt     = fabs(mdtPt);
    trackPattern.charge = mdtPt / fabs(mdtPt);
  }

  if (trackPattern.ptEndcapRadius>0 && trackPattern.ptEndcapRadius<500)
      trackPattern.pt = trackPattern.ptEndcapRadius;//use pt calculated from endcap radius
  
  ATH_MSG_DEBUG("pT determined from alpha and beta: endcapAlpha/endcapBeta/endcapRadius3P/pT/charge/s_address="
		<< trackPattern.endcapAlpha << "/" << trackPattern.endcapBeta << "/" << trackPattern.endcapRadius3P << "/" << trackPattern.pt
		<< "/" << trackPattern.charge << "/" << trackPattern.s_address);
  ATH_MSG_DEBUG("ptEndcapAlpha/ptEndcapBeta/tgcPt/ptEndcapRadius="
		<< trackPattern.ptEndcapAlpha << "/" << trackPattern.ptEndcapBeta << "/" 
		<< tgcPt << "/" << trackPattern.ptEndcapRadius);
		
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtFromAlphaBeta::finalize()
{
  ATH_MSG_DEBUG("Finalizing PtFromAlphaBeta - package version " << PACKAGE_VERSION);
   
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
