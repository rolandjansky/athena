/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UPGRADEPERFORMANCEFUNCTIONS_UPGRADEPERFORMANCEFUNCTIONS_H
#define UPGRADEPERFORMANCEFUNCTIONS_UPGRADEPERFORMANCEFUNCTIONS_H 1

// Framework include(s) - base class
#include "AsgTools/AsgMetadataTool.h"

// Interface class
#include "UpgradePerformanceFunctions/IUpgradePerformanceFunctions.h"

#include <iostream>
#include <string>
#include <memory>
#include <math.h>

#include "TF1.h"
#include "TRandom3.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "UpgradePerformanceFunctions/MuonMomentumResProvider.h"
#include "UpgradePerformanceFunctions/MuonEffProvider.h"

namespace Upgrade {

class UpgradePerformanceFunctions: public asg::AsgMetadataTool, public virtual IUpgradePerformanceFunctions {
  
 public:
  //constructor for athena can be created using special macro
  //Note: if you add a second interface to your tool, you must use: ASG_TOOL_CLASS2( ToolName, Interface1, Interface2) 
  ASG_TOOL_CLASS( UpgradePerformanceFunctions , IUpgradePerformanceFunctions )
  //add another constructor for non-athena use cases
  UpgradePerformanceFunctions( const std::string& name );
  UpgradePerformanceFunctions( const std::string& name, UpgradeLayout layout, double avgMu);
  ~UpgradePerformanceFunctions() {};

  // Function initialising the tool
  StatusCode initialize() override final;

#ifdef XAOD_STANDALONE
  UpgradeLayout getLayout() override final;
#else
  int getLayout() override final;
#endif
  double getAvgMu() override final;

  // Electron performance functions
  float getElectronEfficiency(float ptMeV, float eta) override;
  float getElectronFakeRate(float ptMeV, float eta) override;
  float getElectronEnergyResolution(float eMeV, float eta) override;
  float getElectronSmearedEnergy(float eMeV, float eta) override;
  float getElectronFakeRescaledEnergy(float eMeV, float eta) override; 
  float getElectronChargeFlipProb(float eMeV, float eta) override;
  float interpolateElectronIDMap(float *ptEtaMap,float pT,float eta) override;
  float interpolateElectronResolutionMap(float *ptEtaMap,float pT,float eta) override;

  // Photon performance functions
  StatusCode initPhotonFakeHistograms() override;
  float getPhotonEfficiency(float ptMeV) override;
  float getPhotonFakeRate(float ptMeV) override; // jet faking photon
  float getPhotonPileupFakeRate(float ptMeV) override;// pileup jet faking photon
  float getElectronToPhotonFakeRate(float ptMeV, float eta) override;
  float getPhotonEnergyResolution(float eMeV, float eta) override;
  float getPhotonSmearedEnergy(float eMeV, float eta) override;
  TLorentzVector getPhotonSmearedVector(const TLorentzVector* init) override;
  float getPhotonFakeRescaledEnergy(float eMeV) override { return eMeV; }; //FIXME: this function does nothing
  float getPhotonFakeRescaledET(float eMeV) override;
  float getPhotonPileupFakeRescaledET(float eMeV) override;

  // Muon performance functions
  float getMuonEfficiencyFallback(float ptMeV, float eta) override;
  float getMuonEfficiency(float ptMeV, float eta, float phi=0) override;
  // These interface functions support phi for future updates.
  // However, currently, the resolutions are INDEPENDENT of phi, hence a default can be assigned
  float getMuonPtResolution(float ptMeV, float eta, float phi = 0) override;
  float getMuonQOverPtResolution(float ptMeV, float eta, float phi = 0) override;
  float getMuonMSPtResolution(float ptMeV, float eta, float phi = 0) override;
  float getMuonMSQOverPtResolution(float ptMeV, float eta, float phi = 0) override;
  float getMuonIDPtResolution(float ptMeV, float eta, float phi = 0) override;
  float getMuonIDQOverPtResolution(float ptMeV, float eta, float phi = 0) override;

  // Tracking performance functions 
  float getTrackPtResolution(float ptMeV, float eta) override;
  float getTrackInvPtResolution(float invptMeV, float eta) override;
  float getTrackPtRelativeResolution(float ptMeV, float eta) override;

