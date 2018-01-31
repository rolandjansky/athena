// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_Utility_H
#define TrigCostRootAnalysis_Utility_H

// STL include(s):
#include <string>
#include <sstream>
#include <utility>
#include <map>
#include <set>
#include <fstream>
#include <limits.h>

// ROOT include(s):
#include <Rtypes.h>
#include <TError.h>

//Forward declaration
class TH1F;
class TFile;


// Global compile switch to turn on multi threading
//#define MTHREAD

// The locking mechanism can be switched on here - but it's better to read in everything from the file in thread mode
#ifdef MTHREAD
  #define MUTEX_ON m_mutex.lock();
  #define MUTEX_OFF m_mutex.unlock();
#else
  #define MUTEX_ON //noop
  #define MUTEX_OFF //noop
#endif

/**
 * @def UNUSED(expr)
 * Preprocessed macro to mark deliberately unused variables and hide pedantic compiler warnings.
 */
#define UNUSED(expr) do {(void) (expr);} while (0)

/**
 * @file Utility.h
 *
 * Place for handy utility functions
 *
 * Also a central location for typedefs
 *
 * This should be a temporary dev space, everything here is expected to move elsewhere by the end
 */

namespace TrigCostRootAnalysis {
  //Forward declaration
  class TrigCostData;
  class CounterBase;
  class MonitorBase;
  class DataVariable;
  class CounterBaseRates;
  class RatesChainItem;
  class RatesCPSGroup;

  /**
   * @enum VariableOption
   * List of available monitor options.
   */
  enum VariableOption_t {
    kSavePerCall = 0,  //!< enum save once per call
    kSavePerEvent,  //!< enum save once per event
    kSavePerPeriod, //!< enum save once per user-defined period
    kSavePerEventFraction, //!< enum save once per event, divided by a per-event denominator
    kVariableOption_SIZE //!< Size of this enum, this entry must always come last.
  };

  const static std::string VariableOptionStr[] = {
    "perCall",
    "perEvent",
    "perPeriod",
    "perEventFractional"
  };

  enum EBBunchGroupType_t {
    kBG_NONE = 0,
    kBG_FILLED = 1,
    kBG_CALREQ = 2, // Not used
    kBG_EMPTY = 3,
    kBG_UNPAIRED_ISO = 4,
    kBG_UNPAIRED_NONISO = 5,
    kBG_FIRSTEMPTY = 6,
    kBG_UNPAIRED = 7, // Not used
    kBG_ABORTGAPNOTCALIB = 8,
    kBG_UNSET = 9, // Internal default value
    kEBBunchGroupType_SIZE = 10 //!< Size of this enum, this entry must always come last.
  };

  const static std::string BunchGroupNameStr[] = {
    "NONE", "FILLED", "CALREQ", "EMPTY", "UNPAIRED_ISO", "UNPAIRED_NONISO", "FIRSTEMPTY", "UNPAIRED",
    "ABORTGAPNOTCALIB", "UNSET"
  };

  enum FormatterOption_t {
    kFormatOptionNone = 0, //!< No additional formatting
    kFormatOptionNormaliseEntriesWallTime, //!< Divide the cell entries through by the collection's walltime
    kFormatOptionNormaliseWallTime, //!< Divide the cell value through by the collection's walltime
    kFormatOptionNormaliseLBTimeDec, //!< Divide the cell value through by the counter's kDecLbLength decoration
    kFormatOptionUseWallTime, //!< Display the walltime for this counter collection
    kFormatOptionNormaliseEntries, //!< Divide the cell value through by the number of entries
    kFormatOptionMiliSecToSec, //!< Convert cell from miliseconds to seconds
    kFormatOptionToPercentage, //!< Multiply by 100
    kFormatOptionUseEntries, //!< Use the numer of DataVariable etries rather than the DataVariable value
    kFormatOptionUseStringDecoration, //!< Output a value not from the counter's DataStore, rather a string decoration
                                      // instead.
    kFormatOptionUseFloatDecoration,//!< Output a value not from the counter's DataStore, rather a Float_t decoration
                                    // instead.
    kFormatOptionUseIntDecoration//!< Output a value not from the counter's DataStore, rather a Int_t decoration
                                 // instead.
  };

