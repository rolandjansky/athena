/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UPGRADEPERFORMANCEFUNCTIONS_H
#define UPGRADEPERFORMANCEFUNCTIONS_H

#include <iostream>
#include <string>
#include <math.h>

#include "TH2D.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TLorentzVector.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TTree.h"

class UpgradePerformanceFunctions {

 public:

  enum UpgradeLayout {LoI, LoIVF, gold, silver, bronze, InclBrl, ExtBrl, Step1p6};

  enum ElectronCutLevel {looseElectron, mediumElectron, tightElectron}; 
  enum PhotonCutLevel{loosePhoton, tightPhoton}; 
  enum MuonCutLevel {looseMuon, tightMuon}; 
  enum METSyst {nominal, process, thresholdUp, thresholdDown, resoUp, resoDown}; 
  enum PileupEff{HS,PU};

  UpgradePerformanceFunctions();
  UpgradePerformanceFunctions(UpgradeLayout layout, double avgMu);
  ~UpgradePerformanceFunctions() {};

  void setLayout(UpgradeLayout layout);
  void setAvgMu(double avgMu);
  UpgradeLayout getLayout();
  double getAvgMu();

  // Electron performance functions
  void setElectronWorkingPoint(ElectronCutLevel cutLevel);
  void setElectronRandomSeed(unsigned seed);
  float getElectronEfficiency(float ptMeV, float eta);
  float getElectronFakeRate(float ptMeV, float eta);
  float getElectronEnergyResolution(float eMeV, float eta);
  float getElectronSmearedEnergy(float eMeV, float eta);
  float getElectronFakeRescaledEnergy(float eMeV, float eta); 
  float getElectronChargeFlipProb(float eMeV, float eta);
  float interpolateElectronIDMap(float *ptEtaMap,float pT,float eta);
  float interpolateElectronResolutionMap(float *ptEtaMap,float pT,float eta);

  // Photon performance functions
  void setPhotonWorkingPoint(PhotonCutLevel cutLevel);
  void initPhotonFakeHistograms(TString PhotonFakePath);
  void setPhotonRandomSeed(unsigned seed);
  float getPhotonEfficiency(float ptMeV);
  float getPhotonFakeRate(float ptMeV); // jet faking photon
  float getPhotonPileupFakeRate(float ptMeV);// pileup jet faking photon
  float getElectronToPhotonFakeRate(float ptMeV, float eta);
  float getPhotonEnergyResolution(float eMeV, float eta);
  float getPhotonSmearedEnergy(float eMeV, float eta);
  TLorentzVector getPhotonSmearedVector(const TLorentzVector* init);
  float getPhotonFakeRescaledEnergy(float eMeV);
  float getPhotonFakeRescaledET(float eMeV);
  float getPhotonPileupFakeRescaledET(float eMeV);

  // Muon performance functions
  void setMuonWorkingPoint(MuonCutLevel cutLevel);
  float getMuonEfficiency(float ptMeV, float eta);
  float getMuonPtResolution(float ptMeV, float eta);
  float getMuonQOverPtResolution(float ptMeV, float eta);

  // Tracking performance functions
  float getTrackPtResolution(float ptMeV, float eta);
  float getTrackInvPtResolution(float invptMeV, float eta);
  float getTrackPtRelativeResolution(float ptMeV, float eta);

  // Tau performance functions
  void setTauRandomSeed(unsigned seed);
  float getTauEfficiency(float etMeV, float eta, short prong);
  float getTauFakeRate(float etMeV, float eta, short prong);
  float getTauEnergyResolution(float eMeV, float eta, short prong);
  float getTauSmearedEnergy(float eMeV, float eta, short prong);

  // Jet performance functions
  void setJetRandomSeed(unsigned seed);
  float getJetSmearedEnergy(float ptMeV, float eta, bool useJetArea);
  float getJetEnergyResolution(float ptMeV, float eta, bool useJetArea);
  float getJetPtThresholdForFakeRate(float eta, float fakeRate, bool useTrackConfirmation);

  // Missing ET performance functions
  void setMETRandomSeed(unsigned seed);
  void loadMETHistograms(TString filename);
  typedef std::pair<double,double> MET;
  MET getMETSmeared(float sumEtMeV, float METxMeV, float METyMeV, METSyst systValue=nominal);
  float getMETResolution(float sumEtMeV, METSyst systValue=nominal);  

  // Flavour-tagging performance functions
  void setFlavourTaggingCalibrationFilename(TString flavourTaggingCalibrationFilename);
  float getFlavourTagEfficiency(float ptMeV, float eta, char flavour, TString tagger = "mv1", int operating_point = 70, bool track_confirmation = false);

