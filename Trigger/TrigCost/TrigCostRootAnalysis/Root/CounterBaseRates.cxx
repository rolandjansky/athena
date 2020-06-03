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
   * @see CounterBase(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel);
   */
  CounterBaseRates::CounterBaseRates(const TrigCostData* costData, const std::string& name, Int_t ID,
                                     UInt_t detailLevel, MonitorBase* parent) :
    CounterBase(costData, name, ID, detailLevel, parent),
    m_L2s(),
    m_L1s(),
    m_cannotCompute(kFALSE),
    m_myUniqueCounter(nullptr),
    m_globalRates(nullptr),
    m_lowerRates(nullptr),
    m_doScaleByPS(0),
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

    m_doScaleByPS = Config::config().getInt(kRatesScaleByPS);
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

    if (name == Config::config().getStr(kRateExpressString)) m_alwaysDoExpressPS = kTRUE;
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
   * @param e Unused
   * @param f Unused
   * @param weight Event weight.
   */
  void CounterBaseRates::processEventCounter(UInt_t /*e*/, UInt_t /*f*/, Float_t weight) {
    ++m_calls;

    if (getInEvent() == kFALSE) return;

    // Reminder - _weight here is the enhanced bias event weight. May be already multiplied up by the basic weight (if
    // not the default value of 1)

    // Optional - when not doin EB weighting. We can scale up individual chains by their L1 PS to get the effective PS=1
    // rates.
    // Does not work on combinations!
    Float_t scaleByPS = 1.;
    if (m_doScaleByPS && (getStrDecoration(kDecType) == "Chain" || getStrDecoration(kDecType) == "L1")) {
      scaleByPS = getDecoration(kDecPrescaleValOnlineL1);
    }

    // WEIGHTED Prescale
    Double_t weightPS = runWeight(m_alwaysDoExpressPS); // alwaysDoExpressPS is *only* for the express group
    //m_eventLumiExtrapolation is calculated by runWeight()
    if (m_disableEventLumiExtrapolation) m_eventLumiExtrapolation = 1;

    if (!isZero(weightPS)) {
       m_dataStore.store(kVarEventsPassed, 1., weightPS * weight * m_eventLumiExtrapolation * scaleByPS);
      // Chain passes with weight from PS as a float 0-1. All other weights  inc. 
      // m_dataStore.store(kVarEventsPerLumiblock, m_costData->getLumi(), (weightPS * weight *
      // scaleByPS)/((Float_t)m_costData->getLumiLength()) );
      if (m_detailLevel > 0 && m_L1s.size() == 1) (**m_L1s.begin()).fillHistograms(m_dataStore,
                                                                                   weightPS * weight * m_eventLumiExtrapolation * scaleByPS,
                                                                                   static_cast<MonitorRatesUpgrade*>(
                                                                                     m_parent)->getCollidingBunchFactor());

    }

    // DIRECT Prescale
    if (m_doDirectPS == kTRUE) {
      Float_t weightDirect = runDirect();
      if (!isZero(weightDirect)) m_dataStore.store(kVarEventsPassedDP, 1.,
                                                    weightDirect * weight * m_eventLumiExtrapolation * scaleByPS);                                                                                                                                                   // Chain
      // passes with weight from PS either 0 or 1. All other weights inc.
    }

    if (getIntDecoration(kDecDoExpressChain) == 1) {
      Double_t weightPSExpress = runWeight(/*doExpress =*/ kTRUE);
      if (!isZero(weightPSExpress)) m_dataStore.store(kVarEventsPassedExpress, 1.,
                                                       weightPSExpress * weight * m_eventLumiExtrapolation *
        scaleByPS);
    }

    Float_t passBeforePS = runDirect(/*usePrescales =*/ kFALSE);
    m_dataStore.store(kVarEventsPassedNoPS, 1., passBeforePS * weight * m_eventLumiExtrapolation * scaleByPS);
    // Times chain passes, irrespective of any PS/ All other wieghts inc.
    m_dataStore.store(kVarEventsRun, 1., weight * m_eventLumiExtrapolation * scaleByPS); // Times chain is processed,
                                                                                           // regardless of decision.
                                                                                           // All other weights inc.
    m_dataStore.store(kVarEventsPassRawStat, 1., passBeforePS); // Times chain passes, zero other weights (underlying
                                                                 // statistics of input file)
    m_dataStore.store(kVarEventsRunRawStat, 1.); // Times chain is processed, regardless of decision, zero other weights
                                                 // (underlying statistics of input file).
  }

  /**
   * The Rates counters have a much more sophisticated prescale treatment and hence do not use this function.
   * @return 0 for rates counters, prints error
   */
  Double_t CounterBaseRates::getPrescaleFactor(UInt_t /*e*/) {
    Error("CounterBaseRates::getPrescaleFactor",
          "This function is not defined for a Rates Counter, see the Rates base class.");
    return 0.;
  }

  /**
   * End of event processing. Unused with Rates counters
   */
  void CounterBaseRates::endEvent(Float_t /*weight*/) {
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
   * @param toAdd Add a L1 TriggerItem which is to be used by this rates counter.
   */
  void CounterBaseRates::addL1Item(RatesChainItem* toAdd) {
    assert(toAdd != nullptr);
    m_L1s.insert(toAdd);
    // Add back-link
    toAdd->addCounter(this);
  }

  /**
   * @param toAdd Add a grouping of items in a CPS.
   * @param name The name of the chain in this CPS group which is to be active in this counter
   */
  void CounterBaseRates::addCPSItem(RatesCPSGroup* toAdd, std::string name) {
    if (toAdd != nullptr) m_cpsGroups.insert(toAdd);
    m_myCPSChains.insert(name);
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
  Bool_t CounterBaseRates::checkMultiSeed(RatesChainItem* toAdd) {
    // Perform Union check on number of L1 seeds
    if (dynamic_cast<CounterRatesUnion*>(this) != NULL) { //If I am actually a CounterRatesUnion
      if (toAdd->getLower().size() > (UInt_t) Config::config().getInt(kMaxMultiSeed)) {
        // We are much stricter if this is a GLOBAL counter - don't want to have to disable it
        // HACK - we don't want to vero MU20||MU21 (June 17)
        if (toAdd->getLower().size() == 2) { // TODO this is temporary for July 2017
          bool passesHack = kTRUE;
          for (ChainItemSetIt_t L1Hack = toAdd->getLower().begin(); L1Hack != toAdd->getLower().end(); ++L1Hack) {
            if ((*L1Hack)->getName() == "L1_MU20") continue;
            else if ((*L1Hack)->getName() == "L1_MU21") continue;
            passesHack = kFALSE;
          }
          if (passesHack) return kTRUE;
        }
        Bool_t isGlobal = kFALSE;
        if (getName() == Config::config().getStr(kRateGlobalL1String)) isGlobal = kTRUE;
        else if (getName() == Config::config().getStr(kRateGlobalHLTString)) isGlobal = kTRUE;
        else if (getName() == Config::config().getStr(kRateGlobalPhysicsMainString)) isGlobal = kTRUE;
        else if (getName() == Config::config().getStr(kRateExpressString)) isGlobal = kTRUE;
        else if (getStrDecoration(kDecType) == "UniqueHLT") isGlobal = kTRUE;
        if (isGlobal) {
          // We only warn once per chain
          static std::set<std::string> toWarnAbout;
          if (toWarnAbout.count(toAdd->getName()) == 0) {
            toWarnAbout.insert(toAdd->getName());
            Warning("CounterBaseRates::checkMultiSeed", "Not including %s in RATE_GLOBAL/EXSPRESS/UNIQUE calculations due to %u L1 seeds.",
                    toAdd->getName().c_str(), (UInt_t) toAdd->getLower().size());
          }
          return kFALSE;
        }
      }
    }
    return kTRUE;
  }

  /**
   * @param toAdd Add a HLT TriggerItem which is to be used by this rates counter.
   */
  void CounterBaseRates::addL2Item(RatesChainItem* toAdd) {
    assert(toAdd != nullptr);
    if (checkMultiSeed(toAdd) == kFALSE) return;

    m_L2s.insert(toAdd);
    toAdd->addCounter(this); // Add back-link
    for (ChainItemSetIt_t lower = toAdd->getLowerStart(); lower != toAdd->getLowerEnd(); ++lower) {
      assert((*lower) != nullptr);
      m_L1s.insert((*lower));
      //TODO do i need to add this counter to the L1s here? Don't think so
    }
  }

  /**
   * @param toAdd Add multiple HLT TriggerItems which should be used by this rates counter
   */
  void CounterBaseRates::addL2Items(ChainItemSet_t toAdd) {
    for (ChainItemSetIt_t it = toAdd.begin(); it != toAdd.end(); ++it) {
      RatesChainItem* L2 = (*it);
      // Perform Union check on number of L1 seeds
      if (checkMultiSeed(L2) == kFALSE) continue;
      m_L2s.insert(L2);
      L2->addCounter(this); // Add back-link
      for (ChainItemSetIt_t lower = L2->getLowerStart(); lower != L2->getLowerEnd(); ++lower) {
        m_L1s.insert((*lower));
      }
    }
  }

  /**
   * @param toAdd Add a L3 item, for upgrade this will be the new HLT
   */
  void CounterBaseRates::addL3Item(RatesChainItem* toAdd) {
    m_L3s.insert(toAdd);
    toAdd->addCounter(this); // Add back-link
    for (ChainItemSetIt_t lower = toAdd->getLowerStart(); lower != toAdd->getLowerEnd(); ++lower) {
      m_L2s.insert((*lower));
      for (ChainItemSetIt_t lowerLower = (*lower)->getLowerStart(); lowerLower != (*lower)->getLowerEnd(); ++lowerLower) {
        m_L1s.insert((*lowerLower));
      }
    }
  }

  /**
   * @param toAdd Add many L3 items, for upgrade this will be the new HLT
   */
  void CounterBaseRates::addL3Items(ChainItemSet_t toAdd) {
    for (auto item : toAdd) {
      addL3Item(item);
    }
  }

  /**
   * @param _overlap Pointer to another counter which overlaps with this one, used to get relative rates at the end of
   *run.
   */
  void CounterBaseRates::addOverlap(CounterBase* overlap) {
    m_ovelapCounters.insert(overlap);
  }

  /**
   * @return kTRUE if at least one HLT counter of this chain had info in the D3PD for this event or one L1 counter of an
   *L1 chain was in the bunch group.
   */
  Bool_t CounterBaseRates::getInEvent() {
    if (m_L2s.size() > 0 || m_cpsGroups.size() > 0) { // I'm a HLT Chain(s)
      // Check my individual chains
      for (ChainItemSetIt_t L2It = m_L2s.begin(); L2It != m_L2s.end(); ++L2It) {
        if ((*L2It)->getInEvent() == kTRUE) return kTRUE;
      }
      // Chekc any CPS groups of chains I may have
      for (CPSGroupSetIt_t CPSIt = m_cpsGroups.begin(); CPSIt != m_cpsGroups.end(); ++CPSIt) {
        RatesCPSGroup* cpsGroup = (*CPSIt);
        for (ChainItemSetIt_t it = cpsGroup->getChainStart(); it != cpsGroup->getChainEnd(); ++it) {
          if ((*it)->getInEvent() == kTRUE) return kTRUE;
        }
      }
    } else if (m_L1s.size() > 0) { // I'm a L1 Chain(s)
      // Still need to check that the L1 chain is active for this bunch group
      for (ChainItemSetIt_t L1It = m_L1s.begin(); L1It != m_L1s.end(); ++L1It) {
        if ((*L1It)->getInEvent() == kTRUE) return kTRUE;
      }
    } else if (m_L3s.size() > 0) { // I'm a Upgrade HLT Chain(s)
      for (ChainItemSetIt_t L3It = m_L3s.begin(); L3It != m_L3s.end(); ++L3It) {
        if ((*L3It)->getInEvent() == kTRUE) return kTRUE;
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
    Float_t chainPasses = m_dataStore.getValue(kVarEventsPassed, kSavePerCall);
    Float_t chainPassesDP = m_dataStore.getValue(kVarEventsPassedDP, kSavePerCall);
    Float_t sig = TMath::Sqrt(TMath::Abs(chainPassesDP - chainPasses)) / TMath::Max(chainPassesDP, chainPasses);

    if (sig > 3 && !isZero(chainPassesDP)) {
      Warning("CounterBaseRates::finalise", "Chain %s of %s has Weight:%.4f DP:%.4f MethodDeviation:%.2f sigma! (PS: %s)",
              getName().c_str(), getStrDecoration(kDecRatesGroupName).c_str(),
              chainPasses,
              chainPassesDP,
              sig,
              getStrDecoration(kDecPrescaleStr).c_str());
    }

    // Check zero rates
    const std::string output = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(kOutputRatesWarnings);
    if (getStrDecoration(kDecType) == "Chain" || getStrDecoration(kDecType) == "L1") { // If a plain chain
      if (getBasicPrescale() > 0 && isZero(chainPasses) == kTRUE) { // If PS > 0  but no rate
        // Only display this warning once
        if (Config::config().getDisplayMsg(kMsgZeroRate) == kTRUE) {
          Warning("CounterBaseRates::finalise", "There are chains with non-zero prescale but ZERO rate. Please check %s !", output.c_str());
        }
        std::ofstream fout;
        fout.open(output.c_str(), std::ofstream::out | std::ofstream::app); // Open to append
        fout << "Warning in <CounterBaseRates::finalise>: Chain " << getName() << " has a prescale of " << getStrDecoration(kDecPrescaleStr) << " but ZERO RATE." << std::endl;
        fout.close();
      }
    }

    // This has just been set
    Float_t walltime = getDecoration(kDecLbLength);

    // Do lower chain rate
    Float_t inputRate = 0.;
    if (getStrDecoration(kDecType) == "L1") {      // For L1, just eventsRun
      inputRate = m_dataStore.getValue(kVarEventsRun, kSavePerCall) / walltime;
      decorate(kDecInputRate, floatToString(inputRate, 4)); // Save as string
    } else if (getStrDecoration(kDecType) == "Chain" && m_lowerRates != nullptr) { // For HLT, ask the L1 item
      inputRate = m_lowerRates->m_dataStore.getValue(kVarEventsPassed, kSavePerCall) / walltime;
      decorate(kDecInputRate, floatToString(inputRate, 4)); // Save as string
    } else {
      decorate(kDecInputRate, std::string("-"));
    }

    // Check express
    if (getIntDecoration(kDecDoExpressChain) == 1 || m_alwaysDoExpressPS) {
      Float_t rateExp = m_dataStore.getValue(kVarEventsPassedExpress, kSavePerCall);
      if (m_alwaysDoExpressPS) rateExp = m_dataStore.getValue(kVarEventsPassed, kSavePerCall); // Duplicate this info
      rateExp /= walltime;
      decorate(kDecExpressRate, floatToString(rateExp, 4)); // Save as string
      decorate(kDecExpressRate, rateExp); // Save as float too
    } else {
      decorate(kDecExpressRate, std::string("-")); // Not calculated
      decorate(kDecExpressRate, (Float_t) 0.); // Not calculated
    }

    // Get unique rates
    Float_t uniqueRate = 0.;
    if (getMyUniqueCounter() != 0) {
      uniqueRate = getMyUniqueCounter()->getValue(kVarEventsPassed, kSavePerCall);
      uniqueRate /= walltime;
      Info(getMyUniqueCounter()->getName().c_str(), "Rate %f Hz, Unique Rate %f Hz", m_dataStore.getValue(kVarEventsPassed, kSavePerCall) / walltime, uniqueRate);
      decorate(kDecUniqueRate, floatToString(uniqueRate, 4)); // Save as string
      decorate(kDecUniqueRate, uniqueRate); // Save as float too
    } else {
      decorate(kDecUniqueRate, std::string("-")); // Not calculated
      decorate(kDecUniqueRate, (Float_t) 0.); // Not calculated
    }


    // Get unique fraction. Note this is now a fraction of the chain rate rather than the global rate
    if (getMyUniqueCounter() != 0) {
      Float_t uniqueFraction = 0.;
      Float_t myRate = getValue(kVarEventsPassed, kSavePerCall);
      myRate /= walltime;
      if (!isZero(myRate)) {
        uniqueFraction = uniqueRate / myRate;
      }
      decorate(kDecUniqueFraction, floatToString(uniqueFraction * 100, 2)); // Save in %, as string
      decorate(kDecUniqueFraction, uniqueFraction); // Save as float too
    } else {
      decorate(kDecUniqueFraction, std::string("-")); // Not calculated
      decorate(kDecUniqueFraction, (Float_t) 0.); // Not calculated
    }

    // Next step is only for chain counters
    if (getStrDecoration(kDecType) != "Chain") return;

    if (isZero(chainPasses)) return;

    if (m_ovelapCounters.size() <= 1) return; // Bug - doesn't work for one overlap counter strangely

    // Get the % overlap with each other counter
    std::vector< Float_t > binValues;
    Bool_t allZero = kTRUE;
    for (CounterSetIt_t it = m_ovelapCounters.begin(); it != m_ovelapCounters.end(); ++it) {
      CounterBase* overlapCounter = (*it);
      Float_t overlapPasses = overlapCounter->getValue(kVarEventsPassed, kSavePerCall);
      Float_t overlapPercentage = 100. * (overlapPasses / chainPasses);
      if (overlapPercentage != overlapPercentage || isinf(overlapPercentage)) {
        Error("CounterBaseRates::finalise", "%s vs. %s propagated an inf. [Weight]", getName().c_str(), overlapCounter->getName().c_str());
        binValues.push_back(0.);
      } else {
        binValues.push_back(overlapPercentage);
        if (!isZero(overlapPercentage)) allZero = kFALSE;
      }
    }
    if (allZero == kTRUE) return;

    // Get the names of the other counters, use trickery on the name of the overlap counter
    std::vector<std::string> chainNames;
    for (CounterSetIt_t it = m_ovelapCounters.begin(); it != m_ovelapCounters.end(); ++it) {
      CounterBase* overlapCounter = (*it);
      // Get the name of the other chain from the name of the AND counter
      std::string otherName = overlapCounter->getName();
      otherName.erase(otherName.find(Config::config().getStr(kAndString)), Config::config().getStr(kAndString).size());
      otherName.erase(otherName.find(getName()), getName().size());
      chainNames.push_back(otherName);
    }

    m_dataStore.newVariable(kVarOverlap).setSavePerCall("Chain Overlap Within Rates Group;Chain;Overlap [%]");
    m_dataStore.setBinLabels(kVarOverlap, kSavePerCall, chainNames); // Set the bins to have this name
    for (size_t i = 1; i <= binValues.size(); ++i) {
      m_dataStore.store(kVarOverlap, i, binValues.at(i - 1));  // Use the weight to set the effective value
      if (binValues.at(i - 1) >= Config::config().getInt(kRatesOverlapWarning)) {
        Warning("CounterBaseRates::finalise", "Chain %s overlaps %.1f%% with %s", getName().c_str(), binValues.at(i - 1), chainNames.at(i - 1).c_str());
      }
    }
  }
} // namespace TrigCostRootAnalysis
