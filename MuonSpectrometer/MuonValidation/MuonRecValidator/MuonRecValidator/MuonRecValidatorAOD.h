/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonRecValidatorAOD
// Authors:   M.Schott(LMU) - N. Benekos(MPI) 
// MuonValidation-Team
// Mar. 2007
//
// DESCRIPTION:
// Subject: General Analysis Tool for using CBNtuples in a AOD-way
///////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleKernel/ITruthParticleCnvTool.h"
  
#include "MuonRecValidator/MSMuonAnalysis.h"
#include "MuonRecValidator/MSDiMuonAnalysis.h"

#include "MuonValUtils/MSEvent.h"
#include "MuonValUtils/MSTrackObject.h"
#include "MuonValUtils/MSTrackObjectLoader.h"
#include "MuonValUtils/MSEfficiency.h"
#include "MuonValUtils/MSRootInterface.h"
#include "MuonValUtils/MSPhysicsObject.h"
#include "TH1.h"

#include <string>

class IHistogram1D;
class IHistogram2D;
class TFile;
class TTree;
class IAnalysisTools;

namespace Reco { class ITrackToVertex; }

/////////////////////////////////////////////////////////////////////////////
class MuonRecValidatorAOD:public AthAlgTool
{
 public:
  MuonRecValidatorAOD (const std::string & type, const std::string & name, const IInterface * parent);
  ~MuonRecValidatorAOD();
   
   StatusCode initialize();
   StatusCode finalize();
   StatusCode fillNtuple();
  
 private:
  void	setMSEventDumpBranches();
  double distance(MSPhysicsObject &ob1, MSPhysicsObject &ob2);
  double errorPT(double theta, double qOverP, double D_theta, double D_qOverP);
  double errorEta(double theta, double D_theta);
  double cot(double theta);
  int getMother(TruthParticle *truthParticle);
  void setRanges(vector<float> &vRanges_Eff, vector<float> &vRanges_Res);
  int get_vertex(const TruthParticle* par, double& vx, double& vy, double& vz);
  int m_VersionID;
  long EventNumber;
  long m_FirstEventNumber;
  int m_bufferSize;
  int m_splitLevel;
  double m_MaxConeDistance;
  double m_MinimalMuonPt;
  bool m_TriggerAware;
  bool m_VersionComparing;
  bool m_MustBeCombinedTrack;
  bool m_EventDumping;
  bool m_AnalyseDiMuonFinalState;
  std::string m_Outputfile;
  std::string m_VersionComparingTriggerName;
  std::string m_VersionComparingContainerName;
  std::string m_VersionComparingCollectionName;
  std::string m_VersionComparingName;
  std::string m_VersionComparingName1;
  std::string m_VersionComparingName2;
  std::string m_JetContainer;
  std::string m_ElectronContainer;

  vector<std::string> m_TrackContainerNames;
  vector<std::string> m_MuonCollectionNames;
  vector<std::string> m_TriggerContainerNames;
						
  vector<float> m_EfficiencyBinning_vs_Pt;
  vector<float>	m_EfficiencyBinning_vs_Eta;
  vector<float>	m_EfficiencyBinning_vs_Phi;
  vector<float>	m_ResolutionBinning_vs_Pt;
  vector<float>	m_ResolutionBinning_vs_Eta;
  vector<float>	m_ResolutionBinning_vs_Phi;				
  vector<float>	m_DiMuonBinning_vs_Pt;
  vector<float>	m_DiMuonBinning_vs_Eta;
	
  MSEvent *msPointerEvent;
  MSEvent msCurrectEvent;
  MSEvent msDummyEvent;
  MSMuonAnalysis msMuonAnalysis;
  MSDiMuonAnalysis msDiMuonAnalysis;
  cutMuonAnalDefinitons  cutMu;
			
  /** coverter tool McEventCollection -> TruthParticle */
  ITruthParticleCnvTool* m_cnvTool;
		
  /// name of the AOD truth particle container to retrieve from Store Gate
  std::string m_truthParticlesName;

  /** switch to build TruthParticles from ESD TruthEvent */
  bool m_fromESD;
		
  /// get a handle to the tool helper
  IAnalysisTools *m_analysisTools;
 
  /// Track extraoplation to the vertex
  Reco::ITrackToVertex* m_trackToVertexTool;

  /// a handle on Store Gate
  StoreGateSvc*	m_storeGate;

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  MSRootInterface msRootInterface;
  vector<TH1F*>	m_h_BasicHistos;

  MSTrackObjectLoader	msTrackObjectLoader;

  /** Event Dumoung */
  vector<string> vecMSEventDumpNames;
  vector<string> *pMSEventDumpNames;
  map<string, MSEventConverterObject> mapDumpMSEvent;
 
  TTree* m_tree ;

};
