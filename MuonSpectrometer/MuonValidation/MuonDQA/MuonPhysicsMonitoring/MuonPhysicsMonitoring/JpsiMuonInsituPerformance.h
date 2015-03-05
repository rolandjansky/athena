/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   N. Benekos(Illinois) - I. Nomidis (Thessaloniki)
// MuonDQA
// Mar. 2009
//
// DESCRIPTION:
// Subject: 	This class does a full Insitu-Performance of the MuonSpectrometer
//		reconstruction efficiency, Inner-Tracker-Reconstruction Efficiency and
//		Trigger Efficiency
//		It is derivated from MuonPhysicsAnalysisMonitoring
///////////////////////////////////////////////////////////////////////////


#ifndef JpsiMuonInsituPerformance_H
#define JpsiMuonInsituPerformance_H

// Gaudi Tools
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// Storegate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

// AthenaMonitoring
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

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
#include "MuonPhysicsMonitoring/MuonPhysicsAnalysisMonitoring.h"

//Trigger
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"
//Trigger Jpsi muon matching tool:
#include "TrigEffJpsiTools/ITrigEffJpsiTools.h"

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

class JpsiMuonInsituPerformance: public ManagedMonitorToolBase , public  MuonPhysicsAnalysisMonitoring
{
 public:

  JpsiMuonInsituPerformance( const std::string & type, const std::string & name, const IInterface* parent ); 
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode procHistograms(); // execute
  virtual StatusCode fillHistograms();
  
  StatusCode initialize();
  StatusCode finalize();

  bool isAssociatedWithMSTrack(const Rec::TrackParticle *innerDetectorTrack,std::string MuonContainer) ;
  bool isElectron(const Rec::TrackParticle *track);
  bool isGoodTrack(const Rec::TrackParticle *trk) ;

  bool doVertexing(const Rec::TrackParticle *theTag, const Rec::TrackParticle *theProbe, Trk::RecVertex &primaryVertex, double &vtxChi2, double &mass, double &tau);
  void setDrawOptions(TH1F *h);

 private:

  StoreGateSvc          *m_storeGate;
  ITHistSvc             *m_thistSvc;
  IAnalysisTools        *m_analysisTools;

  // Vertexing
  ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
  ToolHandle < InDet::ConversionFinderUtils > m_helpertool;
  ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
  ToolHandle < Trk::V0Tools > m_V0Tools;

  // Trigger
  ToolHandle<ITrigEffJpsiTools> m_trigEffJpsiTools;	 // trigger matching tool
    
  //container names
  std::string		m_InnerTrackContainer;
  std::string           m_MuonContainer;
  std::vector<std::string> m_muonCollectionName;
  std::string           m_TruthParticleContainer;
  std::string           m_TrackParticleTruthCollection;
  std::string           m_McEventCollection;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //declaration of hists

  TH1F* m_EventSelection_Cut_Flow;
  TH1F* m_Tag_Cut_Flow;
  TH1F* m_Probe_Cut_Flow;

  TH2F* m_Probe_Nhits_vs_NoutliersTRT;
  TH1F* m_Probe_NhitsSCT;
  TH1F* m_Probe_NhitsPix;
  TH1F* m_Probe_d0Sig;
  TH1F* m_Probe_chi2ndf;
  TH1F* m_Probe_P;
 
  TH1F* m_TagAndProbe_Charge;
  TH1F* m_TagAndProbe_Cut_Flow;
  TH1F* m_TagAndProbe_ID_diMuonMass;
  TH1F* m_TagAndProbe_ID_diMuonMass_AfterVtx;
  TH1F* m_TagAndProbe_ID_diMuonMass_Refitted;
  TH1F* m_TagAndProbe_ID_Selected_RefJpsiMass;
  TH1F* m_TagAndProbe_ID_TrackPt;
  TH1F* m_TagAndProbe_ID_NumberOfTracks;
  TH1F* m_TagAndProbe_ID_NCandidates;

  TH1F* m_TagAndProbe_InvariantMass;
  TH1F* m_TagAndProbe_InvariantMass_AfterAssociationSTACO;
  TH1F* m_TagAndProbe_InvariantMass_AfterAssociationMUID;
  TH1F* m_TagAndProbe_InvariantMass_SameSign;
  TH1F* m_TagAndProbe_InvariantMass_SameSign_AfterAssociationSTACO;
  TH1F* m_TagAndProbe_InvariantMass_SameSign_AfterAssociationMUID;
  TH1F* m_TagAndProbe_DeltaR;
  TH1F* m_TagAndProbe_Propertime;
  TH1F* m_TagAndProbe_VertexChi2;