  /**
   * @enum ConfKey_t
   * Keys used with the config store.
   * This long list should be updated as needed.
   */
  enum ConfKey_t {
    kInputFiles = 0,
    kNEvents,
    kNSkip,
    kTreeName,
    kL2Prefix,
    kEFPrefix,
    kHLTPrefix,
    kConfigPrefix,
    kDoL2,
    kDoEF,
    kDoHLT,
    kNThread,
    kDoHLTOnline,
    kDoAllSummary,
    kDoKeySummary,
    kDoLumiBlockSummary,
    kFullSummaryStartLb,
    kLumiStart,
    kLumiEnd,
    kNLbFullSummary,
    kNLbFullSkip,
    kNLbPerHLTConfig,
    kNHLTConfigSummary,
    kFullEventMaxNumToSave,
    kFullEventSaveOnePer,
    kUpgradeScenario,
    kDoUpgradeRatesScan,
    kRatesForcePass,
    kRatesOverlapWarning,
    kRatesScaleByPS,
    kExtrapolate8To13,
    kExtrapolate13To5,
    kLumiExtrapWeight,
    kMaxMultiSeed,
    kWroteProgressFile,
    kBasicEventWeight,
    kOnlinePeakMuAverage,
    kEffectivePrescale,
    kEventWeight,
    kIsRootCore,
    kCurrentEventIsSlow,
    kEventTimeOffset,
    kDoNotWriteMetadata,
    kCurrentEventBunchGroupID,
    kCurrentEventEBWeight,
    kCurrentEventWasRandomOnline, // <BEGIN> Monitors - ORDERING IS IMPORTANT HERE
    kMonitorBegin, //!< This entry must be first (used in loops elsewhere). The rest of the monitors can technically
                   // come in any order, and new ones may be added
    kDoRatesUpgradeMonitor,
    kDoRatesMonitor, //<! Keep RATES first, breaks the partitioning (to be fixed :( ) but other monitors can read from
                     // this one. TODO - this is no longer true?
    kDoChainMonitor,
    kDoChainAlgorithmMonitor,
    kDoSequenceMonitor,
    kDoSequenceAlgorithmMonitor,
    kDoAlgorithmMonitor,
    kDoAlgorithmClassMonitor,
    kDoSliceCPUMonitor,
    kDoROSMonitor,
    kDoROBINMonitor,
    kDoROSAlgorithmMonitor,
    kDoROSChainMonitor,
    kDoROIMonitor,
    kDoGlobalsMonitor,
    kDoFullEventMonitor, //!< This entry must be after the GlobalsMonitor/ breaks the partitioning (to be fixed :( )
    kDoEventProfileMonitor,
    kDoAllMonitor, //!< This entry must be last // <END> Monitors - ORDERING IS IMPORTANT HERE
    kEnableROSToAlgMatching,
    kPatternsMonitor,
    kPatternsOutput,
    kPatternsUnique,
    kPatternsExclude,
    kPatternsOverlap,
    kPatternsNoLumiWeight,
    kPatternsNoMuLumiWeight,
    kPatternsNoBunchLumiWeight,
    kPatternsExpoMuLumiWeight,
    kListOfNoLumiWeightChains,
    kListOfNoMuLumiWeightChains,
    kListOfNoBunchLumiWeightChains,
    kListOfExpoMuLumiWeightChains,
    kOutputPng,
    kOutputPdf,
    kOutputImage,
    kOutputRoot,
    kOutputHist,
    kOutputCanvas,
    kOutputCsv,
    kOutputXML,
    kOutputRatesGraph,
    kOutputRatesWarnings,
    kOutputMenus,
    kOutputRootDirectory, // Note - root as in base directory of program output, not root as in histograms
    kOutputDirectory,
    kOutputTag,
    kOutputImageDirectory,
    kOutputCSVDirectory,
    kOutputXMLDirectory,
    kOutputRatesGraphFilename,
    kOutputRootFilename,
    kLinkOutputDir,
    kLinkOutputDirName,
    kUserDetails,
    kErrorIgnore,
    kSlowThreshold,
    kSlowEvThreshold,
    kIsCPUPrediction,
    kEventElapsed,
    kEventStartTime,
    kNBunchGroups,
    kDoEBWeighting,
    kDoCPS,
    kIgnorePSGreaterThanOne,
    kIgnoreNonPhysBunchGroups,
    kIgnoreGRL,
    kNoLBRescaling,
    kPatternsInvert,
    kDirectlyApplyPrescales,
    kNoUpgradePileupScaling,
    kNoUpgradeBunchScaling,
    kUpgradeEMCountsPerGeV,
    kUpgradeJetCountsPerGeV,
    kUpgradeJetLargeWindow,
    kNoOnlineDeadtimeCorrection,
    kOnlineDeadtime,
    kExpoRateScaleModifierL1,
    kExpoRateScaleModifierHLT,
    kDeadtimeScalingFinal, // Final scaling factor used for online deadtime
    kDoUniqueRates,
    kRatesOnly,
    kDoGroupOverlap,
    kDoAllOverlap,
    kRatesForNonPhysics,
    kDefaultLBLength,
    kPredictionLumi, // From the CLI (priority 1)
    kPredictionLumiRunXML, // From the EB XML (prioirty 3)
    kPredictionLumiMenuXML, // From the prescales XML (priority 2)
    kPredictionLumiFinal, // The value actually chosen to be used
    kPredictionLumiFinalExpoL1, // The final value with exponential mu extrapolation
    kPredictionLumiFinalExpoHLT, // The final value with exponential mu extrapolation
    kPredictionLumiFinalMuComponent, // What part of PredictionLumiFinal is due to increased mu
    kPredictionLumiFinalMuExpoL1Component, // What mu extrapolation chains with an exponential mu dependence should get
    kPredictionLumiFinalMuExpoHLTComponent, // What mu extrapolation chains with an exponential mu dependence should get
    kPredictionLumiFinalBunchComponent, // What part of PredictionLumiFinal is due to extra bunches
    kEmptyBunchgroupExtrapolaion, // How much we extrapolate empty items
    kDoAdvancedLumiScaling,
    kTargetPeakMuAverage,
    kTargetPairedBunches,
    kPatternsExactMatch,
    kJIRA,
    kUseOnlyTheseBCIDs,
    kRunLumi,
    kRunLumiXML,
    kDoExponentialMu,
    kInvertHighMuRunVeto,
    kDebug,
    kNPasses,
    kUpgradeMergeTOBOverlap,
    kCurrentPass,
    kPairedBunches,
    kMessageWait,
    kCleanAll,
    kMultiRun,
    kMaxBunches,
    kMaxBCIDs,
    kSloppyExit,
    kROSXMLPath,
    kROSXMLName,
    kPrescaleXMLPath1,
    kPrescaleXMLPath2,
    kPrescaleXMLName1,
    kPrescaleXMLName2,
    kAutoMonXMLPath,
    kPrescaleSetName,
    kEventPickList,
    kMenuName,
    kEBXMLPath,
    kEBXMLName,
    kHLTPrescaleMenuXMLPath,
    kHLTPrescaleMenuXMLName,
    kL1PrescaleMenuXMLPath,
    kL1PrescaleMenuXMLName,
    kOutputSummaryDirectory,
    kWriteEBWeightXML,
    kWriteDummyPSXML,
    kNoMsgSuppression,
    kIgnoreRerun,
    kHistBins,
    kHistBinMin,
    kHistBinMax,
    kDataDir, // Inernally used strings
    kAFSDataDir,
    kL0String,
    kL1String,
    kL2String,
    kEFString,
    kHLTString,
    kAllString,
    kDelimatorString,
    kLumiBlockString,
    kUnknownString,
    kBlankString,
    kDummyString,
    kMultipleString,
    kAndString,
    kRateGlobalHLTString,
    kRateGlobalPhysicsMainString,
    kRateGlobalL2String,
    kRateGlobalL1String,
    kRateGlobalL0String,
    kRateExpressString,
    kRateUniqueString,
    kRateFallbackPrescaleL1,
    kRateFallbackPrescaleHLT,
    kMatchL1RandomToOnline,
    kUnbiasedWeightThreshold,
    kCachedString,
    kCalledString,
    kAllROIString,
    kNoneString,
    kMuonString,
    kEmTauString,
    kTauString,
    kEmString,
    kJetString,
    kJetEtString,
    kEnergyString,
    kMissingEnergyString,
    kHTString,
    kMHTString,
    kROBINString,
    kROSString,
    kAlwaysPassString,
    kEventsInFiles,
    kEventsProcessed,
    kEventsSkipped,
    kRunNumber,
    kFarmXML,
    kVersionString,
    kLBPerKeyset,
    kVarTime,     // Study Variable ENUMs
    kVarSteeringTimeCPUType,
    kVarEventsCPUType,
    kVarRerunTime,
    kVarPassTime,
    kVarTimeExec,
    kVarTimeElapsed,
    kVarFirstTime,
    kVarAlgTime,
    kVarCPUTime,
    kVarROSTime,
    kVarSteeringTime,
    kVarAlgCalls,
    kVarAlgCaches,
    kVarROSCalls,
    kVarROBReqs,
    kVarROBRets,
    kVarROBReqSize,
    kVarROBRetSize,
    kVarROBOther,
    kVarROBDisabled,
    kVarROBIgnored,
    kVarROBNotOK,
    kVarROBUnclassified,
    kVarROBPrefetched,
    kVarTrigCostTime,
    kVarTexecTime,
    kVarChainExecTime,
    kVarResultBuildingTime,
    kVarMonitoringTime,
    kVarCalls,
    kVarCallsRaw,
    kVarCallsSlow,
    kVarEventsActive,
    kVarEventsPassed,
    kVarEventsPassedDP,
    kVarEventsPassedNoPS,
    kVarEventsPassthrough,
    kVarEventsRun,
    kVarUnbiasedPassed,
    kVarUnbiasedRun,
    kVarEventsPassRawStat,
    kVarEventsRunRawStat,
    kVarEventsPassedExpress,
    kVarEventsSlow,
    kVarMu,
    kVarBunchWeight,
    kVarTotalPrescale,
    kVarL1PassEvents,
    kVarHLTEvents,
    kVarHLTPassEvents,
    kVarHLTPUs,
    kVarJetEta,
    kVarMuEta,
    kVarEmEta,
    kVarTauEta,
    kVarJetNThresh,
    kVarMuNThresh,
    kVarEmNThresh,
    kVarTauNThresh,
    kVarROI,
    kVarType,
    kVarEta,
    kVarPhi,
    kVarArea,
    kVarEt,
    kVarL1Thresh,
    kVarEventsPerLumiblock,
    kVarOverlap,
    kVarOverlapDP,
    kVarWeights,
    kDecStartTime,     // Decoration ENUMs
    kDecCallOrCache,
    kDecROIString,
    kDecROSString,
    kDecAlgClassName,
    kDecCounterClassification,
    kDecChainName,
    kDecSeqName,
    kDecLbLength,
    kDecType,
    kDecID,
    kDecComment,
    kDecIsCPS,
    kDecDoExpressChain,
    kDecElapsedTime,
    kDecRatesGroupName,
    kDecGroupName,
    kDecPrescaleStr,
    kDecPrescaleVal,
    kDecPrescaleValOnlineL1,
    kDecUniqueRate,
    kDecUniqueFraction,
    kDecExpressRate,
    kDecMyROS,
    kDecMyROBIN,
    kDecInputRate,
    kMsgNonPhysics,     // Error message suppression ENUM
    kMsgDivZero,
    kMsgRoISize,
    kMsgRoIHack,
    kMsgCannotFindRoI,
    kMsgUnknownRoIType,
    kMsgBadROB,
    kMsgHourBoundary,
    kMsgSaveFullEvent,
    kMsgXMLWeight,
    kMsgXMLPrescale,
    kMsgUnknownDecoration,
    kMsgDupeTOB,
    kMsgIllegalCharacters,
    kMsgZeroRate,
    kMsgLargeSteerTime,
    kMsgNoLumiInfo,
    kMsgCannotFindVar,
    kMsgCannotFindVO,
    kMsgNewUniqueCounter,
    kMsgLumiScaling,
    kMsgNoTETOB,
    kMsgNoGroup,
    kMsgTOBMerge,
    kConfKey_SIZE //!<  END of config. ENUM keysNumber of configuration keys - keep me as last entry
  };

