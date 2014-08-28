/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PAUegammaAttr.h, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////
//
// This class contains additional attributes that apply to PAU::egamma
// objects -- only from reconstruction
//

#ifndef PHOTONANALYSISUTILS_PAUEGAMMAATTR_H
#define PHOTONANALYSISUTILS_PAUEGAMMAATTR_H

#include <vector>
#include "PhotonAnalysisUtils/PAUnamespace.h"

class Jet ;
namespace Rec { class TrackParticle ; }
class EmTau_ROI ;
class TrigEMCluster ;
class TrigElectron ;
class egamma ;
class CaloCluster ;
namespace PAU {

  //-------------------------------------------------------------------------------------
  // class PAU::egammaAttr:
  class egammaAttr {
  public: // methods
    egammaAttr() :
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
      , etcone10_corrected(UNKNOWN)
      , etcone15_corrected(UNKNOWN)
      , etcone20_corrected(UNKNOWN)
      , etcone25_corrected(UNKNOWN)
      , etcone30_corrected(UNKNOWN)
      , etcone35_corrected(UNKNOWN)
      , etcone40_corrected(UNKNOWN)
      , etcone10_corrected_new(UNKNOWN)
      , etcone15_corrected_new(UNKNOWN)
      , etcone20_corrected_new(UNKNOWN)
      , etcone25_corrected_new(UNKNOWN)
      , etcone30_corrected_new(UNKNOWN)
      , etcone35_corrected_new(UNKNOWN)
      , etcone40_corrected_new(UNKNOWN)
      , etcone10_pt_corrected(UNKNOWN)
      , etcone15_pt_corrected(UNKNOWN)
      , etcone20_pt_corrected(UNKNOWN)
      , etcone25_pt_corrected(UNKNOWN)
      , etcone30_pt_corrected(UNKNOWN)
      , etcone35_pt_corrected(UNKNOWN)
      , etcone40_pt_corrected(UNKNOWN)
      , etcone10_ED_corrected(UNKNOWN)
      , etcone15_ED_corrected(UNKNOWN)
      , etcone20_ED_corrected(UNKNOWN)
      , etcone25_ED_corrected(UNKNOWN)
      , etcone30_ED_corrected(UNKNOWN)
      , etcone35_ED_corrected(UNKNOWN)
      , etcone40_ED_corrected(UNKNOWN)
      , etcone10_ED_corrected_new(UNKNOWN)
      , etcone15_ED_corrected_new(UNKNOWN)
      , etcone20_ED_corrected_new(UNKNOWN)
      , etcone25_ED_corrected_new(UNKNOWN)
      , etcone30_ED_corrected_new(UNKNOWN)
      , etcone35_ED_corrected_new(UNKNOWN)
      , etcone40_ED_corrected_new(UNKNOWN)
      , ED_sigma(UNKNOWN)
      , ED_median(UNKNOWN)
      , ED_Njets(UNKNOWN)
      , ED_sigma_new(UNKNOWN)
      , ED_median_new(UNKNOWN)
      , ED_Njets_new(UNKNOWN)
      , etcone40_ED0_corrected(UNKNOWN)
      , etcone40_ED1_corrected(UNKNOWN)
      , etcone40_ED2_corrected(UNKNOWN)
      , etcone40_ED3_corrected(UNKNOWN)
      , etcone40_ED4_corrected(UNKNOWN)
      , etcone40_ED5_corrected(UNKNOWN)
      , transmin_etcone40(UNKNOWN)
      , transmax_etcone40(UNKNOWN)
      , transmin_etcone100(UNKNOWN)
      , transmax_etcone100(UNKNOWN)
      , transmin(UNKNOWN)
      , transmax(UNKNOWN)
      , transmin_loweta(UNKNOWN)
      , transmax_loweta(UNKNOWN)
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
      , CaloPointing_zvertex(UNKNOWN), CaloPointing_sigma_zvertex(UNKNOWN), CaloPointing_eta(UNKNOWN), CaloPointing_sigma_eta(UNKNOWN)
      , convChi2(UNKNOWN), convDcottheta(UNKNOWN), convDphi(UNKNOWN)
      , convDist(UNKNOWN), convDR1R2(UNKNOWN)
      , convp(UNKNOWN), convtheta(UNKNOWN), convphi(UNKNOWN)
      , convperr(UNKNOWN), convthetaerr(UNKNOWN), convphierr(UNKNOWN)
      , convp_GSF(UNKNOWN), convtheta_GSF(UNKNOWN), convphi_GSF(UNKNOWN)
      , convperr_GSF(UNKNOWN), convthetaerr_GSF(UNKNOWN), convphierr_GSF(UNKNOWN)
      , convp_DNA(UNKNOWN), convtheta_DNA(UNKNOWN), convphi_DNA(UNKNOWN)
      , convperr_DNA(UNKNOWN), convthetaerr_DNA(UNKNOWN), convphierr_DNA(UNKNOWN)
      {} ;
    ~egammaAttr() {} ;
  public: // data members
    int matchPassL1, matchPassL2, matchPassEF ;
    int matchPassL1_ph, matchPassL2_ph, matchPassEF_ph ;
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
      etcone10_corrected_new, etcone15_corrected_new,
      etcone20_corrected_new, etcone25_corrected_new,
      etcone30_corrected_new, etcone35_corrected_new,
      etcone40_corrected_new;
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
      etcone10_ED_corrected_new, etcone15_ED_corrected_new,
      etcone20_ED_corrected_new, etcone25_ED_corrected_new,
      etcone30_ED_corrected_new, etcone35_ED_corrected_new,
      etcone40_ED_corrected_new;
    float 
      ED_sigma, ED_median;
    int 
      ED_Njets;
    float 
      ED_sigma_new, ED_median_new;
    int 
      ED_Njets_new;
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
    float L1_bestMatchDeltaR_ph, L2_bestMatchDeltaR_ph, EF_bestMatchDeltaR_ph;

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
      convTrk1_ToTvar,
      convTrk2_ToTvar,
      convTrk1_trtPID,
      convTrk2_trtPID,
      convTrk1_trtPIDHT,
      convTrk2_trtPIDHT,
      convTrk1_trtPIDToT,
      convTrk2_trtPIDToT,
      convTrk1_trtRatioHT,
      convTrk2_trtRatioHT,
      isConv      ,
      Xconv       ,
      Yconv       ,
      PT1conv     ,
      PT2conv     ,
      PTconv      ,
      PZconv      ,
      isRecovered ,
      convTrk1_expectHitInBLayer   ,
      convTrk1_numBLHits   ,
      convTrk1_numPixelHits,
      convTrk1_numSCTHits  ,
      convTrk1_numTRTHits  ,
      convTrk1_numTRTOutliers  ,
      convTrk1_numTRTHTHits  ,
      convTrk1_numTRTHTOutliers  ,
      convTrk1_charge      ,
      convTrk2_expectHitInBLayer   ,
      convTrk2_numBLHits   ,
      convTrk2_numPixelHits,
      convTrk2_numSCTHits  ,
      convTrk2_numTRTHits  ,
      convTrk2_numTRTOutliers  ,
      convTrk2_numTRTHTHits  ,
      convTrk2_numTRTHTOutliers  ,
      convTrk2_charge      ;

