/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <exception>
#include <cassert>
#include <iostream>
#include <sstream>

// Local
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostBase/UtilHack.h"
#include "TrigCostRate/RateGroup.h"
#include "TrigCostRate/RateChain.h"

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
void Anp::ExpandVector(std::vector<RateChain *> &rvec, Handle<RateChain> rate, bool insert_ptr)
{
  //
  // Expand vector using chain counter as index and insert pointer if requested
  //
  if(!rate.valid()) return;
  
  const unsigned counter = rate->GetCounter();
  
  if(counter >= rvec.size()) {
    RateChain *chain = 0;
    rvec.insert(rvec.end(), 1+counter-rvec.size(), chain);
  }
  
  if(insert_ptr) {
    rvec.at(counter) = rate.get();
  }
}

//-----------------------------------------------------------------------------
std::vector<TrigConfChain> Anp::GetInput(const TrigConfChain &chain,
					 const std::vector<TrigConfChain>::const_iterator ibeg,
					 const std::vector<TrigConfChain>::const_iterator iend)
{
  //
  // Return all chains which feed this chain
  //
  std::vector<TrigConfChain> rvec;
  
  if(chain.getLevelId() == 1 || chain.getLevelId() > 3) {
    return rvec;
  }
  
  for(std::vector<TrigConfChain>::const_iterator it = ibeg; it != iend; ++it) {
    if(chain.getLowerName() == it->getName()) {
      rvec.push_back(*it);
    }
  }
  
  if(rvec.empty()) {
    vector<string> mvec;
    Anp::StringTok(mvec, chain.getLowerName(), ", ");

    for(std::vector<TrigConfChain>::const_iterator it = ibeg; it != iend; ++it) {
      if(std::find(mvec.begin(), mvec.end(), it->getName()) != mvec.end()) {
	rvec.push_back(*it);
      }
    }
    
    if(rvec.empty() && chain.getLowerName().empty()) {
      for(std::vector<TrigConfChain>::const_iterator it = ibeg; it != iend; ++it) {
	if(chain.getLevelId() == it->getLevelId() + 1) {
	  rvec.push_back(*it);
	}
      }
    }
  }

  return rvec;
}

//-----------------------------------------------------------------------------
Anp::RateChain::RateChain()
  :fPassD(0),
   fPassN(0),
   fPassW(0.0),
   fPassW2(0.0),
   fWeight(0.0),
   fWeightHLT(0.0),
   fIsConfig(false),
   fIsMultiSeed(false),
   fIsPT(false),
   fIsRD0Seed(false),
   fIsRD1Seed(false),
   fRateKey(""),
   fTotalPS(0.0),
   fExpressPS(0.0),
   fCommonScale(0.0),
   fTotalEvents(0.0),
   f1EventError(0.0),
   fNode(0),
   fNodeMultiSeed(0),
   fLumiVec(0),
   fCurrentRun(0)
{
}

//-----------------------------------------------------------------------------
Anp::RateChain::~RateChain()
{
  for(RateLumiMap::iterator lit = fLumiMap.begin(); lit != fLumiMap.end(); ++lit) {
    RateLumiVec *vec = lit->second;
    if(vec) delete vec;
  }
}

//-----------------------------------------------------------------------------
void Anp::RateChain::ConfigReg(const Registry &reg)
{
  //
  // Set properties
  //
  reg.Get("RateChain::TotalPS",     fTotalPS);
  reg.Get("RateChain::1EventError", f1EventError);
  reg.Get("RateChain::CommonScale", fCommonScale);
  reg.Get("RateChain::TotalEvents", fTotalEvents);
  reg.Get("RateChain", "RateKey",   fRateKey);
  reg.Get("MakeRates","IsRD0Seeded",fIsRD0Seed);
  reg.Get("MakeRates","IsRD1Seeded",fIsRD1Seed);
}

//-----------------------------------------------------------------------------
void Anp::RateChain::UpdateReg(const Registry &reg)
{
  //
  // Update variables after event loop is finished
  //
  ConfigReg(reg);

  reg.Get("RateChain::PassN",   fPassN);
  reg.Get("RateChain::PassD",   fPassD);
  reg.Get("RateChain::PassW",   fPassW);
  reg.Get("RateChain::PassW2",  fPassW2);
}