  // Typedefs - used throughout
  // All iterators are implicitly const unless they contain "NonConst".
  typedef std::map< Int_t, std::string>       IntStringMap_t;
  typedef IntStringMap_t::const_iterator IntStringMapIt_t;
  typedef std::map< Int_t, Int_t>             IntIntMap_t;
  typedef IntIntMap_t::const_iterator IntIntMapIt_t;
  typedef std::map< UInt_t, UInt_t>           UIntUIntMap_t;
  typedef UIntUIntMap_t::const_iterator UIntUIntMapIt_t;
  typedef std::map< Int_t, Float_t>           IntFloatMap_t;
  typedef IntFloatMap_t::const_iterator IntFloatMapIt_t;
  typedef std::map< Int_t, Double_t>          IntDoubleMap_t;
  typedef IntDoubleMap_t::const_iterator IntDoubleMapIt_t;
  typedef std::map< Float_t, Int_t>           FloatIntMap_t;
  typedef FloatIntMap_t::const_iterator FloatIntMapIt_t;
  typedef std::map< std::string, std::string> StringStringMap_t;
  typedef StringStringMap_t::const_iterator StringStringMapIt_t;
  typedef std::map< std::string, Int_t>       StringIntMap_t;
  typedef StringIntMap_t::const_iterator StringIntMapIt_t;
  typedef StringIntMap_t::iterator StringIntMapNonConstIt_t;
  typedef std::map< std::string, Float_t>     StringFloatMap_t;
  typedef StringFloatMap_t::const_iterator StringFloatMapIt_t;
  typedef std::map< std::string, Double_t>    StringDoubleMap_t;
  typedef StringDoubleMap_t::const_iterator StringDoubleMapIt_t;
  typedef StringDoubleMap_t::iterator StringDoubleMapNonConstIt_t;