    float 
      convChi2, 
      convDcottheta,
      convDphi,
      convDist,
      convDR1R2;

    float
      convp,
      convtheta,
      convphi,
      convperr,
      convthetaerr,
      convphierr,
      convp_GSF,
      convtheta_GSF,
      convphi_GSF,
      convperr_GSF,
      convthetaerr_GSF,
      convphierr_GSF,
      convp_DNA,
      convtheta_DNA,
      convphi_DNA,
      convperr_DNA,
      convthetaerr_DNA,
      convphierr_DNA;

    int 
      isConvMC           ,
      pdgMC              ,
      isG4part           ,
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

    /*
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
      particleIsolation40,
      particleIsolation40_ED_corrected,
      particleIsolation40_ED_corrected_reco;
    */

    int 
      imatchParton;

    float
      parton_pt_max_MC   ,
      parton_eta_MC      ,
      parton_phi_MC      ,
      parton_barcode_MC  ,
      parton_pdg_MC      ;

    float
      convTrk1_chi2      ,
      convTrk2_chi2      ;

    bool
      expectHitInBLayer,
      isRobustLoose,
      isRobustMedium,
      isRobustTight,
      isRobusterTight;

    float 
      EcellS0,
      etacellS0;

    unsigned int
      isGoodOQbits;

    int 
      isGoodOQphoton,
      isGoodOQelectron;
  } ;

}

#endif // PHOTONANALYSISUTILS_PAUEGAMMAATTR_H
