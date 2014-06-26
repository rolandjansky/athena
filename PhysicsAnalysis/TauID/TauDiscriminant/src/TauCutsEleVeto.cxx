/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauDiscriminant/TauCutsEleVeto.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
// #include "tauEvent/TauCommonDetails.h"
// #include "tauEvent/TauPID.h"
// #include "tauEvent/TauJetParameters.h"
#include "xAODTau/TauDefs.h"
#include "TauDiscriminant/TauDetails.h"
#include <PathResolver/PathResolver.h>

using namespace xAOD;

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauCutsEleVeto::prepare(const TauDetailsManager& manager)
{
    this->detailsManager = &manager;
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauCutsEleVeto::finalize()
{
    return StatusCode::SUCCESS;
}
/* Cut-based electron veto implementation 
 *
 *
 * returns false if it a tau
 *         true  if it is an electron candidate
 * This function can also work on D3PD-level:
 * input D3PD variables: ******************************************************
 * eta       : tau_seedCalo_track_eta (track with highest tau_seedCalo_track_pt)
 * TrkAvgDist: tau_seedCalo_trkAvgDist
 * HoP       : tau_seedCalo_etHadAtEMScale/tau_leadTrkPt
 * EoP       : tau_seedCalo_etEMAtEMScale/tau_leadTrkPt
 * TrtRatio  : tau_calcVars_TRTHTOverLT_LeadTrk
 * ****************************************************************************
 * Usage:
 * tauElectronVeto(float eta, float TrkAvgDist, float HoP,
 *                 float EoP, float TrtRatio,   int qual)
 * qual: loose, medium, tight or tighter
 * returns true if the object is identified as an electron by the algorithm
 * ATLAS tauWG supported points: loose, medium and tight, tighter is for test
 * purposes
 *
 * Changes since the last release (12 May 2011)
 *
 * eta range changed: central taus were defined 0.0-1.7 now 0.0-2.0
 * variables replaced: 
 * OLD VARIABLE (ATHENA REL16)          -> NEW VARIABLE (ATHENA REL17)
 * tau_seedTrk_secMaxStripEt            -> tau_seedCalo_trkAvgDist
 * tau_seedTrk_hadLeakEt                -> tau_seedCalo_etHadAtEMScale/tau_leadTrkPt
 * tau_seedTrk_sumEMCellEtOverLeadTrkPt -> tau_seedCalo_etEMAtEMScale/tau_leadTrkPt
 * ****************************************************************************
 * nikolaos.rompotis at cern.ch - 6 Oct 2011
 * 
 */
bool tauElectronVeto(float eta, float TrkAvgDist, float HoP,
		     float EoP, float TrtRatio, int qual) {
  float TrkAvgDist_C0, HoP_C0, EoP_C0, TrtRatio_C0a, TrtRatio_C0b, TrtRatio_C0c;
  float HoP_C1, EoP_C1;

  // define the cut values .............
  if (qual == 0) {
    TrkAvgDist_C0= 0.026;
    HoP_C0       = 0.060;
    EoP_C0       = 0.868;
    TrtRatio_C0a = 0.227;
    TrtRatio_C0b = 0.108;
    TrtRatio_C0c = 0.151;

    HoP_C1       = 0.088;
    EoP_C1       = 0.102;
  }
  else if (qual == 1) {
    TrkAvgDist_C0= 0.051;
    HoP_C0       = 0.090;
    EoP_C0       = 0.812;
    TrtRatio_C0a = 0.162;
    TrtRatio_C0b = 0.069;
    TrtRatio_C0c = 0.097;
		   	    
    HoP_C1       = 0.195;
    EoP_C1       = 1.076;
  }
  else if (qual == 2) {
    TrkAvgDist_C0= 0.096;
    HoP_C0       = 0.035;
    EoP_C0       = 0.104;
    TrtRatio_C0a = 0.107;
    TrtRatio_C0b = 0.254;
    TrtRatio_C0c = 0.085;
		   	      
    HoP_C1       = 0.301;
    EoP_C1       = 2.550;

  }
  else if (qual == 3) {
    TrkAvgDist_C0= 0.096;
    HoP_C0       = 0.066;
    EoP_C0       = 0.103;
    TrtRatio_C0a = 0.098;
    TrtRatio_C0b = 0.708;
    TrtRatio_C0c = 0.056;
		   	 
    HoP_C1       = 0.560;
    EoP_C1       = 1.582;
  }
  else {
    std::cout << "No such option for qual=" << qual << endl;
    return false;
  }
  
  if (qual == 1) {
    if (tauElectronVeto(eta, TrkAvgDist, HoP, EoP, TrtRatio, 0))
      return true;
  }
  else if (qual == 2) {
    if (tauElectronVeto(eta, TrkAvgDist, HoP, EoP, TrtRatio, 1))
      return true;
  }
  else if (qual == 3) { // tighter is defined also wrt to the medium
    if (tauElectronVeto(eta, TrkAvgDist, HoP, EoP, TrtRatio, 2))
      return true;
  }
  
  // ...................................
  if ( fabs(eta) < 2.0 ) {  // central taus
    //
    if (HoP > HoP_C0) { 
      if ( TrtRatio <= TrtRatio_C0a) return false;
      else return true;
    }
    else {
      if (TrkAvgDist > TrkAvgDist_C0) {
	if (TrtRatio <= TrtRatio_C0b) return false;
	else return true;
      }
      else {
	if (TrtRatio <= TrtRatio_C0c && EoP <= EoP_C0) return false;
	else return true;
      }
    }
  }
  else {                     // forward taus
    if (HoP > HoP_C1) return false;
    else {
      if (EoP <= EoP_C1) return false;
      else return true;
    }
  }
}


//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauCutsEleVeto::execute(xAOD::TauJet* tau, FakeTauBits* /*bits*/, FakeTauScores* /*scores*/)
{
  
    if (!detailsManager)
    {
        return StatusCode::FAILURE;
    }
    bool loose(false), medium(false), tight(false);
    // demand exactly 1 track and calo or calo+track driven tau
    if (tau->nTracks() == 1)
      {
// 	const Analysis::TauCommonDetails* commonDetails(tau->details<Analysis::TauCommonDetails>());	
	float etHadAtEMScale;
	tau->detail(TauJetParameters::etHadAtEMScale,etHadAtEMScale);
	float etEMAtEMScale;
	tau->detail(TauJetParameters::etEMAtEMScale,etEMAtEMScale);
	float lcScale = 1;
	if(this->useLCscale) tau->detail(TauJetParameters::LC_TES_precalib,lcScale);

	float eta        = tau->track(0)->eta();//detailsManager->getFloatDetailValue(Details::ABS_ETA_LEAD_TRACK);
	float TrkAvgDist = detailsManager->getFloatDetailValue(Details::TRKAVGDIST);
	float leadTrkPt  = tau->track(0)->pt();
	float HoP        = leadTrkPt!=0?etHadAtEMScale*lcScale/leadTrkPt:0;
	float EoP        = leadTrkPt!=0?etEMAtEMScale*lcScale/leadTrkPt:0;
	//float HoP        = detailsManager->getFloatDetailValue(Details::ETHADATEMSCALE)/leadTrkPt;
	//float EoP        = detailsManager->getFloatDetailValue(Details::ETEMATEMSCALE)/leadTrkPt;
	float TrtRatio   = detailsManager->getFloatDetailValue(Details::TRT_NHT_OVER_NLT);
	
        loose = tauElectronVeto(eta,  TrkAvgDist, HoP, EoP, TrtRatio, 0);
        medium = tauElectronVeto(eta, TrkAvgDist, HoP, EoP, TrtRatio, 1);
        tight = tauElectronVeto(eta,  TrkAvgDist, HoP, EoP, TrtRatio, 2);
      }

//     Analysis::TauPID *p_tauid = tau->tauID();

    tau->setIsTau(TauJetParameters::ElectronVetoLoose, loose);
    tau->setIsTau(TauJetParameters::ElectronVetoMedium, medium);
    tau->setIsTau(TauJetParameters::ElectronVetoTight, tight);

    return StatusCode::SUCCESS;
}
