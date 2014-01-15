/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <string>

// ROOT
#include "TFile.h"

// Framework
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostAthena/TrigNtAnpITool.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtAnpITool::TrigNtAnpITool(const std::string &name,
				     const std::string &type,
				     const IInterface  *parent)
  :AlgTool(name, type, parent),
   m_log(0),
   m_histSvc("THistSvc/THistSvc", name),
   m_file(0),
   m_treeConfig(0),
   m_treeEvent(0)
{
  declareInterface<Trig::ITrigNtTool>(this);

  declareProperty("algType",       m_algType       = "");
  declareProperty("algName",       m_algName       = "");
  declareProperty("outputFile",    m_outputFile    = "");
  declareProperty("registryXML",   m_registryXML   = "");

  declareProperty("streamConfig",  m_streamConfig  = "");
  declareProperty("streamEvent",   m_streamEvent   = "");
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtAnpITool::initialize()
{    
  //
  // Get services
  //
  m_log = new MsgStream(msgSvc(), name());  
  
  //
  // Save TTrees via THistSvc
  //
  if(m_histSvc.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve THistSvc" << endreq;
    return StatusCode::FAILURE;
  }
  
  //
  // Create output file
  //
  if(m_outputFile.size() > 4) {
    log() << MSG::INFO << "Creating output ROOT file: " << m_outputFile << endreq;
    
    TDirectory *dir = gDirectory;
    m_file = new TFile(m_outputFile.c_str(), "RECREATE");      
    gDirectory = dir;
  }

  //
  // Initialize Anp interface
  //
  if(!m_algType.empty()) {
    m_alg = Anp::Factory<Anp::AlgEvent>::Instance().Create(m_algType);
    
    if(!m_alg.valid()) {
      log() << MSG::INFO << "Failed to create Anp::AlgEvent: \"" << m_algType << "\"" << endreq;
    }
    else {
      log() << MSG::INFO << "Created Anp::AlgEvent: " << m_algType << endreq;         

      m_reg.ReadXML(m_registryXML);  
      
      if(outputLevel() <= MSG::DEBUG) {
	m_reg.Print();
      }
      
      m_alg->SetType(m_algType);
      m_alg->SetName(m_algName);
      m_alg->Config(m_reg);
      m_alg->Init();    
      m_alg->Save(m_file);
      
      log() << MSG::INFO 
	    << "outputFile  = " << m_outputFile  << endreq
	    << "registryXML = " << m_registryXML << endreq
	    << "algType     = " << m_algType     << endreq
	    << "algName     = " << m_algName     << endreq;
    }
  }

  //
  // Register output trees
  //
  m_treeConfig = MakeTree("config", m_streamConfig);
  m_treeEvent  = MakeTree("event",  m_streamEvent);
  
  Anp::NtupleSvc::Instance("config").Set(m_treeConfig, Anp::NtupleSvc::FILL);
  Anp::NtupleSvc::Instance("event") .Set(m_treeEvent,  Anp::NtupleSvc::FILL);

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtAnpITool::finalize()
{
  //
  // Clean up internal state
  //
  log() << MSG::DEBUG << "finalize()" << endreq;

  //
  // Finalize and release Anp interface
  //
  if(m_alg.valid()) {
    log() << MSG::DEBUG << "Releasing AlgEvent algorithm: " << m_alg->GetName() << endreq;
    m_alg->Done();
    m_alg.release();
  }

  if(m_file) {
    log() << MSG::DEBUG << "Writing out ROOT file" << endreq;
    
    TDirectory *dir = gDirectory;
    m_file -> Write();
    m_file -> Close();
    m_file = 0;
    gDirectory = dir;
  }

  delete m_log; m_log = 0;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtAnpITool::Fill(TrigMonConfig *confg)
{
  //
  // Copy configuration
  //
  if(!confg) {
    log() << MSG::WARNING << "Null TrigMonConfig pointer" << endreq;
    return false;
  }

  Anp::NtupleSvc &ntuple = Anp::NtupleSvc::Instance("config");
  ntuple.Set(Anp::NtupleSvc::NEW_ENTRY);
  
  if(m_alg.valid()) {     
    m_alg->Exec();
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtAnpITool::Fill(TrigMonEvent &)
{
  //
  // Process current event
  //  
  Anp::NtupleSvc &ntuple = Anp::NtupleSvc::Instance("event");
  ntuple.Set(Anp::NtupleSvc::NEW_ENTRY);
  
  if(m_alg.valid()) {
    m_alg->Exec();    
  }

  return true;
}

//---------------------------------------------------------------------------------------
TTree* Trig::TrigNtAnpITool::MakeTree(const std::string &tree_name, 
				      const std::string &stream_name)
{
  //
  // Make and register TTree via THistSvc
  //
  if(tree_name.empty() || stream_name.empty()) { 
    return 0;
  }

  const std::string tree_key = "/"+stream_name+"/"+tree_name;  
  TTree *tree = 0;

  if(m_histSvc->exists(tree_key) && m_histSvc->getTree(tree_key, tree).isSuccess()) {
    log() << MSG::INFO << "Retrieved TTree from HistSvc: " << tree_key << endreq;    
    return tree;
  }

  tree = new TTree(tree_name.c_str(), tree_name.c_str());
  tree -> SetDirectory(0);
  
  if(m_histSvc -> regTree(tree_key, tree).isFailure()) {
    log() << MSG::WARNING << "Could not register TTree " << tree_name << endreq;
    delete tree; tree = 0;
  }
  else {
    log() << MSG::INFO << "Registered new TTree " << tree_key << endreq;
  }

  return tree;
}
