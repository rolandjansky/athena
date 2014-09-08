/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TElectronIsEMSelector.cxx
PACKAGE:  PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools

AUTHORS:  J. Mitrevski
CREATED:  Dec 2011
COMMENT:  evolved from egammaElectronCutIDTool by (F. Derue)

PURPOSE:  Cut based identification of electrons

UPDATED:

*********************************************************************/

// This class header
#include "ElectronPhotonSelectorTools/TElectronIsEMSelector.h"
#include <cmath>

///***** UGLY!!!!!
///***** in athena would just use the proper constant defined in the track summary header
///***** but here am forced to do this dangerous thing that could break if they decide
///***** to use different constants.
const int SummaryTypeNotSet = -1;

Root::TElectronIsEMSelector::TElectronIsEMSelector(const char* name) :
  TSelectorToolBase(name),
  isEMMask(egammaPID::ElectronMediumPP),
  PIDName(egammaPID::IsEMMedium),
  useTRTOutliers(true),
  useBLOutliers(true),
  usePIXOutliers(true),
  useSCTOutliers(true),
  useTRTXenonHits(false),
  useBLayerHitPrediction(true),

  /** @brief cluster eta range */
  m_cutNameClusterEtaRange_Electron("ClusterEtaRange_Electron"),
  
  /** @brief matching to photon (not necessarily conversion--the name is historical) */
  m_cutNameConversionMatch_Electron("ConversionMatch_Electron"),
  
  /** @brief cluster leakage into the hadronic calorimeter */
  m_cutNameClusterHadronicLeakage_Electron("ClusterHadronicLeakage_Electron"),
  /** @brief energy in 2nd sampling (i.e. E277>0) */
  m_cutNameClusterMiddleEnergy_Electron("ClusterMiddleEnergy_Electron"), 
  /** @brief energy ratio in 2nd sampling (i.e. E237/E277) */
  m_cutNameClusterMiddleEratio37_Electron("ClusterMiddleEratio37_Electron"),
  /** @brief energy ratio in 2nd sampling (i.e. E233/E237) */
  m_cutNameClusterMiddleEratio33_Electron("ClusterMiddleEratio33_Electron"),
  /** @brief width in the second sampling (i.e. Weta2) */
  m_cutNameClusterMiddleWidth_Electron("ClusterMiddleWidth_Electron"),

  /** @brief energy fraction in the third layer (i.e. f3) */
  m_cutNameClusterBackEnergyFraction_Electron("ClusterBackEnergyFraction_Electron"),
  
  /** @brief fraction of energy found in 1st sampling (NB: not used in fact for electrons)*/
  m_cutNameClusterStripsEratio_Electron("ClusterStripsEratio_Electron"),
  /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) */
  m_cutNameClusterStripsDeltaEmax2_Electron("ClusterStripsDeltaEmax2_Electron"),
  /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) */
  m_cutNameClusterStripsDeltaE_Electron("ClusterStripsDeltaE_Electron"),
  /** @brief shower width in 1st sampling */
  m_cutNameClusterStripsWtot_Electron("ClusterStripsWtot_Electron"),
  /** @brief shower shape in shower core 1st sampling */
  m_cutNameClusterStripsFracm_Electron("ClusterStripsFracm_Electron"),
  /** @brief shower width weighted by distance from the maximum one */
  m_cutNameClusterStripsWeta1c_Electron("ClusterStripsWeta1c_Electron"),
  
  /** @brief difference between max and 2nd max in strips */
  m_cutNameClusterStripsDEmaxs1_Electron("ClusterStripsDEmaxs1_Electron"),
  /** @brief B layer hit */
  m_cutNameTrackBlayer_Electron("TrackBlayer_Electron"),
  /** @brief number of Pixel hits */
  m_cutNameTrackPixel_Electron("TrackPixel_Electron"),
  /** @brief number of Pixel and SCT hits */
  m_cutNameTrackSi_Electron("TrackSi_Electron"),
  /** @brief distance of closet approach */
  m_cutNameTrackA0_Electron("TrackA0_Electron"),
  /** @brief eta difference between cluster and extrapolated track in the 1st sampling */
  m_cutNameTrackMatchEta_Electron("TrackMatchEta_Electron"),
  /** @brief phi difference between cluster and extrapolated track in the 2nd sampling */
  m_cutNameTrackMatchPhi_Electron("TrackMatchPhi_Electron"),
  /** @brief energy-momentum match */
  m_cutNameTrackMatchEoverP_Electron("TrackMatchEoverP_Electron"),
  /** @brief number of TRT hits */
  m_cutNameTrackTRThits_Electron("TrackTRThits_Electron"),
  /** @brief ratio of high to all TRT hits for isolated electrons */
  m_cutNameTrackTRTratio_Electron("TrackTRTratio_Electron"),
  /** @brief ratio of high to all TRT hits for non-isolated electrons (not for new ++ menus) */    
  m_cutNameTrackTRTratio90_Electron("TrackTRTratio90_Electron"),
  
  /** @brief distance of closet approach for tight selection (not to be used in new ++ menus) */
  m_cutNameTrackA0Tight_Electron("TrackA0Tight_Electron"),
  /** @brief eta difference between cluster and extrapolated track in the 1st sampling for 
      tight selection (not to be used in new ++ menus)*/
  m_cutNameTrackMatchEtaTight_Electron("TrackMatchEtaTight_Electron"),
  
  /** @brief isolation */
  m_cutNameIsolation_Electron("Isolation_Electron"),
  /** @brief calorimetric isolation */
  m_cutNameClusterIsolation_Electron("ClusterIsolation_Electron"),
  /** @brief tracker isolation */
  m_cutNameTrackIsolation_Electron("TrackIsolation_Electron")
{
}

