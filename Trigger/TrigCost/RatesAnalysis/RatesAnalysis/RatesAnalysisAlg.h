/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RATESANALYSIS_RATESANALYSISALG_H
#define RATESANALYSIS_RATESANALYSISALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include "EnhancedBiasWeighter/EnhancedBiasWeighter.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "RatesTrigger.h"
#include "RatesScanTrigger.h"
#include "RatesGroup.h"

#include "TTree.h"

#include <unordered_set>

namespace TrigConf {
   class ITrigConfigSvc;
}

/**
 * @brief virtual analysis class for performing rates studies on AOD
 * Pure virtual base class for trigger rate studies on EnhancedBias or MC AOD files.
 * This class should be inherited and the user should implement ratesInitialize, ratesExecute
 * and ratesFinalize. Triggers to emulate should be added in ratesInitialize with newTrigger
 * or other members such as addExisting. Manually emulated triggers should have their decision 
 * supplied during ratesExecute using setTriggerDesicison. Rates will be reported at the end.
 */
class RatesAnalysisAlg: public ::AthAnalysisAlgorithm { 
 public: 
  
  /**
   * Method by which the trigger pass/fail decision is calculated. Via manual or automated emulator or 
   * using the Trigger Decision Tool for a pre-existing item.
   */
  enum Method_t{
    kMANUAL, //!< The pass/fail decision is evaluated by the user and supplied per event using setTriggerDesicison
    kAUTO, //!< The pass/fail decision is automatically emulated per event based on decoding the trigger name
    kEXISTING //!< The pass/fail decision is taken from the Trigger Decision Tool for an existing trigger.
  };

  RatesAnalysisAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RatesAnalysisAlg(); 

  virtual StatusCode ratesInitialize() = 0; //!< To be implemented by the user. Register "triggers" to calculate the rate for 
  virtual StatusCode ratesExecute() = 0; //!<  To be implemented by the user. Supply pass/fail for all "triggers"
  virtual StatusCode ratesFinalize() = 0; //!< To be implemented by the user.

  /**
   * @brief Version of newTrigger which accepts a set of group names rather than a comma separated string.
   */
  StatusCode newTrigger(const std::string& name,
    const double prescale = 1.,
    const double expressPrescale = -1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const std::string& groups = "",
    const Method_t method = kMANUAL,
    const ExtrapStrat_t extrapolation = kLINEAR);

  /**
   * @brief Register a new trigger for emulation.
   * @param name The name of the emulated trigger. Recommend either a "L1_" or "HLT_" prefix.
   * @param prescale The prescale value to simulate applying. Default 1
   * @param expressPrescale The prescale value to simulate applying to the express stream. Default -1
   * @param seedName The name of any lower seeding e.g. L1 item. This info is required to get group rates correct when multiple chains seed of the same prescaled L1 item.
   * @param seedPrescale The prescale of any L1 seed item. Default 1.
   * @param Comma separated list of groups to include this trigger in. Any group containing "CPS" will be simulated as a coherent prescale group and implies that all members of that group share a common L1 seed.
   * @param method Determines who is responsible for evaluating the emulated trigger decision, you (kMANUAL) or the algorithm (kAUTO or kEXISTING).
   * @param extrapolation Determines the triggers luminosity extrapolation behaviour.
   */
  StatusCode newTrigger(const std::string& name,
    const double prescale = 1.,
    const double expressPrescale = -1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const std::set<std::string>& groups = std::set<std::string>(),
    const Method_t method = kMANUAL,
    const ExtrapStrat_t extrapolation = kLINEAR);

