/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigMenuNtupleAlg.cxx
*/
#include "TriggerMenuAnalysis/TrigMenuNtupleAlg.h"
#include "TriggerMenuAnalysis/TrigMenuNtComponent.h"
#include "TriggerMenuAnalysis/TrigMenuFlatNtComponent.h"
#include "TriggerMenuAnalysis/TrigDecNtComponent.h"
#include "TriggerMenuAnalysis/HltMuonNtComponent.h"
#include "TriggerMenuAnalysis/L1RoINtComponent.h"
#include "TriggerMenuAnalysis/HltJetNtComponent.h"
#include "TriggerMenuAnalysis/HltConfigNtComponent.h"
#include "TriggerMenuAnalysis/ESNtComponent.h"
#include "TriggerMenuNtuple/CombLinks.h"
#include "TrigMinBiasNtuple/TriggerDecisionNtComponent.h"
#include "TrigMinBiasNtuple/Lvl1ConfigNtComponent.h"
#include "TROOT.h"
#include "TDirectory.h"
#include "TFolder.h"
#include "TList.h"
#include "TObjString.h"
#include "TClonesArray.h"
//#include "Cintex/Cintex.h"

using namespace std;


TrigMenuNtupleAlg::TrigMenuNtupleAlg(const std::string& name, ISvcLocator* svcloc) : 
  NtupleAlgorithm(name, svcloc), 
  mActiveStoreSvc(0), 
  mTrigConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name), 
  mTrigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"), 
  mTrigAccessTool("TrigAccessTool/TrigAccessTool"), 
  mRoILinksCnvTool("RoILinksCnvTool/RoILinksCnvTool") {
  declareProperty("ntupleFile", m_ntupleFile="TrigMenu.root");
  declareProperty("ntupleDir", m_ntupleDir="Menu");
  declareProperty("ntupleTreeName", m_ntupleTreeName="TrigMenu");
  declareProperty("ntupleTreeTitle", m_ntupleTreeTitle="Trigger menu information");
  declareProperty("ntupleComponents", m_ntupleComponentCommands,
		  "A definition ntuple components and their parameters");
  declareProperty("TrigDecisionTool", mTrigDecisionTool, 
		  "Trig::TrigDecisionTool");
  declareProperty("TrigAccessTool", mTrigAccessTool, "TrigAccessTool");
  declareProperty("RoILinksCnvTool", mRoILinksCnvTool, "RoILinksCnvTool");
  m_availableComponents.push_back("HltMuon");
  m_availableComponents.push_back("TrigMenu");
  m_availableComponents.push_back("TrigMenuFlat");
  m_availableComponents.push_back("L1RoI");
  m_availableComponents.push_back("HltJet");
  m_availableComponents.push_back("TrigMuon");
  m_availableComponents.push_back("TileMu");
  m_availableComponents.push_back("EventInfo");
  m_availableComponents.push_back("TrigDec");
  m_availableComponents.push_back("ES");
  m_availableComponents.push_back("TriggerDecision");
  m_availableComponents.push_back("Lvl1Config"); // Once for run
  m_availableComponents.push_back("HltConfig"); // Once for run
}

TrigMenuNtupleAlg::~TrigMenuNtupleAlg() {
}

