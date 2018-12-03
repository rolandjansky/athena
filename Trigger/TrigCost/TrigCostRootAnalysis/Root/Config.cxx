// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <limits.h>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <float.h> // FLT_MIN
#include <getopt.h> //GCC CLI parsing
#include "cstdlib" //std::getenv

// Local include(s):
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/ProcessEvent.h"

// ATHENA Includes
#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif // not ROOTCORE

// ROOT include(s):
#include <TChain.h>
#include <TTree.h>
#include <TError.h>

namespace TrigCostRootAnalysis {
  /**
   * Singleton class retrieval call.
   * @return Reference to the singleton class object.
   */
  Config& Config::config() { // Instance getter
    static Config instance; // Guaranteed to be destroyed.

    return instance;        // Instantiated on first use.
  }

  /**
   * Parse the command line options provided to the application. Setup all configuration keys which are expected to
   * be present by the application.
   * @param argc Argument characters, to be forwarded straight from int main().
   * @param argv Characters, to be forwarded straight from int main().
   * @returns kTRUE if execution can continue. Or Error and kFALSE if vital information was not provided by the user.
   */
  Bool_t Config::parseCLI(int argc, char* argv[]) {
    // Flags

    // Level
    static Int_t _doL2 = kFALSE;
    static Int_t _doEF = kFALSE;
    static Int_t _doL2EF = kFALSE;
    static Int_t _doHLT = kFALSE;
    // Range Summaries
    static Int_t _summaryPerHLTConfig = kFALSE;
    static Int_t _summaryPerLumiBlock = kFALSE;
    static Int_t _summaryAll = kFALSE;
    // Monitors
    static Int_t _monitorAll = kFALSE;
    static Int_t _monitorChains = kFALSE;
    static Int_t _monitorChainAlgs = kFALSE;
    static Int_t _monitorSequences = kFALSE;
    static Int_t _monitorSequenceAlgs = kFALSE;
    static Int_t _monitorAlgs = kFALSE;
    static Int_t _monitorAlgClass = kFALSE;
    static Int_t _monitorAllChainSeqAlgs = kFALSE;
    static Int_t _monitorROS = kFALSE;
    static Int_t _monitorAllROS = kFALSE;
    static Int_t _monitorROBIN = kFALSE;
    static Int_t _monitorROSAlgs = kFALSE;
    static Int_t _monitorROSChains = kFALSE;
    static Int_t _monitorL1 = kFALSE;
    static Int_t _monitorFullEvent = kFALSE;
    static Int_t _monitorGlobals = kFALSE;
    static Int_t _monitorEventProfile = kFALSE;
    static Int_t _monitorROI = kFALSE;
    static Int_t _monitorRates = kFALSE;
    static Int_t _monitorRatesUpgrade = kFALSE;
    static Int_t _monitorSliceCPU = kFALSE;
    // Output
    static Int_t _doOutputPng = kFALSE;
    static Int_t _doOutputPdf = kFALSE;
    static Int_t _doOutputHist = kFALSE;
    static Int_t _doOutputCsv = kFALSE;
    static Int_t _doOutputXML = kFALSE;
    static Int_t _doOutputRatesGraph = kFALSE;
    static Int_t _doOutputMenus = kFALSE;
    static Int_t _doOutputCanvas = kFALSE;
    static Int_t _linkOutputDirectory = kFALSE;
    static Int_t _writeDummyPSXML = kFALSE;
    // Misc
    static Int_t _ratesMode = kFALSE;
    static Int_t _ratesUpgradeMode = kFALSE;
    static Int_t _costMode = kFALSE;
    static Int_t _onlineMode = kFALSE;
    static Int_t _outputModeStandard = kFALSE;
    static Int_t _outputTagFromAthena = kFALSE;
    static Int_t _cleanAll = kFALSE;
    static Int_t _doEBWeighting = kFALSE;
    static Int_t _writeEBWeightXML = kFALSE;
    static Int_t _debug = kFALSE;
    static Int_t _ratesForcePass = kFALSE;
    static Int_t _ratesScaleByPS = kFALSE;
    static Int_t _matchL1RandomToOnline = kFALSE;
    static Int_t _doUniqueRates = kFALSE;
    static Int_t _doGroupOverlap = kFALSE;
    static Int_t _doAllOverlap = kFALSE;
    static Int_t _showVersion = kFALSE;
    static Int_t _extraplolate8To13 = kFALSE;
    static Int_t _extraplolate13To5 = kFALSE;
    static Int_t _doNotWriteMetadata = kFALSE;
    static Int_t _noMsgSup = kFALSE;
    static Int_t _noOnlineDTCorrection = kFALSE;
    static Int_t _noUpgradePileupScaling = kFALSE;
    static Int_t _noUpgradeBunchScaling = kFALSE;
    static Int_t _doCPS = kFALSE;
    static Int_t _patternsInvert = kFALSE;
    static Int_t _isCPUPrediction;
    static Int_t _doUpgradeRatesScan = kFALSE;
    static Int_t _patternsExactMatch = kFALSE;
    static Int_t _ignoreNonPhysBunchGroups = kFALSE;
    static Int_t _noLBRescaling = kFALSE;
    static Int_t _useDefaultLumiScalingExceptions = kFALSE;
    static Int_t _useDefaultExponentialScalingList = kFALSE;
    static Int_t _upgradeMergeTOBOverlap = kFALSE;
    static Int_t _doExponentialMu = kFALSE;
    static Int_t _invertHighMuRunVeto = kFALSE;
    static Int_t _ignoreGRL = kFALSE;
    static Int_t _ignorePSGreaterThanOne = kFALSE;

    // User options
    std::vector< std::string > _inputFiles;
    std::vector< std::string > _patternsMonitor;
    std::vector< std::string > _patternsOutput;
    std::vector< std::string > _patternsUnique;
    std::vector< std::string > _patternsExclude;
    std::vector< std::string > _patternsOverlap;
    std::vector< std::string > _patternsNoLumiWeight;
    std::vector< std::string > _patternsNoMuLumiWeight;
    std::vector< std::string > _patternsNoBunchLumiWeight;
    std::vector< std::string > _patternsExpoMuLumiWeight;
    std::vector< Int_t > _useOnlyTheseBCIDs;
    std::vector< Int_t > _eventPicker;
    std::string _fileList = "";
    std::string _fullEventFileList = "";
    std::string _treeName = "trig_cost";
    std::string _L2Prefix = "TrigCostL2_";
    std::string _EFPrefix = "TrigCostEF_";
    std::string _HLTPrefix = "TrigCostHLT_";
    std::string _configPrefix = "trig_costMeta/TrigConfTree";
    std::string _outRootFilename = "TrigCostRoot_Results.root";
    std::string _outImageDirectory = "images";
    std::string _outCSVDirectory = "csv";
    std::string _outXMLDirectory = "xml";
    std::string _outRatesGraphFilename = "ratesGraph.json";
    std::string _outRatesWarningsFilename = "rates_WARNINGS.txt";
    std::string _outDirectory = "costMonitoring_%t_%r";
    std::string _outRootDirectory = ".";
    std::string _outTag = "LOCAL";
    std::string _userDetails = "";
    std::string _menuXML = "";//"HLTconfig_Physics_pp_v5_19.2.0.xml";
    std::string _prescaleXML1 = "";//"cool_208354_366_366.xml"; // This is an old XML for test purposes
    std::string _prescaleXML2 = "";
    std::string _ROSXML = "rob-ros-robin-2015.xml";
    std::string _version = "TrigCostRootAnalysis-00-10-13";
    std::string _upgradeScenario = "";
    std::string _jira = "";
    std::string _multiRun = "";
    std::string _AutoMonXMLName = "AutoMonConfig.xml";

    Int_t _lbBegin = INT_MIN;
    Int_t _lbEnd = INT_MAX;
    UInt_t _nEvents = INT_MAX;
    UInt_t _nSkip = 0;
    UInt_t _nThread = 5;
    UInt_t _nLbFullSummary = 4;
    UInt_t _nLbFullSkip = 50;
    UInt_t _nHLTConfigSummary = 3;
    UInt_t _nLbPerHLTConfig = 20;
    UInt_t _defaultLBLength = 30;
    UInt_t _slowThreshold = 1000;
    UInt_t _slowEventThreshold = 120000;
    UInt_t _histogramBins = 50;
    UInt_t _maxNumberFullEvents = 10;
    UInt_t _ratesOverlapWarning = 80;
    UInt_t _maxMultiSeed = 1;
    UInt_t _runNumber = 0;
    UInt_t _messageWait = 10;
    Float_t _rateFallbackPrescaleL1 = FLT_MIN;
    Float_t _rateFallbackPrescaleHLT = FLT_MIN;
    Float_t _basicWeight = 1.;
    Float_t _predictionLumi = 0.;
    Float_t _runLumi = 0.;
    Float_t _binMin = FLT_MIN;
    Float_t _binMax = FLT_MIN;
    Float_t _targetMu = 0.;
    Float_t _expoRateScaleModifierL1 = 0.105; // From high-mu run
    Float_t _expoRateScaleModifierHLT = 0.111; // From high-mu run

    // Parse CLI
    Int_t _status = 0;
    while (kTRUE) {
      static struct option _longOptions[] = {
        {
          "help", no_argument, 0, 0
        },
        {
          "cleanAll", no_argument, &_cleanAll, 1
        },
        {
          "debug", no_argument, &_debug, 1
        },
        {
          "noMsgSuppression", no_argument, &_noMsgSup, 1
        },
        {
          "doL2", no_argument, &_doL2, 1
        },
        {
          "doEF", no_argument, &_doEF, 1
        },
        {
          "doL2EF", no_argument, &_doL2EF, 1
        },
        {
          "doHLT", no_argument, &_doHLT, 1
        },
        {
          "doOutputCanvas", no_argument, &_doOutputCanvas, 1
        },
        {
          "doOutputHist", no_argument, &_doOutputHist, 1
        },
        {
          "doOutputPNG", no_argument, &_doOutputPng, 1
        },
        {
          "doOutputPDF", no_argument, &_doOutputPdf, 1
        },
        {
          "doOutputCSV", no_argument, &_doOutputCsv, 1
        },
        {
          "doOutputRatesXML", no_argument, &_doOutputXML, 1
        },
        {
          "doOutputRatesGraph", no_argument, &_doOutputRatesGraph, 1
        },
        {
          "doOutputMenus", no_argument, &_doOutputMenus, 1
        },
        {
          "outputTagFromAthena", no_argument, &_outputTagFromAthena, 1
        },
        {
          "monitorAll", no_argument, &_monitorAll, 1
        },
        {
          "monitorChains", no_argument, &_monitorChains, 1
        },
        {
          "monitorChainAlgs", no_argument, &_monitorChainAlgs, 1
        },
        {
          "monitorSequences", no_argument, &_monitorSequences, 1
        },
        {
          "monitorSequenceAlgs", no_argument, &_monitorSequenceAlgs, 1
        },
        {
          "monitorAlgs", no_argument, &_monitorAlgs, 1
        },
        {
          "monitorAlgClass", no_argument, &_monitorAlgClass, 1
        },
        {
          "monitorAllChainSeqAlgs", no_argument, &_monitorAllChainSeqAlgs, 1
        },
        {
          "monitorAllChainSeqAlg", no_argument, &_monitorAllChainSeqAlgs, 1
        }, // Back comp
        {
          "monitorROS", no_argument, &_monitorROS, 1
        },
        {
          "monitorAllROS", no_argument, &_monitorAllROS, 1
        },
        {
          "monitorROBIN", no_argument, &_monitorROBIN, 1
        },
        {
          "monitorROSAlgs", no_argument, &_monitorROSAlgs, 1
        },
        {
          "monitorROSChains", no_argument, &_monitorROSChains, 1
        },
        {
          "monitorROI", no_argument, &_monitorROI, 1
        },
        {
          "monitorL1", no_argument, &_monitorL1, 1
        },
        {
          "monitorFullEvent", no_argument, &_monitorFullEvent, 1
        },
        {
          "monitorGlobals", no_argument, &_monitorGlobals, 1
        },
        {
          "monitorEventProfile", no_argument, &_monitorEventProfile, 1
        },
        {
          "monitorRates", no_argument, &_monitorRates, 1
        },
        {
          "monitorRatesUpgrade", no_argument, &_monitorRatesUpgrade, 1
        },
        {
          "monitorSliceCPU", no_argument, &_monitorSliceCPU, 1
        },
        {
          "useEBWeight", no_argument, &_doEBWeighting, 1
        },
        {
          "doCPS", no_argument, &_doCPS, 1
        },
        {
          "writeEBXML", no_argument, &_writeEBWeightXML, 1
        },
        {
          "summaryPerHLTConfig", no_argument, &_summaryPerHLTConfig, 1
        },
        {
          "summaryPerLumiBlock", no_argument, &_summaryPerLumiBlock, 1
        },
        {
          "summaryAll", no_argument, &_summaryAll, 1
        },
        {
          "forceAllPass", no_argument, &_ratesForcePass, 1
        },
        {
          "doUniqueRates", no_argument, &_doUniqueRates, 1
        },
        {
          "doGroupOverlap", no_argument, &_doGroupOverlap, 1
        },
        {
          "doAllOverlap", no_argument, &_doAllOverlap, 1
        },
        {
          "scaleRatesByPS", no_argument, &_ratesScaleByPS, 1
        },
        {
          "writeDummyPSXML", no_argument, &_writeDummyPSXML, 1
        },
        {
          "version", no_argument, &_showVersion, 1
        },
        {
          "linkOutputDirectory", no_argument, &_linkOutputDirectory, 1
        },
        {
          "extrapolate8To13", no_argument, &_extraplolate8To13, 1
        },
        {
          "extrapolate13To5", no_argument, &_extraplolate13To5, 1
        },
        {
          "doNotWriteMetadata", no_argument, &_doNotWriteMetadata, 1
        },
        {
          "matchL1RandomToOnline", no_argument, &_matchL1RandomToOnline, 1
        },
        {
          "ratesUpgradeMode", no_argument, &_ratesUpgradeMode, 1
        },
        {
          "ratesMode", no_argument, &_ratesMode, 1
        },
        {
          "costMode", no_argument, &_costMode, 1
        },
        {
          "onlineMode", no_argument, &_onlineMode, 1
        },
        {
          "outputModeStandard", no_argument, &_outputModeStandard, 1
        },
        {
          "noOnlineDTCorrection", no_argument, &_noOnlineDTCorrection, 1
        },
        {
          "noUpgradePileupScaling", no_argument, &_noUpgradePileupScaling, 1
        },
        {
          "noUpgradeBunchScaling", no_argument, &_noUpgradeBunchScaling, 1
        },
        {
          "patternsInvert", no_argument, &_patternsInvert, 1
        },
        {
          "isCPUPrediction", no_argument, &_isCPUPrediction, 1
        },
        {
          "doUpgradeRatesScan", no_argument, &_doUpgradeRatesScan, 1
        },
        {
          "patternsExactMatch", no_argument, &_patternsExactMatch, 1
        },
        {
          "ignoreNonPhyBunchGroups", no_argument, &_ignoreNonPhysBunchGroups, 1
        },
        {
          "noLBRescaling", no_argument, &_noLBRescaling, 1
        },
        {
          "useDefaultLumiScalingExceptions", no_argument, &_useDefaultLumiScalingExceptions, 1
        },
        {
          "useDefaultExponentialScalingList", no_argument, &_useDefaultExponentialScalingList, 1
        },
        {
          "upgradeMergeTOBOverlap", no_argument, &_upgradeMergeTOBOverlap, 1
        },
        {
          "doExponentialMu", no_argument, &_doExponentialMu, 1
        },
        {
          "ignoreGRL", no_argument, &_ignoreGRL, 1
        },  
        {
          "ignorePSGreaterThanOne", no_argument, &_ignorePSGreaterThanOne, 1
        }, 
        {
          "invertHighMuRunVeto", no_argument, &_invertHighMuRunVeto, 1
        }, // Hidden option
        {
          "treeName", required_argument, 0, 't'
        },
        {
          "prescaleXML", required_argument, 0, 'M'
        },
        {
          "prescaleXML1", required_argument, 0, 'g'
        },
        {
          "prescaleXML2", required_argument, 0, 'G'
        },
        {
          "ROSXML", required_argument, 0, 'R'
        },
        {
          "outputRootFile", required_argument, 0, 'r'
        },
        {
          "outputImageDirectory", required_argument, 0, 'i'
        },
        {
          "outputCSVDirectory", required_argument, 0, 'C'
        },
        {
          "outputXMLDirectory", required_argument, 0, 'x'
        },
        {
          "outputDirectory", required_argument, 0, 'o'
        },
        {
          "outputRootDirectory", required_argument, 0, 'O'
        },
        {
          "outputTag", required_argument, 0, 'T'
        },
        {
          "histogramBins", required_argument, 0, 'h'
        },
        {
          "histogramBinMin", required_argument, 0, 'j'
        },
        {
          "histogramBinMax", required_argument, 0, 'J'
        },
        {
          "prefixL2", required_argument, 0, 'l'
        },
        {
          "prefixEF", required_argument, 0, 'E'
        },
        {
          "prefixHLT", required_argument, 0, 'H'
        },
        {
          "prefixConfig", required_argument, 0, 'c'
        },
        {
          "lbBegin", required_argument, 0, 'b'
        },
        {
          "lbEnd", required_argument, 0, 'e'
        },
        {
          "basicWeight", required_argument, 0, 'B'
        },
        {
          "nEvents", required_argument, 0, 'n'
        },
        {
          "nSkip", required_argument, 0, 's'
        },
        {
          "nThreads", required_argument, 0, 'W'
        },
        {
          "nFullEventSummaries", required_argument, 0, 'F'
        },
        {
          "fullEvNumFilelist", required_argument, 0, 'V'
        },
        {
          "slowThreshold", required_argument, 0, 'S'
        },
        {
          "slowEventThreshold", required_argument, 0, 'y'
        },
        {
          "defaultLBLength", required_argument, 0, 'd'
        },
        {
          "predictionLumi", required_argument, 0, 'D'
        },
        {
          "runLumi", required_argument, 0, 'U'
        },
        {
          "nLbFullSummary", required_argument, 0, 'L'
        },
        {
          "nLbFullSkip", required_argument, 0, 'k'
        },
        {
          "ratesOverlapWarning", required_argument, 0, 'w'
        },
        {
          "files", required_argument, 0, 'f'
        },
        {
          "fileList", required_argument, 0, 'K'
        },
        {
          "patternsMonitor", required_argument, 0, 'p'
        },
        {
          "patternsOutput", required_argument, 0, 'P'
        },
        {
          "patternsUnique", required_argument, 0, 'I'
        },
        {
          "patternsExclude", required_argument, 0, 'v'
        },
        {
          "patternsOverlap", required_argument, 0, 'Y'
        },
        {
          "userDetails", required_argument, 0, 'u'
        },
        {
          "maxMultiSeed", required_argument, 0, 'X'
        },
        {
          "runNumber", required_argument, 0, 'N'
        },
        {
          "rateFallbackPrescaleL1", required_argument, 0, 'a'
        },
        {
          "rateFallbackPrescaleHLT", required_argument, 0, 'A'
        },
        {
          "nHLTConfigSummary", required_argument, 0, 'q'
        },
        {
          "nLbPerHLTConfig", required_argument, 0, 'Q'
        },
        {
          "upgradeScenario", required_argument, 0, 'm'
        },
        {
          "messageWait", required_argument, 0, 'z'
        },
        {
          "jira", required_argument, 0, 'Z'
        },
        {
          "targetMu", required_argument, 0, '0'
        }, // allow either
        {
          "predictionMu", required_argument, 0, '0'
        }, // allow either
        {
          "patternsNoLumiWeight", required_argument, 0, '1'
        },
        {
          "patternsNoMuLumiWeight", required_argument, 0, '2'
        },
        {
          "patternsNoBunchLumiWeight", required_argument, 0, '3'
        },
        {
          "patternsExpoMuLumiWeight", required_argument, 0, '4'
        },
        {
          "expoRateScaleModifierL1", required_argument, 0, '5'
        },
        {
          "expoRateScaleModifier", required_argument, 0, '6'
        }, //alow either
        {
          "expoRateScaleModifierHLT", required_argument, 0, '6'
        }, //alow either
        {
          "multiRun", required_argument, 0, '7'
        }, // Hidden option
        {
          "useOnlyTheseBCIDs", required_argument, 0, '8'
        }, // Hidden option

        {
          0, 0, 0, 0
        }
      };

      Int_t _optionIndex = 0;
      _status = getopt_long_only(argc, argv, "betlEf:", _longOptions, &_optionIndex);
      if (_status == -1) {
        break;
      }

      std::stringstream _ss;
      switch (_status) {
      case 0:
        // I set a flag
        if (_longOptions[_optionIndex].flag != 0) {
          break;
        }
        //std::cout << "Given option " << _longOptions[_optionIndex].name;
        if (std::string(_longOptions[_optionIndex].name) == "help") {
          std::cout << "INFO: Trig Cost Root Analysis Program Usage:- " << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ OPERATING MODE ALIASES ~~~~~~~~~~~~~~~" << std::endl;
          std::cout <<
            "--costMode\t\t\t\t\tAlias for: --cleanAll --doHLT --summaryAll --monitorAllChainSeqAlgs --monitorAllROS --monitorROI --monitorGlobals --monitorFullEvent --monitorEventProfile --monitorSliceCPU --ignoreNonPhyBunchGroups --outputModeStandard"
                    << std::endl;
          std::cout <<
            "--onlineMode\t\t\t\t\tAlias for: --cleanAll --summaryPerHLTConfig --summaryPerLumiBlock --monitorAllChainSeqAlg --monitorROS --monitorROBIN --monitorROI --monitorGlobals --monitorFullEvent --monitorEventProfile --monitorSliceCPU --outputModeStandard"
                    << std::endl;
          std::cout <<
            "--ratesMode\t\t\t\t\tAlias for: --cleanAll --doHLT --summaryAll --monitorRates --useEBWeight --matchL1RandomToOnline --doCPS --useDefaultLumiScalingExceptions --useDefaultExponentialScalingList --outputModeStandard"
                    << std::endl;
          std::cout <<
            "--outputModeStandard\t\t\t\tAlias for: --doOutputHist --doOutputCSV --doOutputRatesGraph --doOutputRatesXML --doOutputMenus"
                    << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ HLT LEVELS TO PROCESS ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--doL2\t\t\t\t\t\tProcess just Level 2 cost data if present in ntuple." << std::endl;
          std::cout << "--doEF\t\t\t\t\t\tProcess just Event Filter cost data if present in ntuple." << std::endl;
          std::cout << "--doL2EF\t\t\t\t\tProcess Level 2 and Event Filter cost data if present in ntuple." <<
            std::endl;
          std::cout << "--doHLT\t\t\t\t\t\t[Default] Process HLT (unified L2 and EF) cost data if present in ntuple." <<
            std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ TIME RANGES TO MONITOR  ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--summaryAll\t\t\t\t\tOutput a summary for all events processed." << std::endl;
          std::cout <<
            "--summaryPerHLTConfig\t\t\t\t[Default] Output a summary for each set of trigger keys processed." <<
            std::endl;
          std::cout << "--summaryPerLumiBlock\t\t\t\tOutput a summary for each for the first " << _nLbFullSummary <<
            " lumi block(s) (see --nLbFullSummary)." << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ AREAS TO MONITOR ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--monitorAll\t\t\t\t\t[Default] Enable all the following monitors." << std::endl;
          std::cout << "--monitorChains\t\t\t\t\tPerform cost monitoring for HLT chains." << std::endl;
          std::cout << "--monitorChainAlgs\t\t\t\tPerform cost monitoring for Algorithms associated to HLT Chains." <<
            std::endl;
          std::cout << "--monitorSequences\t\t\t\tPerform cost monitoring for HLT sequences." << std::endl;
          std::cout <<
            "--monitorSequenceAlgs\t\t\t\tPerform cost monitoring for Algorithms associated to HLT sequences." <<
            std::endl;
          std::cout << "--monitorAlgs\t\t\t\t\tPerform cost monitoring for individual HLT algorithms." << std::endl;
          std::cout << "--monitorAlgClass\t\t\t\tPerform cost monitoring for HLT algorithm class types." << std::endl;
          std::cout <<
            "--monitorAllChainSeqAlgs\t\t\tEnables all of monitorChains, monitorChainAlgs, monitorSequences, monitorSequenceAlgs, monitorAlgs and monitorAlgClass."
                    << std::endl;
          std::cout << "--monitorSliceCPU\t\t\tPerform CPU monitoring per trigger slice." << std::endl;
          std::cout <<
            "--monitorROS\t\t\t\t\tPerform cost monitoring for data flow and time in the Read Out System and enables ROS data association with algs."
                    << std::endl;
          std::cout <<
            "--monitorROBIN\t\t\t\t\tPerform cost monitoring for data flow and time in the Read Out System at the ROBIN level."
                    << std::endl;
          std::cout <<
            "--monitorROSAlgs\t\t\t\tPerform cost monitoring for data flow and time in the Read Out System subdivided per algorithm."
                    << std::endl;
          std::cout <<
            "--monitorROSChains\t\t\t\tPerform cost monitoring for data flow and time in the Read Out System subdivided per chain."
                    << std::endl;
          std::cout <<
            "--monitorAllROS\t\t\t\t\tEnables all of monitorROS, monitorROBIN, monitorROSAlgs and monitorROSChains." <<
            std::endl;
          std::cout << "--monitorROI\t\t\t\t\tPerform cost monitoring for Regions of Interest in the trigger." <<
            std::endl;
          std::cout <<
            "--monitorGlobals\t\t\t\tRecord global monitoring quantities for the whole HLT per lumi block." <<
          std::endl;
          std::cout <<
            "--monitorEventProfile\t\t\t\tBreakdown what an average event looks like, including with some (hardcoded) preselections."
                    << std::endl;
          std::cout << "--monitorFullEvent\t\t\t\tFor select events, save detailed record of algorithm execution." <<
            std::endl;
          std::cout <<
            "--monitorRates\t\t\t\t\tPerform rate monitoring on chains and overlaps with other chains in the same group."
                    << std::endl;
          std::cout << "--monitorRatesUpgrade\t\t\t\tRate monitoring with customizable menu and pileup settings." <<
            std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ OUTPUT TO GENERATE ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--doOutputCanvas\t\t\t\tSave results as decorated canvases in output ROOT file." << std::endl;
          std::cout << "--doOutputHist\t\t\t\t\tSave results as raw histograms in output ROOT file." << std::endl;
          std::cout << "--histogramBins " << _histogramBins << "\t\t\t\tNumber of bins to use per histogram." <<
            std::endl;
          std::cout <<
            "--doOutputPNG\t\t\t\t\tSave results as PNG images in folder structure (warning, best used with --patternsOutput)."
                    << std::endl;
          std::cout <<
            "--doOutputPDF\t\t\t\t\tSave results as PDF images in folder structure (warning, best used with --patternsOutput)."
                    << std::endl;
          std::cout << "--doOutputCSV\t\t\t\t\tSave results as CSV table summaries." << std::endl;
          std::cout << "--doOutputRatesGraph\t\t\t\tSave rates chain-graphs in JSON format." << std::endl;
          std::cout << "--doOutputRatesXML\t\t\t\tSave rates XML files for the Rulebook etc." << std::endl;
          std::cout << "--doOutputMenus\t\t\t\t\tSave menu configurations in JSON format." << std::endl;
          std::cout <<
            "--linkOutputDirectory\t\t\t\tSoft link the output directory to 'costMon' in the run directory for automated scripts to use the data."
                    << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ RUN CONFIGURATION ~~~~~~~~~~~~~~~" << std::endl;
#ifdef MTHREAD
          std::cout << "--nThreads " << _nThread <<
            "\t\t\t\tNumber of concurrent threads to use when running monitors. Setting to 1 disables multitasking." <<
            std::endl;
#endif
          std::cout << "--lbBegin INT_MIN\t\t\t\tLowest value luminosity block from input to use." << std::endl;
          std::cout << "--lbEnd INT_MAX\t\t\t\t\tHighest value luminosity block from input to use." << std::endl;
          std::cout << "--jira\t\t\t\t\t\tLink to a bug report associated with this processing." << std::endl;
          std::cout <<
            "--useEBWeight\t\t\t\t\tApply precalculated weights to un-weight EnhancedBias data to MinimumBias data." <<
            std::endl;
          std::cout << "--basicWeight " << _basicWeight <<
            "\t\t\t\t\tBase event weight. Can be used to apply global scaling factor to run." << std::endl;
          std::cout <<
            "--extrapolate8To13\t\t\t\tExperimental parameter! Attempt to evolve 8 TeV data/MC to 13 TeV based on HLT result."
                    << std::endl;
          std::cout <<
            "--extrapolate13To5\t\t\t\tExperimental parameter! Attempt to devolve 13 TeV data/MC to 5 TeV based on HLT result."
                    << std::endl;
          std::cout << "--nEvents INT_MAX\t\t\t\tNumber of input events from all files to run over." << std::endl;
          std::cout << "--nSkip " << _nSkip << "\t\t\t\t\tNumber of events to skip." << std::endl;
          std::cout << "--slowThreshold " << _slowThreshold <<
            "\t\t\t\tTime in milliseconds. Execution times greater than this are flagged as slow." << std::endl;
          std::cout << "--slowEventThreshold " << _slowEventThreshold <<
            "\t\t\tTime in milliseconds. Full event executions times greater than this are flagged as slow." <<
          std::endl;
          std::cout << "--nLbFullSummary " << _nLbFullSummary <<
            "\t\t\t\tNumber of luminosity blocks, starting from lbBegin, to produce a full summary for." << std::endl;
          std::cout << "--nLbFullSkip " << _nLbFullSkip <<
            "\t\t\t\tHow many luminosity blocks to skip per full lb summary." << std::endl;
          std::cout << "--nHLTConfigSummary " << _nHLTConfigSummary << "\t\t\t\tHow many HLT configs to monitor." <<
            std::endl;
          std::cout << "--nLbPerHLTConfig " << _nLbPerHLTConfig <<
            "\t\t\t\tHow many LB to process per key summary, counted from the LB of the 1st event seen with this config."
                    << std::endl;
          std::cout << "--nFullEventSummaries " << _maxNumberFullEvents <<
            "\t\t\tMax number of events (picked at random) to save full executions records for." << std::endl;
          std::cout << "--defaultLBLength " << _defaultLBLength <<
            "\t\t\t\tDefault lumi block length in seconds. Used as an approximate fallback for inputs without LB length data."
                    << std::endl;
          std::cout <<
            "--noLBRescaling\t\t\t\t\tFlag to prevent the rescaling of the effective time per LB in EB runs based on the events processed and the known run size."
                    << std::endl;
          std::cout <<
            "--ignoreGRL\t\t\t\tFlag to switch off the exclusion of LB which fail the good run lists in ehnaced bias runs."
                    << std::endl;
          std::cout <<
            "--patternsMonitor patt1 patt2 ...\t\tPatterns to match in names when running. Regex currently NOT supported. Partial matched allowed. Only entries which match will be analysed."
                    << std::endl;
          std::cout <<
            "--patternsOutput patt1 patt2 ...\t\tPatterns to match in names when saving results. Regex currently NOT supported. Partial matched allowed. Only entries which match will be included in the output."
                    << std::endl;
          std::cout <<
            "--patternsInvert\t\t\t\tInvert the behaviour of --patternsMonitor and --patternsOutput to be a list of chains to explicitly exclude."
                    << std::endl;
          std::cout <<
            "--patternsExactMatch\t\t\t\tUse exact string matching rather than the default fuzzy matching." <<
          std::endl;
          std::cout <<
            "--fullEvNumFilelist\t\t\t\tInput file list with one event number per line. If supplied, only HLT events matching will be used in Full Event Summaries."
                    << std::endl;
          std::cout <<
            "--isCPUPrediction\t\t\t\tIn this mode TrigCost assumes it's running on EnhancedBias data reprocessed with physics prescales and with the costmonitor chain unprescaled."
                    << std::endl;
          std::cout <<
            "--ignoreNonPhyBunchGroups\t\t\tSet by default for costMode, non-physics bunch groups are given weight 0 and hence ignored. Only works if --useEBWeight is true"
                    << std::endl;
          std::cout <<
            "--useOnlyTheseBCIDs BCID1 BCID2 ... \t\t\tGive a separated list of BCIDs. All other BICDs will be skipped. "
                    << std::endl;
          std::cout << "--debug\t\t\t\t\t\tEnable debug output." << std::endl;
          std::cout << "--noMsgSuppression\t\t\t\tDo not suppress any output." << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ TRIGGER RATES CONFIGURATION ~~~~~~~~~~~~~~~" << std::endl;
          std::cout <<
            "--predictionMu\t\t\t\t\tSpecify a target pileup for prescaled rates prediction. This enables advanced weighting and affects the rates of explicit L1 random seeded triggers and must be supplied to get accurate predictions here."
                    << std::endl;
          std::cout << "--prescaleXML1 \"" << _prescaleXML1 <<
            "\"\t\t\t\tPrescale/Menu/L1 XML file from which to read custom prescales for rates calculation (place in /data or current dir for Athena use)."
                    << std::endl;
          std::cout << "--prescaleXML2 \"" << _prescaleXML2 <<
            "\"\t\t\t\tSecond Prescale/Menu/L1 XML file. For if you have L1 and HLT values split over two files. (place in /data or current dir for Athena use)."
                    << std::endl;
          std::cout <<
            "--writeDummyPSXML\t\t\t\tGenerate a file Prescales.xml which contains all chains and can be edited to manually prescale the menu. Use the generated file with --prescaleXML1."
                    << std::endl;
          std::cout <<
            "--predictionLumi\t\t\t\tThe desired instantaneous luminosity to be predicted by this processing. In /cm2/s or in multiples of 1e30"
                    << std::endl;
          std::cout <<
            "--runLumi\t\t\t\t\tOverride the effective instantaneous luminosity of the Enhanced Bias data being used. In /cm2/s or in multiples of 1e30"
                    << std::endl;
          std::cout <<
            "--forceAllPass\t\t\t\t\tForce all L1 and HLT chains to pass-raw in every event. Use to isolate the effect of prescales."
                    << std::endl;
          std::cout <<
            "--ignorePSGreaterThanOne\t\t\t\t\tAll prescales greater than 1 will be set to -1."
                    << std::endl;
          std::cout << "--doUniqueRates\t\t\t\t\tCalculate unique rates for chains. Warning, this is slow." <<
            std::endl;
          std::cout << "--doCPS\t\t\t\t\t\tEnable special treatment for chains in coherent prescale groups." <<
            std::endl;
          std::cout << "--doExponentialMu\t\t\t\t\t\tSwitch on exponential in <mu> dependence for some chains." <<
            std::endl;
          std::cout << "--expoRateScaleModifierL1 " << _expoRateScaleModifierL1 <<
            "\t\t\tMultiplier to exponent for L1 exponential in <mu> rates extrapolation." << std::endl;
          std::cout << "--expoRateScaleModifierHLT " << _expoRateScaleModifierHLT <<
            "\t\t\tMultiplier to exponent for HLT exponential in <mu> rates extrapolation." << std::endl;
          std::cout <<
            "--patternsUnique patt1 patt2 ...\t\tPatterns to match in names doing unique rates, recommended to use this rather than doing unique for all."
                    << std::endl;
          std::cout <<
            "--patternsExclude patt1 patt2 ...\t\tWhen doing unique rates, you may need to explicitly exclude some chains (PS:-1). Force this here."
                    << std::endl;
          std::cout <<
            "--patternsOverlap patt1 patt2 ...\t\tPatterns to match in chain names when doing chain-overlap rates, recommended to use this rather than doing unique for all."
                    << std::endl;
          std::cout <<
            "--patternsNoLumiWeight patt1 patt2 ... \t\tPatterns to match in special-case chains which get no lumi weighting. Auto applied to random L1. (req. advanced weighting mode w. --predictionMu)"
                    << std::endl;
          std::cout <<
            "--patternsNoMuLumiWeight patt1 patt2 ... \tPatterns to match in special-case chains which get no <mu> component of lumi weight (req. advanced weighting mode w. --predictionMu)"
                    << std::endl;
          std::cout <<
            "--patternsNoBunchLumiWeight patt1 patt2 ... \tPatterns to match in special-case chains which get no bunch compoment of lumi weighting. Auto applied to random seeded HLT. (req. advanced weighting mode w. --predictionMu)"
                    << std::endl;
          std::cout <<
            "--patternsExpoMuLumiWeight patt1 patt2 ... \tPatterns to match in special-case chains which get lumi extrap. which is exponential in mu (req. advanced weighting mode w. --predictionMu, see also --expoRateScaleModifier)"
                    << std::endl;
          std::cout <<
            "--doGroupOverlap\t\t\t\tCalculate overlaps between all chains within each rate group. Warning, this is slow."
                    << std::endl;
          std::cout << "--doAllOverlap\t\t\t\t\tCalculate overlaps between all chains. Warning, this is very slow.." <<
            std::endl;
          std::cout << "--ratesOverlapWarning " << _ratesOverlapWarning <<
            "%\t\t\tValue in percent (0-100) above which to warn about chain overlaps within rates groups." <<
          std::endl;
          std::cout <<
            "--rateFallbackPrescaleL1\t\t\tIf prescales are not supplied for some or any items, what value to apply to L1 items."
                    << std::endl;
          std::cout <<
            "--rateFallbackPrescaleHLT\t\t\tIf prescales are not supplied for some or any items, what value to apply to HLT items."
                    << std::endl;
          std::cout <<
            "--useDefaultLumiScalingExceptions\t\tUse hard coded list of common special case chains for advanced lumi extrapolation (requires --predictionMu)."
                    << std::endl;
          std::cout <<
            "--scaleRatesByPS\t\t\t\tScale up chains by their L1 prescale to get their rate for L1 PS=1. Only for basic L1 and HLT chains, not combinations and global rates."
                    << std::endl;
          std::cout << "--maxMultiSeed " << _maxMultiSeed <<
            "\t\t\t\tMaximum number of L1 seeds a chain can have before it is dropped from Union rate groups due to exploding (2^nL1) computational complexity."
                    << std::endl;
          std::cout <<
            "--noOnlineDTCorrection\t\t\t\tFlag to prevent automated scaling to correct for L1 deadtime of EB data." <<
            std::endl;
          std::cout <<
            "--upgradeScenario\t\t\t\tSpecify the name of the scenario to load when doing upgrade rate estimations." <<
            std::endl;
          std::cout <<
            "--doUpgradeRatesScan\t\t\t\tAdd a standard set of spaced out L1 triggers at different energies to get pT dependence."
                    << std::endl;
          std::cout <<
            "--upgradeMergeTOBOverlap\t\t\t\tMerge nearby TOBs of the same type (exc. muon) when doing overlay based pileup simulation."
                    << std::endl;
          std::cout <<
            "--noUpgradePileupScaling\t\t\tWhen doing upgrade rates, use this flag to avoid scaling rates for increased <mu>."
                    << std::endl;
          std::cout <<
            "--noUpgradeBunchScaling\t\t\t\tWhen doing upgrade rates, use this flag to avoid scaling rates for increased bunches."
                    << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ I/O CONFIGURATION ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--files file1 file2 ...\t\t\t\tInput ntuple files." << std::endl;
          std::cout <<
            "--fileList\t\t\t\t\tFile with full path of data, one entry per line. Can be EOS if at CERN and ROOT6." <<
            std::endl;
          std::cout << "--ROSXML \"" << _ROSXML <<
            "\"\t\tROS ID mapping file from which to map ROBIN IDs to subdetectors (place in /data)." << std::endl;
          std::cout << "--treeName \"" << _treeName << "\"\t\t\t\tName of TTree in ntuples containing cost data." <<
            std::endl;
          std::cout << "--prefixL2 \"" << _L2Prefix << "\"\t\t\tPrefix of L2 cost branches." << std::endl;
          std::cout << "--prefixEF \"" << _EFPrefix << "\"\t\t\tPrefix of EF cost branches." << std::endl;
          std::cout << "--prefixHLT \"" << _HLTPrefix << "\"\t\t\tPrefix of HLT cost branches." << std::endl;
          std::cout << "--prefixConfig \"" << _configPrefix <<
            "\"\tName of TTree in ntuple containing trigger configuration metadata." << std::endl;
          std::cout << "--outputRootDirectory \"" << _outRootDirectory <<
            "\"\t\t\tFolder in which to place this programs output directory." << std::endl;
          std::cout << "--outputDirectory \"" << _outDirectory <<
            "\"\tFolder to contain all program output, %r is replaced with the run number, %t with the tag." <<
          std::endl;
          std::cout << "--outputTag \"" << _outTag <<
            "\"\t\t\t\tTag identifying this processing to be used in the output folder name (any underscores will be removed)."
                    << std::endl;
          std::cout <<
            "--outputTagFromAthena\t\t\t\tWhen running in Athena, this option will use the $AtlasVersion environment variable."
                    << std::endl;
          std::cout << "--outputRootFile \"" << _outRootFilename << "\"\tName of ROOT file for results." << std::endl;
          std::cout << "--outputImageDirectory \"" << _outImageDirectory <<
            "\"\t\t\tName of folder for image results." << std::endl;
          std::cout << "--outputCSVDirectory \"" << _outCSVDirectory << "\"\t\t\tName of folder for CSV results." <<
            std::endl;
          std::cout << "--outputXMLDirectory \"" << _outXMLDirectory << "\"\t\t\tName of folder for XML results." <<
            std::endl;
          std::cout << "--messageWait " << _messageWait << "\t\t\t\tTime in seconds between progress reports." <<
            std::endl;
          std::cout <<
            "--writeEBXML\t\t\t\t\tWrite out EnhancedBias weights from D3PD to XML for future use (for use by rate experts)."
                    << std::endl;
          std::cout << "--cleanAll\t\t\t\t\tRemove any old output directory before saving output." << std::endl;
          std::cout <<
            "--userDetails \t\t\t\t\tUser supplied metadata string giving any extra details about this run." <<
          std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ OTHER ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--help\t\t\t\t\t\tDisplays this very message and exits." << std::endl;
          std::cout << "--version\t\t\t\t\tDisplays the application version and exits." << std::endl;
          // Hidden --runNumber
          // Hidden --doNotWriteMetadata
          // Hidden --histogramBinMin
          // Hidden --histogramBinMax
          abort();
        }
        // I did not set a flag, but was also not caught above. Require implementation
        Error("Config::parseCLI", "The following option was not recognised: %s", _longOptions[_optionIndex].name);
        abort();
        break;

      case 'f':
        // I'm setting the input files
        // Do secondary loop to identify all input files
        _inputFiles.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _inputFiles.push_back(std::string(argv[optind++]));
        }
        break;

      case 'p':
        // I'm setting the chain pattern monitor
        // Do secondary loop to identify all patterns
        _patternsMonitor.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsMonitor.push_back(std::string(argv[optind++]));
        }
        break;

      case 'P':
        // I'm setting the chain pattern output
        _patternsOutput.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsOutput.push_back(std::string(argv[optind++]));
        }
        break;