//-----------------------------------------------------------------------------
void Anp::RateChain::ConfigChain(const RateNode *node,
				 const TrigConfChain &chain,
				 const std::vector<TrigConfChain> &confg_vec)
{
  //
  // Configure/update chain
  // 
  if(!node) {
    cout << "RateChain::ConfigChain - invalid RateNode pointer: " << GetName() << endl;
    return;
  }

  fChain = chain;
  fNode  = node;

  if(fChain.getLevelId() == 1) {
    //
    // Read configuration for L1 chain
    //
    if(!(fChain.getPrescale() < 1.0)) {
      fTotalPS = fChain.getPrescale();
    }
  }
  else if(fChain.getLevelId() == 2) {
    //
    // Read configuration for input L1 chain
    //
    if(!(fChain.getPrescale() < 1.0)) {
      fTotalPS = fChain.getPrescale();
    }

    const vector<TrigConfChain> inputL1 = Anp::GetInput(chain, confg_vec.begin(), confg_vec.end());
    
    if(inputL1.size() == 1) {
      //
      // Read configuration and compute total PS
      //
      fInputL1 = inputL1.at(0);
      
      if(!(fInputL1.getPrescale() < 1.0)) { 
	fTotalPS *= fInputL1.getPrescale();
      } 
      else {
	fTotalPS = 0.0;
      }
    }
    else {
      cout << "RateChain::ConfigChain - can not configure multi-seeded L2 chains for: " << chain.getName() << endl;
      return;
    }

    //
    // Compute event weight when PT >= 1.0
    //
    if(!(fChain.getPassThrough() < 1.0) && !(fChain.getPrescale() < 1.0)) {
      fIsPT = true;
    }
  }
  else if(fChain.getLevelId() == 3) {
    //
    // Read configurartion for input L2 chain
    //
    const vector<TrigConfChain> inputL2 = Anp::GetInput(chain, confg_vec.begin(), confg_vec.end());

    if(!(fChain.getPrescale() < 1.0)) {
      fTotalPS = fChain.getPrescale();
    }

    if(inputL2.size() == 1) {
      fInputL2 = inputL2.at(0);

      if(!(fInputL2.getPrescale() < 1.0)) {
	fTotalPS *= fInputL2.getPrescale();
      }
      else {
	fTotalPS = 0.0;
      }
    }
    else {
      cout << "RateChain::ConfigChain - can not configure multi-seeded EF chains for: " << chain.getName() << endl;
      return;
    }

    //
    // Read configurartion for input L1 chain
    //
    const vector<TrigConfChain> inputL1 = Anp::GetInput(fInputL2, confg_vec.begin(), confg_vec.end());
      
    if(inputL1.size() == 1) {
      fInputL1 = inputL1.at(0);
      
      if(!(fInputL1.getPrescale() < 1.0)) {
	fTotalPS *= fInputL1.getPrescale();
      }
      else {
	fTotalPS = 0.0;
      }
         
    }
    else {
      cout << "RateChain::ConfigChain - can not configure multi-seeded EF/L2 chains for: " << chain.getName() << endl;
      return;
    }
    
    if(!(fInputL1.getPrescale()    < 1.0) &&
       !(fInputL2.getPrescale()    < 1.0) && 
       !(fInputL2.getPassThrough() < 1.0)) {
      //
      // Case for PT >= 1 at L2
      //
      fIsPT = true;
    }
    if(!(fInputL1.getPrescale()  < 1.0) &&
       !(fInputL2.getPrescale()  < 1.0) &&
       !(fChain.getPrescale()    < 1.0) &&
       !(fChain.getPassThrough() < 1.0)) {
      //
      // Case for PT>=1 at EF
      //
      fIsPT = true;
    }

    //
    // Fill express stream PS
    // 
    fExpressPS = Anp::GetExpressPS(fChain);
  }

  //
  // Configuration was succesful
  // 
  fIsConfig = true;  
}

