/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTTauMonTool.h
 *   Class declaration for HLTTauMonTool
 *   
 *                          
 *    Artur Kalinowski (Artur.Kalinowski@cern.ch)       
 *   
 */

#ifndef HLTTAUMONTOOL_H
#define HLTTAUMONTOOL_H

#include <string>
#include <vector>
#include <map>
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaMonitoring/AthenaMonManager.h"
//#include "TrigDecision/TrigDecisionTool.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"
#include "tauEvent/TauJet.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrigTauEmulation/ILevel1EmulationTool.h"
#include "TrigTauEmulation/IHltEmulationTool.h"
#include "LumiBlockComps/ILuminosityTool.h"

// Forward declarations
class StatusCode;

class TrigTauCluster;
class TrigTauTracksInfo;
class TrigTau;
class EmTauRoI;
class TauJet;
class TrigRoiDescriptor;
class ILumiBlockMuTool;

class IInterface;
class StoreGateSvc;
class TH1I;
class TH1F;

class HLTTauMonTool : public IHLTMonTool {

 public:
  HLTTauMonTool(const std::string & type, const std::string & name, const IInterface* parent); 
  virtual ~HLTTauMonTool();
  
  StatusCode init();
//#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode book();
//#else
//  StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun);
//#endif
  StatusCode fill();


//#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode proc();
//#else
//  StatusCode proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun);
//#endif

  /// Method fills the histograms for one tau trigger item.
  StatusCode fillHistogramsForItem(const std::string & trigItem);
  
 private:

  /// Method books the histograms for one tau trigger item.
  void bookHistogramsForItem(const std::string & trigItem);
  void bookHistogramsAllItem();  
  /// Method filling the L1 RoI  histograms
  //StatusCode fillL1ForItem(const std::string & trigItem);
  StatusCode fillL1Tau(const xAOD::EmTauRoI * aL1Tau);
  StatusCode fillL1Jet(const xAOD::JetRoI * aL1Jet);
  /// Method filling the efficiency histograms
//  StatusCode fillEfficiency(const std::string & trigItem);

  /// Method filling the EF tau histograms
  StatusCode fillPreselTau(const xAOD::TauJet *aEFTau);
  StatusCode fillEFTau(const xAOD::TauJet *aEFTau, const std::string & trigItem, const std::string & BDTinput_type);
  /// Method filling the EF vs Offline tau histograms
  StatusCode fillEFTauVsOffline(const xAOD::TauJet *aEFTau, const std::string & trigItem, const std::string & BDTinput_type);
  StatusCode fillPreselTauVsOffline(const xAOD::TauJet *aEFTau);
  StatusCode fillL1TauVsOffline(const xAOD::EmTauRoI *aEFTau);
  StatusCode fillEFTauVsTruth(const xAOD::TauJet *aEFTau, const std::string & trigItem);  
  StatusCode fillTopoValidation(const std::string & trigItem_topo, const std::string & trigItem_support);

  /// Method finds  LVL1_ROI object corresponding to  given TrigRoiDescriptor
  const xAOD::EmTauRoI * findLVL1_ROI(const TrigRoiDescriptor * roiDesc);

  /// Method for Turn On Curves
  void examineTruthTau(const xAOD::TruthParticle& xTruthParticle) const;
  StatusCode TruthTauEfficiency(const std::string & trigItem, const std::string & TauCont_type);
  StatusCode RecoTauEfficiency(const std::string & trigItem);
  StatusCode TauEfficiency(const std::string & trigItem, const std::string & TauDenom);
//  StatusCode TauEfficiencyCombo(const std::string & trigItem);

  StatusCode RealZTauTauEfficiency();
  StatusCode dijetFakeTausEfficiency();
  StatusCode FTKtest(const std::string & trigItem);
  StatusCode trackCurves(const std::string & trigItem);
  StatusCode efficiencyRatioPlots (const std::string & trigItem);
  StatusCode L1TopoLeptons(const std::string & trigItem, const std::string & typeOfChain);

