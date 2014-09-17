// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <assert.h>
#include <math.h>

// ROOT include(s):
#include <TError.h>
#include <TMath.h>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterBaseRates.h"
#include "../TrigCostRootAnalysis/CounterRatesUnion.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/RatesChainItem.h"


namespace TrigCostRootAnalysis {
  
  /**
   * Chain rates counter constructor. Pass data to base constructor. Also sets up the datastore for all Rates counters
   * @see CounterBase(const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel);
   */
  CounterBaseRates::CounterBaseRates( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel ) : 
    CounterBase(_costData, _name, _ID, _detailLevel),
    m_L2s(),
    m_L1s(),
    m_cannotCompute(kFALSE) {

      if (m_detailLevel == 0) m_dataStore.setHistogramming(kFALSE);

      m_dataStore.newVariable(kVarEventsPassed).setSavePerCall("Prescale Weighting;Combined Chain Weight;Events");
      m_dataStore.newVariable(kVarEventsPassedRaw).setSavePerCall();

      if (Config::config().getInt(kDirectlyApplyPrescales) == kTRUE) {
        m_dataStore.newVariable(kVarEventsPassedDP).setSavePerCall();
      }
  }
  
  /**
   * Counter base (rates) destructor. Nothing currently to delete.
   */
  CounterBaseRates::~CounterBaseRates() {
  }

  /**
   * Start of event processing. Unused with Rates counters
   */
  void CounterBaseRates::startEvent() {
    Error("CounterBaseRates::startEvent","Not expected to call this for a rates counter");
  }

    /**
   * Monitor rate of this chain
   * @param _e Chain index in D3PD.
   * @param _f unused
   * @param _weight Event weight.
   */
  void CounterBaseRates::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    ++m_calls;
    UNUSED(_e);
    UNUSED(_f);

    if (Config::config().getInt(kDirectlyApplyPrescales) == kTRUE) {
      Float_t _weightDirect = runDirect();
      if (!isZero( _weightDirect )) m_dataStore.store(kVarEventsPassedDP, _weightDirect, _weight);
    }

    Float_t _weightPS = runWeight();
    if (!isZero( _weightPS )) {
      m_dataStore.store(kVarEventsPassed, _weightPS, _weight);
    }

