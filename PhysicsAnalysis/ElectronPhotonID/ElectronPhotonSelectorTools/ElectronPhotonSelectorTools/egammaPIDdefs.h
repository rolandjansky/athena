/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _EGAMMAPIDDEFS_H
#define _EGAMMAPIDDEFS_H
/**
  @class egammaPIDdefs
     This file defines the bit-occupation of the PID and OQ flags of egamma 
     and the enum definition of the identification results

     The enum PID contains variables related to the particle identification
     such as IsEM (cut-based selection) and Multivariate analysis.
     It contains also the word IsGoodOQ which is related to the Object quality

     The IsEM variable is coded through bit mask, each of the 32 bits
     of this variable (coded as an unsigned integer) has a meaning. 
     Two enums exist to determine the meaning of each bit, one for electrons
     and one for photons
      BitDefElectron : definition of the 32 bits defined to identify an electron
      BitDefPhoton : definition of the 32 bits defined to identify a photon

      Note that the enum BitDef is obsolete and kept for files reconstructed before rel 15.2.0
      Following these enums one can find variables which combine these
      bits to obtain different levels of identification, such as ElectronLoose, PhotonTight etc.

      The concrete use of the bits for IsEM is done in 
          egammaElectronCutIDTool.cxx : the cut-based selection for electrons
          egammaElectronCutIDToolBase.py : cut thresholds which are applied for the different menus
          egammaPhotonCutIDTool.cxx : the cut-based selection for photons
          egammaPhotonCutIDToolBase.py : cut thresholds which are applied for the different menus
      In particular it means that even if a bit is defined and enters in the 
      definition of a selection level, it does not mean that the cut is applied in practice as its threshold could have a dummy value

      The concrete use of the bits for IsGoodOQ is done in
         egammaOQFlagsBuilder.cxx : tool to fill Object Quality flags

      UPDATED:
         Jan  5, 2011: add IsEMLoose, IsEMMedium, and IsEMTight PIDs; change
                       EgPidUndefined to be an unsigned int with UINT_MAX;
         May  9, 2011: Change EgPidUndefined back to 0.
         May 31, 2011: Change EgPidUndefined back to UINT_MAX. Add flag on whether
	               the variable was found. Add ++ definitions
   @author Dirk Zerwas
*/

#include <climits>

namespace egammaPID { 
  
  enum egammaIDQuality {
    ElectronIDLoose,
    ElectronIDLoosePP,
    ElectronIDMedium,
    ElectronIDMediumPP,
    ElectronIDTight,
    ElectronIDTightPP,
    PhotonIDLoose,
    PhotonIDTight,
    PhotonIDLooseAR,
    PhotonIDTightAR,
    ElectronIDLooseIso,
    ElectronIDLoosePPIso,
    ElectronIDMediumIso,
    ElectronIDMediumPPIso,
    ElectronIDTightIso,
    ElectronIDTightPPIso,
    PhotonIDLooseIso,
    PhotonIDTightIso,
    PhotonIDLooseARIso,
    PhotonIDTightARIso,
    ForwardElectronIDLoose,
    ForwardElectronIDMedium,
    ForwardElectronIDTight,
    NoIDCut,
    PhotonIDMedium,
    PhotonIDMediumAR,
    ElectronIDLoose1,
    ElectronIDMedium1,
    ElectronIDTight1,
    ElectronIDLooseHLT,
    ElectronIDMediumHLT,
    ElectronIDTightHLT,
    PhotonIDLooseEF,
    PhotonIDMediumEF
  };
    
  //const double EgPidUndefined = -999.;
  //const unsigned int EgPidUndefined = 0.;
  const unsigned int EgPidUndefined = UINT_MAX;