// ====================================================================
Root::TElectronIsEMSelector::~TElectronIsEMSelector() 
{
  //
  // destructor
  //
}

//=============================================================================
// Initialize this selector tool
//=============================================================================
FakeStatusCode Root::TElectronIsEMSelector::initialize()
{

  FakeStatusCode sc(FkStatusCode::SUCCESS);

  // --------------------------------------------------------------------------
  // Register the cuts and check that the registration worked:
  // NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit 1,...

  /** @brief cluster eta range, bit 0 */
  m_cutPositionClusterEtaRange_Electron = 
    m_accept.addCut(m_cutNameClusterEtaRange_Electron, "Electron within eta range");
  if (m_cutPositionClusterEtaRange_Electron < 0) sc = FkStatusCode::FAILURE;
  
  /** @brief matching to photon (not necessarily conversion--the name is historical), but 1 */
  m_cutPositionConversionMatch_Electron = 
    m_accept.addCut(m_cutNameConversionMatch_Electron, "Electron matches a photon with AR > LOOSE");
  if (m_cutPositionConversionMatch_Electron < 0) sc = FkStatusCode::FAILURE;
    
  /** @brief cluster leakage into the hadronic calorimeter, bit 2 */
  m_cutPositionClusterHadronicLeakage_Electron = 
    m_accept.addCut(m_cutNameClusterHadronicLeakage_Electron, "Had leakage < Cut");
  if (m_cutPositionClusterHadronicLeakage_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy in 2nd sampling (i.e. E277>0), bit 3 */
  m_cutPositionClusterMiddleEnergy_Electron = 
    m_accept.addCut(m_cutNameClusterMiddleEnergy_Electron, "Energy in second sampling (E277) > Cut"); 
  if (m_cutPositionClusterMiddleEnergy_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy ratio in 2nd sampling (i.e. E237/E277), bit 4 */
  m_cutPositionClusterMiddleEratio37_Electron = 
    m_accept.addCut(m_cutNameClusterMiddleEratio37_Electron, "E237/377 > Cut");
  if (m_cutPositionClusterMiddleEratio37_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy ratio in 2nd sampling (i.e. E233/E237), bit 5 */
  m_cutPositionClusterMiddleEratio33_Electron = 
    m_accept.addCut(m_cutNameClusterMiddleEratio33_Electron, "E233/E237 > Cut");
  if (m_cutPositionClusterMiddleEratio33_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief width in the second sampling (i.e. Weta2), bit 6 */
  m_cutPositionClusterMiddleWidth_Electron = 
    m_accept.addCut(m_cutNameClusterMiddleWidth_Electron, "Weta2 < Cut");
  if (m_cutPositionClusterMiddleWidth_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy fraction in the third layer, bit 7 */
  m_cutPositionClusterBackEnergyFraction_Electron = 
    m_accept.addCut(m_cutNameClusterBackEnergyFraction_Electron, "f3 < Cut");
  if (m_cutPositionClusterBackEnergyFraction_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief fraction of energy found in 1st sampling (NB: not used in fact for electrons), bit 8*/
  m_cutPositionClusterStripsEratio_Electron = 
    m_accept.addCut(m_cutNameClusterStripsEratio_Electron, "No Cut");
  if (m_cutPositionClusterStripsEratio_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et), bit 9 */
  m_cutPositionClusterStripsDeltaEmax2_Electron = 
    m_accept.addCut(m_cutNameClusterStripsDeltaEmax2_Electron, "emax2/(1000.+0.009*et) < Cut, where emax2 is the energy of the second max");
  if (m_cutPositionClusterStripsDeltaEmax2_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1), bit 10 */
  m_cutPositionClusterStripsDeltaE_Electron = 
    m_accept.addCut(m_cutNameClusterStripsDeltaE_Electron, "difference between 2nd maximium and first minimum < Cut");
  if (m_cutPositionClusterStripsDeltaE_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief shower width in 1st sampling, bit 11 */
  m_cutPositionClusterStripsWtot_Electron = 
    m_accept.addCut(m_cutNameClusterStripsWtot_Electron, "Total shower width in first sampling < Cut");
  if (m_cutPositionClusterStripsWtot_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief shower shape in shower core 1st sampling, bit 12 */
  m_cutPositionClusterStripsFracm_Electron = 
    m_accept.addCut(m_cutNameClusterStripsFracm_Electron, "Fracm (aka Fside) < Cut");
  if (m_cutPositionClusterStripsFracm_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief shower width weighted by distance from the maximum one, bit 13 */
  m_cutPositionClusterStripsWeta1c_Electron = 
    m_accept.addCut(m_cutNameClusterStripsWeta1c_Electron, "Shower width in 3 strips in first sampling < Cut");
  if (m_cutPositionClusterStripsWeta1c_Electron < 0) sc = FkStatusCode::FAILURE;

  int voidcutpos = m_accept.addCut("VOID1", "No Cut"); // bit 14 is not used
  if (voidcutpos < 0) sc = FkStatusCode::FAILURE;


  /** @brief difference between max and 2nd max in strips, bit 15 */
  m_cutPositionClusterStripsDEmaxs1_Electron = 
    m_accept.addCut(m_cutNameClusterStripsDEmaxs1_Electron, "Difference between first and second max > Cut");
  if (m_cutPositionClusterStripsDEmaxs1_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief B layer hit, bit 16 */
  m_cutPositionTrackBlayer_Electron = 
    m_accept.addCut(m_cutNameTrackBlayer_Electron, "nBL > 0, maybe including outliers and using expectHitInBLayer");
  if (m_cutPositionTrackBlayer_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief number of Pixel hits, bit 17 */
  m_cutPositionTrackPixel_Electron = 
    m_accept.addCut(m_cutNameTrackPixel_Electron, "nPi > Cut, maybe including outliers");
  if (m_cutPositionTrackPixel_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief number of Pixel and SCT hits, bit 18 */
  m_cutPositionTrackSi_Electron = 
    m_accept.addCut(m_cutNameTrackSi_Electron, "nSi > Cut, maybe including outliers");
  if (m_cutPositionTrackSi_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief distance of closet approach, bit 19 */
  m_cutPositionTrackA0_Electron = 
    m_accept.addCut(m_cutNameTrackA0_Electron, "A0 (aka d0) wrt beam spot < Cut");
  if (m_cutPositionTrackA0_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief eta difference between cluster and extrapolated track in the 1st sampling, bit 20 */
  m_cutPositionTrackMatchEta_Electron = 
    m_accept.addCut(m_cutNameTrackMatchEta_Electron, "Track match deta in 1st sampling < Cut");
  if (m_cutPositionTrackMatchEta_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief phi difference between cluster and extrapolated track in the 2nd sampling, bit 21 */
  m_cutPositionTrackMatchPhi_Electron = 
    m_accept.addCut(m_cutNameTrackMatchPhi_Electron, "Track match dphi in 2nd sampling < Cut");
  if (m_cutPositionTrackMatchPhi_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief energy-momentum match, bit 22 */
  m_cutPositionTrackMatchEoverP_Electron = 
    m_accept.addCut(m_cutNameTrackMatchEoverP_Electron, "E/p < Cut");
  if (m_cutPositionTrackMatchEoverP_Electron < 0) sc = FkStatusCode::FAILURE;

  voidcutpos = m_accept.addCut("VOID2", "No Cut"); // bit 23 is not used
  if (voidcutpos < 0) sc = FkStatusCode::FAILURE;

  /** @brief number of TRT hits, bit 24 */
  m_cutPositionTrackTRThits_Electron = 
    m_accept.addCut(m_cutNameTrackTRThits_Electron, "number of TRT hits > Cut");
  if (m_cutPositionTrackTRThits_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief ratio of high to all TRT hits for isolated electrons, bit 25 */
  m_cutPositionTrackTRTratio_Electron = 
    m_accept.addCut(m_cutNameTrackTRTratio_Electron, "ration of high to all TRT hits > Cut");
  if (m_cutPositionTrackTRTratio_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief ratio of high to all TRT hits for non-isolated electrons (not for new ++ menus), bit 26 */    
  m_cutPositionTrackTRTratio90_Electron = 
    m_accept.addCut(m_cutNameTrackTRTratio90_Electron, "ration of high to all TRT hits > Cut, 90% cut, (only for old tight menu)");
  if (m_cutPositionTrackTRTratio90_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief distance of closet approach for tight selection (not to be used in new ++ menus), bit 27 */
  m_cutPositionTrackA0Tight_Electron = 
    m_accept.addCut(m_cutNameTrackA0Tight_Electron, "tight cut on d0 for old tight menu (not used otherwis)");
  if (m_cutPositionTrackA0Tight_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief eta difference between cluster and extrapolated track in the 1st sampling for 
      tight selection (not to be used in new ++ menus), bit 28*/
  m_cutPositionTrackMatchEtaTight_Electron = 
    m_accept.addCut(m_cutNameTrackMatchEtaTight_Electron, "tight cut on deta only for old tight menu");
  if (m_cutPositionTrackMatchEtaTight_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief isolation, bit 29 */
  m_cutPositionIsolation_Electron = 
    m_accept.addCut(m_cutNameIsolation_Electron, "Track and calorimetric isolation");
  if (m_cutPositionIsolation_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief calorimetric isolation, bit 30 */
  m_cutPositionClusterIsolation_Electron = 
    m_accept.addCut(m_cutNameClusterIsolation_Electron, "calorimetric isolation only");
  if (m_cutPositionClusterIsolation_Electron < 0) sc = FkStatusCode::FAILURE;

  /** @brief tracker isolation, bit 31 */
  m_cutPositionTrackIsolation_Electron = 
    m_accept.addCut(m_cutNameTrackIsolation_Electron, "track isolation only");
  if (m_cutPositionTrackIsolation_Electron < 0) sc = FkStatusCode::FAILURE;

  if (sc == FkStatusCode::FAILURE) {
    FAKE_MSG_ERROR("Exceeded the number of allowed cuts in TElectronIsEMSelector");
  }

  return sc;
}

const Root::TAccept& Root::TElectronIsEMSelector::fillAccept() const
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
const Root::TAccept& Root::TElectronIsEMSelector::accept( 
							 // eta position in second sampling
							 float eta2,
							 // transverse energy in calorimeter (using eta position in second sampling)
							 double et,
							 //////////////// - calo
							 // E(3*3) in 2nd sampling
							 float e233,
							 // E(3*7) in 2nd sampling
							 float e237,
							 // E(7*7) in 2nd sampling
							 float e277,
							 // transverse energy in 1st scintillator of hadronic calorimeter
							 float ethad1,
							 // transverse energy in hadronic calorimeter
							 float ethad,
							 // shower width in 3 strips in 1st sampling
							 float weta1c,
							 // shower width in 2nd sampling
							 float weta2c,
							 // fraction of energy reconstructed in the 1st sampling
							 float f1,
							 // E of 2nd max between max and min in strips
							 float emax2,
							 // E of 1st max in strips
							 float emax,
							 // E(min) in strips
							 float emin,
							 // total shower width in 1st sampling
							 float wtot,
							 // E(+/-3)-E(+/-1)/E(+/-1)
							 float fracm,
							 // fraction of energy reconstructed in the 3rd sampling
							 float f3,
							 //////////////// - tracking
							 // number of B-layer hits
							 int nBL,
							 int nBLOutliers,
							 // number of Pixel hits
							 int nPi,
							 int nPiOutliers,
							 // number of SCT hits
							 int nSCT,
							 int nSCTOutliers,
							 // TRT hits
							 int nTRThigh,
							 int nTRThighOutliers,
							 int nTRT,
							 int nTRTOutliers,
							 int nTRTXenonHits,
							 // transverse impact parameter
							 float trackd0,
							 // Delta eta,phi matching
							 float deltaeta,
							 float deltaphi,
							 // E/p
							 double ep,
							 bool expectHitInBLayer,
							 // The ambiguity result 
							 EMAmbiguityType::AmbiguityResult ambiguityResult) const 
{
  // Reset the cut result bits to zero (= fail cut)
  m_accept.clear();
  // -----------------------------------------------------------
  // Do the actual selection

  m_isEM = calcIsEm(eta2,
		    et,
                    e233,
		    e237,
		    e277,
		    ethad1,
		    ethad,
		    weta1c,
		    weta2c,
		    f1,
		    emax2,
		    emax,
		    emin,
		    wtot,
		    fracm,
		    f3,
		    nBL,
		    nBLOutliers,
		    nPi,
		    nPiOutliers,
		    nSCT,
		    nSCTOutliers,
		    nTRThigh,
		    nTRThighOutliers,
		    nTRT,
		    nTRTOutliers,
		    nTRTXenonHits,
		    trackd0,
		    deltaeta,
		    deltaphi,
		    ep,
		    expectHitInBLayer,
		    ambiguityResult);

  return fillAccept();

}

// return the isem
unsigned int Root::TElectronIsEMSelector::calcIsEm(
						   // eta position in second sampling
						   float eta2,
						   // transverse energy in calorimeter (using eta position in second sampling)
						   double et,
						   //////////////// - calo
						   // E(3*3) in 2nd sampling
						   float e233,
						   // E(3*7) in 2nd sampling
						   float e237,
						   // E(7*7) in 2nd sampling
						   float e277,
						   // transverse energy in 1st scintillator of hadronic calorimeter
						   float ethad1,
						   // transverse energy in hadronic calorimeter
						   float ethad,
						   // shower width in 3 strips in 1st sampling
						   float weta1c,
						   // shower width in 2nd sampling
						   float weta2c,
						   // fraction of energy reconstructed in the 1st sampling
						   float f1,
						   // E of 2nd max between max and min in strips
						   float emax2,
						   // E of 1st max in strips
						   float emax,
						   // E(min) in strips
						   float emin,
						   // total shower width in 1st sampling
						   float wtot,
						   // E(+/-3)-E(+/-1)/E(+/-1)
						   float fracm,
						   // fraction of energy reconstructed in the 3rd sampling
						   float f3,
						   //////////////// - tracking
						   // number of B-layer hits
						   int nBL,
						   int nBLOutliers,
						   // number of Pixel hits
						   int nPi,
						   int nPiOutliers,
						   // number of SCT hits
						   int nSCT,
						   int nSCTOutliers,
						   // TRT hits
						   int nTRThigh,
						   int nTRThighOutliers,
						   int nTRT,
						   int nTRTOutliers,
						   int nTRTXenonHits,
						   // transverse impact parameter
						   float trackd0,
						   // Delta eta,phi matching
						   float deltaeta,
						   float deltaphi,
						   // E/p
						   double ep,
						   bool expectHitInBLayer,
						   // The ambiguity result 
						   EMAmbiguityType::AmbiguityResult ambiguityResult) const
{ 
  unsigned int iflag = calocuts_electrons(eta2,
					  et,
                                          e233,
					  e237,
					  e277,
					  ethad1,
					  ethad,
					  weta1c,
					  weta2c,
					  f1,
					  emax2,
					  emax,
					  emin,
					  wtot,
					  fracm,
					  f3,
					  0);

  iflag = TrackCut(eta2,
		   et,
		   nBL,
		   nBLOutliers,
		   nPi,
		   nPiOutliers,
		   nSCT,
		   nSCTOutliers,
		   nTRThigh,
		   nTRThighOutliers,
		   nTRT,
		   nTRTOutliers,
		   nTRTXenonHits,
		   trackd0,
		   deltaeta,
		   deltaphi,
		   ep,
		   expectHitInBLayer,
		   iflag);

  iflag = ambiguitycuts_electrons(ambiguityResult, iflag);
  return iflag;
}

// ======================================================================
unsigned int Root::TElectronIsEMSelector::calocuts_electrons(
							     // eta position in second sampling
							     float eta2,
							     // transverse energy in calorimeter (using eta position in second sampling)
							     double et,
							     // E(3*7) in 2nd sampling
							     float e233,
							     // E(7*7) in 2nd sampling
							     float e237,
							     // E(7*7) in 2nd sampling
							     float e277,
							     // transverse energy in 1st scintillator of hadronic calorimeter
							     float ethad1,
							     // transverse energy in hadronic calorimeter
							     float ethad,
							     // shower width in 3 strips in 1st sampling
							     float weta1c,
							     // shower width in 2nd sampling
							     float weta2c,
							     // fraction of energy reconstructed in the 1st sampling
							     float f1,
							     // E of 2nd max between max and min in strips
							     float emax2,
							     // E of 1st max in strips
							     float emax,
							     // E(min) in strips
							     float emin,
							     // total shower width in 1st sampling
							     float wtot,
							     // E(+/-3)-E(+/-1)/E(+/-1)
							     float fracm,
							     // fraction of energy reconstructed in the 3rd sampling
							     float f3,
							     // the starting isEM (should be 0 if new)
							     unsigned int iflag,
                                                             // trigETthr : threshold in ET to apply the cuts at trigger level
                                                             double trigEtTh
							     ) const
{

  // derived variables

  // hadronic leakage variables
  double raphad1 = fabs(et) > 0. ? ethad1/et : 0.;
  double raphad  = fabs(et) > 0. ? ethad/et : 0.;

  // (Emax1-Emax2)/(Emax1+Emax2)
  double demaxs1 = fabs(emax+emax2)>0. ? (emax-emax2)/(emax+emax2) : 0.;

  // parametrizatiion of emax2
  double deltaemax2= emax2/(1000.+0.009*et);

  // modifiy et when dealing with trigger
  // to be sure that it will take the correct bin (VD)
  if(trigEtTh > 0) et = trigEtTh*1.01; 

  int ibin_et = -1;
  // loop on ET range
  for (unsigned int ibinET=0;ibinET<=CutBinET.size();ibinET++) {
    if ( ibinET == 0 ) {
      if (et < CutBinET[ibinET] ) {
	ibin_et = ibinET;
      }
    } 
    else if ( ibinET > 0 && ibinET <CutBinET.size() ) {
      if ( et >= CutBinET[ibinET-1] && 
	   et < CutBinET[ibinET] ) {
	ibin_et = ibinET;
      }
    }
    else if ( ibinET == CutBinET.size() ) {
      if ( et >= CutBinET[ibinET-1] ) {
	ibin_et = ibinET;
      }
    }
  }
      
  int ibin_eta = -1;
  // loop on eta range
  for (unsigned int ibinEta=0;ibinEta<CutBinEta.size();ibinEta++) {
    if ( ibinEta == 0 ){
      if ( eta2 < CutBinEta[ibinEta] ) {
	ibin_eta = ibinEta;
      }
    }
    else {
      if ( eta2 >= CutBinEta[ibinEta-1] && 
	   eta2 < CutBinEta[ibinEta] ) {
	ibin_eta = ibinEta;
      }
    }
  }

  // check the bin number
  int ibin_combined =  ibin_et*CutBinEta.size()+ibin_eta;

  // check eta range 
  if (ibin_eta < 0) {  
     iflag |= (0x1 << egammaPID::ClusterEtaRange_Electron);  
  }
 
  // check if index is defined
  if (ibin_eta>=0 && ibin_et>=0 &&  ibin_combined>=0) {
  
    // hadronic leakage
    if (CheckVar(CutHadLeakage,4)) {
      if (eta2 < 0.8) {
	if (raphad1>CutHadLeakage[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_Electron);
      } else if (eta2 >= 0.8 && eta2 < 1.37) {
	if (raphad>CutHadLeakage[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_Electron);
      } else {
	if (raphad1>CutHadLeakage[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterHadronicLeakage_Electron);
      }
    }
    
    // cut on f3
    if (CheckVar(CutF3,4)) {
      if (f3 > CutF3[ibin_combined]) {
	iflag |= ( 0x1 << egammaPID::ClusterBackEnergyFraction_Electron);
      } 
    }

    // cuts on 2nd sampling
    if (e277<=0.) iflag |= ( 0x1 << egammaPID::ClusterMiddleEnergy_Electron) ; 
    if (CheckVar(CutReta37,4)) {
      if (e277 != 0. && e237/e277<=CutReta37[ibin_combined]) 
	iflag |= ( 0x1 << egammaPID::ClusterMiddleEratio37_Electron);
    }
    // -------------------------------
    if (CheckVar(CutRphi37,4)) {
      if (e237 != 0. && e233/e237<=CutRphi37[ibin_combined]) 
	iflag |= ( 0x1 << egammaPID::ClusterMiddleEratio33_Electron);
    }
    // -------------------------------
    // check Weta2c
    if (CheckVar(CutWeta2c,4)) {
      if (weta2c>CutWeta2c[ibin_combined]) 
	iflag |= ( 0x1 << egammaPID::ClusterMiddleWidth_Electron);
    }
    // cuts on 1st sampling     
    // we remove also crack
    if (f1>CutF1[0] && 
	eta2 < 2.37             && 
	(eta2 < 1.37 || eta2 > 1.52)) {

      // check Rmax2
      if (CheckVar(CutDeltaEmax2,4)) {
	if (emax2>0. && deltaemax2>=CutDeltaEmax2[ibin_combined] )
	  iflag |= ( 0x1 << egammaPID::ClusterStripsDeltaEmax2_Electron);
      }

      // check Delta E
      if (CheckVar(CutDeltaE,4)) {
	if (emax2>0. &&  (emax2-emin)>=CutDeltaE[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterStripsDeltaE_Electron);
      }

      // check DEmaxs1
      if (CheckVar(CutDEmaxs1,4)) {
	if (demaxs1<=CutDEmaxs1[ibin_combined])
	  iflag |= ( 0x1 << egammaPID::ClusterStripsDEmaxs1_Electron);
      }

      // check Wtot
      if (CheckVar(CutWtot,4)) {
	if (wtot>=CutWtot[ibin_combined] )
	  iflag |= ( 0x1 << egammaPID::ClusterStripsWtot_Electron);
      }

      // check Fside
      if (CheckVar(CutFracm,4)) {
	if ( fracm >= CutFracm[ibin_combined] ) 
	  iflag |= ( 0x1 << egammaPID::ClusterStripsFracm_Electron);
      }

      // check Weta1c
      if (CheckVar(CutWeta1c,4)) {
	if (  weta1c >= CutWeta1c[ibin_combined]) 
	  iflag |= ( 0x1 << egammaPID::ClusterStripsWeta1c_Electron);
      }
    }

    // // check cluster isolation
    // if (CheckVar(CutClusterIsolation,4)) {
    //   if ( etconeIso >= CutClusterIsolation[ibin_combined])
    // 	iflag |= ( 0x1 << egammaPID::ClusterIsolation_Electron);
    // }
  }
  
  return iflag; 
}



unsigned int Root::TElectronIsEMSelector::TrackCut(  
						   // eta of the cluster in the 2nd sampling
						   float eta2,
						   // transverse energy in calorimeter (using eta position in second sampling)
						   double et,
						   // number of B-layer hits
						   int nBL,
						   int nBLOutliers,
						   // number of Pixel hits
						   int nPi,
						   int nPiOutliers,
						   // number of SCT hits
						   int nSCT,
						   int nSCTOutliers,
						   // TRT hits
						   int nTRThigh,
						   int nTRThighOutliers,
						   int nTRT,
						   int nTRTOutliers,
						   int nTRTXenonHits,
						   // transverse impact parameter
						   float trackd0,
						   // Delta eta,phi matching
						   float deltaeta,
						   float deltaphi,
						   // E/p
						   double ep,
						   bool expectHitInBLayer,
						   unsigned int iflag) const
{
  int ibin_et = -1;
  // loop on ET range
  for (unsigned int ibinET=0;ibinET<=CutBinET.size();ibinET++) {
    if ( ibinET == 0 ) {
      if (et < CutBinET[ibinET] ) {
	ibin_et = ibinET;
      }
    } 
    else if ( ibinET > 0 && ibinET <CutBinET.size() ) {
      if ( et >= CutBinET[ibinET-1] && 
	   et < CutBinET[ibinET] ) {
	ibin_et = ibinET;
      }
    }
    else if ( ibinET == CutBinET.size() ) {
      if ( et >= CutBinET[ibinET-1] ) {
	ibin_et = ibinET;
      }
    }
  }
  
  int ibin_eta = -1;
  // loop on eta range
  for (unsigned int ibinEta=0;ibinEta<CutBinEta.size();ibinEta++) {
    if ( ibinEta == 0 ){
      if ( eta2 < CutBinEta[ibinEta] ) {
	ibin_eta = ibinEta;
      }
    } else {
      if ( eta2 >= CutBinEta[ibinEta-1] && 
	   eta2 < CutBinEta[ibinEta] ) {
	ibin_eta = ibinEta;
      }
    }
  }
  
  // check the bin number
  int ibin_combined =  ibin_et*CutBinEta.size()+ibin_eta;


  int nSi = nPi + nSCT;
  
  if (useBLOutliers) {
    nBL += nBLOutliers;
  }
  if (usePIXOutliers) {
    nPi += nPiOutliers;
    nSi += nPiOutliers;
  }
  if (useSCTOutliers) {
    nSi += nSCTOutliers;
  }
  
  if (ibin_eta>=0) {
    // Track quality cuts
    // cuts on number of b-layer hits - check if module was alive
    if ((useBLayerHitPrediction &&
	 (nPi == 0 || expectHitInBLayer)) || 
	!useBLayerHitPrediction) {
      if (CheckVar(CutBL,1)) {
	if (nBL<CutBL[ibin_eta])
	  iflag |= ( 0x1 << egammaPID::TrackBlayer_Electron) ; 
      }
    }

    // cuts on number of pixel hits
    if (CheckVar(CutPi,1)) {
      if (nPi<CutPi[ibin_eta]) 
	iflag |= ( 0x1 << egammaPID::TrackPixel_Electron) ; 
    }
    // cuts on number of precision hits
    if (CheckVar(CutSi,1)) {
      if (nSi<CutSi[ibin_eta]) 
	iflag |= ( 0x1 << egammaPID::TrackSi_Electron) ; 
    }
    // cuts on transverse impact parameter
    if (CheckVar(CutA0,1)) {
      if (trackd0>CutA0[ibin_eta]) 
	iflag |= ( 0x1 << egammaPID::TrackA0_Electron) ; 
    }
    // cuts on transverse impact parameter for tight selection
    if (CheckVar(CutA0Tight,1)) {
      if (trackd0>CutA0Tight[ibin_eta]) 
	iflag |= ( 0x1 << egammaPID::TrackA0Tight_Electron) ; 
    }
    
    if (ibin_combined>=0) {
      
      // matching (eta,phi) and energy-momentum
      // cut on Delta Eta and Delta Phi

      deltaeta=fabsf(deltaeta); // use absolute value

      if (CheckVar(CutDeltaEta,4)) {
	if ( deltaeta > CutDeltaEta[ibin_combined] ) 
	  iflag |= ( 0x1 << egammaPID::TrackMatchEta_Electron); 
      }
      if (CheckVar(CutminDeltaPhi,4)) {
	if ( deltaphi < CutminDeltaPhi[ibin_combined] ) 
	  iflag |= ( 0x1 << egammaPID::TrackMatchPhi_Electron); 
      }
      if (CheckVar(CutmaxDeltaPhi,4)) {
	if ( deltaphi > CutmaxDeltaPhi[ibin_combined] ) 
	  iflag |= ( 0x1 << egammaPID::TrackMatchPhi_Electron); 
      }
      // cut on Delta Eta for Tight selection
      if (CheckVar(CutDeltaEtaTight,4)) {
	if ( deltaeta > CutDeltaEtaTight[ibin_combined] ) 
	  iflag |= ( 0x1 << egammaPID::TrackMatchEtaTight_Electron); 
      }

      // cut on E/p
      if (CheckVar(CutminEp,4) && 
	  CheckVar(CutmaxEp,4)) {
	if ( ep < CutminEp[ibin_combined] || 
	     ep > CutmaxEp[ibin_combined] ) 
	  iflag |= ( 0x1 << egammaPID::TrackMatchEoverP_Electron); 
      }
    }
  }

  // use of Transition Radiation Tracker
  double rTRT = 0;
  int nTRTTotal = 0;
  if (useTRTOutliers) { 

    ///**** UGLY, UGLY;
    ///**** can't actually use the constant define in TrackSummary
    if (useTRTXenonHits && (nTRTXenonHits!=SummaryTypeNotSet)) nTRTTotal= nTRTXenonHits; 
    else nTRTTotal = nTRT+nTRTOutliers; 
    
    rTRT = (nTRTTotal) > 0 ?  
      ((double) (nTRThigh+nTRThighOutliers)/(nTRTTotal) ) : 0.; 

  } else {
    rTRT = (nTRT) > 0 ? ((double) (nTRThigh)/(nTRT) ) : 0.;
    nTRTTotal = nTRT;
  }
  
  double DeltaNum = -100;
  // coefficients to aproximate Number of TRT hits:
  // zone 0: eta<0.1 parabolic
  const double a0 = 33.14 ; const double b0 = -129.1 ; const double c0 = 1455.;
  // zone 1: eta<0.625 cubic
  const double a1 = 29.42 ; const double b1 = 27.93 ; const double c1 = -89.96; const double d1 = 91.51;
  // zone 2: eta<1.07 parabolic
  const double a2 = 196.3; const double b2 = -403.; const double c2 = 230.2;
  // zone 3: eta <1.304 linear
  const double a3 = -10.59; const double b3 = 37.29;
  // zone 4: eta <1.752 cubic
  const double a4 = -640.9; const double b4 = 1323.; const double c4 = -851.8; const double d4 = 180.8;
  // zone 5: eta <2.0 linear
  const double a5 = 159.8; const double b5 = -70.9;
  
  int ibin_eta_TRT = -1;
  // loop on eta range
  for (unsigned int ibinEta=0;ibinEta<CutBinEta_TRT.size();ibinEta++) {
    if ( ibinEta == 0 ){
      if ( eta2 < CutBinEta_TRT[ibinEta] ) {
	ibin_eta_TRT = ibinEta;
      }
    } else {
      if ( eta2 >= CutBinEta_TRT[ibinEta-1] && 
	   eta2 < CutBinEta_TRT[ibinEta] ) {
	ibin_eta_TRT = ibinEta;
      }
    }
  }
  if (ibin_eta_TRT >= 0) {
    switch (ibin_eta_TRT) {
      
    case 0:
      DeltaNum = nTRTTotal - (a0 + b0*eta2 + c0*eta2*eta2);
      break;
      
    case 1:
      DeltaNum = nTRTTotal - (a1 + b1*eta2 + c1*eta2*eta2 + d1*eta2*eta2*eta2);
      break;
      
    case 2:
      DeltaNum = nTRTTotal - (a2 + b2*eta2 + c2*eta2*eta2) ;
      break;
      
    case 3:
      DeltaNum = nTRTTotal - (a3 + b3*eta2);
      break;
      
    case 4: 
      DeltaNum = nTRTTotal - (a4 + b4*eta2 + c4*eta2*eta2 + d4*eta2*eta2*eta2);
      break;
      
    case 5: 
      DeltaNum = nTRTTotal - (a5 + b5*eta2);
    }
    
    if (CheckVar(CutNumTRT,2)) {
      if ( DeltaNum < CutNumTRT[ibin_eta_TRT]) {
	iflag |= ( 0x1 << egammaPID::TrackTRThits_Electron);
      }
    }
    if (CheckVar(CutTRTRatio,2)) {
      if ( rTRT < CutTRTRatio[ibin_eta_TRT]  && nTRTTotal > 0) {
	iflag |= ( 0x1 << egammaPID::TrackTRTratio_Electron);
      } 
    }
    if (CheckVar(CutTRTRatio90,2)) { // not really used
      if ( rTRT < CutTRTRatio90[ibin_eta_TRT] ) {
	iflag |= ( 0x1 << egammaPID::TrackTRTratio90_Electron);
      } 
    }
  }
  
  return iflag; 
}

// ==============================================================
unsigned int Root::TElectronIsEMSelector::ambiguitycuts_electrons(EMAmbiguityType::AmbiguityResult ambiguityResult, 
								  unsigned int iflag) const 
{
  // should maybe make it configurable
  if (ambiguityResult <= EMAmbiguityType::LOOSE ||
      ambiguityResult == EMAmbiguityType::MEDIUM) {    
    // set bit to 0 (passed)
    iflag &= ~(0x1 << egammaPID::ConversionMatch_Electron); 
  } else {
    // set bit to 1 (not passed)
    iflag |= (0x1 << egammaPID::ConversionMatch_Electron); 
  }
  return iflag;
}

// ==============================================================
bool Root::TElectronIsEMSelector::CheckVar(std::vector<float> vec, int choice) const
{
  //
  // check vector size
  // 0 : size should be 1
  // 1 : vs etaNB
  // 2 : vs etaTRTNB
  // 3 : vs etNB
  // 4 : vs combinedNB
  //

  unsigned int etaNB = CutBinEta.size();
  unsigned int etaTRTNB = CutBinEta_TRT.size();
  unsigned int etNB =  CutBinET.size();
  unsigned int combinedNB = 0;
  if (etNB>1) 
    combinedNB = 
      etaNB * (etNB+1);
  else 
    combinedNB = 
      etaNB;

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

  // check if size is etaNB
  if (choice==1) {
    if ( vec.size() != etaNB ) {
      FAKE_MSG_ERROR("vector size is " 
		    << vec.size() << " but needs " 
		    << etaNB);
      return false;      
    }
  }

  // check if size is etaTRTNB
  if (choice==2) {
    if ( vec.size() != etaTRTNB ) {
      FAKE_MSG_ERROR("vector size is " 
		    << vec.size() << " but needs " 
		    << etaTRTNB);
      return false;      
    }
  }

  // check if size is etNB
  if (choice==3) {
    if ( vec.size() != etNB ) {
      FAKE_MSG_ERROR("vector size is " 
		    << vec.size() << " but needs " 
		    << etNB);
      return false;      
    }
  }

  // check if size is combinedNB 
  if (choice==4) {
    if ( vec.size() != combinedNB ) {
      FAKE_MSG_ERROR("vector size is " 
		    << vec.size() << " but needs " 
		    << combinedNB);
      return false;      
    }
  }
 
  return true;
}

// ==============================================================
bool Root::TElectronIsEMSelector::CheckVar(std::vector<int> vec, int choice) const
{
  //
  // check vector size
  // 0 : size should be 1
  // 1 : vs etaNB
  // 2 : vs etaTRTNB
  // 3 : vs etNB
  // 4 : vs combinedNB
  //


  // if size of vector is 0 it means cut is not defined
  if (vec.size() == 0) return false;


  switch(choice) {
  case 0:
    if ( vec.size() != 1) {
      FAKE_MSG_ERROR("vector size is " 
		    << vec.size() << " but needs 1"); 
      return false;      
    } else {
      return true;
    }

  // check if size is etaNB
  case 1:
    {
      const unsigned int etaNB = CutBinEta.size();
      if ( vec.size() != etaNB ) {
	FAKE_MSG_ERROR("vector size is " 
		      << vec.size() << " but needs etaNB=" 
		      << etaNB);
	return false;      
      } else {
	return true;
      }
    }
  // check if size is etaTRTNB
  case 2:
    {
      const unsigned int etaTRTNB = CutBinEta_TRT.size();      
      if ( vec.size() != etaTRTNB ) {
	FAKE_MSG_ERROR("vector size is " 
		      << vec.size() << " but needs etaTRTNB=" 
		      << etaTRTNB);
	return false;      
      } else {
	return true;
      }
    }

  // check if size is etNB
  case 3:
    {
      const unsigned int etNB =  CutBinET.size();
      if ( vec.size() != etNB ) {
	FAKE_MSG_ERROR("vector size is " 
		      << vec.size() << " but needs etNB=" 
		      << etNB);
	return false;      
      } else {
	return true;
      }
    }
  // check if size is combinedNB 
  case 4:
    {
      const unsigned int etaNB = CutBinEta.size();
      const unsigned int etNB =  CutBinET.size();
      unsigned int combinedNB = 0;
      if (etNB>1) 
	combinedNB = 
	  etaNB * (etNB+1);
      else 
	combinedNB = 
	  etaNB;
      
      if ( vec.size() != combinedNB ) {
	FAKE_MSG_ERROR("vector size is " 
		      << vec.size() << " but needs combinedNB=" 
		      << combinedNB);
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
