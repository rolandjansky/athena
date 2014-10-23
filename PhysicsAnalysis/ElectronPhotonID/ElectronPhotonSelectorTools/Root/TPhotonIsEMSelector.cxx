/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TPhotonIsEMSelector.cxx
PACKAGE:  PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools

AUTHORS:  J. Mitrevski
CREATED:  Feb. 2012
COMMENT:  evolved from egammaPhotonCutIDTool by (F. Derue)

PURPOSE:  Cut based identification of photons

UPDATED:

*********************************************************************/

// This class header
#include "ElectronPhotonSelectorTools/TPhotonIsEMSelector.h"
#include <cmath>

Root::TPhotonIsEMSelector::TPhotonIsEMSelector(const char* name) :
  TSelectorToolBase(name),
  isEMMask(egammaPID::PhotonTightAR),
  PIDName(egammaPID::IsEM),
  forceConvertedPhotonPID(false),
  forceNonConvertedPhotonPID(false),
  /** @brief cluster eta range */
  m_cutPositionClusterEtaRange_Photon(0),
  /** @brief cluster eta range */
  m_cutPositionClusterEtaRange_PhotonLoose(0),
  // selection for Loose photons
  /** @brief cluster leakage o the hadronic calorimeter */
  m_cutPositionClusterHadronicLeakage_PhotonLoose(0),
  /** @brief energy in 2nd sampling (e277) */
  m_cutPositionClusterMiddleEnergy_PhotonLoose(0),
  /** @brief energy ratio in 2nd sampling */
  m_cutPositionClusterMiddleEratio37_PhotonLoose(0),
  /** @brief energy ratio in 2nd sampling */
  m_cutPositionClusterMiddleEratio33_PhotonLoose(0),
  /** @brief width in the second sampling */
  m_cutPositionClusterMiddleWidth_PhotonLoose(0),
  /** @brief energy fraction in the third layer */
  m_cutPositionClusterBackEnergyFraction_Photon(0),
  // selection for tight photons
  /** @brief cluster leakage o the hadronic calorimeter */
  m_cutPositionClusterHadronicLeakage_Photon(0),
  /** @brief energy in 2nd sampling (e277) */
  m_cutPositionClusterMiddleEnergy_Photon(0),
  /** @brief energy ratio in 2nd sampling */
  m_cutPositionClusterMiddleEratio37_Photon(0),
  /** @brief energy ratio in 2nd sampling for photons */
  m_cutPositionClusterMiddleEratio33_Photon(0),
  /** @brief width in the second sampling */
  m_cutPositionClusterMiddleWidth_Photon(0),
  /** @brief fraction of energy found in 1st sampling */
  m_cutPositionClusterStripsEratio_Photon(0),
  /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) */
  m_cutPositionClusterStripsDeltaEmax2_Photon(0),
  /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) */
  m_cutPositionClusterStripsDeltaE_Photon(0),
  /** @brief shower width in 1st sampling */
  m_cutPositionClusterStripsWtot_Photon(0),
  /** @brief shower shape in shower core 1st sampling */
  m_cutPositionClusterStripsFracm_Photon(0),
  /** @brief shower width weighted by distance from the maximum one */
  m_cutPositionClusterStripsWeta1c_Photon(0),
  /** @brief difference between max and 2nd max in strips */
  m_cutPositionClusterStripsDEmaxs1_Photon(0),
  /** @brief energy-momentum match for photon selection*/
  m_cutPositionTrackMatchEoverP_Photon(0),
  /** @brief ambiguity resolution for photon (vs electron) */
  m_cutPositionAmbiguityResolution_Photon(0),
  /** @brief isolation */
  m_cutPositionIsolation_Photon(0),
  /** @brief calorimetric isolation for photon selection */
  m_cutPositionClusterIsolation_Photon(0),
  /** @brief tracker isolation for photon selection */
  m_cutPositionTrackIsolation_Photon(0),
  /** @brief cluster eta range */
  m_cutNameClusterEtaRange_Photon("ClusterEtaRange_Photon"),
  /** @brief cluster eta range */
  m_cutNameClusterEtaRange_PhotonLoose("ClusterEtaRange_PhotonLoose"),
  // selection for Loose photons
  /** @brief cluster leakage into the hadronic calorimeter */
  m_cutNameClusterHadronicLeakage_PhotonLoose("ClusterHadronicLeakage_PhotonLoose"),
  /** @brief energy in 2nd sampling (e277) */
  m_cutNameClusterMiddleEnergy_PhotonLoose("ClusterMiddleEnergy_PhotonLoose"),
  /** @brief energy ratio in 2nd sampling */
  m_cutNameClusterMiddleEratio37_PhotonLoose("ClusterMiddleEratio37_PhotonLoose"),
  /** @brief energy ratio in 2nd sampling */
  m_cutNameClusterMiddleEratio33_PhotonLoose("ClusterMiddleEratio33_PhotonLoose"),
  /** @brief width in the second sampling */
  m_cutNameClusterMiddleWidth_PhotonLoose("ClusterMiddleWidth_PhotonLoose"),
  
  /** @brief energy fraction in the third layer */
  m_cutNameClusterBackEnergyFraction_Photon("ClusterBackEnergyFraction_Photon"),
  
  // selection for tight photons
  /** @brief cluster leakage into the hadronic calorimeter */
  m_cutNameClusterHadronicLeakage_Photon("ClusterHadronicLeakage_Photon"),
  /** @brief energy in 2nd sampling (e277) */
  m_cutNameClusterMiddleEnergy_Photon("ClusterMiddleEnergy_Photon"),
  /** @brief energy ratio in 2nd sampling */
  m_cutNameClusterMiddleEratio37_Photon("ClusterMiddleEratio37_Photon"),
  /** @brief energy ratio in 2nd sampling for photons */
  m_cutNameClusterMiddleEratio33_Photon("ClusterMiddleEratio33_Photon"),
  /** @brief width in the second sampling */
  m_cutNameClusterMiddleWidth_Photon("ClusterMiddleWidth_Photon"),
  
  /** @brief fraction of energy found in 1st sampling */
  m_cutNameClusterStripsEratio_Photon("ClusterStripsEratio_Photon"),
  /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) */
  m_cutNameClusterStripsDeltaEmax2_Photon("ClusterStripsDeltaEmax2_Photon"),
  /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) */
  m_cutNameClusterStripsDeltaE_Photon("ClusterStripsDeltaE_Photon"),
  /** @brief shower width in 1st sampling */
  m_cutNameClusterStripsWtot_Photon("ClusterStripsWtot_Photon"),
  /** @brief shower shape in shower core 1st sampling */
  m_cutNameClusterStripsFracm_Photon("ClusterStripsFracm_Photon"),
  /** @brief shower width weighted by distance from the maximum one */
  m_cutNameClusterStripsWeta1c_Photon("ClusterStripsWeta1c_Photon"),
  /** @brief difference between max and 2nd max in strips */
  m_cutNameClusterStripsDEmaxs1_Photon("ClusterStripsDEmaxs1_Photon"),
  
  /** @brief energy-momentum match for photon selection*/
  m_cutNameTrackMatchEoverP_Photon("TrackMatchEoverP_Photon"),
  
  /** @brief ambiguity resolution for photon (vs electron) */
  m_cutNameAmbiguityResolution_Photon("AmbiguityResolution_Photon"),
  
  /** @brief isolation */
  m_cutNameIsolation_Photon("Isolation_Photon"),
  /** @brief calorimetric isolation for photon selection */
  m_cutNameClusterIsolation_Photon("ClusterIsolation_Photon"),
  /** @brief tracker isolation for photon selection */
  m_cutNameTrackIsolation_Photon("TrackIsolation_Photon")
{
}
// =================================================================
Root::TPhotonIsEMSelector::~TPhotonIsEMSelector() 
{
}