  /*! \enum Bitdefinitons for the egamma class, for the electron identification 
    see egammaParameters for info on the variable definitions */
  enum BitDefElectron {
    /** @brief cluster eta range */
    ClusterEtaRange_Electron        =  0,
    /** @brief matching to photon (not necessarily conversion--the name is historical) */
    ConversionMatch_Electron        =  1,    
    /** @brief cluster leakage into the hadronic calorimeter */
    ClusterHadronicLeakage_Electron =  2,
    /** @brief energy in 2nd sampling (e.g E277>0) */
    ClusterMiddleEnergy_Electron    =  3, 
    /** @brief energy ratio in 2nd sampling (e.g E237/E277) */
    ClusterMiddleEratio37_Electron  =  4,
    /** @brief energy ratio in 2nd sampling (e.g E233/E237) */
    ClusterMiddleEratio33_Electron  =  5,
    /** @brief width in the second sampling (e.g Weta2) */
    ClusterMiddleWidth_Electron     =  6,
    /** @brief energy fraction in the third layer */
    ClusterBackEnergyFraction_Electron = 7,
    /** @brief fraction of energy found in 1st sampling (NB: not used in fact for electrons)*/
    ClusterStripsEratio_Electron    =  8,
    /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) */
    ClusterStripsDeltaEmax2_Electron =  9,
    /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) */
    ClusterStripsDeltaE_Electron    = 10,
    /** @brief shower width in 1st sampling */
    ClusterStripsWtot_Electron      = 11,
    /** @brief shower shape in shower core 1st sampling */
    ClusterStripsFracm_Electron     = 12,
    /** @brief shower width weighted by distance from the maximum one */
    ClusterStripsWeta1c_Electron    = 13,
    /** @brief difference between max and 2nd max in strips */
    ClusterStripsDEmaxs1_Electron   = 15,
    /** @brief B layer hit */
    TrackBlayer_Electron            = 16,
    /** @brief number of Pixel hits */
    TrackPixel_Electron             = 17,
    /** @brief number of Pixel and SCT hits */
    TrackSi_Electron                = 18,
    /** @brief distance of closet approach */
    TrackA0_Electron                = 19,
    /** @brief eta difference between cluster and extrapolated track in the 1st sampling */
    TrackMatchEta_Electron          = 20,
    /** @brief phi difference between cluster and extrapolated track in the 2nd sampling */
    TrackMatchPhi_Electron          = 21,
    /** @brief energy-momentum match */
    TrackMatchEoverP_Electron       = 22,
    /** @brief Cut on the TRT eProbabilityHT_Electron */
    TrackTRTeProbabilityHT_Electron   = 23,
    /** @brief number of TRT hits */
    TrackTRThits_Electron           = 24,
    /** @brief ratio of high to all TRT hits for isolated electrons */
    TrackTRTratio_Electron          = 25,
    /** @brief ratio of high to all TRT hits for non-isolated electrons (not for new ++ menus) */    
    TrackTRTratio90_Electron        = 26,
    /** @brief distance of closet approach for tight selection (!! Used in new ++ menus dc14) */
    TrackA0Tight_Electron           = 27,
    /** @brief eta difference between cluster and extrapolated track in the 1st sampling for 
	tight selection (!! Used in new ++ menus dc14)*/
    TrackMatchEtaTight_Electron     = 28,
    /** @brief isolation */
    Isolation_Electron              = 29,
    /** @brief calorimetric isolation */
    ClusterIsolation_Electron       = 30,
    /** @brief tracker isolation */
    TrackIsolation_Electron         = 31
  };

  /*! \enum Bitdefinitons for the egamma class for photon identification
    see egammaParameters for info on the variable definitions */
  enum BitDefPhoton {
    /** @brief cluster eta range */
    ClusterEtaRange_Photon        =  0,
    /** @brief energy fraction in the third layer */
    ClusterBackEnergyFraction_Photon = 7,
    /** @brief cluster leakage into the hadronic calorimeter */
    ClusterHadronicLeakage_Photon =  10,
    /** @brief energy in 2nd sampling (e277) */
    ClusterMiddleEnergy_Photon    =  11, 
    /** @brief energy ratio in 2nd sampling */
    ClusterMiddleEratio37_Photon  =  12,
    /** @brief energy ratio in 2nd sampling for photons */
    ClusterMiddleEratio33_Photon  =  13,
    /** @brief width in the second sampling */
    ClusterMiddleWidth_Photon     =  14,
    /** @brief fraction of energy found in 1st sampling */
    ClusterStripsEratio_Photon    =  15,
    /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) */
    ClusterStripsDeltaEmax2_Photon =  16,
    /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) */
    ClusterStripsDeltaE_Photon    = 17,
    /** @brief shower width in 1st sampling */
    ClusterStripsWtot_Photon      = 18,
    /** @brief shower shape in shower core 1st sampling */
    ClusterStripsFracm_Photon     = 19,
    /** @brief shower width weighted by distance from the maximum one */
    ClusterStripsWeta1c_Photon    = 20,
    /** @brief difference between max and 2nd max in strips */
    ClusterStripsDEmaxs1_Photon  = 21,
    /** @brief energy-momentum match for photon selection*/
    TrackMatchEoverP_Photon       = 22,
    /** @brief ambiguity resolution for photon (vs electron) */
    AmbiguityResolution_Photon    = 23,
    /** @brief isolation */
    Isolation_Photon              = 29,
    /** @brief calorimetric isolation for photon selection */
    ClusterIsolation_Photon       = 30,
    /** @brief tracker isolation for photon selection */
    TrackIsolation_Photon         = 31
  };


  //
  // cut definition for electrons
  //
  /** @brief cuts of hadronic leakage*/
  const unsigned int HADLEAKETA_ELECTRON = 
    0x1 << ClusterEtaRange_Electron        | 
    0x1 << ClusterHadronicLeakage_Electron;
  /** @brief cuts in strips (with ClusterStripsDEmaxs1)*/
  const unsigned int CALOSTRIPS_ELECTRON =
    0x1 << ClusterStripsEratio_Electron     |
    0x1 << ClusterStripsDeltaEmax2_Electron |
    0x1 << ClusterStripsDeltaE_Electron     |
    0x1 << ClusterStripsWtot_Electron       |
    0x1 << ClusterStripsFracm_Electron      |
    0x1 << ClusterStripsWeta1c_Electron     |
    0x1 << ClusterStripsDEmaxs1_Electron    ;
  const unsigned int CALOSTRIPS_LOOSE_ELECTRON =
    0x1 << ClusterStripsWtot_Electron       |
    0x1 << ClusterStripsDEmaxs1_Electron    ;
  /** @brief cuts in middle sampling*/
  const unsigned int CALOMIDDLE_ELECTRON =    
    0x1 << ClusterMiddleEnergy_Electron     |
    0x1 << ClusterMiddleEratio37_Electron   |
    0x1 << ClusterMiddleWidth_Electron     ;
  const unsigned int CALOBACK_ELECTRON = 
    0x1 << ClusterBackEnergyFraction_Electron |
    0x1 << ClusterMiddleEratio33_Electron;

  /** @brief calorimeter isolation*/
  const unsigned int CALORIMETRICISOLATION_ELECTRON = 
    0x1 << ClusterIsolation_Electron ;

  /** @brief all cuts in calorimeter (except isolation)*/
  const unsigned int CALO_ELECTRON = HADLEAKETA_ELECTRON | CALOSTRIPS_ELECTRON | CALOMIDDLE_ELECTRON | CALOBACK_ELECTRON;

  /** @brief Track quality cuts except b-layer for electrons*/
  const unsigned int TRACKINGNOBLAYER_ELECTRON =     
    0x1 << TrackPixel_Electron   |
    0x1 << TrackSi_Electron      |
    0x1 << TrackA0_Electron;
  /** @brief Track quality cuts except b-layer for electrons*/
  const unsigned int TRACKINGLOOSE_ELECTRON =     
    0x1 << TrackPixel_Electron   |
    0x1 << TrackSi_Electron;
  /** @brief Track quality cuts for electrons*/
  const unsigned int TRACKING_ELECTRON = 
    TRACKINGNOBLAYER_ELECTRON |  
    0x1 << TrackBlayer_Electron;
  /** @brief Track cluster matching in eta for electrons*/
  const unsigned int TRACKMATCHDETA_ELECTRON =
    0x1 << TrackMatchEta_Electron;
  /** @brief Track cluster matching in eta, phi for electrons*/
  const unsigned int TRACKMATCHNOEOVERP_ELECTRON =
    0x1 << TrackMatchEta_Electron      |
    0x1 << TrackMatchPhi_Electron;
  /** @brief Track cluster matching in eta, phi, E/p for electrons*/
  const unsigned int TRACKMATCH_ELECTRON =
    0x1 << TrackMatchEta_Electron      |
    0x1 << TrackMatchPhi_Electron      |
    0x1 << TrackMatchEoverP_Electron  ;
  /** @brief Tight Track cluster matching */
  const unsigned int TRACKMATCHTIGHT_ELECTRON =
    0x1 << TrackMatchEtaTight_Electron      |
    0x1 << TrackA0Tight_Electron;
  /** @brief Track cluster matching in eta tight for electrons*/
  const unsigned int TRACKMATCHDETATIGHT_ELECTRON = 
    0x1 << TrackMatchEtaTight_Electron;
  /** @brief Tight conversion matching */
  const unsigned int CONVMATCH_ELECTRON =
    0x1 << ConversionMatch_Electron;

  /** @brief TRT hits and TR ratio and TR Probability for electrons*/
  const unsigned int TRT_ELECTRON = 
    0x1 << TrackTRThits_Electron   |
    0x1 << TrackTRTratio_Electron  |
    0x1 << TrackTRTeProbabilityHT_Electron ;

  /** @brief TR ratio for electrons*/
  const unsigned int TRT_RATIO_ELECTRON = 
    0x1 << TrackTRTratio_Electron;

  /** @brief TR PID tool cut  for electrons*/
  const unsigned int TRT_EPROB_ELECTRON = 
    0x1 << TrackTRTeProbabilityHT_Electron ;
    
  /** @brief isolation by tracker */
  const unsigned int TRACKINGISOLATION_ELECTRON = 
    0x1 << TrackIsolation_Electron;
  /** @brief isolation for electrons is combination of calo and tracker */
  const unsigned int ISOLATION_ELECTRON = 
    0x1 << Isolation_Electron ;
  /** @brief isolation for electrons is combination of calo and tracker */
  const unsigned int CALOTRACKISOLATION_ELECTRON = 
    CALORIMETRICISOLATION_ELECTRON | TRACKINGISOLATION_ELECTRON;

  /** @brief all cuts except TRT for electrons*/
  const unsigned int ALLNOTRT_ELECTRON = 
    TRACKING_ELECTRON | TRACKMATCH_ELECTRON | CALO_ELECTRON;
  /** @brief all cuts for electrons*/
  const unsigned int ALL_ELECTRON = 
    ALLNOTRT_ELECTRON | TRT_ELECTRON;

  //////////////////////////
  // new ++ menus
  //
  /** @brief Loose++ electron selection */
  const unsigned int ElectronLoosePP = 
    CALOMIDDLE_ELECTRON | HADLEAKETA_ELECTRON | CALOSTRIPS_LOOSE_ELECTRON | 
    TRACKMATCHDETA_ELECTRON | TRACKINGLOOSE_ELECTRON;

  /** @brief Loose++ electron selecton with isolation */
  const unsigned int ElectronLoosePPIso = 
    ElectronLoosePP |
    ISOLATION_ELECTRON;

  /** @brief Medium++ electron selecton */
  const unsigned int ElectronMediumPP =
    CALO_ELECTRON | 
    TRACKING_ELECTRON |
    TRACKMATCHDETA_ELECTRON | 
    TRACKMATCHDETATIGHT_ELECTRON |
    TRT_RATIO_ELECTRON |
    TRT_EPROB_ELECTRON ;

  /** @brief Medium++ electron selecton with isolation */
  const unsigned int ElectronMediumPPIso = 
    ElectronMediumPP |
    ISOLATION_ELECTRON;

  /** @brief Tight++ electron selecton */
  const unsigned int ElectronTightPP =
    CALO_ELECTRON | 
    TRACKING_ELECTRON | 
    TRACKMATCH_ELECTRON |
    TRACKMATCHTIGHT_ELECTRON |
    CONVMATCH_ELECTRON |
    TRT_ELECTRON;

  /** @brief Tight++ electron selecton with isolation */
  const unsigned int ElectronTightPPIso = 
    ElectronTightPP |
    ISOLATION_ELECTRON;
  //////////////////////////
  // definitions as in 16.6, but with outliers
  /** @brief Loose electron selection */
  const unsigned int ElectronLoose = 
    CALOMIDDLE_ELECTRON | HADLEAKETA_ELECTRON;
  /** @brief Loose electron selecton with isolation */
  const unsigned int ElectronLooseIso = 
    ElectronLoose |
    ISOLATION_ELECTRON;
  /** @brief Medium electron selection with the delta eta cut has been removed */
  const unsigned int ElectronMedium_WithoutTrackMatch = 
    CALO_ELECTRON | TRACKINGNOBLAYER_ELECTRON;
  /** @brief Medium electron selection without deta match with isolation requirement */
  const unsigned int ElectronMediumIso_WithoutTrackMatch = 
    ElectronMedium_WithoutTrackMatch | 
    ISOLATION_ELECTRON;
  /** @brief Medium electron selection with deta match (was ElectronMedium in 15.X)*/
  const unsigned int ElectronMedium_WithTrackMatch = 
    CALO_ELECTRON | TRACKINGNOBLAYER_ELECTRON | TRACKMATCHDETA_ELECTRON;
  /** @brief MediumIso electron selection with deta match (was ElectronMediumIso in 15.X)*/
  const unsigned int ElectronMediumIso_WithTrackMatch = 
    ElectronMedium_WithTrackMatch | 
    ISOLATION_ELECTRON;
  /** @brief Medium electron selecton */
  const unsigned int ElectronMedium = ElectronMedium_WithTrackMatch;
  /** @brief MediumIso electron selecton */
  const unsigned int ElectronMediumIso = ElectronMediumIso_WithTrackMatch;
  /** @brief Tight electron without track match (corresponds to RobusterTight in 15.6.X but 
      without the deltaEta bits TrackMatchEta_Electron and TrackMatchEtaTight_Electron) */
  const unsigned int ElectronTight_WithoutTrackMatch = 
    CALO_ELECTRON | 
    TRACKING_ELECTRON | 
    0x1 << TrackMatchEoverP_Electron | 
    0x1 << TrackA0Tight_Electron |
    CONVMATCH_ELECTRON | 
    TRT_ELECTRON ;

  /** @brief Tight without conversion requirement or track match */
  const unsigned int ElectronTight_WithoutTrackMatch_NoConvCut = 
    CALO_ELECTRON | 
    TRACKING_ELECTRON | 
    0x1 << TrackMatchEoverP_Electron | 
    0x1 << TrackA0Tight_Electron |
    TRT_ELECTRON ;

  /** @brief Tight_WithTrackMatch electron selection with isolation requirement */
  const unsigned int ElectronTightIso_WithoutTrackMatch = 
    ElectronTight_WithoutTrackMatch | 
    ISOLATION_ELECTRON;

  /** @brief Tight electron selection with track match*/
  const unsigned int ElectronTight_WithTrackMatch = 
    CALO_ELECTRON | TRACKING_ELECTRON | TRACKMATCH_ELECTRON | 
    TRACKMATCHTIGHT_ELECTRON | CONVMATCH_ELECTRON | TRT_ELECTRON ;

  /** @brief Tight with track match without conversion requirement */
  const unsigned int ElectronTight_WithTrackMatch_NoConvCut = 
    CALO_ELECTRON | TRACKING_ELECTRON | TRACKMATCH_ELECTRON | 
    TRACKMATCHTIGHT_ELECTRON | TRT_ELECTRON ;

  /** @brief Tight_WithTrackMatch electron selection with isolation requirement */
  const unsigned int ElectronTightIso_WithTrackMatch = 
    ElectronTight_WithTrackMatch | 
    ISOLATION_ELECTRON;

  /** @brief Tight electron selection */
  const unsigned int ElectronTight = ElectronTight_WithTrackMatch;
  /** @brief Tight without conversion requirement */
  const unsigned int ElectronTight_NoConvCut = ElectronTight_WithTrackMatch_NoConvCut;
  /** @brief Tight electron selection with isolation requirement */
  const unsigned int ElectronTightIso = ElectronTightIso_WithTrackMatch;

  /** @brief Tight electron (corresponds to RobusterTight in 15.6.X with the loose deltaEta bit
      TrackMatchEta_Electron but with out the tight one TrackMatchEtaTight_Electron) */
  const unsigned int ElectronTight_WithLooseEtaTrackMatch = 
    CALO_ELECTRON | 
    TRACKING_ELECTRON | 
    TRACKMATCHDETA_ELECTRON |
    0x1 << TrackMatchEoverP_Electron | 
    0x1 << TrackA0Tight_Electron |
    CONVMATCH_ELECTRON | 
    TRT_ELECTRON ;

  /** @brief Tight electron (corresponds to RobusterTight in 15.6.X with the deltaEta bits
      TrackMatchEta_Electron and TrackMatchEtaTight_Electron) */
  const unsigned int ElectronTight_WithTightEtaTrackMatch = 
    CALO_ELECTRON | 
    TRACKING_ELECTRON | 
    TRACKMATCHDETA_ELECTRON |
    TRACKMATCHTIGHT_ELECTRON |
    0x1 << TrackMatchEoverP_Electron | 
    CONVMATCH_ELECTRON | 
    TRT_ELECTRON ;

  /** @brief Tight_WithLooseEtaTrackMatch electron selection with isolation requirement */
  const unsigned int ElectronTightIso_WithLooseEtaTrackMatch = 
    ElectronTight_WithLooseEtaTrackMatch |
    ISOLATION_ELECTRON;

  /** @brief Tight_WithTightEtaTrackMatch electron selection with isolation requirement */
  const unsigned int ElectronTightIso_WithTightEtaTrackMatch = 
    ElectronTight_WithTightEtaTrackMatch |
    ISOLATION_ELECTRON;

 
  /** @brief trigger specfic definitions **/
  /** @brief calo middle with rphi **/
  const unsigned int CALOMIDDLE_ELECTRON_HLT = 
      CALOMIDDLE_ELECTRON |
      0X1 << ClusterMiddleEratio33_Electron;
  

  /** @brief all cuts in calorimeter (except isolation)*/
  const unsigned int CALO_ELECTRON_HLT = 
      HADLEAKETA_ELECTRON | 
      CALOSTRIPS_ELECTRON | 
      CALOMIDDLE_ELECTRON | 
      CALOBACK_ELECTRON;

  /** @brief Tight Track cluster matching redefined for EF **/
  const unsigned int TRACKMATCHTIGHT_ELECTRON_EF = 0x1 << TrackA0Tight_Electron;
  /** @brief Track-cluster matching in eta, E/p for electrons redefined for EF **/
  const unsigned int TRACKMATCH_ELECTRON_EF = 0x1 << TrackMatchEta_Electron | 
      0x1 << TrackMatchEoverP_Electron;  
  /** @brief Track-cluster matching in eta for electrons redefined for EF **/
  const unsigned int TRACKMATCH_ELECTRON_NoEoP_EF = 0x1 << TrackMatchEta_Electron;
  
  /** Electron trigger PID definitions */
  
  /** @brief Loose1 tunes electron selection */
  // Add Rphi to Calo selection
  const unsigned int ElectronLoose1 = 
    CALOMIDDLE_ELECTRON | HADLEAKETA_ELECTRON | CALOSTRIPS_LOOSE_ELECTRON | 
    TRACKMATCHDETA_ELECTRON | TRACKINGLOOSE_ELECTRON;

  /** @brief Medium1 tunes electron selecton */
  // Add Rphi to Calo selection and CALOBACK
  const unsigned int ElectronMedium1 =
    CALO_ELECTRON | 
    TRACKING_ELECTRON | 
    TRACKMATCHDETA_ELECTRON |
    TRT_RATIO_ELECTRON |
    TRT_EPROB_ELECTRON;
  
   /** @brief Tight1 tunes electron selecton */
  const unsigned int ElectronTight1 = 
      HADLEAKETA_ELECTRON | 
      CALOSTRIPS_ELECTRON | 
      CALOMIDDLE_ELECTRON |
      TRACKING_ELECTRON | 
      TRACKMATCHDETA_ELECTRON | 
      TRACKMATCH_ELECTRON_EF | 
      TRACKMATCHTIGHT_ELECTRON | 
      TRT_ELECTRON;
  
  /** @brief Loose 2014 tunes electron selection */
  // Add Rphi to Calo selection
  const unsigned int ElectronLooseHLT = 
    CALOMIDDLE_ELECTRON | HADLEAKETA_ELECTRON | CALOSTRIPS_LOOSE_ELECTRON | 
    TRACKMATCHDETA_ELECTRON | TRACKINGLOOSE_ELECTRON;
  /** @brief Medium 2014 tunes electron selecton */
  // Add Rphi to Calo selection and CALOBACK
  const unsigned int ElectronMediumHLT =
    CALO_ELECTRON_HLT | 
    TRACKING_ELECTRON | 
    TRACKMATCHDETA_ELECTRON |
    TRACKMATCHDETATIGHT_ELECTRON |
    TRT_RATIO_ELECTRON |
    TRT_EPROB_ELECTRON;

  /** @brief Tight 2014 tunes electron selecton */
  // Add Rphi to Calo selection and CALOBACK
  const unsigned int ElectronTightHLT =
    CALO_ELECTRON_HLT | 
    TRACKING_ELECTRON | 
    TRACKMATCHDETA_ELECTRON |
    TRACKMATCHTIGHT_ELECTRON |
    TRT_ELECTRON;
  //
  // for photon selection
  //
  /** @brief cuts of hadronic leakage*/
  const unsigned int HADLEAKETA_PHOTON = 
    0x1 << ClusterEtaRange_Photon        | 
    0x1 << ClusterHadronicLeakage_Photon;
  /** @brief cuts in middle sampling*/
  const unsigned int CALOMIDDLE_PHOTON =    
    0x1 << ClusterMiddleEnergy_Photon     |
    0x1 << ClusterMiddleEratio37_Photon   |
    0x1 << ClusterMiddleEratio33_Photon   |
    0x1 << ClusterMiddleWidth_Photon     ;
 /** @brief cuts in strips (with ClusterStripsDEmaxs1)*/
  const unsigned int CALOSTRIPS_PHOTONTIGHT =
    0x1 << ClusterStripsEratio_Photon     |
    0x1 << ClusterStripsDeltaEmax2_Photon |
    0x1 << ClusterStripsDeltaE_Photon     |
    0x1 << ClusterStripsWtot_Photon       |
    0x1 << ClusterStripsFracm_Photon      |
    0x1 << ClusterStripsWeta1c_Photon     |
    0x1 << ClusterStripsDEmaxs1_Photon    ;

 /** @brief cuts in strips (with ClusterStripsDEmaxs1)*/
  const unsigned int CALOSTRIPS_PHOTONMEDIUM =
    0x1 << ClusterStripsDEmaxs1_Photon    ;

  /** @brief calorimeter isolation*/
  const unsigned int CALORIMETRICISOLATION_PHOTON = 
    0x1 << ClusterIsolation_Photon;

  /** @brief isolation by tracker */
  const unsigned int TRACKINGISOLATION_PHOTON = 
    0x1 << TrackIsolation_Photon;
  /** @brief isolation for photons is combination of calo and tracker */
  const unsigned int ISOLATION_PHOTON = 
    0x1 << Isolation_Photon ;
  /** @brief isolation for photons is combination of calo and tracker */
  const unsigned int CALOTRACKISOLATION_PHOTON = 
    CALORIMETRICISOLATION_PHOTON | TRACKINGISOLATION_PHOTON;
  /** @brief Track cluster matching E/p for photons*/
  const unsigned int TRACKMATCH_PHOTON =
    0x1 << TrackMatchEoverP_Photon  ;
  /** @brief Ambigiuty resolve for photons */
  const unsigned int AMBIGUITYRESOLVE_PHOTON =
    0x1 << AmbiguityResolution_Photon;
  /** @brief bit definitions for Photon triggers */
  // Added for Trigger
  /** @brief cuts of hadronic leakage (for Loose selection) */
  const unsigned int HADLEAKETA_PHOTON_EF = 
      0x1 << ClusterEtaRange_Photon | 
      0x1 << ClusterHadronicLeakage_Photon;

  /** @brief cuts of Eratio (for Medium selection) */
  const unsigned int CALO_PHOTON_RETA_WETA2_ERATIO = 
      0x1 << ClusterEtaRange_Photon | 
      0x1 << ClusterMiddleEnergy_Photon | 
      0x1 << ClusterMiddleEratio37_Photon | 
      0x1 << ClusterMiddleWidth_Photon   | 
      0x1 << ClusterStripsEratio_Photon;

  /** @brief Loose photon selection */
  const unsigned int PhotonLoose = HADLEAKETA_PHOTON | CALOMIDDLE_PHOTON;
  /** @brief Loose photon selection with Isolation*/
  const unsigned int PhotonLooseIso = PhotonLoose | ISOLATION_PHOTON;
  /** @brief Loose photon selection with Ambiguity resolver*/
  const unsigned int PhotonLooseAR = PhotonLoose | AMBIGUITYRESOLVE_PHOTON;
  /** @brief Loose photon selection with Ambiguity resolver and Isolation*/
  const unsigned int PhotonLooseARIso = PhotonLooseAR | ISOLATION_PHOTON;
  /** @brief Medium photon selection */
  const unsigned int PhotonMedium = HADLEAKETA_PHOTON | CALOMIDDLE_PHOTON | CALOSTRIPS_PHOTONMEDIUM;
  /** @brief Medium photon selection with Isolation*/
  const unsigned int PhotonMediumIso = PhotonMedium | ISOLATION_PHOTON;
  /** @brief Medium photon selection with Ambiguity revolver*/
  const unsigned int PhotonMediumAR = PhotonMedium | AMBIGUITYRESOLVE_PHOTON;
  /** @brief Medium photon selection with Ambiguity resolver and Isolation*/
  const unsigned int PhotonMediumARIso = PhotonMediumAR | ISOLATION_PHOTON;
  /** @brief Tight photon selection */
  const unsigned int PhotonTight = HADLEAKETA_PHOTON | CALOMIDDLE_PHOTON | CALOSTRIPS_PHOTONTIGHT;
  /** @brief Tight photon selection with isolation*/
  const unsigned int PhotonTightIso = PhotonTight | ISOLATION_PHOTON;
  /** @brief Tight photon selection with Ambiguity resolver*/
  const unsigned int PhotonTightAR = PhotonTight | AMBIGUITYRESOLVE_PHOTON;
  /** @brief Tight photon selection with isolation and Ambiguity resolver*/
  const unsigned int PhotonTightARIso = PhotonTightAR | ISOLATION_PHOTON;
  /** @brief TrigEgamma Pid Definitions */
  /** @brief Loose photon selection for online EF */
  const unsigned int PhotonLooseEF =  CALOMIDDLE_PHOTON | HADLEAKETA_PHOTON_EF;
  // Added for 2g20_medium
  /** @brief Medium photon selection for online EF */
  const unsigned int PhotonMediumEF = HADLEAKETA_PHOTON_EF | CALO_PHOTON_RETA_WETA2_ERATIO;

