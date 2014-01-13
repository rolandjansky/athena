/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_MAKERATES_H
#define ANP_MAKERATES_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : MakeRates
 * @Author : Rustem Ospanov, Josh Kunkle, Elliot Lipeles
 *
 * @Brief  : Algorithm for computing trigger rates
 *
 * Algorithm for rates computation
 *  - collect passed chains for event
 *  - create RateChain and RateGroup (rates) objects
 *  - pass trigger decision to rates objects
 *  - provide rates data to web pages via RateABC interface
 *  - can be executed as sub-algorithm of MakePuRates
 *
 **********************************************************************************/

// C/C++
#include <set>
#include <string>

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/ScaleABC.h"

// Local
#include "TrigCostRate/RateABC.h"
#include "TrigCostRate/SimlABC.h"
#include "TrigCostRate/MakeNodes.h"
#include "TrigCostRate/RateChain.h"
#include "TrigCostRate/RateGroup.h"
#include "TrigCostRate/RateBand.h"

namespace Anp
{
  class MakeRates: public virtual AlgEvent, public virtual RateABC {
  public:

    MakeRates();
    virtual ~MakeRates();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Exec();

    void Done();

    void Set(const TrigMonConfig &config);
    void Run(const TrigMonConfig &config);
    void Run(const TrigMonEvent  &event);

    std::vector<Handle<RateChain> >& GetRateChains() { return fChain; }
    std::vector<Handle<RateChain> >& GetPTOnlyRate() { return fChainPT; }
    std::vector<Handle<RateChain> >& GetMultiSeeds() { return fMultiSeedChain; }
    std::vector<Handle<RateGroup> >& GetRateGroups() { return fGroup; }
    std::vector<Handle<RateGroup> >& GetRateUnions() { return fUnion; }
    std::vector<Handle<RateGroup> >& GetRateCumuls() { return fCumul; }
    std::vector<Handle<RateGroup> >& GetRateLevels() { return fLevel; }
    std::vector<Handle<RateGroup> >& GetRateStream() { return fStream; }

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    Handle<RateBand> GetRateBand() const { return fBand; }

    TrigMonL1Item::Decision GetDecisionBitLV1() const { return fDecisionBitLV1; }
    TrigMonChain::Decision  GetDecisionBitHLT() const { return fDecisionBitHLT; }

    double GetScale() const { return fScale; }
    double GetTotal() const { return fTotal; }

    std::string GetPhysicsStream(const Handle<RateChain> chain) const;
    std::string GetLv1RandomSeed(const TrigMonConfig &config, 
				 const TrigConfChain &chain, 
				 const std::vector<TrigConfChain> &config_chains) const;

  private:

    typedef std::vector<Anp::Handle<Anp::SimlABC> >   SimulVec;
    typedef std::vector<Anp::Handle<Anp::RateChain> > ChainVec;
    typedef std::vector<Anp::Handle<Anp::RateGroup> > GroupVec;
    typedef std::vector<Handle<ScaleABC> >            ScaleVec;

  private:

    Handle<RateGroup> MakeGroup(const std::string &name, const std::string &type,
				std::vector<Handle<RateChain> > chains);

    Handle<RateChain> MakeChain(const TrigConfChain &chain, 
				const std::vector<TrigConfChain> &config_chains,
				const std::string &opt = "");

    Handle<RateChain> MakeMultiSeed(const TrigConfChain &chain, 
				    const std::vector<TrigConfChain> &multi_seeds,
				    const std::vector<TrigConfChain> &conf_chains,
				    const std::string &opt = "");
    
    Handle<RateGroup> MakeExpressStream(const std::string &name, 
					std::vector<Handle<RateChain> > chains,
					const std::vector<TrigConfChain> &config_chains);

    void ReadConfig(const TrigMonConfig &config,
		    std::vector<TrigConfChain> &config_chains,
		    std::vector<TrigConfChain> &multis_chains,
		    std::vector<TrigConfChain> &ignore_chains);

    bool Ignore(const TrigConfChain &confg) const;

    Handle<RateGroup> MakeCompliment(Handle<RateChain> chain);
    Handle<RateGroup> MakeCompliment(Handle<RateGroup> group);
    Handle<RateGroup> MakeUnion(Handle<RateGroup> g1, Handle<RateGroup> g2);
    
    void PrepareGroups();
    
    void AddActiveChain(unsigned level, unsigned counter);

  private:

