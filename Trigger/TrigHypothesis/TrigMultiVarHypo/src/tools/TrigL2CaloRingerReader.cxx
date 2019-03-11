/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include "TrigMultiVarHypo/tools/TrigL2CaloRingerReader.h"
#include "TTree.h"
//#include "TFile.h"
#include "TParameter.h"
#include <vector>
// Use this to find the absoluty path
#include "PathResolver/PathResolver.h"

TrigL2CaloRingerReader::TrigL2CaloRingerReader(const std::string& name) :
  asg::AsgMessaging(name),
  m_name(name)
{
  m_etaBins=nullptr;
  m_etBins=nullptr;
  m_thresholds=nullptr;
  m_weights=nullptr;
  m_bias=nullptr;
  // current version
  m_version = 3;
}


// ========================================================================

TrigL2CaloRingerReader::~TrigL2CaloRingerReader()
{;}

// ========================================================================
template <class T>
void TrigL2CaloRingerReader::InitBranch(TTree* fChain, std::string branch_name, T* param){
  bool message=true;
  std::string bname = branch_name;
  if (fChain->GetAlias(bname.c_str()))
     bname = std::string(fChain->GetAlias(bname.c_str()));

  if (!fChain->FindBranch(bname.c_str()) && message) {
    msg()  << MSG::WARNING << " unknown branch " << bname << endmsg;
    return; 
  }
  fChain->SetBranchStatus(bname.c_str(), 1.);
  fChain->SetBranchAddress(bname.c_str(), param);
}
// ========================================================================

bool TrigL2CaloRingerReader::retrieve( std::string &calibPath, std::vector<MultiLayerPerceptron*> &discriminators )
{
  m_etaBins=nullptr;
  m_etBins=nullptr;
  m_muBins=nullptr;
  m_weights=nullptr;
  m_bias=nullptr;
  m_nodes=nullptr;
  discriminators.clear();

  // Retrive the
  msg() << MSG::INFO << "Checking discriminators CalibPath: "<< calibPath<< endmsg;
  auto fullpath = PathResolverFindCalibFile(calibPath);
	TFile file(fullpath.c_str(),"READ");
  auto version = ((TParameter<int>*)file.Get("__version__"))->GetVal();
  TTree *t = (TTree*)file.Get("tuning/discriminators");

  if(version == 2){
	  // Link all branches
    InitBranch(t, "weights", &m_weights);
    InitBranch(t, "bias"   , &m_bias   );
    InitBranch(t, "nodes"  , &m_nodes  );
    InitBranch(t, "etaBin" , &m_etaBins );
    InitBranch(t, "etBin"  , &m_etBins  );
   
    for(Long64_t d=0; d<t->GetEntries();++d){
      t->GetEntry(d);
      // Create the discriminator object
      try{
	  	  discriminators.push_back(new MultiLayerPerceptron(*m_nodes,*m_weights,*m_bias,m_etBins->at(0),
              m_etBins->at(1),m_etaBins->at(0),m_etaBins->at(1), -999, 999) ); 
        //msg() << MSG::INFO << "Added new discriminator into the list." << endmsg;
	    }catch(const std::bad_alloc& xa){
        msg() << MSG::ERROR << "Can not alloc cutDefs on memory." << endmsg;
        return false;
      }
    }	// Loop over ttree events	
 
    // retrieve metadata
    m_useEtaVar = ((TParameter<bool>*)file.Get("metadata/UseEtaVar"))->GetVal();
    m_useLumiVar = ((TParameter<bool>*)file.Get("metadata/UseLumiVar"))->GetVal();
  

  // After version 2, the neural networks include mu bins
  }else if ( version == m_version ){ // version 3
 	  // Link all branches
    InitBranch(t, "weights", &m_weights);
    InitBranch(t, "bias"   , &m_bias   );
    InitBranch(t, "nodes"  , &m_nodes  );
    InitBranch(t, "etaBin" , &m_etaBins );
    InitBranch(t, "etBin"  , &m_etBins  );
    InitBranch(t, "muBin"  , &m_muBins  );
   
    for(Long64_t d=0; d<t->GetEntries();++d){
      t->GetEntry(d);
      // Create the discriminator object
      try{
	  	  discriminators.push_back(new MultiLayerPerceptron(*m_nodes,*m_weights,*m_bias,m_etBins->at(0),
              m_etBins->at(1),m_etaBins->at(0),m_etaBins->at(1), m_muBins->at(0), m_muBins->at(1) ) ); 
        //msg() << MSG::INFO << "Added new discriminator into the list." << endmsg;
	    }catch(const std::bad_alloc& xa){
        msg() << MSG::ERROR << "Can not alloc cutDefs on memory." << endmsg;
        return false;
      }
    }	// Loop over ttree events	
 
    // retrieve metadata
    m_useEtaVar = ((TParameter<bool>*)file.Get("metadata/UseEtaVar"))->GetVal();
    m_useLumiVar = ((TParameter<bool>*)file.Get("metadata/UseLumiVar"))->GetVal();

  }else{
    msg() << MSG::WARNING << "version not supported" << endmsg;
    return false;
  }

  msg() << MSG::INFO << "Config file version                         : " << version << endmsg;
  msg() << MSG::INFO << "Total of discriminators retrievied is       : " << discriminators.size() << endmsg;
  msg() << MSG::INFO << "Using eta variable                          : " << (m_useEtaVar?"Yes":"No") << endmsg;
  msg() << MSG::INFO << "Using lumi variable                         : " << (m_useLumiVar?"Yes":"No") << endmsg;
  file.Close();
  
  return true;
}

