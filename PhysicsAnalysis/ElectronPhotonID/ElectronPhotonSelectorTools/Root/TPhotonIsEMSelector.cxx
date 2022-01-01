/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**  TPhotonIsEMSelector.cxx
*
*  Original by  Jovan Mitrevski (Feb. 2012)
*  Modified by Roger Naranjo
*
*  Package: PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools 
*  
*  This class in intented to apply a cut based identification criteria to photons
*
*
**/
#include "TPhotonIsEMSelector.h"
#include <cmath>

Root::TPhotonIsEMSelector::TPhotonIsEMSelector(const char *name) :
  asg::AsgMessaging(std::string(name)),
  m_isEMMask(0),//All will pass if not defined
  m_forceConvertedPhotonPID(false),
  m_forceNonConvertedPhotonPID(false),
  /** @brief cluster eta range */
  m_cutPositionClusterEtaRange_Photon(0),
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
  /** @brief energy fraction in the third layer */
  m_cutNameClusterBackEnergyFraction_Photon("ClusterBackEnergyFraction_Photon"),
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
  m_cutNameTrackIsolation_Photon("TrackIsolation_Photon") {
  m_cutPositionClusterEtaRange_Photon = 0;
  m_cutPositionClusterBackEnergyFraction_Photon = 0;
  m_cutPositionClusterHadronicLeakage_Photon = 0;
  m_cutPositionClusterMiddleEnergy_Photon = 0;
  m_cutPositionClusterMiddleEratio37_Photon = 0;
  m_cutPositionClusterMiddleEratio33_Photon = 0;
  m_cutPositionClusterMiddleWidth_Photon = 0;
  m_cutPositionClusterStripsEratio_Photon = 0;
  m_cutPositionClusterStripsDeltaE_Photon = 0;
  m_cutPositionClusterStripsWtot_Photon = 0;
  m_cutPositionClusterStripsFracm_Photon = 0;
  m_cutPositionClusterStripsWeta1c_Photon = 0;
  m_cutPositionClusterStripsDEmaxs1_Photon = 0;
  m_cutPositionTrackMatchEoverP_Photon = 0;
  m_cutPositionAmbiguityResolution_Photon = 0;
  m_cutPositionIsolation_Photon = 0;
  m_cutPositionClusterIsolation_Photon = 0;
  m_cutPositionTrackIsolation_Photon = 0;
}

// =================================================================
Root::TPhotonIsEMSelector::~TPhotonIsEMSelector() {
}