  TH1F* m_TagAndProbe_Mass_BeforeProbeAssociation;
  TH1F* m_TagAndProbe_Mass_BeforeProbeAssociation_L;  
  TH1F* m_TagAndProbe_Mass_BeforeProbeAssociation_SR;  
  TH1F* m_TagAndProbe_Mass_BeforeProbeAssociation_R;  
  TH1F* m_TagAndProbe_MassBkg_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_MassBkgExtracted_BeforeProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Mass_AfterProbeAssociation;
  std::vector<TH1F*> m_TagAndProbe_Mass_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Mass_AfterProbeAssociation_L;
  std::vector<TH1F*> m_TagAndProbe_Mass_AfterProbeAssociation_R;

  std::vector<TH1F*> m_TagAndProbe_MassBkgExtracted_AfterProbeAssociation;
  std::vector<TH1F*> m_TagAndProbe_MassBkgExtracted_AfterProbeAssociation_SR;

  TH1F* m_TagAndProbe_Pt_BeforeProbeAssociation_L;
  TH1F* m_TagAndProbe_Pt_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_Pt_BeforeProbeAssociation_R;
  TH1F* m_TagAndProbe_PtBkg_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_PtBkgExtracted_BeforeProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Pt_AfterProbeAssociation_L;
  std::vector<TH1F*> m_TagAndProbe_Pt_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Pt_AfterProbeAssociation_R;
  std::vector<TH1F*> m_TagAndProbe_PtBkg_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_PtBkgExtracted_AfterProbeAssociation_SR;


  TH1F* m_TagAndProbe_Eta_BeforeProbeAssociation_L;
  TH1F* m_TagAndProbe_Eta_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_Eta_BeforeProbeAssociation_R;
  TH1F* m_TagAndProbe_EtaBkg_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_EtaBkgExtracted_BeforeProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Eta_AfterProbeAssociation_L;
  std::vector<TH1F*> m_TagAndProbe_Eta_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Eta_AfterProbeAssociation_R;
  std::vector<TH1F*> m_TagAndProbe_EtaBkg_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_EtaBkgExtracted_AfterProbeAssociation_SR;

  TH1F* m_TagAndProbe_Phi_BeforeProbeAssociation_L;
  TH1F* m_TagAndProbe_Phi_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_Phi_BeforeProbeAssociation_R;
  TH1F* m_TagAndProbe_PhiBkg_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_PhiBkgExtracted_BeforeProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Phi_AfterProbeAssociation_L;
  std::vector<TH1F*> m_TagAndProbe_Phi_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Phi_AfterProbeAssociation_R;
  std::vector<TH1F*> m_TagAndProbe_PhiBkg_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_PhiBkgExtracted_AfterProbeAssociation_SR;

  ///////////////
  TH1F* m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation;
  TH1F* m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_L;  
  TH1F* m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_SR;  
  TH1F* m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_R;  
  TH1F* m_TagAndProbe_MassBkg_SameSign_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_MassBkgExtracted_SameSign_BeforeProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Mass_SameSign_AfterProbeAssociation;
  std::vector<TH1F*> m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_L;
  std::vector<TH1F*> m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_R;
  std::vector<TH1F*> m_TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation;
  std::vector<TH1F*> m_TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation_SR;

  TH1F* m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_L;
  TH1F* m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_R;
  TH1F* m_TagAndProbe_PtBkg_SameSign_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_PtBkgExtracted_SameSign_BeforeProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_L;
  std::vector<TH1F*> m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_R;
  std::vector<TH1F*> m_TagAndProbe_PtBkg_SameSign_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_PtBkgExtracted_SameSign_AfterProbeAssociation_SR;


  TH1F* m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_L;
  TH1F* m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_R;
  TH1F* m_TagAndProbe_EtaBkg_SameSign_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_EtaBkgExtracted_SameSign_BeforeProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_L;
  std::vector<TH1F*> m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_R;
  std::vector<TH1F*> m_TagAndProbe_EtaBkg_SameSign_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_EtaBkgExtracted_SameSign_AfterProbeAssociation_SR;

  TH1F* m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_L;
  TH1F* m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_R;
  TH1F* m_TagAndProbe_PhiBkg_SameSign_BeforeProbeAssociation_SR;
  TH1F* m_TagAndProbe_PhiBkgExtracted_SameSign_BeforeProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_L;
  std::vector<TH1F*> m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_R;
  std::vector<TH1F*> m_TagAndProbe_PhiBkg_SameSign_AfterProbeAssociation_SR;
  std::vector<TH1F*> m_TagAndProbe_PhiBkgExtracted_SameSign_AfterProbeAssociation_SR;


  //
  TH1F* m_TagAndProbe_NpassedTruth_vs_Pt;
  TH1F* m_TagAndProbe_NpassedTruth_vs_Phi;
  TH1F* m_TagAndProbe_NpassedTruth_vs_Eta;

  TH1F* m_TagAndProbe_Ntotal_vs_Pt;
  TH1F* m_TagAndProbe_Ntotal_vs_Phi;
  TH1F* m_TagAndProbe_Ntotal_vs_Eta;

