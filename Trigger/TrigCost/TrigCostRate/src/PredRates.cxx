/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/UtilHack.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostRate/PredRates.h"

REGISTER_ANP_OBJECT(AlgEvent,PredRates)

using namespace std;

//-----------------------------------------------------------------------------
namespace Anp
{
  void ExpandVector(std::vector<PredRates::Chains> &vec, const PredRates::Chains &c) 
  {
    //
    // Expand vector using chain counter as index and insert pointer if requested
    //
    if(!c.chainE) return;
    
    const unsigned counter = c.chainE->GetCounter();
    
    if(counter >= vec.size()) {
      vec.insert(vec.end(), 1+counter-vec.size(), PredRates::Chains());
    }
    
    vec.at(counter) = c;
  }
}

//-----------------------------------------------------------------------------
Anp::PredRates::PredRates()
  :fDebug(false),
   fPrint(false),
   fIsConfig(false),
   fTotal(0.0),
   fScale(0.0),
   fCountPass(0),
   fCountFail(0),
   fCountPTChange(false),
   fDecisionBitLV1(TrigMonL1Item::kL1AfterVeto),
   fDecisionBitHLT(TrigMonChain::kPassed)
{
}

//-----------------------------------------------------------------------------
Anp::PredRates::~PredRates()
{
}

//-----------------------------------------------------------------------------
void Anp::PredRates::ConfigReg(const Registry &reg)
{
  fReg = reg;

  reg.Get("PredRates", "Debug",   fDebug);
  reg.Get("PredRates", "Print",   fPrint);

  reg.Get("PredRates::RatesName1", fName1);
  reg.Get("PredRates::RatesName2", fName2);

  //
  // Book tree branches before Init()
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");

  //
  // Register self for clients
  //
  Factory<RateABC>::Instance().Hold(AlgEvent::GetName(), Handle<RateABC>(this));
}

//-----------------------------------------------------------------------------
bool Anp::PredRates::Init()
{ 
  return true;
}

//-----------------------------------------------------------------------------
void Anp::PredRates::Exec()
{
  //
  // Check that valid event is available - ensures that input algorithms are configured
  //
  if(!fEntry.valid() ||
     fEntry.GetState() != Ntuple::kREAD ||
     fEntry.GetEvent() != Ntuple::kPASS) {
    return;
  }

  //
  // Compute predicted rates
  //  
  ProcessConfig();

  if(!fRate1.valid() || !fRate2.valid()) {
    return;
  }

  ++fCountPass;

  //
  // Compute chains predicted rates
  //  - use chains activated for current event
  //  - use chains activated for all events
  //  - UPDATE: MakeRates no longer provides list of active and Always chains
  // 
  //Compute(fRate1->GetActiveChains(), fRate2->GetActiveChains());
  //Compute(fRate1->GetAlwaysChains(), fRate2->GetAlwaysChains());

  //
  // Compute group predicted rates
  //
  RateData data;

  for(unsigned i = 0; i < fLookupGroups.size(); ++i) {
    Groups &g = fLookupGroups.at(i);
    
    if(!g.group1 || !g.group2 || !g.groupE) {
      log() << "Exec - null RateGroup pointer(s)" << endl;
      continue;
    }
    
    data.event_weight = 0.0;
    data.event_number = fCountPass;

    const double w1 = g.group1->GetWeight();
    const double w2 = g.group2->GetWeight();
    
    if(w1 > 0.0) {
      data.event_weight = w2/w1;
    }
    else {
      data.event_weight = w2;
    
      if(w2 > 0.0) {
	log() << "Exec - group logic error: " << g.groupE->GetName() << endl
	      << "   w1=" << w1 << ", w2=" << w2 << endl;
	
	const vector<Handle<RateChain> > &chains = g.group2->GetChains();
	for(unsigned i = 0; i < chains.size(); ++i) {
	  cout << "   isPT=" << chains.at(i)->IsPT() << endl;
	}
      }
    }
    
    g.groupE->PredRates(data);
  }
}