  typedef std::map< std::pair<std::string, Float_t>, Int_t > PairStringFloat_Float_Map_t;
  typedef PairStringFloat_Float_Map_t::const_iterator PairStringFloat_Float_MapIt_t;

  typedef std::map< Int_t, std::pair<std::string, Int_t> > Int_PairStringInt_Map_t;
  typedef Int_PairStringInt_Map_t::const_iterator Int_PairStringInt_MapIt_t;

  typedef std::map< ConfKey_t, std::string> confStringMap_t;
  typedef confStringMap_t::const_iterator confStringMapIt_t;
  typedef std::map< ConfKey_t, Int_t>       confIntMap_t;
  typedef confIntMap_t::const_iterator confIntMapIt_t;
  typedef std::map< ConfKey_t, Float_t>     confFloatMap_t;
  typedef confFloatMap_t::const_iterator confFloatMapIt_t;

  typedef std::map< ConfKey_t, MonitorBase* >  monitorMap_t;
  typedef monitorMap_t::const_iterator monitorIt_t;
  typedef monitorMap_t::iterator monitorNonConstIt_t;
  typedef std::pair< ConfKey_t, MonitorBase* > monitorPair_t;

  typedef std::map< std::string, CounterBase* > CounterMap_t;
  typedef CounterMap_t::const_iterator CounterMapIt_t;