    Anp::Registry               fReg;            // Registry configuration
    Anp::Registry               fRegIndex;       // Registry configuration for index page

    Branch<TrigMonEvent>        fEntry;          // Trigger cost monitoring data for one event
    
    // Properties:
    bool                        fDebug;          // Print debugging info
    bool                        fIsData;         // Are we using data events?
    bool                        fIsSubAlg;       // Is sub-algorithm? Parent will manage setup

    bool                        fDoBandwidth;    // Compute bandwidth groups
    bool                        fDoGroups;       // Compute rates for groups
    bool                        fDoOverlap;      // Compute group overlaps
    bool                        fDoUnique;       // Compute unique rates

    std::string                 fLumiNameAlg;    // Name of luminosity algorithm
    std::vector<std::string>    fIgnore;         // Ignore chains on this list

    // Variables:
    bool                        fIsConfig;       // Is class properly configured?
    double                      fTotal;          // Total number of events
    double                      fScale;          // Scale factor used to convert fraction to rate
    long                        fCountPass;      // Count events which passed accepted trigger(s)
    long                        fCountFailL1;    // Count events which failed accepted trigger(s)
    long                        fCountFailL2;    // Count events which failed accepted trigger(s)
    long                        fCountFailEF;    // Count events which failed accepted trigger(s)

    TrigMonL1Item::Decision     fDecisionBitLV1; // Decision bit used for LV1 rates
    TrigMonChain::Decision      fDecisionBitHLT; // Decision bit used for HLT rates
    
    ChainVec                    fChain;          // List of all menu    RateChain objects
    ChainVec                    fChainPT;        // List of all PT only RateChain objects
    ChainVec                    fAllChain;       // List of all created RateChain objects
    ChainVec                    fMultiSeedChain; // List of all created RateChain objects

    GroupVec                    fGroup;          // List of configured trigger groups
    GroupVec                    fOwnGroup;       // List of created and owned trigger groups 
    GroupVec                    fStream;         // List of stream     trigger groups
    GroupVec                    fLevel;          // List of level      trigger groups
    GroupVec                    fCompl;          // List of compliment trigger groups
    GroupVec                    fCumul;          // List of cumulative trigger groups
    GroupVec                    fUnion;          // List of unions of trigger group pairs

    SimulVec                    fSimulVec;       // List of trigger simulation objects
    ScaleVec                    fScaleVec;       // Scaling algorithm(s) for event by event scaling

    Handle<RateBand>            fBand;           // Bandwidth calculator
    Handle<ConfigABC>           fConf;           // Configuration algorithm

    // Data:
    std::vector<RateChain *>    fActiveChains;   // Current active non-special chains
    std::vector<RateChain *>    fAlwaysChains;   // Special case: chains with PassThrough >= 1.0

    std::vector<RateChain *>    fLookupL1;       // L1 chains for fast lookup by counter
    std::vector<RateChain *>    fLookupL2;       // L2 chains for fast lookup by counter
    std::vector<RateChain *>    fLookupEF;       // EF chains for fast lookup by counter
    std::vector<RateChain *>    fLookupES;       // EF chains for fast lookup by counter

    std::vector<RateGroup *>    fAllGroup;       // Pointer to all groups
    
    RateData                    fEventData;      // Current event data
    RateBits                    fEventBits;      // Current event trigger bits

    // Structures for new tree-based rate calculation:
    MakeNodes                   fNode;           // Factory for nodes for regular triggers
    MakeNodes                   fNodeMS;         // Factory for nodes for multi-seeded triggers
  };

  //
  // Inlined functions:
  //
  inline void MakeRates::AddActiveChain(unsigned level, unsigned counter) 
  {
    RateChain *chain = 0;
    if     (level == 1 && counter < fLookupL1.size()) chain = fLookupL1.at(counter);
    else if(level == 2 && counter < fLookupL2.size()) chain = fLookupL2.at(counter);
    else if(level == 3 && counter < fLookupEF.size()) chain = fLookupEF.at(counter);
    
    if(chain) {
      fActiveChains.push_back(chain);
    }
    else {
      if(fDebug) {
        log() << "AddActiveChain - uknown level, counter: " << level << ", " << counter << std::endl;
      }
    }
    
    fEventBits.SetTriggerBit(level, counter);
    
    if(level == 3 && counter < fLookupES.size()) {
      RateChain *chainES = fLookupES.at(counter);
      if(chainES) {
	fActiveChains.push_back(chainES);
      }
    }
  }
}

#endif