//-----------------------------------------------------------------------------
void Anp::PredRates::Done()
{
  //
  // Finalize rate objects
  //
  if(!fRate1.valid() || !fRate2.valid()) {
    return;
  }

  if(std::fabs(fRate1->GetScale()-fRate2->GetScale()) > 0.0) {
    log() << "Done - different scale factor for 2 rates algorithms" << endl;
  }
  if(std::fabs(fRate1->GetTotal()-fRate2->GetTotal()) > 0.0) {
    log() << "Done - different total factor for 2 rates algorithms" << endl;
  }

  fScale = fRate2->GetScale();
  fTotal = fRate2->GetTotal();
  
  log() << "Done - rates prediction summary" << endl
	<< "   total = " << fTotal << endl
	<< "   scale = " << fScale << endl;

  Registry reg;
  reg.Set("RateChain::CommonScale", fScale);
  reg.Set("RateChain::TotalEvents", fTotal);

  reg.Set("RateGroup::CommonScale", fScale);
  reg.Set("RateGroup::TotalEvents", fTotal);

  for(unsigned i = 0; i < fChain.size(); ++i) {
    fChain.at(i)->UpdateReg(reg);
  }
  for(unsigned i = 0; i < fLookupGroups.size(); ++i) {
    fLookupGroups.at(i).groupE->UpdateReg(reg);
  }
}

//-----------------------------------------------------------------------------
void Anp::PredRates::ProcessConfig()
{
  //
  // Process cinfiguration event once
  //
  if(fIsConfig) { 
    return;
  }
  else {
    fIsConfig = true;
  }

  log() << "ProcessConfig - extracting algorithms and doing configuration..." << endl;

  //
  // Extract algroithms and configure self
  //
  fRate1 = Factory<RateABC>::Instance().Get(fName1);
  fRate2 = Factory<RateABC>::Instance().Get(fName2);
  
  if(!fRate1.valid()) {
    log() << "ProcessConfig - failed to get alg 1: " << fName1 << endl;
    return;
  }
  if(!fRate2.valid()) {
    log() << "ProcessConfig - failed to get alg 2: " << fName2 << endl;
    return;
  }
  
  //
  // Make chains and groups for rate extrapolation 
  //
  MakeChains(fRate1->GetRateChains(), fRate2->GetRateChains());
  MakeGroups(fRate1->GetRateGroups(), fRate2->GetRateGroups(), "group");
  MakeGroups(fRate1->GetRateLevels(), fRate2->GetRateLevels(), "level");
  MakeGroups(fRate1->GetRateStream(), fRate2->GetRateStream(), "stream");
  
  if(fRate1->GetDecisionBitLV1() != fRate2->GetDecisionBitLV1()) {
    log() << "ProcessConfig - LV1 bit mismatch" << endl;
  }
  if(fRate1->GetDecisionBitHLT() != fRate2->GetDecisionBitHLT()) {
    log() << "ProcessConfig - HLT bit mismatch" << endl;
  }

  fDecisionBitLV1 = fRate2->GetDecisionBitLV1();
  fDecisionBitHLT = fRate2->GetDecisionBitHLT();
  
  const vector<Handle<RateChain> > &chains1 = fRate1->GetRateChains();
  const vector<Handle<RateChain> > &chains2 = fRate2->GetRateChains();

  for(unsigned i = 0; i < chains1.size(); ++i) {
    const TrigConfChain &chain1 = chains1.at(i)->GetChain();
    
    const vector<Handle<RateChain> >::const_iterator cit = 
      std::find(chains2.begin(), chains2.end(), chain1.getName());
    if(cit == chains2.end()) {
      log() << "ProcessConfig - failed to find chain: " << chain1.getName() << endl;
      continue;
    }
    const TrigConfChain &chain2 = (*cit)->GetChain();

    if(std::fabs(chain1.getPassThrough()-chain2.getPassThrough()) > 0.0) {
      ++fCountPTChange;
    }
  }

  log() << "ProcessConfig - configuration summary:" << endl
	<< "   algorithm 1:  " << fName1         << endl
	<< "   algorithm 2:  " << fName2         << endl
	<< "   PT changes:   " << fCountPTChange << endl
	<< "   # of chains:  " << fChain.size()  << endl
	<< "   # of groups:  " << fGroup.size()  << endl
	<< "   # of levels:  " << fLevel.size()  << endl
	<< "   # of streams: " << fStream.size() << endl;
}

