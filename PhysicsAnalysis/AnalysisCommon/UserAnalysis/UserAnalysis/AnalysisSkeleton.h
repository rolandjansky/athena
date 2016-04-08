/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSIS_SKELETON_H
#define ANALYSIS_SKELETON_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : AnalysisSkeleton.h
/// Package : offline/PhysicsAnalysis/AnalysisCommon/UserAnalysis
/// Author  : Ketevi A. Assamagan
/// Created : July 2004
///
/// DESCRIPTION:
///
/// This class is an analysis skeleton - The user can implement his analysis here
/// This class is also used for the demonstration of the distributed analysis
/// Ketevi A. Assamagan on June 9, 2004
///
/// Vivek Jain - March 11, 2010
/// This file has been extensively modified since creation. It now shows you how to access
/// trigger information. Preparation of objects, e.g., electrons, muons, jets, etc., 
/// has been pushed off into the UserAnalysisUtils package
///
/// Vivek Jain - July 13, 2011
/// Modified to use AthAlgorithm insead of CBNT_AthenaAwareBase.

/// The code here also does overlap removal
///
/// Please see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PhysicsAnalysisWorkBookAODAnalysisRel17#Writing_your_own_analysis_code for details relating to release 17
/// 
///
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "AnalysisTools/AnalysisTools.h"

#include "UserAnalysisUtils/UserAnalysisSelectionTool.h"
#include "UserAnalysisUtils/UserAnalysisPreparationTool.h"
#include "UserAnalysisUtils/UserAnalysisOverlapCheckingTool.h"
#include "UserAnalysisUtils/UserAnalysisOverlapRemovalTool.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

//#include "TLorentzVector.h"
//#include "CLHEP/Vector/LorentzVector.h"


#include <string>

#include "TH1.h"

class JetCollection;

using namespace Trig;
namespace Trig {
  class ChainGroup;
}

// upgrade to inherit from AthAlgorithm
 
class AnalysisSkeleton : public AthAlgorithm  {

 public:

   AnalysisSkeleton(const std::string& name, ISvcLocator* pSvcLocator);
   ~AnalysisSkeleton();

   virtual StatusCode beginRun();
   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode execute();
   virtual StatusCode initEvent();

 private:


   /** methods called by execute() */

   // to add event info to new ntuple (used to go by default in CollectionTree)
   StatusCode addEventInfo(); 
   //
   StatusCode electronSkeleton();
   StatusCode triggerSkeleton();

   /** an example of pre-selection, overlap-checking and overlap removal */
   StatusCode analysisPreparation();

   /** look at b-jet tagging information */
   StatusCode bjetInfo();

   /** get quark flavour of jets */
   int getQuarkJetFlavour(JetCollection::const_iterator jetItr);

   /** get missing ET information */
   StatusCode getMissingET();

   /** make plots for SUSY studies */
   StatusCode SusyStudies();

   /** get pT of top quarks */
   StatusCode getTopQpT(int &, double&, double&);

 private:

   /** get a handle to the tool helper */
   ToolHandle<AnalysisTools> m_analysisTools;

   /** get a handle on the user tool for pre-selection and overlap removal */
   ToolHandle<UserAnalysisSelectionTool>       m_analysisSelectionTool;
   ToolHandle<UserAnalysisPreparationTool>     m_analysisPreparationTool;
   ToolHandle<UserAnalysisOverlapCheckingTool> m_analysisOverlapCheckingTool;
   ToolHandle<UserAnalysisOverlapRemovalTool>  m_analysisOverlapRemovalTool;

   /** tool to access the trigger decision */
   ToolHandle<Trig::TrigDecisionTool> m_trigDec;

   /** a handle on the Hist/TTree registration service */
   ITHistSvc * m_thistSvc;

   /** a handle on Store Gate for access to the Event Store */
   //StoreGateSvc* m_storeGate;

   /** the key of the Electron Container to retrieve from the AOD */
   std::string m_electronContainerName; 

   /** name of the AOD truth particle container to retrieve from StoreGate */
   std::string m_truthParticleContainerName;

   /** key to get missing ET information */
   std::string m_missingETObjectName;


   /// The missing ET object
   const MissingET * m_pMissing;
   double m_pxMiss;
   double m_pyMiss;
   double m_ptMiss;   

   /** additional user cuts after pre-selections */ 
 
   double m_deltaRMatchCut;
   double m_maxDeltaR;

   /** electron specific cuts */
   double m_etElecCut;
   double m_elecCone;
   double m_etaElecCut;

   /** bjet specific cuts */
   double m_bjetWt_ip3dsv1Cut;
   double m_bjet_etaCut;
   double m_bjet_etCut;

   /** missing ET cuts */
   double m_missingETCut;

   /** Atlfast data? */
   bool m_isAtlFastData;

