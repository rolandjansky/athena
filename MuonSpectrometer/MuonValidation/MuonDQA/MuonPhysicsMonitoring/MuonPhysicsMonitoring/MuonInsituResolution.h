/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonInsituResolution.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MuonInsituResolution_H
#define MuonInsituResolution_H

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
  
/// AthenaMonitoring
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
 
/// ROOT Classes
#include "TH1.h"
#include "TH2.h"
#include "TChain.h"
#include "TLorentzVector.h"
/// TrackParticles
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
/// Muon
#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"
/// common implementation of all particles
//#include "ParticleEvent/ParticleBaseContainer.h"
/// Truth
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
/// Own Tools
#include "MuonDQAUtils/IProbeCollectorTool.h"
#include "MuonDQAUtils/IInsituTrackTools.h"

//#include "MuonPhysicsMonitoring/APMuonEfficiencyMatrix.h"

/// General C++ Classes
#include <stdint.h>
#include <algorithm>
#include <math.h>
#include <functional>
#include <string>

class AtlasDetectorID;
class Identifier;
class StoreGateSvc; 
class IInterface;
 
/** @class MuonInsituResolution

This is for the Doxygen-Documentation.  
Please delete these lines and fill in information about
the Algorithm!
Please precede every member function declaration with a
short Doxygen comment stating the purpose of this function.

@author  Nektarios Chr. Benekos <Nektarios.Benekos@cern.ch>
*/  

struct MCTruthTrack
{
  double	pt_truth;
  double	pt_rec;
  double	eta;
  double	phi;
};


//class MuonInsituResolution : public Algorithm
class MuonInsituResolution : public ManagedMonitorToolBase
{
 public:
	
  /** Standard Athena-Algorithm Constructor */
  //MuonInsituResolution(const std::string& name, ISvcLocator* pSvcLocator);
  MuonInsituResolution( const std::string & type, const std::string & name, const IInterface* parent );
		
  /** Default Destructor */
  //~MuonInsituResolution();
  virtual ~MuonInsituResolution();
		
  /** standard Athena-Algorithm method */
  StatusCode          initialize();
  
  /** standard Athena-Algorithm method */
  //StatusCode          execute();
  /** standard Athena-Algorithm method */
  StatusCode          finalize();
	
  /** Standard AManagedMonitorToolBase Methods */
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
		
 protected:

 private:

  StatusCode	readMCTruthResolutionTracks();
  StatusCode	fillMCTruthZBosonPlots(double scale, double res, const std::string CurrentTrackCollection);
  // StatusCode	fillDeterminedResolutionMatrix(double scale, double res, std::string mstrackcollection);
  StatusCode	writeToStoreGate();
  StatusCode	determineMuonResolution_MCTruth(const std::string m_Truthcurrentcontainer_key);
  StatusCode	determineZBosonMasses(std::string mstrackcollection);

  double	findMomentumScale(double from, double to, double res, const std::string CurrentTrackCollection);
  double	findResolutionScale(double from, double to, double scale, const std::string CurrentTrackCollection);
  bool		canAssociateMuonTrack(const INavigable4Momentum *truth, double &associatedMuonPt);

  /// a handle on Store Gate 
  StoreGateSvc* m_storeGate;
  StoreGateSvc* m_detStore;
 
  /** class member version of retrieving StoreGate */
  StoreGateSvc*  m_sgSvc;
 		
		
  /// get a handle to the MSProbeCollectorTool
  //ToolHandle<IInsituDatabaseTools> 	m_InsituDatabaseTool;
  ToolHandle<IInsituTrackTools> 	m_InsituPerformanceTool;

  /// a handle on the Hist/TTree registration service
  //ITHistSvc * m_thistSvc;
 
  /** member variables for algorithm properties: */
  //std::string	m_MSTrackContainerName;
  //std::string	m_CombinedMuonTracksContainerName;
  std::vector<std::string>	m_MSTrackContainerName;
  std::vector<int>              m_MSTrackContainerFlag;
  std::vector<std::string>      m_tracksName;
		
  std::vector<std::string>	m_CombinedMuonTracksContainerName;
  std::vector<int>              m_CombinedMuonTracksContainerFlag;
				
  /// Container for Track Parctilces
  const Rec::TrackParticleContainer* trackTES;

  /// Container for CombinedMuons
  const Analysis::MuonContainer* muonTDS;
		
  /// Histograms for plotting
  std::map<std::string, TH1F*>	m_histograms;
				
  double d_assoziationDistance;
						
  MCTruthTrack	m_track1;
  MCTruthTrack	m_track2;
  std::vector<MCTruthTrack>	m_vecMuon1;
  std::vector<MCTruthTrack>	m_vecMuon2;
  std::string m_MCTruthTrackRootFileName;
  std::string m_Truthcurrentcontainer_key;

}; 

#endif 