  // Trigger performance functions
  float getSingleElectronTriggerEfficiency(float ptMeV, float eta);
  float getDiElectronTriggerEfficiency(float pt1MeV, float pt2MeV, float eta1, float eta2);
  float getSinglePhotonTriggerEfficiency(float etMeV, float eta);
  float getDiPhotonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2);
  float getSingleMuonTriggerEfficiency(float etMeV, float eta);
  float getDiMuonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2);
  float getElectronMuonTriggerEfficiency(float elecEtMeV, float muonEtMeV, float elecEta, float muonEta);
  float getSingleTauTriggerEfficiency(float etMeV, float eta, short prong);
  float getDiTauTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2, short prong1, short prong2);
  float getSingleJetTriggerEfficiency(float smearedEtMeV, float eta);
  float getThreeJetTriggerEfficiency(float smearedEtMeV1, float eta1, float smearedEtMeV2, float eta2, float smearedEtMeV3, float eta3);
  float getFourJetTriggerEfficiency(float smearedEtMeV1, float eta1, float smearedEtMeV2, float eta2, float smearedEtMeV3, float eta3, float smearedEtMeV4, float eta4);
  float getHTTriggerEfficiency(float smearedHTMeV);
  float getEtmissTriggerEfficiency(float smearedEtmissMeV);
  float getJetEtmissTriggerEfficiency(float smearedJetEtMeV, float jetEta, float smearedEtmissMeV);

  // Pile-up overlay functions
  void setPileupRandomSeed(unsigned seed);
  void setPileupEfficiencyScheme(PileupEff puscheme);
  void setPileupEff(float hseff = 0.9); // set efficiency either for HS or PU depending on your scheme 
  void setPileupUseTrackConf(bool usetr = false); // TC yes/no - user's choice
  void setPileupJetPtThresholdMeV(float ptThresholdMeV=30000.); // default approved minimum of 30 GeV
  void setPileupTemplatesPath(TString pileupTemplateFilenamePath); // user provides folder with PULibraries

  bool getPileupTrackConfSetting();
  float getPileupJetPtThresholdMeV();
  std::vector<TLorentzVector> getPileupJets();
  float getTrackJetConfirmEff(float ptMeV, float eta, TString jetType);

  // HGTD setters
  void setUseHGTD0(bool usehgtd0 = false); // turn on the HGTD in the forward region for eta 2.4 - 4.3
  void setUseHGTD1(bool usehgtd1 = false); // turn on the HGTD in the forward region for eta 0.0 - 4.3
  float getHGTDMinEta(); // returns minimum eta for the HGTD depending on the scenario (0/1)
  float getHGTDMaxEta(); // returns maximum eta for the HGTD, currently 4.3
  void extendJetTrigger(bool exttrig = false); // nominal = 3.2, exttrig = true => 3.8

 private:

  // Member variables
  UpgradeLayout m_layout;
  double m_avgMu;
  ElectronCutLevel m_eleCutLevel;
  PhotonCutLevel m_photonCutLevel;
  MuonCutLevel m_muonCutLevel;
  PileupEff m_pueff;

  float m_BronzeEtaMax = 2.4;
  float m_SilverEtaMax = 3.2;
  float m_GoldEtaMax = 3.8;
  static const int m_nPUPtBins = 4;

  float m_fEff;
  bool m_bUseTrackConf;
  float m_effNominalArray[m_nPUPtBins];
  float m_effMediumArray[m_nPUPtBins];
  float m_effExtendedArray[m_nPUPtBins];

  // -- new stuff for HGTD --> to be included into the Golden Scenario
  // -- first set of preliminary working points to contain only flat factors of 
  //       - 90% HS vs 10% or 50% PU
  //       - 80% HS vs 10% or 50% PU
  // -- the central region has to stay the same and correspond to the gold performance
  //     -- etamin_HGTD = 2.4
  //     -- etamax_HGTD = 5.0

  bool m_bUseHGTD0 = false;
  bool m_bUseHGTD1 = false;
  float m_HGTD0min = 2.4;
  float m_HGTD1min = 0.0;
  float m_HGTDmax = 4.3;
  float m_HGTDHSEff;
  float m_HGTDPUEff;
  bool m_ExtHGTDTrig = false;
  float m_HGTDMaxTrigEta = 3.8;
  float m_HGTD0MinTrigPt    = 71000.;
  float m_HGTD0MinExtTrigPt = 72000.;
  float m_HGTD1MinTrigPt    = 58000.;
  float m_HGTD1MinExtTrigPt = 60000.;

  float m_fPileupJetThresholdMeV;
  float m_fPileupTCMaxPtMeV;
  
  float m_JetTrigMaxEta = 3.2;
  float m_JetTrig1jMin = 180000.;
  float m_JetTrig3jMin = 85000.;
  float m_JetTrig4jMin = 75000.;

  // Default TRandom3 seed is 4357
  TRandom3 m_electronRandom;
  TRandom3 m_photonRandom;
  TRandom3 m_tauRandom;
  TRandom3 m_jetRandom;
  TRandom3 m_METRandom;
  TRandom3 m_pileupRandom;

  // Missing ET templates
  TH1F  *m_SumEtH[4][6];
  TGraphErrors *m_Ress[4];

  // Flavour tagging filename
  TString m_flavourTaggingCalibrationFilename;

  // Pileup overlay templates
  TTree *m_pileupTree30;
  std::vector<float> *m_pileupPt30,*m_pileupEta30,*m_pileupPhi30,*m_pileupE30;

  float muonEtaTriggerEfficiency(float eta);

  void initializePileupTemplates(TString pileupTemplateFilename); // needs to be called by "setAvgMu" - PU Libraries only for mu 140 and 200 so far

  // Photon globals
  std::vector<TH1D*> m_hsfakes;
  std::vector<TH1D*> m_pufakes;

};

#endif