// ========================================================================

bool TrigL2CaloRingerReader::retrieve( std::string &calibPath, std::vector<TrigCaloRingsHelper::CutDefsHelper*> &cutDefs )
{
  m_etaBins=nullptr;
  m_etBins=nullptr;
  m_muBins=nullptr;
  m_thresholds=nullptr;

  cutDefs.clear();
  msg() << MSG::INFO <<"Checking thresholds CalibPath: "<< calibPath << endmsg;
 	auto fullpath = PathResolverFindCalibFile(calibPath);
	TFile file(fullpath.c_str(),"READ");
  auto version = ((TParameter<int>*)file.Get("__version__"))->GetVal();
  TTree *t = (TTree*)file.Get("tuning/thresholds");
	
  if(version == 2){  
    InitBranch(t, "thresholds", &m_thresholds);
    InitBranch(t, "etBin"     , &m_etBins  );
    InitBranch(t, "etaBin"    , &m_etaBins );
	  
	  for(Long64_t d=0; d<t->GetEntries();++d){
	    t->GetEntry(d);
      // Create the discriminator object
      try{
        cutDefs.push_back(new TrigCaloRingsHelper::CutDefsHelper(*m_thresholds,m_etaBins->at(0),
                                                    m_etaBins->at(1), m_etBins->at(0),m_etBins->at(1),-999 , 999));
        //msg() << MSG::INFO << "Added new cutDef into the list." << endmsg;
      }catch(const std::bad_alloc& xa){
        msg() << MSG::ERROR << "Can not alloc cutDefs on memory." << endmsg;
        return false;
      }
	  }	// Loop over ttree events	
    
    // retrieve metadata
    m_useNoActivationFunctionInTheLastLayer = ((TParameter<bool>*)file.Get("metadata/UseNoActivationFunctionInTheLastLayer"))->GetVal();
    m_lumiCut = ((TParameter<int>*)file.Get("metadata/LumiCut"))->GetVal();
    m_doPileupCorrection = ((TParameter<bool>*)file.Get("metadata/DoPileupCorrection"))->GetVal();
  
  }else if(version == m_version){ // version 3
  
    InitBranch(t, "thresholds", &m_thresholds);
    InitBranch(t, "etBin"     , &m_etBins  );
    InitBranch(t, "etaBin"    , &m_etaBins );
    InitBranch(t, "muBin"     , &m_muBins );
	  
	  for(Long64_t d=0; d<t->GetEntries();++d){
	    t->GetEntry(d);
      // Create the discriminator object
      try{
        cutDefs.push_back(new TrigCaloRingsHelper::CutDefsHelper(*m_thresholds,m_etaBins->at(0),
                                                    m_etaBins->at(1), m_etBins->at(0),m_etBins->at(1),
                                                    m_muBins->at(0), m_muBins->at(1)) );
        //msg() << MSG::INFO << "Added new cutDef into the list." << endmsg;
      }catch(const std::bad_alloc& xa){
        msg() << MSG::ERROR << "Can not alloc cutDefs on memory." << endmsg;
        return false;
      }
	  }	// Loop over ttree events	
    
    // retrieve metadata
    m_useNoActivationFunctionInTheLastLayer = ((TParameter<bool>*)file.Get("metadata/UseNoActivationFunctionInTheLastLayer"))->GetVal();
    m_lumiCut = ((TParameter<int>*)file.Get("metadata/LumiCut"))->GetVal();
    m_doPileupCorrection = ((TParameter<bool>*)file.Get("metadata/DoPileupCorrection"))->GetVal();
  
  }else{
    msg() << MSG::WARNING << "version not supported" << endmsg;
    return false;
  }
  
  file.Close();
  
  msg() << MSG::INFO << "Config file version                         : " << version << endmsg;
  msg() << MSG::INFO << "Total of cutDefs retrievied is              : " << cutDefs.size() << endmsg;
  msg() << MSG::INFO << "Using pileup correction                     : " << (m_doPileupCorrection?"Yes":"No") << endmsg;
  msg() << MSG::INFO << "Using useNoActivationFunctionInTheLastLayer : " << (m_useNoActivationFunctionInTheLastLayer?"Yes":"No") << endmsg;
  msg() << MSG::INFO << "Using lumi cut                              : " << (m_lumiCut) << endmsg;
  return true;
}





