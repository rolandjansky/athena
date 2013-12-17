/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ROOT
#include "TFile.h"
#include "TTree.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/RunModule.h"

// Local
#include "TrigCostExec/MakeExecs.h"
#include "TrigCostExec/CostService.h"

using namespace std;

//------------------------------------------------------------------------------------------
// return static instance
//------------------------------------------------------------------------------------------
Anp::CostService& Anp::CostService::Instance()
{
  static Anp::CostService svc;
  return svc;
}

//-----------------------------------------------------------------------------------------      
Anp::CostService::CostService()
  :fModule(new RunModule()),
   fFile(0),
   fTree(0),
   fCurrFileIndex(0),
   fCurrTreeIndex(0)
{
}

//-----------------------------------------------------------------------------------------      
Anp::CostService::~CostService()
{
  delete fModule;
}

//-----------------------------------------------------------------------------------------      
void Anp::CostService::Config(const string &config_path)
{ 
  //
  // Read registry at input path and configure RunModule
  //  
  Registry reg;
  reg.ReadXML(config_path);

  fModule->Config(reg);
  fModule->Init();
}

//-----------------------------------------------------------------------------------------      
bool Anp::CostService::LoadNextEvent()
{
  //
  // Load and process next TrigMonEvent
  //
  
  if(!fFile) {
    //
    // Open next file
    //
    if(fCurrFileIndex >= fModule->GetInputFiles().size()) {
      cerr << "CostService::LoadNextEvent - bad file index: " << fCurrFileIndex << endl;
      return false;
    }

    const string fpath = fModule->GetInputFiles().at(fCurrFileIndex);
    ++fCurrFileIndex;

    if(fCurrFileIndex >= fModule->GetInputFiles().size()) {
      fCurrFileIndex = 0;
    }

    fFile = TFile::Open(fpath.c_str(), "READ");
    if(!fFile || !fFile->IsOpen()) {
      fFile = 0;
      cerr << "CostService::LoadNextEvent - failed to open:\n" << "   " << fpath << endl;
      return false;
    }
    
    cout << "CostService::LoadNextEvent - opened new file:\n   " << "   " << fpath << endl;

    //
    // Read configuration tree
    //
    TTree *tree = dynamic_cast<TTree *>(fFile->Get("config"));
    if(tree) {
      NtupleSvc &svc_alg = NtupleSvc::Instance("config");
      AlgEvent *alg = fModule->GetAlg();

      svc_alg.Reset();
      svc_alg.Set(tree, Anp::NtupleSvc::READ);  

      for(long i = 0; i < tree->GetEntries(); ++i) {	
	tree -> GetEntry(i);
	svc_alg.Set(Anp::NtupleSvc::NEW_ENTRY);
	if(alg) alg->Exec();
      }
    }

    //
    // Get event tree
    //
    fTree = dynamic_cast<TTree *>(fFile->Get("event"));
    if(!fTree || fTree->GetEntries() < 1) {
      delete fFile;
      fFile = 0;
      fTree = 0;
      cerr << "CostService::LoadNextEvent - missing/bad event tree: " << fpath << endl;
      return false;
    }
    
    NtupleSvc::Instance("event").Reset();
    NtupleSvc::Instance("event").Set(fTree, Anp::NtupleSvc::READ);
  }

  //
  // Read next entry
  //
  NtupleSvc &svc = NtupleSvc::Instance("event");
  AlgEvent *alg = fModule->GetAlg();

  fTree -> GetEntry(fCurrTreeIndex);
  svc.Set(Anp::NtupleSvc::NEW_ENTRY);
  if(alg) alg->Exec();

  //
  // Increment and check tree index
  //
  ++fCurrTreeIndex;
  
  if(fCurrTreeIndex >= fTree->GetEntries()) {
    delete fFile;
    fFile = 0;
    fTree = 0;
    fCurrTreeIndex = 0;
  }
  
  return true;
}

//-----------------------------------------------------------------------------------------      
Anp::Exec::Event Anp::CostService::GetEvent(const std::string &alg_name) const
{
  //
  // Return handles to algorithm timing and ROB request data structures
  //
  Handle<MakeExecs> alg = Factory<MakeExecs>::Instance().Get(alg_name);

  if(!alg.valid()) {    
    cout << "CostService::GetEvent - failed to find algorithm: " << alg_name << endl;
    return Exec::Event();
  }
  
  return alg->GetEvent();
}

//-----------------------------------------------------------------------------------------      
std::vector<Anp::Ptr<Anp::Exec::AlgCall> > Anp::CostService::GetAlgCalls(const std::string &alg_name) const
{
  //
  // Return handles to algorithm timing and ROB request data structures
  //
  Handle<MakeExecs> alg = Factory<MakeExecs>::Instance().Get(alg_name);

  if(!alg.valid()) {    
    cout << "CostService::GetAlgCalls - failed to find algorithm: " << alg_name << endl;
    return std::vector<Anp::Ptr<Anp::Exec::AlgCall> >();
  }
  
  return alg->GetAlgCalls();
}