  // Tau performance functions
  float getTauEfficiency(float etMeV, float eta, short prong, short wp) override;
  float getTauFakeRate(float etMeV, float eta, short prong, short wp) override;
  float getTauEnergyResolution(float eMeV, float eta, short prong) override;
  float getTauSmearedEnergy(float eMeV, float eta, short prong) override;

  // dump jet smearing settings to screen
  void printJetSmearingSettings() override;

  // Set pT and rapidity jet acceptance criteria
  // Used by getSmearedJets and getSmearedBtaggedJets
  void setJetPtRapidiyCuts(double ptMeV=30e3, double absY=3.8) override;

  // Master method for anti-kT R=0.4 that takes list of hard-scatter truth jets and
  // a) smear the jets (account for detector resolution)
  // b) overlay pileup jets
  // c) apply pT and rapidity selection according to setPtRapidiyCut
  // d) apply track-confirmation (JVT)
  // e) sort selected jets by pT
  // For usage, see: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/UpgradePerformanceFunctions
  Upgrade::Jets getSmearedJets(Upgrade::Jets hsJets) override;

  // Same as the above method, but also apply b-tagging to the jets.
  // Note that the input HS truth jets need to be labelled with their truth flavour
  // Upgrade::Jets getSmearedBtaggedJets(Upgrade::Jets hsJets, TString btagAlgo="mv2c10", int btagOP=70);

  // Jet algorithm and constant term penalty affect small-R jet smearing
  void setJetAlgorithm(JetAlgorithm jetAlgo) override;
  void setConstantTermPenaltyFactor(double DeltaC) override;

  // Method to get JER for small-R jets (anti-kt R=0.4)
  double getJER(double ptMeV, double eta, double mu, JetAlgorithm jetAlgo) override;
  // Same as above, but uses m_avgMu and m_jetAlgo
  double getJER(double ptMeV, double eta) override;

  // Returns smeared anti-kt R=0.4 jet given truth 4-vector as input
  TLorentzVector getSmearedJet(TLorentzVector tjet_MeV) override;
  TLorentzVector getSmearedJet(double ptMeV, double eta, double phi, double massMeV) override;

  // Returns smeared large-R jet given truth 4-vector as input and topology, that must be QCD, Wprime or Zprime
  // This was derived using trimmed Anti-kT R=1.0 jets
  TLorentzVector getSmearedLargeRjet(double ptMeV, double eta, double phi, double massMeV, LargeRjetTopology jetTopo=Multijet) override;
  TLorentzVector getSmearedLargeRjet(TLorentzVector truthLargeRjet_MeV, LargeRjetTopology jetTopo=Multijet) override;

  // Returns vector of {JER,JMS,JMR} - jet energy resolution, mass scale (median) and mass resolution (68% quantile interval)
  std::vector<double> getLargeRjet_JER_JMS_JMR(double ptMeV, double massMeV, LargeRjetTopology jetTopo=Multijet) override;

  // Returns each of the component of largeR jet smearing
  double getLargeRjetJER(double ptMeV, double massMeV, LargeRjetTopology jetTopo=Multijet) override { return getLargeRjet_JER_JMS_JMR(ptMeV,massMeV,jetTopo)[0]; }
  double getLargeRjetJMS(double ptMeV, double massMeV, LargeRjetTopology jetTopo=Multijet) override { return getLargeRjet_JER_JMS_JMR(ptMeV,massMeV,jetTopo)[1]; }
  double getLargeRjetJMR(double ptMeV, double massMeV, LargeRjetTopology jetTopo=Multijet) override { return getLargeRjet_JER_JMS_JMR(ptMeV,massMeV,jetTopo)[2]; }
  
  /*
   *  These four methods are obsolete!
   */
  float getJetSmearedEnergy(float ptMeV, float eta, bool useJetArea) override;
  float getJetEnergyResolution(float ptMeV, float eta, bool useJetArea) override;
  float getJetPtThresholdForFakeRate(float eta, float fakeRate, bool useTrackConfirmation) override;
  float getJetSmearedMass(float ptMeV, float massMeV, FatJetType jetType) override;

