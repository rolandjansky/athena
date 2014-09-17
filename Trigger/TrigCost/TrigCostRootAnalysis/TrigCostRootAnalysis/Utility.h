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

/**
 * @def UNUSED(expr)
 * Preprocessed macro to mark deliberately unused variables and hide pedantic compiler warnings.
 */
#define UNUSED(expr) do { (void)(expr); } while (0)

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

  enum FormatterOption_t {
    kFormatOptionNone = 0, //!< No additional formatting
    kFormatOptionNormaliseEntriesWallTime, //!< Divide the cell entries through by the collection's walltime
    kFormatOptionNormaliseWallTime, //!< Divide the cell value through by the collection's walltime
    kFormatOptionUseWallTime, //!< Display the walltime for this counter collection
    kFormatOptionNormaliseEntries, //!< Divide the cell value through by the number of entries
    kFormatOptionMiliSecToSec, //!< Convert cell from miliseconds to seconds
    kFormatOptionUseEntries, //!< Use the numer of DataVariable etries rather than the DataVariable value 
    kFormatOptionUseStringDecoration, //!< Output a value not from the counter's DataStore, rather a string decoration instead.
    kFormatOptionUseFloatDecoration//!< Output a value not from the counter's DataStore, rather a Float_t decoration instead.
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
    kDoHLTOnline,
    kDoAllSummary,
    kDoKeySummary,
    kDoLumiBlockSummary,
    kFullSummaryStartLb,
    kLumiStart,
    kLumiEnd,
    kNLbFullSummary,
    kFullEventMaxNumToSave,
    kFullEventSaveOnePer,
    kRatesForcePass,
    kRatesOverlapWarning,
    kMaxMultiSeed,
    kWroteProgressFile,
    // <BEGIN> Monitors - ORDERING IS IMPORTANT HERE
    kMonitorBegin, //!< This entry must be first (used in loops elsewhere)
    // The rest of the monitors can come in any order, and new ones may be added
    kDoChainMonitor, 
    kDoChainAlgorithmMonitor,
    kDoSequenceMonitor,
    kDoSequenceAlgorithmMonitor,
    kDoAlgorithmMonitor,
    kDoROSMonitor,
    kDoROIMonitor,
    kDoL1ChainMapMonitor,
    kDoFullEventMonitor,
    kDoGlobalsMonitor,
    kDoRatesMonitor,
    kDoAllMonitor, //!< This entry must be last
    // <END> Monitors - ORDERING IS IMPORTANT HERE
    kPatternsMonitor,
    kPatternsOutput,
    kOutputPng,
    kOutputPdf,
    kOutputImage,
    kOutputRoot,
    kOutputHist,
    kOutputCanvas,
    kOutputCsv,
    kOutputXML,
    kOutputRatesGraph,
    kOutputMenus,
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
    kSlowEventThreshold,
    kDoEBWeighting,
    kDirectlyApplyPrescales,
    kDoUniqueRates,
    kDefaultLBLength,
    kDebug,
    kCleanAll,
    kSloppyExit,
    kROSXMLPath,
    kROSXMLName,
    kMenuXMLPath,
    kMenuXMLName,
    kPrescaleXMLPath,
    kPrescaleXMLName,
    kHLTPrescaleMenuXMLPath,
    kHLTPrescaleMenuXMLName,
    kL1PrescaleMenuXMLPath,
    kL1PrescaleMenuXMLName,
    kOutputSummaryDirectory,
    kHistBins,
    // Inernally used strings
    kDataDir,
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
    kRateGlobalL1String,
    kRateUniqueString,
    kCachedString,
    kCalledString,
    kAllROIString,
    kNoneString,
    kMuonString,
    kEmTauString,
    kJetString,
    kJetEtString,
    kEnergyString,
    kROBINString,
    kROSString,
    kAlwaysPassString,
    kEventsInFiles,
    kRunNumber,
    // Study Variable ENUMs
    kVarTime,
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
    kVarCalls,
    kVarCallsSlow,
    kVarEventsPassed,
    kVarEventsPassedDP,
    kVarEventsPassedRaw,
    kVarEventsPassthrough,
    kVarEventsSlow,
    kVarTotalPrescale,
    kVarL1PassEvents,
    kVarHLTEvents,
    kVarHLTPassEvents,
    kVarHLTPUs,
    kVarROI,
    kVarType,
    kVarEta,
    kVarPhi,
    kVarArea,
    kVarL1Thresh,
    kVarEventsPerLumiblock,
    kVarOverlap,
    kVarOverlapDP,
    // Decoration ENUMs
    kDecStartTime,
    kDecCallOrCache,
    kDecROIString,
    kDecROSString,
    kDecAlgClassName,
    kDecChainName,
    kDecSeqName,
    kDecLbLength,
    kDecType,
    kDecRatesGroupName,
    kDecPrescale,
    // END of config ENUM keys
    kConfKey_SIZE //!< Number of configuration keys - keep me as last entry
  };

  // Typedefs - used throughout
  // All iterators are implicitly const unless they contain "NonConst".
  typedef std::map< Int_t, std::string>       IntStringMap_t;
  typedef IntStringMap_t::const_iterator      IntStringMapIt_t;
  typedef std::map< Int_t, Int_t>             IntIntMap_t;
  typedef IntIntMap_t::const_iterator         IntIntMapIt_t;
  typedef std::map< Int_t, Float_t>           IntFloatMap_t;
  typedef IntFloatMap_t::const_iterator       IntFloatMapIt_t;
  typedef std::map< std::string, std::string> StringStringMap_t;
  typedef StringStringMap_t::const_iterator   StringStringMapIt_t;
  typedef std::map< std::string, Int_t>       StringIntMap_t;
  typedef StringIntMap_t::const_iterator      StringIntMapIt_t;
  typedef StringIntMap_t::iterator            StringIntMapNonConstIt_t;
  typedef std::map< std::string, Float_t>     StringFloatMap_t;
  typedef StringFloatMap_t::const_iterator    StringFloatMapIt_t;

  typedef std::map< ConfKey_t, std::string> confStringMap_t;
  typedef confStringMap_t::const_iterator   confStringMapIt_t;
  typedef std::map< ConfKey_t, Int_t>       confIntMap_t;
  typedef confIntMap_t::const_iterator      confIntMapIt_t;
  typedef std::map< ConfKey_t, Float_t>     confFloatMap_t;
  typedef confFloatMap_t::const_iterator    confFloatMapIt_t;

  typedef std::map< ConfKey_t, MonitorBase* >  monitorMap_t;
  typedef monitorMap_t::const_iterator         monitorIt_t;
  typedef monitorMap_t::iterator               monitorNonConstIt_t;
  typedef std::pair< ConfKey_t, MonitorBase* > monitorPair_t;

  typedef std::map< std::string, CounterBase* > CounterMap_t;
  typedef CounterMap_t::const_iterator          CounterMapIt_t;

  typedef std::map< std::string, CounterMap_t > CounterCollection_t;
  typedef CounterCollection_t::const_iterator   CounterCollectionIt_t;
  typedef CounterCollection_t::iterator         CounterCollectionNonConstIt_t;

  typedef std::set< CounterMap_t* >       CounterMapSet_t;
  typedef CounterMapSet_t::const_iterator CounterMapSetIt_t;

  typedef std::set< CounterBase* >     CounterSet_t;
  typedef CounterSet_t::const_iterator CounterSetIt_t;

  typedef std::pair< ConfKey_t, VariableOption_t >  ConfVariableOptionPair_t;
  typedef std::vector< ConfVariableOptionPair_t >   VariableOptionVector_t; //!< Structure to hold a reference to what is stored in a DataStore
  
  typedef std::map< ConfKey_t, DataVariable* >::const_iterator VariableMapIt_t; //!< Internal use name to DataVariable pointer iterator typedef.

  typedef std::set<CounterBaseRates*>           CounterBaseRatesSet_t;
  typedef CounterBaseRatesSet_t::const_iterator CounterBaseRatesSetIt_t;

  typedef std::set<RatesChainItem*>      ChainItemSet_t;
  typedef ChainItemSet_t::const_iterator ChainItemSetIt_t;

  typedef std::map< std::string, RatesChainItem* > ChainItemMap_t;
  typedef ChainItemMap_t::const_iterator           ChainItemMapIt_t;


  // Helper functions
  // These need to find a more permanent home
  
  Int_t stringToInt(const std::string &_i);
  Float_t stringToFloat(const std::string &_i);
  std::string intToString(Int_t _i, UInt_t _pad = 0);
  std::string intToString(UInt_t _i, UInt_t _pad = 0);
  std::string floatToString(Float_t _f, Int_t _precision = 4);
  std::string doubleToString(Double_t _d, Int_t _precision = 4);
  void plotText(Double_t x, Double_t y, const char *text);
  void plotHistogram(TH1F* _h, Bool_t _isLogY = kTRUE, std::string _opt = "");
  Bool_t isZero(Float_t _float, Float_t _precision = 0.00000001);
  Bool_t isEqual(Float_t _float1, Float_t _float2, Float_t _precision = 0.00000001);
  ConfVariableOptionPair_t makePair(ConfKey_t _name, VariableOption_t _vo);
  Int_t stringToIntHash( std::string );
  Int_t intIntToIntHash( Int_t _a, Int_t _b );
  const std::string& getLevelString(UInt_t _level);
  
  class JsonExport {
  public:

    JsonExport() : m_level(0), m_hasLeaves(kFALSE), m_justEnded(kFALSE), m_minimal(kFALSE) {
    }

    void addNode(std::ostream& _fout, std::string _name, std::string _icon = "") {

      if (!m_minimal) _fout << std::string(m_level*2, ' ');
      if (m_justEnded == kTRUE) _fout << ",";
      _fout << "{";
      if (!m_minimal) _fout << std::endl;
      m_justEnded = kFALSE;

      ++m_level;

      if (!m_minimal) _fout << std::string(m_level*2, ' ');
      _fout << "\"text\": \"" << _name << "\",";
      if (!m_minimal) _fout << std::endl << std::string(m_level*2, ' ');
      if (_icon != "") _fout << "\"icon\": \"images/" << _icon << ".png\",";
      if (!m_minimal && _icon != "") _fout << std::endl << std::string(m_level*2, ' ');
      _fout << "\"children\": [";
      ++m_level;
      if (!m_minimal) _fout << std::endl;
    }

    void addLeaf(std::ostream& _fout, std::string _name, std::string _icon = "") {
      if (m_hasLeaves != 0) _fout << "," << std::endl;
      m_hasLeaves = kTRUE;
      if (!m_minimal)  _fout << std::string(m_level*2, ' ');
      _fout << "{\"text\": \"" << _name << "\"";
      if (_icon != "") _fout <<  ", \"icon\": \"images/" << _icon << ".png\"";
      _fout << "}";
    }

    void addLeafCustom(std::ostream& _fout, std::string _name, std::string _value, std::string _name2 = "", std::string _value2 = "") {
      if (m_hasLeaves != 0) _fout << "," << std::endl;
      m_hasLeaves = kTRUE;
      if (!m_minimal)  _fout << std::string(m_level*2, ' ');
      _fout << "{\"" << _name <<  "\": \"" << _value << "\"";
      if (_name2 != "") {
        _fout << ", \"" << _name2 <<  "\": \"" << _value2 << "\"}";
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


      if (!m_minimal) _fout << std::string(m_level*2, ' ');
      _fout << "]";
      --m_level; // Leaves


      if (!m_minimal) _fout << std::endl;
      _fout << std::string(m_level*2, ' ') << "}" << std::endl; //always do this endl to make the file not crazy
      --m_level; // Node
    }

    UInt_t m_level;
    Bool_t m_hasLeaves;
    Bool_t m_justEnded;
    Bool_t m_minimal;
  };

}



