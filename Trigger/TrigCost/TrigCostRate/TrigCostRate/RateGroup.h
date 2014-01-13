/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_RATEGROUP_H
#define ANP_RATEGROUP_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : RateGroup
 * @Author : Rustem Ospanov
 *
 * @Brief  : Main class for computing and accessing trigger rate results for groups
 * 
 **********************************************************************************/

// C/C++
#include <cmath>
#include <functional>
#include <map>
#include <string>
#include <sstream>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/HtmlNode.h"

// Local
#include "TrigCostRate/RateChain.h"

namespace Anp
{
  class RateGroup: virtual public Base {
  public:

    RateGroup();
    virtual ~RateGroup() {}

    void ConfigReg(const Registry &reg);
    void UpdateReg(const Registry &reg);

    void ConfigGroup(const RateNode *Node,
		     const std::vector<Handle<RateChain> > &chains);

    void PassEvent(const RateData &data, const RateBits &bits);
    void PredRates(const RateData &data);

    void SetCompliment(Handle<RateGroup> compl_handle);
    void SetLevelGroup(Handle<RateGroup> group_handle);
    void SetUnionGroup(Handle<RateGroup> g1, Handle<RateGroup> g2);

    const std::string& GetName()    const { return fName; }
    const std::string& GetRateKey() const { return fRateKey; }

    unsigned GetLevel() const { return fLevel; }

    std::vector<Handle<RateChain> >& GetChains() { return fChains; }

    std::vector<TrigConfChain> GetConfigChains() const;

    Handle<RateGroup> GetGroup1() { return fGroup1; }
    Handle<RateGroup> GetGroup2() { return fGroup2; }

    double GetEventsPassed()    const;
    double GetEventsPassedRaw() const;

    std::pair<double, double> GetPassFrac()    const;
    std::pair<double, double> GetPassFracRaw() const;
    std::pair<double, double> GetPassRate()    const;
    std::pair<double, double> GetPassRateRaw() const;

    std::pair<double, double> GetFracUnique()  const;
    std::pair<double, double> GetRateUnique()  const;

    std::pair<double, double> GetFracOverlap() const;
    std::pair<double, double> GetRateOverlap() const;
    
    double GetWeight() const { return fWeight; }
    
    const RateLumiMap& GetCounts() const { return fLumiMap; }

  public:
    
    struct L1Seed {
      L1Seed() : counter(0) {}
      L1Seed(unsigned counter_, RateChain *chain_) : counter(counter_)
      {
	chains.push_back(chain_);
      }
      
      unsigned                 counter;  // L1 counter
      std::vector<RateChain *> chains;   // Chains starting at this L1 item
    };

    struct L2Seed {
      L2Seed() {}
      ~L2Seed() {}
    };

    typedef std::vector<TrigConfChain>    ConfigVec;
    typedef std::vector<Anp::Handle<Anp::RateChain> > ChainsVec;

  private:

    LumiCount &GetLumi(unsigned run, unsigned lumi);

    std::ostream& log() const;

  private:

    // Event counters:
    unsigned          fPassD;        // Count passed events as in data
    unsigned          fPassN;        // Count passed events
    long double       fPassW;        // Event weight sum
    long double       fPassW2;       // Event weight sum squared
    
    // Event variables:
    long double       fWeight;       // Current total event weight
    bool              fIsRD0Seed;      // Does this chain seed from RD0
    bool              fIsRD1Seed;      // Does this chain seed from RD1

    // Properties:
    bool              fDebug;        // Print debug info
    unsigned          fLevel;        // Trigger level
    std::string       fName;         // Group name
    std::string       fRateKey;      // Which type of rate calcuation should we use?

    double            fCommonScale;  // Common scale factor: converts fraction to rate
    double            fTotalEvents;  // Total number of events (denominator)
    double            fEventError;   // Single event error
    double            fEventWeight;  // Overall event weight (eg for xsecs)

    // Objects:
    ChainsVec                  fChains;      // RateChains for this group
    std::vector<unsigned>      fCounters;    // Chain counters for group triggers

    Handle<RateGroup>          fCompliment;  // Pointer to compliment of this chain
    Handle<RateGroup>          fLevelGroup;  // Pointer to total level rate group

    Handle<RateGroup>          fGroup1;      // Pointer to first  group in union
    Handle<RateGroup>          fGroup2;      // Pointer to second group in union

    std::vector<L1Seed>        fL1Seeds;     // L1 cummulative chain weight
    std::vector<L2Seed>        fL2Seeds;     // TODO: implement cumulative L2 seeds
    
    const RateNode            *fNode;        // Rate elem for tree based rate code

    //
    // Count passed events by run and lumi blocks
    //    
    RateLumiVec               *fLumiVec;     // Number of events by lumi block
    RateLumiMap                fLumiMap;     // Lumi block counts by run number
    unsigned                   fCurrentRun;  // Current run number    
  }; 


  //-----------------------------------------------------------------------------
  // Count passed events for trigger group
  //
  inline void Anp::RateGroup::PassEvent(const RateData &data, const RateBits &bits)
  {
    //
    // Compute weight using tree based rate method
    //
    if(fNode) {
      fWeight = fNode->ComputeWeight()*data.event_weight;

      // Apply weights for the Bunch group scaling
      if(fIsRD0Seed) { 
	fWeight *= data.event_scale_rd0; 
      }  
      else if(fIsRD1Seed) {
	fWeight *= data.event_scale_rd1; 
      }  
      else { 
	fWeight *= data.event_scale_lumi;
      }
    }

    else{
      fWeight = 0.0;
    }
    bool pass = false;
    for(std::vector<unsigned>::const_iterator it = fCounters.begin(); it != fCounters.end(); ++it) {
      if(bits.TestTriggerBit(fLevel, *it)) {
	pass = true;
	break;
      }
    }

    fPassD  += pass;
    fPassN  += unsigned(fWeight > 0.0);
    fPassW  += fWeight;
    fPassW2 += fWeight*fWeight;
  }

  // 
  // Inlined comparisons operators
  //
  inline bool operator==(const Handle<RateGroup> &lhs, const Handle<RateGroup> &rhs) {
    return lhs->GetName() == rhs->GetName();
  }
  inline bool operator!=(const Handle<RateGroup> &lhs, const Handle<RateGroup> &rhs) {
    return lhs->GetName() != rhs->GetName();
  }
  inline bool operator==(const Handle<RateGroup> &elem, const std::string &name) {
    return elem->GetName() == name;
  }
  inline bool operator==(const std::string &name, const Handle<RateGroup> &elem) {
    return elem->GetName() == name;
  }

  inline bool operator==(unsigned id, const RateGroup::L1Seed &rhs) { return id == rhs.counter; }
  inline bool operator==(const RateGroup::L1Seed &rhs, unsigned id) { return id == rhs.counter; }

  //
  // Helper structure to sort by total time, mean event time, etc
  //
  struct RateGroupSort {

    RateGroupSort() :fKey() {}
    explicit RateGroupSort(const std::string &key) :fKey(key) {}

    bool operator()(const Anp::Handle<Anp::RateGroup> &lhs,
		    const Anp::Handle<Anp::RateGroup> &rhs);
    
    private:

    std::string fKey;
  };
}

#endif
