/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_RATECHAIN_H
#define ANP_RATECHAIN_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : RateChain
 * @Author : Rustem Ospanov
 *
 * @Brief  : Main class for computing and accessing trigger rate results for chains
 * 
 **********************************************************************************/

// C/C++
#include <bitset>
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
#include "TrigCostRate/RateNode.h"

//
// This class computes rate for single trigger chain
// 
// GetTotalEvents(): total number of events in sample
// GetCommonScale(): common scale factor to convert pass fraction to rate in Hz
// GetChainEvents(): total number of passed events (with or without weight)
//
// GetPassFrac() pair: pass fraction   (first) and statistical error (second)
// GetPassFrac() pair: pass rate in Hz (first) and statistical error (second)
//

namespace Anp
{
  struct RateData
  {
    RateData() : 
      run_number(0), lumi_block(0), event_number(0), event_weight(1.0), event_scale_lumi(1.0), 
      event_scale_rd0(1.0), event_scale_rd1(1.0), event_scale_pileup(1.0) {}

    unsigned run_number;
    unsigned lumi_block;
    unsigned event_number;
    double   event_weight;
    double   event_scale_lumi;
    double   event_scale_rd0;
    double   event_scale_rd1;
    double   event_scale_pileup;
  };

  struct RateBits
  {    
    std::bitset<512>   bitsL1;   // Decision bits for L1 triggers
    std::bitset<8192>  bitsL2;   // Decision bits for L2 triggers (hardcoded number of chains)
    std::bitset<8192>  bitsEF;   // Decision bits for EF triggers (hardcoded number of chains)
    
    bool TestTriggerBit(unsigned level, unsigned counter) const
    {
      switch(level) {
      case 1: return bitsL1.test(counter);
      case 2: return bitsL2.test(counter);
      case 3: return bitsEF.test(counter);
      }
      
      return false;
    }

    void SetTriggerBit(unsigned level, unsigned counter)
    {

      switch(level) {
      case 1: bitsL1.set(counter, 1); return;
      case 2: bitsL2.set(counter, 1); return;
      case 3: bitsEF.set(counter, 1); return;
      }
    }
  };

  struct LumiCount 
  {
    LumiCount() : lumi_block(-1), rate_count(0),
    passD(0), passN(0), passW(0.0), passW2(0.0) {}
    
    int GetLumi() const { return lumi_block; }

    double GetLumiEventsPassed()    const { return passW; }
    double GetLumiEventsPassedRaw() const { return passD; }

    int      lumi_block;
    unsigned rate_count;
    
    unsigned passD;
    unsigned passN;
    double   passW;
    double   passW2;
  };

  typedef std::vector<LumiCount>            RateLumiVec;
  typedef std::map<unsigned, RateLumiVec *> RateLumiMap;

  class RateGroup;
  
  //
  // Make class for computing chain rates
  //
  class RateChain: virtual public Base {
  public:

    RateChain();
    virtual ~RateChain();

    void ConfigReg(const Registry &reg);
    void UpdateReg(const Registry &reg);
    
    void ConfigChain(const RateNode *node,
		     const TrigConfChain &chain,
		     const std::vector<TrigConfChain> &confg_vec);

    void ConfigMultiSeed(const RateNode *node,
			 const RateNode *nodeMultiSeed,
			 const TrigConfChain &chain,
			 const std::vector<TrigConfChain> &config_vec);

    void Config(const TrigConfChain &chain);
    
    void PassEvent(const RateData &data, const RateBits &bits);
    void PredRates(const RateData &data);
    
    void SetCompliment(Handle<RateGroup> compl_handle);
    void SetLevelGroup(Handle<RateGroup> group_handle);
    void SetOutputGroup(Handle<RateGroup> output_handle);
    void SetStreamGroup(Handle<RateGroup> stream_handle);

    const std::string&   GetName()    const { return fChain.getName(); }
    const std::string&   GetRateKey() const { return fRateKey; }
    const TrigConfChain& GetChain()   const { return fChain; }

    const TrigConfChain& GetInputL1() const { return fInputL1; }
    const TrigConfChain& GetInputL2() const { return fInputL2; }
   
    const RateGroup * GetOutputGroup() const {return fOutputGroup.get();}

    double   GetTotalPS()   const { return fTotalPS; }
    double   GetExpressPS() const { return fExpressPS; }
    unsigned GetCounter()   const { return fChain.getCounter(); }
    unsigned GetLevel()     const { return fChain.getLevelId(); }   

    bool IsMultiSeed() const { return fIsMultiSeed; }
    bool HasUnique()   const { return fStreamGroup.valid() && fCompliment.valid(); }
    bool IsPT()        const { return fIsPT; }
    
    double GetWeight()    const { return fWeight; }

    double GetEventsPassed()    const;
    double GetEventsPassedRaw() const;

    std::pair<double, double> GetPassFrac() const;
    std::pair<double, double> GetPassRate() const;

    std::pair<double, double> GetPassFracRaw() const;
    std::pair<double, double> GetPassRateRaw() const;

    std::pair<double, double> GetFracUnique() const;
    std::pair<double, double> GetRateUnique() const;

    const RateLumiMap& GetCounts() const { return fLumiMap; }

    void PrintRateResult(std::ostream &os = std::cout) const;

  private:

    void ResetConfig();
    