      case 'I':
        // I'm setting the chain pattern output
        _patternsUnique.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsUnique.push_back(std::string(argv[optind++]));
        }
        break;

      case 'v':
        // I'm setting the rate exclusion
        _patternsExclude.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsExclude.push_back(std::string(argv[optind++]));
        }
        break;

      case 'Y':
        // I'm setting the overlap list
        _patternsOverlap.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsOverlap.push_back(std::string(argv[optind++]));
        }
        break;

      case 'u':
        // I'm setting the user details string
        _userDetails = std::string(optarg);
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _userDetails += std::string(" ") + std::string(argv[optind++]);
        }
        break;

      case '1':
        // I'm setting the _patternsNoLumiWeight strings
        _patternsNoLumiWeight.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsNoLumiWeight.push_back(std::string(argv[optind++]));
        }
        break;

      case '2':
        // I'm setting the _patternsNoMuLumiWeight strings
        _patternsNoMuLumiWeight.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsNoMuLumiWeight.push_back(std::string(argv[optind++]));
        }
        break;

      case '3':
        // I'm setting the _patternsNoBunchLumiWeight strings
        _patternsNoBunchLumiWeight.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsNoBunchLumiWeight.push_back(std::string(argv[optind++]));
        }
        break;

      case '4':
        // I'm setting the patternsExpoMuLumiWeight strings
        _patternsExpoMuLumiWeight.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsExpoMuLumiWeight.push_back(std::string(argv[optind++]));
        }
        break;

      case '8':
        // I'm setting the useOnlyTheseBCIDs
        _useOnlyTheseBCIDs.push_back(stringToInt(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _useOnlyTheseBCIDs.push_back(stringToInt(argv[optind++]));
        }
        break;

      case 'n':
        // N Events
        _ss << optarg;
        _ss >> _nEvents;
        break;

      case 'a':
        // Rate fallback prescale L1
        _ss << optarg;
        _ss >> _rateFallbackPrescaleL1;
        break;

      case 'A':
        // Rate fallback prescale HLT
        _ss << optarg;
        _ss >> _rateFallbackPrescaleHLT;
        break;

      case 'h':
        // N histogram bins
        _ss << optarg;
        _ss >> _histogramBins;
        break;

      case 'j':
        // Histogram bin min
        _ss << optarg;
        _ss >> _binMin;
        break;

      case 'J':
        // N histogram bin max
        _ss << optarg;
        _ss >> _binMax;
        break;

      case 's':
        // N Events to skip
        _ss << optarg;
        _ss >> _nSkip;
        break;

      case 'W':
        // N Threads
        _ss << optarg;
        _ss >> _nThread;
        break;

      case 'S':
        // Slow threshold
        _ss << optarg;
        _ss >> _slowThreshold;
        break;

      case 'y':
        // Slow event threshold
        _ss << optarg;
        _ss >> _slowEventThreshold;
        break;

      case 'F':
        // Max full event summaries
        _ss << optarg;
        _ss >> _maxNumberFullEvents;
        break;

      case 'b':
        // Lumi block begining
        _ss << optarg;
        _ss >> _lbBegin;
        break;

      case 'e':
        // Lumi block end
        _ss << optarg;
        _ss >> _lbEnd;
        break;

      case 'z':
        // Message wait time
        _ss << optarg;
        _ss >> _messageWait;
        break;

      case 'B':
        // Basic event weight
        _ss << optarg;
        _ss >> _basicWeight;
        break;

      case 'L':
        // Number of lumi blocks to do a full summary for
        _ss << optarg;
        _ss >> _nLbFullSummary;
        break;

      case 'k':
        // Number of lumi blocks to skip per full summary
        _ss << optarg;
        _ss >> _nLbFullSkip;
        break;

      case 'd':
        // Default lumiblock length
        _ss << optarg;
        _ss >> _defaultLBLength;
        break;

      case 'D':
        // inst L for prediction
        _ss << optarg;
        _ss >> _predictionLumi;
        break;

      case 'U':
        // inst L of EB run
        _ss << optarg;
        _ss >> _runLumi;
        break;

      case 'N':
        // User supplied run number
        _ss << optarg;
        _ss >> _runNumber;
        break;

      case 'X':
        // Default lumiblock length
        _ss << optarg;
        _ss >> _maxMultiSeed;
        break;

      case 'q':
        // nHLTConfigSummary
        _ss << optarg;
        _ss >> _nHLTConfigSummary;
        break;

      case 'Q':
        // nLbPerHLTConfig
        _ss << optarg;
        _ss >> _nLbPerHLTConfig;
        break;

      case '5':
        // expoRateScaleModifier
        _ss << optarg;
        _ss >> _expoRateScaleModifierL1;

      case '6':
        // expoRateScaleModifier
        _ss << optarg;
        _ss >> _expoRateScaleModifierHLT;
        break;

      case 'K':
        // File list
        _fileList = std::string(optarg);
        break;

      case 'V':
        // Full Event File List
        _fullEventFileList = std::string(optarg);
        break;

      case 't':
        // Different tree name
        _treeName = std::string(optarg);
        break;

      case '7':
        // Multirun
        _multiRun = std::string(optarg);
        break;

      case 'm':
        // Specify upgrade scenario
        _upgradeScenario = std::string(optarg);
        break;

      case 'T':
        // Different tag name
        _outTag = std::string(optarg);
        break;

      case 'M':
        // Different prescale menu XML
        _prescaleXML1 = std::string(optarg);
        break;

      case 'g': // NOTE same as M
        // Different prescale menu XML
        _prescaleXML1 = std::string(optarg);
        break;

      case 'G':
        // Different prescale menu XML
        _prescaleXML2 = std::string(optarg);
        break;

      case 'R':
        // Different ROS mapping
        _ROSXML = std::string(optarg);
        break;

      case 'r':
        // Different output root
        _outRootFilename = std::string(optarg);
        break;

      case 'i':
        // Different output image
        _outImageDirectory = std::string(optarg);
        break;

      case 'C':
        // Different output csv dir
        _outCSVDirectory = std::string(optarg);
        break;

      case 'x':
        // Different output xml dir
        _outXMLDirectory = std::string(optarg);
        break;

      case 'o':
        // Different output directory
        _outDirectory = std::string(optarg);
        break;

      case 'O':
        // Different output ROOT directory
        _outRootDirectory = std::string(optarg);
        break;

      case 'l':
        // Different L2 prefix
        _L2Prefix = std::string(optarg);
        break;

      case 'E':
        // Different EF prefix
        _EFPrefix = std::string(optarg);
        break;

      case 'w':
        // different overlap percent
        _ss << optarg;
        _ss >> _ratesOverlapWarning;
        break;

      case 'Z':
        // JIRA
        _ss << optarg;
        _ss >> _jira;
        break;

      case 'H':
        // Different HLT prefix
        _HLTPrefix = std::string(optarg);
        break;

      case 'c':
        // Different HLT prefix
        _configPrefix = std::string(optarg);
        break;

      case '0':
        // Target mu
        _ss << optarg;
        _ss >> _targetMu;
        break;

      default:
        Error("Config::parseCLI", "Supplied argument '%c' not recognised.", Char_t(_status));
        abort();
      }
    }

    // Settings read. Now to check them and store them for use

    Info("Config::parseCLI", "Version %s", _version.c_str());
    if (_showVersion == kTRUE) {
      abort();
    }

    //////////////////////////
    //        ALIASES - how handy
    //////////////////////////

    if (_ratesMode == kTRUE || _ratesUpgradeMode == kTRUE) {
      Info("Config::parseCLI", "Setting up default options for a rates prediction job.");
      _cleanAll = 1;
      _doHLT = 1;
      _summaryAll = 1;
      if (_ratesMode) _monitorRates = 1;
      if (_ratesUpgradeMode) _monitorRatesUpgrade = 1;
      _doEBWeighting = 1;
      _matchL1RandomToOnline = 1;
      _outputModeStandard = 1;
      _doCPS = 1;
      _useDefaultLumiScalingExceptions = 1;
      // _doExponentialMu = 1; // No longer have this flag by default
      if (_doExponentialMu) _useDefaultExponentialScalingList = 1;
    }

    if (_costMode == kTRUE) {
      Info("Config::parseCLI", "Setting up default options for a cost job.");
      _cleanAll = 1;
      _doHLT = 1;
      _summaryAll = 1;
      _monitorChains = 1;
      _monitorChainAlgs = 1;
      _monitorSequences = 1;
      _monitorAlgs = 1;
      _monitorAlgClass = 1;
      _monitorROS = 1;
      _monitorROBIN = 1;
      _monitorROI = 1;
      _monitorGlobals = 1;
      _monitorFullEvent = 1;
      _outputModeStandard = 1;
      _ignoreNonPhysBunchGroups = 1;
      _monitorSliceCPU = 1;
    }

    if (_onlineMode == kTRUE) {
      Info("Config::parseCLI", "Setting up default options for a online job.");
      _cleanAll = 1;
      _doHLT = 1;
      _summaryPerHLTConfig = 1;
      _summaryPerLumiBlock = 1;
      _monitorAllChainSeqAlgs = 1;
      _monitorChains = 1;
      _monitorChainAlgs = 1;
      _monitorSequences = 1;
      _monitorAlgs = 1;
      _monitorAlgClass = 1;
      _monitorROS = 1;
      _monitorROBIN = 1;
      _monitorROI = 1;
      _monitorGlobals = 1;
      _monitorFullEvent = 1;
      _outputModeStandard = 1;
      _monitorSliceCPU = 1;
    }

    if (_outputModeStandard == kTRUE) {
      Info("Config::parseCLI", "Setting up standard outputs.");
      _doOutputHist = 1;
      _doOutputCsv = 1;
      _doOutputRatesGraph = 1;
      _doOutputMenus = 1;
      _doOutputXML = 1;
    }

    //////////////////////////
    //        ROOT FILES, CHAIN AND TREE NAMES
    //////////////////////////

    if (_inputFiles.size() == 0 && _fileList == m_blankString) {
      Error("Config::parseCLI", "Please supply at least one input file with --files or --fileList");
      return kFALSE;
    }
    // If we have a file list - fill the input files
    if (_fileList != m_blankString) {
      std::ifstream _ifs(_fileList.c_str());
      std::string _line;
      while (std::getline(_ifs, _line)) {
        _inputFiles.push_back(_line);
      }
    }
    // check names
    for (UInt_t _n = 0; _n < _inputFiles.size(); ++_n) {
      if (_inputFiles.at(_n).substr(0, 5) == "/eos/") {
        _inputFiles.at(_n) = std::string("root:://eosatlas/" + _inputFiles.at(_n));
      } else if (_inputFiles.at(_n).substr(0, 27) == "srm://srm-eosatlas.cern.ch/") {
        _inputFiles.at(_n) = std::string("root:://eosatlas//") + _inputFiles.at(_n).substr(27, std::string::npos);
      }
    }

    // If we have a full event list - fill the input files
    if (_fullEventFileList != m_blankString) {
      std::ifstream _ifs(_fullEventFileList.c_str());
      std::string _line;
      while (std::getline(_ifs, _line)) {
        _eventPicker.push_back(stringToInt(_line));
      }
      set(kEventPickList, _eventPicker, "EventPickList");
    }

    set(kInputFiles, _inputFiles, "InputFiles");
    setLong(kNEvents, _nEvents, "NEvents");
    setLong(kNSkip, _nSkip, "NSkip");
    set(kTreeName, _treeName, "TreeName");
    set(kL2Prefix, _L2Prefix, "L2Prefix");
    set(kEFPrefix, _EFPrefix, "EFPrefix");
    set(kHLTPrefix, _HLTPrefix, "HLTPrefix");
    set(kConfigPrefix, _configPrefix, "ConfigPrefix");

    //////////////////////////
    //        HLT LEVELS AND RUN RANGES TO PROCESS
    //////////////////////////

    // Trigger Level settings
    if (_doL2 + _doEF + _doL2EF + _doHLT != 1) {
      //  Allow use of doL2 and doEF instead of --doL2EF
      if (!(_doL2 == 1 && _doEF == 1 && _doHLT == 0)) {
        Warning("Config::parseCLI",
                "Not specified a HLT level to run (--doL2, --doEF, --doL2EF or --doHLT). Will default to --doHLT.");
        _doHLT = 1;
      }
    }
    set(kDoL2, (Int_t) (_doL2 || _doL2EF), "DoL2");
    set(kDoEF, (Int_t) (_doEF || _doL2EF), "DoEF");
    set(kDoHLT, _doHLT, "DoHLT");
    set(kDoHLTOnline, 0, "DoHLTOnline"); // If unified HLT was used online - not yet ever true

    // Summary range settings
    if (_summaryAll + _summaryPerHLTConfig + _summaryPerLumiBlock == 0) {
      Warning("Config::parseCLI", "Not specified a time range to summaries. Will default to --summaryPerHLTConfig.");
      _summaryPerHLTConfig = 1;
    }
    set(kDoAllSummary, _summaryAll, "DoSummaryAll");
    set(kDoKeySummary, _summaryPerHLTConfig, "DoKeySummary");
    set(kDoLumiBlockSummary, _summaryPerLumiBlock, "DoLBSummary");

    // Number of events to save full information for
    set(kFullEventMaxNumToSave, _maxNumberFullEvents, "MaxNumberFullEvents");
    set(kCurrentEventIsSlow, 0, "SlowFlag", kUnlocked);
    if (_nEvents != INT_MAX) {
      set(kFullEventSaveOnePer, (Int_t) (1 + (_nEvents / getInt(kFullEventMaxNumToSave))), "FullEventSaveOncePer");
    } else {
      set(kFullEventSaveOnePer, 1000000, "FullEventSaveOncePer", kUnlocked);//This is just a generic fallback. We can
                                                                            // update it later in the event loop if need
                                                                            // be
    }

    //////////////////////////
    //        MONITORS TO INCLUDE
    //////////////////////////

    // This flag just enables others
    if (_monitorAllChainSeqAlgs == 1) {
      _monitorChains = 1;
      _monitorChainAlgs = 1;
      _monitorSequences = 1;
      _monitorSequenceAlgs = 1;
      _monitorAlgs = 1;
      _monitorAlgClass = 1;
    }

    if (_monitorAllROS == 1) {
      _monitorROS = 1;
      _monitorROBIN = 1;
      _monitorROSAlgs = 1;
      _monitorROSChains = 1;
    }

    int _nMon = _monitorAll
                + _monitorChains
                + _monitorChainAlgs
                + _monitorSequences
                + _monitorSequenceAlgs
                + _monitorAlgs
                + _monitorAlgClass
                + _monitorROS
                + _monitorROBIN
                + _monitorROSAlgs
                + _monitorROSChains
                + _monitorROI
                + _monitorL1
                + _monitorFullEvent
                + _monitorGlobals
                + _monitorEventProfile
                + _monitorRates
                + _monitorRatesUpgrade
                + _monitorSliceCPU
                + _writeEBWeightXML;

    if (_nMon == 0) {
      Warning("Config::parseCLI", "Not specified a monitor to use. Will default to --monitorAll.");
      _monitorAll = 1;
    }

    // Check for rates only processing, and relax the LB requirement
    Int_t _ratesOnly = 0;
    if ((_monitorRates == 1 || _monitorRatesUpgrade == 1) && _nMon == (_monitorRates + _monitorRatesUpgrade)) {
      _ratesOnly = 1;
      if (_nLbPerHLTConfig != INT_MAX && _summaryAll == 0 && _summaryPerHLTConfig == 1) {
        Warning("Config::parseCLI",
                "We're in 'rates only' mode, --nLbPerHLTConfig is ignored and all LB are processed per HLT config.");
      }
    }
    set(kRatesOnly, _ratesOnly, "RatesOnly");

    // We save monitorAll separately as it is picked up in a time saving loop later
    set(kDoAllMonitor, _monitorAll, "MonitorAll"); // This one is magic, and will be picked up in a loop later - force
                                                   // enabling all others
    set(kDoChainMonitor, _monitorChains, "ChainMonitor");
    set(kDoChainAlgorithmMonitor, _monitorChainAlgs, "ChainAlgorithmMonitor");
    set(kDoSequenceMonitor, _monitorSequences, "SequenceMonitor");
    set(kDoSequenceAlgorithmMonitor, _monitorSequenceAlgs, "SequenceAlgorithmMonitor");
    set(kDoAlgorithmMonitor, _monitorAlgs, "AlgorithmMonitor");
    set(kDoAlgorithmClassMonitor, _monitorAlgClass, "AlgorithmClassMonitor");
    set(kDoSliceCPUMonitor, _monitorSliceCPU, "SliceCPUMonitor");
    set(kDoROSMonitor, _monitorROS, "ROSMonitor");
    set(kDoROBINMonitor, _monitorROBIN, "ROBINMonitor");
    set(kDoROSAlgorithmMonitor, _monitorROSAlgs, "ROSAlgorithmMonitor");
    set(kDoROSChainMonitor, _monitorROSChains, "ROSChainMonitor");
    set(kDoROIMonitor, _monitorROI, "ROIMonitor");
    set(kDoFullEventMonitor, _monitorFullEvent, "FullEventMonitor");
    set(kDoGlobalsMonitor, _monitorGlobals, "GlobalsMonitor");
    set(kDoEventProfileMonitor, _monitorEventProfile, "EventProfileMonitor");
    set(kDoRatesMonitor, _monitorRates, "RatesMonitor");
    set(kDoRatesUpgradeMonitor, _monitorRatesUpgrade, "RatesUpgradeMonitor");

    // Number of passes, UpgradeMonitor needs 2 if we are doing upgrade pileup scaling
    if (_monitorRatesUpgrade == kTRUE && _noUpgradePileupScaling == kFALSE) {
      set(kNPasses, 2, "NPasses");
    } else {
      set(kNPasses, 1, "NPasses");
    }

    if (isZero(_targetMu) &&
        (_patternsNoLumiWeight.size() || _patternsNoMuLumiWeight.size() || _patternsNoBunchLumiWeight.size() ||
         _patternsExpoMuLumiWeight.size())) {
      Error("Config::parseCLI",
            "If using --patternsNoLumiWeight, --patternsNoMuLumiWeight or --patternsNoBunchLumiWeight then --predictionMu must be specified too.");
      return kFALSE;
    }

    // Hard code some special cases
    if (_useDefaultLumiScalingExceptions) {
      _patternsNoLumiWeight.push_back("HLT_lumipeb_L1RD");
      _patternsNoLumiWeight.push_back("HLT_lumipeb_vdm_L1RD");
      _patternsNoLumiWeight.push_back("HLT_j0_perf_L1RD");
      _patternsNoLumiWeight.push_back("HLT_ibllumi_L1RD");
      _patternsNoLumiWeight.push_back("HLT_l1calocalib");
      _patternsNoLumiWeight.push_back("HLT_sct_noise");
      _patternsNoLumiWeight.push_back("HLT_noalg_L1RD");
    }

    if (_useDefaultExponentialScalingList) {
      _patternsExpoMuLumiWeight.push_back("HLT_noalg_L1XE");
      _patternsExpoMuLumiWeight.push_back("HLT_xe");
      _patternsExpoMuLumiWeight.push_back("L1_XE");
    }

    // String patterns to match when doing monitoring
    set(kPatternsMonitor, _patternsMonitor, "PatternsMonitor", kUnlocked);
    set(kPatternsOutput, _patternsOutput, "PatternsOutput", kUnlocked);
    set(kPatternsUnique, _patternsUnique, "PatternsUnique");
    set(kPatternsExclude, _patternsExclude, "PatternsExclude");
    set(kPatternsOverlap, _patternsOverlap, "PatternsOverlap");
    set(kPatternsNoLumiWeight, _patternsNoLumiWeight, "PatternsNoLumiWeight");
    set(kPatternsNoMuLumiWeight, _patternsNoMuLumiWeight, "PatternsNoMuLumiWeight");
    set(kPatternsNoBunchLumiWeight, _patternsNoBunchLumiWeight, "PatternsNoBunchLumiWeight");
    set(kPatternsExpoMuLumiWeight, _patternsExpoMuLumiWeight, "PatternsExpoMuLumiWeight");


    // If there are patterns here, make sure we switch on unique rates
    if (_patternsUnique.size() > 0 && _doUniqueRates == kFALSE) {
      _doUniqueRates = kTRUE;
      Info("Config::parseCLI", "Setting --doUniqueRates to TRUE because one or more --patternsUnique were supplied");
    }
    if (_doUniqueRates && _doExponentialMu) {
      Warning("Config::parseCLI", "Forcing doExponentialMu flag to FALSE otherwise can get negative unique rates.");
      _doExponentialMu = kFALSE;
    }

    // Lumi block settings. Start, end, and number to fully summarise
    if (_lbEnd < _lbBegin) {
      Error("Config::parseCLI", "End lumi block number cannot be before starting lumi block number.");
      return kFALSE;
    }
    set(kLumiStart, _lbBegin, "LumiBlockStart");
    set(kLumiEnd, _lbEnd, "LumiBlockEnd");
    set(kNLbFullSummary, _nLbFullSummary, "NLBFullSummary");
    set(kNLbFullSkip, _nLbFullSkip, "NLBFullSkip");
    set(kNLbPerHLTConfig, _nLbPerHLTConfig, "NLBPerHLTConfig");
    set(kNHLTConfigSummary, _nHLTConfigSummary, "NHLTConfigSummary");

    // Are we doing time consuming ROS matching?
    Bool_t _matching = kFALSE;
    if (_monitorROS || _monitorROBIN || _monitorROSChains || _monitorROSAlgs || _monitorEventProfile) _matching = kTRUE;

    set(kEnableROSToAlgMatching, (Int_t) _matching, "RosToAlgMatching");

    //////////////////////////
    //        OUTPUT TO GENERATE
    //////////////////////////

    if (_doOutputHist + _doOutputCanvas + _doOutputPng + _doOutputPdf + _doOutputCsv + _doOutputXML == 0) {
      Warning("Config::parseCLI",
              "No output format specified. Will default to --doOutputHist --doOutputCSV --doOutputMenus --doOutputRatesGraph --doOutputRatesXML");
      _doOutputHist = 1;
      _doOutputCsv = 1;
      _doOutputRatesGraph = 1;
      _doOutputMenus = 1;
      _doOutputXML = 1;
    }

    set(kOutputRootDirectory, _outRootDirectory, "OutputRootFileSystemDirectory");
    // Combine the root output directory with the programs directory name here
    _outDirectory = _outRootDirectory + "/" + _outDirectory;

    // See if we have been requested to get the AtlasVersion from Athena and use it in the tag
    if (_outputTagFromAthena == kTRUE) {
      const Char_t* _env = std::getenv("AtlasBuildStamp");
      if (_env != NULL) {
        _outTag = std::string(_env);
      }
    }

    // Remove any "_" from the tag
    while (_outTag.find("_") != std::string::npos) {
      _outTag.replace(_outTag.find("_"), 1, m_blankString);
    }

    set(kOutputPng, _doOutputPng, "DoPNG");
    set(kOutputPdf, _doOutputPdf, "DoPDF");
    set(kOutputImage, (Int_t) (_doOutputPdf || _doOutputPng), "DoOutputImage");
    set(kOutputRoot, (Int_t) (_doOutputHist || _doOutputCanvas), "DoOutputROOT");
    set(kOutputHist, _doOutputHist, "DoOutputHist");
    set(kOutputCanvas, _doOutputCanvas, "DoOutputCanvas");
    set(kOutputCsv, _doOutputCsv, "DoOutputCSV");
    set(kOutputXML, _doOutputXML, "DoOutputXML");
    set(kOutputRatesGraph, _doOutputRatesGraph, "DoOutputRatesGraph");
    set(kOutputMenus, _doOutputMenus, "DoOutputMenus");
    set(kOutputDirectory, _outDirectory, "OutputDirectory", kUnlocked); // Do NOT lock this as we need to modify it to
                                                                        // add the run number in
    set(kOutputTag, _outTag, "OutputTag");
    set(kOutputImageDirectory, _outImageDirectory, "OutputImageDirectory");
    set(kOutputCSVDirectory, _outCSVDirectory, "OutputCSVDirectory");
    set(kOutputXMLDirectory, _outXMLDirectory, "OutputXMLDirectory");
    set(kOutputRatesGraphFilename, _outRatesGraphFilename, "OutputRatesGraphFilename");
    set(kOutputRatesWarnings, _outRatesWarningsFilename, "OutputRatesWarningsFilename");
    set(kOutputRootFilename, _outRootFilename, "OutputROOTFileName");
    set(kLinkOutputDir, _linkOutputDirectory, "LinkOutputDirectory");
    set(kWriteDummyPSXML, _writeDummyPSXML, "WriteDummyPSXML");
    if (_userDetails != "") {
      set(kUserDetails, _userDetails, "Details", kUnlocked); // Modified later to include metadata
    }

    //////////////////////////
    //        MISC SETTINGS
    //////////////////////////

    // Cannot both use EB weights and export them
    if (_doEBWeighting == kTRUE && _writeEBWeightXML == kTRUE) {
      Error("Config::parseCLI", "Cannot use both --useEBWeight and --writeEBXML");
      abort();
    }

    if (_doEBWeighting == kTRUE && _ratesScaleByPS == kTRUE) {
      Error("Config::parseCLI",
            "--useEBWeight with --scaleRatesByPS is not valid. When using EB wieghts, all PS are supplied by the user. You can then set everything to PS=1.");
      Error("Config::parseCLI",
            "(this is the default, if no PS XML is supplied). The --scaleRatesByPS option should only be used to undo online prescales for chains.");
      abort();
    }

    setFloat(kBasicEventWeight, _basicWeight, "BasicEventWeight");
    if (!isZero(_predictionLumi)) {
      // Allow either notation here. Either explicit or in multiples of 10^30
      if (_predictionLumi < 1e20) _predictionLumi *= 1e30;
      setFloat(kPredictionLumi, _predictionLumi, "PredictionLumi"); // The lumi we're currently predicting for (may be
                                                                    // set in XML)
    }
    if (!isZero(_runLumi)) {
      if (_runLumi < 1e20) _runLumi *= 1e30;
      setFloat(kRunLumi, _runLumi, "RunLumi"); // The lumi of the EB run online
    }
