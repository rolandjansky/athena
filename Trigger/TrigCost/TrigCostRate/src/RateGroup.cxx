/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <cassert>
#include <iostream>
#include <sstream>

// Local
#include "TrigCostBase/UtilCore.h"

// Trigger
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostRate/RateGroup.h"

using namespace std;

//-----------------------------------------------------------------------------
Anp::RateGroup::RateGroup()
  :fPassD(0),
   fPassN(0),
   fPassW(0.0),
   fPassW2(0.0),
   fWeight(0.0),
   fIsRD0Seed(false),
   fIsRD1Seed(false),
   fDebug(false),
   fLevel(0),
   fRateKey(""),
   fCommonScale(0.0),
   fTotalEvents(0.0),
   fEventError(0.0),
   fEventWeight(1.0),
   fNode(0),
   fLumiVec(0),
   fCurrentRun(0)
{
}

//-----------------------------------------------------------------------------
void Anp::RateGroup::ConfigReg(const Registry &reg)
{
  //
  // Read configuration
  //
  reg.Get("RateGroup", "Debug",      fDebug);
  reg.Get("RateGroup", "RateKey",    fRateKey);
  reg.Get("RateGroup::Name",         fName);
  reg.Get("RateGroup::Level",        fLevel);
   
  if(fLevel != 1 && fLevel != 2 && fLevel != 3) {
    string level;
    reg.Get("RateGroup::Level", level);

    if     (level == "L1") fLevel = 1;
    else if(level == "L2") fLevel = 2;
    else if(level == "EF") fLevel = 3;
  }

  reg.Get("RateGroup::EventError",  fEventError);
  reg.Get("RateGroup::EventWeight", fEventWeight);
  reg.Get("RateGroup::CommonScale", fCommonScale);
  reg.Get("RateGroup::TotalEvents", fTotalEvents);
  reg.Get("MakeRates","IsRD0Seeded",fIsRD0Seed);
  reg.Get("MakeRates","IsRD1Seeded",fIsRD1Seed);

}

//-----------------------------------------------------------------------------
void Anp::RateGroup::UpdateReg(const Registry &reg)
{
  //
  // Read scalling information
  //
  ConfigReg(reg);
}

//-----------------------------------------------------------------------------
void Anp::RateGroup::ConfigGroup(const RateNode *node,
				 const vector<Handle<RateChain> > &chains)
{
  //
  // Check internal state and inputs 
  //
  if(!fChains.empty()) {
    cout << "RateGroup::Config - configuration is already set" << endl;
    return;
  }
  if(!node) {
    cout << "RateGroup::Config - null RateNode pointer: " << GetName() << endl;
    return;
  }
  if(chains.empty()) {
    cout << "RateGroup::Config - empty chain list: " << GetName() << endl;
    return;
  }

  //
  // Check input chains
  //
  for(unsigned i = 0; i < chains.size(); ++i) {
    Handle<RateChain> chain = chains.at(i);
    if(!chain.valid()) {      
      cout << "RateGroup::Config - invalid RateChain handle" << endl;
      return;
    }
    
    if(fLevel == 0) {
      fLevel = chain->GetLevel();
    }
    else if(fLevel != chain->GetLevel()) {
      cout << "RateGroup::Config - level mismatch: " << chain->GetLevel() << endl;
      return;
    }
    
    fCounters.push_back(chain->GetChain().getCounter());
  }
  
  fChains = chains;
  fNode   = node;
  
  if(fChains.empty()) {
    cout << "RateGroup::Config - group has no chains: " << GetName() << endl;
    return;
  }
}

//-----------------------------------------------------------------------------
void Anp::RateGroup::PredRates(const RateData &data)
{
  //
  // Special case: this object is filled by PredRates (rate prediction using ratio of weights)
  //

  fWeight  = data.event_weight;
  fPassD  += unsigned(fWeight > 0.0);
  fPassN  += unsigned(fWeight > 0.0);
  fPassW  += fWeight;
  fPassW2 += fWeight*fWeight;

  //
  // Increment event count for current lumi block
  //
  LumiCount &count = GetLumi(data.run_number, data.lumi_block);
  
  count.passN += unsigned(fWeight > 0.0);
  count.passD += unsigned(fWeight > 0.0);
  count.passW += fWeight;
  count.passW += fWeight*fWeight;
}

//-----------------------------------------------------------------------------
void Anp::RateGroup::SetCompliment(Handle<RateGroup> compl_handle)
{
  fCompliment = compl_handle; 
}

