/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// ROOT
#include "TRandom3.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostRate/RateChain.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostRate/SimlPS.h"

REGISTER_ANP_OBJECT(AlgEvent,SimlPS)

using namespace std;

//-----------------------------------------------------------------------------
Anp::SimlPS::SimlPS()
  :fRandom(0),
   fDebug(false),
   fIsConfig(false),
   fSeed(65539),
   fDecisionBitLV1(TrigMonL1Item::kL1AfterVeto),
   fDecisionBitHLT(TrigMonChain::kPassed)
{
}

//-----------------------------------------------------------------------------
Anp::SimlPS::~SimlPS()
{
}

//-----------------------------------------------------------------------------
void Anp::SimlPS::Config(const Registry &reg)
{
  //
  // Read configuration
  //
  reg.Get("SimlPS", "Debug",   fDebug);
  reg.Get("SimlPS::Seed",      fSeed);
  
  fRandom = new TRandom3(fSeed);
  
  string decisionKeyLV1, decisionKeyHLT;
  if(reg.Get("SimlPS", "DecisionKeyLV1", decisionKeyLV1)) {
    fDecisionBitLV1 = Anp::AsTriggerBitLV1(decisionKeyLV1);
  }
  if(reg.Get("SimlPS", "DecisionKeyHLT", decisionKeyHLT)) {
    fDecisionBitHLT = Anp::AsTriggerBitHLT(decisionKeyHLT);
  }

  //
  // Register for callback with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "SimlPS::ConfAlg", fConf, this);

  //
  // Register event tree branch
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");

  log() << "Config" << endl
	<< "   LV1 decision bit: " << Anp::AsString(fDecisionBitLV1) << endl
	<< "   HLT decision bit: " << Anp::AsString(fDecisionBitHLT) << endl;
}

//-----------------------------------------------------------------------------
void Anp::SimlPS::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback from configuration algorithm 
  //
  if(signal == "TRIG_CONFIG" || signal == "TRIG_CONFIG_DB") {
    if(!fConf.valid()) { 
      log() << "Signal - invalid configuration algorithm" << endl;
      return;
    }

    if(!fIsConfig) {
      Set(fConf->GetConfig());
    }
    else {
      fIsConfig = true;
    }
  }
}

//-----------------------------------------------------------------------------
bool Anp::SimlPS::Init()
{  
  return true;
}

