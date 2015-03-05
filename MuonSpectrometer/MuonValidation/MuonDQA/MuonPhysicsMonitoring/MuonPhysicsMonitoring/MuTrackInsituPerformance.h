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


#ifndef MuTrackInsituPerformance_H
#define MuTrackInsituPerformance_H

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

// Track Summary
//#include "TrkTrackSummary/TrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

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
//#include "IsolationTool/IIsolationTool.h"

// ROOT Classes
#include "TH1F.h"
#include "TH2F.h"
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
class TH1I_LW;
class TH1F_LW;
class TH2F_LW;
class TProfile_LW;
class TString;

class AnalysisTools;
 
using namespace Trig;

namespace Trk {
  class ITrackSummaryTool;
  class TrackSummary;
}

class MuTrackInsituPerformance: public ManagedMonitorToolBase//, public  MuonPhysicsAnalysisMonitoring
{
 public:

  MuTrackInsituPerformance( const std::string & type, const std::string & name, const IInterface* parent );
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode procHistograms(); // execute
  virtual StatusCode fillHistograms();
  
  StatusCode initialize();
  StatusCode finalize();
	    
 private:

  StoreGateSvc          *m_storeGate;
  ITHistSvc             * m_thistSvc;
  //  ITruthParticleCnvTool *m_cnvTool;

  //  bool makeTrackQualityCuts(const Trk::Track* track);
  bool makeTrackQualityCuts(const Rec::TrackParticle* ptcl);
  bool applyTrkIsol(const Rec::TrackParticle* ptcl);
  bool getCaloEnergyIsolation(const Analysis::Muon* muon);
  const Trk::TrackParameters* getTrkParams(const Trk::Track* trk);

  bool isPassedZSelectionCuts(const Rec::TrackParticle *track1,const Rec::TrackParticle *track2,std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > listOfTrackPairs);
  bool isPassedTrackQualityCuts(const Rec::TrackParticle *trk);
   
  bool isIDMuonTrack(const Rec::TrackParticle *innerDetectorTrack);
  bool isElectron(const Rec::TrackParticle *track)const;
  bool isAssociatedWithMSTrack(const Rec::TrackParticle *track);

  std::pair<double,double> getTrackPropertiesInCone(const Rec::TrackParticle *track,double coneMin, double coneMax);
  double getTrackIsolationInCone(const Rec::TrackParticle *track,double coneMin, double coneMax);

  double getJetEnergyInCone(const Rec::TrackParticle *track) const;
  double getETIsolation(const Analysis::Muon *muon);
  double getImpactParameter(const Rec::TrackParticle *track)const;
  double getTrackIsolationP(const INavigable4Momentum *particle);
  
  ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool;    //for retrieval of SCT/Pixel hits
  ToolHandle<Trk::ITrackToVertexIPEstimator> m_trackToVertexIPEstimator; //for the estimation of the IPs to the Vertex, d0/sigma
 // ToolHandle<IIsolationTool>  m_muonIsolationTool;//!< MuonIsolationTool
 
  /** get a handle to the tool helper */
  ToolHandle<AnalysisTools> m_analysisTools;

  IToolSvc* m_tools; // tool service
 // ToolHandle<IIsolationTool>  m_isolationTool;
  
  /** Where should the perigee be taken from: the track or the TrackParticle? 
      Some Tracks don't have a perigee or a summary cannot be created out of them.
      TrackParticles constructed with the TrkParticleCreator(Tool) always have both.
      With this string one can choose were it should come from:
      Track ... from the Trk::Track (the TrackParticle points to).
      trackParticle .. from the TrackParticle.*/
  std::string m_TrkOrTrackParticle;

  //container names
  std::string		m_InnerTrackContainer;
  std::string		m_MuonSpectrometerTrackContainer;
  std::string           m_MuonContainer;
  std::string           m_JetContainer;
  std::string           m_ElectronContainer;
  std::string           m_TruthParticleContainer;
  std::string           m_TrackParticleTruthCollection;
  std::string           m_McEventCollection;
  std::string   m_vxContainerName;

  // Temporary histograms to calculate efficiency vs pt/eta/phi
  // std::vector<TH1F_LW*> 
  TH1F_LW* m_trk_pttot;
  TH1F_LW* m_trk_pttrig;
  TH1F_LW* m_trk_pttruth;
  TH1F_LW* m_trk_ptpassed;
  TH1F_LW* m_mutrk_pT;