  /**
   * @brief Register a new threshold scan trigger which plots rate as a function of some dependent variable
   * @param name The name of the emulated scan trigger. Recommend either a "L1_" or "HLT_" prefix.
   * @param thresholdMin The lower threshold of this trigger, rates will not be available below this threshold
   * @param thresholdMax The upper threshold of this trigger, rates will not be available above this threshold
   * @param thresholdBins Granularity 
   * @param behaviour If the trigger should activate above (kTriggerAboveThreshold) or below (kTriggerBelowThreshold) the threshold
   * @param prescale The prescale value to simulate applying. Default 1
   * @param seedName The name of any lower seeding e.g. L1 item. This info is required to get group rates correct
   * @param seedPrescale The prescale of any L1 seed item. Default 1.
   * @param method Determines who is responsible for evaluating the emulated trigger decision, you (kMANUAL) or the algorithm (kAUTO or kEXISTING).
   * @param extrapolation Determines the triggers luminosity extrapolation behaviour.
   */
  StatusCode newScanTrigger(const std::string& name,
    const double thresholdMin,
    const double thresholdMax,
    const uint32_t thresholdBins = 100,
    const RatesScanTrigger::TriggerBehaviour_t behaviour = RatesScanTrigger::TriggerBehaviour_t::kTriggerAboveThreshold,
    const double prescale = 1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const Method_t method = kMANUAL,
    const ExtrapStrat_t extrapolation = kLINEAR);

  /**
   * @brief Version of newScanTrigger which takes a vector of bin edges
   * @param thresholdBinEdged Vector of bin edges to use for quantifying rate as a function of threshold
   */
  StatusCode newScanTrigger(const std::string& name,
    const std::vector<double>& thresholdBinEdges,
    const RatesScanTrigger::TriggerBehaviour_t behaviour = RatesScanTrigger::TriggerBehaviour_t::kTriggerAboveThreshold,
    const double prescale = 1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const Method_t method = kMANUAL,
    const ExtrapStrat_t extrapolation = kLINEAR);


  StatusCode addAllExisting(); //!< Register all existing triggers in the AOD into the rates algorithm.

  /**
   * @brief Register some existing triggers based on wild-card match, e.g. "L1_.*" for all L1.
   * @param pattern Wild-card string to match in trigger name 
   */
  StatusCode addExisting(const std::string pattern);

  /**
   * Set the pass/fail decision for an item.
   * @param name Name of the registered trigger
   * @param triggerIsPassed Trigger decision.
   */
  StatusCode setTriggerDesicison(const std::string& name, const bool triggerIsPassed = true, const bool triggerIsActive = true);

  /**
   * Set the pass threshold for a Scan Trigger item.
   * @param name Name of the registered scan-trigger
   * @param threshold The passed threshold in the event
   */
  StatusCode setTriggerDesicison(const std::string& name, const double threshold);

  /**
   * Set the target instantaneous luminosity and mu. Number of bunches in the ring is derived.
   * @param lumi The target lumi in cm-2s-1
   * @param mu The target pileup
   */
  void setTargetLumiMu(const double lumi, const double mu);

  /**
   * @brief Set the target instantaneous luminosity and number of bunches. Mu is derived.
   * @param lumi The target lumi in cm-2s-1
   * @param bunches The target number of paired bunches
   */
  void setTargetLumiBunches(const double lumi, const int32_t bunches);

  /**
   * @brief Set the target mu and number of bunches. The instantaneous luminosity is derived.
   * @param mu The target pileup
   * @param bunches The target number of paired bunches 
   */
  void setTargetMuBunches(const double mu, const int32_t bunches);

  /**
   * @brief Set the target instantaneous luminosity. Assumes a full-ring. Mu is derived.
   * @param lumi The target lumi in cm-2s-1
   */
  void setTargetLumi(const double lumi) { setTargetLumiBunches(lumi, EnhancedBiasWeighter::FULL_RING); }

  /**
   * @brief Set the target pileup. Assumes a full-ring. Inst. lumi is derived.
   * @param mu The target pileup
   */
  void setTargetMu(const double mu) { setTargetMuBunches(mu, EnhancedBiasWeighter::FULL_RING); } 

  /**
   * @brief Set the exponential scaling factor for relevant chains. scaling = exp(f * (targetMu - eventMu))
   * @param f Exponential factor
   */
  void setExponentialMuScalingFactor(const double f) { m_expoScalingFactor = f; }

