/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNtupleBase/NtupleAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"

//---------------------------------------------------------------------

NtupleAlgorithm::NtupleAlgorithm(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm (name, pSvcLocator),
  m_tree(nullptr),
  m_treeRun(nullptr),
  m_activeStore("ActiveStoreSvc", name),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool", this)
{
  // Ntuple properties
  declareProperty("ntupleFile", m_ntupleFile = "trigmb");
  declareProperty("ntupleDir", m_ntupleDir = "MinBias");
  declareProperty("ntupleTreeName", m_ntupleTreeName = "TrigMinBias");
  declareProperty("ntupleTreeTitle", m_ntupleTreeTitle = "Minbias Trigger Validation");
  declareProperty("ntupleComponents", m_ntupleComponentCommands, "A definition ntuple components and their parameters");
}

//---------------------------------------------------------------------

StatusCode NtupleAlgorithm::initialize(){
  StatusCode sc;

  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "entering intialize()" << endreq;
  
  // retrive pointer to THistSvc
  ITHistSvc *tHistSvc;
  sc =  service("THistSvc", tHistSvc);
  if (sc.isFailure() || !tHistSvc) {
    msg(MSG::ERROR) << "Unable to locate Service THistSvc" << endreq;
    return sc;
  }

  // Create TTree and register it in the THistSvc.  The TTree object
  // must not be deleted such that it can be writen into the TFile.
  m_tree = new TTree(m_ntupleTreeName.c_str(), m_ntupleTreeTitle.c_str());
  
  std::string fullNtupleName =  "/"+m_ntupleFile+"/"+m_ntupleDir+"/"+m_ntupleTreeName;
  sc = tHistSvc->regTree(fullNtupleName, m_tree);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endreq;
    return sc;
  }

  // Initialise all ntuple components
  m_itrNtComp = m_ntupleComponents.begin();
  m_itrNtCompEnd = m_ntupleComponents.end();
  for(;m_itrNtComp!=m_itrNtCompEnd; ++m_itrNtComp) {
    if((sc = (*m_itrNtComp)->setupServices()).isFailure()) return sc;
    if((sc = (*m_itrNtComp)->book()).isFailure()) return sc;
  }

  // Setup the run-wise TTree
  if (m_ntupleRunComponents.size() > 0) {
    m_treeRun = new TTree((m_ntupleTreeName+"RunInfo").c_str(), 
			  (m_ntupleTreeTitle+" (For each run)").c_str());
    fullNtupleName =  "/"+m_ntupleFile+"/"+m_ntupleDir+"/"+m_ntupleTreeName+"RunInfo";
    sc = tHistSvc->regTree(fullNtupleName, m_treeRun);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endreq;
      return sc;
    }

    // Initialise all ntuple components
    m_itrNtComp = m_ntupleRunComponents.begin();
    m_itrNtCompEnd = m_ntupleRunComponents.end();
    for(;m_itrNtComp!=m_itrNtCompEnd; ++m_itrNtComp) {
      (*m_itrNtComp)->setIsRunInfoComponent(true);
      if((sc = (*m_itrNtComp)->setupServices()).isFailure()) return sc;
      if((sc = (*m_itrNtComp)->book()).isFailure()) return sc;
    }
  }

  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "leaving intialize()" << endreq;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------