  // Missing ET performance functions
  StatusCode loadMETHistograms() override;
  typedef std::pair<double,double> MET;
  MET getMETSmeared(float truthSumEtMeV, float METxMeV, float METyMeV, METSyst systValue=nominal) override;
  float getMETResolution(float sumEtMeV, METSyst systValue=nominal) override;  
  float getSumETwithPileup(float truthSumEtMeV, METSyst systValue=nominal) override;
  TVector3 getTSTsmearing(TVector3 pthard) override;

  // Flavour-tagging performance functions
  float getFlavourTagEfficiency(float ptMeV, float eta, char flavour, TString tagger = "mv1", int operating_point = 70, bool track_confirmation = false) override;

  // Trigger performance functions
  float getSingleElectronTriggerEfficiency(float ptMeV, float eta) override;
  float getDiElectronTriggerEfficiency(float pt1MeV, float pt2MeV, float eta1, float eta2) override;
  float getSinglePhotonTriggerEfficiency(float etMeV, float eta) override;
  float getDiPhotonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2) override;
  float getSingleMuonTriggerEfficiency(float etMeV, float eta) override;
  float getDiMuonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2) override;
  float getElectronMuonTriggerEfficiency(float elecEtMeV, float muonEtMeV, float elecEta, float muonEta) override;
  float getSingleTauTriggerEfficiency(float etMeV, float eta, short prong) override;
  float getDiTauTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2, short prong1, short prong2) override;
  float getSingleJetTriggerEfficiency(float smearedEtMeV, float eta) override;
  float getThreeJetTriggerEfficiency(float smearedEtMeV1, float eta1, float smearedEtMeV2, float eta2, float smearedEtMeV3, float eta3) override;
  float getFourJetTriggerEfficiency(float smearedEtMeV1, float eta1, float smearedEtMeV2, float eta2, float smearedEtMeV3, float eta3, float smearedEtMeV4, float eta4) override;
  float getHTTriggerEfficiency(float smearedHTMeV) override;
  float getEtmissTriggerEfficiency(float smearedEtmissMeV) override;
  float getJetEtmissTriggerEfficiency(float smearedJetEtMeV, float jetEta, float smearedEtmissMeV) override;

  // Pile-up overlay functions
  std::string setPileupTemplatesPath() override;
  StatusCode initializePileupTemplates() override;
  bool getPileupTrackConfSetting() override;
  float getPileupJetPtThresholdMeV() override;
  std::vector<TLorentzVector> getPileupJets() override;

  // Pileup jet tagging efficiency
  // 1. big all-in-one function
  float getTrackJetConfirmEff(float ptMeV, float eta, TString jetType) override;

  // 2. Sub methods, that does the same job
  double getJVTeff(const Upgrade::Jet jet_MeV) override;
  double getJVTeff_HSjet(double ptMeV, double eta) override;
  double getJVTeff_PUjet(double ptMeV, double eta) override;
  
  // HGTD setters
  float getHGTDMinEta() override; // returns minimum eta for the HGTD depending on the scenario (0/1)
  float getHGTDMaxEta() override; // returns maximum eta for the HGTD, currently 4.3
  void extendJetTrigger(bool exttrig = false) override; // nominal = 3.2, exttrig = true => 3.8

 private:
  // Helper method to create a TGraph from vectors
  TGraph* makeGraph(std::vector<double> x, std::vector<double> y);
  bool setupMuonResProvider() override;
  bool setupMuonEffProvider() override;

  // Internal tools
  std::unique_ptr<MuonMomentumResProvider> m_muonRes;
  std::unique_ptr<MuonEffProvider> m_muonEff;

  // General properties
#ifdef XAOD_STANDALONE // more convenient for property setting
  UpgradeLayout m_layout;
#else
  int m_layout;
#endif
  double m_avgMu;

  // Electron properties
#ifdef XAOD_STANDALONE // more convenient for property setting
  ElectronCutLevel m_eleCutLevel;
#else
  int m_eleCutLevel;
#endif

  // Muon properties
#ifdef XAOD_STANDALONE // more convenient for property setting
  MuonCutLevel m_muonCutLevel;
#else
  int m_muonCutLevel;
#endif
  bool m_useMuonHighEta;

  // Photon properties
#ifdef XAOD_STANDALONE // more convenient for property setting
  PhotonCutLevel m_photonCutLevel;
