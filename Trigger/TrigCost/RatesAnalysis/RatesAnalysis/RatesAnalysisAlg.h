#ifndef RATESANALYSIS_RATESANALYSISALG_H
#define RATESANALYSIS_RATESANALYSISALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include "EnhancedBiasWeighter/EnhancedBiasWeighter.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "RatesTrigger.h"
#include "RatesScanTrigger.h"
#include "RatesGroup.h"

#include <unordered_set>

/**
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

  virtual StatusCode ratesInitialize() = 0;
  virtual StatusCode ratesExecute() = 0;
  virtual StatusCode ratesFinalize() = 0;

  StatusCode newTrigger(const std::string& name,
    const double prescale = 1.,
    const double expressPrescale = -1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const std::string& groups = "",
    const Method_t method = kMANUAL,
    const ExtrapStrat_t extrapolation = kLINEAR);
  StatusCode newTrigger(const std::string& name,
    const double prescale = 1.,
    const double expressPrescale = -1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const std::set<std::string>& groups = std::set<std::string>(),
    const Method_t method = kMANUAL,
    const ExtrapStrat_t extrapolation = kLINEAR);

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
  StatusCode newScanTrigger(const std::string& name,
    const std::vector<double>& thresholdBinEdges,
    const RatesScanTrigger::TriggerBehaviour_t behaviour = RatesScanTrigger::TriggerBehaviour_t::kTriggerAboveThreshold,
    const double prescale = 1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const Method_t method = kMANUAL,
    const ExtrapStrat_t extrapolation = kLINEAR);


  StatusCode addAllExisting();
  StatusCode addExisting(const std::string pattern);

  StatusCode setTriggerDesicison(const std::string& name, const bool triggerIsPassed = true);
  StatusCode setTriggerDesicison(const std::string& name, const double threshold);

  void setTargetLumiMu(const double lumi, const double mu);
  void setTargetLumiBunches(const double lumi, const int32_t bunches);
  void setTargetMuBunches(const double mu, const int32_t bunches);
  void setTargetLumi(const double lumi) { setTargetLumiBunches(lumi, EnhancedBiasWeighter::FULL_RING); }
  void setTargetMu(const double mu) { setTargetMuBunches(mu, EnhancedBiasWeighter::FULL_RING); } 
  void setExponentialMuScalingFactor(const double f) { m_expoScalingFactor = f; }

 private: 

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  virtual StatusCode beginInputFile();

  StatusCode populateTriggers();
  StatusCode executeTrigDecisionToolTriggers();
  StatusCode executeTriggerEmulation();
  StatusCode checkExistingTrigger(const std::string& name, const std::string& seedName);
  StatusCode checkGotTDT();

  void printTarget() const;
  void printInputSummary() const;
  void printStatistics() const;
  bool isCPS(const std::string& group) const;
  bool isRandomSeed(const std::string& me, const std::string& seed) const;
  uint32_t getLevel(const std::string& name) const;
  bool isZero(double v) const { return fabs(v) < 1e-10; }

  std::unordered_map<std::string, RatesTrigger> m_triggers; //!< All individual triggers (L1 or HLT)
  std::unordered_map<std::string, RatesScanTrigger> m_scanTriggers; //!< All individual rates-scan triggers (L1 or HLT)
  std::unordered_map<std::string, RatesGroup> m_groups; //!< All regular and CPS groups 
  std::unordered_map<std::string, RatesGroup> m_globalGroups; //!< Big (master) groups which do the OR of the whole menu 
  std::unordered_map<std::string, RatesGroup> m_uniqueGroups; //!< Groups used to obtain unique rates for chains.

  std::unordered_set<RatesTrigger*> m_activatedTriggers; //!< Triggers which were changed & hence need to be reset at the event end.
  std::unordered_set<RatesTrigger*> m_expressTriggers; //!< Triggers with non-zero express PS, used to print them at the end.
  std::unordered_set<RatesGroup*> m_activeGroups; //!< All groups which are enabled (PS >= 1)
  std::unordered_map<size_t, double> m_lowestPrescale; //!< Lowest prescale within a CPS group, key is the hash of the CPS group name.
  std::vector<std::string> m_autoTriggers; //!< List of triggers which it is up to us to the algorithm to work out the pass/fail for
  std::unordered_map<std::string, const Trig::ChainGroup*> m_existingTriggers; //!< Map of triggers which we ask the TDT ChainGroup for the pass/fail 

  std::unordered_map<std::string, ChainDetail> m_loadedXML; //!< Details loaded from a prescale XML are stored here

  const std::string m_l1GroupName = "L1";
  const std::string m_l2GroupName = "Main";
  const std::string m_expressGroupName = "Express";

  ToolHandle<IEnhancedBiasWeighter> m_enhancedBiasRatesTool; //!< Tool to access weighting information required for trigger rates.
  ToolHandle<Trig::TrigDecisionTool> m_tdt;

  double m_inelasticCrossSection; //!< Sigma_inel in units of cm^2. Default of 80 mb @ 13 TeV = 8e-26 cm^2
  double m_targetMu; //!< What pileup level the prediction is targeting
  double m_targetBunches; //!< How many bunches the prediction is targeting
  double m_targetLumi; //!< What instantaneous luminosity the prediction is targeting
  double m_expoScalingFactor; //!< Exponential factor for exponential-in-mu chains
  bool m_populatedTriggers; //!< Set to true once we enter the event loop. No new triggers can be added from this point.
  bool m_doUniqueRates; //!< What rate is unique to each trigger. More computationally taxing.
  bool m_useBunchCrossingTool; //!< If rates should be done vs. position in the train. Requires DB access
  bool m_currentEventIsUnbiased; //!< If the current event was triggered online by RDx or not. Random seeded HLT chains must only see these
  bool m_doHistograms; //!< If histogram export is enabled or not.
  bool m_isMC; //!< If input is Monte Carlo
  bool m_normaliseHistograms; //!< Flag to apply normalisation to histograms. Easier to set this to FALSE if will be needing to hadd
  bool m_enableLumiExtrapolation; //!< Global flag if extrapolation is to be used. If not, results will be presented at the input file's lumi
  double m_ratesDenominator; //!< How much walltime is seen by the algorithm. This is what we need to normalise to.
  double m_mcCrossSection; //!< If MC, the cross section in nb
  double m_mcFilterEfficiency; //!< If MC, the filter efficiency
  double m_mcKFactor; //!< If MC, any higher order k-factor
  uint32_t m_eventCounter; //!< Count how many events processed
  double   m_weightedEventCounter; //!< Count how many weighted events were processed
  std::string m_prescaleXML; //!< Filename of XML of prescales to load

  TH1D* m_scalingHist; //!< One-bin histogram to store the normalisation of the sample, for use in later combinations

  WeightingValuesSummary_t m_weightingValues; //!< Possible weighting & lumi extrapolation values for the current event 
}; 

#endif //> !RATESANALYSIS_RATESANALYSISALG_H