#ifdef MTHREAD
    set(kNThread, _nThread, "NumberThreads");
#else
    set(kNThread, 1, "NumberThreads");
#endif
    set(kSlowThreshold, _slowThreshold, "SlowThreshold");
    set(kSlowEvThreshold, _slowEventThreshold, "SlowEventThreshold");
    set(kDoEBWeighting, _doEBWeighting, "DoEBWeighting");
    set(kWriteEBWeightXML, _writeEBWeightXML, "WriteEBWeightXML");
    set(kDefaultLBLength, _defaultLBLength, "DefaultLBLength");
    set(kDebug, _debug, "Debug");
    set(kCleanAll, _cleanAll, "CleanAll"); //Specifies if the output directory is to be regenerated
    set(kRatesForcePass, _ratesForcePass, "RatesForceAllChainsToPassRaw");
    set(kDoUniqueRates, _doUniqueRates, "DoUniqueRates");
    set(kDoCPS, _doCPS, "DoCPS");
    set(kDoGroupOverlap, _doGroupOverlap, "DoGroupOverlap");
    set(kDoAllOverlap, _doAllOverlap, "DoAllOverlap");
    set(kHistBins, _histogramBins, "HistogramBins");
    set(kRatesOverlapWarning, _ratesOverlapWarning, "RatesOverlapWarning");
    set(kRatesScaleByPS, _ratesScaleByPS, "RatesScaleByPS");
    set(kExtrapolate8To13, _extraplolate8To13, "Extrapolate8To13TeV");
    set(kExtrapolate13To5, _extraplolate13To5, "Extrapolate13To5TeV");
    set(kDoNotWriteMetadata, _doNotWriteMetadata, "DoNotWriteMetadata");
    set(kNoMsgSuppression, _noMsgSup, "NoMessageSuppression");
    // If set to true, RD0/1/2/3 triggers will only be allowed to fire if the event was taken online with a random
    // trigger
    // This prevents the biased triggers inflating the rate of chains which are supposed to run off of the unbiased
    // triggers
    set(kMatchL1RandomToOnline, _matchL1RandomToOnline, "MatchL1RandomToOnline");
    set(kNoOnlineDeadtimeCorrection, _noOnlineDTCorrection, "NoOnlineDeadtimeCorrection");
    set(kUpgradeScenario, _upgradeScenario, "UpgradeScenario");
    set(kNoUpgradePileupScaling, _noUpgradePileupScaling, "NoUpgradePileupScaling");
    set(kNoUpgradeBunchScaling, _noUpgradeBunchScaling, "NoUpgradeBunchScaling");
    set(kDoUpgradeRatesScan, _doUpgradeRatesScan, "DoUpgradeRatesScan");
    set(kMessageWait, _messageWait, "MessageWait");
    setFloat(kEventElapsed, 0., "EventElasped", kUnlocked);
    setFloat(kEventStartTime, 0., "EventStart", kUnlocked);
    set(kPatternsInvert, _patternsInvert, "PatternsInvert");
    set(kIsCPUPrediction, _isCPUPrediction, "IsCPUPrediction");
    set(kIgnoreRerun, 0, "IgnoreRerun", kUnlocked);
    set(kJIRA, _jira, "JIRA");
    set(kPatternsExactMatch, _patternsExactMatch, "PatternsExactMatch");
    set(kIgnoreNonPhysBunchGroups, _ignoreNonPhysBunchGroups, "IgnoreNonPhyBunchgroups");
    set(kNoLBRescaling, _noLBRescaling, "NoLBRescaling");
    setFloat(kTargetPeakMuAverage, _targetMu, "TargetMu", kUnlocked);
    setFloat(kExpoRateScaleModifierL1, _expoRateScaleModifierL1, "ExpoRateScaleModifierL1");
    setFloat(kExpoRateScaleModifierHLT, _expoRateScaleModifierHLT, "ExpoRateScaleModifierHLT");
    set(kUpgradeMergeTOBOverlap, _upgradeMergeTOBOverlap, "UpgradeMergeTOBOverlap");
    set(kDoExponentialMu, _doExponentialMu, "DoExponentialMu");
    set(kInvertHighMuRunVeto, _invertHighMuRunVeto, "InvertHighMuRunVeto");
    set(kUseOnlyTheseBCIDs, _useOnlyTheseBCIDs, "UseOnlyTheseBCIDs");
    set(kIgnoreGRL, _ignoreGRL, "IgnoreGRL");
    set(kIgnorePSGreaterThanOne, _ignorePSGreaterThanOne, "IgnorePSGreaterThanOne");

    std::stringstream _multiRunss(_multiRun); // Comma separated
    std::string _tempStr;
    std::vector<Int_t> _multRunInts;
    while (std::getline(_multiRunss, _tempStr, ',')) _multRunInts.push_back(stringToInt(_tempStr));
    set(kMultiRun, _multRunInts, "MultiRun");


    set(kMaxMultiSeed, _maxMultiSeed, "MaxMultiSeed");
    if (_runNumber != 0) set(kRunNumber, _runNumber, "RunNucmber");

    // Load files to be accessed
    // File Names
    if (_ROSXML != m_blankString) set(kROSXMLName, _ROSXML, "ROSXML");
    if (_prescaleXML1 != m_blankString) set(kPrescaleXMLName1, _prescaleXML1, "PrescaleXML1");
    if (_prescaleXML2 != m_blankString) set(kPrescaleXMLName2, _prescaleXML2, "PrescaleXML2");

    // Choose sensible default PS behaviour. If XML supplied, default to -1, else to +1.
    if (_prescaleXML1 != m_blankString || _prescaleXML2 != m_blankString) {
      if (_rateFallbackPrescaleL1 == FLT_MIN) _rateFallbackPrescaleL1 = -1.;
      if (_rateFallbackPrescaleHLT == FLT_MIN) _rateFallbackPrescaleHLT = -1.;
    } else {
      if (_rateFallbackPrescaleL1 == FLT_MIN) _rateFallbackPrescaleL1 = 1.;
      if (_rateFallbackPrescaleHLT == FLT_MIN) _rateFallbackPrescaleHLT = 1.;
    }
    if (isZero(_rateFallbackPrescaleL1 - 1.) == kFALSE) {
      Warning("Config::parseCLI", "PLEASE NOTE: Setting fall-back prescale value for unknown L1 items to %f.",
              _rateFallbackPrescaleL1);
    }
    if (isZero(_rateFallbackPrescaleHLT - 1.) == kFALSE) {
      Warning("Config::parseCLI", "PLEASE NOTE: Setting fall-back prescale value for unknown HLT items to %f.",
              _rateFallbackPrescaleHLT);
    }
    setFloat(kRateFallbackPrescaleL1, _rateFallbackPrescaleL1, "RateFallbackPrescaleL1");
    setFloat(kRateFallbackPrescaleHLT, _rateFallbackPrescaleHLT, "RateFallbackPrescaleHLT");

    //////////////////////////
    //        END OF USER SETTINGS
    //////////////////////////

    // Print any remaining command line arguments (not parsed options)
    if (optind < argc) {
      Warning("Config::parseCLI", "The following options were supplied but not parsed:");
      while (optind < argc) {
        Warning("Config::parseCLI", "  -> %s", argv[optind++]);
      }
    }

    dump();

    //////////////////////////
    //        BEGIN INTERNAL SETTINGS
    //////////////////////////

    // Set external data location
    set(kAFSDataDir, "/afs/cern.ch/user/a/attradm/public", "AFSDataDir");

    //Check if we're in ROOT CORE
    const Char_t* _env = std::getenv("ROOTCOREBIN");
    if (_env != NULL) {
      set(kIsRootCore, 1, "IsRootCore");
      set(kDataDir, std::string(_env) + std::string("/data/TrigCostRootAnalysis/"), "DataDir");
      if (getIsSet(kROSXMLName)) set(kROSXMLPath, getStr(kDataDir) + getStr(kROSXMLName));
      if (getIsSet(kPrescaleXMLName1)) {
        if (getStr(kPrescaleXMLName1).substr(0, 1) == "/") set(kPrescaleXMLPath1, getStr(kPrescaleXMLName1));
        else set(kPrescaleXMLPath1, getStr(kDataDir) + getStr(kPrescaleXMLName1));
      }
      if (getIsSet(kPrescaleXMLName2)) {
        if (getStr(kPrescaleXMLName2).substr(0, 1) == "/") set(kPrescaleXMLPath2, getStr(kPrescaleXMLName2));
        else set(kPrescaleXMLPath2, getStr(kDataDir) + getStr(kPrescaleXMLName2));
      }
      set(kAutoMonXMLPath, getStr(kDataDir) + _AutoMonXMLName, "AutoMonXMLPath");
    } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
      set(kIsRootCore, 0, "IsRootCore");
      if (getIsSet(kROSXMLName)) {
        std::string _locAthena = PathResolverFindDataFile(getStr(kROSXMLName));
        if (_locAthena ==
            m_blankString) Error("Config::parseCLI", "Athena cannot find ROS mapping file %s", getStr(
            kROSXMLName).c_str());

        else {
          set(kROSXMLPath, _locAthena, "ROSXMLPath");
          Info("Config::parseCLI", "Athena has found the file: %s", getStr(kROSXMLPath).c_str());
        }
      }
      //
      if (getIsSet(kPrescaleXMLName1)) {
        std::string _locAthena = PathResolverFindDataFile(getStr(kPrescaleXMLName1));
        if (_locAthena ==
        m_blankString) Error("Config::parseCLI", "Athena cannot find prescale XML file #1 %s", getStr(
            kPrescaleXMLName1).c_str());
        else {
          set(kPrescaleXMLPath1, _locAthena, "PrescaleXMLPath1");
          Info("Config::parseCLI", "Athena has found the file: %s", getStr(kPrescaleXMLPath1).c_str());
        }
      }
      if (getIsSet(kPrescaleXMLName2)) {
        std::string _locAthena = PathResolverFindDataFile(getStr(kPrescaleXMLName2));
        if (_locAthena == m_blankString) Error("Config::parseCLI", "Athena cannot find prescale XML file #2 %s", getStr(kPrescaleXMLName2).c_str());
        else {
          set(kPrescaleXMLPath2, _locAthena, "PrescaleXMLPath2");
          Info("Config::parseCLI", "Athena has found the file: %s", getStr(kPrescaleXMLPath2).c_str());
        }
      }
      std::string _locAthena = PathResolverFindDataFile(_AutoMonXMLName);
      if (_locAthena == m_blankString) Error("Config::parseCLI", "Athena cannot find AutoMonConfig XML file #1 %s", _AutoMonXMLName.c_str());
      else {
        set(kAutoMonXMLPath, _locAthena, "AutoMonXMLPath");
      }
