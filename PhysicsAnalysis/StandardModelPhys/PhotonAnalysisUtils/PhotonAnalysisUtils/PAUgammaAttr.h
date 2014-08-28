/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PAUgammaAttr.h, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////
//
// This class contains additional attributes that apply to PAU::gamma
// objects -- only from reconstruction
//

#ifndef PHOTONANALYSISUTILS_PAUGAMMAATTR_H
#define PHOTONANALYSISUTILS_PAUGAMMAATTR_H

#include <vector>
class Jet ;
namespace Rec { class TrackParticle ; }
class EmTau_ROI ;
class TrigEMCluster ;
class TrigElectron ;
class egamma ;
class CaloCluster ;
namespace PAU {

  //-------------------------------------------------------------------------------------
  // class PAU::gammaAttr:
  class gammaAttr {
  public: // methods
    gammaAttr() :
      matchPassL1(4095), matchPassL2(4095), matchPassEF(4095)
      , L1RoI(0), L2Clus(0), L2Electron(0), EFegamma(0)
      , ambiguityResult(UNKNOWN)
      , itune1(IUNKNOWN), itune2(IUNKNOWN), itune3(IUNKNOWN) 
      , logLikelihoodID(UNKNOWN)
      , trackIsolation(UNKNOWN)
      , selected(false)
      , cells55etcone20(UNKNOWN), cells55etcone30(UNKNOWN), cells55etcone40(UNKNOWN)
      , cells35etcone20(UNKNOWN), cells35etcone30(UNKNOWN), cells35etcone40 (UNKNOWN)
      , etcone20iso(UNKNOWN), etcone30iso(UNKNOWN), etcone40iso(UNKNOWN)
      , cells55etcone20iso(UNKNOWN), cells55etcone30iso(UNKNOWN), cells55etcone40iso(UNKNOWN)
      , cells35etcone20iso(UNKNOWN), cells35etcone30iso(UNKNOWN), cells35etcone40iso(UNKNOWN)
      , etcone10(UNKNOWN), etcone15(UNKNOWN), etcone25(UNKNOWN), etcone35(UNKNOWN)
      , etcone20topoCluster(UNKNOWN), etcone40topoCluster(UNKNOWN), etcone60topoCluster(UNKNOWN)
      , topoCluster(0), EMtopoCluster(0)
      //, etaTopoClus(UNKNOWN), phiTopoClus(UNKNOWN), ptTopoClus(UNKNOWN), eTopoClus(UNKNOWN)
      //, etaEMTopoClus(UNKNOWN), phiEMTopoClus(UNKNOWN), ptEMTopoClus(UNKNOWN), eEMTopoClus(UNKNOWN)
      , convIP(UNKNOWN), convIPRev(UNKNOWN)
      , convTrack(0), convPT(UNKNOWN), convPZ(UNKNOWN), convPhi(UNKNOWN)
      , nearestJet(0), deltaRnearestJet(UNKNOWN)
      //, L1_e(UNKNOWN), L1_eta(UNKNOWN), L1_phi(UNKNOWN), L1_EMIsol(UNKNOWN), L1_HadIsol(UNKNOWN), L1_HadCore(UNKNOWN)
      //, L2_e(UNKNOWN), L2_et(UNKNOWN), L2_eta(UNKNOWN), L2_phi(UNKNOWN), L2_e237(UNKNOWN), L2_e277(UNKNOWN), L2_fracs1(UNKNOWN)
      //, L2_weta2(UNKNOWN), L2_ehad1(UNKNOWN), L2_emaxs1(UNKNOWN), L2_e2tsts1(UNKNOWN)
      //, EF_e(UNKNOWN), EF_et(UNKNOWN), EF_eta(UNKNOWN), EF_phi(UNKNOWN), EF_isem(0xffffffff), EF_ethad(UNKNOWN), EF_ethad1(UNKNOWN)
      //, EF_e233(UNKNOWN), EF_e237(UNKNOWN), EF_e277(UNKNOWN), EF_emaxs1(UNKNOWN), EF_e2tsts1(UNKNOWN), EF_fracs1(UNKNOWN), EF_weta1(UNKNOWN)
      //, EF_weta2(UNKNOWN), EF_wtots1(UNKNOWN), EF_f1(UNKNOWN), EF_emins1(UNKNOWN)
      , HPV_zvertex(UNKNOWN), HPV_sigma_zvertex(UNKNOWN), HPV_eta(UNKNOWN), HPV_sigma_eta(UNKNOWN)
      , HPV_zvertex_charged(UNKNOWN), HPV_sigma_zvertex_charged(UNKNOWN)
      , HPV_zvertex_neutral(UNKNOWN), HPV_sigma_zvertex_neutral(UNKNOWN)
      , HPV_convTrk1_zvertex(UNKNOWN), HPV_convTrk1_sigma_zvertex(UNKNOWN)
      , HPV_convTrk2_zvertex(UNKNOWN), HPV_convTrk2_sigma_zvertex(UNKNOWN)
      , CaloPointing_zvertex(UNKNOWN), CaloPointing_sigma_zvertex(UNKNOWN)
      , CaloPointing_eta(UNKNOWN), CaloPointing_sigma_eta(UNKNOWN)
      {} ;
      ~gammaAttr() {} ;
  public: // data members
    int matchPassL1, matchPassL2, matchPassEF ;
    const EmTau_ROI* L1RoI ;
    const TrigEMCluster* L2Clus ;
    const TrigElectron* L2Electron;
    const egamma* EFegamma ;
    int ambiguityResult ;
    int itune1, itune2, itune3 ;
    float logLikelihoodID ;
    float trackIsolation ;
    bool selected ;
    float cells55etcone20, cells55etcone30, cells55etcone40, cells35etcone20, cells35etcone30, cells35etcone40 ;
    float etcone20iso, etcone30iso, etcone40iso,
      cells55etcone20iso, cells55etcone30iso, cells55etcone40iso, 
      cells35etcone20iso, cells35etcone30iso, cells35etcone40iso ;
    float etcone10, etcone15, etcone25, etcone35 ;
    float 
      etcone10_corrected, etcone15_corrected,
      etcone20_corrected, etcone25_corrected,
      etcone30_corrected, etcone35_corrected,
      etcone40_corrected;
    float 
      etcone10_pt_corrected, etcone15_pt_corrected,
      etcone20_pt_corrected, etcone25_pt_corrected,
      etcone30_pt_corrected, etcone35_pt_corrected,
      etcone40_pt_corrected;
    float 
      etcone10_ED_corrected, etcone15_ED_corrected,
      etcone20_ED_corrected, etcone25_ED_corrected,
      etcone30_ED_corrected, etcone35_ED_corrected,
      etcone40_ED_corrected;
    float
      etcone40_ED0_corrected,
      etcone40_ED1_corrected,
      etcone40_ED2_corrected,
      etcone40_ED3_corrected,
      etcone40_ED4_corrected,
      etcone40_ED5_corrected;
    float
      transmin_etcone40, transmax_etcone40,
      transmin_etcone100, transmax_etcone100,
      transmin, transmax,
      transmin_loweta, transmax_loweta;
    