    if (getInEvent() == kTRUE) {
      m_dataStore.store(kVarEventsPassedRaw, runDirect(kFALSE), _weight);
    }

  }

  /**
   * End of event processing. Unused with Rates counters
   */
  void CounterBaseRates::endEvent() {
    Error("CounterBaseRates::endEvent","Not expected to call this for a rates counter");
  }

  /**
   * Get all L2 items which make up this combination
   * @return Set of pointers to L2 RatesChainItem(s) which know their L1 seeds
   */
  ChainItemSet_t& CounterBaseRates::getHLTItemSet() {
    return m_L2s;
  }

  /**
   * This function will return the prescale of a basic item. It is suppoed to be called on L1 or L2 chains
   * (not combinations of chains) and will return the prescale for just the top level.
   * @return Prescale of top level of basic chain
   */
  UInt_t CounterBaseRates::getBasicPrescale() {
    if (getStrDecoration(kDecType) == "L1") {
      if (m_L1s.size() != 1) {
        Warning("CounterBaseRates::getBasicPrescale", "Expected only 1x L1 chain but found %i.", (Int_t)m_L1s.size());
      }
      return (**m_L1s.begin()).getPS();
    } else if (getStrDecoration(kDecType) == "Chain") {
      if (m_L2s.size() != 1) {
        Warning("CounterBaseRates::getBasicPrescale", "Expected only 1x HLT chain but found %i.", (Int_t)m_L2s.size());
      }
      return (**m_L2s.begin()).getPS();
    } else {
      Warning("CounterBaseRates::getBasicPrescale", "Called on a CounterRates of type %s, this is only intented for basic chains. Returning 1.", getStrDecoration(kDecType).c_str());
    }
    return 1;
  }

  /**
   * @param _toAdd Add a L1 TriggerItem which is to be used by this rates counter.
   */
  void CounterBaseRates::addL1Item( RatesChainItem* _toAdd ) {
    m_L1s.insert( _toAdd );
  }

  /**
   * @param _toAdd Add a HLT TriggerItem which is to be used by this rates counter.
   */
  void CounterBaseRates::addHLTItem( RatesChainItem* _toAdd ) {
    // Perform Union check on number of L1 seeds
    if ( dynamic_cast<CounterRatesUnion*>(this) != NULL) { //If I am actually a CounterRatesUnion
      if (_toAdd->getLower().size() > (UInt_t) Config::config().getInt(kMaxMultiSeed)) {
        Warning("CounterBaseRates::addHLTItem","Not including %s in %s due to %u L1 seeds (see: --maxMultiSeed)",
          _toAdd->getName().c_str(), getName().c_str(), (UInt_t) _toAdd->getLower().size());
        return;
      }
    }
    m_L2s.insert( _toAdd );
    for (ChainItemSetIt_t _lower = _toAdd->getLowerStart(); _lower != _toAdd->getLowerEnd(); ++_lower) {
      m_L1s.insert( (*_lower) );
    }
  }

  /**
   * @param _toAdd Add multiple HLT TriggerItems which should be used by this rates counter
   */
  void CounterBaseRates::addHLTItems( ChainItemSet_t _toAdd ) {
    for (ChainItemSetIt_t _it = _toAdd.begin(); _it != _toAdd.end(); ++_it) {
      RatesChainItem* _L2 = (*_it);
      // Perform Union check on number of L1 seeds
      if ( dynamic_cast<CounterRatesUnion*>(this) != NULL) { //If I am actually a CounterRatesUnion
        if (_L2->getLower().size() > (UInt_t) Config::config().getInt(kMaxMultiSeed)) {
          Warning("CounterBaseRates::addHLTItem","Not including %s in %s due to %u L1 seeds (see: --maxMultiSeed)",
            _L2->getName().c_str(), getName().c_str(), (UInt_t) _L2->getLower().size());
          continue;
        }
      }
      m_L2s.insert( _L2 );
      for (ChainItemSetIt_t _lower = _L2->getLowerStart(); _lower != _L2->getLowerEnd(); ++_lower) {
        m_L1s.insert( (*_lower) );
      }
    }
  }


  /**
   * @param _overlap Pointer to another counter which overlaps with this one, used to get relative rates at the end of run.
   */
  void CounterBaseRates::addOverlap( CounterBase* _overlap ) {
    m_ovelapCounters.insert( _overlap );
  }


  /**
   * @return kTRUE if at least one HLT counter of this chain had info in the D3PD for this event.
   */
  Bool_t CounterBaseRates::getInEvent() {
    if (m_L2s.size() > 0) { // I'm a HLT Chain
      for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
        RatesChainItem* _L2 = (*_L2It);
        if (_L2->getInEvent() == kTRUE) return kTRUE;
      }
    } else if (m_L1s.size() > 0) { // I'm a L1 Chain
      // By definition - L1 chains are always active!
      return kTRUE;
    }
    return kFALSE;
  }

  /**
   * End of run, use stored information to build the overlap histogram
   */
  void CounterBaseRates::finalise() {
    //Check compatability of both methods
    Float_t _chainPasses   = m_dataStore.getValue(kVarEventsPassed,   kSavePerCall);
    Float_t _chainPassesDP = m_dataStore.getValue(kVarEventsPassedDP, kSavePerCall);
    Float_t _sig = TMath::Sqrt(TMath::Abs(_chainPassesDP - _chainPasses))/TMath::Max(_chainPassesDP,_chainPasses);
    if (_sig > 3 && !isZero(_chainPassesDP)) {
      Warning("CounterBaseRates::finalise","Chain %s of %s has Weight:%.4f DP:%.4f MethodDeviation:%.2f sigma! (PS: %s)", 
        getName().c_str(), getStrDecoration(kDecRatesGroupName).c_str(), 
        _chainPasses,
        _chainPassesDP,
        _sig,
        getStrDecoration(kDecPrescale).c_str());
    }

    // Next step is only for chain counters
    if (getStrDecoration(kDecType) != "Chain") return;
    if (isZero(_chainPasses)) return;
    if (m_ovelapCounters.size() <= 1) return; // Bug - doesn't work for one overlap counter strangely

    // Get the % overlap with each other counter
    std::vector< Float_t > _binValues;
    Bool_t _allZero = kTRUE;
    for (CounterSetIt_t _it = m_ovelapCounters.begin(); _it != m_ovelapCounters.end(); ++_it) {
      CounterBase* _overlapCounter = (*_it);
      Float_t _overlapPasses = _overlapCounter->getValue(kVarEventsPassed, kSavePerCall);
      Float_t _overlapPercentage = 100. * ( _overlapPasses / _chainPasses  ) ;
      if (_overlapPercentage != _overlapPercentage || isinf(_overlapPercentage)) {
        Error("CounterBaseRates::finalise","%s vs. %s propagated an inf. [Weight]", getName().c_str(), _overlapCounter->getName().c_str());
        _binValues.push_back( 0. );
      } else {
        _binValues.push_back( _overlapPercentage );
        if (!isZero(_overlapPercentage)) _allZero = kFALSE;
      }
    }
    if (_allZero == kTRUE) return;

    // Get the names of the other counters, use trickery on the name of the overlap counter
    std::vector<std::string> _chainNames;
    for (CounterSetIt_t _it = m_ovelapCounters.begin(); _it != m_ovelapCounters.end(); ++_it) {
      CounterBase* _overlapCounter = (*_it);
      // Get the name of the other chain from the name of the AND counter
      std::string _otherName = _overlapCounter->getName();
      _otherName.erase( _otherName.find(Config::config().getStr(kAndString)), Config::config().getStr(kAndString).size());
      _otherName.erase( _otherName.find(getName()), getName().size());
      _chainNames.push_back( _otherName );
    }

    m_dataStore.newVariable(kVarOverlap).setSavePerCall("Chain Overlap Within Rates Group;Chain;Overlap [%]");
    m_dataStore.setBinLabels(kVarOverlap, kSavePerCall, _chainNames); // Set the bins to have this name
    for (size_t _i = 1; _i <= _binValues.size(); ++_i) {
      m_dataStore.store(kVarOverlap, _i, _binValues.at( _i - 1));  // Use the weight to set the effective value
      if (_binValues.at( _i - 1) >= Config::config().getInt(kRatesOverlapWarning)) {
        Warning("CounterBaseRates::finalise", "Chain %s overlaps %.1f%% with %s", getName().c_str(), _binValues.at( _i - 1), _chainNames.at(_i - 1).c_str());
      }
    }

  }
  
} // namespace TrigCostRootAnalysis
