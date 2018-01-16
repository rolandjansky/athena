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
#include "../TrigCostRootAnalysis/TrigXMLService.h"
#include "../TrigCostRootAnalysis/RatesCPSGroup.h"


namespace TrigCostRootAnalysis {
  /**
   * Chain rates counter constructor. Pass data to base constructor. Also sets up the datastore for all Rates counters
   * @see CounterBase(const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel);
   */
  CounterBaseRates::CounterBaseRates(const TrigCostData* _costData, const std::string& _name, Int_t _ID,
                                     UInt_t _detailLevel, MonitorBase* _parent) :
    CounterBase(_costData, _name, _ID, _detailLevel, _parent),
    m_L2s(),
    m_L1s(),
    m_cannotCompute(kFALSE),
    m_myUniqueCounter(nullptr),
    m_globalRates(nullptr),
    m_lowerRates(nullptr),
    m_doSacleByPS(0),
    m_doDirectPS(0),
    m_cachedWeight(0),
    m_alwaysDoExpressPS(kFALSE),
    m_eventLumiExtrapolation(0.) {
    if (m_detailLevel == 0) {
      m_dataStore.setHistogramming(kFALSE);
      m_disableEventLumiExtrapolation = kFALSE;
    } else { // this is UPGRADE RATES MODE
      m_dataStore.newVariable(kVarJetEta).setSavePerCall("Jet ROI #eta Distribution;#eta;ROIs");
      m_dataStore.newVariable(kVarMuEta).setSavePerCall("Muon ROI #eta Distribution;#eta;ROIs");
      m_dataStore.newVariable(kVarEmEta).setSavePerCall("Electron ROI #eta Distribution;#eta;ROIs");
      m_dataStore.newVariable(kVarTauEta).setSavePerCall("Tau ROI #eta Distribution;#eta;ROIs");

      m_dataStore.newVariable(kVarJetNThresh).setSavePerEvent("Jet ROI N Pass Threshold;N RoIs;Events");
      m_dataStore.newVariable(kVarMuNThresh).setSavePerEvent("Muon ROI N Pass Threshold;N RoIs;Events");
      m_dataStore.newVariable(kVarEmNThresh).setSavePerEvent("Electron ROI N Pass Threshold;N RoIs;Events");
      m_dataStore.newVariable(kVarTauNThresh).setSavePerEvent("Tau ROI N Pass Threshold;N RoIs;Events");

      m_dataStore.newVariable(kVarMu).setSavePerEvent("Obtained <mu> Profile;<mu>;Events");
      m_dataStore.newVariable(kVarBunchWeight).setSavePerEvent("Number of Bunch Extrapolation;Bunch Weight;Events");


      m_disableEventLumiExtrapolation = kTRUE;
    }

    m_doSacleByPS = Config::config().getInt(kRatesScaleByPS);
    m_doDirectPS = Config::config().getInt(kDirectlyApplyPrescales);
    decorate(kDecDoExpressChain, 0); // this will be overwritten as needed

    if (m_doDirectPS == kTRUE) m_dataStore.newVariable(kVarEventsPassedDP).setSavePerCall();
    m_dataStore.newVariable(kVarEventsPassedExpress).setSavePerCall();
    m_dataStore.newVariable(kVarUnbiasedPassed).setSavePerCall();
    m_dataStore.newVariable(kVarUnbiasedRun).setSavePerCall();
    m_dataStore.newVariable(kVarEventsPassed).setSavePerCall();
    m_dataStore.newVariable(kVarEventsPassedNoPS).setSavePerCall();
    m_dataStore.newVariable(kVarEventsRun).setSavePerCall();
    m_dataStore.newVariable(kVarEventsPassRawStat).setSavePerCall();
    m_dataStore.newVariable(kVarEventsRunRawStat).setSavePerCall();

    if (_name == Config::config().getStr(kRateExpressString)) m_alwaysDoExpressPS = kTRUE;
    //m_dataStore.newVariable(kVarEventsPerLumiblock).setSavePerCall("Rate Per Lumi Block;Lumi Block;Rate [Hz]");
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
    Error("CounterBaseRates::startEvent", "Not expected to call this for a rates counter");
  }

