/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UPGRADEPERFORMANCEFUNCTIONS_IUPGRADEPERFORMANCEFUNCTIONS_H
#define UPGRADEPERFORMANCEFUNCTIONS_IUPGRADEPERFORMANCEFUNCTIONS_H 1

#include "AsgTools/IAsgTool.h"

#include <iostream>
#include <string>
#include <memory>
#include <math.h>

#include "TLorentzVector.h"

namespace Upgrade {
class Jet : public TLorentzVector {
public:
	Jet() { }
	Jet(const TLorentzVector &tlv): TLorentzVector(tlv) { }
	bool truthIsHS    = true;  // true for HS, false for PU
	int  truthFlavour = -99;   // truth label: HadronConeExclTruthLabelID
	bool isBtagged    = false; // whether it's b-tagged

	// Prints information about this jet to the screen
	void print(TString msg = "") {
		printf(" Jet (pT,eta,phi,m) = (%6.1f GeV,%5.2f,%5.2f,%5.1f GeV), Truth %s jet, flav=%3i %sb-tagged. %s\n",
		       Pt() / 1000, Eta(), Phi(), M() / 1000, truthIsHS ? "HS" : "PU", truthFlavour, isBtagged ? "" : "not ", msg.Data());
	}
};

typedef std::vector<Jet> Jets;

/*
namespace PileupEff {
	static const unsigned int HS = 1001;
	static const unsigned int PU = 1002;
}
*/

class IUpgradePerformanceFunctions : public virtual asg::IAsgTool {
public:
	ASG_TOOL_INTERFACE( IUpgradePerformanceFunctions ) //declares the interface to athena

	enum UpgradeLayout {LoI, LoIVF, gold, silver, bronze, InclBrl, ExtBrl, Step1p6, run2};
	enum ElectronCutLevel {looseElectron, mediumElectron, tightElectron};
	enum PhotonCutLevel {loosePhoton, tightPhoton};
	enum MuonCutLevel {looseMuon, tightMuon, highPtMuon};
	enum METSyst {nominal, process, thresholdUp, thresholdDown, resoUp, resoDown};
	enum FatJetType {QCD, Top, WZ, NUM_FAT_JET_TYPES};
	enum LargeRjetTopology { Multijet, Wprime, Zprime };
	enum JetAlgorithm { TopoEM, PFlow };
	enum PileupEff { HS, PU };

#ifdef XAOD_STANDALONE
	virtual UpgradeLayout getLayout() = 0;
#else
	virtual int getLayout() = 0;
#endif

	virtual double getAvgMu() = 0;

	// Electron performance functions
	virtual float getElectronEfficiency(float ptMeV, float eta) = 0;
	virtual float getElectronFakeRate(float ptMeV, float eta) = 0;
	virtual float getElectronEnergyResolution(float eMeV, float eta) = 0;
	virtual float getElectronSmearedEnergy(float eMeV, float eta) = 0;
	virtual float getElectronFakeRescaledEnergy(float eMeV, float eta) = 0;
	virtual float getElectronChargeFlipProb(float eMeV, float eta) = 0;
	virtual float interpolateElectronIDMap(float *ptEtaMap, float pT, float eta) = 0;
	virtual float interpolateElectronResolutionMap(float *ptEtaMap, float pT, float eta) = 0;

	// Photon performance functions
	virtual StatusCode initPhotonFakeHistograms() = 0;
	virtual float getPhotonEfficiency(float ptMeV) = 0;
	virtual float getPhotonFakeRate(float ptMeV) = 0; // jet faking photon
	virtual float getPhotonPileupFakeRate(float ptMeV) = 0;// pileup jet faking photon
	virtual float getElectronToPhotonFakeRate(float ptMeV, float eta) = 0;
	virtual float getPhotonEnergyResolution(float eMeV, float eta) = 0;
	virtual float getPhotonSmearedEnergy(float eMeV, float eta) = 0;
	virtual TLorentzVector getPhotonSmearedVector(const TLorentzVector* init) = 0;
	virtual float getPhotonFakeRescaledEnergy(float eMeV) = 0;
	virtual float getPhotonFakeRescaledET(float eMeV) = 0;
	virtual float getPhotonPileupFakeRescaledET(float eMeV) = 0;

