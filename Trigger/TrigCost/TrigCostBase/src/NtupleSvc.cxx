/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Local
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/NtupleSvc.h"

//--------------------------------------------------------------------------------------
Anp::NtupleSvc& Anp::NtupleSvc::Instance(const std::string &tree_name)
{  
  //
  // Create and return NtupleSvc singleton
  //
  static std::map<std::string, Anp::NtupleSvc *> instance;

  std::map<std::string, Anp::NtupleSvc *>::iterator it = instance.find(tree_name);
  if(it == instance.end()) {
    it = instance.insert(std::map<std::string, Anp::NtupleSvc *>::value_type(tree_name, new Anp::NtupleSvc())).first;
  }

  return *(it->second);
}

using namespace std;

//--------------------------------------------------------------------------------------
Anp::NtupleSvc::NtupleSvc()
  :fTree(0),
   fMode(NONE),
   fBranchStatusOff(false),
   fDebug(false)
{
}

//--------------------------------------------------------------------------------------
bool Anp::NtupleSvc::Set(TTree *tree, Mode mode)
{
  //
  // Set branches to new Tree
  //
  if(!tree) { 
    cerr << "NtupleSvc::Set - null input TTree pointer" << endl;
    return false;
  }
  if(fTree) { 
    cerr << "NtupleSvc::Set - TTree pointer is already set" << endl;
    return false;
  }
  
  const bool debug = false;
  if(debug) cout << "NtupleSvc::Set - tree, mode=" << tree << ", " << mode << endl;

  if(mode != READ && mode != FILL) {
    cerr << "NtupleSvc::Set - invalid mode option" << endl;    
    return false;
  }

  if(fMode == NONE) { 
    fMode = mode;
    if(debug) cout << "NtupleSvc::Set - save mode: " << mode << endl;
  }
  else if(fMode != mode) {
    cerr << "NtupleSvc::Set - mode mismatch: " << mode << "!=" << fMode << endl;
    return false;
  }
    
  // Store TTree pointer and mode    
  fTree = tree;

  if(fMode == READ && fBranchStatusOff) {
    fTree->SetBranchStatus("*", 0);
  }

  //
  // Set all existing branches
  //
  for(BranchMap::iterator bit = fBranchMap.begin(); bit != fBranchMap.end(); ++bit) {
    NtupleBase *h = bit -> second;
    if(!h) continue;
    
    if(debug) cout << "NtupleSvc::Set - loop at tree=\"" << fTree->GetName() << "\"" 
		   << "branch=\"" << bit->first << "\"" << endl;

    //
    // Attempt to set branch for read/write
    //
    if(!(h->Branch(fTree, bit->first, fMode == FILL))) {
      cerr << "NtupleSvc::Set - bad new branch for " << tree->GetName() 
	   << " for branch \"" << bit->first << "\" with class \"" << h->GetType() << "\"" << endl;
    }
  }
  
  return true;
}

//--------------------------------------------------------------------------------------
bool Anp::NtupleSvc::Set(Mode mode)
{
  //
  // Set branch status
  //
  if(mode != NEW_ENTRY) {
    cerr << "NtupleSvc::Set - invalid mode option" << endl;    
    return false;
  }

  //
  // First, reset all existing branches
  //
  for(BranchMap::iterator bit = fBranchMap.begin(); bit != fBranchMap.end(); ++bit) {
    NtupleBase *h = bit -> second;
    if(h) {
      h -> SetEvent(Ntuple::kPASS);
      h -> SetState(Ntuple::kREAD);
    }
  }
  
  return true;
}

//--------------------------------------------------------------------------------------
bool Anp::NtupleSvc::Reset()
{
  //
  // Reset tree
  //
  fTree = 0;
  fMode = NONE;

  // Reset branches
  for(BranchMap::iterator bit = fBranchMap.begin(); bit != fBranchMap.end(); ++bit) {
    NtupleBase *h = bit -> second;
    
    //
    // This tree is no longer valid - inform branches
    //
    if(h) {	
      h -> Branch(0, bit->first, false);
      h -> SetEvent(Ntuple::kNONE);
    }
  }

  return true;
}

//--------------------------------------------------------------------------------------
void Anp::NtupleSvc::CopyTo(NtupleSvc &rhs)
{
  //
  // Copy branches
  //
  rhs.Reset();

  for(BranchMap::iterator bit = fBranchMap.begin(); bit != fBranchMap.end(); ++bit) {
    NtupleBase *b = bit -> second;

    BranchMap::iterator rit = rhs.fBranchMap.find(bit->first);
    if(rit == rhs.fBranchMap.end()) {
      rit = rhs.fBranchMap.insert(BranchMap::value_type(bit->first, b->Clone())).first;
    }

    NtupleBase *r = rit -> second;
    r -> Copy(b);
  }
}

//--------------------------------------------------------------------------------------
void Anp::NtupleSvc::Print() const
{
  //
  // Copy branches
  //
  cout << "NtupleSvc::Print - " << fBranchMap.size() << " branches:" << endl;

  for(BranchMap::const_iterator bit = fBranchMap.begin(); bit != fBranchMap.end(); ++bit) {
    cout << "   " << bit->first << endl;
  }
}

//--------------------------------------------------------------------------------------
void Anp::NtupleSvc::Config(const Registry &reg)
{
  reg.Get("NtupleSvc", "Debug",         fDebug);
  reg.Get("NtupleSvc::BranchStatusOff", fBranchStatusOff);
}