   /** truth input? */
   bool m_doTruth;

   /** min Jet ET cut for SUSY studies */
   double m_SusyJetMinEt;
   
  /** Histograms */
  TH1F* m_h_elecpt;
  TH1F* m_h_eleceta;
  TH1F* m_h_elec_deltaRMatch;

  TH1F* m_h_jet_eta_beforeOR;
  TH1F* m_h_jet_et_beforeOR;
  TH1F* m_h_jet_ip3dsv1Wt_beforeOR;
  TH1F* m_h_jet_label_beforeOR;
  TH1F* m_h_jet_ip3dsv1Wt_bjet_beforeOR;
  TH1F* m_h_jet_ip3dsv1Wt_ujet_beforeOR;

  TH1F* m_h_jet_eta_afterOR;
  TH1F* m_h_jet_et_afterOR;
  TH1F* m_h_jet_ip3dsv1Wt_afterOR;
  TH1F* m_h_jet_label_afterOR;
  TH1F* m_h_jet_ip3dsv1Wt_bjet_afterOR;
  TH1F* m_h_jet_ip3dsv1Wt_ujet_afterOR;

  TH1F* m_pxMis;
  TH1F* m_pyMis;
  TH1F* m_ptMis;

  TH1F* m_triggerAccepts;

  /** Athena-Aware Ntuple (AAN) variables - branches of the AAN TTree */

  // stuff for new ntuple
  // The standard AANT, CollectionTree, is bare bones
  TTree* m_tree_AS; 

  /** Simple variables by Ketevi */
  int m_aan_size;
  std::vector<double> * m_aan_eta;
  std::vector<double> * m_aan_pt;
  std::vector<double> * m_aan_elecetres;

  /** Variables by VJ */
  double m_aan_ptMiss;
  int    m_aan_njets;
  int    m_aan_njets_etaLT25;
  int    m_aan_njets_SusyETCut;
  double m_aan_effmass;
  double m_aan_ht;
  double m_aan_maxJetET;
  int    m_aan_nbjets;

  std::vector<double>* m_aan_JetEta;
  std::vector<double>* m_aan_JetEt;
  std::vector<double>* m_aan_JetBTagWt;

  /** Look at final electrons/muons */
  int m_aan_NFinalEl;
  int m_aan_NFinalMu;

  std::vector<double>* m_aan_FinalElEta;
  std::vector<double>* m_aan_FinalElPt;
  std::vector<double>* m_aan_FinalElEtCone20;
  //std::vector<double>* m_aan_FinalElPtrat;

  std::vector<double>* m_aan_FinalMuEta;
  std::vector<double>* m_aan_FinalMuPt;
  std::vector<double>* m_aan_FinalMuEtCone20;
  std::vector<int>*    m_aan_FinalMuBestMat;
  std::vector<double>* m_aan_FinalMuMatChi2;

  double m_aan_FinalLepEtSum;
  double m_aan_FinalElEtSum;
  double m_aan_FinalMuEtSum;

  /** number top quarks */

  int m_aan_NumTopQ;
  double m_aan_pTtop1;
  double m_aan_pTtop2;

  /** trigger branches */
  float m_aan_Trig_efJet_et  ;
  float m_aan_Trig_efJet_eta ;
  float m_aan_Trig_efJet_phi ;
  float m_aan_Trig_l2Jet_et  ;
  float m_aan_Trig_l2Jet_eta ;
  float m_aan_Trig_l2Jet_phi ;
  float m_aan_Trig_l1Jet_et88;
  float m_aan_Trig_l1Jet_eta ;
  float m_aan_Trig_l1Jet_phi ;

  bool m_doTrigger;
  std::string m_investigateChain;
  std::vector<std::string> m_triggerChains;
  std::map<std::string,int> m_triggersPassed;

  unsigned int m_eventNr;
  const ChainGroup* m_all;
  const ChainGroup* m_allL1;
  const ChainGroup* m_allL2;
  const ChainGroup* m_allEF;
  std::vector<std::string> m_chain_names;

  unsigned int    m_runNumber;
  unsigned int    m_eventNumber;
  unsigned int    m_eventTime;
  unsigned int    m_lumiBlock;
  unsigned int    m_bCID;
  double  m_eventWeight;
  //unsigned int    m_statusElement;
  /*
  unsigned int    m_lVL1ID;
  unsigned int    m_lvl1TriggerType;
  std::vector<unsigned int>* m_lvl1TriggerInfo;
  std::vector<unsigned int>* m_lvl2TriggerInfo;
  std::vector<unsigned int>* m_evtFilterInfo;
  std::vector<std::string>* m_streamTagName;
  std::vector<std::string>* m_streamTagType;
  */

};

#endif // ANALYSIS_SKELETON_H

