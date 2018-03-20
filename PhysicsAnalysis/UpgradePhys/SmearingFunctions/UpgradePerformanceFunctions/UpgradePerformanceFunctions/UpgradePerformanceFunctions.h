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

class UpgradePerformanceFunctions: public asg::AsgMetadataTool, public virtual IUpgradePerformanceFunctions {
 public:
  //constructor for athena can be created using special macro
  //Note: if you add a second interface to your tool, you must use: ASG_TOOL_CLASS2( ToolName, Interface1, Interface2) 
  ASG_TOOL_CLASS( UpgradePerformanceFunctions , IUpgradePerformanceFunctions )
  //add another constructor for non-athena use cases
  UpgradePerformanceFunctions( const std::string& name );
  UpgradePerformanceFunctions( const std::string& name, UpgradeLayout layout, double avgMu);
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
  void setPhotonNoiseScaling(float scale) { m_photonNoiseScaling = scale; };
  float getPhotonEfficiency(float ptMeV);
  float getPhotonFakeRate(float ptMeV); // jet faking photon
  float getPhotonPileupFakeRate(float ptMeV);// pileup jet faking photon
  float getElectronToPhotonFakeRate(float ptMeV, float eta);
  float getPhotonEnergyResolution(float eMeV, float eta);
  float getPhotonSmearedEnergy(float eMeV, float eta);
  TLorentzVector getPhotonSmearedVector(const TLorentzVector* init);
  float getPhotonFakeRescaledEnergy(float eMeV) { return eMeV; }; //FIXME: this function does nothing
  float getPhotonFakeRescaledET(float eMeV);
  float getPhotonPileupFakeRescaledET(float eMeV);

  // Muon performance functions
  void setMuonWorkingPoint(MuonCutLevel cutLevel);
  void setMuonHighEtaTagger(bool useIt);
  float getMuonEfficiencyFallback(float ptMeV, float eta);
  float getMuonEfficiency(float ptMeV, float eta, float phi=0);
  // These interface functions support phi for future updates.
  // However, currently, the resolutions are INDEPENDENT of phi, hence a default can be assigned
  float getMuonPtResolution(float ptMeV, float eta, float phi = 0);
  float getMuonQOverPtResolution(float ptMeV, float eta, float phi = 0);
  float getMuonMSPtResolution(float ptMeV, float eta, float phi = 0);
  float getMuonMSQOverPtResolution(float ptMeV, float eta, float phi = 0);
  float getMuonIDPtResolution(float ptMeV, float eta, float phi = 0);
  float getMuonIDQOverPtResolution(float ptMeV, float eta, float phi = 0);

  // Tracking performance functions 
  float getTrackPtResolution(float ptMeV, float eta);
  float getTrackInvPtResolution(float invptMeV, float eta);
  float getTrackPtRelativeResolution(float ptMeV, float eta);

  // Tau performance functions
  void setTauRandomSeed(unsigned seed);
  float getTauEfficiency(float etMeV, float eta, short prong, short wp);
  float getTauFakeRate(float etMeV, float eta, short prong, short wp);
  float getTauEnergyResolution(float eMeV, float eta, short prong);
  float getTauSmearedEnergy(float eMeV, float eta, short prong);

  /// Jet performance functions
  // Random seed affect both smearing and track confirmation (JVT)
  void setJetRandomSeed(unsigned seed);

  // dump jet smearing settings to screen
  void printJetSmearingSettings();

  // Set pT and rapidity jet acceptance criteria
  // Used by getSmearedJets and getSmearedBtaggedJets
  void setJetPtRapidiyCuts(double ptMeV=30e3, double absY=3.8);

  // Master method for anti-kT R=0.4 that takes list of hard-scatter truth jets and
  // a) smear the jets (account for detector resolution)
  // b) overlay pileup jets
  // c) apply pT and rapidity selection according to setPtRapidiyCut
  // d) apply track-confirmation (JVT)
  // e) sort selected jets by pT
  // For usage, see: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/UpgradePerformanceFunctions
  Upgrade::Jets getSmearedJets(Upgrade::Jets hsJets);

  // Same as the above method, but also apply b-tagging to the jets.
  // Note that the input HS truth jets need to be labelled with their truth flavour
  // Upgrade::Jets getSmearedBtaggedJets(Upgrade::Jets hsJets, TString btagAlgo="mv2c10", int btagOP=70);

  // Jet algorithm and constant term penalty affect small-R jet smearing
  void setJetAlgorithm(JetAlgorithm jetAlgo);
  void setConstantTermPenaltyFactor(double DeltaC);

  // Method to get JER for small-R jets (anti-kt R=0.4)
  double getJER(double ptMeV, double eta, double mu, JetAlgorithm jetAlgo);
  // Same as above, but uses m_avgMu and m_jetAlgo
  double getJER(double ptMeV, double eta);

  // Returns smeared anti-kt R=0.4 jet given truth 4-vector as input
  TLorentzVector getSmearedJet(TLorentzVector tjet_MeV);
  TLorentzVector getSmearedJet(double ptMeV, double eta, double phi, double massMeV);

  // Returns smeared large-R jet given truth 4-vector as input and topology, that must be QCD, Wprime or Zprime
  // This was derived using trimmed Anti-kT R=1.0 jets
  TLorentzVector getSmearedLargeRjet(double ptMeV, double eta, double phi, double massMeV, LargeRjetTopology jetTopo=Multijet);
  TLorentzVector getSmearedLargeRjet(TLorentzVector truthLargeRjet_MeV, LargeRjetTopology jetTopo=Multijet);