    float etcone20topoClus, etcone40topoClus, etcone60topoClus ;
    float etcone20topoCluster, etcone40topoCluster, etcone60topoCluster ;
    float PtIsolationCone,PtIsolationConePhAngle;
    const CaloCluster *topoCluster, *EMtopoCluster ;
    //float etaTopoClus, phiTopoClus, ptTopoClus, eTopoClus,
    //  etaEMTopoClus, phiEMTopoClus, ptEMTopoClus, eEMTopoClus ;
    float convIP, convIPRev ;
    std::vector<const Rec::TrackParticle*> convTrack; /// list of Rec::TrackParticle 's from conversion
    float convPT, convPZ, convPhi ;
    int inearestJet ;
    const Jet* nearestJet ;
    float deltaRnearestJet ;

    float HPV_zvertex, HPV_sigma_zvertex, HPV_eta, HPV_sigma_eta, 
      HPV_zvertex_charged, HPV_sigma_zvertex_charged, 
      HPV_zvertex_neutral, HPV_sigma_zvertex_neutral,
      HPV_convTrk1_zvertex, HPV_convTrk1_sigma_zvertex, 
      HPV_convTrk2_zvertex, HPV_convTrk2_sigma_zvertex,
      CaloPointing_zvertex, CaloPointing_sigma_zvertex, CaloPointing_eta, CaloPointing_sigma_eta;