#else
  int m_photonCutLevel;
#endif
  float m_photonNoiseScaling;

  // Pileup properties
  bool m_bUseTrackConf; 
  float m_fPileupJetThresholdMeV;
  float m_fEff;

#ifdef XAOD_STANDALONE // more convenient for property setting
  PileupEff m_pueff;
#else
  int m_pueff;
#endif

  // HGTD properties
  bool m_bUseHGTD0;
  bool m_bUseHGTD1;

  // WarmToroid properties
  bool m_useWarmToroid;

  static const int m_nPUPtBins = 4;
  float fEffNominalArray[m_nPUPtBins];
  float fEffMediumArray[m_nPUPtBins];
  float fEffExtendedArray[m_nPUPtBins];

  JetAlgorithm m_jetAlgo = TopoEM;
  
  double m_JER_Cterm_penalty = 0.01;

  double m_jet_pTcut = 30e3;
  double m_jet_absYcut = 3.8;

  // Step 1.6 onwards TC implementation
  std::unique_ptr<TF1> func_TC_lt50_central;
  std::unique_ptr<TF1> func_TC_gt50_central;
  std::unique_ptr<TF1> func_TC_lt50_forward;
  std::unique_ptr<TF1> func_TC_gt50_forward;
  std::unique_ptr<TF1> func_IH_lt50_central;
  std::unique_ptr<TF1> func_IH_gt50_central;
  std::unique_ptr<TF1> func_IH_lt50_forward;
  std::unique_ptr<TF1> func_IH_gt50_forward;


  // -- new stuff for HGTD --> to be included into the Golden Scenario
  // -- first set of preliminary working points to contain only flat factors of 
  //       - 90% HS vs 10% or 50% PU
  //       - 80% HS vs 10% or 50% PU
  // -- the central region has to stay the same and correspond to the gold performance
  //     -- etamin_HGTD = 2.4
  //     -- etamax_HGTD = 5.0
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

  float m_fPileupTCMaxPtMeV;
  
  float m_JetTrigMaxEta = 3.2;
  float m_JetTrig1jMin = 180000.;
  float m_JetTrig3jMin = 85000.;
  float m_JetTrig4jMin = 75000.;

  // Missing ET templates
  TH1F  *m_SumEtH[4][6];

  std::unique_ptr<TGraphErrors> m_met_resoperp;
  std::unique_ptr<TGraphErrors> m_met_resopara; 
  std::unique_ptr<TGraphErrors> m_met_shiftpara;

  // Pileup overlay templates
  std::unique_ptr<TTree> m_pileupTree30;
  std::vector<float> *m_pileupPt30,*m_pileupEta30,*m_pileupPhi30,*m_pileupE30;

  float muonEtaTriggerEfficiency(float eta) override;

  void initializePileupTemplates(std::string pileupTemplateFilename); // needs to be called by "setAvgMu" - PU Libraries only for mu 140 and 200 so far

  // Photon globals
  std::vector<TH1D*> hsfakes;
  std::vector<TH1D*> pufakes;

  // Random seeds
  int m_electronRandomSeed;
  int m_photonRandomSeed;
  int m_tauRandomSeed;
  int m_jetRandomSeed;
  int m_metRandomSeed;
  int m_pileupRandomSeed;

  // Smearing filenames
  std::string m_flavourTaggingCalibrationFilename;
  std::string m_electronEnergyResolutionFilename;
  std::string m_muonEnergyResolutionFilename;
  std::string m_photonFakeFilename;
  std::string m_chargeFlipRateFilename;
  std::string m_METFilename;
  std::string m_puPath;

  // Below -- variables that are not meant to be configured

  // Flag to check for tool initialisation
  bool m_tool_init;
  
  // Default TRandom3 seed is 4357
  TRandom3 m_electronRandom;
  TRandom3 m_photonRandom;
  TRandom3 m_tauRandom;
  TRandom3 m_jetRandom;
  TRandom3 m_METRandom;
  TRandom3 m_pileupRandom;

  // Pileup overlay eta ranges
  const float m_BronzeEtaMax = 2.4;
  const float m_SilverEtaMax = 3.2;
  const float m_GoldEtaMax = 3.8;

};

} //namespace upgrade

#endif