  //Methods for HLT and L1 Matching
  bool HLTTauMatching(const std::string & trigItem, const TLorentzVector & TLV, float DR);
  bool PresTauMatching(const std::string & trigItem, const TLorentzVector & TLV, float DR);
  bool L1TauMatching(const std::string & trigItem, const TLorentzVector & TLV, float DR);
  //  bool  emulate2StepTracking(float RoI_eta, float RoI_phi, bool do2step, bool doReject0trk, float track_pt_min);
  //StatusCode test2StepTracking();
  void testClusterNavigation(const xAOD::TauJet *aEFTau);
  void testL1TopoNavigation(const std::string & trigItem);
  float PrescaleRetrieval(const std::string & trigItem, const std::string & level);
  bool Selection(const xAOD::TauJet *aTau);
  bool Selection(const xAOD::EmTauRoI *aTau);
  int m_selection_nTrkMax, m_selection_nTrkMin;
  float m_selection_ptMax, m_selection_ptMin;
  float m_selection_absEtaMax, m_selection_absEtaMin;
  float m_selection_absPhiMax, m_selection_absPhiMin; 
  bool m_selection_BDT;

  bool TruthMatched( const xAOD::TauJet * tau);

  StatusCode Emulation();
  std::string LowerChain(std::string hlt_item);
  /// Method for managing the histogram divisions
  void divide(TH1 *num, TH1 *den, TH1 *quo);
  void divide(TH2 *num, TH2 *den, TH2 *quo);
  void divide(const std::string & h_name_num, const std::string & h_name_den, const std::string & h_name_div, const std::string & dir);
  void divide2(const std::string & h_name_num, const std::string & h_name_den, const std::string & h_name_div, const std::string & dir);
  float deltaEta(float eta1, float eta2);
  float deltaPhi(float phi1, float phi2);
  float deltaR(float eta1, float eta2, float phi1, float phi2);
  float deltaR(const xAOD::TauJet* aEFTau, const xAOD::EmTauRoI* l1Tau);
  void plotUnderOverFlow(TH1* hist);
  void cloneProfile(const std::string name, const std::string folder);
  void cloneHistogram(const std::string name, const std::string folder); 
  void cloneHistogram2(const std::string name, const std::string folder);
  /* StatusCode OfflineEfficiency(const std::string & trigItem); */
  /* StatusCode OfflineEfficiencyBDTMedium(const std::string & trigItem); */
  bool Match_Offline_L1(const xAOD::TauJet *aOfflineTau, const std::string & trigItem);
  bool Match_Offline_EF(const xAOD::TauJet *aOfflineTau, const std::string & trigItem);
  int PrimaryVertices();
  float Pileup();
  void FillRelDiffHist(TH1 * hist, float val1, float val2, float checkVal, int checkMode);
  template<class T> void FillRelDiffProfile(TProfile * prof, float val1, float val2, T val3, float checkVal, int checkMode);

  bool m_turnOnCurves;
  std::string m_turnOnCurvesDenom;
  bool m_truth;
  //bool m_doTestTracking;
  bool m_emulation;
  bool m_RealZtautauEff;
  bool m_dijetFakeTausEff;
  bool m_doTrackCurves;
  bool m_doTopoValidation;
  bool m_doL1JetPlots;
  bool m_doEFTProfiles;
  bool m_domuCut40; 
  bool m_doEfficiencyRatioPlots;
  bool m_doL1TopoLeptonsMonitoringWarnings;
  bool m_bootstrap;
  bool m_isData;

  std::vector<TLorentzVector> m_true_taus;
  std::vector<int> m_true_taus_nprong;
  std::vector<const xAOD::TauJet *> m_taus;
  const xAOD::TauJetContainer* m_tauCont;

  unsigned int m_L1TriggerCondition;
  unsigned int m_HLTTriggerCondition;
  std::string m_L1StringCondition; 
  std::string m_HLTStringCondition;

  ToolHandle<TrigTauEmul::ILevel1EmulationTool> m_l1emulationTool;
  ToolHandle<TrigTauEmul::IHltEmulationTool> m_hltemulationTool;

  ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;
  ToolHandle<ILuminosityTool>  m_luminosityToolOnline;
  float m_mu_offline;
  int m_mu_online;