StatusCode NtupleAlgorithm::execute() {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "entering execute()" << endreq;

  // Fill ntuple components
  m_itrNtComp = m_ntupleComponents.begin();
  m_itrNtCompEnd = m_ntupleComponents.end();
  for(;m_itrNtComp!=m_itrNtCompEnd; ++m_itrNtComp) {
     StatusCode sc = (*m_itrNtComp)->fill();
     if(sc.isFailure()) return sc;
  }
  
  // Write the record
  m_tree->Fill();

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "leaving execute()" << endreq;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------

StatusCode  NtupleAlgorithm::finalize() {
  msg(MSG::INFO) << "entering finalize()" << endreq;

  // Delete ntuple components
  m_itrNtComp = m_ntupleComponents.begin();
  m_itrNtCompEnd = m_ntupleComponents.end();
  for(;m_itrNtComp!=m_itrNtCompEnd; ++m_itrNtComp) {
    delete (*m_itrNtComp);
  }

  msg(MSG::INFO) << "leaving finalize()" << endreq;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------

StatusCode NtupleAlgorithm::parseNtupleComponentStrings() {
  NtComponent::NtComponentParameters ntComponentParameters;

  std::string tmpString, before, after;
  size_t stringLength, i, pos;
  std::vector<std::string>::iterator itr;
  std::vector<std::string>::iterator itr_end;
  std::vector<std::string>::iterator allowed_itr_begin;
  std::vector<std::string>::iterator allowed_itr_end;

  allowed_itr_begin = m_availableComponents.begin();
  allowed_itr_end = m_availableComponents.end();

  // Clear the ntComponentParameters object
  ntComponentParameters.name.clear();
  ntComponentParameters.containerKeys.clear();
  ntComponentParameters.options.clear();

  // Loop over all vector elements where one element defines one
  // ntuple component.
  itr = m_ntupleComponentCommands.begin();
  itr_end = m_ntupleComponentCommands.end();
  for(;itr!=itr_end;++itr) {
    
    // Push back the last set of parameters if the name of the
    // NtComponent class is present.
    if(itr!=m_ntupleComponentCommands.begin()) {
      if(ntComponentParameters.name.length()>0) {
        m_ntupleComponentParameters.push_back(ntComponentParameters);
      }
      else {
        msg(MSG::WARNING) << "No NtComponent class name given in string." << endreq;
      }

      // Clear the ntComponentParameters object
      ntComponentParameters.name.clear();
      ntComponentParameters.containerKeys.clear();
      ntComponentParameters.options.clear();
    }
    
    stringLength = (*itr).length();
    
    i=0;
    while(i<stringLength) {
      tmpString.clear();
      
      // Copy all non-white space characters until a ',' is found.
      while((*itr)[i] != ',' && i<stringLength) {
        if((*itr)[i] != ' ') {
          tmpString.push_back((*itr)[i]);
        }
        i++;
      }

      // Check if this is an NtComponent name and therefore does not
      // contain ':'
      if((pos=tmpString.find(':')) == std::string::npos) {
	
        // Check if this is an allowed NtComponent class.
        if(find(allowed_itr_begin,allowed_itr_end,tmpString) == allowed_itr_end) {
          msg(MSG::WARNING) << tmpString << " is not an available ntuple component." << endreq;
          i = stringLength; // Break out of loop.
          continue;
        }
	
        // Copy the name.
        ntComponentParameters.name = tmpString;
      }
      else {
        // Split the string keeping before ':' and after. 
        before = tmpString.substr(0,pos);
        after = tmpString.substr(pos+1,std::string::npos);

        if(before == "opt") {
          ntComponentParameters.options.push_back(after);
        }
        else {
          ntComponentParameters.containerKeys.insert(std::make_pair(before,after));
        }
      }
      
      i++;
    }
  }
  
  if(ntComponentParameters.name.length()>0) {
    m_ntupleComponentParameters.push_back(ntComponentParameters);
  }

  // Print out the settings if the message level is low enough.
  if (msgLvl(MSG::INFO)) {
    std::vector<NtComponent::NtComponentParameters>::iterator para_itr;
    std::vector<NtComponent::NtComponentParameters>::iterator para_itr_end;
    std::map<std::string,std::string>::iterator map_str_itr;
    std::vector<std::string>::iterator vec_str_itr;

    para_itr = m_ntupleComponentParameters.begin();
    para_itr_end = m_ntupleComponentParameters.end();

    msg(MSG::INFO) << "===================================================== " << endreq;
    msg(MSG::INFO) << " Activated NtComponent classes and settings " << endreq;
    msg(MSG::INFO) << "----------------------------------------------------- " << endreq;
    
    for(;para_itr!=para_itr_end;para_itr++) {
      msg(MSG::INFO)  << (*para_itr).name << "NtComponent";

      for(map_str_itr = (*para_itr).containerKeys.begin();
          map_str_itr != (*para_itr).containerKeys.end();
          map_str_itr++) {
        msg(MSG::INFO) << ", class=" << (*map_str_itr).first << ",key=\"" << (*map_str_itr).second << "\"";
      }
      for(vec_str_itr = (*para_itr).options.begin();
          vec_str_itr != (*para_itr).options.end();
          vec_str_itr++) {
        msg(MSG::INFO) << ", opt:" << (*vec_str_itr);
      }
      msg(MSG::INFO) << endreq;
    }
    
    msg(MSG::INFO) << "===================================================== " << endreq;
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------

StatusCode NtupleAlgorithm::beginRun(){
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "entering beginRun()" << endreq;

  // Fill ntuple components for the RunInfo  TTree
  std::vector<NtComponent::NtupleComponent *>::iterator itrNtComp = m_ntupleRunComponents.begin();
  for(;itrNtComp!=m_ntupleRunComponents.end(); ++itrNtComp) {
     StatusCode sc = (*itrNtComp)->fill();
     if(sc.isFailure()) return sc;
  }

  // Write the record
  if (m_ntupleRunComponents.size() > 0)
    m_treeRun->Fill();

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "leaving beginRun()" << endreq;
  return StatusCode::SUCCESS;

}

//---------------------------------------------------------------------