  std::vector<TH1F*> m_TagAndProbe_NpassedReco_vs_Pt;
  std::vector<TH1F*> m_TagAndProbe_NpassedReco_vs_Phi;
  std::vector<TH1F*> m_TagAndProbe_NpassedReco_vs_Eta;

  std::vector<TH1F*> m_TagAndProbe_NpassedL1_vs_Pt;
  std::vector<TH1F*> m_TagAndProbe_NpassedL1_vs_Phi;
  std::vector<TH1F*> m_TagAndProbe_NpassedL1_vs_Eta;

  std::vector<TH1F*> m_TagAndProbe_NpassedL2_vs_Pt;
  std::vector<TH1F*> m_TagAndProbe_NpassedL2_vs_Phi;
  std::vector<TH1F*> m_TagAndProbe_NpassedL2_vs_Eta;

  std::vector<TH1F*> m_TagAndProbe_NpassedEF_vs_Pt;
  std::vector<TH1F*> m_TagAndProbe_NpassedEF_vs_Phi;
  std::vector<TH1F*> m_TagAndProbe_NpassedEF_vs_Eta;

  ///// Efficiency histograms
  std::vector<TH1F*> m_TagAndProbe_Efficiency_Eta;
  std::vector<TH1F*> m_TagAndProbe_Efficiency_Phi;
  std::vector<TH1F*> m_TagAndProbe_Efficiency_Pt;
  TH1F* m_TagAndProbe_Efficiency_Eta_Truth;
  TH1F* m_TagAndProbe_Efficiency_Phi_Truth;
  TH1F* m_TagAndProbe_Efficiency_Pt_Truth;
  std::vector<TH1F*> m_TagAndProbe_L1TriggerEfficiency_Eta;
  std::vector<TH1F*> m_TagAndProbe_L1TriggerEfficiency_Phi;
  std::vector<TH1F*> m_TagAndProbe_L1TriggerEfficiency_Pt;
  std::vector<TH1F*> m_TagAndProbe_L2TriggerEfficiency_Eta;
  std::vector<TH1F*> m_TagAndProbe_L2TriggerEfficiency_Phi;
  std::vector<TH1F*> m_TagAndProbe_L2TriggerEfficiency_Pt;
  std::vector<TH1F*> m_TagAndProbe_EFTriggerEfficiency_Eta;
  std::vector<TH1F*> m_TagAndProbe_EFTriggerEfficiency_Phi;
  std::vector<TH1F*> m_TagAndProbe_EFTriggerEfficiency_Pt;
  ////

  TProfile*       m_ID_MassShift_vs_Pt;
  TProfile*       m_ID_MassShift_vs_Eta;
  TProfile*       m_ID_MassShift_vs_Phi;

  std::vector<double>  m_binsPt;   //variable binning for Pt-efficiency plot
  std::vector<double>  m_binsEta;  //variable binning for eta-efficiency plot
  std::vector<double>  m_binsPhi;  //variable binning for phi-efficiency plot
 
  bool   m_doVertexing;
  bool   m_doTruth;
  bool   m_useVKalVrt;
  bool   m_doTrigger;
  bool   m_doTriggerEffL1;
  bool   m_doTriggerEffL2;
  bool   m_doTriggerEffEF;

  bool   m_doEfficiencies;
  bool   m_doBkgExtraction;
  bool   m_doBkgExtractionAfterProbeAssociation;

  bool   m_tagCombinedMuonOnly;

  //cuts
  double m_cut_tagMuonPt;
  double m_cut_probeMuonPt;
  double m_cut_probeMuonP;
  double m_cut_deltaRMin;
  double m_cut_deltaRMax;
  double m_cut_muonPt;
  double m_cut_muonEta;
  double m_cut_diMuonMass;
  double m_cut_properTime;
  double m_cut_JpsiVertexChi2;
  double m_cut_lowerMass;
  double m_cut_upperMass;

  //trk quality cuts
  double m_cut_nhTRT;
  double m_cut_noTRT;
  double m_cut_nhPix;
  double m_cut_nhSCT;
  double m_cut_d0Significance;
  double m_cut_chi2;
  double m_cut_p;

  double m_jpsiMass;
  double m_jpsiMassBinning;
  double m_jpsiSigma;
  double m_jpsiSideband;
  double m_jpsiSignalRegion;

  int nEvents;
  int nJpsiReco;
  int nJpsiTruth;
  int nJpsiMatch;

  //trigger items
  const ChainGroup* m_allL1;
  const ChainGroup* m_allL2;
  const ChainGroup* m_allEF;

  std::vector<std::string> m_trigMenus;
  std::string m_trigSelection;
  std::string m_tagMatchL1;
  std::string m_tagMatchL2;
  std::string m_tagMatchEF;

};

#endif
