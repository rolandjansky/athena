/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Author: N. Benekos(Illinois) - I. Nomidis (Thessaloniki)
// MuonDQA
// Mar. 2009
//
// DESCRIPTION:
// Subject: 	This class does a full Zmumu-Analysis based on AODs.
//		
// Modified:    Venkat Kaushik (Venkat.Kaushik@cern.ch)
//		        This class derives from ManagedMonitorToolBase and is used for 
// 		        Physics level monitoring using Z --> mu mu tag/probe method
// Apr. 2008
///////////////////////////////////////////////////////////////////////////
#ifndef ZMUMUPHYSMONTOOL_H
#define ZMUMUPHYSMONTOOL_H

// Gaudi Tools
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Storegate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

// Track Particles
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

// Truth Particles
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h" 

// Muon
#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"
// Electron
#include "egammaEvent/ElectronContainer.h"
// Jet
#include "JetEvent/JetCollection.h"
// Common implementation of all particles
#include "FourMomUtils/P4Helpers.h"

//Trigger
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include <vector>
#include <string>
#include "TLorentzVector.h"
#include <math.h>
#include "CLHEP/Units/PhysicalConstants.h"

using namespace Trig;

/////////////////////////////////////////////////////////////////////////////
class ZmumuPhysMonTool: public ManagedMonitorToolBase//, public  MuonPhysicsAnalysisMonitoring
{
 public:
  ZmumuPhysMonTool (const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode bookHistogramsRecurrent();  
  virtual StatusCode procHistograms(); 
  virtual StatusCode fillHistograms();
  
  StatusCode initialize();
  StatusCode finalize();

 private: 

  bool isElectron(const Rec::TrackParticle *track);
  std::pair<double,double> getTrackPropertiesInCone(const Rec::TrackParticle *track);
  double getJetEnergyInCone(const Rec::TrackParticle *track);
  double getETIsolation(const Analysis::Muon *muon);

  /// a handle on Store Gate
  StoreGateSvc*		m_storeGate;

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  /** coverter tool McEventCollection -> TruthParticle */
  //  ITruthParticleCnvTool* m_cnvTool;

  std::string	        m_Outputfile;
  std::vector<std::string> m_muonCollectionName;
  std::string	        m_InnerTrackContainer;
  std::string	        m_MuonSpectrometerTrackContainer;
  std::string           m_ElectronContainer;
  std::string           m_JetContainer;
  std::string           m_TruthParticleContainer;
  std::string           m_TrackParticleTruthCollection;

  int nEvents;

  bool   m_doTruth;
  bool   m_doTrigger;
  //cuts
  double m_cut_muonHigherPt;
  double m_cut_muonLowerPt;
  double m_cut_deltaPhi;
  double m_cut_muonPt;
  double m_cut_muonEta;
  double m_cut_diMuonMass;
  double m_cut_isolationJet; 
  double m_cut_isolationNtracks;
  double m_cut_isolationPtSum;
  double m_cut_isolationET; 
 
  //isolation
  double m_isoConeMin;
  double m_isoConeMax;

  //Declaration of hists
  std::vector<TH1F*> m_True_NumberOfMuons;
  std::vector<TH1F*> m_True_ZMass;
  std::vector<TH1F*> m_True_DiMuonMass;
  std::vector<TH1F*> m_True_ZPt;
  std::vector<TH1F*> m_True_MuonPt;
  
  std::vector<TH1F*> m_Reco_NumberOfTracks;
  std::vector<TH1F*> m_Reco_DiMuonPt;
  std::vector<TH1F*> m_Reco_PtMin;
  std::vector<TH1F*> m_Reco_PtMax;
  std::vector<TH1F*> m_Reco_DeltaPhi;
  std::vector<TH1F*> m_Reco_Charge;
  std::vector<TH1F*> m_Selected_DiMuonMass;
  std::vector<TH1F*> m_Reco_MuonPt;
  std::vector<TH1F*> m_Reco_MuonEta;

  std::vector<TH1F*> m_Reco_DiMuonMass;
  std::vector<TH1F*> m_Reco_DiMuonMass_ID;
  std::vector<TH1F*> m_Reco_ZMass;
  
  std::vector<TH1F*> m_Cut_Efficiency;
  std::vector<TH1F*> m_Cut_Flow;
  std::vector<TH1F*> m_nCandidates_perEvent;

  std::vector<TH2F*> m_Reco_DeltaPhi_vs_Pt;  
  
  std::vector<TH1F*> m_Reco_JetEnergy_InHalo;
  std::vector<TH1F*> m_Reco_NumberOfTracks_InHalo;
  std::vector<TH1F*> m_Reco_TrackPt_InHalo;
  std::vector<TH1F*> m_Reco_ET_InHalo;

  std::vector<TProfile*> m_Reco_Mass_vs_Pt;
  std::vector<TProfile*> m_Reco_Mass_vs_Eta;
  std::vector<TProfile*> m_Reco_Mass_vs_Phi;

  std::vector<TH1F*> m_Reco_MassShift_vs_Pt;
  std::vector<TH1F*> m_Reco_MassShift_vs_Eta;
  std::vector<TH1F*> m_Reco_MassShift_vs_Phi;

  TH1F* m_Trig_Cut_Flow;
  TH1F* m_numZ_per_TriggerItem;
  TH1F* m_numDimuons_per_TriggerItem;

  std::vector<TH1F*> m_Z_TrigEfficiency_Eta;
  std::vector<TH1F*> m_Z_TrigEfficiency_Phi;
  std::vector<TH1F*> m_Z_TrigEfficiency_Pt;

  //temporary hists for Z trigger efficiency
  TH1F* m_h_pttotal;
  std::vector<TH1F*> m_h_pttrig;
  TH1F* m_h_pttruth;

  TH1F* m_h_phitotal;
  std::vector<TH1F*> m_h_phitrig;
  TH1F* m_h_phitruth;

  TH1F* m_h_etatotal;
  std::vector<TH1F*> m_h_etatrig;
  TH1F* m_h_etatruth;
  
  std::vector<double>  m_binsPt;   //variable binning for Pt-efficiency plot
  std::vector<double>  m_binsEta;  //variable binning for eta-efficiency plot
  std::vector<double>  m_binsPhi;  //variable binning for phi-efficiency plot

  //trigger items
  const ChainGroup* m_allL1;
  const ChainGroup* m_allL2;
  const ChainGroup* m_allEF;

  std::string           m_trigSelection;

};
#endif