    LumiCount &GetLumi(unsigned run, unsigned lumi);

  private:

    // Global variables for total event counts
    unsigned            fPassD;          // Number of passed events - data count
    unsigned            fPassN;          // Number of passed events - raw count
    long double         fPassW;          // Number of events weighted
    long double         fPassW2;         // Number of events weighted squared 

    // Event variable(s):
    double              fWeight;         // Total event weight
    double              fWeightHLT;      // Event weight from L2 chain or L2+EF chains

    // Properties that do not change with events
    bool                fIsConfig;       // Is this object configured?
    bool                fIsMultiSeed;    // Does this chain have multiple seeds at L2 or EF?
    bool                fIsPT;           // This chain has pass-through >= 1
    bool                fIsRD0Seed;      // Does this chain seed from RD0
    bool                fIsRD1Seed;      // Does this chain seed from RD1
    std::string         fRateKey;        // Which type of rate calcuation should we use?

    double              fTotalPS;        // Total PS of all levels    
    double              fExpressPS;      // Express stream PS for EF chains
    double              fCommonScale;    // Common scale factor: converts fraction to rate
    double              fTotalEvents;    // Total number of events (denominator)
    double              f1EventError;    // Single event error

    // Objects
    TrigConfChain       fChain;          // Configuration for this chain (L1, L2 or EF)
    TrigConfChain       fInputL1;        // Input L1 chain
    TrigConfChain       fInputL2;        // Input L2 chain

    Handle<RateGroup>   fOutputGroup;    // Pointer to output group for L1 chains
    Handle<RateGroup>   fCompliment;     // Pointer to compliment of this chain
    Handle<RateGroup>   fLevelGroup;     // Pointer to total level rate group
    Handle<RateGroup>   fStreamGroup;    // Pointer to total level rate group

    // New rate calculation:
    const RateNode      *fNode;           // Root node for rate tree
    const RateNode      *fNodeMultiSeed;  // Root node for input group tree in multi-seeded case

    //
    // Count passed events by run and lumi blocks
    //    
    RateLumiVec        *fLumiVec;        // Number of events by lumi block
    RateLumiMap         fLumiMap;        // Lumi block counts by run number
    unsigned            fCurrentRun;     // Current run number    
  }; 


  //-----------------------------------------------------------------------------
  //
  // Count passed events for a single trigger 
  //
  inline void RateChain::PassEvent(const RateData &data, const RateBits &bits)
  {
    if(!fIsConfig || !fNode) {
      return;
    }
    
    //
    // Compute weight using tree based rate method
    //
    fWeight = fNode->ComputeWeight()*data.event_weight;
    
    if(fNodeMultiSeed) {
      fWeight *= fNodeMultiSeed->ComputeWeight();
    }

    // Apply weights for the Bunch group scaling
    if(fIsRD0Seed) { 
      fWeight *= data.event_scale_rd0; 
    }  
    else if(fIsRD1Seed) {
      fWeight *= data.event_scale_rd1; 
    }  
    else { 
      fWeight *= data.event_scale_lumi;
      // Apply weights for pileup scaling
      fWeight *= data.event_scale_pileup;
    }

    fPassN  += unsigned(fWeight > 0.0);
    fPassD  += bits.TestTriggerBit(fChain.getLevelId(), fChain.getCounter());
    fPassW  += fWeight;
    fPassW2 += fWeight*fWeight;
  }
  
  //
  // Find input chains
  //
  std::vector<TrigConfChain> GetInput(const TrigConfChain &chain,
				      const std::vector<TrigConfChain>::const_iterator ibeg,
				      const std::vector<TrigConfChain>::const_iterator iend);

  //
  // Helper functions for making lookup tables
  // 
  void ExpandVector(std::vector<RateChain *> &rvec, Handle<RateChain> rate, bool insert);

  // 
  // Inlined comparisons operators
  //
  inline bool operator==(const Handle<RateChain> &lhs, const Handle<RateChain> &rhs) {
    return lhs->GetName() == rhs->GetName();
  }
  inline bool operator <(const Handle<RateChain> &lhs, const Handle<RateChain> &rhs) {
    return lhs->GetName()  < rhs->GetName();
  }

  inline bool operator==(const Handle<RateChain> &elem, const std::string &name) {
    return elem->GetName() == name;
  }
  inline bool operator==(const std::string &name, const Handle<RateChain> &elem) {
    return elem->GetName() == name;
  }

  inline bool operator==(const Handle<RateChain> &elem, const TrigConfChain &conf) {
    return elem->GetName() == conf.getChainName();
  }
  inline bool operator==(const TrigConfChain &conf, const Handle<RateChain> &elem) {
    return elem->GetName() == conf.getChainName();
  }

  inline bool operator==(int lumi, const LumiCount &count) {
    return lumi == count.lumi_block;
  }
  inline bool operator==(const LumiCount &count, int lumi) {
    return lumi == count.lumi_block;
  }

  //
  // Helper structure to sort by total time, mean event time, etc
  //
  struct RateChainSort {

    RateChainSort() :fKey() {}
    explicit RateChainSort(const std::string &key) :fKey(key) {}

    bool operator()(const Anp::Handle<Anp::RateChain> &lhs,
		    const Anp::Handle<Anp::RateChain> &rhs);
    
    private:

    std::string fKey;
  };
}

#endif
