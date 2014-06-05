/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Author: N. Benekos(Illinois) - I. Nomidis (Thessaloniki)
// MuonDQA
// Mar. 2009
//
// DESCRIPTION:
// Subject: 	This class does a full Insitu-Performance of the MuonSpectrometer
//		reconstruction efficiency, Inner-Tracker-Reconstruction Efficiency and
//		Trigger Efficiency
//		It is derivated from MuonPhysicsAnalysisMonitoring
///////////////////////////////////////////////////////////////////////////


#ifndef MuTrackPhysMonTool_H
#define MuTrackPhysMonTool_H

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
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Track Particles
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
 
// IP estimator
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

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

// Trigger
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"

// Isolation
//#include "TrackIsolationTools/TrackIsolationTool.h"

// ROOT Classes
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

//ROOT includes
class TH1;
class TH2;
class TString;
class TH1F_LW;
class TH2F_LW;

using namespace Trig;

class MuTrackPhysMonTool: public ManagedMonitorToolBase//, public  MuonPhysicsAnalysisMonitoring
{
 public:

  MuTrackPhysMonTool( const std::string & type, const std::string & name, const IInterface* parent );
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode procHistograms(); // execute
  virtual StatusCode fillHistograms();
  
  StatusCode initialize();
  StatusCode finalize();
          
 private:

  StoreGateSvc          *m_storeGate;
  ITHistSvc             * m_thistSvc;
  //  ITruthParticleCnvTool *m_cnvTool;

  bool isPassedTrackQualityCuts(const Rec::TrackParticle *track);
  bool isIDMuonTrack(const Rec::TrackParticle *innerDetectorTrack);
  bool isElectron(const Rec::TrackParticle *track)const;
  std::pair<double,double> getTrackPropertiesInCone(const Rec::TrackParticle *track,double coneMin, double coneMax);
  double getTrackIsolationInCone(const Rec::TrackParticle *track,double coneMin, double coneMax)const;

  double getJetEnergyInCone(const Rec::TrackParticle *track) const;
  double getETIsolation(const Analysis::Muon *muon);
  double getImpactParameter(const Rec::TrackParticle *track)const;

  ToolHandle<Trk::ITrackToVertexIPEstimator> m_trackToVertexIPEstimator; //for the estimation of the IPs to the Vertex, d0/sigma
 // ToolHandle<IIsolationTool>  m_muonIsolationTool;//!< MuonIsolationTool 
 
  //container names
  std::string		m_InnerTrackContainer;
  std::string		m_MuonSpectrometerTrackContainer;
  std::string           m_MuonContainer;
  std::string           m_JetContainer;
  std::string           m_ElectronContainer;
  std::string           m_TruthParticleContainer;
  std::string           m_TrackParticleTruthCollection;
  std::string           m_McEventCollection;

  // Temporary histograms to calculate efficiency vs pt/eta/phi
  TH1F_LW* m_mutrk_pttot;
  TH1F_LW* m_mutrk_pttrig;
  TH1F_LW* m_mutrk_pttruth;
  TH1F_LW* m_mutrk_ptpassed;

  TH1F_LW* m_mutrk_phitot;
  TH1F_LW* m_mutrk_phitrig;
  TH1F_LW* m_mutrk_phitruth;
  TH1F_LW* m_mutrk_phipassed;

  TH1F_LW* m_mutrk_etatot;
  TH1F_LW* m_mutrk_etatrig;
  TH1F_LW* m_mutrk_etatruth;
  TH1F_LW* m_mutrk_etapassed;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //declaration of hists

  TH1F_LW*           m_TagAndProbe_Charge;

  TH1F_LW*           m_TagAndProbe_Cut_Flow;
  TH1F_LW*           m_TagAndProbe_diMuonMass;
  TH1F_LW*           m_TagAndProbe_ZMass;
  TH1F_LW*           m_TagAndProbe_DeltaPhi;
  TH1F_LW*           m_TagAndProbe_NCandidates;
  TH1F_LW*           m_TagAndProbe_Pt;

  TH1F_LW*           m_Tag_TrackPt;
  TH1F_LW*           m_Tag_TrackEta;
  TH1F_LW*           m_Tag_RelTrkIso;
  TH1F_LW*           m_Tag_RelCaloIso;
  TH1F_LW*           m_Tag_RelJetIso;
  TH1F_LW*           m_Tag_AbsTrkIso;
  TH1F_LW*           m_Tag_AbsCaloIso;
  TH1F_LW*           m_Tag_AbsJetIso;
  TH1F_LW*           m_Tag_NCandidates;

  TH1F_LW*           m_Probe_TrackPt;  
  TH1F_LW*           m_Probe_TrackEta;
  TH1F_LW*           m_Probe_RelTrkIso;
  TH1F_LW*           m_Probe_RelCaloIso;
  TH1F_LW*           m_Probe_RelJetIso;
  TH1F_LW*           m_Probe_AbsTrkIso;
  TH1F_LW*           m_Probe_AbsCaloIso;
  TH1F_LW*           m_Probe_AbsJetIso;
  TH1F_LW*           m_Probe_NCandidates;

  TH1F_LW*           m_ProbeTrk_Cut_Flow;
  TH1F_LW*           m_ProbeTrk_TrackPt;
  //TH1F_LW*           m_ProbeTrk_TrackIso;
  TH1F_LW*           m_ProbeTrk_NhitsSCT;
  TH1F_LW*           m_ProbeTrk_NhitsPIX;
  TH1F_LW*           m_ProbeTrk_d0;
  TH1F_LW*           m_ProbeTrk_chi2ndf;

  TH1F*           m_TagAndProbe_Efficiency_Eta;
  TH1F*           m_TagAndProbe_Efficiency_Phi;
  TH1F*           m_TagAndProbe_Efficiency_Pt;
  TH1F*           m_TagAndProbe_Efficiency_Eta_Truth;
  TH1F*           m_TagAndProbe_Efficiency_Phi_Truth;
  TH1F*           m_TagAndProbe_Efficiency_Pt_Truth;
   
  TProfile*       m_ID_MassShift_vs_Pt;
  TProfile*       m_ID_MassShift_vs_Eta;
  TProfile*       m_ID_MassShift_vs_Phi;

  std::vector<double>  m_binsPt;   //variable binning for Pt-efficiency plot
  std::vector<double>  m_binsEta;  //variable binning for eta-efficiency plot
  std::vector<double>  m_binsPhi;  //variable binning for phi-efficiency plot

  bool   m_doTruth;
  bool   m_doTrigger;

  //cuts
  double m_cut_tagPt;
  double m_cut_probePt;
  double m_cut_deltaPhi;
  double m_cut_muonPt;
  double m_cut_muonEta;
  double m_cut_diMuonMass;
  double m_cut_isolationJet; 
  double m_cut_isolationTrk;
  double m_cut_isolationET; 
  double m_cut_charge;

  double m_tag_isoConeMax;

  //track quality requirements
  double m_trk_coneIso;
  double m_trk_pt;
  double m_trk_relTrkIso;
  double m_trk_nhitsSCT;
  double m_trk_nhitsPix;
  double m_trk_d0;
  double m_trk_chi2ndf;
  

  int nEvents;
  int nZReco;
  int nZTruth;
  int nZMatch;
  
  //isolation
  double m_isoConeMin;
  double m_isoConeMax;
  
  //trigger items
  const ChainGroup* m_allL1;
  const ChainGroup* m_allL2;
  const ChainGroup* m_allEF;
  std::vector<std::string> m_chains;

};

#endif