//-----------------------------------------------------------------------------
void Anp::PredRates::Compute(const vector<RateChain *> &act1,
			     const vector<RateChain *> &act2)
{
  //
  // Compute rate as ratio: extrapolated weight (2)/actual weight (1)
  //
  RateData data;

  for(unsigned i = 0; i < act1.size(); ++i) {
    const RateChain *r1 = act1.at(i);    
    const RateChain *r2 = 0;

    //
    // Reset event weight 
    //
    data.event_weight = 0.0;
    data.event_number = fCountPass;

    //
    // Find event weight for extrapolated/predicted chain
    // 
    for(unsigned j = 0; j < act2.size(); ++j) {
      RateChain *rt = act2.at(j);

      if(r1->GetChain().getEncodedId() == rt->GetChain().getEncodedId()) {
	r2 = rt;
	break;
      }
    }
    
    if(r2) {
      const double w1 = r1->GetWeight();
      const double w2 = r2->GetWeight();

      if(w1 > 0.0) {
	data.event_weight = w2/w1;
      }
      else {
	if(w2 > 0.0) {
	  cerr << "PredRates::Compute - logic error: " << r1->GetName() << endl;
	}
      }
    }

    const unsigned level   = r1->GetLevel();
    const unsigned counter = r1->GetCounter();

    //
    // Find chain for rate count
    //
    RateChain *rate = 0;
    
    if     (level == 1 && counter < fLookupL1.size()) rate = fLookupL1.at(counter).chainE;
    else if(level == 2 && counter < fLookupL2.size()) rate = fLookupL2.at(counter).chainE;
    else if(level == 3 && counter < fLookupEF.size()) rate = fLookupEF.at(counter).chainE;
    
    if(rate) {
      rate -> PredRates(data);
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::PredRates::MakeChains(const ChainVec &vec1, const ChainVec &vec2)
{
  //
  // Check two lists of chains for consistency
  //

  for(unsigned i = 0; i < vec2.size(); ++i) {
    Handle<RateChain> r1;
    Handle<RateChain> r2 = vec2.at(i);

    if(!r2.valid()) {
      log() << "MakeChains - invalid chain handle" << endl;
      continue;
    }   
    
    //
    // Check that no new chains is introduced
    //
    ChainVec::const_iterator rit1 = std::find(vec1.begin(), vec1.end(), r2->GetName());
    if(rit1 == vec1.end()) {
      log() << "MakeChains - no matching chain 1 for: " << r2->GetName() << endl;
    }
    else {
      r1 = *rit1;
    }
    
    if(r1.valid() && r2.valid()) {
      Handle<RateChain> rate = MakeChain(r1, r2);
      fChain.push_back(rate);

      if     (rate->GetLevel() == 1) ExpandVector(fLookupL1, Chains(r1.get(), r2.get(), rate.get()));
      else if(rate->GetLevel() == 2) ExpandVector(fLookupL2, Chains(r1.get(), r2.get(), rate.get()));
      else if(rate->GetLevel() == 3) ExpandVector(fLookupEF, Chains(r1.get(), r2.get(), rate.get()));
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::PredRates::MakeGroups(const GroupVec &vec1, const GroupVec &vec2, const string &key)
{
  //
  // Check two lists of chains for consistency
  //

  for(unsigned i = 0; i < vec2.size(); ++i) {
    if(!vec2.at(i).valid()) {
      log() << "MakeGroups - invalid group handle" << endl;
      continue;
    }
    
    Handle<RateGroup> r1;
    Handle<RateGroup> r2 = vec2.at(i);
    
    //
    // Check that no new chains is introduced
    //
    GroupVec::const_iterator rit1 = std::find(vec1.begin(), vec1.end(), r2->GetName());
    if(rit1 == vec1.end()) {
      log() << "MakeGroups - no matching group 1 for: " << r2->GetName() << endl;
      continue;
    }
    else {
      r1 = *rit1;
    }
    
    if(r1.valid() && r2.valid()) {
      Handle<RateGroup> rate = MakeGroup(r1, r2);
      if(!rate.valid()) {
	log() << "MakeGroups - failed to make group from: " 
	      << r1->GetName() << " and " << r2->GetName() << endl;
	continue;
      }

      fLookupGroups.push_back(Groups(r1.get(), r2.get(), rate.get()));

      if     (key == "stream") fStream.push_back(rate);
      else if(key == "level")  fLevel.push_back(rate);
      else if(key == "group")  fGroup.push_back(rate);
      else {
	log() << "MakeGroups - unknown key: " << key << endl;
      }
    }
    else {
      log() << "MakeGroups - invalid group handle(s)" << endl; 
    }
  }
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::RateChain> Anp::PredRates::MakeChain(const Handle<RateChain> &r1,
						      const Handle<RateChain> &r2) const
{
  //
  // Create new RateChain object using initial (r1) and extrapolated (r2) chains
  //
  Registry reg;
  reg.Set("RateKey", "WEIGHTED");

  Anp::Handle<Anp::RateChain> rate(new RateChain());
  rate->ConfigReg(reg);

  //
  // Find configuration for new (extrapolated) chain
  //
  TrigConfChain chain;
  
  if(r1.valid() && r2.valid()) {
    chain = r2->GetChain();

    //
    // Check that chains is not activated with new PS >= 1
    //
    if(r1->GetTotalPS() < 1.0 && !(r2->GetTotalPS() < 1.0)) {
      log() << "MakeChain - can not activate PS rate 2 for: " << r2->GetName() << endl;
      chain.setPrescale(0);
    }
    
    //
    // Check that chains is not activated with new PT >= 1
    //
    if(false && !r1->IsPT() && r2->IsPT()) {
      cerr << "MakeChain - can not activate PT rate 2 for: " << r2->GetName() << endl;
      chain.setPassThrough(0);
    }
  }
  else {
    log() << "MakeChain - invalid chain handle(s)" << endl;
  }
  
  //
  // Use special config method for rates prediction
  //
  rate->Config(chain);

  return rate;
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::RateGroup> Anp::PredRates::MakeGroup(const Handle<RateGroup> &r1,
						      const Handle<RateGroup> &r2) const
{
  //
  // Make group for rate prediction
  //  
  if(!r1.valid() || !r2.valid()) {
    log() << "MakeGroup - invalid group handle(s)" << endl;
    return Handle<RateGroup>();
  }

  if(r1->GetName() != r2->GetName()) {
    log() << "MakeGroup - name mistmatch: " << r1->GetName() << "!=" << r2->GetName() << endl;
    return Handle<RateGroup>();
  } 

  const vector<Handle<RateChain> > &vec1 = r1->GetChains();
  const vector<Handle<RateChain> > &vec2 = r2->GetChains();
  
  vector<Handle<RateChain> > chains;
  unsigned level = 0;

  if(vec1.size() != vec2.size()) {
    log() << "MakeGroup - size mismatch: " << vec1.size() << "!=" << vec2.size() << endl;
    return Handle<RateGroup>();
  }
  
  for(unsigned i = 0; i < vec1.size(); ++i) {
    Handle<RateChain> r1 = vec1.at(i);
    Handle<RateChain> r2;

    if(!r1.valid()) {
      log() << "MakeGroup - invalid chain handle" << endl;
      continue;
    }
    
    //
    // Check that no new chains is introduced
    //
    ChainVec::const_iterator rit2 = std::find(vec2.begin(), vec2.end(), r1->GetName());
    if(rit2 != vec1.end()) {
      r2 = *rit2;
    }
    else {
      log() << "MakeGroup - no matching chain 1 for: " << r1->GetName() << endl;
      continue;
    }
    
    //
    // Find predicted chain for this group
    //
    ChainVec::const_iterator ritE = std::find(fChain.begin(), fChain.end(), r1->GetName());
    if(ritE != fChain.end()) {
      chains.push_back(*ritE);
    }
    else {
      log() << "MakeGroup - no matching chain E for: " << r1->GetName() << endl;
      continue;
    }   

    if(level == 0) { 
      level = r1->GetLevel();
    }
    else if(level != r1->GetLevel()) {
      log() << "MakeGroup - level error 1: " << r1->GetName() << endl;
    }
    else if(level != r2->GetLevel()) {
      log() << "MakeGroup - level error 2: " << r2->GetName() << endl;
    }    
  }
  
  if(chains.size() != vec1.size() || chains.size() != vec2.size()) {
    log() << "MakeGroup - chains size mismatch: " << chains.size() << "!=" << vec1.size() << endl;
  }

  if(chains.empty()) {
    log() << "MakeGroup - empty chain vector for: " << r1->GetName() << endl;
    return Handle<RateGroup>();
  }

  Registry reg;
  reg.Set("RateKey", "WEIGHTED");
  reg.Set("RateGroup::Name", r1->GetName()); 
  reg.Set("RateGroup::Level", level); 

  Anp::Handle<Anp::RateGroup> rate(new RateGroup());
  rate->ConfigReg(reg);

  return rate;
}