//-----------------------------------------------------------------------------
void Anp::RateGroup::SetLevelGroup(Handle<RateGroup> group_handle)
{
  fLevelGroup = group_handle; 
}

//-----------------------------------------------------------------------------
void Anp::RateGroup::SetUnionGroup(Handle<RateGroup> g1,
				   Handle<RateGroup> g2)
{
  fGroup1 = g1; 
  fGroup2 = g2; 
}

//-----------------------------------------------------------------------------
std::vector<TrigConfChain> Anp::RateGroup::GetConfigChains() const
{
  std::vector<TrigConfChain> chains;

  for(unsigned i = 0; i < fChains.size(); ++i) {
    chains.push_back(fChains.at(i)->GetChain());
  }
  
  return chains;
}

//-----------------------------------------------------------------------------
double Anp::RateGroup::GetEventsPassed() const
{
  //
  // Get number of passed events
  //
  if     (fRateKey == "WEIGHTED")     { return fPassW; }
  else if(fRateKey == "COUNT_WITHPS") { return fPassN; }
  else if(fRateKey == "COUNT")        { return fPassD; }
  else {
    cout << "RateGroup::GetEventsPassed - unknown key: \"" << fRateKey << "\"" << endl;
  }

  return 0;
}

//-----------------------------------------------------------------------------
double Anp::RateGroup::GetEventsPassedRaw() const
{
  //
  // Get raw number of passed events
  //
  return fPassD;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateGroup::GetFracUnique() const
{
  //
  // Return unique pass rate
  //
  std::pair<double, double> res(0.0, 0.0);

  //
  // Check parameters and variables for sensible values
  //
  if(!(fCommonScale > 0.0) || fTotalEvents < 1.0) {
    cout << "RateGroup::GetFracUnique - error! "
	 << "CommonScale, TotalEvents=" << fCommonScale << ", " << fTotalEvents << endl;
    return res;
  }
  
  double compl_count = 0.0, level_count = 0.0, group_count = 0.0;

  if(fCompliment.valid() && fLevelGroup.valid()) {      
    compl_count = fCompliment->GetEventsPassed();
    level_count = fLevelGroup->GetEventsPassed();
    group_count =              GetEventsPassed();
  }
      
  res.first  = (level_count-compl_count)/fTotalEvents;
  res.second = std::sqrt(group_count)/fTotalEvents;
  
  if(res.first < 0.0) {
    cout << "RateChain::GetFracUnique - logic error for: " << GetName() << endl;
  }

  //
  // Reset error for zero rate chain
  //
  if(!(res.first > 0.0)) {
    res.second = fEventError/fTotalEvents;
  } 

  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateGroup::GetRateUnique() const
{
  //
  // Return unique pass rate
  //
  std::pair<double, double> res = GetFracUnique();
  
  res.first  *= fCommonScale;
  res.second *= fCommonScale;
  
  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateGroup::GetFracOverlap() const
{
  //
  // Return unique pass rate
  //
  std::pair<double, double> res(0.0, 0.0);

  //
  // Check parameters and variables for sensible values
  //
  if(!(fCommonScale > 0.0) || fTotalEvents < 1.0) {
    cout << "RateGroup::GetFracOverlap - error! "
	 << "CommonScale, TotalEvents=" << fCommonScale << ", " << fTotalEvents << endl;
    return res;
  }

  double count1 = 0.0, count2 = 0.0, countM = 0.0;

  if(fGroup1.valid() && fGroup2.valid()) {            
    count1 = fGroup1->GetEventsPassed();
    count2 = fGroup2->GetEventsPassed();
    countM =          GetEventsPassed();
  }
      
  res.first  = (count1+count2-countM)/fTotalEvents;
  res.second = std::sqrt(countM)/fTotalEvents;

  //
  // Reset error for zero rate chain
  //
  if(!(res.first > 0.0)) {
    res.second = fEventError/fTotalEvents;
  }

  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateGroup::GetRateOverlap() const
{
  //
  // Return unique pass rate
  //
  std::pair<double, double> res = GetFracOverlap();
  
  res.first  *= fCommonScale;
  res.second *= fCommonScale;
  
  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateGroup::GetPassFrac() const
{
  //
  // Get pass fraction
  //
  std::pair<double, double> res(0.0, 0.0);

  //
  // Check parameters and variables for sensible values
  //
  if(!(fCommonScale > 0.0) || fTotalEvents < 1.0) {
    cout << "RateGroup::GetPassFrac - error! "
	 << "CommonScale, TotalEvents=" << fCommonScale << ", " << fTotalEvents << endl;
    return res;
  }

  if(fRateKey == "WEIGHTED") { 
    //
    // Get fraction using events weighted by prescale
    //
    res.first  = fPassW/fTotalEvents;
    res.second = std::sqrt(fPassW2)/fTotalEvents;
  }
  else if(fRateKey == "COUNT_WITHPS") {
    //
    // Get fraction using raw (no prescales) event count
    //
    res.first  = fPassN/fTotalEvents;
    res.second = std::sqrt(fPassN)/fTotalEvents;
  }
  else if(fRateKey == "COUNT"){
    //
    // Get fraction using raw (no prescales) event count for data (ignore PS and PT)
    //
    res.first  = fPassD/fTotalEvents;
    res.second = std::sqrt(fPassD)/fTotalEvents;
  }
  else {
    cout << "RateGroup::GetPassFrac - unknown key: \"" << fRateKey << "\"" << endl;
  }

  //
  // Reset error for zero rate chain
  //
  if(!(res.first > 0.0)) {
    res.second = fEventError/fTotalEvents;
  }
  
  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateGroup::GetPassRate() const
{
  //
  // Get pass rate
  //
  std::pair<double, double> res = GetPassFrac();

  res.first  *= fCommonScale;
  res.second *= fCommonScale;
  
  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateGroup::GetPassFracRaw() const
{
  //
  // Get pass rate
  //
  std::pair<double, double> res(0.0, 0.0);

  //
  // Check parameters and variables for sensible values
  //
  if(!(fCommonScale > 0.0) || fTotalEvents < 1.0) {
    cout << "RateGroup::GetPassFracRaw - error! "
	 << "CommonScale, TotalEvents=" << fCommonScale << ", " << fTotalEvents << endl;
    return res;
  }

  res.first  = fPassD/fTotalEvents;
  res.second = std::sqrt(fPassD)/fTotalEvents;
  
  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateGroup::GetPassRateRaw() const
{
  //
  // Get pass rate
  //
  std::pair<double, double> res = GetPassFracRaw();

  res.first  *= fCommonScale;
  res.second *= fCommonScale;
  
  return res;
}

//---------------------------------------------------------------------------------------
bool Anp::RateGroupSort::operator()(const Anp::Handle<Anp::RateGroup> &lhs,
				    const Anp::Handle<Anp::RateGroup> &rhs)
{
  //
  // Compare two objects based on key
  //
  if(!lhs.valid() || !rhs.valid()) { 
    cout << "RateGroupSort::operator() - lhs.ptr=" << lhs.get() << ", " << "rhs.ptr=" << rhs.get() << endl;
    return false;
  }

  if(!fKey.empty() && fKey == "rate") {
    //
    // Compare by variables
    //
    return (lhs->GetPassRate() > rhs->GetPassRate());
  }
  else if(!fKey.empty() && fKey == "rate_data") {
    //
    // Compare by variables
    //
    return (lhs->GetPassRateRaw() > rhs->GetPassRateRaw());
  }
  else if(!fKey.empty() && fKey == "name") {
    //
    // Compare by variables
    //
    return (lhs->GetName() > rhs->GetName());
  }

  //
  // Compare by name or type
  //
  return lhs->GetName() < rhs->GetName();
}

//-----------------------------------------------------------------------------
Anp::LumiCount& Anp::RateGroup::GetLumi(unsigned run, unsigned lumi)
{
  //
  // Find current run number and lumi block
  //
  if(fCurrentRun == 0 || run != fCurrentRun) {
    RateLumiMap::iterator rit = fLumiMap.find(run);
    if(rit == fLumiMap.end()) {
      rit = fLumiMap.insert(RateLumiMap::value_type(run, new RateLumiVec())).first;
    }
    
    fCurrentRun = run;
    fLumiVec = rit->second;
  }

  if(lumi >= fLumiVec->size()) {
    fLumiVec->insert(fLumiVec->end(), 1 + lumi - fLumiVec->size(), Anp::LumiCount());
    fLumiVec->at(lumi).lumi_block = lumi;
  }

  return fLumiVec->at(lumi);
}

//-----------------------------------------------------------------------------
std::ostream& Anp::RateGroup::log() const
{
  std::cout << "RateGroup:" << GetName() << "::";
  return std::cout;
}