enum BitDefForwardElectron {
        /** @brief cluster eta range */
        BinEta_ForwardElectron        =  0,
        /** @brief second lambda shape moment */
        SECONDLAMBDA_ForwardElectron        =  1,
        /** @brief lateral shape moment */
        LATERAL_ForwardElectron        =  2,
        /** @brief Longitudinal shape moment */
        LONGITUDINAL_ForwardElectron        =  3,
        /** @brief cell max frac shape moment */
        CELLMAXFRAC_ForwardElectron        =  4,
        /** @brief secondR shape moment */
        SECONDR_ForwardElectron        =  5,
        /** @brief center lambda shape moment */
        CENTERLAMBDA_ForwardElectron        =  6,

};

//
  // cut definition for Forw electrons
  //
  /** @brief cuts on all variables */
  const unsigned int ID_ForwardElectron =
    0x1 << BinEta_ForwardElectron        |
    0x1 << SECONDLAMBDA_ForwardElectron  |
    0x1 << LATERAL_ForwardElectron       |
    0x1 << LONGITUDINAL_ForwardElectron  |
    0x1 << CELLMAXFRAC_ForwardElectron   |
    0x1 << SECONDR_ForwardElectron       |
    0x1 << CENTERLAMBDA_ForwardElectron  ;
  
  const unsigned int ForwardElectronTight = 126;
  const unsigned int ForwardElectronLoose = 104;

  struct ROOT6_NamespaceAutoloadHook{};
}


#endif // EGAMMAREC_EGAMMAPIDDEFS_H