	// Muon performance functions
	virtual float getMuonEfficiencyFallback(float ptMeV, float eta) = 0;
	virtual float getMuonEfficiency(float ptMeV, float eta, float phi = 0) = 0;
	// These interface functions support phi for future updates.
	// However, currently, the resolutions are INDEPENDENT of phi, hence a default can be assigned
	virtual float getMuonPtResolution(float ptMeV, float eta, float phi = 0) = 0;
	virtual float getMuonQOverPtResolution(float ptMeV, float eta, float phi = 0) = 0;
	virtual float getMuonMSPtResolution(float ptMeV, float eta, float phi = 0) = 0;
	virtual float getMuonMSQOverPtResolution(float ptMeV, float eta, float phi = 0) = 0;
	virtual float getMuonIDPtResolution(float ptMeV, float eta, float phi = 0) = 0;
	virtual float getMuonIDQOverPtResolution(float ptMeV, float eta, float phi = 0) = 0;

	// Tracking performance functions
	virtual float getTrackPtResolution(float ptMeV, float eta) = 0;
	virtual float getTrackInvPtResolution(float invptMeV, float eta) = 0;
	virtual float getTrackPtRelativeResolution(float ptMeV, float eta) = 0;

	// Tau performance functions
	virtual float getTauEfficiency(float etMeV, float eta, short prong, short wp) = 0;
	virtual float getTauFakeRate(float etMeV, float eta, short prong, short wp) = 0;
	virtual float getTauEnergyResolution(float eMeV, float eta, short prong) = 0;
	virtual float getTauSmearedEnergy(float eMeV, float eta, short prong) = 0;

	// dump jet smearing settings to screen
	virtual void printJetSmearingSettings() = 0;

	// Set pT and rapidity jet acceptance criteria
	// Used by getSmearedJets and getSmearedBtaggedJets
	virtual void setJetPtRapidiyCuts(double ptMeV = 30e3, double absY = 3.8) = 0;

	// Master method for anti-kT R=0.4 that takes list of hard-scatter truth jets and
	// a) smear the jets (account for detector resolution)
	// b) overlay pileup jets
	// c) apply pT and rapidity selection according to setPtRapidiyCut
	// d) apply track-confirmation (JVT)
	// e) sort selected jets by pT
	// For usage, see: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/UpgradePerformanceFunctions
	virtual Upgrade::Jets getSmearedJets(Upgrade::Jets hsJets) = 0;

	// Same as the above method, but also apply b-tagging to the jets.
	// Note that the input HS truth jets need to be labelled with their truth flavour
	// Upgrade::Jets getSmearedBtaggedJets(Upgrade::Jets hsJets, TString btagAlgo="mv2c10", int btagOP=70) = 0;

	// Jet algorithm and constant term penalty affect small-R jet smearing
	virtual void setJetAlgorithm(JetAlgorithm jetAlgo) = 0;
	virtual void setConstantTermPenaltyFactor(double DeltaC) = 0;

	// Method to get JER for small-R jets (anti-kt R=0.4)
	virtual double getJER(double ptMeV, double eta, double mu, JetAlgorithm jetAlgo) = 0;
	// Same as above, but uses m_avgMu and m_jetAlgo
	virtual double getJER(double ptMeV, double eta) = 0;

	// Returns smeared anti-kt R=0.4 jet given truth 4-vector as input
	virtual TLorentzVector getSmearedJet(TLorentzVector tjet_MeV) = 0;
	virtual TLorentzVector getSmearedJet(double ptMeV, double eta, double phi, double massMeV) = 0;

	// Returns smeared large-R jet given truth 4-vector as input and topology, that must be QCD, Wprime or Zprime
	// This was derived using trimmed Anti-kT R=1.0 jets
	virtual TLorentzVector getSmearedLargeRjet(double ptMeV, double eta, double phi, double massMeV, LargeRjetTopology jetTopo = Multijet) = 0;
	virtual TLorentzVector getSmearedLargeRjet(TLorentzVector truthLargeRjet_MeV, LargeRjetTopology jetTopo = Multijet) = 0;

	// Returns vector of {JER,JMS,JMR} - jet energy resolution, mass scale (median) and mass resolution (68% quantile interval)
	virtual std::vector<double> getLargeRjet_JER_JMS_JMR(double ptMeV, double massMeV, LargeRjetTopology jetTopo = Multijet) = 0;

	// Returns each of the component of largeR jet smearing
	virtual double getLargeRjetJER(double ptMeV, double massMeV, LargeRjetTopology jetTopo = Multijet) = 0;
	virtual double getLargeRjetJMS(double ptMeV, double massMeV, LargeRjetTopology jetTopo = Multijet) = 0;
	virtual double getLargeRjetJMR(double ptMeV, double massMeV, LargeRjetTopology jetTopo = Multijet) = 0;