    float L1_bestMatchDeltaR, L2_bestMatchDeltaR, EF_bestMatchDeltaR;

    float eS0_real,eS1_real,eS2_real,eS3_real;

    float 
      ptIsolCone_woIC       ,
      ptIsolConePhAngle_woIC,
      ptIsolCone            ,
      dRisol                ,
      trackIsol_woIC        ;

    float
      convTrk1_DeltaEta_track_calo,
      convTrk2_DeltaEta_track_calo,
      convTrk1_DeltaPhi_track_calo,
      convTrk2_DeltaPhi_track_calo,
      convTrk1_trtPID,
      convTrk2_trtPID,
      convTrk1_trtRatioHT,
      convTrk2_trtRatioHT,
      convTrk1_ToTvar,
      convTrk2_ToTvar,
      isConv      ,
      Xconv       ,
      Yconv       ,
      PT1conv     ,
      PT2conv     ,
      PTconv      ,
      PZconv      ,
      isRecovered ,
      convTrk1_numBLHits   ,
      convTrk1_numPixelHits,
      convTrk1_numSCTHits  ,
      convTrk1_numTRTHits  ,
      convTrk1_numTRTOutliers  ,
      convTrk1_numTRTHTHits  ,
      convTrk1_numTRTHTOutliers  ,
      convTrk1_charge      ,
      convTrk2_numBLHits   ,
      convTrk2_numPixelHits,
      convTrk2_numSCTHits  ,
      convTrk2_numTRTHits  ,
      convTrk2_numTRTOutliers  ,
      convTrk2_numTRTHTHits  ,
      convTrk2_numTRTHTOutliers  ,
      convTrk2_charge      ;

    int 
      isConvMC           ,
      pdgMC              ,
      isG4part           ,
      pdgMother          ,
      isBremMC           ,
      isHardProcMC       ,
      isHardProcPhotonMC ,
      matchMC            ,
      particleType       ,
      particleOrig       ,
      imatchleadingTruthParton;

    float
      etaMC              ,      
      phiMC              ,
      ptMC               ,
      eMC                ,
      etaS2MC            ,
      phiS2MC            ,
      RconvMC            ,
      XconvMC            ,
      YconvMC            ,
      ZconvMC            ;

    double
      etaCaloMC          ,
      phiCaloMC          ;

    float
      partonIsolation15_UE  ,
      partonIsolation20_UE  ,
      partonIsolation25_UE  ,
      partonIsolation30_UE  ,
      partonIsolation35_UE  ,
      partonIsolation40_UE  ,
      partonIsolation45_UE  ,
      partonIsolation60_UE  ,
      partonIsolation15  ,
      partonIsolation20  ,
      partonIsolation25  ,
      partonIsolation30  ,
      partonIsolation35  ,
      partonIsolation40  ,
      partonIsolation45  ,
      partonIsolation60  ;

    float 
      particleIsolation40;

    int 
      imatchParton;

    float
      parton_pt_max_MC   ,
      parton_eta_MC      ,
      parton_phi_MC      ,
      parton_barcode_MC  ,
      parton_pdg_MC      ;

  } ;

}

#endif // PHOTONANALYSISUTILS_PAUGAMMAATTR_H