  TH1F_LW* m_trk_phitot;
  TH1F_LW* m_trk_phitrig;
  TH1F_LW* m_trk_phitruth;
  TH1F_LW* m_trk_phipassed;

  TH1F_LW* m_trk_etatot;
  TH1F_LW* m_trk_etatrig;
  TH1F_LW* m_trk_etatruth;
  TH1F_LW* m_trk_etapassed;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //declaration of hists

  TH1F_LW*           m_TagAndProbe_Charge;
  TH1F_LW*           m_TagAndProbe_Cut_Flow;
  TH1F_LW*           m_TagAndProbe_ZMass;
  TH1F_LW*           m_TagAndProbe_diMuonMass;
  TH1F_LW*           m_TagAndProbe_DeltaPhi;
  TH1F_LW*           m_TagAndProbe_DeltaR;
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
  TH1F_LW*           m_ProbeTrk_TrackIso;
  TH1F_LW*           m_ProbeTrk_NhitsSCT;
  TH1F_LW*           m_ProbeTrk_NhitsPIX;
  TH1F_LW*           m_ProbeTrk_d0;
  TH1F_LW*           m_ProbeTrk_pT;
  TH1F_LW*           m_ProbeTrk_chi2ndf;
  
  TH1F_LW*           m_Z_NCandidates;

  TH1F*           m_TagAndProbe_Efficiency_Eta;
  TH1F*           m_TagAndProbe_Efficiency_Phi;
  TH1F*           m_TagAndProbe_Efficiency_Pt;
  TH1F*           m_TagAndProbe_Efficiency_Eta_Truth;
  TH1F*           m_TagAndProbe_Efficiency_Phi_Truth;
  TH1F*           m_TagAndProbe_Efficiency_Pt_Truth;

  // Track Quality plots
  TH1F_LW* m_mutrk_BLayerHits;

  TH1F_LW* m_mutrk_BLayerSharedHits;
  TH1F_LW* m_mutrk_PixelHits;
  TH1F_LW* m_mutrk_PixelLayers;
  TH1F_LW* m_mutrk_SiHits;
  TH1F_LW* m_mutrk_TRTHits;
  TH1F_LW* m_mutrk_d0;
  TH1F_LW* m_mutrk_chi2;
  TH1F_LW* m_mutrk_relIso;

  ///ptcone20 vs etcone20
  TH2F_LW * m_ptcone20_vs_etcone20;


  TProfile_LW* m_mutrk_relIso_vs_pT;
  TProfile_LW* m_Probe_RelTrkIso_vs_pT;
 
  std::vector<double>  m_binsPt;   //variable binning for Pt-efficiency plot
  std::vector<double>  m_binsEta;  //variable binning for eta-efficiency plot
  std::vector<double>  m_binsPhi;  //variable binning for phi-efficiency plot

  bool   m_doTruth;
  bool   m_doTrigger;

  bool   m_doMuonTypeCut;
  std::string m_muonType;

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
  
  double m_DRTagProbeMatchCut;

  // Calorimeter Isolation
  double m_energyIsoCone; //!< Cone size for energy isolation
  double m_eIsoBarrelCut;  //!< Energy isolation for a .45 cone in Barrel
  double m_eIsoPtRatioBarrelCut;  //!<E nergy isolation for a .45 cone in Barrel, normalized to track pt
  double m_eIsoTransitionCut;  //!< Energy isolation  for a .45 cone in Barrel-EndCap transition region
  double m_eIsoPtRatioTransitionCut;  //!< Energy isolation  for a .45 cone in Barrel-EndCap transition region, normalized to track pt
  double m_eIsoEndCapCut;  //!< Energy isolation for a .45 cone in Endcap 
  double m_eIsoPtRatioEndCapCut;  //!<  Energy isolation for a .45 cone in Endcap, normalized to track pt
 
  //track quality requirements
  double m_trk_coneIso;
  double m_trk_pt;
  double m_trk_relTrkIso;
  double m_trk_nhitsSCT;
  double m_trk_nhitsPix;
  double m_trk_d0;
  double m_trk_chi2ndf;
  bool   m_doProbeTrkMuonId;

  int nEvents;
  int nZReco;
  int nZTruth;
  int nZMatch;
  
  //isolation
  double m_isoConeMin;
  double m_isoConeMax;
  bool  m_CaloIsol;

  //trigger items
  const ChainGroup* m_allL1;
  const ChainGroup* m_allL2;
  const ChainGroup* m_allEF;
  std::vector<std::string> m_chains;

};

#endif