  typedef std::map< std::string, CounterMap_t > CounterCollection_t;
  typedef CounterCollection_t::const_iterator CounterCollectionIt_t;
  typedef CounterCollection_t::iterator CounterCollectionNonConstIt_t;

  typedef std::set< CounterMap_t* >       CounterMapSet_t;
  typedef CounterMapSet_t::const_iterator CounterMapSetIt_t;

  typedef std::set< CounterBase* >     CounterSet_t;
  typedef CounterSet_t::const_iterator CounterSetIt_t;

  typedef std::pair< ConfKey_t, VariableOption_t >  ConfVariableOptionPair_t;
  typedef std::vector< ConfVariableOptionPair_t >   VariableOptionVector_t; //!< Structure to hold a reference to what
                                                                            // is stored in a DataStore

  typedef std::map< ConfKey_t, DataVariable* >::const_iterator VariableMapIt_t; //!< Internal use name to DataVariable
                                                                                // pointer iterator typedef.

  typedef std::map< CounterMap_t*, ConfKey_t > CounterMapType_t; // We iterate over a set of CounterMap_t, nice to know
                                                                 // what kind of collection it was from

  typedef std::set<CounterBaseRates*>           CounterBaseRatesSet_t;
  typedef CounterBaseRatesSet_t::const_iterator CounterBaseRatesSetIt_t;

