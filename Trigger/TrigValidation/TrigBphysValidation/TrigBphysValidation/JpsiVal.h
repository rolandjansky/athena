/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JPSIVAL_H
#define JPSIVAL_H
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
/// Name    : Jpsi_Val.h
/// Package : offline/Trigger/TrigValidation/BphysValidation
/// Author  : J.Kirk
/// Created : March 2009
///
/// DESCRIPTION:
///
/// Perform trigger validation on JpsiphiPi sample (modified from AnalyssiSkeleton.h
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

//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"

#include <string>

#include "TH1.h"

class TrackParticleTruthCollection;

using namespace Trig;
namespace Trig {
  class ChainGroup;
}

// upgrade to inherit from AthAlgorithm
 
class JpsiVal : public AthAlgorithm  {

 public:

   JpsiVal(const std::string& name, ISvcLocator* pSvcLocator);
   ~JpsiVal();

   virtual StatusCode beginRun();
   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode execute();
   virtual StatusCode initEvent();

 private:


   /** methods called by execute() */

   // to add event info to new ntuple (used to go by default in CollectionTree)
   //   StatusCode addEventInfo(); 
   //
   //StatusCode electronSkeleton();
   //StatusCode triggerSkeleton();

   /** an example of pre-selection, overlap-checking and overlap removal */
   //StatusCode analysisPreparation();

   /** look at b-jet tagging information */
   //StatusCode bjetInfo();

   /** get quark flavour of jets */
   //int getQuarkJetFlavour(JetCollection::const_iterator jetItr);

   /** get missing ET information */
   //StatusCode getMissingET();

   /** make plots for SUSY studies */
   //StatusCode SusyStudies();

   /** get pT of top quarks */
   //StatusCode getTopQpT(int &, double&, double&);

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
   //std::string m_electronContainerName; 

   /** name of the AOD truth particle container to retrieve from StoreGate */
   std::string m_truthParticleContainerName;

   /** key to get missing ET information */
   //std::string m_missingETObjectName;

   /** key for MC event container */
   std::string m_MCEventContainerName;

   /** key for L2 Track container */
   std::string m_TrackContainerName;

   /// The missing ET object
   //const MissingET * m_pMissing;
   //double m_pxMiss;
   //double m_pyMiss;
   //double m_ptMiss;   

   /** additional user cuts after pre-selections */ 
 
   //double m_deltaRMatchCut;
   //double m_maxDeltaR;

   /** electron specific cuts */
   //double m_etElecCut;
   //double m_elecCone;
   //double m_etaElecCut;

   /** bjet specific cuts */
   //double m_bjetWt_ip3dsv1Cut;
   //double m_bjet_etaCut;
   //double m_bjet_etCut;

   /** missing ET cuts */
   //double m_missingETCut;

   /** Atlfast data? */
   //bool m_isAtlFastData;

   /** truth input? */
   //bool m_doTruth;

   /** min Jet ET cut for SUSY studies */
   //double m_SusyJetMinEt;
   
  /** Histograms */
  TH1F* m_h_jpsiphysn;
  TH1F* m_h_jpsiphysmass;
  TH1F* m_h_jpsiphystype;
  TH1F* m_hiNRoI_1;
  TH1F* m_hiNRoI_2;
  TH1F* m_hiNRoI_3;
  TH1F* m_hiNRoI_4;
  TH1F* m_hiNRoI_5;
  TH1F* m_hiNRoI_6;
  TH1F* m_hiNRoI_7;
  TH1F* m_hiNRoI_8;
  TH1F* m_hiNRoI_9;
  TH1F* m_hiNRoI_10;

  TH1F* m_hpt_muplus;
  TH1F* m_hpt_muminus;
  TH1F* m_hphi_muplus;
  TH1F* m_hphi_muminus;
  TH1F* m_heta_muplus;
  TH1F* m_heta_muminus;
  TH1F* m_hpt_muplus_inroi;
  TH1F* m_hpt_muminus_inroi;
  TH1F* m_hphi_muplus_inroi;
  TH1F* m_hphi_muminus_inroi;
  TH1F* m_heta_muplus_inroi;
  TH1F* m_heta_muminus_inroi;
  TH1F* m_hpt_muplus_tr;
  TH1F* m_hpt_muminus_tr;
  TH1F* m_hphi_muplus_tr;
  TH1F* m_hphi_muminus_tr;
  TH1F* m_heta_muplus_tr;
  TH1F* m_heta_muminus_tr;

  TH1F* m_hmass_Jpsi;
  TH1F* m_hmass_Jpsi_b;
  TH1F* m_hmass_Jpsi_m;
  TH1F* m_hmass_Jpsi_e;

  
  int m_Ntrack_Mup;
  int m_Ntrack_Mup_pT;
  int m_Ntrack_Mup_inRoI;
  int m_Ntrack_Mup_inRoI_pT;
  int m_Ntrack_Mum;
  int m_Ntrack_Mum_pT;
  int m_Ntrack_Mum_inRoI;
  int m_Ntrack_Mum_inRoI_pT;

  int m_trackAlgo;
  std::string m_triggerName;
  /** Athena-Aware Ntuple (AAN) variables - branches of the AAN TTree */

  TTree* m_tree_AS; 
  /** Simple variables */
  int m_aan_Jpsiphys;
  std::vector<double> * m_aan_JpsiMass;
  std::vector<double> * m_aan_JpsiFitMass;

  bool m_doTrigger;
  const ChainGroup* m_all;
  const ChainGroup* m_allL1;
  const ChainGroup* m_allL2;
  const ChainGroup* m_allEF;
  std::vector<std::string> m_chain_names;

  StatusCode getTrackParticleTruth( TrackParticleTruthCollection& p);


};

#endif // JPSIVAL_H