/** The initialization of this tool registers all applied cuts
    and make sure that they got registered
    
    The order of the cuts does matter since each cut corresponds
    to a bit, i.e, Cut0 corresponds to bit 0, Cut1 to bit 1, etc.
*/
StatusCode Root::TPhotonIsEMSelector::initialize() {

  StatusCode sc(StatusCode::SUCCESS);

  /** @brief cluster eta range, bit 0*/
  m_cutPositionClusterEtaRange_Photon =
    m_acceptInfo.addCut(m_cutNameClusterEtaRange_Photon, "Photon within eta range");
  if (m_cutPositionClusterEtaRange_Photon < 0) sc = StatusCode::FAILURE;

  int voidcutpos = m_acceptInfo.addCut("VOID1", "No Cut"); // bit 1 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID2", "No Cut"); // bit 2 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID3", "No Cut"); // bit 3 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID4", "No Cut"); // bit 4 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID5", "No Cut"); // bit 5 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID6", "No Cut"); // bit 6 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  /** @brief energy fraction in the third layer (f3), bit 7 */
  m_cutPositionClusterBackEnergyFraction_Photon =
    m_acceptInfo.addCut(m_cutNameClusterBackEnergyFraction_Photon, "f3 < Cut");
  if (m_cutPositionClusterBackEnergyFraction_Photon < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID7", "No Cut"); // bit 8 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID8", "No Cut"); // bit 9 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  /** @brief cluster leakage into the hadronic calorimeter, bit 10 */
  m_cutPositionClusterHadronicLeakage_Photon =
    m_acceptInfo.addCut(m_cutNameClusterHadronicLeakage_Photon, "Had leakage < Cut");
  if (m_cutPositionClusterHadronicLeakage_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief energy in 2nd sampling (e277), bit 11 */
  m_cutPositionClusterMiddleEnergy_Photon =
    m_acceptInfo.addCut(m_cutNameClusterMiddleEnergy_Photon, "Energy in second sampling (E277) > Cut");
  if (m_cutPositionClusterMiddleEnergy_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief energy ratio in 2nd sampling, bit 12 */
  m_cutPositionClusterMiddleEratio37_Photon =
    m_acceptInfo.addCut(m_cutNameClusterMiddleEratio37_Photon, "E237/E277 > Cut");
  if (m_cutPositionClusterMiddleEratio37_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief energy ratio in 2nd sampling for photons, bit 13 */
  m_cutPositionClusterMiddleEratio33_Photon =
    m_acceptInfo.addCut(m_cutNameClusterMiddleEratio33_Photon, "E233/E237 > Cut");
  if (m_cutPositionClusterMiddleEratio33_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief width in the second sampling, bit 14 */
  m_cutPositionClusterMiddleWidth_Photon =
    m_acceptInfo.addCut(m_cutNameClusterMiddleWidth_Photon, "Weta2 < Cut");
  if (m_cutPositionClusterMiddleWidth_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief fraction of energy found in 1st sampling, bit 15 */
  m_cutPositionClusterStripsEratio_Photon =
    m_acceptInfo.addCut(m_cutNameClusterStripsEratio_Photon, "f1 > Cut");
  if (m_cutPositionClusterStripsEratio_Photon < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID9", "No Cut"); // bit 16 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1), bit 17 */
  m_cutPositionClusterStripsDeltaE_Photon =
    m_acceptInfo.addCut(m_cutNameClusterStripsDeltaE_Photon,
                        "difference between 2nd maximum and 1st minimum in strips < Cut");
  if (m_cutPositionClusterStripsDeltaE_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief shower width in 1st sampling, bit 18 */
  m_cutPositionClusterStripsWtot_Photon =
    m_acceptInfo.addCut(m_cutNameClusterStripsWtot_Photon, "Wtot < Cut");
  if (m_cutPositionClusterStripsWtot_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief shower shape in shower core 1st sampling, bit 19 */
  m_cutPositionClusterStripsFracm_Photon =
    m_acceptInfo.addCut(m_cutNameClusterStripsFracm_Photon, "Fracm < Cut");
  if (m_cutPositionClusterStripsFracm_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief shower width weighted by distance from the maximum one, bit 20 */
  m_cutPositionClusterStripsWeta1c_Photon =
    m_acceptInfo.addCut(m_cutNameClusterStripsWeta1c_Photon, "Weta1c < Cut");
  if (m_cutPositionClusterStripsWeta1c_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief difference between max and 2nd max in strips, bit 21 */
  m_cutPositionClusterStripsDEmaxs1_Photon =
    m_acceptInfo.addCut(m_cutNameClusterStripsDEmaxs1_Photon,
                        "difference between max and 2nd max in strips > Cut");
  if (m_cutPositionClusterStripsDEmaxs1_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief energy-momentum match for photon selection, bit 22 */
  m_cutPositionTrackMatchEoverP_Photon =
    m_acceptInfo.addCut(m_cutNameTrackMatchEoverP_Photon, "E/p within allowed range (conversions only)");
  if (m_cutPositionTrackMatchEoverP_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief ambiguity resolution for photon (vs electron), bit 23 */
  m_cutPositionAmbiguityResolution_Photon =
    m_acceptInfo.addCut(m_cutNameAmbiguityResolution_Photon,
                        "Passes tighter ambiguity resolution vs electron");
  if (m_cutPositionAmbiguityResolution_Photon < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID10", "No Cut"); // bit 24 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID11", "No Cut"); // bit 25 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID12", "No Cut"); // bit 26 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID13", "No Cut"); // bit 27 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  voidcutpos = m_acceptInfo.addCut("VOID14", "No Cut"); // bit 28 is not used
  if (voidcutpos < 0) sc = StatusCode::FAILURE;

  /** @brief isolation, bit 29 */
  m_cutPositionIsolation_Photon =
    m_acceptInfo.addCut(m_cutNameIsolation_Photon, "Track and calorimetric isolation");
  if (m_cutPositionIsolation_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief calorimetric isolation for photon selection, bit 30 */
  m_cutPositionClusterIsolation_Photon =
    m_acceptInfo.addCut(m_cutNameClusterIsolation_Photon, "calorimetric isolation only");
  if (m_cutPositionClusterIsolation_Photon < 0) sc = StatusCode::FAILURE;

  /** @brief tracker isolation for photon selection, bit 31 */
  m_cutPositionTrackIsolation_Photon =
    m_acceptInfo.addCut(m_cutNameTrackIsolation_Photon, "track isolation only");
  if (m_cutPositionTrackIsolation_Photon < 0) sc = StatusCode::FAILURE;

  if (sc == StatusCode::FAILURE) {
    ATH_MSG_ERROR("Exceeded the number of allowed cuts in TPhotonIsEMSelector");
  }

  return sc;
}

asg::AcceptData Root::TPhotonIsEMSelector::fillAccept(unsigned int isEM) const {
  asg::AcceptData acceptData(&m_acceptInfo);
  for (int i = 0; i < 32; i++) {
    const unsigned int mask = (0x1 << i) & m_isEMMask;
    acceptData.setCutResult(i, (isEM & mask) == 0);
  }

  return acceptData;
}


/** Method to calculate the actual accept of each cut individually.
  * @param shower_shapes each shower shape
  * @param isConverted   appy cuts to converted or unconverted photons
  * @return AcceptData
*/
asg::AcceptData
Root::TPhotonIsEMSelector::accept(
  // eta position in second sampling
  float eta2,
  // transverse energy in calorimeter (using eta position in second sampling)
  double et,
  // transverse energy in 1st scintillator of hadronic calorimeter/ET
  float Rhad1,
  // transverse energy in hadronic calorimeter/ET
  float Rhad,
  // E(7*7) in 2nd sampling
  float e277,
  // E(3*7)/E(7*7) in 2nd sampling
  float Reta,
  // E(3*3)/E(3*7) in 2nd sampling
  float Rphi,
  // shower width in 2nd sampling
  float weta2c,
  // fraction of energy reconstructed in strips
  float f1,
  // (E of 1st max in strips-E of 2nd max)/(E of 1st max+E of 2nd max)
  float Eratio,
  // E(2nd max)-E(min) in strips
  float DeltaE,
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
  bool isConversion) {

  // Do the actual selection

  unsigned int isEM = calcIsEm(eta2,
                               et,
                               Rhad1,
                               Rhad,
                               e277,
                               Reta,
                               Rphi,
                               weta2c,
                               f1,
                               Eratio,
                               DeltaE,
                               weta1c,
                               wtot,
                               fracm,
                               f3,
                               ep,
                               isConversion);

  return fillAccept(isEM);
}


//=============================================================================
// calculate the isEM. (Used internally by accept)
//=============================================================================
unsigned int Root::TPhotonIsEMSelector::calcIsEm(
  // eta position in second sampling
  float eta2,
  // transverse energy in calorimeter (using eta position in second sampling)
  double et,
  // transverse energy in 1st scintillator of hadronic calorimeter/ET
  float Rhad1,
  // transverse energy in hadronic calorimeter/ET
  float Rhad,
  // E(7*7) in 2nd sampling
  float e277,
  // E(3*7)/E(7*7) in 2nd sampling
  float Reta,
  // E(3*3)E(3*7) in 2nd sampling
  float Rphi,
  // shower width in 2nd sampling
  float weta2c,
  // fraction of energy reconstructed in strips
  float f1,
  // (E of 1st max in strips-E of 2nd max)/(E of 1st max+E of 2nd max)
  float Eratio,
  // E(2nd max)-E(min) in strips
  float DeltaE,
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
  bool isConversion) const {
  unsigned int iflag = 0;

  // apply calorimeter selection for photons
  if (m_forceConvertedPhotonPID) {
    // force to test converted photon hypothesis
    iflag = calocuts_photonsConverted(eta2,
                                      et,
                                      Rhad1,
                                      Rhad,
                                      e277,
                                      Reta,
                                      Rphi,
                                      weta2c,
                                      f1,
                                      Eratio,
                                      DeltaE,
                                      weta1c,
                                      wtot,
                                      fracm,
                                      f3,
                                      ep,
                                      iflag);

  } else if (m_forceNonConvertedPhotonPID || !isConversion) {
    iflag = calocuts_photonsNonConverted(eta2,
                                         et,
                                         Rhad1,
                                         Rhad,
                                         e277,
                                         Reta,
                                         Rphi,
                                         weta2c,
                                         f1,
                                         Eratio,
                                         DeltaE,
                                         weta1c,
                                         wtot,
                                         fracm,
                                         f3,
                                         iflag);
  } else {
    iflag = calocuts_photonsConverted(eta2,
                                      et,
                                      Rhad1,
                                      Rhad,
                                      e277,
                                      Reta,
                                      Rphi,
                                      weta2c,
                                      f1,
                                      Eratio,
                                      DeltaE,
                                      weta1c,
                                      wtot,
                                      fracm,
                                      f3,
                                      ep,
                                      iflag);
  }


  return iflag;
}


/** Method that applies the selection to unconverted photons 
 *  @params Shower Shapes
 *  @return unsigned int with the bits that tells if the cuts passed
*/
unsigned int Root::TPhotonIsEMSelector::calocuts_photonsNonConverted(
  // eta position in second sampling
  float eta2,
  // transverse energy in calorimeter
  double et,
  // hadronic leakage ratios
  float Rhad1,
  float Rhad,
  // E(7*7) in 2nd sampling
  float e277,
  // ratios
  float Reta,
  float Rphi,
  // shower width in 2nd sampling
  float weta2c,
  // fraction of energy reconstructed in strips
  float f1,
  // (Emax1-Emax2)/(Emax1+Emax2)
  float Eratio,
  // difference of energy between max and min
  float DeltaE,
  // shower width in 3 strips in 1st sampling
  float weta1c,
  // total shower width in strips
  float wtot,
  // E(+/-3)-E(+/-1)/E(+/-1)
  float fracm,
  // fraction of energy reconstructed in the 3rd sampling
  float f3,
  unsigned int iflag) const {
  //
  // second sampling cuts
  //
  if ((m_e277_photonsNonConverted.empty())) {
    ATH_MSG_WARNING("e277 needs to  be set ");
  }
  if (!m_e277_photonsNonConverted.empty() && e277 >= m_e277_photonsNonConverted[0]) {
   int ibine = 0;
    // loop on ET range
    for (unsigned int ibe = 1; ibe <= m_cutBinEnergy_photonsNonConverted.size();
         ibe++) {
      if (ibe < m_cutBinEnergy_photonsNonConverted.size()) {
        if (et >= m_cutBinEnergy_photonsNonConverted[ibe - 1] &&
            et < m_cutBinEnergy_photonsNonConverted[ibe]) {
          ibine = ibe;
        }
      } else if (ibe == m_cutBinEnergy_photonsNonConverted.size()) {
        if (et >= m_cutBinEnergy_photonsNonConverted[ibe - 1]) {
          ibine = ibe;
        }
      }
    }

    int ibinEta = -1;
    // loop on eta range
    for (unsigned int ibin = 0; ibin < m_cutBinEta_photonsNonConverted.size();
         ibin++) {
      if (ibin == 0) {
        if (eta2 < m_cutBinEta_photonsNonConverted[0]) {
          ibinEta = 0;
        }
      } else {
        if (eta2 >= m_cutBinEta_photonsNonConverted[ibin - 1] &&
            eta2 < m_cutBinEta_photonsNonConverted[ibin]) {
          ibinEta = ibin;
        }
      }
    }

    if (ibinEta < 0) {
      iflag |= (0x1 << egammaPID::ClusterEtaRange_Photon);
      return iflag;
    }

    // check the bin number
    const int ibin_combined = ibine * m_cutBinEta_photonsNonConverted.size() + ibinEta;
    
    // hadronic leakage
    if (checkVar(m_cutHadLeakage_photonsNonConverted, 23)) {
      if (eta2 < 0.8) {
        if (Rhad1 > m_cutHadLeakage_photonsNonConverted[ibin_combined])
          iflag |= (0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      } else if (eta2 >= 0.8 && eta2 < 1.37) {
        if (Rhad > m_cutHadLeakage_photonsNonConverted[ibin_combined])
          iflag |= (0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      } else {
        if (Rhad1 > m_cutHadLeakage_photonsNonConverted[ibin_combined])
          iflag |= (0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      }
    }

    // F3
    if (checkVar(m_cutF3_photonsNonConverted, 23)) {
      if (f3 > m_cutF3_photonsNonConverted[ibin_combined]) {
        iflag |= (0x1 << egammaPID::ClusterBackEnergyFraction_Photon);
      }
    }

    // E237/E277
    if (checkVar(m_Reta37_photonsNonConverted, 23)) {
      if (Reta < m_Reta37_photonsNonConverted[ibin_combined]) {
        iflag |= (0x1 << egammaPID::ClusterMiddleEratio37_Photon);
      }
    }

    // E233/E237
    if (checkVar(m_Rphi33_photonsNonConverted, 23)) {
      if (Rphi < m_Rphi33_photonsNonConverted[ibin_combined]) {
        iflag |= (0x1 << egammaPID::ClusterMiddleEratio33_Photon);
      }
    }

    // width in 2n sampling
    if (checkVar(m_weta2_photonsNonConverted, 23)) {
      if (weta2c > m_weta2_photonsNonConverted[ibin_combined]) {
        iflag |= (0x1 << egammaPID::ClusterMiddleWidth_Photon);
      }
    }
  } else {
    iflag |= (0x1 << egammaPID::ClusterMiddleEnergy_Photon);
  }

  //
  // first sampling cuts
  //
  if (!m_cutBinEtaStrips_photonsNonConverted.empty()) {

    int ibineStrips = 0;
    // loop on ET range
    for (unsigned int ibe = 1; ibe <= m_cutBinEnergyStrips_photonsNonConverted.size(); ibe++) {
      if (ibe < m_cutBinEnergyStrips_photonsNonConverted.size()) {
        if (et >= m_cutBinEnergyStrips_photonsNonConverted[ibe - 1] &&
            et < m_cutBinEnergyStrips_photonsNonConverted[ibe]) {
          ibineStrips = ibe;
        }
      } else if (ibe == m_cutBinEnergyStrips_photonsNonConverted.size()) {
        if (et >= m_cutBinEnergyStrips_photonsNonConverted[ibe - 1]) {
          ibineStrips = ibe;
        }
      }
    }

    int ibinEtaStrips = -1;
    // loop on eta range
    for (unsigned int ibin = 0; ibin < m_cutBinEtaStrips_photonsNonConverted.size(); ibin++) {
      if (ibin == 0) {
        if (eta2 < m_cutBinEtaStrips_photonsNonConverted[0]) {
          ibinEtaStrips = 0;
        }
      } else {
        if (eta2 >= m_cutBinEtaStrips_photonsNonConverted[ibin - 1] &&
            eta2 < m_cutBinEtaStrips_photonsNonConverted[ibin]) {
          ibinEtaStrips = ibin;
        }
      }
    }

    // check the bin number
    if (ibinEtaStrips == -1) {
      iflag |= (0x1 << egammaPID::ClusterEtaRange_Photon);
      return iflag;
    }

    const int ibin_combinedStrips = ibineStrips * m_cutBinEtaStrips_photonsNonConverted.size() + ibinEtaStrips;

    if (checkVar(m_f1_photonsNonConverted, 0)) {
      if (f1 < m_f1_photonsNonConverted[0]) {
        iflag |= (0x1 << egammaPID::ClusterStripsEratio_Photon);
      }
    }

    // Delta E
    if (checkVar(m_deltae_photonsNonConverted, 26)) {
      if (DeltaE > m_deltae_photonsNonConverted[ibin_combinedStrips]) {
        iflag |= (0x1 << egammaPID::ClusterStripsDeltaE_Photon);
      }
    }

    // Eratio
    if (checkVar(m_DEmaxs1_photonsNonConverted, 26)) {
      if (Eratio <= m_DEmaxs1_photonsNonConverted[ibin_combinedStrips])
        iflag |= (0x1 << egammaPID::ClusterStripsDEmaxs1_Photon);
    }

    // total width in strips
    if (checkVar(m_wtot_photonsNonConverted, 26)) {
      if (wtot > m_wtot_photonsNonConverted[ibin_combinedStrips]) {
        iflag |= (0x1 << egammaPID::ClusterStripsWtot_Photon);
      }
    }

    // (E(+/-3)-E(+/-1))/E(+/-1)
    if (checkVar(m_fracm_photonsNonConverted, 26)) {
      if (fracm > m_fracm_photonsNonConverted[ibin_combinedStrips]) {
        iflag |= (0x1 << egammaPID::ClusterStripsFracm_Photon);
      }
    }

    // width in 3 strips
    if (checkVar(m_w1_photonsNonConverted, 26)) {
      if (weta1c > m_w1_photonsNonConverted[ibin_combinedStrips]) {
        iflag |= (0x1 << egammaPID::ClusterStripsWeta1c_Photon);
      }
    }
  }
  return iflag;
}

/** Method that applies the selection to converted photons
 *  @params Shower Shapes
 *  @return unsigned int with the bits that tells if the cuts passed
 * 
 */
unsigned int Root::TPhotonIsEMSelector::calocuts_photonsConverted(
  // eta position in second sampling
  float eta2,
  // transverse energy in calorimeter
  double et,
  // hadronic leakage ratios
  float Rhad1,
  float Rhad,
  // E(7*7) in 2nd sampling
  float e277,
  // ratios
  float Reta,
  float Rphi,
  // shower width in 2nd sampling
  float weta2c,
  // fraction of energy reconstructed in strips
  float f1,
  // (Emax1-Emax2)/(Emax1+Emax2)
  float Eratio,
  // difference of energy between max and min
  float DeltaE,
  // parametrization of E(2nd max)
  // float deltaemax2,
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
  unsigned int iflag) const {
  int ibine = 0;
  // loop on ET range
  for (unsigned int ibe = 1; ibe <= m_cutBinEnergy_photonsConverted.size(); ibe++) {
    if (ibe < m_cutBinEnergy_photonsConverted.size()) {
      if (et >= m_cutBinEnergy_photonsConverted[ibe - 1] &&
          et < m_cutBinEnergy_photonsConverted[ibe]) {
        ibine = ibe;
      }
    } else if (ibe == m_cutBinEnergy_photonsConverted.size()) {
      if (et >= m_cutBinEnergy_photonsConverted[ibe - 1]) {
        ibine = ibe;
      }
    }
  }

  int ibinEta = -1;
  // loop on eta range
  for (unsigned int ibin = 0; ibin < m_cutBinEta_photonsConverted.size(); ibin++) {
    if (ibin == 0) {
      if (eta2 < m_cutBinEta_photonsConverted[0]) {
        ibinEta = 0;
      }
    } else {
      if (eta2 >= m_cutBinEta_photonsConverted[ibin - 1] &&
          eta2 < m_cutBinEta_photonsConverted[ibin]) {
        ibinEta = ibin;
      }
    }
  }

  // check the bin number
  const int ibin_combined = ibine * m_cutBinEta_photonsConverted.size() + ibinEta;

  //
  // second sampling cuts
  //
  if ((m_e277_photonsConverted.empty())) {
    ATH_MSG_WARNING("e277 needs to  be set ");
  }
  if (!m_e277_photonsConverted.empty() && e277 >= m_e277_photonsConverted[0]) {

    if (ibinEta < 0) {
      iflag |= (0x1 << egammaPID::ClusterEtaRange_Photon);
      return iflag;
    }

    // hadronic leakage
    if (checkVar(m_cutHadLeakage_photonsConverted, 13)) {
      if (eta2 < 0.8) {
        if (Rhad1 > m_cutHadLeakage_photonsConverted[ibin_combined])
          iflag |= (0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      } else if (eta2 >= 0.8 && eta2 < 1.37) {
        if (Rhad > m_cutHadLeakage_photonsConverted[ibin_combined])
          iflag |= (0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      } else {
        if (Rhad1 > m_cutHadLeakage_photonsConverted[ibin_combined])
          iflag |= (0x1 << egammaPID::ClusterHadronicLeakage_Photon);
      }
    }

    // F3
    if (checkVar(m_cutF3_photonsConverted, 13)) {
      if (f3 > m_cutF3_photonsConverted[ibin_combined]) {
        iflag |= (0x1 << egammaPID::ClusterBackEnergyFraction_Photon);
      }
    }

    // E237/E277
    if (checkVar(m_Reta37_photonsConverted, 13)) {
      if (Reta < m_Reta37_photonsConverted[ibin_combined]) {
        iflag |= (0x1 << egammaPID::ClusterMiddleEratio37_Photon);
      }
    }

    // E233/E237
    if (checkVar(m_Rphi33_photonsConverted, 13)) {
      if (Rphi < m_Rphi33_photonsConverted[ibin_combined]) {
        iflag |= (0x1 << egammaPID::ClusterMiddleEratio33_Photon);
      }
    }

    // width in 2n sampling
    if (checkVar(m_weta2_photonsConverted, 13)) {
      if (weta2c > m_weta2_photonsConverted[ibin_combined]) {
        iflag |= (0x1 << egammaPID::ClusterMiddleWidth_Photon);
      }
    }
  } else {
    iflag |= (0x1 << egammaPID::ClusterMiddleEnergy_Photon);
  }

  //
  // first sampling cuts
  //
  if (!m_cutBinEtaStrips_photonsConverted.empty()) {

    int ibineStrips = 0;
    // loop on ET range
    for (unsigned int ibe = 1; ibe <= m_cutBinEnergyStrips_photonsConverted.size(); ibe++) {
      if (ibe < m_cutBinEnergyStrips_photonsConverted.size()) {
        if (et >= m_cutBinEnergyStrips_photonsConverted[ibe - 1] &&
            et < m_cutBinEnergyStrips_photonsConverted[ibe]) {
          ibineStrips = ibe;
        }
      } else if (ibe == m_cutBinEnergyStrips_photonsConverted.size()) {
        if (et >= m_cutBinEnergyStrips_photonsConverted[ibe - 1]) {
          ibineStrips = ibe;
        }
      }
    }

    int ibinEtaStrips = -1;
    // loop on eta range
    for (unsigned int ibin = 0; ibin < m_cutBinEtaStrips_photonsConverted.size(); ibin++) {
      if (ibin == 0) {
        if (eta2 < m_cutBinEtaStrips_photonsConverted[0]) {
          ibinEtaStrips = 0;
        }
      } else {
        if (eta2 >= m_cutBinEtaStrips_photonsConverted[ibin - 1] &&
            eta2 < m_cutBinEtaStrips_photonsConverted[ibin]) {
          ibinEtaStrips = ibin;
        }
      }
    }

    // check the bin number
    if (ibinEtaStrips == -1) {
      iflag |= (0x1 << egammaPID::ClusterEtaRange_Photon);
      return iflag;
    }

    const int ibin_combinedStrips = ibineStrips * m_cutBinEtaStrips_photonsConverted.size() + ibinEtaStrips;

    if (checkVar(m_f1_photonsConverted, 0)) {
      if (f1 < m_f1_photonsConverted[0]) {
        iflag |= (0x1 << egammaPID::ClusterStripsEratio_Photon);
      }
    }

    // Delta E
    if (checkVar(m_deltae_photonsConverted, 16)) {
      if (DeltaE > m_deltae_photonsConverted[ibin_combinedStrips]) {
        iflag |= (0x1 << egammaPID::ClusterStripsDeltaE_Photon);
      }
    }

    // Eratio
    if (checkVar(m_DEmaxs1_photonsConverted, 16)) {
      if (Eratio <= m_DEmaxs1_photonsConverted[ibin_combinedStrips])
        iflag |= (0x1 << egammaPID::ClusterStripsDEmaxs1_Photon);
    }

    // total width in strips
    if (checkVar(m_wtot_photonsConverted, 16)) {
      if (wtot > m_wtot_photonsConverted[ibin_combinedStrips]) {
        iflag |= (0x1 << egammaPID::ClusterStripsWtot_Photon);
      }
    }

    // (E(+/-3)-E(+/-1))/E(+/-1)
    if (checkVar(m_fracm_photonsConverted, 16)) {
      if (fracm > m_fracm_photonsConverted[ibin_combinedStrips]) {
        iflag |= (0x1 << egammaPID::ClusterStripsFracm_Photon);
      }
    }

    // width in 3 strips
    if (checkVar(m_w1_photonsConverted, 16)) {
      if (weta1c > m_w1_photonsConverted[ibin_combinedStrips]) {
        iflag |= (0x1 << egammaPID::ClusterStripsWeta1c_Photon);
      }
    }
  }

  // cut on E/p
  //
  if (checkVar(m_cutminEp_photonsConverted, 13) &&
      checkVar(m_cutmaxEp_photonsConverted, 13)) {
    if (ep < m_cutminEp_photonsConverted[ibin_combined] ||
        ep > m_cutmaxEp_photonsConverted[ibin_combined])
      iflag |= (0x1 << egammaPID::TrackMatchEoverP_Photon);
  }

  return iflag;
}

/** Method that check vector size
 *
 * 0 : size should be 1
 *
 * 11 : vs etaNB_photonsConv
 * 12 : vs etNB_photonsConv
 * 13 : vs combinedNB_photonsConv
 * 14 : vs etaStripsNB_photonsConv
 * 15 : vs etStripsNB_photonsConv
 * 16 : combinedStripsNB_photonsConv
 * 21 : vs etaNB_photonsNonConv
 * 22 : vs etNB_photonsNonConv
 * 23 : vs combinedNB_photonsNonConv
 * 24 : vs etaStripsNB_photonsNonConv
 * 25 : vs etStripsNB_photonsNonConv
 * 26 : combinedStripsNB_photonsNonConv
 *  @params Vector, choice to check
 *  @return boolean
*/
template<typename T>
bool Root::TPhotonIsEMSelector::checkVar(const std::vector <T> &vec, int choice) const {
  // check vector size


  const unsigned int etaNB_photonsConv = m_cutBinEta_photonsConverted.size();
  const unsigned int etNB_photonsConv = m_cutBinEnergy_photonsConverted.size();
  const unsigned int etaStripsNB_photonsConv = m_cutBinEtaStrips_photonsConverted.size();
  const unsigned int etStripsNB_photonsConv = m_cutBinEnergyStrips_photonsConverted.size();
  const unsigned int etaNB_photonsNonConv = m_cutBinEta_photonsNonConverted.size();
  const unsigned int etNB_photonsNonConv = m_cutBinEnergy_photonsNonConverted.size();
  const unsigned int etaStripsNB_photonsNonConv = m_cutBinEtaStrips_photonsNonConverted.size();
  const unsigned int etStripsNB_photonsNonConv = m_cutBinEnergyStrips_photonsNonConverted.size();

  unsigned int combinedStripsNB_photonsConv = etaStripsNB_photonsConv;
  unsigned int combinedStripsNB_photonsNonConv = etaStripsNB_photonsNonConv;
  unsigned int combinedNB_photonsNonConv = etaNB_photonsNonConv;
  unsigned int combinedNB_photonsConv = etaNB_photonsConv;

  if (etStripsNB_photonsNonConv > 0)
    combinedStripsNB_photonsNonConv = etaStripsNB_photonsNonConv * (etStripsNB_photonsNonConv + 1);

  if (etNB_photonsConv > 0)
    combinedNB_photonsConv = etaNB_photonsConv * (etNB_photonsConv + 1);

  if (etStripsNB_photonsConv > 0)
    combinedStripsNB_photonsConv = etaStripsNB_photonsConv * (etStripsNB_photonsConv + 1);

  if (etNB_photonsNonConv > 0)
    combinedNB_photonsNonConv = etaNB_photonsNonConv * (etNB_photonsNonConv + 1);


  // if size of vector is 0 it means cut is not defined
  if (vec.empty()) return false;
  // check if size is 1
  if (choice == 0) {
    if (vec.size() != 1) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs 1");
      return false;
    }
  }

  // check if size is etaNB_photonsConv
  if (choice == 11) {

    if (vec.size() != etaNB_photonsConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs etaNB_photonsConv "
                      << etaNB_photonsConv);
      return false;

    }
  }

  // check if size is etNB_photonsConv
  if (choice == 12) {
    if (vec.size() != etNB_photonsConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs etNB_photonsConv="
                      << etNB_photonsConv);
      return false;
    }
  }

  // check if size is combinedNB_photonsConv
  if (choice == 13) {
    if (vec.size() != combinedNB_photonsConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs combinedNB_photonsConv="
                      << combinedNB_photonsConv);
      return false;
    }
  }

  // check if size is etaStripsNB_photonsConv
  if (choice == 14) {
    if (vec.size() != etaStripsNB_photonsConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs etaStripsNB_photonsConv="
                      << etaStripsNB_photonsConv);
      return false;
    }
  }

  // check if size is etStripsNB_photonsConv
  if (choice == 15) {
    if (vec.size() != etStripsNB_photonsConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs etStripsNB_photonsConv="
                      << etStripsNB_photonsConv);
      return false;
    }
  }

  // check if size is combinedStripsNB_photonsConv
  if (choice == 16) {
    if (vec.size() != combinedStripsNB_photonsConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs combinedStripsNB_photonsConv="
                      << combinedStripsNB_photonsConv);
      return false;
    }
  }

  // check if size is etaNB_photonsNonConv
  if (choice == 21) {
    if (vec.size() != etaNB_photonsNonConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs etaNB_photonsNonConv "
                      << etaNB_photonsNonConv);
      return false;
    }
  }

  // check if size is etNB_photonsNonConv
  if (choice == 22) {
    if (vec.size() != etNB_photonsNonConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs etNB_photonsNonConv="
                      << etNB_photonsNonConv);
      return false;
    }
  }

  // check if size is combinedNB_photonsNonConv
  if (choice == 23) {
    if (vec.size() != combinedNB_photonsNonConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs combinedNB_photonsNonConv="
                      << combinedNB_photonsNonConv);
      return false;
    }
  }

  // check if size is etaStripsNB_photonsNonConv
  if (choice == 24) {
    if (vec.size() != etaStripsNB_photonsNonConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs etaStripsNB_photonsNonConv="
                      << etaStripsNB_photonsNonConv);
      return false;
    }
  }

  // check if size is etStripsNB_photonsNonConv
  if (choice == 25) {
    if (vec.size() != etStripsNB_photonsNonConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs etStripsNB_photonsNonConv="
                      << etStripsNB_photonsNonConv);
      return false;
    }
  }

  // check if size is combinedStripsNB_photonsNonConv
  if (choice == 26) {
    if (vec.size() != combinedStripsNB_photonsNonConv) {
      ATH_MSG_ERROR("vector size is "
                      << vec.size() << " but needs combinedStripsNB_photonsNonConv="
                      << combinedStripsNB_photonsNonConv);
      return false;
    }
  }

  return true;


}

template bool Root::TPhotonIsEMSelector::checkVar<float>(const std::vector<float> &vec, int choice) const;

template bool Root::TPhotonIsEMSelector::checkVar<int>(const std::vector<int> &vec, int choice) const;