//-----------------------------------------------------------------------------
void Anp::RateChain::ConfigMultiSeed(const RateNode *node,
				     const RateNode *nodeMultiSeed,
				     const TrigConfChain &chain,
				     const std::vector<TrigConfChain> &config_vec)
{
  //
  // Special case: multi-seeded chain or unseeded chains - called after normal config!
  //
  if(fIsConfig || fNode) {
    cout << "RateChain::ConfigMultiSeed - error! This object is already configured." << endl;
    return;
  }
  if(fChain.getLevelId() == 1) {
    cout << "RateChain::ConfigMultiSeed - error! L1 can not be multi-seeded." << endl;
    return;
  }
  if(!node || !nodeMultiSeed) {
    cout << "RateChain::ConfigChain - invalid RateNode pointer for: " << GetName() << endl;
    return;
  }

  fChain         = chain;
  fNode          = node;
  fNodeMultiSeed = nodeMultiSeed;
  fIsConfig      = true;
  
  if(chain.getLevelId() == 2) {
    //
    // Case for unseeded L2 chain:
    //   - connect HLT directly to RootNode
    //
    const string &seed = fChain.getLowerName();

    if(seed.find(",") == std::string::npos && !seed.empty()) {
      cout << "RateChain::ConfigMultiSeed - wrong L1 input for chain: " << chain.getLowerName() << endl;
    }
  }
  else if(chain.getLevelId() == 3) {

    const vector<TrigConfChain> inputL2 = Anp::GetInput(chain, config_vec.begin(), config_vec.end());

    if(inputL2.size() == 1) {
      //
      // Case for unseeded L2 chain: input is EF chain seeded by this L2 chain
      //
      fInputL2 = inputL2.front();     
      const string &seedL1 = fInputL2.getLowerName();

      if(seedL1.find(",") == std::string::npos && !seedL1.empty()) {
	cout << "RateChain::ConfigMultiSeed - wrong L1 input for chain: " << chain.getLowerName() << endl;
      } 
    }
    else {
      //
      // Case for unseeded/multi-seeded EF chain - same as unseeded L2 chain  
      //
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::RateChain::Config(const TrigConfChain &chain)
{
  //
  // Special case: this chain is used for rate prediction with PredRates
  //
  fIsConfig = true;
  fChain = chain;
}

//-----------------------------------------------------------------------------
void Anp::RateChain::PredRates(const RateData &data)
{
  //
  // Special case: weight is computed by PredRates 
  //
  fWeight  = data.event_weight;
  fPassN  += unsigned(fWeight > 0.0);
  fPassD  += 1;
  fPassW  += fWeight;
  fPassW2 += fWeight*fWeight; 
}

//-----------------------------------------------------------------------------
void Anp::RateChain::SetCompliment(Handle<RateGroup> compl_handle)
{
  fCompliment = compl_handle; 
}

//-----------------------------------------------------------------------------
void Anp::RateChain::SetLevelGroup(Handle<RateGroup> group_handle)
{
  fLevelGroup = group_handle; 
}

//-----------------------------------------------------------------------------
void Anp::RateChain::SetOutputGroup(Handle<RateGroup> output_handle)
{
  fOutputGroup = output_handle;
}

//-----------------------------------------------------------------------------
void Anp::RateChain::SetStreamGroup(Handle<RateGroup> stream_handle)
{
  fStreamGroup = stream_handle;
}

//-----------------------------------------------------------------------------
double Anp::RateChain::GetEventsPassed() const
{
  //
  // Get total number of passed events
  //
  if     (fRateKey == "WEIGHTED")     { return fPassW; }
  else if(fRateKey == "COUNT_WITHPS") { return fPassN; }
  else if(fRateKey == "COUNT")        { return fPassD; }
  else {
    cerr << "RateChain::GetEventsPassed - unknown key: \"" << fRateKey << "\"" << endl;
  }

  return 0.0;
}

//-----------------------------------------------------------------------------
double Anp::RateChain::GetEventsPassedRaw() const
{
  return fPassD;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateChain::GetPassFrac() const
{
  //
  // Get pass rate
  //
  std::pair<double, double> res(0.0, 0.0);

  //
  // Check parameters and variables for sensible values
  //
  if(fTotalEvents < 1.0) {
    cerr << "RateChain::GetPassFrac - internal error! "
	 << "TotalEvents=" << fTotalEvents << " for chain: " << GetName() << endl;
    return res;
  }

  if(fRateKey == "WEIGHTED") {
    res.first  = fPassW/fTotalEvents;
    res.second = std::sqrt(fPassW2)/fTotalEvents;
  }
  else if(fRateKey == "COUNT_WITHPS") {
    res.first  = fPassN/fTotalEvents;
    res.second = std::sqrt(fPassN)/fTotalEvents;
  }
  else if(fRateKey == "COUNT"){
    res.first  = fPassD/fTotalEvents;
    res.second = std::sqrt(fPassD)/fTotalEvents;
  }

  //
  // Reset error for zero rate chain
  //
  if(!(res.first > 0.0)) {
    res.second = f1EventError/fTotalEvents;
  }
  
  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateChain::GetPassRate() const
{
  //
  // Get pass fraction
  //
  std::pair<double, double> res = GetPassFrac();

  //
  // Check parameters and variables for sensible values
  //
  if(!(fCommonScale > 0.0)) {
    cerr << "RateChain::GetPassRate - internal error! "
	 << "CommonScale=" << fCommonScale << " for chain: " << GetName() << endl;
    return res;
  }

  res.first  *= fCommonScale;
  res.second *= fCommonScale;
  
  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateChain::GetPassFracRaw() const
{
  //
  // Get pass rate
  //
  std::pair<double, double> res(0.0, 0.0);

  //
  // Check parameters and variables for sensible values
  //
  if(fTotalEvents < 1.0) {
    cout << "RateChain::GetPassFracRaw - internal error! "
	 << "TotalEvents=" << fTotalEvents << " for chain: " << GetName() << endl;
    return res;
  }

  res.first  = fPassD/fTotalEvents;
  res.second = std::sqrt(fPassD)/fTotalEvents;
  
  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateChain::GetPassRateRaw() const
{
  //
  // Get pass fraction
  //
  std::pair<double, double> res = GetPassFracRaw();

  if(!(fCommonScale > 0.0)) {
    cerr << "RateChain::GetPassRateRaw - internal error! "
	 << "CommonScale=" << fCommonScale << " for chain: " << GetName() << endl;
    return res;
  }

  res.first  *= fCommonScale;
  res.second *= fCommonScale;
  
  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateChain::GetFracUnique() const
{
  //
  // Get unqiue fractions
  //
  std::pair<double, double> res(0.0, 0.0);

  //
  // Check parameters and variables for sensible values
  //
  if(fTotalEvents < 1.0) {
    cout << "RateChain::GetFracUnique - internal error! "
	 << "TotalEvents=" << fTotalEvents << " for chain: " << GetName() << endl;
    return res;
  }

  double compl_count = 0.0, stream_count = 0.0, group_count = 0.0;

  if(fCompliment.valid() && fStreamGroup.valid()) {      
    compl_count = fCompliment->GetEventsPassed();
    stream_count = fStreamGroup->GetEventsPassed();
  }

  if     (fRateKey == "WEIGHTED")     { group_count = fPassW2; }
  else if(fRateKey == "COUNT_WITHPS") { group_count = fPassN; }
  else if(fRateKey == "COUNT")        { group_count = fPassD; }
  else {
    cerr << "RateChain::GetFracUnique - unknown key: \"" << fRateKey << "\"" << endl;
  }
  
  //
  // Coompute unique rate
  //
  res.first  = (stream_count-compl_count)/fTotalEvents;
  res.second = std::sqrt(group_count)/fTotalEvents;


  if(res.first < 0.0) {
    cerr << "RateChain::GetPassFrac - kUNIQUE_WEIGHTED error for chain " 
	 << GetName() << ": rate, error = " << res.first << ", " << res.second << endl;
  }

  //
  // Reset error for zero rate chain
  //
  if(!(res.first > 0.0)) {
    res.second = f1EventError/fTotalEvents;
  }
  
  return res;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Anp::RateChain::GetRateUnique() const
{
  //
  // Get unqiue fractions
  //
  std::pair<double, double> res = GetFracUnique();

  if(!(fCommonScale > 0.0)) {
    cerr << "RateChain::GetRateUnique - internal error! "
	 << "CommonScale=" << fCommonScale << " for chain: " << GetName() << endl;
    return res;
  }

  res.first  *= fCommonScale;
  res.second *= fCommonScale;
  
  return res;
}

//---------------------------------------------------------------------------------------
bool Anp::RateChainSort::operator()(const Anp::Handle<Anp::RateChain> &lhs,
				    const Anp::Handle<Anp::RateChain> &rhs)
{
  //
  // Compare two objects based on key
  //
  if(!lhs.valid() || !rhs.valid()) { 
    cerr << "RateChainSort::operator() - lhs.ptr=" << lhs.get() << ", " << "rhs.ptr=" << rhs.get() << endl;
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

  //
  // Compare by name or type
  //
  return lhs->GetName() < rhs->GetName();
}

//-----------------------------------------------------------------------------
void Anp::RateChain::PrintRateResult(std::ostream &os) const
{
  //
  // Print rate result summary for debugging
  // 
  os << "RateChain::PrintRateResult - " << GetName() << endl
     << "   Chain PS        = " << GetChain().getPrescale()   << endl
     << "   Total PS        = " << GetTotalPS()               << endl
     << "   Pass frac       = " << GetPassFrac().first        << endl
     << "   Pass rate       = " << GetPassRate().first        << endl
     << "   Pass evts       = " << GetEventsPassed()          << endl
     << "   Pass rate raw   = " << GetPassRateRaw().first     << endl
     << "   Pass evts raw   = " << GetEventsPassedRaw()       << endl;
}

//-----------------------------------------------------------------------------
void Anp::RateChain::ResetConfig()
{
  //
  // Reset configuration variables - keep event counts
  //
  fIsConfig      = false;
  fIsMultiSeed   = false;
  fIsRD0Seed     = false;
  fIsRD1Seed     = false;
  fIsPT          = false;
  fExpressPS     = 0.0;
  fWeightHLT     = 0.0;
  fTotalPS       = 0.0;
  fInputL1       = TrigConfChain();
  fInputL2       = TrigConfChain();
  fChain         = TrigConfChain();
}

//-----------------------------------------------------------------------------
Anp::LumiCount& Anp::RateChain::GetLumi(unsigned run, unsigned lumi)
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