// =================================================================
FakeStatusCode Root::TPhotonIsEMSelector::initialize() 
{

  FakeStatusCode sc(FkStatusCode::SUCCESS);

  // --------------------------------------------------------------------------
  // Register the cuts and check that the registration worked:
  // NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit 1,...

  /** @brief cluster eta range, bit 0*/
  m_cutPositionClusterEtaRange_Photon = 
    m_accept.addCut(m_cutNameClusterEtaRange_Photon, "Photon within tight eta range");
  if (m_cutPositionClusterEtaRange_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief cluster eta range for loose photons, bit 1 */
  m_cutPositionClusterEtaRange_PhotonLoose = 
    m_accept.addCut(m_cutNameClusterEtaRange_PhotonLoose, "Photon within loose eta range");
  if (m_cutPositionClusterEtaRange_PhotonLoose < 0) sc = FkStatusCode::FAILURE;

  // selection for Loose photons
  /** @brief cluster leakage into the hadronic calorimeter, bit 2 */
  m_cutPositionClusterHadronicLeakage_PhotonLoose = 
    m_accept.addCut(m_cutNameClusterHadronicLeakage_PhotonLoose, "Had leakage < loose Cut");
  if (m_cutPositionClusterHadronicLeakage_PhotonLoose < 0) sc = FkStatusCode::FAILURE;
  
  /** @brief energy in 2nd sampling (e277), bit 3 */
  m_cutPositionClusterMiddleEnergy_PhotonLoose = 
    m_accept.addCut(m_cutNameClusterMiddleEnergy_PhotonLoose, "Energy in second sampling (E277) > loose Cut");
  if (m_cutPositionClusterMiddleEnergy_PhotonLoose < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy ratio in 2nd sampling, bit 4 */
  m_cutPositionClusterMiddleEratio37_PhotonLoose = 
    m_accept.addCut(m_cutNameClusterMiddleEratio37_PhotonLoose, "E237/377 > loose Cut");
  if (m_cutPositionClusterMiddleEratio37_PhotonLoose < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy ratio in 2nd sampling, bit 5 */
  m_cutPositionClusterMiddleEratio33_PhotonLoose = 
    m_accept.addCut(m_cutNameClusterMiddleEratio33_PhotonLoose, "E233/E237 > loose Cut");
  if (m_cutPositionClusterMiddleEratio33_PhotonLoose < 0) sc = FkStatusCode::FAILURE;

  /** @brief width in the second sampling, bit 6 */
  m_cutPositionClusterMiddleWidth_PhotonLoose = 
    m_accept.addCut(m_cutNameClusterMiddleWidth_PhotonLoose, "Weta2 < loose Cut");
  if (m_cutPositionClusterMiddleWidth_PhotonLoose < 0) sc = FkStatusCode::FAILURE;
  
  /** @brief energy fraction in the third layer (f3), bit 7 */
  m_cutPositionClusterBackEnergyFraction_Photon = 
    m_accept.addCut(m_cutNameClusterBackEnergyFraction_Photon, "f3 < Cut");
  if (m_cutPositionClusterBackEnergyFraction_Photon < 0) sc = FkStatusCode::FAILURE;
  
  int voidcutpos = m_accept.addCut("VOID1", "No Cut"); // bit 8 is not used
  if (voidcutpos < 0) sc = FkStatusCode::FAILURE;

  voidcutpos = m_accept.addCut("VOID2", "No Cut"); // bit 9 is not used
  if (voidcutpos < 0) sc = FkStatusCode::FAILURE;

  // selection for tight photons
  /** @brief cluster leakage into the hadronic calorimeter, bit 10 */
  m_cutPositionClusterHadronicLeakage_Photon = 
    m_accept.addCut(m_cutNameClusterHadronicLeakage_Photon, "Had leakage < Cut");
  if (m_cutPositionClusterHadronicLeakage_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy in 2nd sampling (e277), bit 11 */
  m_cutPositionClusterMiddleEnergy_Photon = 
    m_accept.addCut(m_cutNameClusterMiddleEnergy_Photon, "Energy in second sampling (E277) > Cut");
  if (m_cutPositionClusterMiddleEnergy_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy ratio in 2nd sampling, bit 12 */
  m_cutPositionClusterMiddleEratio37_Photon = 
    m_accept.addCut(m_cutNameClusterMiddleEratio37_Photon, "E237/377 > Cut");
  if (m_cutPositionClusterMiddleEratio37_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy ratio in 2nd sampling for photons, bit 13 */
  m_cutPositionClusterMiddleEratio33_Photon = 
    m_accept.addCut(m_cutNameClusterMiddleEratio33_Photon, "E233/E237 > Cut");
  if (m_cutPositionClusterMiddleEratio33_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief width in the second sampling, bit 14 */
  m_cutPositionClusterMiddleWidth_Photon = 
    m_accept.addCut(m_cutNameClusterMiddleWidth_Photon, "Weta2 < Cut");
  if (m_cutPositionClusterMiddleWidth_Photon < 0) sc = FkStatusCode::FAILURE;
  
  /** @brief fraction of energy found in 1st sampling, bit 15 */
  m_cutPositionClusterStripsEratio_Photon = 
    m_accept.addCut(m_cutNameClusterStripsEratio_Photon, "f1 > Cut");
  if (m_cutPositionClusterStripsEratio_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et), bit 16 */
  m_cutPositionClusterStripsDeltaEmax2_Photon = 
    m_accept.addCut(m_cutNameClusterStripsDeltaEmax2_Photon, "DeltaEmax2 < Cut");
  if (m_cutPositionClusterStripsDeltaEmax2_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1), bit 17 */
  m_cutPositionClusterStripsDeltaE_Photon = 
    m_accept.addCut(m_cutNameClusterStripsDeltaE_Photon, 
		    "difference between 2nd maximum and 1st minimum in strips < Cut");
  if (m_cutPositionClusterStripsDeltaE_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief shower width in 1st sampling, bit 18 */
  m_cutPositionClusterStripsWtot_Photon = 
    m_accept.addCut(m_cutNameClusterStripsWtot_Photon, "Wtot < Cut");
  if (m_cutPositionClusterStripsWtot_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief shower shape in shower core 1st sampling, bit 19 */
  m_cutPositionClusterStripsFracm_Photon = 
    m_accept.addCut(m_cutNameClusterStripsFracm_Photon, "Fracm < Cut");
  if (m_cutPositionClusterStripsFracm_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief shower width weighted by distance from the maximum one, bit 20 */
  m_cutPositionClusterStripsWeta1c_Photon = 
    m_accept.addCut(m_cutNameClusterStripsWeta1c_Photon, "Weta1c < Cut");
  if (m_cutPositionClusterStripsWeta1c_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief difference between max and 2nd max in strips, bit 21 */
  m_cutPositionClusterStripsDEmaxs1_Photon = 
    m_accept.addCut(m_cutNameClusterStripsDEmaxs1_Photon, 
		    "difference between max and 2nd max in strips > Cut");
  if (m_cutPositionClusterStripsDEmaxs1_Photon < 0) sc = FkStatusCode::FAILURE;
  
  /** @brief energy-momentum match for photon selection, bit 22 */
  m_cutPositionTrackMatchEoverP_Photon = 
    m_accept.addCut(m_cutNameTrackMatchEoverP_Photon, "E/p within allowed range (conversions only)");
  if (m_cutPositionTrackMatchEoverP_Photon < 0) sc = FkStatusCode::FAILURE;
  
  /** @brief ambiguity resolution for photon (vs electron), bit 23 */
  m_cutPositionAmbiguityResolution_Photon = 
    m_accept.addCut(m_cutNameAmbiguityResolution_Photon, 
		    "Passes tighter ambiguity resolution vs electron");
  if (m_cutPositionAmbiguityResolution_Photon < 0) sc = FkStatusCode::FAILURE;

  voidcutpos = m_accept.addCut("VOID3", "No Cut"); // bit 24 is not used
  if (voidcutpos < 0) sc = FkStatusCode::FAILURE;

  voidcutpos = m_accept.addCut("VOID4", "No Cut"); // bit 25 is not used
  if (voidcutpos < 0) sc = FkStatusCode::FAILURE;

  voidcutpos = m_accept.addCut("VOID5", "No Cut"); // bit 26 is not used
  if (voidcutpos < 0) sc = FkStatusCode::FAILURE;

  voidcutpos = m_accept.addCut("VOID6", "No Cut"); // bit 27 is not used
  if (voidcutpos < 0) sc = FkStatusCode::FAILURE;

  voidcutpos = m_accept.addCut("VOID7", "No Cut"); // bit 28 is not used
  if (voidcutpos < 0) sc = FkStatusCode::FAILURE;
  
  /** @brief isolation, bit 29 */
  m_cutPositionIsolation_Photon = 
    m_accept.addCut(m_cutNameIsolation_Photon, "Track and calorimetric isolation");
  if (m_cutPositionIsolation_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief calorimetric isolation for photon selection, bit 30 */
  m_cutPositionClusterIsolation_Photon = 
    m_accept.addCut(m_cutNameClusterIsolation_Photon, "calorimetric isolation only");
  if (m_cutPositionClusterIsolation_Photon < 0) sc = FkStatusCode::FAILURE;

  /** @brief tracker isolation for photon selection, bit 31 */
  m_cutPositionTrackIsolation_Photon = 
    m_accept.addCut(m_cutNameTrackIsolation_Photon, "track isolation only");
  if (m_cutPositionTrackIsolation_Photon < 0) sc = FkStatusCode::FAILURE;

  if (sc == FkStatusCode::FAILURE) {
    FAKE_MSG_ERROR("Exceeded the number of allowed cuts in TPhotonIsEMSelector");
  }

  return sc;
}

const Root::TAccept& Root::TPhotonIsEMSelector::fillAccept()
{ 
  for (int i = 0; i < 32; i++) {
    const unsigned int mask = (0x1 << i) & isEMMask;
    m_accept.setCutResult(i, (m_isEM & mask) == 0);
  }
  
  return m_accept;
}

//=============================================================================
// Calculate the actual accept of each cut individually.
//=============================================================================
const Root::TAccept& Root::TPhotonIsEMSelector::accept( 
						       // eta position in second sampling
						       float eta2,
						       // transverse energy in calorimeter (using eta position in second sampling)
						       double et,
						       // transverse energy in 1st scintillator of hadronic calorimeter
						       float ethad1,
						       // transverse energy in hadronic calorimeter
						       float ethad,
						       // E(3*3) in 2nd sampling
						       float e233,
						       // E(3*7) in 2nd sampling
						       float e237,
						       // E(7*7) in 2nd sampling
						       float e277,
						       // shower width in 2nd sampling
						       float weta2c,
						       //  E of 2nd max between max and min in strips
						       float emax2,
						       // E of 1st max in strips
						       float emax,
						       // E(min) in strips
						       float emin,
						       // fraction of energy reconstructed in strips
						       float f1,
						       // shower width in 3 strips in 1st sampling
						       float weta1c,
						       // total shower width in strips
						       float wtot,
						       // E(+/-3)-E(+/-1)/E(+/-1)
						       float fracm,
						       // fraction of energy reconstructed in the 3rd sampling
						       float f3,
						       // E/p
						       double ep,
						       // is it a conversion
						       bool isConversion,
						       // The ambiguity result 
						       EMAmbiguityType::AmbiguityResult ambiguityResult)
{
  // Reset the cut result bits to zero (= fail cut)
  m_accept.clear();

  // -----------------------------------------------------------
  // Do the actual selection

  m_isEM = calcIsEm(eta2,
		    et,
		    ethad1,
		    ethad,
		    e233,
		    e237,
		    e277,
		    weta2c,
		    emax2,
		    emax,
		    emin,
		    f1,
		    weta1c,
		    wtot,
		    fracm,
		    f3,
		    ep,
		    isConversion,
		    ambiguityResult);

  return fillAccept();
}


//=============================================================================
// calculate the isEM. (Used internally by accept)
//=============================================================================
unsigned int Root::TPhotonIsEMSelector::calcIsEm(
						 // eta position in second sampling
						 float eta2,
						 // transverse energy in calorimeter (using eta position in second sampling)
						 double et,
						 // transverse energy in 1st scintillator of hadronic calorimeter
						 float ethad1,
						 // transverse energy in hadronic calorimeter
						 float ethad,
						 // E(3*3) in 2nd sampling
						 float e233,
						 // E(3*7) in 2nd sampling
						 float e237,
						 // E(7*7) in 2nd sampling
						 float e277,
						 // shower width in 2nd sampling
						 float weta2c,
						 //  E of 2nd max between max and min in strips
						 float emax2,
						 // E of 1st max in strips
						 float emax,
						 // E(min) in strips
						 float emin,
						 // fraction of energy reconstructed in strips
						 float f1,
						 // shower width in 3 strips in 1st sampling
						 float weta1c,
						 // total shower width in strips
						 float wtot,
						 // E(+/-3)-E(+/-1)/E(+/-1)
						 float fracm,
						 // fraction of energy reconstructed in the 3rd sampling
						 float f3,
						 // E/p
						 double ep,
						 // is it a conversion
						 bool isConversion,
						 // The ambiguity result 
						 EMAmbiguityType::AmbiguityResult ambiguityResult) const
{
  // calculated variables
  // hadronic leakage variables
  const double raphad1 = fabs(et)>0. ? ethad1/et : 0.;
  const double raphad  = fabs(et)>0. ? ethad/et : 0.;
  // ratios
  const double Reta37 = fabs(e277)>0. ? e237/e277 : 0.;
  const double Rphi33 = fabs(e237)>0. ? e233/e237 : 0.;
  // (Emax1-Emax2)/(Emax1+Emax2)
  const double demaxs1 = fabs(emax+emax2)>0. ? (emax-emax2)/(emax+emax2) : 0.;
  // difference of energy between max and min
  const double deltae = emax2-emin;
  // parametrization of E(2nd max)
  const double deltaemax2 = emax2/(1000.+0.009*et);

  // apply calorimeter selection for Loose photons
  unsigned int iflag = calocuts_photonsLoose(eta2,
					     et,
					     raphad1,
					     raphad,
					     e277,
					     Reta37,
					     Rphi33,
					     weta2c,
					     0);
  
  // apply calorimeter selection for Tight photons
  if (forceConvertedPhotonPID) {
    // force to test converted photon hypothesis
    iflag = calocuts_photonsTightConverted(eta2,
					   et,
					   raphad1,
					   raphad,
					   e277,
					   Reta37,
					   Rphi33,
					   weta2c,
					   demaxs1,
					   deltae,
					   deltaemax2,
					   f1,
					   weta1c,
					   wtot,
					   fracm,
					   f3,
					   ep,
					   iflag);

  } else if (forceNonConvertedPhotonPID || !isConversion) {
    iflag = calocuts_photonsTightNonConverted(eta2,
					      et,
					      raphad1,
					      raphad,
					      e277,
					      Reta37,
					      Rphi33,
					      weta2c,
					      demaxs1,
					      deltae,
					      deltaemax2,
					      f1,
					      weta1c,
					      wtot,
					      fracm,
					      f3,
					      iflag);
  } else {
    iflag = calocuts_photonsTightConverted(eta2,
					   et,
					   raphad1,
					   raphad,
					   e277,
					   Reta37,
					   Rphi33,
					   weta2c,
					   demaxs1,
					   deltae,
					   deltaemax2,
					   f1,
					   weta1c,
					   wtot,
					   fracm,
					   f3,
					   ep,
					   iflag);
  }

  iflag = ambiguitycuts_photons(ambiguityResult, iflag);

  return iflag;
}


// ======================================================================
unsigned int Root::TPhotonIsEMSelector::calocuts_photonsLoose(
							      // eta position in second sampling
							      float eta2,
							      // transverse energy in calorimeter (using eta position in second sampling)
							      double et,
							      // hadronic leakage ratios
							      float raphad1,
							      float raphad,
							      // E(7*7) in 2nd sampling
							      float e277,
							      // ratios
							      float Reta37,
							      float Rphi33,
							      // shower width in 2nd sampling
							      float weta2c,
							      unsigned int iflag) const
{
  // 
  // apply selection for Loose photons
  //

  int ibine = 0;
  // loop on ET range
  for (unsigned int ibe=1;ibe<=CutBinEnergy_photonsLoose.size();ibe++) {
    if ( ibe <CutBinEnergy_photonsLoose.size() ) {
      if ( et >= CutBinEnergy_photonsLoose[ibe-1] && 
	   et < CutBinEnergy_photonsLoose[ibe] ) {
	ibine = ibe;
      }
    }
    else if ( ibe == CutBinEnergy_photonsLoose.size() ) {
      if ( et >= CutBinEnergy_photonsLoose[ibe-1] ) {
	ibine = ibe;
      }
    }
  }

      
  int ibinEta = -1;
  // loop on eta range
  for (unsigned int ibin=0;ibin<CutBinEta_photonsLoose.size();ibin++) {
    if ( ibin == 0 ){
      if ( eta2 < CutBinEta_photonsLoose[0] ) {
	ibinEta = 0;
      }
    }
    else {
      if ( eta2 >= CutBinEta_photonsLoose[ibin-1] && 
	   eta2 < CutBinEta_photonsLoose[ibin] ) {
	ibinEta = ibin;
      }
    }
  }

  // check the bin number
  const int ibin_combined =  ibine*CutBinEta_photonsLoose.size()+ibinEta;

  // second sampling cuts  
  if (e277 >= e277_photonsLoose[0] ) {
    if (ibinEta==-1) {
      //std::cout << " pb with eta range = " << eta2 << std::endl;
      iflag |= ( 0x1 << egammaPID::ClusterEtaRange_PhotonLoose);
      return iflag;
    }
    
    // hadronic leakage
    if (CheckVar(CutHadLeakage_photonsLoose,3)) {
      if (eta2 < 0.8) {
	if (raphad1>CutHadLeakage_photonsLoose[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_PhotonLoose);
      } else if (eta2 >= 0.8 && eta2 < 1.37) {
	if (raphad>CutHadLeakage_photonsLoose[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_PhotonLoose);
      } else {
	if (raphad1>CutHadLeakage_photonsLoose[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_PhotonLoose);
      }
    }
    
    // E237/E277
    if (CheckVar(Reta37_photonsLoose,3)) {
      if (Reta37 < Reta37_photonsLoose[ibin_combined] ) {
	iflag |= ( 0x1 << egammaPID::ClusterMiddleEratio37_PhotonLoose);
      }
    }

    // E233/E237
    if (CheckVar(Rphi33_photonsLoose,3)) {
      if (Rphi33 < Rphi33_photonsLoose[ibin_combined] ) {
	iflag |= ( 0x1 << egammaPID::ClusterMiddleEratio33_PhotonLoose);
      }
    }

    // width in 2nd sampling
    if (CheckVar(weta2_photonsLoose,3)) {
      if (weta2c > weta2_photonsLoose[ibin_combined]  ) {
	iflag |= ( 0x1 << egammaPID::ClusterMiddleWidth_PhotonLoose);
      }
    }
  }
  else {
    iflag |= ( 0x1 << egammaPID::ClusterMiddleEnergy_PhotonLoose);
  }

  return iflag;
}

// ======================================================================
unsigned int Root::TPhotonIsEMSelector::calocuts_photonsTightNonConverted(
									  // eta position in second sampling
									  float eta2,
									  // transverse energy in calorimeter 
									  double et,
									  // hadronic leakage ratios
									  float raphad1,
									  float raphad,
									  // E(7*7) in 2nd sampling
									  float e277,
									  // ratios
									  float Reta37,
									  float Rphi33,
									  // shower width in 2nd sampling
									  float weta2c,
									  // (Emax1-Emax2)/(Emax1+Emax2)
									  float demaxs1,
									  // difference of energy between max and min
									  float deltae,
									  // parametrization of E(2nd max)
									  float deltaemax2,
									  // fraction of energy reconstructed in strips
									  float f1,
									  // shower width in 3 strips in 1st sampling
									  float weta1c,
									  // total shower width in strips
									  float wtot,
									  // E(+/-3)-E(+/-1)/E(+/-1)
									  float fracm,
									  // fraction of energy reconstructed in the 3rd sampling
									  float f3,
									  unsigned int iflag) const
{
  // 
  // apply selection for Tight photons, non converted
  //

  int ibine = 0;
  // loop on ET range
  for (unsigned int ibe=1;ibe<=CutBinEnergy_photonsNonConverted.size();ibe++) {
    if ( ibe <CutBinEnergy_photonsNonConverted.size() ) {
      if ( et >= CutBinEnergy_photonsNonConverted[ibe-1] && 
	   et < CutBinEnergy_photonsNonConverted[ibe] ) {
	ibine = ibe;
      }
    }
    else if ( ibe == CutBinEnergy_photonsNonConverted.size() ) {
      if ( et >= CutBinEnergy_photonsNonConverted[ibe-1] ) {
	ibine = ibe;
      }
    }
  }
      
  int ibinEta = -1;
  // loop on eta range
  for (unsigned int ibin=0;ibin<CutBinEta_photonsNonConverted.size();ibin++) {
    if ( ibin == 0 ){
      if ( eta2 < CutBinEta_photonsNonConverted[0] ) {
	ibinEta = 0;
      }
    }
    else {
      if ( eta2 >= CutBinEta_photonsNonConverted[ibin-1] && 
	   eta2 < CutBinEta_photonsNonConverted[ibin] ) {
	ibinEta = ibin;
      }
    }
  }

  // check the bin number
  const int ibin_combined =  ibine*CutBinEta_photonsNonConverted.size()+ibinEta;
  
  //
  // second sampling cuts  
  //
  if (e277 >= e277_photonsNonConverted[0] ) {
    if (ibinEta==-1) {
      //std::cout << " pb with eta range = " << eta2 << std::endl;
      iflag |= ( 0x1 << egammaPID::ClusterEtaRange_Photon);
      return iflag;
    }

    // hadronic leakage
    if (CheckVar(CutHadLeakage_photonsNonConverted,23)) {
      if (eta2 < 0.8) {
	if (raphad1>CutHadLeakage_photonsNonConverted[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      } else if (eta2 >= 0.8 && eta2 < 1.37) {
	if (raphad>CutHadLeakage_photonsNonConverted[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      } else {
	if (raphad1>CutHadLeakage_photonsNonConverted[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      }
    }

    // F3
    if (CheckVar(CutF3_photonsNonConverted,23)) {
      if (f3 > CutF3_photonsNonConverted[ibin_combined] ) {
	iflag |= ( 0x1 << egammaPID::ClusterBackEnergyFraction_Photon);
      }
    }
    
    // E237/E277
    if (CheckVar(Reta37_photonsNonConverted,23)) {
      if (Reta37 < Reta37_photonsNonConverted[ibin_combined] ) {
	iflag |= ( 0x1 << egammaPID::ClusterMiddleEratio37_Photon);
      }
    }

    // E233/E237
    if (CheckVar(Rphi33_photonsNonConverted,23)) {
      if (Rphi33 < Rphi33_photonsNonConverted[ibin_combined] ) {
	iflag |= ( 0x1 << egammaPID::ClusterMiddleEratio33_Photon);
      }
    }
    
    // width in 2n sampling
    if (CheckVar(weta2_photonsNonConverted,23)) {
      if (weta2c > weta2_photonsNonConverted[ibin_combined]  ) {
	iflag |= ( 0x1 << egammaPID::ClusterMiddleWidth_Photon);
      }
    }
  }
  else {
    iflag |= ( 0x1 << egammaPID::ClusterMiddleEnergy_Photon);
  }

  //
  // first sampling cuts  
  //

  int ibineStrips = 0;
  // loop on ET range
  for (unsigned int ibe=1;ibe<=CutBinEnergyStrips_photonsNonConverted.size();ibe++) {
    if ( ibe <CutBinEnergyStrips_photonsNonConverted.size() ) {
      if ( et >= CutBinEnergyStrips_photonsNonConverted[ibe-1] && 
	   et < CutBinEnergyStrips_photonsNonConverted[ibe] ) {
	ibineStrips = ibe;
      }
    }
    else if ( ibe == CutBinEnergyStrips_photonsNonConverted.size() ) {
      if ( et >= CutBinEnergyStrips_photonsNonConverted[ibe-1] ) {
	ibineStrips = ibe;
      }
    }
  }
      
  int ibinEtaStrips = -1;
  // loop on eta range
  for (unsigned int ibin=0;ibin<CutBinEtaStrips_photonsNonConverted.size();ibin++) {
    if ( ibin == 0 ){
      if ( eta2 < CutBinEtaStrips_photonsNonConverted[0] ) {
	ibinEtaStrips = 0;
      }
    }
    else {
      if ( eta2 >= CutBinEtaStrips_photonsNonConverted[ibin-1] && 
	   eta2 < CutBinEtaStrips_photonsNonConverted[ibin] ) {
	ibinEtaStrips = ibin;
      }
    }
  }

  // check the bin number
  const int ibin_combinedStrips =  ibineStrips*CutBinEtaStrips_photonsNonConverted.size()+ibinEtaStrips;
  
  if (f1<f1_photonsNonConverted[0] ) {
    iflag |= ( 0x1 << egammaPID::ClusterStripsEratio_Photon);
  }  
  if (ibinEtaStrips==-1) {
    iflag |= ( 0x1 << egammaPID::ClusterEtaRange_Photon);
    return iflag;
  }
  
  // Rmax2
  if (CheckVar(emax2r_photonsNonConverted,26)) {
    if (deltaemax2 > emax2r_photonsNonConverted[ibin_combinedStrips] ) {
      iflag |= ( 0x1 << egammaPID::ClusterStripsDeltaEmax2_Photon);
    }
  }

  // Delta E
  if (CheckVar(deltae_photonsNonConverted,26)) {
    if (deltae > deltae_photonsNonConverted[ibin_combinedStrips]) {
      iflag |= ( 0x1 << egammaPID::ClusterStripsDeltaE_Photon);
    }
  }

  // Demaxs1
  if (CheckVar(deltae_photonsNonConverted,26)) {
    if (demaxs1<=DEmaxs1_photonsNonConverted[ibin_combinedStrips])
      iflag |= ( 0x1 << egammaPID::ClusterStripsDEmaxs1_Photon);    
  }

  // total width in strips
  if (CheckVar(wtot_photonsNonConverted,26)) {
    if (wtot > wtot_photonsNonConverted[ibin_combinedStrips]   ) {
      iflag |= ( 0x1 << egammaPID::ClusterStripsWtot_Photon);
    }
  }

  // (E(+/-3)-E(+/-1))/E(+/-1)
  if (CheckVar(fracm_photonsNonConverted,26)) {
    if (fracm > fracm_photonsNonConverted[ibin_combinedStrips] ) {
      iflag |= ( 0x1 << egammaPID::ClusterStripsFracm_Photon);
    }
  }

  // width in 3 strips
  if (CheckVar(w1_photonsNonConverted,26)) {
    if (weta1c > w1_photonsNonConverted[ibin_combinedStrips]) {
      iflag |= ( 0x1 << egammaPID::ClusterStripsWeta1c_Photon);
    }
  }
  
  return iflag;
}

// ======================================================================
// ======================================================================
unsigned int Root::TPhotonIsEMSelector::calocuts_photonsTightConverted(
								       // eta position in second sampling
								       float eta2,
								       // transverse energy in calorimeter 
								       double et,
								       // hadronic leakage ratios
								       float raphad1,
								       float raphad,
								       // E(7*7) in 2nd sampling
								       float e277,
								       // ratios
								       float Reta37,
								       float Rphi33,
								       // shower width in 2nd sampling
								       float weta2c,
								       // (Emax1-Emax2)/(Emax1+Emax2)
								       float demaxs1,
								       // difference of energy between max and min
								       float deltae,
								       // parametrization of E(2nd max)
								       float deltaemax2,
								       // fraction of energy reconstructed in strips
								       float f1,
								       // shower width in 3 strips in 1st sampling
								       float weta1c,
								       // total shower width in strips
								       float wtot,
								       // E(+/-3)-E(+/-1)/E(+/-1)
								       float fracm,
								       // fraction of energy reconstructed in the 3rd sampling
								       float f3,
								       // E/p
								       double ep,
								       unsigned int iflag) const
// 
// apply selection for Tight photons, converted
//
{
  int ibine = 0;
  // loop on ET range
  for (unsigned int ibe=1;ibe<=CutBinEnergy_photonsConverted.size();ibe++) {
    if ( ibe <CutBinEnergy_photonsConverted.size() ) {
      if ( et >= CutBinEnergy_photonsConverted[ibe-1] && 
	   et < CutBinEnergy_photonsConverted[ibe] ) {
	ibine = ibe;
      }
    }
    else if ( ibe == CutBinEnergy_photonsConverted.size() ) {
      if ( et >= CutBinEnergy_photonsConverted[ibe-1] ) {
	ibine = ibe;
      }
    }
  }
      
  int ibinEta = -1;
  // loop on eta range
  for (unsigned int ibin=0;ibin<CutBinEta_photonsConverted.size();ibin++) {
    if ( ibin == 0 ){
      if ( eta2 < CutBinEta_photonsConverted[0] ) {
	ibinEta = 0;
      }
    }
    else {
      if ( eta2 >= CutBinEta_photonsConverted[ibin-1] && 
	   eta2 < CutBinEta_photonsConverted[ibin] ) {
	ibinEta = ibin;
      }
    }
  }

  // check the bin number
  const int ibin_combined =  ibine*CutBinEta_photonsConverted.size()+ibinEta;
  
  //
  // second sampling cuts  
  //
  if (e277 >= e277_photonsConverted[0] ) {

    if (ibinEta==-1) {
      iflag |= ( 0x1 << egammaPID::ClusterEtaRange_Photon);
      return iflag;
    }

    // hadronic leakage
    if (CheckVar(CutHadLeakage_photonsConverted,13)) {
      if (eta2 < 0.8) {
	if (raphad1>CutHadLeakage_photonsConverted[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      } else if (eta2 >= 0.8 && eta2 < 1.37) {
	if (raphad>CutHadLeakage_photonsConverted[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      } else {
	if (raphad1>CutHadLeakage_photonsConverted[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      }
    }

    // F3
    if (CheckVar(CutF3_photonsConverted,13)) {
      if (f3 > CutF3_photonsConverted[ibin_combined] ) {
	iflag |= ( 0x1 << egammaPID::ClusterBackEnergyFraction_Photon);
      }
    }

    // E237/E277
    if (CheckVar(Reta37_photonsConverted,13)) {
      if (Reta37 < Reta37_photonsConverted[ibin_combined] ) {
	iflag |= ( 0x1 << egammaPID::ClusterMiddleEratio37_Photon);
      }
    }

    // E233/E237
    if (CheckVar(Rphi33_photonsConverted,13)) {
      if (Rphi33 < Rphi33_photonsConverted[ibin_combined] ) {
	iflag |= ( 0x1 << egammaPID::ClusterMiddleEratio33_Photon);
      }
    }
    
    // width in 2n sampling
    if (CheckVar(weta2_photonsConverted,13)) {
      if (weta2c > weta2_photonsConverted[ibin_combined]  ) {
	iflag |= ( 0x1 << egammaPID::ClusterMiddleWidth_Photon);
      }
    }
  }
  else {
    iflag |= ( 0x1 << egammaPID::ClusterMiddleEnergy_Photon);
  }

  //
  // first sampling cuts  
  //
  int ibineStrips = 0;
  // loop on ET range
  for (unsigned int ibe=1;ibe<=CutBinEnergyStrips_photonsConverted.size();ibe++) {
    if ( ibe <CutBinEnergyStrips_photonsConverted.size() ) {
      if ( et >= CutBinEnergyStrips_photonsConverted[ibe-1] && 
	   et < CutBinEnergyStrips_photonsConverted[ibe] ) {
	ibineStrips = ibe;
      }
    }
    else if ( ibe == CutBinEnergyStrips_photonsConverted.size() ) {
      if ( et >= CutBinEnergyStrips_photonsConverted[ibe-1] ) {
	ibineStrips = ibe;
      }
    }
  }
      
  int ibinEtaStrips = -1;
  // loop on eta range
  for (unsigned int ibin=0;ibin<CutBinEtaStrips_photonsConverted.size();ibin++) {
    if ( ibin == 0 ){
      if ( eta2 < CutBinEtaStrips_photonsConverted[0] ) {
	ibinEtaStrips = 0;
      }
    }
    else {
      if ( eta2 >= CutBinEtaStrips_photonsConverted[ibin-1] && 
	   eta2 < CutBinEtaStrips_photonsConverted[ibin] ) {
	ibinEtaStrips = ibin;
      }
    }
  }

  // check the bin number
  const int ibin_combinedStrips =  ibineStrips*CutBinEtaStrips_photonsConverted.size()+ibinEtaStrips;

  if (f1<f1_photonsConverted[0] ) {
    iflag |= ( 0x1 << egammaPID::ClusterStripsEratio_Photon);
  }  
  if (ibinEtaStrips==-1) {
    iflag |= ( 0x1 << egammaPID::ClusterEtaRange_Photon);
    return iflag;
  }
  
  // Rmax2
  if (CheckVar(emax2r_photonsConverted,16)) {
    if (deltaemax2 > emax2r_photonsConverted[ibin_combinedStrips] ) {
      iflag |= ( 0x1 << egammaPID::ClusterStripsDeltaEmax2_Photon);
    }
  }

  // Delta E
  if (CheckVar(deltae_photonsConverted,16)) {
    if (deltae > deltae_photonsConverted[ibin_combinedStrips]) {
      iflag |= ( 0x1 << egammaPID::ClusterStripsDeltaE_Photon);
    }
  }

  // Demaxs1
  if (CheckVar(DEmaxs1_photonsConverted,16)) {
    if (demaxs1<=DEmaxs1_photonsConverted[ibin_combinedStrips])
      iflag |= ( 0x1 << egammaPID::ClusterStripsDEmaxs1_Photon);    
  }

  // total width in strips
  if (CheckVar(wtot_photonsConverted,16)) {
    if (wtot > wtot_photonsConverted[ibin_combinedStrips]   ) {
      iflag |= ( 0x1 << egammaPID::ClusterStripsWtot_Photon);
    }
  }

  // (E(+/-3)-E(+/-1))/E(+/-1)
  if (CheckVar(fracm_photonsConverted,16)) {
    if (fracm > fracm_photonsConverted[ibin_combinedStrips] ) {
      iflag |= ( 0x1 << egammaPID::ClusterStripsFracm_Photon);
    }
  }

  // width in 3 strips
  if (CheckVar(w1_photonsConverted,16)) {
    if (weta1c > w1_photonsConverted[ibin_combinedStrips]) {
      iflag |= ( 0x1 << egammaPID::ClusterStripsWeta1c_Photon);
    }
  }

  // cut on E/p
  //
  if (CheckVar(CutminEp_photonsConverted,13) && 
      CheckVar(CutmaxEp_photonsConverted,13)) {
    if ( ep < CutminEp_photonsConverted[ibin_combined] || 
	 ep > CutmaxEp_photonsConverted[ibin_combined] ) 
      iflag |= ( 0x1 << egammaPID::TrackMatchEoverP_Photon); 
  }

  return iflag;
}

// ==============================================================
unsigned int Root::TPhotonIsEMSelector::ambiguitycuts_photons(EMAmbiguityType::AmbiguityResult ambiguityResult, 
							      unsigned int iflag) const 
{

  if (ambiguityResult > EMAmbiguityType::LOOSE) {    
    // set bit to 0 (passed)
    iflag &= ~(0x1 << egammaPID::AmbiguityResolution_Photon); 
  } else {
    // set bit to 1 (not passed)
    iflag |= (0x1 << egammaPID::AmbiguityResolution_Photon); 
  }
  return iflag;
}

// ==============================================================
bool Root::TPhotonIsEMSelector::CheckVar(std::vector<float> vec, int choice) const
{
  //
  // check vector size
  // 0 : size should be 1
  // 1 : vs etaNB_photonsLoose
  // 2 : vs etNB_photonsLoose
  // 3 : vs combinedNB_photonsLoose
  //
  // 11 : vs etaNB_photonsTightConv
  // 12 : vs etNB_photonsTightConv
  // 13 : vs combinedNB_photonsTightConv
  // 14 : vs etaStripsNB_photonsTightConv
  // 15 : vs etStripsNB_photonsTightConv
  // 16 : combinedStripsNB_photonsTightConv
  // 
  // 21 : vs etaNB_photonsTightNonConv
  // 22 : vs etNB_photonsTightNonConv
  // 23 : vs combinedNB_photonsTightNonConv
  // 24 : vs etaStripsNB_photonsTightNonConv
  // 25 : vs etStripsNB_photonsTightNonConv
  // 26 : combinedStripsNB_photonsTightNonConv
  //

  // selection of Loose objects
  unsigned int etaNB_photonsLoose = 
    CutBinEta_photonsLoose.size();
  unsigned int etNB_photonsLoose  = 
    CutBinEnergy_photonsLoose.size();
  unsigned int combinedNB_photonsLoose = 0;
  if (etNB_photonsLoose>0) 
    combinedNB_photonsLoose = etaNB_photonsLoose * (etNB_photonsLoose+1);
  else 
    combinedNB_photonsLoose = etaNB_photonsLoose;

  //std::cout << " etaNB_photonsLoose = " << etaNB_photonsLoose
  //    << " " << etNB_photonsLoose
  //    << " " << combinedNB_photonsLoose
  //    << std::endl;

  // selection of Tight non converted objects
  unsigned int etaNB_photonsTightNonConv = 
    CutBinEta_photonsNonConverted.size();
  unsigned int etNB_photonsTightNonConv  = 
    CutBinEnergy_photonsNonConverted.size();
  unsigned int combinedNB_photonsTightNonConv = 0;
  if (etNB_photonsTightNonConv>0) 
    combinedNB_photonsTightNonConv = 
      etaNB_photonsTightNonConv * (etNB_photonsTightNonConv+1);
  else
    combinedNB_photonsTightNonConv = etaNB_photonsTightNonConv;

  //std::cout << " etaNB_photonsTightNonConv = " << etaNB_photonsTightNonConv
  //    << " " << etNB_photonsTightNonConv
  //    << " " << combinedNB_photonsTightNonConv
  //    << std::endl;
  
  unsigned int etaStripsNB_photonsTightNonConv = 
    CutBinEtaStrips_photonsNonConverted.size();
  unsigned int etStripsNB_photonsTightNonConv = 
    CutBinEnergyStrips_photonsNonConverted.size();
  unsigned int combinedStripsNB_photonsTightNonConv = 0;
  if (etStripsNB_photonsTightNonConv>0)
    combinedStripsNB_photonsTightNonConv = 
      etaStripsNB_photonsTightNonConv * (etStripsNB_photonsTightNonConv+1);
  else 
    combinedStripsNB_photonsTightNonConv = 
      etaStripsNB_photonsTightNonConv;

  //std::cout << " etaStripsNB_photonsTightNonConv = " << etaStripsNB_photonsTightNonConv
  //    << " " << etStripsNB_photonsTightNonConv
  //    << " " << combinedStripsNB_photonsTightNonConv
  //    << std::endl;
  
  // selection of Tight converted objects
  unsigned int etaNB_photonsTightConv = 
    CutBinEta_photonsConverted.size();
  unsigned int etNB_photonsTightConv  = 
    CutBinEnergy_photonsConverted.size();
  unsigned int combinedNB_photonsTightConv = 0;
  if (etNB_photonsTightConv>0) 
    combinedNB_photonsTightConv = 
      etaNB_photonsTightConv * (etNB_photonsTightConv+1);
  else 
    combinedNB_photonsTightConv = 
      etaNB_photonsTightConv;

  //std::cout << " etaNB_photonsTightConv = " << etaNB_photonsTightConv
  //    << " " << etNB_photonsTightConv
  //    << " " << combinedNB_photonsTightConv
  //    << std::endl;

  unsigned int etaStripsNB_photonsTightConv = 
    CutBinEtaStrips_photonsConverted.size();
  unsigned int etStripsNB_photonsTightConv = 
    CutBinEnergyStrips_photonsConverted.size();
  unsigned int combinedStripsNB_photonsTightConv = 0;
  if (etStripsNB_photonsTightConv>0)
    combinedStripsNB_photonsTightConv =
      etaStripsNB_photonsTightConv * (etStripsNB_photonsTightConv+1);
  else
    combinedStripsNB_photonsTightConv =
      etaStripsNB_photonsTightConv;

  //std::cout << " etaStripsNB_photonsTightConv = " << etaStripsNB_photonsTightConv
  //    << " " << etStripsNB_photonsTightConv
  //    << " " << combinedStripsNB_photonsTightConv
  //    << std::endl;

  // if size of vector is 0 it means cut is not defined
  if (vec.size() == 0) return false;

  // check if size is 1
  if (choice==0) {
    if ( vec.size() != 1) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs 1"); 
      return false;      
    }
  }

  // check if size is etaNB_photonsLoose
  if (choice==1) {
    if ( vec.size() != etaNB_photonsLoose ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs etaNB_photonsLoose=" 
		     << etaNB_photonsLoose);
      return false;      
    }
  }

  // check if size is etNB_photonsLoose
  if (choice==2) {
    if ( vec.size() != etNB_photonsLoose ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs etNB_photonsLoose=" 
		     << etNB_photonsLoose);
      return false;      
    }
  }

  // check if size is combinedNB_photonsLoose
  if (choice==3) {
    if ( vec.size() != combinedNB_photonsLoose ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs combinedNB_photonsLoose=" 
		     << combinedNB_photonsLoose);
      return false;      
    }
  }

  // check if size is etaNB_photonsTightConv
  if (choice==11) {
    if ( vec.size() != etaNB_photonsTightConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs etaNB_photonsTightConv " 
		     << etaNB_photonsTightConv);
      return false;      
    }
  }

  // check if size is etNB_photonsTightConv 
  if (choice==12) {
    if ( vec.size() != etNB_photonsTightConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs etNB_photonsTightConv=" 
		     << etNB_photonsTightConv);
      return false;      
    }
  }
 
  // check if size is combinedNB_photonsTightConv
  if (choice==13) {
    if ( vec.size() != combinedNB_photonsTightConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs combinedNB_photonsTightConv=" 
		     << combinedNB_photonsTightConv);
      return false;      
    }
  }

  // check if size is etaStripsNB_photonsTightConv
  if (choice==14) {
    if ( vec.size() != etaStripsNB_photonsTightConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs etaStripsNB_photonsTightConv=" 
		     << etaStripsNB_photonsTightConv);
      return false;      
    }
  }

  // check if size is etStripsNB_photonsTightConv
  if (choice==15) {
    if ( vec.size() != etStripsNB_photonsTightConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs etStripsNB_photonsTightConv=" 
		     << etStripsNB_photonsTightConv);
      return false;      
    }
  }

  // check if size is combinedStripsNB_photonsTightConv
  if (choice==16) {
    if ( vec.size() != combinedStripsNB_photonsTightConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs combinedStripsNB_photonsTightConv=" 
		     << combinedStripsNB_photonsTightConv);
      return false;      
    }
  }

  // check if size is etaNB_photonsTightNonConv
  if (choice==21) {
    if ( vec.size() != etaNB_photonsTightNonConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs etaNB_photonsTightNonConv " 
		     << etaNB_photonsTightNonConv);
      return false;      
    }
  }

  // check if size is etNB_photonsTightNonConv 
  if (choice==22) {
    if ( vec.size() != etNB_photonsTightNonConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs etNB_photonsTightNonConv=" 
		     << etNB_photonsTightNonConv);
      return false;      
    }
  }
 
  // check if size is combinedNB_photonsTightNonConv
  if (choice==23) {
    if ( vec.size() != combinedNB_photonsTightNonConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs combinedNB_photonsTightNonConv=" 
		     << combinedNB_photonsTightNonConv);
      return false;      
    }
  }

  // check if size is etaStripsNB_photonsTightNonConv
  if (choice==24) {
    if ( vec.size() != etaStripsNB_photonsTightNonConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs etaStripsNB_photonsTightNonConv=" 
		     << etaStripsNB_photonsTightNonConv);
      return false;      
    }
  }

  // check if size is etStripsNB_photonsTightNonConv
  if (choice==25) {
    if ( vec.size() != etStripsNB_photonsTightNonConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs etStripsNB_photonsTightNonConv=" 
		     << etStripsNB_photonsTightNonConv);
      return false;      
    }
  }

  // check if size is combinedStripsNB_photonsTightNonConv
  if (choice==26) {
    if ( vec.size() != combinedStripsNB_photonsTightNonConv ) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs combinedStripsNB_photonsTightNonConv=" 
		     << combinedStripsNB_photonsTightNonConv);
      return false;      
    }
  }

  return true;
}

// ==============================================================
bool Root::TPhotonIsEMSelector::CheckVar(std::vector<int> vec, int choice) const
{
  //
  // check vector size
  // 0 : size should be 1
  // 1 : vs etaNB_photonsLoose
  // 2 : vs etNB_photonsLoose
  // 3 : vs combinedNB_photonsLoose
  //
  // 11 : vs etaNB_photonsTightConv
  // 12 : vs etNB_photonsTightConv
  // 13 : vs combinedNB_photonsTightConv
  // 14 : vs etaStripsNB_photonsTightConv
  // 15 : vs etStripsNB_photonsTightConv
  // 16 : combinedStripsNB_photonsTightConv
  // 
  // 21 : vs etaNB_photonsTightNonConv
  // 22 : vs etNB_photonsTightNonConv
  // 23 : vs combinedNB_photonsTightNonConv
  // 24 : vs etaStripsNB_photonsTightNonConv
  // 25 : vs etStripsNB_photonsTightNonConv
  // 26 : combinedStripsNB_photonsTightNonConv
  //

  // if size of vector is 0 it means cut is not defined
  if (vec.size() == 0) return false;

  switch(choice) {
    // check if size is 1
  case 0:
    if ( vec.size() != 1) {
      FAKE_MSG_ERROR("vector size is " 
		     << vec.size() << " but needs 1"); 
      return false;      
    } else {
      return true;
    }

    // check if size is etaNB_photonsLoose
  case 1: 
    {
      const unsigned int etaNB_photonsLoose = 
	CutBinEta_photonsLoose.size();
      if ( vec.size() != etaNB_photonsLoose ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs etaNB_photonsLoose=" 
		       << etaNB_photonsLoose);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is etNB_photonsLoose
  case 2:
    {
      const unsigned int etNB_photonsLoose = 
	CutBinEnergy_photonsLoose.size();
      if ( vec.size() != etNB_photonsLoose ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs etNB_photonsLoose=" 
		       << etNB_photonsLoose);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is combinedNB_photonsLoose
  case 3:
    {
      const unsigned int etaNB_photonsLoose = 
	CutBinEta_photonsLoose.size();
      const unsigned int etNB_photonsLoose = 
	CutBinEnergy_photonsLoose.size();
      unsigned int combinedNB_photonsLoose = 0;
      if (etNB_photonsLoose>0) 
	combinedNB_photonsLoose = etaNB_photonsLoose * ( etNB_photonsLoose+1);
      else 
	combinedNB_photonsLoose = etaNB_photonsLoose;
      
      if ( vec.size() != combinedNB_photonsLoose ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs combinedNB_photonsLoose=" 
		       << combinedNB_photonsLoose);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is etaNB_photonsTightConv
  case 11:
    {
      const unsigned int etaNB_photonsTightConv = 
	CutBinEta_photonsConverted.size();
      if ( vec.size() != etaNB_photonsTightConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs etaNB_photonsTightConv " 
		       << etaNB_photonsTightConv);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is etNB_photonsTightConv 
  case 12:
    {
      const unsigned int etNB_photonsTightConv  = 
	CutBinEnergy_photonsConverted.size();
      if ( vec.size() != etNB_photonsTightConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs etNB_photonsTightConv=" 
		       << etNB_photonsTightConv);
	return false;      
      } else {
	return true;
      }
    }
 
    // check if size is combinedNB_photonsTightConv
  case 13:
    {
      const unsigned int etaNB_photonsTightConv = 
	CutBinEta_photonsConverted.size();
      const unsigned int etNB_photonsTightConv  = 
	CutBinEnergy_photonsConverted.size();
      unsigned int combinedNB_photonsTightConv = 0;
      if (etNB_photonsTightConv>0) 
	combinedNB_photonsTightConv = 
	  etaNB_photonsTightConv * ( etNB_photonsTightConv+1);
      else 
	combinedNB_photonsTightConv = 
	  etaNB_photonsTightConv;
      if ( vec.size() != combinedNB_photonsTightConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs combinedNB_photonsTightConv=" 
		       << combinedNB_photonsTightConv);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is etaStripsNB_photonsTightConv
  case 14:
    {
      const unsigned int etaStripsNB_photonsTightConv = 
	CutBinEtaStrips_photonsConverted.size();
      if ( vec.size() != etaStripsNB_photonsTightConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs etaStripsNB_photonsTightConv=" 
		       << etaStripsNB_photonsTightConv);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is etStripsNB_photonsTightConv
  case 15:
    {
      const unsigned int etStripsNB_photonsTightConv = 
	CutBinEnergyStrips_photonsConverted.size();
      if ( vec.size() != etStripsNB_photonsTightConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs etStripsNB_photonsTightConv=" 
		       << etStripsNB_photonsTightConv);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is combinedStripsNB_photonsTightConv
  case 16:
    {
      const unsigned int etaStripsNB_photonsTightConv = 
	CutBinEtaStrips_photonsConverted.size();
      const unsigned int etStripsNB_photonsTightConv = 
	CutBinEnergyStrips_photonsConverted.size();
      unsigned int combinedStripsNB_photonsTightConv = 0;
      if (etStripsNB_photonsTightConv>0)
	combinedStripsNB_photonsTightConv =
	  etaStripsNB_photonsTightConv * (etStripsNB_photonsTightConv+1);
      else
	combinedStripsNB_photonsTightConv =
	  etaStripsNB_photonsTightConv;
      if ( vec.size() != combinedStripsNB_photonsTightConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs combinedStripsNB_photonsTightConv=" 
		       << combinedStripsNB_photonsTightConv);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is etaNB_photonsTightNonConv
  case 21:
    {
      const unsigned int etaNB_photonsTightNonConv = 
	CutBinEta_photonsNonConverted.size();     
      if ( vec.size() != etaNB_photonsTightNonConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs etaNB_photonsTightNonConv " 
		       << etaNB_photonsTightNonConv);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is etNB_photonsTightNonConv 
  case 22:
    {
      const unsigned int etNB_photonsTightNonConv  = 
	CutBinEnergy_photonsNonConverted.size();
      if ( vec.size() != etNB_photonsTightNonConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs etNB_photonsTightNonConv=" 
		       << etNB_photonsTightNonConv);
	return false;      
      } else {
	return true;
      }
    }
    // check if size is combinedNB_photonsTightNonConv
  case 23:
    {
      const unsigned int etaNB_photonsTightNonConv = 
	CutBinEta_photonsNonConverted.size();     
      const unsigned int etNB_photonsTightNonConv  = 
	CutBinEnergy_photonsNonConverted.size();
      unsigned int combinedNB_photonsTightNonConv = 0;
      if (etNB_photonsTightNonConv>0) 
	combinedNB_photonsTightNonConv = 
	  etaNB_photonsTightNonConv * ( etNB_photonsTightNonConv+1);
      else
	combinedNB_photonsTightNonConv = etaNB_photonsTightNonConv;
      
      if ( vec.size() != combinedNB_photonsTightNonConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs combinedNB_photonsTightNonConv=" 
		       << combinedNB_photonsTightNonConv);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is etaStripsNB_photonsTightNonConv
  case 24:
    {
      const unsigned int etaStripsNB_photonsTightNonConv = 
	CutBinEtaStrips_photonsNonConverted.size();
      if ( vec.size() != etaStripsNB_photonsTightNonConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs etaStripsNB_photonsTightNonConv=" 
		       << etaStripsNB_photonsTightNonConv);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is etStripsNB_photonsTightNonConv
  case 25:
    {
      const unsigned int etStripsNB_photonsTightNonConv = 
	CutBinEnergyStrips_photonsNonConverted.size();
      if ( vec.size() != etStripsNB_photonsTightNonConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs etStripsNB_photonsTightNonConv=" 
		       << etStripsNB_photonsTightNonConv);
	return false;      
      } else {
	return true;
      }
    }

    // check if size is combinedStripsNB_photonsTightNonConv
  case 26:
    {
      const unsigned int etaStripsNB_photonsTightNonConv = 
	CutBinEtaStrips_photonsNonConverted.size();
      const unsigned int etStripsNB_photonsTightNonConv = 
	CutBinEnergyStrips_photonsNonConverted.size();
      unsigned int combinedStripsNB_photonsTightNonConv = 0;
      if (etStripsNB_photonsTightNonConv>0)
	combinedStripsNB_photonsTightNonConv = 
	  etaStripsNB_photonsTightNonConv * (etStripsNB_photonsTightNonConv+1);
      else 
	combinedStripsNB_photonsTightNonConv = 
	  etaStripsNB_photonsTightNonConv;
      if ( vec.size() != combinedStripsNB_photonsTightNonConv ) {
	FAKE_MSG_ERROR("vector size is " 
		       << vec.size() << " but needs combinedStripsNB_photonsTightNonConv=" 
		       << combinedStripsNB_photonsTightNonConv);
	return false;      
      } else {
	return true;
      }
    }
  default:
    FAKE_MSG_ERROR("choice " << choice << " is not implemented");
    return false;
  }
}

