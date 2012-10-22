/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI) - M.Bellomo (INFN)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class does a full Zmumu-Analysis based on AODs.
//		First it loads the AOD-information to MSEvent. The analysis itself
//		is based on MSEvent and done in MSZBosonAnalysis
///////////////////////////////////////////////////////////////////////////

// GaudiKernel and StoreGate
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

// EventInfo - HepMC - McParticleEvent
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "HepMC/GenEvent.h"//new
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthParticle.h" 

//JetTagEvent
//#include "JetTagEvent/ParticleJetContainer.h"
//#include "JetTagEvent/ParticleJet.h"
#include "JetTagEvent/JetConstituent.h"
#include "JetEvent/JetCollection.h"
#include "JetTagEvent/TrackConstituents.h"
 
//AnalysisTools
#include "AnalysisTools/IAnalysisTools.h"

// Particle and Track
#include "Particle/TrackParticleContainer.h"
#include "ITrackToVertex/ITrackToVertex.h"

//Muon and  Electron Container 
#include "muonEvent/MuonContainer.h" 
#include "egammaEvent/ElectronContainer.h"
 
//For Trigger
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigSteeringEvent/TriggerDecision.h"

#include "MuonInSituPerformance/ZmumuValidator.h"

#include "TTree.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////

ZmumuValidator::ZmumuValidator(const std::string& type, const std::string& name, const IInterface * parent) 
              : AthAlgTool(type,name,parent), m_analysisTools(0)
{

  // Part 2: Properties go here


  declareProperty("OutputFile",			m_Outputfile = "ZmumuValidation.root");
  declareProperty("FromESD",			m_fromESD = false);
  declareProperty("MaximalConeDistance",	m_MaxConeDistance = 0.05);
  declareProperty("MinimalMuonPt",		m_MinimalMuonPt   = 6.*CLHEP::GeV);

  declareProperty("MuonContainer",		m_MuonContainer = "StacoMuonCollection");
  declareProperty("MuonSpectrometerContainer",	m_MuonSpectrometerTrackContainer = "MuonboyTrackParticles");
  declareProperty("InnerTrackContainer",	m_InnerTrackContainer = "TrackParticleCandidate");
  declareProperty("TriggerContainer",		m_TriggerContainer);

  declareProperty("TriggerLevel",		m_TriggerLevel = TriggerDecision::EF);
  declareProperty("TriggerItem",		m_TriggerItem = "EF_mu20i");
  declareProperty("Cut_TrigMu_Pt",	        cutD.TrigMu_Pt	= 20000.); // MSInsituPerformance and MSPhysicsAnalysis
    
  declareProperty("Cut_Pt_loose",               cutD.Pt_loose	= 15000.);
  declareProperty("Cut_Pt_tight",	        cutD.Pt_tight	= 20000.);
  declareProperty("Cut_diMuonMass",	        cutD.diMuonMass	= 30000.);
  
  declareProperty("Cut_ZInvmMassWnd",	        cutD.ZInvmMassWnd = 12000.);


  declareProperty("Cut_deltaPhi",	        cutD.deltaPhi	= 0.1);
  declareProperty("Cut_Jet_InHalo_Loose",	cutD.Jet_InHalo_Loose		= 25000.);
  declareProperty("Cut_NTracks_InHalo_Loose",	cutD.NTracks_InHalo_Loose	= 6.);
  declareProperty("Cut_PTTracks_InHalo_Loose",	cutD.PTTracks_InHalo_Loose	= 15000.);
  declareProperty("Cut_ET_InHalo_Loose",	cutD.ET_InHalo_Loose		= 20000.);

  declareProperty("Cut_Jet_InHalo_Tight",	cutD.Jet_InHalo_Tight		= 15000.);
  declareProperty("Cut_NTracks_InHalo_Tight",	cutD.NTracks_InHalo_Tight	= 4.);
  declareProperty("Cut_PTTracks_InHalo_Tight",	cutD.PTTracks_InHalo_Tight	= 8000.);
  declareProperty("Cut_ET_InHalo_Tight",	cutD.ET_InHalo_Tight		= 6000.);
  declareProperty("TruthParticles",             m_truthParticlesName = "SpclMC_ESD");
  declareProperty("ConvertFromESD",             m_fromESD = false );
}
/***************************************************************************/ 
StatusCode ZmumuValidator::initialize()
{
  vector<float>	vRanges_Eff, vRanges_Res;

  // Part 1: Get the messaging service, print where you are
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::INFO <<  "Initializing ZmumuValidator" << endreq;

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR
	   << "Unable to retrieve pointer to StoreGateSvc"
	   << endreq;
      return sc;
    }

  /// get a handle on the analysis tools
  IToolSvc* toolSvc;
  sc = service("ToolSvc",toolSvc);
  if (StatusCode::SUCCESS != sc)
    {
      mLog << MSG::ERROR << " Can't get ToolSvc" << endreq;
      return StatusCode::FAILURE;
    }

  IAlgTool *tmp_analysisTools;
  sc = toolSvc->retrieveTool("AnalysisTools",tmp_analysisTools);
  if (StatusCode::SUCCESS != sc)
    {
      mLog << MSG::ERROR << "Can't get handle on analysis tools" << endreq;
      return StatusCode::FAILURE;
    }
  m_analysisTools=dynamic_cast<IAnalysisTools *>(tmp_analysisTools);

  sc = toolSvc->retrieveTool( "TruthParticleCnvTool/CnvTool", tmp_analysisTools, this );
  if (StatusCode::SUCCESS != sc)
    {
      mLog << MSG::ERROR << "Creation of algTool ITruthParticleCnvTool" << " FAILED !" << endreq;
      return StatusCode::FAILURE;
 
    } else {
      mLog << MSG::DEBUG << "AlgTool ITruthParticleCnvTool successfully created" << endreq;
      m_cnvTool = dynamic_cast<ITruthParticleCnvTool*>(tmp_analysisTools);
    }

  sc = service("THistSvc", m_thistSvc);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR
	   << "Unable to retrieve pointer to THistSvc"
	   << endreq;
      return sc;
    }

  msZBosonAnalysis.setup(m_Outputfile, "ZmumuAnalysis", cutD, m_MuonContainer);
  msInsituPerformance.setup(m_Outputfile, "InsituPerformance", cutD, m_InnerTrackContainer, m_MuonSpectrometerTrackContainer, m_MuonContainer, m_TriggerContainer);
  msTrackObjectLoader.setup(m_storeGate, true, true, m_fromESD,   m_cnvTool);
  msTrackObjectLoader.setupTrackPropertyAssoziator(true, true, 0.05, 0.5);
  msTrackObjectLoader.setAssoziationTrackContainerName(m_MuonSpectrometerTrackContainer);
  msTrackObjectLoader.setAssoziationTrackContainerName(m_InnerTrackContainer);
  msTrackObjectLoader.setAssoziationMuonContainerName(m_MuonContainer);

  EventNumber = 0;

  return StatusCode::SUCCESS;
}
/***************************************************************************/
StatusCode ZmumuValidator::fillNtuple()
{
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::INFO << "execute()" << endreq;

  const EventInfo* pevt;
  if (m_storeGate->retrieve(pevt).isFailure())
    {
      mLog << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
    }
  else
    {
      mLog << MSG::DEBUG << "Found EventInfo in SG" << endreq;
    }

  msCurrectEvent.clear();
  msTrackObjectLoader.loadTruthInfo(msCurrectEvent);
  msTrackObjectLoader.loadTrackInfo(m_MuonSpectrometerTrackContainer, msCurrectEvent);
  msTrackObjectLoader.loadTrackInfo(m_InnerTrackContainer, msCurrectEvent);
  msTrackObjectLoader.loadMuonCollectionInfo(m_MuonContainer, msCurrectEvent);
  msTrackObjectLoader.loadMuonTriggerInfo(msCurrectEvent);

  EventNumber++;

  msZBosonAnalysis.analyse(msCurrectEvent);
	
  mLog << MSG::INFO << "*** Tag&Probe Method *** testing reconstruction and trigger efficiency with item " 
       << m_TriggerItem << endreq;
	
  msInsituPerformance.analyseMuonSpectrometerEfficiencies(msCurrectEvent, EventNumber);
  msInsituPerformance.analyseInnerTrackerEfficiency(msCurrectEvent);
	
  mLog << MSG::INFO << "*** DO Method *** testing trigger efficiency with item " 
       << m_TriggerItem << endreq;
	
  msInsituPerformance.analyseMuonSpectrometerTriggerEfficienciesDO(msCurrectEvent, EventNumber, m_TriggerItem, m_TriggerLevel);

  return StatusCode::SUCCESS;
}
/***************************************************************************/ 
StatusCode ZmumuValidator::finalize()
{

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;

  TDirectory *tDir = new TDirectory();

  std::string	name;
  name=m_Outputfile+":/";

  tDir->cd(name.c_str());
  msZBosonAnalysis.writeResults();
  msInsituPerformance.writeResults();
  tDir->cd(name.c_str());
  delete tDir;
  return StatusCode::SUCCESS;
}
/***************************************************************************/
ZmumuValidator::~ZmumuValidator()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "ZmumuValidator destructor called" << endreq;
}
/***************************************************************************/ 