  typedef std::set<RatesChainItem*>      ChainItemSet_t;
  typedef ChainItemSet_t::const_iterator ChainItemSetIt_t;

  typedef std::map< std::string, RatesChainItem* > ChainItemMap_t;
  typedef ChainItemMap_t::const_iterator ChainItemMapIt_t;

  typedef std::set<RatesCPSGroup*>      CPSGroupSet_t;
  typedef CPSGroupSet_t::const_iterator CPSGroupSetIt_t;

  typedef std::map< std::string, RatesCPSGroup* > CPSGroupMap_t;
  typedef CPSGroupMap_t::const_iterator CPSGroupMapIt_t;

  typedef std::set< Int_t >        IntSet_t;
  typedef IntSet_t::const_iterator IntSetIt_t;

  typedef std::set< std::string >     StringSet_t;
  typedef StringSet_t::const_iterator StringSetIt_t;

  typedef std::map< std::string, IntSet_t > StringIntSetMap_t;
  typedef StringIntSetMap_t::const_iterator StringIntSetMapIt_t;
  typedef StringIntSetMap_t::iterator StringIntSetMapNonConstIt_t;


  // Helper functions
  // These need to find a more permanent home

  Bool_t checkPatternNameMonitor(const std::string& _patternName, Bool_t _invert, Bool_t _isRerun = kFALSE);
  Bool_t checkPatternNameOutput(const std::string& _patternName, Bool_t _invert);
  Bool_t checkPatternUnique(const std::string& _patternName, Bool_t _invert);
  Bool_t checkPatternOverlap(const std::string& _patternName, Bool_t _invert);
  Bool_t checkPatternInternal(const std::string& _counterName, ConfKey_t _list, Bool_t _invert);
  Bool_t checkPatternNoLumiWeight(const std::string& _counterName);
  Bool_t checkPatternNoMuLumiWeight(const std::string& _counterName);
  Bool_t checkPatternNoBunchLumiWeight(const std::string& _counterName);
  Bool_t checkPatternExponentialWithMu(const std::string& _counterName);