#endif // not ROOTCORE
    }

    // Internally used parameters - do not need to be included in dump
    set(kSloppyExit, 1, "SloppyExit");
    set(kOutputSummaryDirectory, "summary");
    set(kErrorIgnore, (Int_t) gErrorIgnoreLevel, "ErrorIgnoreLevel"); //Cache this internal ROOT variable
    set(kDirectlyApplyPrescales, 1, "DirectlyApplyPrescales");
    set(kNBunchGroups, 16, "NumberOfBunchGroups");
    set(kMaxBunches, 2808, "MaxmimumNumberOfBunches");
    set(kMaxBCIDs, 3474, "MaxmimumNumberOfBCIDs"); // Excluding the abort gap and reset
    set(kLinkOutputDirName, "costMon", "OutputDirLinkName");
    setFloat(kUnbiasedWeightThreshold, 2.5e5, "UnbiasedWeightThreshold"); // RUn1 ONLY: Events with w>this will be
                                                                          // considered to be RD0 streamers (unbiased)
    if (_binMin != FLT_MIN && _binMax != FLT_MIN) {
      setFloat(kHistBinMin, _binMin, "HistogramBinMin");
      setFloat(kHistBinMax, _binMax, "HistogramBinMax");
    }
    set(kUpgradeEMCountsPerGeV, 2, "UpgradeEMCountsPerGeV");
    set(kUpgradeJetCountsPerGeV, 1, "UpgradeJetCountsPerGeV");
    set(kUpgradeJetLargeWindow, 1, "UpgradeJetLargeWindow");
    setFloat(kLumiExtrapWeight, 1., "FinalLumiExtrapWeight", kUnlocked); // This should be overwritten with the real
                                                                         // weight later
    set(kDoAdvancedLumiScaling, 0, "DoAdvancedLumiScaling", kUnlocked); // This should be overwritten if decided it is
                                                                        // enabled
    set(kFarmXML, "HLTFarm.xml", "FarmXML");

    // Maximum number of certain messages to show
    setDisplayMsg(kMsgDivZero, 5, "Division by zero");
    setDisplayMsg(kMsgBadROB, 10, "Bad ROB Data");
    setDisplayMsg(kMsgRoISize, 2, "RoI Container Size Missmatch");
    setDisplayMsg(kMsgRoIHack, 5, "RoI Hack");
    setDisplayMsg(kMsgCannotFindRoI, 2, "Cannot Find RoI");
    setDisplayMsg(kMsgUnknownRoIType, 2, "Unknown RoI Type");
    setDisplayMsg(kMsgHourBoundary, 10, "Hour boundary in ROS request association");
    setDisplayMsg(kMsgSaveFullEvent, 10, "Saving full event");
    setDisplayMsg(kMsgXMLWeight, 50, "Cannot find XML event weight");
    setDisplayMsg(kMsgXMLPrescale, 50, "Cannot find XML prescale");
    setDisplayMsg(kMsgUnknownDecoration, 25, "Unknown decoration");
    setDisplayMsg(kMsgIllegalCharacters, 10, "Illegal character in output");
    setDisplayMsg(kMsgZeroRate, 1, "Chain with positive prescale but zero rate");
    setDisplayMsg(kMsgLargeSteerTime, 3, "Hour boundary in steering time calculation");
    setDisplayMsg(kMsgNoLumiInfo, 3, "No luminosity block length info in file, using default value");
    setDisplayMsg(kMsgCannotFindVar, 5, "Cannot find variable in data store");
    setDisplayMsg(kMsgCannotFindVO, 5, "Cannot find variable option (VO) for variable in data store");
    setDisplayMsg(kMsgNewUniqueCounter, 30, "Created new unique counter");
    setDisplayMsg(kMsgLumiScaling, 1, "No XML to do partial run scaling (only important if processing an EB run)");
    setDisplayMsg(kMsgNoTETOB, 5, "Event did not contain any Total Energy TOB");
    setDisplayMsg(kMsgDupeTOB, 5, "Event contained multiple TOBs with the same ID");
    setDisplayMsg(kMsgNoGroup, 5, "Chain had no BW group associated with it");
    setDisplayMsg(kMsgTOBMerge, 5, "TOB merged");

    // Const strings literals to be referenced throughout
    set(kL0String, "L0");
    set(kL1String, "L1");
    set(kL2String, "L2");
    set(kEFString, "EF");
    set(kHLTString, "HLT");
    set(kAllString, "All");
    set(kDelimatorString, "____");
    set(kLumiBlockString, "LumiBlock");
    set(kUnknownString, "UNKNOWN");
    set(kBlankString, m_blankString);
    set(kDummyString, "dummy");
    set(kMultipleString, "Multiple");
    set(kAndString, "__AND__");
    set(kRateGlobalHLTString, "RATE_GLOBAL_HLT");
    set(kRateGlobalPhysicsMainString, "RATE_GLOBAL_PHYSICS");
    set(kRateGlobalL2String, "RATE_GLOBAL_L2");
    set(kRateGlobalL1String, "RATE_GLOBAL_L1");
    set(kRateGlobalL0String, "RATE_GLOBAL_L0");
    set(kRateExpressString, "RATE_EXPRESS");
    set(kRateUniqueString, "UNIQUE_");
    set(kCachedString, "Cached");
    set(kCalledString, "Called");
    set(kAllROIString, "All_ROIs");
    set(kNoneString, "None");
    set(kMuonString, "Muon");
    set(kEmTauString, "EMTau");
    set(kTauString, "Tau");
    set(kEmString, "EM");
    set(kJetString, "Jet");
    set(kJetEtString, "JetEt");
    set(kEnergyString, "Energy");
    set(kMissingEnergyString, "MET");
    set(kHTString, "HT");
    set(kMHTString, "MHT");
    set(kROBINString, "ROBIN");
    set(kROSString, "ROS");
    set(kAlwaysPassString, "UNSEEDED");
    set(kVersionString, _version, "Version");

    // Different variables to save
    for (int i = 0; i <= 90; ++i) {
      set(ConfKey_t(kVarSteeringTimeCPUType + 8192 + i), std::string("Rack" + intToString(i)) );
    }
    set(kVarTime, "Time");
    set(kVarRerunTime, "RerunTime");
    set(kVarPassTime, "PassTime");
    set(kVarTimeExec, "TimeExec");
    set(kVarTimeElapsed, "TimeElapsed");
    set(kVarFirstTime, "FirstTime");
    set(kVarAlgTime, "AlgTime");
    set(kVarCPUTime, "CPUTime");
    set(kVarROSTime, "ROSTime");
    set(kVarSteeringTime, "SteeringTime");
    set(kVarAlgCalls, "AlgCalls");
    set(kVarAlgCaches, "AlgCaches");
    set(kVarROSCalls, "ROSCalls");
    set(kVarROBReqs, "ROBCaches");
    set(kVarROBRets, "ROBRetrievals");
    set(kVarROBReqSize, "ROBCacheSize");
    set(kVarROBRetSize, "ROBRetrievalSize");
    set(kVarROBOther, "ROBOther");
    set(kVarROBNotOK, "ROBNotOK");
    set(kVarROBDisabled, "ROBDisabled");
    set(kVarROBPrefetched, "ROBPrefetched");
    set(kVarROBUnclassified, "ROBUnclassified");
    set(kVarROBIgnored, "ROBIgnored");
    set(kVarMu, "Mu");
    set(kVarBunchWeight, "BunchExtrapolationWeight");
    set(kVarCalls, "Calls");
    set(kVarCallsRaw, "CallsRaw");
    set(kVarCallsSlow, "CallsSlow");
    set(kVarEventsActive, "EventsActive");
    set(kVarEventsPassed, "EventsPassed");
    set(kVarEventsPassedDP, "EventsPassedDirectPrescale");
    set(kVarEventsPassedNoPS, "EventsPassedNoPS");
    set(kVarEventsPassthrough, "EventsPassthrough");
    set(kVarEventsPassRawStat, "EventsPassedRawStatistics");
    set(kVarEventsRunRawStat, "EventsRunRawStatistics");
    set(kVarUnbiasedPassed, "UnbiasedPassed");
    set(kVarUnbiasedRun, "UnbiasedRun");
    set(kVarEventsRun, "EventsRun");
    set(kVarEventsSlow, "EventsSlow");
    set(kVarTrigCostTime, "CostMonTime");
    set(kVarTexecTime, "TexecTime");
    set(kVarChainExecTime, "ChainExecTime");
    set(kVarResultBuildingTime, "ResultBuildingTime");
    set(kVarMonitoringTime, "MonitoringTime");
    set(kVarTotalPrescale, "TotalPrescale");
    set(kVarL1PassEvents, "L1PassEvents");
    set(kVarHLTEvents, "HLTEvents");
    set(kVarHLTPassEvents, "HLTPassEvents");
    set(kVarHLTPUs, "HLTPUs");
    set(kVarROI, "ROI");
    set(kVarType, "Type");
    set(kVarEta, "Eta");
    set(kVarPhi, "Phi");
    set(kVarEt, "ET");
    set(kVarArea, "Area");
    set(kVarL1Thresh, "L1Thresholds");
    set(kVarEventsPerLumiblock, "EventsPerLB");
    set(kVarOverlap, "Overlap");
    set(kVarOverlapDP, "OverlapDirectPrescale");
    set(kVarJetEta, "JetROIEta");
    set(kVarMuEta, "MuROIEta");
    set(kVarEmEta, "EMROIEta");
    set(kVarTauEta, "TauROIEta");
    set(kVarJetNThresh, "JetROINThresh");
    set(kVarMuNThresh, "MuROINThresh");
    set(kVarEmNThresh, "EMROINThresh");
    set(kVarTauNThresh, "TauROINThresh");
    // Different types of decoration
    set(kDecStartTime, "StartTime");
    set(kDecCallOrCache, "CallOrCache");
    set(kDecROIString, "ROIString");
    set(kDecROSString, "ROSString");
    set(kDecAlgClassName, "AlgClassName");
    set(kDecChainName, "ChainName");
    set(kDecSeqName, "SequenceName");
    set(kDecLbLength, "LBLength");
    set(kDecType, "Type");
    set(kDecID, "ID");
    set(kDecElapsedTime, "ElapsedTime");
    set(kDecRatesGroupName, "RatesGroupName");
    set(kDecPrescaleStr, "PrescaleString");
    set(kDecPrescaleVal, "PrescaleValue");
    set(kDecPrescaleValOnlineL1, "PrescaleValueOnlineL1");
    set(kDecUniqueRate, "UniqueRate");
    set(kDecUniqueFraction, "UniqueFraction");

    set(kListOfNoBunchLumiWeightChains, std::vector<std::string>(), "NoBunchLumiWeightChains", kUnlocked); // Populated
                                                                                                           // during
                                                                                                           // execution
    set(kListOfNoMuLumiWeightChains, std::vector<std::string>(), "NoMuLumiWeightChains", kUnlocked); // Populated during
                                                                                                     // execution
    set(kListOfNoLumiWeightChains, std::vector<std::string>(), "NoLumiWeightChains", kUnlocked); // Populated during
                                                                                                 // execution
    set(kListOfExpoMuLumiWeightChains, std::vector<std::string>(), "ExpoMuLumiWeightChains", kUnlocked); // Populated
                                                                                                         // during
                                                                                                         // execution

    set(kLBPerKeyset, std::vector<std::string>(), "LumiBlocksPerKeyset", kUnlocked); // Populated during execution

    // End of option parsing

    return kTRUE;
  }

  /**
   * Prints the contents of the program configuration.
   */
  void Config::dump() {
    Info("Config::dump", "TrigCostD3PD has %i configuration settings saved for this processing.", Int_t(m_settingsStr.size() + m_settingsVecStr.size() + m_settingsInt.size()));

    std::map<ConfKey_t, Int_t>::iterator _itInt;
    std::map<ConfKey_t, Long64_t>::iterator _itLong;
    std::map<ConfKey_t, Float_t>::iterator _itFloat;
    std::map<ConfKey_t, std::string>::iterator _itStr;
    std::map<ConfKey_t, std::vector<std::string> > ::iterator _itStrVec;
    std::map<ConfKey_t, std::vector<Int_t> > ::iterator _itIntVec;

    Int_t _i = -1;
    for (_itInt = m_settingsInt.begin(); _itInt != m_settingsInt.end(); ++_itInt) {
      const std::string _name = m_settingsName[_itInt->first];
      Info("Config::dump", " [INT:%i]\t[LOCK:%i]\t%s = %i", ++_i, (Int_t) getIsLocked(_itInt->first), _name.c_str(), _itInt->second);
    }
    _i = -1;
    for (_itLong = m_settingsLong.begin(); _itLong != m_settingsLong.end(); ++_itLong) {
      const std::string _name = m_settingsName[_itLong->first];
      Info("Config::dump", " [LONG:%i]\t[LOCK:%i]\t%s = %lli", ++_i, (Int_t) getIsLocked(_itLong->first), _name.c_str(), _itLong->second);
    }
    _i = -1;
    for (_itFloat = m_settingsFloat.begin(); _itFloat != m_settingsFloat.end(); ++_itFloat) {
      const std::string _name = m_settingsName[_itFloat->first];
      // Big or small?
      if (_itFloat->second > 1e4) { //BIG
        Info("Config::dump", " [FLOAT:%i]\t[LOCK:%i]\t%s = %e", ++_i, (Int_t) getIsLocked(_itFloat->first), _name.c_str(), _itFloat->second);
      } else { // small
        Info("Config::dump", " [FLOAT:%i]\t[LOCK:%i]\t%s = %f", ++_i, (Int_t) getIsLocked(_itFloat->first), _name.c_str(), _itFloat->second);
      }
    }
    _i = -1;
    for (_itStr = m_settingsStr.begin(); _itStr != m_settingsStr.end(); ++_itStr) {
      const std::string _name = m_settingsName[_itStr->first];
      Info("Config::dump", " [STRING:%i]\t[LOCK:%i]\t%s = \"%s\"", ++_i, (Int_t) getIsLocked(_itStr->first), _name.c_str(), _itStr->second.c_str());
    }
    _i = -1;
    for (_itStrVec = m_settingsVecStr.begin(); _itStrVec != m_settingsVecStr.end(); ++_itStrVec) {
      const std::string _name = m_settingsName[_itStrVec->first];
      Info("Config::dump", " [VEC<STR>:%i]\t[LOCK:%i]\t%s, size %u", ++_i, (Int_t) getIsLocked(_itStrVec->first), _name.c_str(), (UInt_t) _itStrVec->second.size());
      for (UInt_t _v = 0; _v < _itStrVec->second.size(); ++_v) {
        Info("Config::dump", "\t\t\t\t  [%u] \"%s\"", _v, _itStrVec->second.at(_v).c_str());
      }
    }
    _i = -1;
    for (_itIntVec = m_settingsVecInt.begin(); _itIntVec != m_settingsVecInt.end(); ++_itIntVec) {
      const std::string _name = m_settingsName[_itIntVec->first];
      Info("Config::dump", " [VEC<INT>:%i]\t[LOCK:%i]\t%s, size %u", ++_i, (Int_t) getIsLocked(_itIntVec->first), _name.c_str(), (UInt_t) _itIntVec->second.size());
      for (UInt_t _v = 0; _v < _itIntVec->second.size(); ++_v) {
        Info("Config::dump", "\t\t\t\t  [%u] \"%i\"", _v, _itIntVec->second.at(_v));
      }
    }
  }

  void Config::dumpToMeta(std::ofstream& _fout, JsonExport& _json) {
    std::map<ConfKey_t, Int_t>::iterator _itInt;
    std::map<ConfKey_t, Long64_t>::iterator _itLong;
    std::map<ConfKey_t, Float_t>::iterator _itFloat;
    std::map<ConfKey_t, std::string>::iterator _itStr;
    std::map<ConfKey_t, std::vector<std::string> > ::iterator _itStrVec;
    std::map<ConfKey_t, std::vector<Int_t> > ::iterator _itIntVec;

    for (_itInt = m_settingsInt.begin(); _itInt != m_settingsInt.end(); ++_itInt) {
      const std::string _name = m_settingsName[_itInt->first];
      _json.addLeafCustom(_fout, _name, intToString(_itInt->second));
    }
    for (_itLong = m_settingsLong.begin(); _itLong != m_settingsLong.end(); ++_itLong) {
      const std::string _name = m_settingsName[_itLong->first];
      _json.addLeafCustom(_fout, _name, intToString(_itLong->second));
    }
    for (_itFloat = m_settingsFloat.begin(); _itFloat != m_settingsFloat.end(); ++_itFloat) {
      _json.addLeafCustom(_fout, m_settingsName[_itFloat->first], floatToString(_itFloat->second));
    }
    for (_itStr = m_settingsStr.begin(); _itStr != m_settingsStr.end(); ++_itStr) {
      if (m_settingsName[_itStr->first] == _itStr->second) continue;
      _json.addLeafCustom(_fout, m_settingsName[_itStr->first], _itStr->second);
    }
    for (_itStrVec = m_settingsVecStr.begin(); _itStrVec != m_settingsVecStr.end(); ++_itStrVec) {
      for (UInt_t _v = 0; _v < _itStrVec->second.size(); ++_v) {
        _json.addLeafCustom(_fout, std::string(m_settingsName[_itStrVec->first] + intToString(_v)), _itStrVec->second.at(_v).c_str());
      }
    }
    for (_itIntVec = m_settingsVecInt.begin(); _itIntVec != m_settingsVecInt.end(); ++_itIntVec) {
      for (UInt_t _v = 0; _v < _itIntVec->second.size(); ++_v) {
        _json.addLeafCustom(_fout, std::string(m_settingsName[_itIntVec->first] + intToString(_v)), intToString(_itIntVec->second.at(_v)));
      }
    }
  }

  void Config::messageSuppressionReport() {
    for (std::map<ConfKey_t, Int_t>::iterator _itInt = m_settingsInt.begin(); _itInt != m_settingsInt.end(); ++_itInt) {
      ConfKey_t _key = _itInt->first;
      if (m_settingsMaxCalls[_key] == 0) {
        continue; // If no max message calls
      }
      if (m_settingsInt[_key] < m_settingsMaxCalls[_key]) {
        continue; // If no overflow suppresion messages
      }
      const std::string _name = m_settingsName[_key];
      UInt_t _suppressed = m_settingsInt[_key] - m_settingsMaxCalls[_key];
      Warning("Config::messageSuppressionReport", "There were %i additional instances of message '%s' which were suppressed.", _suppressed, _name.c_str());
    }
  }

  /**
   * Retrieve string configuration for given enum key.
   * @param _key enum key for this config.
   * @returns Requested configuration string or blank string if key not found.
   */
  const std::string& Config::getStr(ConfKey_t _key) {
    if (m_settingsStr.count(_key) == 1) {
      return m_settingsStr[_key];// Return the requested string
    } else if (m_settingsVecStr.count(_key) == 1) {
      static std::string _scratch; // singleton so this is OK
      std::stringstream _ss;
      for (auto _entry : m_settingsVecStr[_key]) {
        _ss << _entry << ", ";
      }
      _scratch = _ss.str();
      return _scratch;
    } else {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::getStr", "Unknown string-key %i. (name, if any:%s)", _key, _name.c_str());
    }
    return m_blankString;
  }

  /**
   * Retrieve string name of config enum
   * @param _key enum key for this config.
   * @returns Requested configuration enum's na,me.
   */
  const std::string& Config::getName(ConfKey_t _key) {
    if (m_settingsName.count(_key) == 0) {
      Error("Config::getName", "Unknown key %i", _key);
      return m_blankString;
    }
    // Return the requested string
    return m_settingsName[_key];
  }

  /**
   * Retrieve integer configuration for given enum key.
   * @param _key enum key for this config.
   * @return Requested configuration integer or -1 if key not found.
   */
  Int_t Config::getInt(ConfKey_t _key) {
    if (m_settingsInt.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::getInt", "Unknown key %i %s", _key, _name.c_str());
      return -1;
    }
    return m_settingsInt[_key];
  }

  /**
   * Increase by one the integer or long for given enum key.
   * @param _key enum key for this config.
   */
  void Config::increment(ConfKey_t _key) {
    if (getIsLocked(_key, kTRUE) == kTRUE) return;

    if (m_settingsInt.count(_key) == 1) {
      ++m_settingsInt[_key];
    } else if (m_settingsLong.count(_key) == 1) {
      ++m_settingsLong[_key];
    } else {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::increment", "Unknown key %i %s", _key, _name.c_str());
      return;
    }
  }

  /**
   * Decrease by one the integer for given enum key.
   * @param _key enum key for this config.
   */
  void Config::decrement(ConfKey_t _key) {
    if (m_settingsInt.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::decrement", "Unknown key %i %s", _key, _name.c_str());
      return;
    }
    if (getIsLocked(_key, kTRUE) == kTRUE) return;

    --m_settingsInt[_key];
  }

  /**
   * Retrieve float configuration for given enum key.
   * @param _key enum key for this config.
   * @return Requested configuration integer or -1 if key not found.
   */
  Float_t Config::getFloat(ConfKey_t _key) {
    if (m_settingsFloat.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::getFloat", "Unknown key %i %s", _key, _name.c_str());
      return -1.;
    }
    return m_settingsFloat[_key];
  }

  /**
   * Retrieve long int configuration for given enum key.
   * @param _key enum key for this config.
   * @return Requested configuration integer or -1 if key not found.
   */
  Long64_t Config::getLong(ConfKey_t _key) {
    if (m_settingsLong.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::getLong", "Unknown key %i %s", _key, _name.c_str());
      return -1;
    }
    return m_settingsLong[_key];
  }

  /**
   * Retrieve size of vector storage for given enum key.
   * @param _key enum key for this config.
   * @return Size of vector for given key, or INT_MAX if key not found.
   */
  UInt_t Config::getVecSize(ConfKey_t _key) {
    if (m_settingsVecStr.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::getVecSize", "Unknown key %i %s", _key, _name.c_str());
      return INT_MAX;
    }
    return m_settingsVecStr[_key].size();
  }

  /**
   * Retrieve vector configuration for given key.
   * @param _key enum key for this config.
   * @param _location Location within vector to return.
   * @return String at vector location for requested key or blank string if key not found or out of range.
   */
  const std::string& Config::getVecEntry(ConfKey_t _key, UInt_t _location) {
    if (m_settingsVecStr.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::getVecEntry", "Unknown key %i %s", _key, _name.c_str());
      return m_blankString;
    }
    if (_location >= getVecSize(_key)) {
      Error("Config::getVecEntry", "Location %i is out of range", _location);
      return m_blankString;
    }
    return m_settingsVecStr[_key].at(_location);
  }

  /**
   * If a settings vector is registered with a given key, and it is unlocked, then append the given string.
   * @param _key enum key for this config.
   * @param _toAdd Const reference to string to add to the settings vector.
   * @return true on success.
   */
  Bool_t Config::addVecEntry(ConfKey_t _key, const std::string& _toAdd) {
    if (m_settingsVecStr.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::addVecEntry", "Unknown key %i %s", _key, _name.c_str());
      return kFALSE;
    }
    if (getIsLocked(_key, kTRUE) == kTRUE) return kFALSE;

    m_settingsVecStr[_key].push_back(_toAdd);
    return kTRUE;
  }

  /**
   * If a settings vector is registered with a given key, and it is unlocked, then remove the given entry.
   * Note the test is for an exact match, whereas getVecMatches() checks for a fuzzy match.
   * @param _key enum key for this config.
   * @param _toRemove Const reference to string to remove from the settings vector.
   * @return true if one or more entries were removed.
   */
  Bool_t Config::removeVecEntry(ConfKey_t _key, const std::string& _toRemove) {
    Bool_t _entriesRemoved = kFALSE;

    if (m_settingsVecStr.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::removeVecEntry", "Unknown key %i %s", _key, _name.c_str());
      return kFALSE;
    }
    if (getIsLocked(_key, kTRUE) == kTRUE) return kFALSE;

    std::vector<std::string>::iterator _it = m_settingsVecStr[_key].begin();
    while (_it != m_settingsVecStr[_key].end()) {
      if (_toRemove.compare((*_it)) == 0) { // Compare == 0 for exact match only
        _it = m_settingsVecStr[_key].erase(_it);
        _entriesRemoved = kTRUE;
      } else {
        ++_it;
      }
    }
    return _entriesRemoved;
  }

  /**
   * Remove all entries in a config vector
   * @param _key enum key for this config.
   * @return true if one or more entries were removed.
   */
  Bool_t Config::clearVec(ConfKey_t _key) {
    Bool_t _entriesRemoved = kFALSE;

    if (m_settingsVecStr.count(_key) == 0 && m_settingsVecInt.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::clearVec", "Unknown key %i %s", _key, _name.c_str());
      return kFALSE;
    }
    if (getIsLocked(_key, kTRUE) == kTRUE) return kFALSE;

    if (m_settingsVecStr[_key].size()) _entriesRemoved = kTRUE;
    m_settingsVecStr[_key].clear();
    if (m_settingsVecInt[_key].size()) _entriesRemoved = kTRUE;
    m_settingsVecInt[_key].clear();
    return _entriesRemoved;
  }

  /**
   * Check if a vector of strings configuration contains a match to the given string.
   * Note: the pattern may match a sub-string, it does not need exactly equal one vector entry.
   * To be clear: the stored value must be a sub-string of _entry
   * This behaviour can be changed based on the stored kPatternsExactMatch config
   * @param _key enum key for this config.
   * @param _entry The string to match.
   * @return kTRUE if the patter.
   */
  Bool_t Config::getVecMatches(ConfKey_t _key, const std::string& _entry) {
    if (m_settingsVecStr.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::getVecMatches", "Unknown key %i %s", _key, _name.c_str());
      return kFALSE;
    }

    for (UInt_t _i = 0; _i < m_settingsVecStr[_key].size(); ++_i) {
      if (m_settingsInt[kPatternsExactMatch] == 1) {
        // This chain is exactly the correct name
        if (_entry == m_settingsVecStr[_key].at(_i)) return kTRUE;
      } else {
        // This chain contains the correct string name
        if (_entry.find(m_settingsVecStr[_key].at(_i)) != std::string::npos) return kTRUE;
      }
    }
    return kFALSE;
  }

  /**
   * Check if a vector of ints configuration contains a match to the given int.
   * @param _key enum key for this config.
   * @param _entry The int to match.
   * @return kTRUE if matched.
   */
  Bool_t Config::getIntVecMatches(ConfKey_t _key, const Int_t _entry) {
    if (m_settingsVecInt.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::getIntVecMatches", "Unknown key %i %s", _key, _name.c_str());
      return kFALSE;
    }
    if (std::find(m_settingsVecInt[_key].begin(), m_settingsVecInt[_key].end(), _entry) != m_settingsVecInt[_key].end()) return kTRUE;

    return kFALSE;
  }

  /**
   * @param _key enum key for this config.
   * @return the vector of ints.
   */
  const std::vector<Int_t>& Config::getIntVec(ConfKey_t _key) {
    static std::vector<Int_t> _empty;

    if (m_settingsVecInt.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::getIntVecMatches", "Unknown key %i %s", _key, _name.c_str());
      return _empty;
    }
    return m_settingsVecInt[_key];
  }

  /**
   * Get if there is a setting stored with the given key. Regardless of type.
   * @param _key enum key for this config.
   * @return kTRUE if a setting is stored, with any type, for the given key.
   */
  Bool_t Config::getIsSet(ConfKey_t _key) {
    if (m_settingsVecStr.count(_key) == 1) return kTRUE;

    if (m_settingsVecInt.count(_key) == 1) return kTRUE;

    if (m_settingsInt.count(_key) == 1) return kTRUE;

    if (m_settingsLong.count(_key) == 1) return kTRUE;

    if (m_settingsStr.count(_key) == 1) return kTRUE;

    if (m_settingsFloat.count(_key) == 1) return kTRUE;

    return kFALSE;
  }

  /**
   * Get if to display a message, this assumes that _key has been preset to the maximum number of times the message
   *should be displayed before suppression.
   * @param _key enum key for this config.
   * @return kTRUE if a setting is stored, with any type, for the given key.
   */
  Bool_t Config::getDisplayMsg(ConfKey_t _key) {
    if (m_settingsInt.count(_key) == 0) {
      std::string _name;
      if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
      Error("Config::getVecEntry", "Unknown key %i %s", _key, _name.c_str());
      return kFALSE;
    }

    if (getInt(kNoMsgSuppression) == kTRUE) return kTRUE;

    m_settingsInt[_key]++;
    if (m_settingsInt[_key] == m_settingsMaxCalls[_key]) { // Warn of last time
      Info("Config::getDisplayMsg", "The following message has reached its suppression limit of %i and will not be displayed any more.", m_settingsInt[_key]);
    } else if (m_settingsInt[_key] > m_settingsMaxCalls[_key]) {
      return kFALSE;
    }
    return kTRUE;
  }

  /**
   * Store or overwrite a string configuration with a enum key.
   * If no name is given, then it is assumed it is its name is its value
   * @param _key enum key for this config.
   * @param _value Const string value to store.
   */
  void Config::set(ConfKey_t _key, const std::string _value, const std::string _name, LockStatus_t _lock) {
    if (getIsLocked(_key, kTRUE) == kTRUE) return;

    m_settingsStr[_key] = _value;
    m_settingsName[_key] = _name;
    if (_name == m_blankString) m_settingsName[_key] = _value;
    m_settingsLock[_key] = _lock;
  }

  /**
   * Store or overwrite a integer configuration with a enum key. Can be given an optional name.
   * @param _key enum key for this config.
   * @param _name Optional name for this entry.
   * @param _value Const integer value to store.
   */
  void Config::set(ConfKey_t _key, const Int_t _value, const std::string _name, LockStatus_t _lock) {
    if (getIsLocked(_key, kTRUE) == kTRUE) return;

    m_settingsInt[_key] = _value;
    m_settingsMaxCalls[_key] = 0;
    m_settingsName[_key] = _name;
    m_settingsLock[_key] = _lock;
    // This is a special flag, set for easy access.
    if (_key == kDebug) {
      m_debug = (Bool_t) _value;
    }
  }

  /**
   * Store or overwrite a integer configuration with a enum key. This versions is without any method to name the var.
   * @param _key enum key for this config.
   * @param _value Const integer value to store.
   */
  void Config::set(ConfKey_t _key, const Int_t _value, LockStatus_t _lock) {
    if (getIsLocked(_key, kTRUE) == kTRUE) return;

    m_settingsInt[_key] = _value;
    m_settingsMaxCalls[_key] = 0;
    m_settingsLock[_key] = _lock;
    // This is a special flag, set for easy access.
    if (_key == kDebug) {
      m_debug = (Bool_t) _value;
    }
  }

  /**
   * Store or overwrite a integer configuration with a enum key. Can be given an optional name.
   * This version is specific for config entries which will be used to restrict the display of error messages
   * @param _key enum key for this config.
   * @param _name Optional name for this entry.
   * @param _value Maximum number of itmes to display this error/warning/info message.
   */
  void Config::setDisplayMsg(ConfKey_t _key, const Int_t _value, const std::string _name) {
    m_settingsInt[_key] = 0;
    m_settingsMaxCalls[_key] = _value;
    m_settingsName[_key] = _name;
    m_settingsLock[_key] = kUnlocked; // Must not be locked as needs to be increased
  }

  /**
   * Store or overwrite a vector of strings configuration with a enum key.
   * @param _key enum key for this config.
   * @param _name Optional name for this entry.
   * @param _value Const vector of strings to store.
   */
  void Config::set(ConfKey_t _key, const std::vector< std::string > _value, const std::string _name, LockStatus_t _lock) {
    if (getIsLocked(_key, kTRUE) == kTRUE) return;

    m_settingsVecStr[_key] = _value;
    m_settingsName[_key] = _name;
    m_settingsLock[_key] = _lock;
  }

  /**
   * Store or overwrite a vector of ints configuration with a enum key.
   * @param _key enum key for this config.
   * @param _name Optional name for this entry.
   * @param _value Const vector of ints to store.
   */
  void Config::set(ConfKey_t _key, const std::vector< Int_t > _value, const std::string _name, LockStatus_t _lock) {
    if (getIsLocked(_key, kTRUE) == kTRUE) return;

    m_settingsVecInt[_key] = _value;
    m_settingsName[_key] = _name;
    m_settingsLock[_key] = _lock;
  }

  /**
   * Store or overwrite a long integer configuration with a enum key.
   * Note - this setter has a unique name to prevent ambiguity with the Int_t setter
   * @param _key enum key for this config.
   * @param _name Optional name for this entry.
   * @param _value Long64_t to store.
   */
  void Config::setLong(ConfKey_t _key, const Long64_t _value, const std::string _name, LockStatus_t _lock) {
    if (getIsLocked(_key, kTRUE) == kTRUE) return;

    m_settingsLong[_key] = _value;
    m_settingsName[_key] = _name;
    m_settingsLock[_key] = _lock;
  }

  /**
   * Store or overwrite a float configuration with a enum key.
   * Note - this setter has a unique name to prevent ambiguity with the Int_t setter
   * @param _key enum key for this config.
   * @param _name Optional name for this entry.
   * @param _value Float to store.
   */
  void Config::setFloat(ConfKey_t _key, const Float_t _value, const std::string _name, LockStatus_t _lock) {
    if (getIsLocked(_key, kTRUE) == kTRUE) return;

    m_settingsFloat[_key] = _value;
    m_settingsName[_key] = _name;
    m_settingsLock[_key] = _lock;
  }

  /**
   * Check if this key has been used before, and if so then whether the lock was set to true. Regardles of type.
   * @param _key enum key for this config.
   * @param _printErrorMsg Set to kTRUE if the intention was to modify this record, will display error message to user.
   * @return kTRUE if the key exists and the lock was set.
   */
  Bool_t Config::getIsLocked(ConfKey_t _key, Bool_t _printErrorMsg) {
    if (m_settingsLock.count(_key) == 1) {
      if (m_settingsLock[_key] == kLocked) {
        std::string _name;
        if (m_settingsName.count(_key) == 1) _name = m_settingsName[_key];
        if (_printErrorMsg == kTRUE) Error("Config::getIsLocked", "The configuration key %i (name, if any:%s) is LOCKED and cannot be modified.", _key, _name.c_str());
        return kTRUE;
      }
    }
    return kFALSE;
  }

  /**
   * As checking the debug flag is such a common opertaion, we move it to having its own dedicated bool.
   * @return kTRUE if the debug flag is set.
   */
  Bool_t Config::debug() {
    return m_debug;
  }

  Bool_t Config::getConfKeyNameFromString(const std::string& s, ConfKey_t& result) {
    Bool_t foundConfKey = (Bool_t) false;

    for (auto iter = m_settingsName.begin(); iter != m_settingsName.end(); ++iter) {
      if (iter->second == s) {
        foundConfKey = (Bool_t) true;
        result = iter->first;
      }
    }
    return foundConfKey;
  }
} // namespace TrigCostRootAnalysis
