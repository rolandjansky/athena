/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_PREDRATES_H
#define ANP_PREDRATES_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : PredRates
 * @Author : Rustem Ospanov
 *
 * @Brief  : A first draft for algorithm for online rate prediction
 * 
 **********************************************************************************/

// C/C++
#include <string>

// Local
#include "TrigCostBase/AlgEvent.h"

// Local
#include "TrigCostRate/RateABC.h"

namespace Anp
{
  class PredRates: public virtual AlgEvent, public virtual RateABC {
  public:

    PredRates();
    virtual ~PredRates();
    
    void ConfigReg(const Registry &reg);

    bool Init();

    void Exec();

    void Done();

    std::vector<Handle<RateChain> >& GetRateChains() { return fChain; }
    std::vector<Handle<RateChain> >& GetPTOnlyRate() { return fChain; }
    std::vector<Handle<RateChain> >& GetMultiSeeds() { return fChainMultiSeed; }
    std::vector<Handle<RateGroup> >& GetRateGroups() { return fGroup; }
    std::vector<Handle<RateGroup> >& GetRateUnions() { return fUnion; }
    std::vector<Handle<RateGroup> >& GetRateCumuls() { return fCumul; }
    std::vector<Handle<RateGroup> >& GetRateLevels() { return fLevel; }
    std::vector<Handle<RateGroup> >& GetRateStream() { return fStream; }

    const std::vector<Anp::RateChain *>& GetActiveChains() const { return fActiveChains; }
    const std::vector<Anp::RateChain *>& GetAlwaysChains() const { return fAlwaysChains; } 

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    TrigMonL1Item::Decision GetDecisionBitLV1() const { return fDecisionBitLV1; }
    TrigMonChain::Decision  GetDecisionBitHLT() const { return fDecisionBitHLT; }

    double GetScale() const { return fScale; }
    double GetTotal() const { return fTotal; }

  public:

    typedef std::vector<Anp::Handle<Anp::RateChain> > ChainVec;
    typedef std::vector<Anp::Handle<Anp::RateGroup> > GroupVec;
    
    struct Groups {
      Groups() :group1(0), group2(0), groupE(0) {}
      Groups(RateGroup *g1, RateGroup *g2, RateGroup *ge) :
	group1(g1), group2(g2), groupE(ge) {}

      RateGroup *group1;
      RateGroup *group2;
      RateGroup *groupE;
    };

    struct Chains {
      Chains() :chain1(0), chain2(0), chainE(0) {}
      Chains(RateChain *c1, RateChain *c2, RateChain *ce)
	: chain1(c1), chain2(c2), chainE(ce) {}

      unsigned   counter;
      RateChain *chain1;
      RateChain *chain2;
      RateChain *chainE;
    };

  private:
    
    void ProcessConfig();

    void Compute(const std::vector<RateChain *> &act1,
		 const std::vector<RateChain *> &act2);

    void MakeChains(const ChainVec &vec1, const ChainVec &vec2);
    void MakeGroups(const GroupVec &vec1, const GroupVec &vec2, const std::string &key);

    Handle<RateChain> MakeChain(const Handle<RateChain> &r1, const Handle<RateChain> &r2) const;
    Handle<RateGroup> MakeGroup(const Handle<RateGroup> &r1, const Handle<RateGroup> &r2) const;

  private:

    Anp::Registry               fReg;            // Registry configuration
    Anp::Registry               fRegIndex;       // Registry configuration for index page

    Branch<TrigMonEvent>        fEntry;          // Trigger cost monitoring data for one event

    // Properties:
    bool                        fDebug;          // Print debugging info
    bool                        fPrint;          // Print summary
    std::string                 fName1;          // MakeRates algorithm for actual rates
    std::string                 fName2;          // MakeRates algorithm for extrapolated rates

    // Variables:
    bool                        fIsConfig;       // Is class properly configured?
    double                      fTotal;          // Total number of events
    double                      fScale;          // Scale factor used to convert fraction to rate
    long                        fCountPass;      // Count events which passed accepted trigger(s)
    long                        fCountFail;      // Count events which failed accepted trigger(s)
    long                        fCountPTChange;  // Count PT changes between two algs

    TrigMonL1Item::Decision     fDecisionBitLV1; // Decision bit used for LV1 rates
    TrigMonChain::Decision      fDecisionBitHLT; // Decision bit used for HLT rates

    ChainVec                    fChain;          // List of trigger chains
    ChainVec                    fChainMultiSeed; // List of trigger chains
    ChainVec                    fMarks;          // List of trigger chains

    GroupVec                    fGroup;          // List of configured trigger groups
    GroupVec                    fStream;         // List of stream     trigger groups
    GroupVec                    fLevel;          // List of level      trigger groups
    GroupVec                    fCompl;          // List of compliment trigger groups
    GroupVec                    fCumul;          // List of cumulative trigger groups
    GroupVec                    fUnion;          // List of unions of trigger group pairs

    Handle<RateABC>             fRate1;          // Algorithm for actual rates
    Handle<RateABC>             fRate2;          // Algorithm for extrapolated rates
    
    std::vector<Chains>         fLookupL1;       // L1 chains for fast lookup by counter
    std::vector<Chains>         fLookupL2;       // L2 chains for fast lookup by counter
    std::vector<Chains>         fLookupEF;       // EF chains for fast lookup by counter

    std::vector<Groups>         fLookupGroups;   // Vector of all groups: 1, 2 and this class

    std::vector<RateChain *>    fActiveChains;   // Current active non-special chains
    std::vector<RateChain *>    fAlwaysChains;   // Special case: chains with PassThrough >= 1.0
  };

  //
  // Helper functions for making lookup tables
  // 
  void ExpandVector(std::vector<PredRates::Chains> &vec, const PredRates::Chains &c);
}

#endif
