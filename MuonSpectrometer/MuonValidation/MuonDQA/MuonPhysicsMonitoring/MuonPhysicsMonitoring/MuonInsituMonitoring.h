/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// MuonInsituMonitoring.h, (c) ATLAS Detector software
// **********************************************************************

#ifndef MUONINSITUMONITORING_H
#define MUONINSITUMONITORING_H

/// General Classes
#include <stdint.h>
#include <algorithm>
#include <math.h>
#include <functional>
#include <string>
/// Gaudi Tools
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/AlgTool.h"
/// Storegate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
/// AthenaMonitoring
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
/// ROOT Classes
#include "TH1.h"
#include "TH2.h"
#include "TLorentzVector.h"
/// Event Info
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
/// Muon Trigger
//#include "AnalysisTriggerEvent/LVL1_ROI.h"
//#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
//#include "TrigSteeringEvent/TriggerDecision.h"
/// Track Particles
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
/// Electron
#include "egammaEvent/Electron.h"
/// Muon
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
/// common implementation of all particles
//#include "ParticleEvent/ParticleBaseContainer.h"
/// particle jets
//#include "JetTagEvent/ParticleJetContainer.h"
//#include "JetTagEvent/JetConstituent.h"
#include "JetEvent/JetCollection.h"
//#include "JetTagEvent/TrackConstituents.h"  

/// Truth
//#include "McParticleEvent/TruthParticle.h"
//#include "McParticleEvent/TruthParticleContainer.h"

/// Own Tools
#include "MuonDQAUtils/IProbeCollectorTool.h"
#include "MuonDQAUtils/IInsituTrackTools.h"

//#include "MuonPhysicsMonitoring/APMuonEfficiencyMatrix.h"

//using namespace Insitu;

class AtlasDetectorID;
class Identifier;
class StoreGateSvc;
class IInterface;

/** @class MuonInsituMonitoring

This is for the Doxygen-Documentation.  
Please delete these lines and fill in information about the Algorithm!
Please precede every member function declaration with a
short Doxygen comment stating the purpose of this function.

@author  Nektarios Chr. Benekos <nbenekos@illinois.edu>
*/  

class MuonInsituMonitoring : public ManagedMonitorToolBase
{
 public:
		
  /** Standard AthenaManagedMonitorToolBase Constructor */
  MuonInsituMonitoring( const std::string & type, const std::string & name, const IInterface* parent );
		
  /** Default Destructor */
  virtual ~MuonInsituMonitoring();
  
  StatusCode initialize();
  StatusCode finalize();
		
  /** Standard AManagedMonitorToolBase Methods */
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

 protected:

 private:
	        
  StatusCode    bookMuonSpecHistograms();
  StatusCode    bookInDetHistograms();
  StatusCode    bookCombinedHistograms();
  StatusCode    procMuonSpecHistograms();
  StatusCode    procInDetHistograms();
  StatusCode    procCombinedHistograms();
  StatusCode	determineMuonEfficiency(const Rec::TrackParticle* msTrackParticle, int h_index);
  StatusCode	determineCombinedEfficiency(const Rec::TrackParticle* idTrackParticle, int h_index);
  StatusCode	determineIDEfficiency(const Rec::TrackParticle* combTrackParticle, int h_index);						
  StatusCode	determineMuonInsituEfficiency(const std::string m_MScurrentcontainer_key);
  StatusCode	determineCombinedInsituEfficiency(std::string m_MCBcurrentcontainer_key);
  StatusCode	determineIDInsituEfficiency(std::string m_IDcurrentcontainer_key);

  /// The following three functions should be exported to a new Algorithm/Tool 
  StatusCode	plotInvariantMasses(const Rec::TrackParticle*  track1, const Rec::TrackParticle* track2, std::string type, int h_index);
  double	getInvariantMass(const Rec::TrackParticle *track1, const Rec::TrackParticle *track2);
  bool		isAssociated(const Rec::TrackParticle *track1, const Rec::TrackParticle *track2);
  bool		MuonTrackAssociation(INavigable4Momentum *truth, std::string currentcontainer);
  bool		CombinedMuonTrackAssociation(INavigable4Momentum *truth, std::string currentcontainer);
  bool		InnerDetectorTrackAssociation(INavigable4Momentum *truth, std::string currentcontainer);

  std::string m_MScurrentcontainer_key;
  std::string m_MCBcurrentcontainer_key;
  std::string m_IDcurrentcontainer_key;

  /** a handle on Store Gate */
  StoreGateSvc* m_storeGate;
		
