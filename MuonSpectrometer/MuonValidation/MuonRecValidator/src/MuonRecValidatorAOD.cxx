/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonRecValidatorAOD
// Author:   M.Schott(LMU) - N. Benekos(MPI) 
// MuonValidation-Team
// Mar. 2007
//
// DESCRIPTION:
// Subject: General Analysis Tool for using CBNtuples in a AOD-way
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

#include "MuonRecValidator/MuonRecValidatorAOD.h"

#include "TTree.h"
#include "TDirectory.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////

MuonRecValidatorAOD::MuonRecValidatorAOD(const std::string& type, const std::string& name, const IInterface * parent)
  : AthAlgTool(type,name,parent),
    m_analysisTools(0)
{

  // Part 2: Properties go here
  m_TrackContainerNames.clear();
  mapDumpMSEvent.clear();
  declareProperty("OutputFile",			   m_Outputfile = "MuonValidation.root");
  declareProperty("MaximalConeDistance",	   m_MaxConeDistance = 0.05);
  declareProperty("MinimalMuonPt",		   m_MinimalMuonPt   = 6.*CLHEP::GeV);
  declareProperty("MuonRecTrackPtCut",		   cutMu.muonRec_Pt  = 20.*CLHEP::GeV); // MSMuonAnalysis
  declareProperty("TrigMu_PtCut",	           cutMu.TrigMu_Pt  = 20.*CLHEP::GeV); // MSMuonAnalysis
    
  declareProperty("OutputFile",			   m_Outputfile = "MuonValidation.root");

  declareProperty("VersionComparingName1",	   m_VersionComparingName1="Version1");
  declareProperty("VersionComparingName2",	   m_VersionComparingName2="Version2");
  declareProperty("VersionComparing",		   m_VersionComparing = false);
  declareProperty("VersionComparingContainerName", m_VersionComparingContainerName="MuonboyTrackParticles");
  declareProperty("VersionComparingCollectionName",m_VersionComparingCollectionName="");
  declareProperty("VersionComparingTriggerName",   m_VersionComparingTriggerName="Trigger_Level_1");

  declareProperty("TrackContainerNames",	   m_TrackContainerNames);
  declareProperty("MuonCollectionNames",	   m_MuonCollectionNames);
	
  declareProperty("TriggerAware",  	           m_TriggerAware = false);
	
  declareProperty("ElectronCollectionName",	   m_ElectronContainer="egammaCollection");
  declareProperty("JetParticleContainerName",	   m_JetContainer="Kt4H1TowerJets");

  declareProperty("MustBeCombinedTrack",	   m_MustBeCombinedTrack = false);

  declareProperty("EfficiencyBinning_vs_Pt",	   m_EfficiencyBinning_vs_Pt);
  declareProperty("EfficiencyBinning_vs_Eta",	   m_EfficiencyBinning_vs_Eta);
  declareProperty("EfficiencyBinning_vs_Phi",	   m_EfficiencyBinning_vs_Phi);

  declareProperty("ResolutionBinning_vs_Pt",	   m_ResolutionBinning_vs_Pt);
  declareProperty("ResolutionBinning_vs_Eta",	   m_ResolutionBinning_vs_Eta);
  declareProperty("ResolutionBinning_vs_Phi",	   m_ResolutionBinning_vs_Phi);

  declareProperty("AnalyseDiMuonFinalState",	   m_AnalyseDiMuonFinalState = false);
  declareProperty("DiMuonBinning_vs_Pt",	   m_DiMuonBinning_vs_Pt);
  declareProperty("DiMuonBinning_vs_Eta",	   m_DiMuonBinning_vs_Eta);

  declareProperty("EventDumping",		   m_EventDumping=false);

  declareProperty("TTreeBranchBufferSize",	   m_bufferSize=32000);
  declareProperty("TTreeBranchSplitLevel",	   m_splitLevel=0);
	
  declareProperty("TruthParticles",                m_truthParticlesName = "SpclMC_ESD");
  declareProperty("ConvertFromESD",                m_fromESD = false );
}
/***************************************************************************/
StatusCode MuonRecValidatorAOD::initialize()
{
  vector<float>	vRanges_Eff, vRanges_Res;
  setRanges(vRanges_Eff, vRanges_Res);
  
  // Part 1: Get the messaging service, print where you are
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::INFO <<  "Initializing MuonRecValidatorAOD" << endreq;

  StatusCode sc = StatusCode::SUCCESS;
  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "Unable to retrieve pointer to StoreGateSvc" << endreq;
      return sc;
    }

  /// get a handle on the analysis tools
  IToolSvc* toolSvc;
  sc = service("ToolSvc",toolSvc);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << " Can't get ToolSvc" << endreq;
      return sc;
    } 
	
  IAlgTool* algTool = 0; 
  sc = toolSvc->retrieveTool( "TruthParticleCnvTool/CnvTool", algTool, this );
				
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "Creation of algTool ITruthParticleCnvTool failed" << endreq;
      return sc;
    }  	
  m_cnvTool = dynamic_cast<ITruthParticleCnvTool *>(algTool);
	
  msTrackObjectLoader.setup(m_storeGate, true, true, m_fromESD, m_cnvTool);
  //msTrackObjectLoader.setupTrackPropertyAssoziator(true, true, 0.05, 0.4);

  IAlgTool *tmp_analysisTools;
  sc = toolSvc->retrieveTool("AnalysisTools",tmp_analysisTools);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "Can't get handle on analysis tools" << endreq;
      return sc;
    }
  m_analysisTools=dynamic_cast<IAnalysisTools *>(tmp_analysisTools);

  sc = service("THistSvc", m_thistSvc);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "Unable to retrieve pointer to THistSvc" << endreq;
      return sc;
    }

  if (m_VersionComparing==true) // Keep only the first Container!
    {
      string tmp;
      m_TrackContainerNames.clear();
      m_MuonCollectionNames.clear();
      if (m_VersionComparingCollectionName=="")
	{
	  m_VersionComparingName=m_VersionComparingContainerName;
	  m_TrackContainerNames.push_back(m_VersionComparingContainerName);
	}
      else
	{
	  m_VersionComparingName=m_VersionComparingCollectionName;
	  m_MuonCollectionNames.push_back(m_VersionComparingCollectionName);
	}
    }


  vector<std::string>	m_ContainerNames;
  for (int i=0; i<(int)m_TrackContainerNames.size(); i++)
    {
      m_ContainerNames.push_back(m_TrackContainerNames[i]);
      msTrackObjectLoader.setAssoziationTrackContainerName(m_TrackContainerNames[i]);
    }
  for (int i=0; i<(int)m_MuonCollectionNames.size(); i++)
    {
      m_ContainerNames.push_back(m_MuonCollectionNames[i]);
      msTrackObjectLoader.setAssoziationMuonContainerName(m_MuonCollectionNames[i]);
    }

  std::string	name;

  EventNumber=0;
  m_FirstEventNumber=-1;
  m_VersionID=0;
  
  if (m_EventDumping==true) setMSEventDumpBranches();
  
  m_TriggerContainerNames.push_back("Trigger_Level_1");     // 0 
  m_TriggerContainerNames.push_back("Trigger_Level_2_MS");  // 1
  m_TriggerContainerNames.push_back("Trigger_Level_2_CB");  // 2
  m_TriggerContainerNames.push_back("Trigger_Level_3_MS");  // 3
  m_TriggerContainerNames.push_back("Trigger_Level_3_CB");  // 4
  m_TriggerContainerNames.push_back("Trigger_OverAll_MS");  // 5
  m_TriggerContainerNames.push_back("Trigger_OverAll_CB");  // 6

  if (m_VersionComparing==true)
    {
      msMuonAnalysis.setup(	m_Outputfile, 
                                cutMu,
				m_MaxConeDistance,
				m_MinimalMuonPt,
				m_VersionComparingName,
				m_VersionComparingTriggerName,
				vRanges_Eff,
				vRanges_Res,
				m_VersionComparingName1,
				m_VersionComparingName2);
      
      if (m_AnalyseDiMuonFinalState)
	{
	  msDiMuonAnalysis.setup(m_Outputfile,
				 m_MaxConeDistance,
				 m_MinimalMuonPt,
				 m_VersionComparingName,
				 m_VersionComparingName1,
				 m_VersionComparingName2,
				 m_DiMuonBinning_vs_Pt,
				 m_DiMuonBinning_vs_Eta);
	}
    } else
      {
	msMuonAnalysis.setup(m_Outputfile, 
	                     cutMu,
			     m_MaxConeDistance,
			     m_MinimalMuonPt,
			     m_ContainerNames,
			     m_TriggerContainerNames,
			     vRanges_Eff,
			     vRanges_Res,
			     m_TriggerAware);

	if (m_AnalyseDiMuonFinalState)
	  {
	    msDiMuonAnalysis.setup(m_Outputfile,
				   m_MaxConeDistance,
				   m_MinimalMuonPt,
				   m_ContainerNames,
				   m_DiMuonBinning_vs_Pt,
				   m_DiMuonBinning_vs_Eta);
	  }
      }

  return sc;
}
/***************************************************************************/
void MuonRecValidatorAOD::setMSEventDumpBranches()
{
  vecMSEventDumpNames.push_back("Truth");
  //vecMSEventDumpNames.push_back(m_JetContainer);
  //vecMSEventDumpNames.push_back(m_ElectronContainer);
  for (int i=0; i<(int)m_TriggerContainerNames.size(); i++) vecMSEventDumpNames.push_back(m_TriggerContainerNames[i]);
  for (int i=0; i<(int)m_TrackContainerNames.size(); i++) vecMSEventDumpNames.push_back(m_TrackContainerNames[i]);
  for (int i=0; i<(int)m_MuonCollectionNames.size(); i++) vecMSEventDumpNames.push_back(m_MuonCollectionNames[i]);

  pMSEventDumpNames = new vector<string>;
  for (int i=0; i<(int)vecMSEventDumpNames.size(); i++) pMSEventDumpNames->push_back(vecMSEventDumpNames[i]);
  
//  addBranch("MSEventDump_ContainerNames",	pMSEventDumpNames);
  m_tree->Branch("MSEventDump_ContainerNames",pMSEventDumpNames);

  MSEventConverterObject ECODummy;
  for (int i=0; i<(int)vecMSEventDumpNames.size();i++)	mapDumpMSEvent[vecMSEventDumpNames[i]]=ECODummy;

  for (int i=0; i<(int)vecMSEventDumpNames.size();i++)
    {
      msDummyEvent.setupMSEventConverterObject(mapDumpMSEvent[vecMSEventDumpNames[i]]);
/*
      addBranch((vecMSEventDumpNames[i]+"_m_pdgID").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_pdgID);
      addBranch((vecMSEventDumpNames[i]+"_m_mother").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_mother);
      addBranch((vecMSEventDumpNames[i]+"_m_fPt").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_fPt);
      addBranch((vecMSEventDumpNames[i]+"_m_fEta").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_fEta);
      addBranch((vecMSEventDumpNames[i]+"_m_fPhi").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_fPhi);
      addBranch((vecMSEventDumpNames[i]+"_m_fR0").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_fR0);
      addBranch((vecMSEventDumpNames[i]+"_m_fZ0").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_fZ0);
      addBranch((vecMSEventDumpNames[i]+"_m_charge").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_charge);

      addBranch((vecMSEventDumpNames[i]+"_m_fDeltaPt").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_fDeltaPt);
      addBranch((vecMSEventDumpNames[i]+"_m_fDeltaEta").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_fDeltaEta);
      addBranch((vecMSEventDumpNames[i]+"_m_fDeltaPhi").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_fDeltaPhi);

      addBranch((vecMSEventDumpNames[i]+"_m_type").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_type);
      addBranch((vecMSEventDumpNames[i]+"_m_numberOfHits").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_numberOfHits);
      addBranch((vecMSEventDumpNames[i]+"_m_chi2").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_chi2);
      addBranch((vecMSEventDumpNames[i]+"_m_chi2oDOF").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_chi2oDOF);
      addBranch((vecMSEventDumpNames[i]+"_m_energyLoss").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_energyLoss);

      addBranch((vecMSEventDumpNames[i]+"_m_triggerStatus").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_triggerStatus);
      addBranch((vecMSEventDumpNames[i]+"_m_isElectron").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_isElectron);
      addBranch((vecMSEventDumpNames[i]+"_m_isZmuon").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_isZmuon);
      addBranch((vecMSEventDumpNames[i]+"_m_trackPt_iso").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_trackPt_iso);
      addBranch((vecMSEventDumpNames[i]+"_m_ntrack_iso").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_ntrack_iso);
      addBranch((vecMSEventDumpNames[i]+"_m_jet_iso").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_jet_iso);
      addBranch((vecMSEventDumpNames[i]+"_m_et_iso").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_et_iso);
      addBranch((vecMSEventDumpNames[i]+"_m_mass").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_mass);
*/

      m_tree->Branch((vecMSEventDumpNames[i]+"_m_pdgID").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_pdgID);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_mother").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_mother);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_fPt").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_fPt);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_fEta").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_fEta);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_fPhi").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_fPhi);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_fR0").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_fR0);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_fZ0").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_fZ0);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_charge").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_charge);

      m_tree->Branch((vecMSEventDumpNames[i]+"_m_fDeltaPt").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_fDeltaPt);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_fDeltaEta").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_fDeltaEta);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_fDeltaPhi").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_fDeltaPhi);

      m_tree->Branch((vecMSEventDumpNames[i]+"_m_type").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_type);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_numberOfHits").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_numberOfHits);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_chi2").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_chi2);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_chi2oDOF").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_chi2oDOF);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_energyLoss").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_energyLoss);

      m_tree->Branch((vecMSEventDumpNames[i]+"_m_triggerStatus").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_triggerStatus);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_isElectron").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_isElectron);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_isZmuon").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_isZmuon);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_trackPt_iso").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_trackPt_iso);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_ntrack_iso").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_ntrack_iso);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_jet_iso").c_str(),	mapDumpMSEvent[vecMSEventDumpNames[i]].m_jet_iso);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_et_iso").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_et_iso);
      m_tree->Branch((vecMSEventDumpNames[i]+"_m_mass").c_str(),		mapDumpMSEvent[vecMSEventDumpNames[i]].m_mass);

    }
}
/***************************************************************************/ 
StatusCode MuonRecValidatorAOD::fillNtuple()
{

  // Part 1: Get the messaging service, print where you are
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::INFO << "execute()" << endreq;

  const EventInfo* pevt;
  if (m_storeGate->retrieve(pevt).isFailure())
    {
      mLog << MSG::FATAL << "Could not find event" << endreq;
      return StatusCode::FAILURE;
    }
  else
    {
      mLog << MSG::DEBUG << "MuonRecValidatorAOD :: Found EventInfo in SG" << endreq;
      return StatusCode::SUCCESS;
    }

  if (m_FirstEventNumber==(int)pevt->event_ID()->event_number())
    {
      std::cout<<"Version Switched ! "<<std::endl;
      m_VersionID=1;
    }
  if (EventNumber==0)
    {
      m_FirstEventNumber = pevt->event_ID()->event_number();
      std::cout<<"First Event Number set to be "<<m_FirstEventNumber<<std::endl;
    }
  
  msCurrectEvent.clear();
  msTrackObjectLoader.loadTruthInfo(msCurrectEvent);
  for (int i=0; i<(int)m_TrackContainerNames.size(); i++) msTrackObjectLoader.loadTrackInfo(m_TrackContainerNames[i], msCurrectEvent);
  for (int i=0; i<(int)m_MuonCollectionNames.size(); i++) msTrackObjectLoader.loadMuonCollectionInfo(m_MuonCollectionNames[i], msCurrectEvent);
  msTrackObjectLoader.loadMuonTriggerInfo(msCurrectEvent);

  std::cout<<"Event Number "<<EventNumber<<std::endl;

  if (m_EventDumping==true)
    {
      // Dumping the Event Info

      mapDumpMSEvent[vecMSEventDumpNames[0]];
      msCurrectEvent.convertParticles(mapDumpMSEvent[vecMSEventDumpNames[0]]);
      for (int i=1; i<(int)vecMSEventDumpNames.size();i++)
	msCurrectEvent.convertTracks(vecMSEventDumpNames[i], mapDumpMSEvent[vecMSEventDumpNames[i]]);
    }
  if (m_VersionComparing==false)
    {
      msMuonAnalysis.analyse(msCurrectEvent);
      if (m_AnalyseDiMuonFinalState) msDiMuonAnalysis.analyse(msCurrectEvent);
    } else
      {
	msMuonAnalysis.analyse_TwoVersions(msCurrectEvent,m_VersionID);
	if (m_AnalyseDiMuonFinalState) msDiMuonAnalysis.analyse(msCurrectEvent);
      }

  EventNumber++;

  return StatusCode::SUCCESS;
}
/***************************************************************************/ 
StatusCode MuonRecValidatorAOD::finalize()
{

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;
  std::cout<<"Number Of Events: "<<EventNumber<<std::endl;

  TDirectory *tDir = new TDirectory();

  std::string	name;
  name=m_Outputfile+":/";

  tDir->cd(name.c_str());
  msMuonAnalysis.writeResults();
  if (m_AnalyseDiMuonFinalState) msDiMuonAnalysis.writeResults();
  tDir->cd(name.c_str());
  delete tDir;
  return StatusCode::SUCCESS;
}
/***************************************************************************/ 