  ///Name of the trigger items to be monitored.
  ///Set by job options
  std::vector<std::string> m_trigItems;
  std::vector<std::string> m_trigItemsAll;
  std::vector<std::string> m_trigItemsHighPt;
  std::vector<std::string> m_trigItemsZtt;
  std::vector<std::string> m_highpt_tau;
  std::vector<std::string> m_ztt_tau;
  std::vector<std::string> m_primary_tau;
  std::vector<std::string> m_monitoring_tau;
  std::vector<std::string> m_prescaled_tau;
  std::vector<std::string> m_emulation_l1_tau;
  std::vector<std::string> m_emulation_hlt_tau;
  std::vector<std::string> m_topo_chains_ditau;
  std::vector<std::string> m_topo_chains_mutau;
  std::vector<std::string> m_topo_chains_eltau;
  std::vector<std::string> m_topo_support_chains;
  std::vector<std::string> m_LST_HLTsel_FTK_chains;
  std::vector<std::string> m_LST_HLTsel0Prong_FTK_chains;
  std::vector<std::string> m_LST_HLTsel_FTKNoPrec_chains;
  std::vector<std::string> m_LST_HLTsel0Prong_FTKNoPrec_chains;
  std::vector<std::string> m_LST_HLTsel_tracktwo_chains;
  std::vector<std::string> m_Ratio;
  std::string m_lowest_singletau;
  //std::string m_lowest_ditau;
  //std::string m_lowest_etau;
  //std::string m_lowest_mutau;
  //std::string m_lowest_mettau;
  //std::string m_cosmic_chain;
  // offline tau pt threshold for efficiency plots as a function of eta, phi, and nvtx
  float m_effOffTauPtCut;

  int counterOfdR0_Topomutau;
  int counterOfdR0_Topoeltau;

  unsigned int activeFTKvsNonFTKEffRatioChains;
  unsigned int activeFTKvsNonFTKEffRatio0prongChains;
  unsigned int activeFTKvsNonFTKEffRatioNoPrecChains;
  unsigned int activeFTKvsNonFTKEffRatio0prongNoPrecChains;
  std::string lstE_FTK;	// energy cut of lowest single tau in FTK
//  std::string lstE_tracktwo;	// energy cut of lowest single tau in std chains
  unsigned chainFTKName_tau;		// *.
  unsigned chainFTKName_sep;		// stings to determine the LST energy in the FTK menu
  std::string chainFTKName_tauE;	// .*
  std::string lstChainFTK_idperf;
  std::string lstChainFTK_perf;
  std::string lstChainFTK_medium1;
  bool FTKLST_idperfActive;
  bool FTKLST_perfActive;
  bool FTKLST_medium1Active;
  bool FTKLST_perf0Active;
  bool FTKLST_medium0Active;
  bool FTKLST_medium1NoPrecActive;
  bool FTKLST_medium0NoPrecActive;

  // FTKEffTProf flags
  bool doFTKEffTProf;
  bool doFTKEffTProf_2;
  bool do0prongFTKEffTProf;
  bool do0prongFTKEffTProf_2;
  bool doFTKNoPrecEffTProf_2;
  bool do0prongFTKNoPrecEffTProf_2;

  std::vector<int> LST_FTK_PassHLTsel;
  std::vector<int> LST_FTK0Prong_PassHLTsel;
  std::vector<int> LST_FTKBDT_PassHLTsel;
  std::vector<int> LST_FTK0ProngBDT_PassHLTsel;
  std::vector<int> LST_tracktwo_PassHLTsel;


  // EffRatioPlots flags
  //bool doEffRatioPlots = false;
  bool effRatioChains_Active;
  //bool do0prongEffRatioPlots = false;
  bool effRatio0ProngChains_Active;
  //bool doBDTEffRatioPlots = false;
  bool effRatioBDTChains_Active;
  //bool do0prongBDTEffRatioPlots = false;
  bool effRatio0ProngBDTChains_Active;

  bool muCut40Passed;

};

#endif