StatusCode TrigMenuNtupleAlg::initialize() {
  msg(MSG::INFO) << "entering intialize()" << endreq;

//   ROOT::Cintex::Cintex::Enable();
//   log << MSG::DEBUG << "after Cintex" << endreq;
//   System::ImageHandle handle = 0;
//   const std::string libname = "TriggerMenuNtupleDict";
//   if ( System::loadDynamicLib(libname, &handle) != 1) {
//     log << MSG::WARNING << "Cannot load " << libname << endreq;
//   } else {
//     log << MSG::INFO << "loaded : " << libname << endreq;
//   }

  if (serviceLocator()->service("ActiveStoreSvc", mActiveStoreSvc).isFailure()) {
    msg(MSG::WARNING) << "Cannot retrieve ActiveStoreSvc" << endreq;
  }
  if (mTrigAccessTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot retrieve TrigAccessTool" << endreq;
  }
  if (mRoILinksCnvTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot retrieve RoILinksCnvTool" << endreq;
  }

  // Needed by design of NtupleAlgorithm
  if(NtupleAlgorithm::parseNtupleComponentStrings().isFailure()) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't initialize NtupleAlgorithm base class." << endreq;
    return StatusCode::FAILURE;
  }

  std::vector<NtComponent::NtComponentParameters>::iterator para_itr;
  std::vector<NtComponent::NtComponentParameters>::iterator para_itr_end;
  para_itr = m_ntupleComponentParameters.begin();
  para_itr_end = m_ntupleComponentParameters.end();
  for(;para_itr!=para_itr_end;para_itr++) {
    msg(MSG::INFO) << "Initialize NtComponent: " << para_itr->name << endreq;
    if (para_itr->name == "HltMuon") {
      m_ntupleComponents.push_back(new HltMuonNtComponent(this, *para_itr));
    } else if (para_itr->name == "TrigMenu") {
      m_ntupleComponents.push_back(new TrigMenuNtComponent(this, *para_itr));
    } else if (para_itr->name == "TrigMenuFlat") {
      m_ntupleComponents.push_back(new TrigMenuFlatNtComponent(this, *para_itr));
    } else if (para_itr->name == "L1RoI") {
      m_ntupleComponents.push_back(new L1RoINtComponent(this, *para_itr));
    } else if (para_itr->name == "HltJet") {
      m_ntupleComponents.push_back(new HltJetNtComponent(this, *para_itr));
    } else if (para_itr->name == "TrigDec") {
      m_ntupleComponents.push_back(new TrigDecNtComponent(this, *para_itr));
    } else if (para_itr->name == "ES") {
      m_ntupleComponents.push_back(new ESNtComponent(this, *para_itr));
    } else if (para_itr->name == "Lvl1Config") {
      m_ntupleRunComponents.push_back(new NtComponent::Lvl1ConfigNtComponent(this, *para_itr));
    } else if (para_itr->name == "HltConfig") {
      m_ntupleRunComponents.push_back(new HltConfigNtComponent(this, *para_itr));
    } else if (para_itr->name == "TriggerDecision") {
      m_ntupleComponents.push_back(new NtComponent::TriggerDecisionNtComponent(this, *para_itr));
    } else {
      msg(MSG::WARNING) << "The NtComponent " << para_itr->name 
			<< " has not been implemented." << endreq;
    }
  }


  // Needed by design of NtupleAlgorithm
  if(NtupleAlgorithm::initialize().isFailure()) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't initialize NtupleAlgorithm base class." << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigMenuNtupleAlg::beginRun() {
  return mRoILinksCnvTool->beginRun();
}

StatusCode TrigMenuNtupleAlg::finalize() {
  TDirectory* dir = gDirectory;
  std::map<int, std::string>::const_iterator p;
  int i=0;
  std::string tname = "ChainFeatureMapping";
  std::string tmpname = name();

  const std::string prefix = "TrigMenuNtupleAlg";
  string::size_type itmp = tmpname.find(prefix);
  if (itmp != string::npos) {
    tname += tmpname.substr(prefix.size());
  }

  tree()->GetDirectory()->cd();

  TTree *t = new TTree(tname.c_str(), 
		       "Chain & feature mapping saved as TTree");
  TClonesArray* c1 = new TClonesArray("TObjString", 500);
  TClonesArray* c2 = new TClonesArray("TObjString", 1000);

  t->Branch("Chain", "TClonesArray", &c1);
  t->Branch("Feature", "TClonesArray", &c2);

  msg(MSG::INFO) << "ChainEntry::chainIdMap(): " << endreq;
  const std::map<int, std::string>& m1 = ChainEntry::chainIdMap();
  for (i=0,p=m1.begin(); p!=m1.end(); ++p,++i) {
    msg(MSG::INFO) << "  Chain: " << p->first << ": " << p->second
		   << endreq;
    new ( (*c1)[i]) TObjString(p->second.c_str());
  }

  msg(MSG::INFO) << "CombLinks::featureIdMap(): " << endreq;
  const std::map<int, std::string>& m = CombLinks::featureIdMap();
  for (i=0,p=m.begin(); p!=m.end(); ++p,++i) {
    msg(MSG::INFO) << "  Feature: " << p->first << ": " << p->second
		   << endreq;
    new ( (*c2)[i]) TObjString(p->second.c_str());
  }

  t->Fill();
  t->Write();

  dir->cd();

  return StatusCode::SUCCESS;
}

ActiveStoreSvc* TrigMenuNtupleAlg::activeStoreSvc() {
  return mActiveStoreSvc;
}

TrigConf::ITrigConfigSvc* TrigMenuNtupleAlg::trigConfigSvc() {
  return &(*mTrigConfigSvc);
}

Trig::TrigDecisionTool* TrigMenuNtupleAlg::trigDecisionTool() {
  return &(*mTrigDecisionTool);
}

TrigAccessTool* TrigMenuNtupleAlg::trigAccessTool() {
  return &(*mTrigAccessTool);
}

RoILinksCnvTool* TrigMenuNtupleAlg::roILinksCnvTool() {
  return &(*mRoILinksCnvTool);
}

