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


#ifndef MuonInsituPerformance_H
#define MuonInsituPerformance_H

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

//#include "MuonPhysicsMonitoring/MuonPhysicsAnalysisMonitoring.h"

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
 
class MuonInsituPerformance: public ManagedMonitorToolBase//, public  MuonPhysicsAnalysisMonitoring
{
 public:

  MuonInsituPerformance( const std::string & type, const std::string & name, const IInterface* parent );
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode procHistograms(); // execute
  virtual StatusCode fillHistograms();
  
  StatusCode initialize();
  StatusCode finalize();
          
 private:

  StoreGateSvc          *m_storeGate;
  ITHistSvc             * m_thistSvc;
  //  ITruthParticleCnvTool *m_cnvTool;

  bool isAssociatedWithMSTrack(const Rec::TrackParticle *innerDetectorTrack);
  bool isElectron(const Rec::TrackParticle *track);
  std::pair<double,double> getTrackPropertiesInCone(const Rec::TrackParticle *track);
  double getJetEnergyInCone(const Rec::TrackParticle *track) const;
  double getETIsolation(const Analysis::Muon *muon);

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
  TH1F* m_h_pt_tot;
  TH1F* m_h_pt_trig;
  TH1F* m_h_pt_truth;
  TH1F* m_h_pt_recon;

  TH1F* m_h_phi_tot;
  TH1F* m_h_phi_trig;
  TH1F* m_h_phi_truth;
  TH1F* m_h_phi_recon;

  TH1F* m_h_eta_tot;
  TH1F* m_h_eta_trig;
  TH1F* m_h_eta_truth;
  TH1F* m_h_eta_recon;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //declaration of hists

  TH1F*           m_TagAndProbe_Charge;

  TH1F*           m_TagAndProbe_Cut_Flow;
  TH1F*           m_TagAndProbe_ID_diMuonMass;
  TH1F*           m_TagAndProbe_ID_Selected_ZMass;
  TH1F*           m_TagAndProbe_ID_DeltaPhi;
  TH1F*           m_TagAndProbe_ID_TrackPt;
  TH1F*           m_TagAndProbe_ID_NumberOfTracks;
  TH1F*           m_TagAndProbe_ID_NCandidates;
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
  double m_cut_tagMuonPt;
  double m_cut_probeMuonPt;
  double m_cut_deltaPhi;
  double m_cut_muonPt;
  double m_cut_muonEta;
  double m_cut_diMuonMass;
  double m_cut_isolationJet; 
  double m_cut_isolationNtracks;
  double m_cut_isolationPtSum;
  double m_cut_isolationET; 

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