  /** Get a handle to the MSProbeCollectorTool */
  ToolHandle<IProbeCollectorTool> m_MSProbeCollectorTool;
		
  /** Get a handle to the MSProbeCollectorTool */
  ToolHandle<IProbeCollectorTool> m_IDProbeCollectorTool;
		
  /** Get a handle to the InsituTrackPerformanceTools */
  ToolHandle<IInsituTrackTools> 	m_InsituPerformanceTool;

  /** class member version of retrieving StoreGate */
  StoreGateSvc*  m_sgSvc;
				
  /** member variables for algorithm properties: */
	
  /** Information on Containers */
  std::vector<std::string>  m_MuonContainerLocation;
  std::vector<std::string>  m_MuonAuthor;
				
  std::vector<int>        m_doInnerDetector;
  std::vector<int> 	  m_doMuonSpectrometer;
  std::vector<int> 	  m_doCombined;
		
  ///+++ Nectar
  /// Representation of Efficiencies
 // std::map<std::string, Insitu::APMuonEfficiencyMatrix*>	m_apMuonEfficiencyMatrix;
  //+++Nectar
		
  double d_associationDistance;
		
  /** Cuts */
  double			m_DeltaR;
  double			m_MinPt;
  double			m_MaxEta;

  /// Histograms for plotting
  std::vector<TH1F*>	m_TH1F_TP_MuonInvariantMass;
  std::vector<TH1F*>	m_TH1F_TP_IDInvariantMass;
  std::vector<TH1F*>	m_TH1F_TP_CombinedInvariantMass;

  /// Histograms for efficiencies

  std::vector<TH1F*>	m_TH1F_TP_MuonEff_Eta;
  std::vector<TH1F*>	m_TH1F_TP_MuonEff_Phi;
  std::vector<TH1F*>	m_TH1F_TP_MuonEff_Pt;
		
  std::vector<TH1F*>	m_TH1F_TP_IDEff_Eta;
  std::vector<TH1F*>	m_TH1F_TP_IDEff_Phi;
  std::vector<TH1F*>	m_TH1F_TP_IDEff_Pt;
 		
  std::vector<TH1F*>	m_TH1F_TP_CombinedEff_Eta;
  std::vector<TH1F*>	m_TH1F_TP_CombinedEff_Phi;
  std::vector<TH1F*>	m_TH1F_TP_CombinedEff_Pt;
		
  std::map<std::string, TH1F*> m_EfficiencyHistograms;		
  //+++ Nectar
		
  /// Histograms for internal calculations
  std::vector<TH1F*>	m_TH1F_TP_MuonEff_D_Eta;
  std::vector<TH1F*>	m_TH1F_TP_MuonEff_N_Eta;
  std::vector<TH1F*>	m_TH1F_TP_MuonEff_D_Phi;
  std::vector<TH1F*>	m_TH1F_TP_MuonEff_N_Phi;
  std::vector<TH1F*>	m_TH1F_TP_MuonEff_D_Pt;
  std::vector<TH1F*>	m_TH1F_TP_MuonEff_N_Pt;

  std::vector<TH1F*>	m_TH1F_TP_IDEff_D_Eta;
  std::vector<TH1F*>	m_TH1F_TP_IDEff_N_Eta;
  std::vector<TH1F*>	m_TH1F_TP_IDEff_D_Phi;
  std::vector<TH1F*>	m_TH1F_TP_IDEff_N_Phi;
  std::vector<TH1F*>	m_TH1F_TP_IDEff_D_Pt;
  std::vector<TH1F*>	m_TH1F_TP_IDEff_N_Pt;

  std::vector<TH1F*>	m_TH1F_TP_CombinedEff_D_Eta;
  std::vector<TH1F*>	m_TH1F_TP_CombinedEff_N_Eta;
  std::vector<TH1F*>	m_TH1F_TP_CombinedEff_D_Phi;
  std::vector<TH1F*>	m_TH1F_TP_CombinedEff_N_Phi;
  std::vector<TH1F*>	m_TH1F_TP_CombinedEff_D_Pt;
  std::vector<TH1F*>	m_TH1F_TP_CombinedEff_N_Pt;
		
  /// Histograms for Pt
  TH1F*                 m_TH1F_msProbePt;
  TH1F*                 m_TH1F_idProbePt;
  TH1F*                 m_TH1F_combProbePt;
  std::vector<TH1F*>	m_TH1F_MuonPt;
  std::vector<TH1F*>	m_TH1F_IDPt;
  std::vector<TH1F*>	m_TH1F_CombinedPt;

				
}; 

#endif 