 private: 

  virtual StatusCode initialize(); //!< Get the trigger decision tool and set up global groups
  virtual StatusCode execute(); //!< In first call - register all triggers. Then load event weighting parameters, fill trigger decisions, compute group rates.
  virtual StatusCode finalize(); //!< Print rates

  StatusCode populateTriggers(); //!< Register all triggers to emulate. This is actually done at the start of the event loop such that the TDT has access to the configuration.
  StatusCode executeTrigDecisionToolTriggers(); //!< Internal call to get the pass/fail for all TDT triggers
  StatusCode executeTriggerEmulation(); //!< Internal call to get the pass/fail for all automatically emulated triggers

  /**
   * @brief Internal function to check if a supplied HLT trigger and L1 seed match what is stored in the AOD config
   * @param name Name of the L1 or HLT trigger.
   * @param seedName For HLT triggers, name of the L1 seed item.
   * @return StatusCode::FAILURE if the trigger does not match the AOD.
   */
  StatusCode checkExistingTrigger(const std::string& name, const std::string& seedName);
  StatusCode checkGotTDT(); //!< Internal check that the TDT is fetched

  /**
   * @brief Print the input data instantaneous luminosity, mu and number of bunches.
   * Note this is averaged as the data are processed - so should only be printed at the end.
   */
  void printInputSummary() const;

  void printStatistics() const;  //!< Print some extra statistics on events processed
  void printTarget() const; //!< Print the target instantaneous luminosity, mu and number of bunches.
  void writeMetadata(); //!< Write to outpute tree (if any) the metadata needed downstream.


  /**
   * @brief String match coherent prescale groups.
   * @param group Name of a trigger group
   * @return If name-parsing thinks that the group is a coherent prescale group
   */
  bool isCPS(const std::string& group) const;

  /**
   * @brief String match random L1 items.
   * @param me Name of the trigger
   * @param seed Name of the trigger's seed
   * @return if name-parsing thinks that the trigger seeds from a random L1 trgigger
   */
  bool isRandomSeed(const std::string& me, const std::string& seed) const;

  /**
   * @brief String match to a trigger level. If unknown, we assume HLT.
   * @return trigger level (1 or 2)
   */
  uint32_t getLevel(const std::string& name) const;
  
  bool isZero(double v) const { return fabs(v) < 1e-10; } //!< Helper function for floating point subtraction

  std::unordered_map<std::string, std::unique_ptr<RatesTrigger>> m_triggers; //!< All individual triggers (L1 or HLT)
  std::unordered_map<std::string, std::unique_ptr<RatesScanTrigger>> m_scanTriggers; //!< All individual rates-scan triggers (L1 or HLT)
  std::unordered_map<std::string, std::unique_ptr<RatesGroup>> m_groups; //!< All regular and CPS groups 
  std::unordered_map<std::string, std::unique_ptr<RatesGroup>> m_globalGroups; //!< Big (master) groups which do the OR of the whole menu 
  std::unordered_map<std::string, std::unique_ptr<RatesGroup>> m_uniqueGroups; //!< Groups used to obtain unique rates for chains.

  std::unordered_set<RatesTrigger*> m_activatedTriggers; //!< Triggers which were changed & hence need to be reset at the event end.
  std::unordered_set<RatesTrigger*> m_expressTriggers; //!< Triggers with non-zero express PS, used to print them at the end.
  std::unordered_set<RatesGroup*> m_activeGroups; //!< All groups which are enabled (PS >= 1)
  std::unordered_map<size_t, double> m_lowestPrescale; //!< Lowest prescale within a CPS group, key is the hash of the CPS group name.
  std::vector<std::string> m_autoTriggers; //!< List of triggers which it is up to us to the algorithm to work out the pass/fail for

  std::unordered_map<std::string, const Trig::ChainGroup*> m_existingTriggers; //!< Map of triggers which we ask the TDT ChainGroup for the pass/fail 
  std::unordered_map<std::string, std::string> m_lowerTrigger; //!< Map of triggers lower chain, to tell if a HLT trigger ran or not. 