  /**
   * Monitor rate of this chain
   * @param _e Unused
   * @param _f Unused
   * @param _weight Event weight.
   */
  void CounterBaseRates::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    UNUSED(_e);
    UNUSED(_f);
    ++m_calls;

    if (getInEvent() == kFALSE) return;

    // Reminder - _weight here is the enhanced bias event weight. May be already multiplied up by the basic weight (if
    // not the default value of 1)

    // Optional - when not doin EB weighting. We can scale up individual chains by their L1 PS to get the effective PS=1
    // rates.
    // Does not work on combinations!
    Float_t _scaleByPS = 1.;
    if (m_doSacleByPS && (getStrDecoration(kDecType) == "Chain" || getStrDecoration(kDecType) == "L1")) {
      _scaleByPS = getDecoration(kDecPrescaleValOnlineL1);
    }

    // WEIGHTED Prescale
    Double_t _weightPS = runWeight(m_alwaysDoExpressPS); // alwaysDoExpressPS is *only* for the express group
    //m_eventLumiExtrapolation is calculated by runWeight()
    if (m_disableEventLumiExtrapolation) m_eventLumiExtrapolation = 1;

    if (!isZero(_weightPS)) {
      m_dataStore.store(kVarEventsPassed, 1., _weightPS * _weight * m_eventLumiExtrapolation * _scaleByPS);
      // Chain passes with weight from PS as a float 0-1. All other weights  inc. 
      // m_dataStore.store(kVarEventsPerLumiblock, m_costData->getLumi(), (_weightPS * _weight *
      // _scaleByPS)/((Float_t)m_costData->getLumiLength()) );
      if (m_detailLevel > 0 && m_L1s.size() == 1) (**m_L1s.begin()).fillHistograms(m_dataStore,
                                                                                   _weightPS * _weight * m_eventLumiExtrapolation * _scaleByPS,
                                                                                   static_cast<MonitorRatesUpgrade*>(
                                                                                     m_parent)->getCollidingBunchFactor());

    }

    // DIRECT Prescale
    if (m_doDirectPS == kTRUE) {
      Float_t _weightDirect = runDirect();
      if (!isZero(_weightDirect)) m_dataStore.store(kVarEventsPassedDP, 1.,
                                                    _weightDirect * _weight * m_eventLumiExtrapolation * _scaleByPS);                                                                                                                                                   // Chain
      // passes with weight from PS either 0 or 1. All other weights inc.
    }

    if (getIntDecoration(kDecDoExpressChain) == 1) {
      Double_t _weightPSExpress = runWeight(/*doExpress =*/ kTRUE);
      if (!isZero(_weightPSExpress)) m_dataStore.store(kVarEventsPassedExpress, 1.,
                                                       _weightPSExpress * _weight * m_eventLumiExtrapolation *
        _scaleByPS);
    }