//-----------------------------------------------------------------------------
void Anp::SimlPS::Exec()
{
  //
  // Read new event entry
  //
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {
    Run(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::SimlPS::Done()
{
}

//-----------------------------------------------------------------------------
void Anp::SimlPS::Run(TrigMonEvent &event)
{
  //
  // Process one event: apply PS and PT calculation
  //
  fPassPS.clear();
  fPassPT.clear();

  const std::vector<uint16_t> &lv1_vec = event.getL1Item();
  const std::vector<uint32_t> &hlt_vec = event.getChain();
 
  std::vector<uint16_t> lv1_new;
  std::vector<uint32_t> hlt_new;
 
  //
  // Precalculate coherent prescale acceptance
  //
  for (CpsMap::iterator cpsgrp = fCpsGroups.begin(); 
       cpsgrp != fCpsGroups.end(); cpsgrp++) 
    {	
      double ps_inv_last  = 1.0;
      bool   passed_last = true;
      for (std::vector< Data* >::iterator ch = cpsgrp->second->chains.begin();
	   ch != cpsgrp->second->chains.end(); ch++) {
	if ( passed_last &&
	     (fRandom->Uniform(1.0) < (*ch)->ps_inv/ps_inv_last) ) 
	  {
	    ps_inv_last = (*ch)->ps_inv;
	    (*ch)->cps_passed = true;
	  }
	else 
	  {
	    passed_last = false;
	    (*ch)->cps_passed = false;
	  }
      }
    }


  //
  // Apply PS to L1 items
  //
  for(unsigned i = 0; i < lv1_vec.size(); ++i) {
    const TrigMonL1Item item(lv1_vec.at(i));
    
    if(!item.isPassed(fDecisionBitLV1)) {
      continue;
    }

    const map<unsigned, Data>::const_iterator dit = fChains.find(item.getEncodedId());
    if(dit == fChains.end()) {
      continue;
    }
    
    const Data &data = dit->second;

    if(data.ps_inv > 0.0 && fRandom->Uniform(1.0) < data.ps_inv) {
      lv1_new.push_back(item.getEncoded());
      ComputeNextLevel(item.getEncodedId());
    }
  }
  
  //
  // Process L2 triggers
  //
  for(unsigned i = 0; i < hlt_vec.size(); ++i) {
    const TrigMonChain chain(hlt_vec.at(i));
    
    if(chain.getLevel() != 2 || !chain.isPassed(fDecisionBitHLT)) { 
      continue;
    }
    
    //
    // Check that input chain passed and that this chain passed PS computation
    //
    if(fPassPS.count(chain.getEncodedId())) {
      hlt_new.push_back(chain.getEncoded());
    }
  }
  
  //
  // Insert PT triggers for L2
  //
  InsertPT(hlt_new);

  //
  // Prepare PT and PS for EF
  //
  for(unsigned i = 0; i < hlt_new.size(); ++i) {
    ComputeNextLevel(TrigMonChain(hlt_new.at(i)).getEncodedId());
  }

  //
  // Process and include existing EF triggers
  //
  for(unsigned i = 0; i < hlt_vec.size(); ++i) {
    const TrigMonChain chain(hlt_vec.at(i));

    if(chain.getLevel() != 3 || !chain.isPassed(fDecisionBitHLT)) { 
      continue;
    }

    //
    // Check L2 result
    //
    if(fPassPS.count(chain.getEncodedId())) {
      hlt_new.push_back(chain.getEncoded());
    }
  }

  //
  // Insert PT triggers for EF
  //
  InsertPT(hlt_new);

  event.getL1Item() = lv1_new;
  event.getChain()  = hlt_new;
}

//-----------------------------------------------------------------------------
void Anp::SimlPS::ComputeNextLevel(unsigned id)
{
  //
  // Compute PS and PT decisions for chains seeded by 'id'
  //
  const map<unsigned, Data>::const_iterator dit = fChains.find(id);
  if(dit == fChains.end()) {
    return;
  }
    
  const Data &d = dit->second;

  //
  // Compute PS
  //
  for(set<unsigned>::const_iterator it = d.next_ids.begin(); it != d.next_ids.end(); ++it) {
    const map<unsigned, Data>::const_iterator nit = fChains.find(*it);
    if(nit == fChains.end()) {
      continue;
    }
    
    // next chain (seeded by 'id')
    const Data &n = nit->second;

    if (n.cps) 
      {
	if (n.cps_passed) 
	  {
	    fPassPS.insert(n.encoded);
	  }
      }
    else 
      {
	if(n.ps_inv > 0.0 && fRandom->Uniform(1.0) < n.ps_inv) {
	  fPassPS.insert(n.encoded);
	}
      }

    //
    // Forced accept chains which passsed PT at L2
    //
    if(fPassPT.count(id)) {
      fPassPT.insert(n.encoded);
    }
  }
  
  //
  // Compute PT
  //
  for(set<unsigned>::const_iterator it = d.pass_ids.begin(); it != d.pass_ids.end(); ++it) {
    const map<unsigned, Data>::const_iterator nit = fChains.find(*it);
    if(nit == fChains.end()) {
      continue;
    }
    
    const Data &n = nit->second;

    if(n.pt_inv > 0.0 && fRandom->Uniform(1.0) < n.pt_inv && fPassPS.count(n.encoded)) {
      fPassPT.insert(n.encoded);
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::SimlPS::InsertPT(std::vector<uint32_t> &hlt)
{
  //
  // Insert triggers which pass PT from previous level
  //

  for(std::set<unsigned>::const_iterator it = fPassPT.begin(); it != fPassPT.end(); ++it) {
    TrigMonChain chain(*it);
    chain.addDecision(fDecisionBitHLT);

    bool is_included = false;

    for(unsigned i = 0; i < hlt.size(); ++i) {
      TrigMonChain chain_hlt(hlt.at(i));
      is_included = is_included || chain_hlt.getEncodedId() == chain.getEncodedId();
    }
    
    if(!is_included) {
      hlt.push_back(chain.getEncoded());
    }
  }
}

//-----------------------------------------------------------------------------
// comparison function for sorting the chains in the cps group
static bool Cps_ps_less(const Anp::SimlPS::Data* ch1, const Anp::SimlPS::Data* ch2)
{
  return ch2->ps_inv < ch1->ps_inv;
}
  


//-----------------------------------------------------------------------------
void Anp::SimlPS::Set(const TrigMonConfig &config)
{
  //
  // Read and save prescales and passthroughs
  //

  log() << "Set - processing " << config.size<TrigConfChain>() << " chain(s)" << endl;

  for(unsigned i = 0; i < config.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = config.at<TrigConfChain>(i);    

    map<unsigned, Data>::iterator it = fChains.find(chain.getEncodedId());
    if(it != fChains.end()) {
      log() << "Config - duplicate chain: " << chain.getName() << endl;
      continue;
    }

    Data data;   
    data.encoded = chain.getEncodedId();
    
    if(!(chain.getPrescale() < 1.0)) {
      data.ps_inv = 1.0/chain.getPrescale();
    }
    if(!(chain.getPassThrough() < 1.0)) {
      data.pt_inv = 1.0/chain.getPassThrough();
    }

    fChains[data.encoded] = data;

    // check coherent prescale (CPS)
    const std::vector<std::string>& groups = chain.getGroup();
    for ( std::vector<std::string>::const_iterator group = groups.begin();
	  group != groups.end(); group++) {
      if (group->find("CPS:")!= string::npos) {
	// check if group already exists
	CpsMap::const_iterator cpsgroup = fCpsGroups.find(*group);
	if(cpsgroup == fCpsGroups.end()) {
	  cout << "SimlPS:Adding new CPS group " << *group << endl;
	  Handle<CpsGroup> cpshandle(new CpsGroup());	  
	  fCpsGroups[*group] = cpshandle;
	}

	fChains[data.encoded].cps = fCpsGroups[*group].get();
	fCpsGroups[*group]->chains.push_back(&(fChains[data.encoded]));
      }
    }
  }

  // Sort the chains in the CPS groups by prescale
  for (CpsMap::iterator cpsgrp = fCpsGroups.begin(); 
       cpsgrp != fCpsGroups.end(); cpsgrp++) 
    {	
      std::sort(cpsgrp->second->chains.begin(), cpsgrp->second->chains.end(), Cps_ps_less);
      
      cout << "SimlPS:Sort of " << cpsgrp->first << endl;
      for (std::vector< Data* >::iterator ch = cpsgrp->second->chains.begin();
	   ch != cpsgrp->second->chains.end(); ch++) {
	cout << "SimlPS:ps_inv " << (*ch)->ps_inv << endl;
      }
    }

  //
  // Fill L1->L2 and L2->EF seeds
  //
  for(unsigned i = 0; i < config.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = config.at<TrigConfChain>(i);    
    
    if(chain.getLevelId() == 2) {
      const std::vector<TrigConfChain> ichains = Anp::GetInput(chain, 
							       config.begin<TrigConfChain>(),
							       config.end<TrigConfChain>());
      if(ichains.size() != 1) {
	log() << "Set - bad L1 input for: " << chain.getName() << endl;
	continue;
      }

      const TrigConfChain &chainL1 = ichains.front();
    
      map<unsigned, Data>::iterator lit1 = fChains.find(chainL1.getEncodedId());
      if(lit1 == fChains.end()) {
	log() << "Config - missing L1 chain: " << chainL1.getName() << endl;
	continue;
      }
      else {
	lit1->second.next_ids.insert(chain.getEncodedId());
	
	if(!(chain.getPassThrough() < 1.0)) {
	  lit1->second.pass_ids.insert(chain.getEncodedId());
	}
      }
    }
    
    if(chain.getLevelId() == 3) {
      const std::vector<TrigConfChain> ichains = Anp::GetInput(chain, 
							       config.begin<TrigConfChain>(),
							       config.end<TrigConfChain>());
      if(ichains.size() != 1) {
	log() << "Set - bad L2 input for: " << chain.getName() << endl;
	continue;
      }

      const TrigConfChain &chainL2 = ichains.front();
      
      map<unsigned, Data>::iterator lit2 = fChains.find(chainL2.getEncodedId());
      if(lit2 == fChains.end()) {
	log() << "Config - missing L2 chain: " << chainL2.getName() << endl;
	continue;
      }
      
      lit2->second.next_ids.insert(chain.getEncodedId());
      
      if(!(chain.getPassThrough() < 1.0)) {
	lit2->second.pass_ids.insert(chain.getEncodedId());
      }
    }
  }
}