// Style constants. To be removed

namespace TrigCostStyle {
  static const UInt_t nBinsTime = 500;
  static const Float_t xMinTime = 0.;
  static const Float_t xMaxTime = 1000.;
  
  static const UInt_t nBinsAlgCalls = 51;
  static const Float_t xMinAlgCalls = -0.5;
  static const Float_t xMaxAlgCalls = 50.5;
  
  
  //This was a bad idea.
  //static const Float_t xTimeBins[] = {.1,.2,.3,.4,.5,.6,.7,.8,.9,1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,100,200,300,400,500,600,700,800,900,1000,2000,3000,4000,5000,6000,7000,8000,9000,10000};
  // Precomputed logarithmic binning covering 1e-1 to 1e4 : this is probably redundant
  static const Float_t xTimeBins[] = {0.1, 0.10685, 0.114584, 0.123317, 0.133179, 0.144313, 0.156886, 0.171083, 0.187114,
                                      0.205215, 0.225654, 0.248733, 0.274792, 0.304217, 0.337443, 0.374961, 0.417323, 0.465158, 0.51917,
                                      0.580159, 0.649024, 0.726785, 0.814588, 0.913732, 1.02568, 1.15209, 1.29482, 1.45599, 1.63798,
                                      1.84347, 2.0755, 2.33751, 2.63335, 2.9674, 3.34459, 3.7705, 4.25143, 4.79447, 5.40764,
                                      6.10001, 6.88181, 7.76458, 8.76137, 9.8869, 11.1578, 12.5928, 14.2132, 16.0429, 18.1089,
                                      20.4418, 23.0759, 26.0503, 29.4088, 33.2011, 37.4832, 42.3183, 47.778, 53.9428, 60.9039,
                                      68.764, 77.6393, 87.661, 98.977, 111.755, 126.182, 142.474, 160.869, 181.641, 205.095,
                                      231.578, 261.482, 295.249, 333.376, 376.428, 425.04, 479.932, 541.912, 611.898, 690.923,
                                      780.155, 880.912, 994.683, 1123.15, 1268.2, 1432, 1616.94, 1825.78, 2061.58, 2327.85,
                                      2628.5, 2967.98, 3351.32, 3784.16, 4272.9, 4824.78, 5447.92, 6151.56, 6946.07, 7843.2,
                                      8856.21, 10000
                                     };
  static const Int_t xTimeBinsN = 100;
  
}

#endif //TrigCostRootAnalysis_Utility_H