  Int_t stringToInt(const std::string& _i);
  Float_t stringToFloat(const std::string& _i);
  Double_t stringToDouble(const std::string& _i);
  std::string intToString(Long64_t _i, UInt_t _pad = 0);
  std::string intToString(Int_t _i, UInt_t _pad = 0);
  std::string intToString(UInt_t _i, UInt_t _pad = 0);
  std::string floatToString(Float_t _f, Int_t _precision = 4);
  std::string doubleToString(Double_t _d, Int_t _precision = 4);
  void plotText(Double_t x, Double_t y, const char* text);
  void plotHistogram(TH1F* _h, Bool_t _isLogY = kTRUE, std::string _opt = "");
  Bool_t isZero(Float_t _float, Float_t _precision = 0.00000001);
  Bool_t isEqual(Float_t _float1, Float_t _float2, Float_t _precision = 0.00000001);
  ConfVariableOptionPair_t makePair(ConfKey_t _name, VariableOption_t _vo);
  UInt_t stringToIntHash(const std::string& s);
  const std::string& getLevelString(UInt_t _level);
  Float_t deltaR(Float_t _phi1, Float_t _phi2, Float_t _eta1, Float_t _eta2);

  class JsonExport {
  public:
    JsonExport() : m_level(0), m_hasLeaves(kFALSE), m_justEnded(kFALSE), m_minimal(kFALSE) {
    }

    void addNode(std::ostream& _fout, std::string _name, std::string _icon = "") {
      if (!m_minimal) _fout << std::string(m_level * 2, ' ');
      if (m_justEnded == kTRUE) _fout << ",";
      _fout << "{";
      if (!m_minimal) _fout << std::endl;
      m_justEnded = kFALSE;

      ++m_level;

      if (!m_minimal) _fout << std::string(m_level * 2, ' ');
      _fout << "\"text\": \"" << _name << "\",";
      if (!m_minimal) _fout << std::endl << std::string(m_level * 2, ' ');
      if (_icon != "") _fout << "\"icon\": \"images/" << _icon << ".png\",";
      if (!m_minimal && _icon != "") _fout << std::endl << std::string(m_level * 2, ' ');
      _fout << "\"children\": [";
      ++m_level;
      if (!m_minimal) _fout << std::endl;
    }

    void addLeaf(std::ostream& _fout, std::string _name, std::string _icon = "") {
      if (m_hasLeaves != 0) _fout << "," << std::endl;
      m_hasLeaves = kTRUE;
      if (!m_minimal) _fout << std::string(m_level * 2, ' ');
      _fout << "{\"text\": \"" << _name << "\"";
      if (_icon != "") _fout << ", \"icon\": \"images/" << _icon << ".png\"";
      _fout << "}";
    }

    void addLeafCustom(std::ostream& _fout, std::string _name, std::string _value, std::string _name2 = "",
                       std::string _value2 = "") {
      if (m_hasLeaves != 0) _fout << "," << std::endl;
      m_hasLeaves = kTRUE;
      if (!m_minimal) _fout << std::string(m_level * 2, ' ');
      _fout << "{\"" << _name << "\": \"" << _value << "\"";
      if (_name2 != "") {
        _fout << ", \"" << _name2 << "\": \"" << _value2 << "\"}";
      } else {
        _fout << "}";
      }
    }

    void endNode(std::ostream& _fout) {
      if (m_hasLeaves != 0) {
        _fout << std::endl;
      }
      m_hasLeaves = kFALSE;
      m_justEnded = kTRUE;


      if (!m_minimal) _fout << std::string(m_level * 2, ' ');
      _fout << "]";
      --m_level; // Leaves


      if (!m_minimal) _fout << std::endl;
      _fout << std::string(m_level * 2, ' ') << "}" << std::endl; //always do this endl to make the file not crazy
      --m_level; // Node
    }

    UInt_t m_level;
    Bool_t m_hasLeaves;
    Bool_t m_justEnded;
    Bool_t m_minimal;
  };
}

#endif //TrigCostRootAnalysis_Utility_H