/// Clear - clear CBNT members
MuonRecValidatorAOD::~MuonRecValidatorAOD()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "MuonRecValidatorAOD destructor called" << endreq;
}
/***************************************************************************/ 
void MuonRecValidatorAOD::setRanges(vector<float> &vRanges_Eff, vector<float> &vRanges_Res)
{
  vRanges_Eff.clear();
  vRanges_Res.clear();
  for (int i=0; i<9; i++) vRanges_Eff.push_back(1.0);
  for (int i=0; i<9; i++) vRanges_Res.push_back(1.0);
  
  if ((int)m_EfficiencyBinning_vs_Pt.size()==3)
    {
      vRanges_Eff[0] = m_EfficiencyBinning_vs_Pt[0];
      vRanges_Eff[1] = m_EfficiencyBinning_vs_Pt[1];
      vRanges_Eff[2] = m_EfficiencyBinning_vs_Pt[2];
    }
  if ((int)m_EfficiencyBinning_vs_Eta.size()==3)
    {
      vRanges_Eff[3] = m_EfficiencyBinning_vs_Eta[0];
      vRanges_Eff[4] = m_EfficiencyBinning_vs_Eta[1];
      vRanges_Eff[5] = m_EfficiencyBinning_vs_Eta[2];
    }
  if ((int)m_EfficiencyBinning_vs_Phi.size()==3)
    {
      vRanges_Eff[6] = m_EfficiencyBinning_vs_Phi[0];
      vRanges_Eff[7] = m_EfficiencyBinning_vs_Phi[1];
      vRanges_Eff[8] = m_EfficiencyBinning_vs_Phi[2];
    }
  if ((int)m_ResolutionBinning_vs_Pt.size()==3)
    {
      vRanges_Res[0]  = m_ResolutionBinning_vs_Pt[0];
      vRanges_Res[1] = m_ResolutionBinning_vs_Pt[1];
      vRanges_Res[2] = m_ResolutionBinning_vs_Pt[2];
    }
  if ((int)m_ResolutionBinning_vs_Eta.size()==3)
    {
      vRanges_Res[3] = m_ResolutionBinning_vs_Eta[0];
      vRanges_Res[4] = m_ResolutionBinning_vs_Eta[1];
      vRanges_Res[5] = m_ResolutionBinning_vs_Eta[2];
    }
  if ((int)m_ResolutionBinning_vs_Phi.size()==3)
    {
      vRanges_Res[6] = m_ResolutionBinning_vs_Phi[0];
      vRanges_Res[7] = m_ResolutionBinning_vs_Phi[1];
      vRanges_Res[8] = m_ResolutionBinning_vs_Phi[2];
    }

}
/***************************************************************************/ 

 