	/*
	 *  These four methods are obsolete!
	 */
	virtual float getJetSmearedEnergy(float ptMeV, float eta, bool useJetArea) = 0;
	virtual float getJetEnergyResolution(float ptMeV, float eta, bool useJetArea) = 0;
	virtual float getJetPtThresholdForFakeRate(float eta, float fakeRate, bool useTrackConfirmation) = 0;
	virtual float getJetSmearedMass(float ptMeV, float massMeV, FatJetType jetType) = 0;

	// Missing ET performance functions
	virtual StatusCode loadMETHistograms() = 0;
	typedef std::pair<double, double> MET;
	virtual MET getMETSmeared(float truthSumEtMeV, float METxMeV, float METyMeV, METSyst systValue = nominal) = 0;
	virtual float getMETResolution(float sumEtMeV, METSyst systValue = nominal) = 0;
	virtual float getSumETwithPileup(float truthSumEtMeV, METSyst systValue = nominal) = 0;
	virtual TVector3 getTSTsmearing(TVector3 pthard) = 0;

	// Flavour-tagging performance functions
	virtual float getFlavourTagEfficiency(float ptMeV, float eta, char flavour, TString tagger = "mv1", int operating_point = 70, bool track_confirmation = false) = 0;

	// Trigger performance functions
	virtual float getSingleElectronTriggerEfficiency(float ptMeV, float eta) = 0;
	virtual float getDiElectronTriggerEfficiency(float pt1MeV, float pt2MeV, float eta1, float eta2) = 0;
	virtual float getSinglePhotonTriggerEfficiency(float etMeV, float eta) = 0;
	virtual float getDiPhotonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2) = 0;
	virtual float getSingleMuonTriggerEfficiency(float etMeV, float eta) = 0;
	virtual float getDiMuonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2) = 0;
	virtual float getElectronMuonTriggerEfficiency(float elecEtMeV, float muonEtMeV, float elecEta, float muonEta) = 0;
	virtual float getSingleTauTriggerEfficiency(float etMeV, float eta, short prong) = 0;
	virtual float getDiTauTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2, short prong1, short prong2) = 0;
	virtual float getSingleJetTriggerEfficiency(float smearedEtMeV, float eta) = 0;
	virtual float getThreeJetTriggerEfficiency(float smearedEtMeV1, float eta1, float smearedEtMeV2, float eta2, float smearedEtMeV3, float eta3) = 0;
	virtual float getFourJetTriggerEfficiency(float smearedEtMeV1, float eta1, float smearedEtMeV2, float eta2, float smearedEtMeV3, float eta3, float smearedEtMeV4, float eta4) = 0;
	virtual float getHTTriggerEfficiency(float smearedHTMeV) = 0;
	virtual float getEtmissTriggerEfficiency(float smearedEtmissMeV) = 0;
	virtual float getJetEtmissTriggerEfficiency(float smearedJetEtMeV, float jetEta, float smearedEtmissMeV) = 0;

	// Pile-up overlay functions
	virtual std::string setPileupTemplatesPath() = 0; // user provides folder with PULibraries
	virtual bool getPileupTrackConfSetting() = 0;
	virtual float getPileupJetPtThresholdMeV() = 0;
	virtual std::vector<TLorentzVector> getPileupJets() = 0;

	// Pileup jet tagging efficiency
	// 1. big all-in-one function
	virtual float getTrackJetConfirmEff(float ptMeV, float eta, TString jetType) = 0;

	// 2. Sub methods, that does the same job
	virtual double getJVTeff(const Upgrade::Jet jet_MeV) = 0;
	virtual double getJVTeff_HSjet(double ptMeV, double eta) = 0;
	virtual double getJVTeff_PUjet(double ptMeV, double eta) = 0;

	// HGTD setters
	virtual float getHGTDMinEta() = 0; // returns minimum eta for the HGTD depending on the scenario (0/1)
	virtual float getHGTDMaxEta() = 0; // returns maximum eta for the HGTD, currently 4.3
	virtual void extendJetTrigger(bool exttrig = false) = 0; // nominal = 3.2, exttrig = true => 3.8

	virtual float muonEtaTriggerEfficiency(float eta) = 0;
	virtual StatusCode initializePileupTemplates() = 0; // needs to be called by "setAvgMu" - PU Libraries only for mu 140 and 200 so far
	virtual bool setupMuonResProvider() = 0;
	virtual bool setupMuonEffProvider() = 0;

};

} // namespace Upgrade

#endif //> !UPGRADEPERFORMANCEFUNCTIONS_IUPGRADEPERFORMANCEFUNCTIONS_H