  std::vector<std::vector<std::string>> m_hltChainIDGroup;

  const std::string m_l1GroupName = "L1";
  const std::string m_l2GroupName = "Main";
  const std::string m_expressGroupName = "Express";

  ToolHandle<IEnhancedBiasWeighter> m_enhancedBiasRatesTool{this, "EnhancedBiasRatesTool", "EnhancedBiasWeighter/EnhancedBiasRatesTool"};
  ToolHandle<Trig::TrigDecisionTool> m_tdt{this, "TrigDecisionTool", "Trig::TrigDecisionTool/TrigDecisionTool"};
  ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc{this, "TrigConfigSvc", ""};

  Gaudi::Property<double> m_expoScalingFactor{this, "ExpoScalingFactor", 0.1, "Optional. Exponential factor if using exponential-mu rates scaling."};
  Gaudi::Property<double> m_inelasticCrossSection{this, "InelasticCrossSection", 8e-26, "Inelastic cross section in units cm^2. Default 80 mb at 13 TeV."};
  Gaudi::Property<bool> m_doUniqueRates{this, "DoUniqueRates", false, "Calculate unique rates for all chains (slow). Requires DoGlobalGroups=True too."}; 
  Gaudi::Property<bool> m_doGlobalGroups{this, "DoGlobalGroups", false, "Calculate total rates for each trigger level."};
  Gaudi::Property<bool> m_doTriggerGroups{this, "DoTriggerGroups", false, "Calculate total rates for each group of triggers."};
  Gaudi::Property<bool> m_doExpressRates{this, "DoExpressRates", false, "Calculate total rates for the express stream."};
  Gaudi::Property<bool> m_useBunchCrossingTool{this, "UseBunchCrossingTool", true, "BunchCrossing tool requires CONDBR2 access. Can be disabled here if this is a problem."};
  Gaudi::Property<bool> m_currentEventIsUnbiased; //!< If the current event was triggered online by RDx or not. Random seeded HLT chains must only see these
  Gaudi::Property<bool> m_doHistograms{this, "DoHistograms", true, "Switch on histogram output of rate vs. mu and position in train."};
  Gaudi::Property<bool> m_enableLumiExtrapolation{this, "EnableLumiExtrapolation", true, "If false then no extrapolation in L, N_bunch or <mu> will be performed.."};
  Gaudi::Property<uint32_t> m_vetoStartOfTrain{this, "VetoStartOfTrain", 0, "How many BCID to veto at the start of a bunch train."};
  //Gaudi::Property<std::string> m_prescalesJSON{this, "PrescalesJSON", "",  "Optional JSON of prescales from the TrigMenuRuleBook to apply."};
  Gaudi::Property<std::map<std::string, double>> m_prescalesJSON{this, "PrescalesJSON", {},  "Optional JSON of prescales from the TrigMenuRuleBook to apply."};


  double m_targetMu; //!< What pileup level the prediction is targeting
  double m_targetBunches; //!< How many bunches the prediction is targeting
  double m_targetLumi; //!< What instantaneous luminosity the prediction is targeting
  uint32_t m_runNumber; //!<What is the RunNumber
  double m_ratesDenominator; //!< How much walltime is seen by the algorithm. This is what we need to normalise to.
  uint32_t m_eventCounter; //!< Count how many events processed
  double m_weightedEventCounter; //!< Count how many weighted events were processed

  TH1D* m_scalingHist; //!< One-bin histogram to store the normalisation of the sample, for use in later combinations
  TH1D* m_bcidHist; //!< Histogram of the BCIDs distribution of the processing

  TTree* m_metadataTree; //!< Used to write out some metadata needed by post-processing (e.g. bunchgroup, lumi)

  WeightingValuesSummary_t m_weightingValues; //!< Possible weighting & lumi extrapolation values for the current event 
}; 

#endif //> !RATESANALYSIS_RATESANALYSISALG_H
