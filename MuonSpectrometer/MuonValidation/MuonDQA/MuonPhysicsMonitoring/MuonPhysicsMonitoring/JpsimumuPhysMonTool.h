/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   N. Benekos(Illinois) - I. Nomidis (Thessaloniki)
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
#ifndef JPSIMUMUPHYSMONTOOL_H
#define JPSIMUMUPHYSMONTOOL_H

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

#include "egammaEvent/ElectronContainer.h"

// Common implementation of all particles
#include "FourMomUtils/P4Helpers.h"

//Vertexing
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"


#include "AnalysisTools/IAnalysisTools.h"

//Trigger
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"
//Trigger Jpsi muon matching tool:
#include "TrigEffJpsiTools/ITrigEffJpsiTools.h"

#include "MuonPhysicsMonitoring/MuonPhysicsAnalysisMonitoring.h"

// ROOT Classes
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TLorentzVector.h"

// C++
#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#include "CLHEP/Units/PhysicalConstants.h"

using namespace Trig;

class IAnalysisTools;

/////////////////////////////////////////////////////////////////////////////
class JpsimumuPhysMonTool: public ManagedMonitorToolBase , public  MuonPhysicsAnalysisMonitoring
{
 public:
  JpsimumuPhysMonTool (const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode bookHistogramsRecurrent();  
  virtual StatusCode procHistograms(); 
  virtual StatusCode fillHistograms();
  
  StatusCode initialize();
  StatusCode finalize();

  bool isElectron(const Rec::TrackParticle *track);
  bool doVertexing(const Rec::TrackParticle *trk1, const Rec::TrackParticle *trk2, double &vtxChi2, double &mass, double &tau);


 private: 

  /// a handle on Store Gate
  StoreGateSvc*		m_storeGate;

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  IAnalysisTools * m_analysisTools;

  // Vertexing
  ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
  ToolHandle < InDet::ConversionFinderUtils > m_helpertool;
  ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
  ToolHandle < Trk::V0Tools > m_V0Tools;

  // Trigger
  ToolHandle<ITrigEffJpsiTools> m_trigEffJpsiTools;	 // trigger matching tool
		  
  /** coverter tool McEventCollection -> TruthParticle */
  //  ITruthParticleCnvTool* m_cnvTool;

  std::string	        m_Outputfile;
  std::vector<std::string> m_muonCollectionName;
  std::string	        m_InnerTrackContainer;
  //std::string	        m_MuonSpectrometerTrackContainer;
  std::string           m_trigSelection;
  std::string           m_TruthParticleContainer;
  std::string           m_TrackParticleTruthCollection;

  int nEvents;

  bool   m_doTruth;
  bool   m_doVertexing;
  bool   m_useVKalVrt;
  bool   m_doTrigger;

  double m_jpsiMass;
  double m_jpsiMassBinning;

  //cuts
  double m_cut_muonHigherPt;
  double m_cut_muonLowerPt;
  double m_cut_deltaR;
  double m_cut_muonPt;
  double m_cut_muonEta;
  double m_cut_diMuonMass;
  double m_cut_properTime;
  double m_cut_JpsiVertexChi2;
  double m_cut_lowerMass;
  double m_cut_upperMass;
  

  //Declaration of hists
  std::vector<TH1F*> m_True_NumberOfMuons;
  std::vector<TH1F*> m_True_JpsiMass;
  std::vector<TH1F*> m_True_DiMuonMass;
  std::vector<TH1F*> m_True_JpsiPt;
  std::vector<TH1F*> m_True_MuonPt;
  
  std::vector<TH1F*> m_Muon_NCandidates;
  std::vector<TH1F*> m_Muon_Pt;
  std::vector<TH1F*> m_Muon_Eta;
  std::vector<TH2F*> m_Muon_Pt_vs_Eta;
  std::vector<TH2F*> m_Muon_Type_vs_Eta;
  std::vector<TH2F*> m_Muon_Type_vs_Pt;

  std::vector<TH1F*> m_Dimuon_Mass;
  std::vector<TH1F*> m_Dimuon_MassAfterVtx;
  std::vector<TH1F*> m_Dimuon_VertexChi2;
  std::vector<TH1F*> m_Dimuon_DeltaR;
  std::vector<TH1F*> m_Dimuon_PtMin;
  std::vector<TH1F*> m_Dimuon_PtMax;
  std::vector<TH2F*> m_Dimuon_PtMax_vs_PtMin;
  std::vector<TH1F*> m_Dimuon_Charge;
  std::vector<TH1F*> m_Dimuon_NCandidates;
  std::vector<TH1F*> m_Dimuon_NCandidatesSameSign;

  std::vector<TH1F*> m_Jpsi_MassRefitted;
  std::vector<TH1F*> m_Jpsi_Mass;
  std::vector<TH1F*> m_Jpsi_MassSameSign;
  std::vector<TH1F*> m_Jpsi_Propertime;
  std::vector<TH1F*> m_Jpsi_VertexChi2;
  std::vector<TH1F*> m_Jpsi_Pt;
  std::vector<TH1F*> m_Jpsi_DeltaR;
  std::vector<TH2F*> m_Jpsi_DeltaR_vs_Pt;
  std::vector<TH2F*> m_Jpsi_PtMax_vs_PtMin;
  std::vector<TH1F*> m_Jpsi_NCandidates;
  
  std::vector<TProfile*> m_Jpsi_Mass_vs_Pt;
  std::vector<TProfile*> m_Jpsi_Mass_vs_Eta;
  std::vector<TProfile*> m_Jpsi_Mass_vs_Phi;

  std::vector<TH1F*> m_Jpsi_MassShift_vs_Pt;
  std::vector<TH1F*> m_Jpsi_MassShift_vs_Eta;
  std::vector<TH1F*> m_Jpsi_MassShift_vs_Phi;

  std::vector<TH1F*> m_Jpsi_Mass_CC;
  std::vector<TH1F*> m_Jpsi_Mass_CT;
  std::vector<TH1F*> m_Jpsi_Mass_TT;

  std::vector<TH1F*> m_Jpsi_Mass_BB;
  std::vector<TH1F*> m_Jpsi_Mass_BEA;
  std::vector<TH1F*> m_Jpsi_Mass_BEC;
  std::vector<TH1F*> m_Jpsi_Mass_EEA;
  std::vector<TH1F*> m_Jpsi_Mass_EEC;

  std::vector<TH1F*> m_Jpsi_Mass_CC_BB;
  std::vector<TH1F*> m_Jpsi_Mass_CC_BE;
  std::vector<TH1F*> m_Jpsi_Mass_CC_EE;

  std::vector<TH1F*> m_Jpsi_Mass_CT_BB;
  std::vector<TH1F*> m_Jpsi_Mass_CT_BE;
  std::vector<TH1F*> m_Jpsi_Mass_CT_EE;

  std::vector<TH1F*> m_Jpsi_Mass_TT_BB;
  std::vector<TH1F*> m_Jpsi_Mass_TT_BE;
  std::vector<TH1F*> m_Jpsi_Mass_TT_EE;

  std::vector<TProfile*> m_Jpsi_Mass_per_Region;

  std::vector<TH1F*> m_Cut_Efficiency;
  std::vector<TH1F*> m_Cut_Flow;

  TH1F* m_Trig_Cut_Flow;
  TH1F* m_numJpsi_per_TriggerItem;
  TH1F* m_numDimuons_per_TriggerItem;

  std::vector<double>  m_binsPt;   //variable binning for Pt-efficiency plot
  std::vector<double>  m_binsEta;  //variable binning for eta-efficiency plot
  std::vector<double>  m_binsPhi;  //variable binning for phi-efficiency plot

  //trigger items
  const ChainGroup* m_allL1;
  const ChainGroup* m_allL2;
  const ChainGroup* m_allEF;

};
#endif