  // Returns vector of {JER,JMS,JMR} - jet energy resolution, mass scale (median) and mass resolution (68% quantile interval)
  std::vector<double> getLargeRjet_JER_JMS_JMR(double ptMeV, double massMeV, LargeRjetTopology jetTopo=Multijet);

  // Returns each of the component of largeR jet smearing
  double getLargeRjetJER(double ptMeV, double massMeV, LargeRjetTopology jetTopo=Multijet) { return getLargeRjet_JER_JMS_JMR(ptMeV,massMeV,jetTopo)[0]; }
  double getLargeRjetJMS(double ptMeV, double massMeV, LargeRjetTopology jetTopo=Multijet) { return getLargeRjet_JER_JMS_JMR(ptMeV,massMeV,jetTopo)[1]; }
  double getLargeRjetJMR(double ptMeV, double massMeV, LargeRjetTopology jetTopo=Multijet) { return getLargeRjet_JER_JMS_JMR(ptMeV,massMeV,jetTopo)[2]; }
  
  /*
   *  These four methods are obsolete!
   */
  float getJetSmearedEnergy(float ptMeV, float eta, bool useJetArea);
  float getJetEnergyResolution(float ptMeV, float eta, bool useJetArea);
  float getJetPtThresholdForFakeRate(float eta, float fakeRate, bool useTrackConfirmation);
  float getJetSmearedMass(float ptMeV, float massMeV, FatJetType jetType);

  // Missing ET performance functions
  void setMETRandomSeed(unsigned seed);
  void loadMETHistograms(TString filename);
  typedef std::pair<double,double> MET;
  MET getMETSmeared(float truthSumEtMeV, float METxMeV, float METyMeV, METSyst systValue=nominal);
  float getMETResolution(float sumEtMeV, METSyst systValue=nominal);  
  float getSumETwithPileup(float truthSumEtMeV, METSyst systValue=nominal);
  TVector3 getTSTsmearing(TVector3 pthard);

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

  // Pileup jet tagging efficiency
  // 1. big all-in-one function
  float getTrackJetConfirmEff(float ptMeV, float eta, TString jetType);

  // 2. Sub methods, that does the same job
  double getJVTeff(const Upgrade::Jet jet_MeV);
  double getJVTeff_HSjet(double ptMeV, double eta);
  double getJVTeff_PUjet(double ptMeV, double eta);
  

  // HGTD setters
  void setUseHGTD0(bool usehgtd0 = false); // turn on the HGTD in the forward region for eta 2.4 - 4.3
  void setUseHGTD1(bool usehgtd1 = false); // turn on the HGTD in the forward region for eta 0.0 - 4.3
  float getHGTDMinEta(); // returns minimum eta for the HGTD depending on the scenario (0/1)
  float getHGTDMaxEta(); // returns maximum eta for the HGTD, currently 4.3
  void extendJetTrigger(bool exttrig = false); // nominal = 3.2, exttrig = true => 3.8

  void setChargeFlipRateFilename(TString chargeFlipFleName) { m_chargeFlipRateFilename = chargeFlipFleName; }


 private:
  // Helper method to create a TGraph from vectors
  TGraph *makeGraph(std::vector<double> x, std::vector<double> y);

  // Member variables
  UpgradeLayout m_layout;
  double m_avgMu;
  ElectronCutLevel m_eleCutLevel;
  PhotonCutLevel m_photonCutLevel;
  MuonCutLevel m_muonCutLevel;
  PileupEff m_pueff = PU;
  TString m_puPath;
  bool m_puInitialized = false;

  float m_BronzeEtaMax = 2.4;
  float m_SilverEtaMax = 3.2;
  float m_GoldEtaMax = 3.8;
  static const int m_nPUPtBins = 4;

  float m_fEff = 0.02;
  bool m_bUseTrackConf = true;
  float fEffNominalArray[m_nPUPtBins];
  float fEffMediumArray[m_nPUPtBins];
  float fEffExtendedArray[m_nPUPtBins];
  JetAlgorithm m_jetAlgo = TopoEM;
  double m_JER_Cterm_penalty = 0.01;

  double m_jet_pTcut = 30e3;
  double m_jet_absYcut = 3.8;

  // Step 1.6 onwards TC implementation
  TF1 *func_TC_lt50;
  TF1 *func_TC_gt50;
  TF1 *func_IH_lt50;
  TF1 *func_IH_gt50;


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
  TGraphErrors* m_met_resoperp;  //run2 parametrizations 
  TGraphErrors* m_met_resopara; 
  TGraphErrors* m_met_shiftpara;

  // Flavour tagging filename
  TString m_flavourTaggingCalibrationFilename;

  // Electron energy resolution filename
  TString m_electronEnergyResolutionFilename = "";

  // Charge-flip rate filename
  TString m_chargeFlipRateFilename = "";

  // Pileup overlay templates
  TTree *m_pileupTree30;
  std::vector<float> *m_pileupPt30,*m_pileupEta30,*m_pileupPhi30,*m_pileupE30;

  float muonEtaTriggerEfficiency(float eta);

  void initializePileupTemplates(TString pileupTemplateFilename); // needs to be called by "setAvgMu" - PU Libraries only for mu 140 and 200 so far

  // Photon globals
  std::vector<TH1D*> hsfakes;
  std::vector<TH1D*> pufakes;
  float m_photonNoiseScaling;
  // since the UPF class doesn't seem to have a non-default destructor, we use smart pointers to make sure the muon tools get deleted
  bool setupMuonResProvider();
  std::unique_ptr<MuonMomentumResProvider> m_muonRes;
  bool setupMuonEffProvider();
  std::unique_ptr<MuonEffProvider> m_muonEff;
  bool m_useMuonHighEta;


};

#endif