    Float_t _passBeforePS = runDirect(/*usePrescales =*/ kFALSE);
    m_dataStore.store(kVarEventsPassedNoPS, 1., _passBeforePS * _weight * m_eventLumiExtrapolation * _scaleByPS);
    // Times chain passes, irrespective of any PS/ All other wieghts inc.
    m_dataStore.store(kVarEventsRun, 1., _weight * m_eventLumiExtrapolation * _scaleByPS); // Times chain is processed,
                                                                                           // regardless of decision.
                                                                                           // All other weights inc.
    m_dataStore.store(kVarEventsPassRawStat, 1., _passBeforePS); // Times chain passes, zero other weights (underlying
                                                                 // statistics of input file)
    m_dataStore.store(kVarEventsRunRawStat, 1.); // Times chain is processed, regardless of decision, zero other weights
                                                 // (underlying statistics of input file).
  }

  /**
   * The Rates counters have a much more sophisticated prescale treatment and hence do not use this function.
   * @return 0 for rates counters, prints error
   */
  Double_t CounterBaseRates::getPrescaleFactor(UInt_t _e) {
    UNUSED(_e);
    Error("CounterBaseRates::getPrescaleFactor",
          "This function is not defined for a Rates Counter, see the Rates base class.");
    return 0.;
  }

  /**
   * End of event processing. Unused with Rates counters
   */
  void CounterBaseRates::endEvent(Float_t _weight) {
    UNUSED(_weight);
    if (m_detailLevel == 0) Error("CounterBaseRates::endEvent", "Not expected to call this for a rates counter with detail level = 0 (no histograms)");
    m_dataStore.endEvent();
  }

  /**
   * Get all L3 items which make up this combination
   * @return Set of pointers to L3 RatesChainItem(s) which know their L2 seeds
   */
  ChainItemSet_t& CounterBaseRates::getL3ItemSet() {
    return m_L3s;
  }

  /**
   * Get all L2 items which make up this combination
   * @return Set of pointers to L2 RatesChainItem(s) which know their L1 seeds
   */
  ChainItemSet_t& CounterBaseRates::getL2ItemSet() {
    return m_L2s;
  }

  /**
   * Get all L1 items which make up this combination
   * @return Set of pointers to L1 RatesChainItem(s) which know their HLT items
   */
  ChainItemSet_t& CounterBaseRates::getL1ItemSet() {
    return m_L1s;
  }

  /**
   * Get all CPS groupings assigned to this combination
   * @return Set of pointers to CPS groups
   */
  CPSGroupSet_t& CounterBaseRates::getCPSGroupSet() {
    return m_cpsGroups;
  }

  /**
   * This function will return the prescale of a basic item. It is supposed to be called on L1 or L2 chains
   * (not combinations of chains) and will return the prescale for just the top level.
   * @return Prescale of top level of basic chain
   */
  UInt_t CounterBaseRates::getBasicPrescale() {
    if (getStrDecoration(kDecType) == "L1") {
      if (m_L1s.size() != 1) {
        Warning("CounterBaseRates::getBasicPrescale", "Expected only 1x L1 chain but found %i.", (Int_t) m_L1s.size());
      }
      return (**m_L1s.begin()).getPS();
    } else if (getStrDecoration(kDecType) == "Chain" || getStrDecoration(kDecType) == "L2") {
      if (m_L2s.size() != 1) {
        Warning("CounterBaseRates::getBasicPrescale", "Expected only 1x L2 chain but found %i.", (Int_t) m_L2s.size());
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
  void CounterBaseRates::addL1Item(RatesChainItem* _toAdd) {
    assert(_toAdd != nullptr);
    m_L1s.insert(_toAdd);
    // Add back-link
    _toAdd->addCounter(this);
  }

  /**
   * @param _toAdd Add a grouping of items in a CPS.
   * @param _name The name of the chain in this CPS group which is to be active in this counter
   */
  void CounterBaseRates::addCPSItem(RatesCPSGroup* _toAdd, std::string _name) {
    if (_toAdd != nullptr) m_cpsGroups.insert(_toAdd);
    m_myCPSChains.insert(_name);
  }

  /**
   * @return Last calculated weight (cached).
   */
  Double_t CounterBaseRates::getLastWeight() {
    return m_cachedWeight;
  }

  /**
   * We risk poisoning the GLOBAL rate counters if we add chains which are multi-seeded from > 1 L1 item
   * This is because they will no doubt loose the "AllOneToMany" topology and become too slow to run
   * By excluding these here, we loose their contributions from the global rate.
   * @return If the L2 item should be excluded at this early stage.
   */
  Bool_t CounterBaseRates::checkMultiSeed(RatesChainItem* _toAdd) {
    // Perform Union check on number of L1 seeds
    if (dynamic_cast<CounterRatesUnion*>(this) != NULL) { //If I am actually a CounterRatesUnion
      if (_toAdd->getLower().size() > (UInt_t) Config::config().getInt(kMaxMultiSeed)) {
        // We are much stricter if this is a GLOBAL counter - don't want to have to disable it
        // HACK - we don't want to vero MU20||MU21 (June 17)
        if (_toAdd->getLower().size() == 2) { // TODO this is temporary for July 2017
          bool passesHack = kTRUE;
          for (ChainItemSetIt_t _L1Hack = _toAdd->getLower().begin(); _L1Hack != _toAdd->getLower().end(); ++_L1Hack) {
            if ((*_L1Hack)->getName() == "L1_MU20") continue;
            else if ((*_L1Hack)->getName() == "L1_MU21") continue;
            passesHack = kFALSE;
          }
          if (passesHack) return kTRUE;
        }
        Bool_t _isGlobal = kFALSE;
        if (getName() == Config::config().getStr(kRateGlobalL1String)) _isGlobal = kTRUE;
        else if (getName() == Config::config().getStr(kRateGlobalHLTString)) _isGlobal = kTRUE;
        else if (getName() == Config::config().getStr(kRateGlobalPhysicsMainString)) _isGlobal = kTRUE;
        else if (getName() == Config::config().getStr(kRateExpressString)) _isGlobal = kTRUE;
        else if (getStrDecoration(kDecType) == "UniqueHLT") _isGlobal = kTRUE;
        if (_isGlobal) {
          // We only warn once per chain
          static std::set<std::string> _toWarnAbout;
          if (_toWarnAbout.count(_toAdd->getName()) == 0) {
            _toWarnAbout.insert(_toAdd->getName());
            Warning("CounterBaseRates::checkMultiSeed", "Not including %s in RATE_GLOBAL/EXSPRESS/UNIQUE calculations due to %u L1 seeds.",
                    _toAdd->getName().c_str(), (UInt_t) _toAdd->getLower().size());
          }
          return kFALSE;
        }
      }
    }
    return kTRUE;
  }

  /**
   * @param _toAdd Add a HLT TriggerItem which is to be used by this rates counter.
   */
  void CounterBaseRates::addL2Item(RatesChainItem* _toAdd) {
    assert(_toAdd != nullptr);
    if (checkMultiSeed(_toAdd) == kFALSE) return;

    m_L2s.insert(_toAdd);
    _toAdd->addCounter(this); // Add back-link
    for (ChainItemSetIt_t _lower = _toAdd->getLowerStart(); _lower != _toAdd->getLowerEnd(); ++_lower) {
      assert((*_lower) != nullptr);
      m_L1s.insert((*_lower));
      //TODO do i need to add this counter to the L1s here? Don't think so
    }
  }

  /**
   * @param _toAdd Add multiple HLT TriggerItems which should be used by this rates counter
   */
  void CounterBaseRates::addL2Items(ChainItemSet_t _toAdd) {
    for (ChainItemSetIt_t _it = _toAdd.begin(); _it != _toAdd.end(); ++_it) {
      RatesChainItem* _L2 = (*_it);
      // Perform Union check on number of L1 seeds
      if (checkMultiSeed(_L2) == kFALSE) continue;
      m_L2s.insert(_L2);
      _L2->addCounter(this); // Add back-link
      for (ChainItemSetIt_t _lower = _L2->getLowerStart(); _lower != _L2->getLowerEnd(); ++_lower) {
        m_L1s.insert((*_lower));
      }
    }
  }

  /**
   * @param _toAdd Add a L3 item, for upgrade this will be the new HLT
   */
  void CounterBaseRates::addL3Item(RatesChainItem* _toAdd) {
    m_L3s.insert(_toAdd);
    _toAdd->addCounter(this); // Add back-link
    for (ChainItemSetIt_t _lower = _toAdd->getLowerStart(); _lower != _toAdd->getLowerEnd(); ++_lower) {
      m_L2s.insert((*_lower));
      for (ChainItemSetIt_t _lowerLower = (*_lower)->getLowerStart(); _lowerLower != (*_lower)->getLowerEnd(); ++_lowerLower) {
        m_L1s.insert((*_lowerLower));
      }
    }
  }

  /**
   * @param _toAdd Add many L3 items, for upgrade this will be the new HLT
   */
  void CounterBaseRates::addL3Items(ChainItemSet_t _toAdd) {
    for (auto _item : _toAdd) {
      addL3Item(_item);
    }
  }

  /**
   * @param _overlap Pointer to another counter which overlaps with this one, used to get relative rates at the end of
   *run.
   */
  void CounterBaseRates::addOverlap(CounterBase* _overlap) {
    m_ovelapCounters.insert(_overlap);
  }

  /**
   * @return kTRUE if at least one HLT counter of this chain had info in the D3PD for this event or one L1 counter of an
   *L1 chain was in the bunch group.
   */
  Bool_t CounterBaseRates::getInEvent() {
    if (m_L2s.size() > 0 || m_cpsGroups.size() > 0) { // I'm a HLT Chain(s)
      // Check my individual chains
      for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
        if ((*_L2It)->getInEvent() == kTRUE) return kTRUE;
      }
      // Chekc any CPS groups of chains I may have
      for (CPSGroupSetIt_t _CPSIt = m_cpsGroups.begin(); _CPSIt != m_cpsGroups.end(); ++_CPSIt) {
        RatesCPSGroup* _cpsGroup = (*_CPSIt);
        for (ChainItemSetIt_t _it = _cpsGroup->getChainStart(); _it != _cpsGroup->getChainEnd(); ++_it) {
          if ((*_it)->getInEvent() == kTRUE) return kTRUE;
        }
      }
    } else if (m_L1s.size() > 0) { // I'm a L1 Chain(s)
      // Still need to check that the L1 chain is active for this bunch group
      for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
        if ((*_L1It)->getInEvent() == kTRUE) return kTRUE;
      }
    } else if (m_L3s.size() > 0) { // I'm a Upgrade HLT Chain(s)
      for (ChainItemSetIt_t _L3It = m_L3s.begin(); _L3It != m_L3s.end(); ++_L3It) {
        if ((*_L3It)->getInEvent() == kTRUE) return kTRUE;
      }
    }
    return kFALSE;
  }

  /**
   * End of run, use stored information to build the overlap histogram
   */
  void CounterBaseRates::finalise() {
    //Info("CounterBaseRates::finalise","Finalise chain: %s", getName().c_str() );


    //Check compatability of both methods
    Float_t _chainPasses = m_dataStore.getValue(kVarEventsPassed, kSavePerCall);
    Float_t _chainPassesDP = m_dataStore.getValue(kVarEventsPassedDP, kSavePerCall);
    Float_t _sig = TMath::Sqrt(TMath::Abs(_chainPassesDP - _chainPasses)) / TMath::Max(_chainPassesDP, _chainPasses);

    if (_sig > 3 && !isZero(_chainPassesDP)) {
      Warning("CounterBaseRates::finalise", "Chain %s of %s has Weight:%.4f DP:%.4f MethodDeviation:%.2f sigma! (PS: %s)",
              getName().c_str(), getStrDecoration(kDecRatesGroupName).c_str(),
              _chainPasses,
              _chainPassesDP,
              _sig,
              getStrDecoration(kDecPrescaleStr).c_str());
    }

    // Check zero rates
    const std::string _output = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(kOutputRatesWarnings);
    if (getStrDecoration(kDecType) == "Chain" || getStrDecoration(kDecType) == "L1") { // If a plain chain
      if (getBasicPrescale() > 0 && isZero(_chainPasses) == kTRUE) { // If PS > 0 but no rate
        // Only display this warning once
        if (Config::config().getDisplayMsg(kMsgZeroRate) == kTRUE) {
          Warning("CounterBaseRates::finalise", "There are chains with non-zero prescale but ZERO rate. Please check %s !", _output.c_str());
        }
        std::ofstream _fout;
        _fout.open(_output.c_str(), std::ofstream::out | std::ofstream::app); // Open to append
        _fout << "Warning in <CounterBaseRates::finalise>: Chain " << getName() << " has a prescale of " << getStrDecoration(kDecPrescaleStr) << " but ZERO RATE." << std::endl;
        _fout.close();
      }
    }

    // This has just been set
    Float_t _walltime = getDecoration(kDecLbLength);

    // Do lower chain rate
    Float_t _inputRate = 0.;
    if (getStrDecoration(kDecType) == "L1") {      // For L1, just eventsRun
      _inputRate = m_dataStore.getValue(kVarEventsRun, kSavePerCall) / _walltime;
      decorate(kDecInputRate, floatToString(_inputRate, 4)); // Save as string
    } else if (getStrDecoration(kDecType) == "Chain" && m_lowerRates != nullptr) { // For HLT, ask the L1 item
      _inputRate = m_lowerRates->m_dataStore.getValue(kVarEventsPassed, kSavePerCall) / _walltime;
      decorate(kDecInputRate, floatToString(_inputRate, 4)); // Save as string
    } else {
      decorate(kDecInputRate, std::string("-"));
    }

    // Check express
    if (getIntDecoration(kDecDoExpressChain) == 1 || m_alwaysDoExpressPS) {
      Float_t _rateExp = m_dataStore.getValue(kVarEventsPassedExpress, kSavePerCall);
      if (m_alwaysDoExpressPS) _rateExp = m_dataStore.getValue(kVarEventsPassed, kSavePerCall); // Duplicate this info
      _rateExp /= _walltime;
      decorate(kDecExpressRate, floatToString(_rateExp, 4)); // Save as string
      decorate(kDecExpressRate, _rateExp); // Save as float too
    } else {
      decorate(kDecExpressRate, std::string("-")); // Not calculated
      decorate(kDecExpressRate, (Float_t) 0.); // Not calculated
    }

    // Get unique rates
    Float_t _uniqueRate = 0.;
    if (getMyUniqueCounter() != 0) {
      _uniqueRate = getMyUniqueCounter()->getValue(kVarEventsPassed, kSavePerCall);
      _uniqueRate /= _walltime;
      Info(getMyUniqueCounter()->getName().c_str(), "Rate %f Hz, Unique Rate %f Hz", m_dataStore.getValue(kVarEventsPassed, kSavePerCall) / _walltime, _uniqueRate);
      decorate(kDecUniqueRate, floatToString(_uniqueRate, 4)); // Save as string
      decorate(kDecUniqueRate, _uniqueRate); // Save as float too
    } else {
      decorate(kDecUniqueRate, std::string("-")); // Not calculated
      decorate(kDecUniqueRate, (Float_t) 0.); // Not calculated
    }


    // Get unique fraction. Note this is now a fraction of the chain rate rather than the global rate
    if (getMyUniqueCounter() != 0) {
      Float_t _uniqueFraction = 0.;
      Float_t _myRate = getValue(kVarEventsPassed, kSavePerCall);
      _myRate /= _walltime;
      if (!isZero(_myRate)) {
        _uniqueFraction = _uniqueRate / _myRate;
      }
      decorate(kDecUniqueFraction, floatToString(_uniqueFraction * 100, 2)); // Save in %, as string
      decorate(kDecUniqueFraction, _uniqueFraction); // Save as float too
    } else {
      decorate(kDecUniqueFraction, std::string("-")); // Not calculated
      decorate(kDecUniqueFraction, (Float_t) 0.); // Not calculated
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
      Float_t _overlapPercentage = 100. * (_overlapPasses / _chainPasses);
      if (_overlapPercentage != _overlapPercentage || isinf(_overlapPercentage)) {
        Error("CounterBaseRates::finalise", "%s vs. %s propagated an inf. [Weight]", getName().c_str(), _overlapCounter->getName().c_str());
        _binValues.push_back(0.);
      } else {
        _binValues.push_back(_overlapPercentage);
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
      _otherName.erase(_otherName.find(Config::config().getStr(kAndString)), Config::config().getStr(kAndString).size());
      _otherName.erase(_otherName.find(getName()), getName().size());
      _chainNames.push_back(_otherName);
    }

    m_dataStore.newVariable(kVarOverlap).setSavePerCall("Chain Overlap Within Rates Group;Chain;Overlap [%]");
    m_dataStore.setBinLabels(kVarOverlap, kSavePerCall, _chainNames); // Set the bins to have this name
    for (size_t _i = 1; _i <= _binValues.size(); ++_i) {
      m_dataStore.store(kVarOverlap, _i, _binValues.at(_i - 1));  // Use the weight to set the effective value
      if (_binValues.at(_i - 1) >= Config::config().getInt(kRatesOverlapWarning)) {
        Warning("CounterBaseRates::finalise", "Chain %s overlaps %.1f%% with %s", getName().c_str(), _binValues.at(_i - 1), _chainNames.at(_i - 1).c_str());
      }
    }
  }
} // namespace TrigCostRootAnalysis
