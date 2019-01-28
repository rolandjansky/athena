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
    static Int_t doL2 = kFALSE;
    static Int_t doEF = kFALSE;
    static Int_t doL2EF = kFALSE;
    static Int_t doHLT = kFALSE;
    // Range Summaries
    static Int_t summaryPerHLTConfig = kFALSE;
    static Int_t summaryPerLumiBlock = kFALSE;
    static Int_t summaryAll = kFALSE;
    // Monitors
    static Int_t monitorAll = kFALSE;
    static Int_t monitorChains = kFALSE;
    static Int_t monitorChainAlgs = kFALSE;
    static Int_t monitorSequences = kFALSE;
    static Int_t monitorSequenceAlgs = kFALSE;
    static Int_t monitorAlgs = kFALSE;
    static Int_t monitorAlgClass = kFALSE;
    static Int_t monitorAllChainSeqAlgs = kFALSE;
    static Int_t monitorROS = kFALSE;
    static Int_t monitorAllROS = kFALSE;
    static Int_t monitorROBIN = kFALSE;
    static Int_t monitorROSAlgs = kFALSE;
    static Int_t monitorROSChains = kFALSE;
    static Int_t monitorL1 = kFALSE;
    static Int_t monitorFullEvent = kFALSE;
    static Int_t monitorGlobals = kFALSE;
    static Int_t monitorEventProfile = kFALSE;
    static Int_t monitorROI = kFALSE;
    static Int_t monitorRates = kFALSE;
    static Int_t monitorRatesUpgrade = kFALSE;
    static Int_t monitorSliceCPU = kFALSE;
    // Output
    static Int_t doOutputPng = kFALSE;
    static Int_t doOutputPdf = kFALSE;
    static Int_t doOutputHist = kFALSE;
    static Int_t doOutputCsv = kFALSE;
    static Int_t doOutputXML = kFALSE;
    static Int_t doOutputRatesGraph = kFALSE;
    static Int_t doOutputMenus = kFALSE;
    static Int_t doOutputCanvas = kFALSE;
    static Int_t linkOutputDirectory = kFALSE;
    static Int_t writeDummyPSXML = kFALSE;
    // Misc
    static Int_t ratesMode = kFALSE;
    static Int_t ratesUpgradeMode = kFALSE;
    static Int_t costMode = kFALSE;
    static Int_t onlineMode = kFALSE;
    static Int_t outputModeStandard = kFALSE;
    static Int_t outputTagFromAthena = kFALSE;
    static Int_t cleanAll = kFALSE;
    static Int_t doEBWeighting = kFALSE;
    static Int_t writeEBWeightXML = kFALSE;
    static Int_t debug = kFALSE;
    static Int_t ratesForcePass = kFALSE;
    static Int_t ratesScaleByPS = kFALSE;
    static Int_t matchL1RandomToOnline = kFALSE;
    static Int_t doUniqueRates = kFALSE;
    static Int_t doGroupOverlap = kFALSE;
    static Int_t doAllOverlap = kFALSE;
    static Int_t showVersion = kFALSE;
    static Int_t extraplolate8To13 = kFALSE;
    static Int_t extraplolate13To5 = kFALSE;
    static Int_t doNotWriteMetadata = kFALSE;
    static Int_t noMsgSup = kFALSE;
    static Int_t noOnlineDTCorrection = kFALSE;
    static Int_t noUpgradePileupScaling = kFALSE;
    static Int_t noUpgradeBunchScaling = kFALSE;
    static Int_t doCPS = kFALSE;
    static Int_t patternsInvert = kFALSE;
    static Int_t isCPUPrediction;
    static Int_t doUpgradeRatesScan = kFALSE;
    static Int_t patternsExactMatch = kFALSE;
    static Int_t ignoreNonPhysBunchGroups = kFALSE;
    static Int_t noLBRescaling = kFALSE;
    static Int_t useDefaultLumiScalingExceptions = kFALSE;
    static Int_t useDefaultExponentialScalingList = kFALSE;
    static Int_t upgradeMergeTOBOverlap = kFALSE;
    static Int_t doExponentialMu = kFALSE;
    static Int_t invertHighMuRunVeto = kFALSE;
    static Int_t ignoreGRL = kFALSE;
    static Int_t ignorePSGreaterThanOne = kFALSE;

    // User options
    std::vector< std::string > inputFiles;
    std::vector< std::string > patternsMonitor;
    std::vector< std::string > patternsOutput;
    std::vector< std::string > patternsUnique;
    std::vector< std::string > patternsExclude;
    std::vector< std::string > patternsOverlap;
    std::vector< std::string > patternsNoLumiWeight;
    std::vector< std::string > patternsNoMuLumiWeight;
    std::vector< std::string > patternsNoBunchLumiWeight;
    std::vector< std::string > patternsExpoMuLumiWeight;
    std::vector< Int_t > useOnlyTheseBCIDs;
    std::vector< Int_t > eventPicker;
    std::string fileList = "";
    std::string fullEventFileList = "";
    std::string treeName = "trig_cost";
    std::string L2Prefix = "TrigCostL2_";
    std::string EFPrefix = "TrigCostEF_";
    std::string HLTPrefix = "TrigCostHLT_";
    std::string configPrefix = "trig_costMeta/TrigConfTree";
    std::string outRootFilename = "TrigCostRoot_Results.root";
    std::string outImageDirectory = "images";
    std::string outCSVDirectory = "csv";
    std::string outXMLDirectory = "xml";
    std::string outRatesGraphFilename = "ratesGraph.json";
    std::string outRatesWarningsFilename = "rates_WARNINGS.txt";
    std::string outDirectory = "costMonitoring_%t_%r";
    std::string outRootDirectory = ".";
    std::string outTag = "LOCAL";
    std::string userDetails = "";
    std::string menuXML = "";//"HLTconfig_Physics_pp_v5_19.2.0.xml";
    std::string prescaleXML1 = "";//"cool_208354_366_366.xml"; // This is an old XML for test purposes
    std::string prescaleXML2 = "";
    std::string ROSXML = "rob-ros-robin-2015.xml";
    std::string version = "TrigCostRootAnalysis-00-10-13";
    std::string upgradeScenario = "";
    std::string jira = "";
    std::string multiRun = "";
    std::string AutoMonXMLName = "AutoMonConfig.xml";

    Int_t lbBegin = INT_MIN;
    Int_t lbEnd = INT_MAX;
    UInt_t nEvents = INT_MAX;
    UInt_t nSkip = 0;
    UInt_t nThread = 5;
    UInt_t nLbFullSummary = 4;
    UInt_t nLbFullSkip = 50;
    UInt_t nHLTConfigSummary = 3;
    UInt_t nLbPerHLTConfig = 20;
    UInt_t defaultLBLength = 30;
    UInt_t slowThreshold = 1000;
    UInt_t slowEventThreshold = 120000;
    UInt_t histogramBins = 50;
    UInt_t maxNumberFullEvents = 10;
    UInt_t ratesOverlapWarning = 80;
    UInt_t maxMultiSeed = 1;
    UInt_t maxMultiSeedForGroup = 15;
    UInt_t runNumber = 0;
    UInt_t messageWait = 10;
    Float_t rateFallbackPrescaleL1 = FLT_MIN;
    Float_t rateFallbackPrescaleHLT = FLT_MIN;
    Float_t basicWeight = 1.;
    Float_t predictionLumi = 0.;
    Float_t runLumi = 0.;
    Float_t binMin = FLT_MIN;
    Float_t binMax = FLT_MIN;
    Float_t targetMu = 0.;
    Float_t expoRateScaleModifierL1 = 0.105; // From high-mu run
    Float_t expoRateScaleModifierHLT = 0.111; // From high-mu run

    // Parse CLI
    Int_t status = 0;
    while (kTRUE) {
      static struct option longOptions[] = {
        {
          "help", no_argument, 0, 0
        },
        {
          "cleanAll", no_argument, &cleanAll, 1
        },
        {
          "debug", no_argument, &debug, 1
        },
        {
          "noMsgSuppression", no_argument, &noMsgSup, 1
        },
        {
          "doL2", no_argument, &doL2, 1
        },
        {
          "doEF", no_argument, &doEF, 1
        },
        {
          "doL2EF", no_argument, &doL2EF, 1
        },
        {
          "doHLT", no_argument, &doHLT, 1
        },
        {
          "doOutputCanvas", no_argument, &doOutputCanvas, 1
        },
        {
          "doOutputHist", no_argument, &doOutputHist, 1
        },
        {
          "doOutputPNG", no_argument, &doOutputPng, 1
        },
        {
          "doOutputPDF", no_argument, &doOutputPdf, 1
        },
        {
          "doOutputCSV", no_argument, &doOutputCsv, 1
        },
        {
          "doOutputRatesXML", no_argument, &doOutputXML, 1
        },
        {
          "doOutputRatesGraph", no_argument, &doOutputRatesGraph, 1
        },
        {
          "doOutputMenus", no_argument, &doOutputMenus, 1
        },
        {
          "outputTagFromAthena", no_argument, &outputTagFromAthena, 1
        },
        {
          "monitorAll", no_argument, &monitorAll, 1
        },
        {
          "monitorChains", no_argument, &monitorChains, 1
        },
        {
          "monitorChainAlgs", no_argument, &monitorChainAlgs, 1
        },
        {
          "monitorSequences", no_argument, &monitorSequences, 1
        },
        {
          "monitorSequenceAlgs", no_argument, &monitorSequenceAlgs, 1
        },
        {
          "monitorAlgs", no_argument, &monitorAlgs, 1
        },
        {
          "monitorAlgClass", no_argument, &monitorAlgClass, 1
        },
        {
          "monitorAllChainSeqAlgs", no_argument, &monitorAllChainSeqAlgs, 1
        },
        {
          "monitorAllChainSeqAlg", no_argument, &monitorAllChainSeqAlgs, 1
        }, // Back comp
        {
          "monitorROS", no_argument, &monitorROS, 1
        },
        {
          "monitorAllROS", no_argument, &monitorAllROS, 1
        },
        {
          "monitorROBIN", no_argument, &monitorROBIN, 1
        },
        {
          "monitorROSAlgs", no_argument, &monitorROSAlgs, 1
        },
        {
          "monitorROSChains", no_argument, &monitorROSChains, 1
        },
        {
          "monitorROI", no_argument, &monitorROI, 1
        },
        {
          "monitorL1", no_argument, &monitorL1, 1
        },
        {
          "monitorFullEvent", no_argument, &monitorFullEvent, 1
        },
        {
          "monitorGlobals", no_argument, &monitorGlobals, 1
        },
        {
          "monitorEventProfile", no_argument, &monitorEventProfile, 1
        },
        {
          "monitorRates", no_argument, &monitorRates, 1
        },
        {
          "monitorRatesUpgrade", no_argument, &monitorRatesUpgrade, 1
        },
        {
          "monitorSliceCPU", no_argument, &monitorSliceCPU, 1
        },
        {
          "useEBWeight", no_argument, &doEBWeighting, 1
        },
        {
          "doCPS", no_argument, &doCPS, 1
        },
        {
          "writeEBXML", no_argument, &writeEBWeightXML, 1
        },
        {
          "summaryPerHLTConfig", no_argument, &summaryPerHLTConfig, 1
        },
        {
          "summaryPerLumiBlock", no_argument, &summaryPerLumiBlock, 1
        },
        {
          "summaryAll", no_argument, &summaryAll, 1
        },
        {
          "forceAllPass", no_argument, &ratesForcePass, 1
        },
        {
          "doUniqueRates", no_argument, &doUniqueRates, 1
        },
        {
          "doGroupOverlap", no_argument, &doGroupOverlap, 1
        },
        {
          "doAllOverlap", no_argument, &doAllOverlap, 1
        },
        {
          "scaleRatesByPS", no_argument, &ratesScaleByPS, 1
        },
        {
          "writeDummyPSXML", no_argument, &writeDummyPSXML, 1
        },
        {
          "version", no_argument, &showVersion, 1
        },
        {
          "linkOutputDirectory", no_argument, &linkOutputDirectory, 1
        },
        {
          "extrapolate8To13", no_argument, &extraplolate8To13, 1
        },
        {
          "extrapolate13To5", no_argument, &extraplolate13To5, 1
        },
        {
          "doNotWriteMetadata", no_argument, &doNotWriteMetadata, 1
        },
        {
          "matchL1RandomToOnline", no_argument, &matchL1RandomToOnline, 1
        },
        {
          "ratesUpgradeMode", no_argument, &ratesUpgradeMode, 1
        },
        {
          "ratesMode", no_argument, &ratesMode, 1
        },
        {
          "costMode", no_argument, &costMode, 1
        },
        {
          "onlineMode", no_argument, &onlineMode, 1
        },
        {
          "outputModeStandard", no_argument, &outputModeStandard, 1
        },
        {
          "noOnlineDTCorrection", no_argument, &noOnlineDTCorrection, 1
        },
        {
          "noUpgradePileupScaling", no_argument, &noUpgradePileupScaling, 1
        },
        {
          "noUpgradeBunchScaling", no_argument, &noUpgradeBunchScaling, 1
        },
        {
          "patternsInvert", no_argument, &patternsInvert, 1
        },
        {
          "isCPUPrediction", no_argument, &isCPUPrediction, 1
        },
        {
          "doUpgradeRatesScan", no_argument, &doUpgradeRatesScan, 1
        },
        {
          "patternsExactMatch", no_argument, &patternsExactMatch, 1
        },
        {
          "ignoreNonPhyBunchGroups", no_argument, &ignoreNonPhysBunchGroups, 1
        },
        {
          "noLBRescaling", no_argument, &noLBRescaling, 1
        },
        {
          "useDefaultLumiScalingExceptions", no_argument, &useDefaultLumiScalingExceptions, 1
        },
        {
          "useDefaultExponentialScalingList", no_argument, &useDefaultExponentialScalingList, 1
        },
        {
          "upgradeMergeTOBOverlap", no_argument, &upgradeMergeTOBOverlap, 1
        },
        {
          "doExponentialMu", no_argument, &doExponentialMu, 1
        },
        {
          "ignoreGRL", no_argument, &ignoreGRL, 1
        },  
        {
          "ignorePSGreaterThanOne", no_argument, &ignorePSGreaterThanOne, 1
        }, 
        {
          "invertHighMuRunVeto", no_argument, &invertHighMuRunVeto, 1
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
          "maxMultiSeedForGroup", required_argument, 0, '9'
        },

        {
          0, 0, 0, 0
        }
      };

      Int_t optionIndex = 0;
      status = getopt_long_only(argc, argv, "betlEf:", longOptions, &optionIndex);
      if (status == -1) {
        break;
      }

      std::stringstream ss;
      switch (status) {
      case 0:
        // I set a flag
        if (longOptions[optionIndex].flag != 0) {
          break;
        }
        //std::cout << "Given option " << _longOptions[optionIndex].name;
        if (std::string(longOptions[optionIndex].name) == "help") {
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
          std::cout << "--summaryPerLumiBlock\t\t\t\tOutput a summary for each for the first " << nLbFullSummary <<
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
          std::cout << "--histogramBins " << histogramBins << "\t\t\t\tNumber of bins to use per histogram." <<
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
          std::cout << "--nThreads " << nThread <<
            "\t\t\t\tNumber of concurrent threads to use when running monitors. Setting to 1 disables multitasking." <<
            std::endl;
#endif
          std::cout << "--lbBegin INT_MIN\t\t\t\tLowest value luminosity block from input to use." << std::endl;
          std::cout << "--lbEnd INT_MAX\t\t\t\t\tHighest value luminosity block from input to use." << std::endl;
          std::cout << "--jira\t\t\t\t\t\tLink to a bug report associated with this processing." << std::endl;
          std::cout <<
            "--useEBWeight\t\t\t\t\tApply precalculated weights to un-weight EnhancedBias data to MinimumBias data." <<
            std::endl;
          std::cout << "--basicWeight " << basicWeight <<
            "\t\t\t\t\tBase event weight. Can be used to apply global scaling factor to run." << std::endl;
          std::cout <<
            "--extrapolate8To13\t\t\t\tExperimental parameter! Attempt to evolve 8 TeV data/MC to 13 TeV based on HLT result."
                    << std::endl;
          std::cout <<
            "--extrapolate13To5\t\t\t\tExperimental parameter! Attempt to devolve 13 TeV data/MC to 5 TeV based on HLT result."
                    << std::endl;
          std::cout << "--nEvents INT_MAX\t\t\t\tNumber of input events from all files to run over." << std::endl;
          std::cout << "--nSkip " << nSkip << "\t\t\t\t\tNumber of events to skip." << std::endl;
          std::cout << "--slowThreshold " << slowThreshold <<
            "\t\t\t\tTime in milliseconds. Execution times greater than this are flagged as slow." << std::endl;
          std::cout << "--slowEventThreshold " << slowEventThreshold <<
            "\t\t\tTime in milliseconds. Full event executions times greater than this are flagged as slow." <<
            std::endl;
          std::cout << "--nLbFullSummary " << nLbFullSummary <<
            "\t\t\t\tNumber of luminosity blocks, starting from lbBegin, to produce a full summary for." << std::endl;
          std::cout << "--nLbFullSkip " << nLbFullSkip <<
            "\t\t\t\tHow many luminosity blocks to skip per full lb summary." << std::endl;
          std::cout << "--nHLTConfigSummary " << nHLTConfigSummary << "\t\t\t\tHow many HLT configs to monitor." <<
            std::endl;
          std::cout << "--nLbPerHLTConfig " << nLbPerHLTConfig <<
            "\t\t\t\tHow many LB to process per key summary, counted from the LB of the 1st event seen with this config."
                    << std::endl;
          std::cout << "--nFullEventSummaries " << maxNumberFullEvents <<
            "\t\t\tMax number of events (picked at random) to save full executions records for." << std::endl;
          std::cout << "--defaultLBLength " << defaultLBLength <<
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
          std::cout << "--prescaleXML1 \"" << prescaleXML1 <<
            "\"\t\t\t\tPrescale/Menu/L1 XML file from which to read custom prescales for rates calculation (place in /data or current dir for Athena use)."
                    << std::endl;
          std::cout << "--prescaleXML2 \"" << prescaleXML2 <<
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
          std::cout << "--expoRateScaleModifierL1 " << expoRateScaleModifierL1 <<
            "\t\t\tMultiplier to exponent for L1 exponential in <mu> rates extrapolation." << std::endl;
          std::cout << "--expoRateScaleModifierHLT " << expoRateScaleModifierHLT <<
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
          std::cout << "--ratesOverlapWarning " << ratesOverlapWarning <<
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
          std::cout << "--maxMultiSeed " << maxMultiSeed <<
            "\t\t\t\tMaximum number of L1 seeds a chain can have before it is dropped from global Union rate groups due to exploding (2^nL1) computational complexity."
                    << std::endl;
          std::cout << "--maxMultiSeedForGroup " << maxMultiSeedForGroup <<
            "\t\t\tMaximum number of L1 seeds any individual Union group will allow due to exploding (2^nL1) computational complexity."
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
          std::cout << "--ROSXML \"" << ROSXML <<
            "\"\t\tROS ID mapping file from which to map ROBIN IDs to subdetectors (place in /data)." << std::endl;
          std::cout << "--treeName \"" << treeName << "\"\t\t\t\tName of TTree in ntuples containing cost data." <<
            std::endl;
          std::cout << "--prefixL2 \"" << L2Prefix << "\"\t\t\tPrefix of L2 cost branches." << std::endl;
          std::cout << "--prefixEF \"" << EFPrefix << "\"\t\t\tPrefix of EF cost branches." << std::endl;
          std::cout << "--prefixHLT \"" << HLTPrefix << "\"\t\t\tPrefix of HLT cost branches." << std::endl;
          std::cout << "--prefixConfig \"" << configPrefix <<
            "\"\tName of TTree in ntuple containing trigger configuration metadata." << std::endl;
          std::cout << "--outputRootDirectory \"" << outRootDirectory <<
            "\"\t\t\tFolder in which to place this programs output directory." << std::endl;
          std::cout << "--outputDirectory \"" << outDirectory <<
            "\"\tFolder to contain all program output, %r is replaced with the run number, %t with the tag." <<
          std::endl;
          std::cout << "--outputTag \"" << outTag <<
            "\"\t\t\t\tTag identifying this processing to be used in the output folder name (any underscores will be removed)."
                    << std::endl;
          std::cout <<
            "--outputTagFromAthena\t\t\t\tWhen running in Athena, this option will use the $AtlasVersion environment variable."
                    << std::endl;
          std::cout << "--outputRootFile \"" << outRootFilename << "\"\tName of ROOT file for results." << std::endl;
          std::cout << "--outputImageDirectory \"" << outImageDirectory <<
            "\"\t\t\tName of folder for image results." << std::endl;
          std::cout << "--outputCSVDirectory \"" << outCSVDirectory << "\"\t\t\tName of folder for CSV results." <<
            std::endl;
          std::cout << "--outputXMLDirectory \"" << outXMLDirectory << "\"\t\t\tName of folder for XML results." <<
            std::endl;
          std::cout << "--messageWait " << messageWait << "\t\t\t\tTime in seconds between progress reports." <<
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
        Error("Config::parseCLI", "The following option was not recognised: %s", longOptions[optionIndex].name);
        abort();
        break;

      case 'f':
        // I'm setting the input files
        // Do secondary loop to identify all input files
        inputFiles.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          inputFiles.push_back(std::string(argv[optind++]));
        }
        break;

      case 'p':
        // I'm setting the chain pattern monitor
        // Do secondary loop to identify all patterns
        patternsMonitor.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          patternsMonitor.push_back(std::string(argv[optind++]));
        }
        break;

      case 'P':
        // I'm setting the chain pattern output
        patternsOutput.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          patternsOutput.push_back(std::string(argv[optind++]));
        }
        break;

      case 'I':
        // I'm setting the chain pattern output
        patternsUnique.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          patternsUnique.push_back(std::string(argv[optind++]));
        }
        break;

      case 'v':
        // I'm setting the rate exclusion
        patternsExclude.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          patternsExclude.push_back(std::string(argv[optind++]));
        }
        break;

      case 'Y':
        // I'm setting the overlap list
        patternsOverlap.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          patternsOverlap.push_back(std::string(argv[optind++]));
        }
        break;

      case 'u':
        // I'm setting the user details string
        userDetails = std::string(optarg);
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          userDetails += std::string(" ") + std::string(argv[optind++]);
        }
        break;

      case '1':
        // I'm setting the patternsNoLumiWeight strings
        patternsNoLumiWeight.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          patternsNoLumiWeight.push_back(std::string(argv[optind++]));
        }
        break;

      case '2':
        // I'm setting the patternsNoMuLumiWeight strings
        patternsNoMuLumiWeight.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          patternsNoMuLumiWeight.push_back(std::string(argv[optind++]));
        }
        break;

      case '3':
        // I'm setting the patternsNoBunchLumiWeight strings
        patternsNoBunchLumiWeight.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          patternsNoBunchLumiWeight.push_back(std::string(argv[optind++]));
        }
        break;

      case '4':
        // I'm setting the patternsExpoMuLumiWeight strings
        patternsExpoMuLumiWeight.push_back(std::string(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          patternsExpoMuLumiWeight.push_back(std::string(argv[optind++]));
        }
        break;

      case '8':
        // I'm setting the useOnlyTheseBCIDs
        useOnlyTheseBCIDs.push_back(stringToInt(optarg));
        while (optind < argc) {
          if (std::string(argv[optind]).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          useOnlyTheseBCIDs.push_back(stringToInt(argv[optind++]));
        }
        break;

      case 'n':
        // N Events
        ss << optarg;
        ss >> nEvents;
        break;

      case 'a':
        // Rate fallback prescale L1
        ss << optarg;
        ss >> rateFallbackPrescaleL1;
        break;

      case 'A':
        // Rate fallback prescale HLT
        ss << optarg;
        ss >> rateFallbackPrescaleHLT;
        break;

      case 'h':
        // N histogram bins
        ss << optarg;
        ss >> histogramBins;
        break;

      case 'j':
        // Histogram bin min
        ss << optarg;
        ss >> binMin;
        break;

      case 'J':
        // N histogram bin max
        ss << optarg;
        ss >> binMax;
        break;

      case 's':
        // N Events to skip
        ss << optarg;
        ss >> nSkip;
        break;

      case 'W':
        // N Threads
        ss << optarg;
        ss >> nThread;
        break;

      case 'S':
        // Slow threshold
        ss << optarg;
        ss >> slowThreshold;
        break;

      case 'y':
        // Slow event threshold
        ss << optarg;
        ss >> slowEventThreshold;
        break;

      case 'F':
        // Max full event summaries
        ss << optarg;
        ss >> maxNumberFullEvents;
        break;

      case 'b':
        // Lumi block begining
        ss << optarg;
        ss >> lbBegin;
        break;

      case 'e':
        // Lumi block end
        ss << optarg;
        ss >> lbEnd;
        break;

      case 'z':
        // Message wait time
        ss << optarg;
        ss >> messageWait;
        break;

      case 'B':
        // Basic event weight
        ss << optarg;
        ss >> basicWeight;
        break;

      case 'L':
        // Number of lumi blocks to do a full summary for
        ss << optarg;
        ss >> nLbFullSummary;
        break;

      case 'k':
        // Number of lumi blocks to skip per full summary
        ss << optarg;
        ss >> nLbFullSkip;
        break;

      case 'd':
        // Default lumiblock length
        ss << optarg;
        ss >> defaultLBLength;
        break;

      case 'D':
        // inst L for prediction
        ss << optarg;
        ss >> predictionLumi;
        break;

      case 'U':
        // inst L of EB run
        ss << optarg;
        ss >> runLumi;
        break;

      case 'N':
        // User supplied run number
        ss << optarg;
        ss >> runNumber;
        break;

      case 'X':
        // Max multi seed for chains going into "big" groups such as total rates
        ss << optarg;
        ss >> maxMultiSeed;
        break;

      case '9':
        // Max multi seed for an individual grouo
        ss << optarg;
        ss >> maxMultiSeedForGroup;
        break;

      case 'q':
        // nHLTConfigSummary
        ss << optarg;
        ss >> nHLTConfigSummary;
        break;

      case 'Q':
        // nLbPerHLTConfig
        ss << optarg;
        ss >> nLbPerHLTConfig;
        break;

      case '5':
        // expoRateScaleModifier
        ss << optarg;
        ss >> expoRateScaleModifierL1;
        break;

      case '6':
        // expoRateScaleModifier
        ss << optarg;
        ss >> expoRateScaleModifierHLT;
        break;

      case 'K':
        // File list
        fileList = std::string(optarg);
        break;

      case 'V':
        // Full Event File List
        fullEventFileList = std::string(optarg);
        break;

      case 't':
        // Different tree name
        treeName = std::string(optarg);
        break;

      case '7':
        // Multirun
        multiRun = std::string(optarg);
        break;

      case 'm':
        // Specify upgrade scenario
        upgradeScenario = std::string(optarg);
        break;

      case 'T':
        // Different tag name
        outTag = std::string(optarg);
        break;

      case 'M':
        // Different prescale menu XML
        prescaleXML1 = std::string(optarg);
        break;

      case 'g': // NOTE same as M
        // Different prescale menu XML
        prescaleXML1 = std::string(optarg);
        break;

      case 'G':
        // Different prescale menu XML
        prescaleXML2 = std::string(optarg);
        break;

      case 'R':
        // Different ROS mapping
        ROSXML = std::string(optarg);
        break;

      case 'r':
        // Different output root
        outRootFilename = std::string(optarg);
        break;

      case 'i':
        // Different output image
        outImageDirectory = std::string(optarg);
        break;

      case 'C':
        // Different output csv dir
        outCSVDirectory = std::string(optarg);
        break;

      case 'x':
        // Different output xml dir
        outXMLDirectory = std::string(optarg);
        break;

      case 'o':
        // Different output directory
        outDirectory = std::string(optarg);
        break;

      case 'O':
        // Different output ROOT directory
        outRootDirectory = std::string(optarg);
        break;

      case 'l':
        // Different L2 prefix
        L2Prefix = std::string(optarg);
        break;

      case 'E':
        // Different EF prefix
        EFPrefix = std::string(optarg);
        break;

      case 'w':
        // different overlap percent
        ss << optarg;
        ss >> ratesOverlapWarning;
        break;

      case 'Z':
        // JIRA
        ss << optarg;
        ss >> jira;
        break;

      case 'H':
        // Different HLT prefix
        HLTPrefix = std::string(optarg);
        break;

      case 'c':
        // Different HLT prefix
        configPrefix = std::string(optarg);
        break;

      case '0':
        // Target mu
        ss << optarg;
        ss >> targetMu;
        break;

      default:
        Error("Config::parseCLI", "Supplied argument '%c' not recognised.", Char_t(status));
        abort();
      }
    }

    // Settings read. Now to check them and store them for use

    Info("Config::parseCLI", "Version %s", version.c_str());
    if (showVersion == kTRUE) {
      abort();
    }

    //////////////////////////
    //        ALIASES - how handy
    //////////////////////////

    if (ratesMode == kTRUE || ratesUpgradeMode == kTRUE) {
      Info("Config::parseCLI", "Setting up default options for a rates prediction job.");
      cleanAll = 1;
      doHLT = 1;
      summaryAll = 1;
      if (ratesMode) monitorRates = 1;
      if (ratesUpgradeMode) monitorRatesUpgrade = 1;
      doEBWeighting = 1;
      matchL1RandomToOnline = 1;
      outputModeStandard = 1;
      doCPS = 1;
      useDefaultLumiScalingExceptions = 1;
      // doExponentialMu = 1; // No longer have this flag by default
      if (doExponentialMu) useDefaultExponentialScalingList = 1;
    }

    if (costMode == kTRUE) {
      Info("Config::parseCLI", "Setting up default options for a cost job.");
      cleanAll = 1;
      doHLT = 1;
      summaryAll = 1;
      monitorChains = 1;
      monitorChainAlgs = 1;
      monitorSequences = 1;
      monitorAlgs = 1;
      monitorAlgClass = 1;
      monitorROS = 1;
      monitorROBIN = 1;
      monitorROI = 1;
      monitorGlobals = 1;
      monitorFullEvent = 1;
      outputModeStandard = 1;
      ignoreNonPhysBunchGroups = 1;
      monitorSliceCPU = 1;
    }

    if (onlineMode == kTRUE) {
      Info("Config::parseCLI", "Setting up default options for a online job.");
      cleanAll = 1;
      doHLT = 1;
      summaryPerHLTConfig = 1;
      summaryPerLumiBlock = 1;
      monitorAllChainSeqAlgs = 1;
      monitorChains = 1;
      monitorChainAlgs = 1;
      monitorSequences = 1;
      monitorAlgs = 1;
      monitorAlgClass = 1;
      monitorROS = 1;
      monitorROBIN = 1;
      monitorROI = 1;
      monitorGlobals = 1;
      monitorFullEvent = 1;
      outputModeStandard = 1;
      monitorSliceCPU = 1;
    }

    if (outputModeStandard == kTRUE) {
      Info("Config::parseCLI", "Setting up standard outputs.");
      doOutputHist = 1;
      doOutputCsv = 1;
      doOutputRatesGraph = 1;
      doOutputMenus = 1;
      doOutputXML = 1;
    }

    //////////////////////////
    //        ROOT FILES, CHAIN AND TREE NAMES
    //////////////////////////

    if (inputFiles.size() == 0 && fileList == m_blankString) {
      Error("Config::parseCLI", "Please supply at least one input file with --files or --fileList");
      return kFALSE;
    }
    // If we have a file list - fill the input files
    if (fileList != m_blankString) {
      std::ifstream ifs(fileList.c_str());
      std::string line;
      while (std::getline(ifs, line)) {
        inputFiles.push_back(line);
      }
    }
    // check names
    for (UInt_t n = 0; n < inputFiles.size(); ++n) {
      if (inputFiles.at(n).substr(0, 5) == "/eos/") {
        inputFiles.at(n) = std::string("root:://eosatlas.cern.ch/" + inputFiles.at(n));
      } else if (inputFiles.at(n).substr(0, 27) == "srm://srm-eosatlas.cern.ch/") {
        inputFiles.at(n) = std::string("root:://eosatlas.cern.ch//") + inputFiles.at(n).substr(27, std::string::npos);
      }
    }

    // If we have a full event list - fill the input files
    if (fullEventFileList != m_blankString) {
      std::ifstream ifs(fullEventFileList.c_str());
      std::string line;
      while (std::getline(ifs, line)) {
        eventPicker.push_back(stringToInt(line));
      }
      set(kEventPickList, eventPicker, "EventPickList");
    }

    set(kInputFiles, inputFiles, "InputFiles");
    setLong(kNEvents, nEvents, "NEvents");
    setLong(kNSkip, nSkip, "NSkip");
    set(kTreeName, treeName, "TreeName");
    set(kL2Prefix, L2Prefix, "L2Prefix");
    set(kEFPrefix, EFPrefix, "EFPrefix");
    set(kHLTPrefix, HLTPrefix, "HLTPrefix");
    set(kConfigPrefix, configPrefix, "ConfigPrefix");

    //////////////////////////
    //        HLT LEVELS AND RUN RANGES TO PROCESS
    //////////////////////////

    // Trigger Level settings
    if (doL2 + doEF + doL2EF + doHLT != 1) {
      //  Allow use of doL2 and doEF instead of --doL2EF
      if (!(doL2 == 1 && doEF == 1 && doHLT == 0)) {
        Warning("Config::parseCLI",
                "Not specified a HLT level to run (--doL2, --doEF, --doL2EF or --doHLT). Will default to --doHLT.");
        doHLT = 1;
      }
    }
    set(kDoL2, (Int_t) (doL2 || doL2EF), "DoL2");
    set(kDoEF, (Int_t) (doEF || doL2EF), "DoEF");
    set(kDoHLT, doHLT, "DoHLT");
    set(kDoHLTOnline, 0, "DoHLTOnline"); // If unified HLT was used online - not yet ever true

    // Summary range settings
    if (summaryAll + summaryPerHLTConfig + summaryPerLumiBlock == 0) {
      Warning("Config::parseCLI", "Not specified a time range to summaries. Will default to --summaryPerHLTConfig.");
      summaryPerHLTConfig = 1;
    }
    set(kDoAllSummary, summaryAll, "DoSummaryAll");
    set(kDoKeySummary, summaryPerHLTConfig, "DoKeySummary");
    set(kDoLumiBlockSummary, summaryPerLumiBlock, "DoLBSummary");

    // Number of events to save full information for
    set(kFullEventMaxNumToSave, maxNumberFullEvents, "MaxNumberFullEvents");
    set(kCurrentEventIsSlow, 0, "SlowFlag", kUnlocked);
    if (nEvents != INT_MAX) {
      set(kFullEventSaveOnePer, (Int_t) (1 + (nEvents / getInt(kFullEventMaxNumToSave))), "FullEventSaveOncePer");
    } else {
      set(kFullEventSaveOnePer, 1000000, "FullEventSaveOncePer", kUnlocked);//This is just a generic fallback. We can
                                                                            // update it later in the event loop if need
                                                                            // be
    }

    //////////////////////////
    //        MONITORS TO INCLUDE
    //////////////////////////

    // This flag just enables others
    if (monitorAllChainSeqAlgs == 1) {
      monitorChains = 1;
      monitorChainAlgs = 1;
      monitorSequences = 1;
      monitorSequenceAlgs = 1;
      monitorAlgs = 1;
      monitorAlgClass = 1;
    }

    if (monitorAllROS == 1) {
      monitorROS = 1;
      monitorROBIN = 1;
      monitorROSAlgs = 1;
      monitorROSChains = 1;
    }

    int nMon = monitorAll
                + monitorChains
                + monitorChainAlgs
                + monitorSequences
                + monitorSequenceAlgs
                + monitorAlgs
                + monitorAlgClass
                + monitorROS
                + monitorROBIN
                + monitorROSAlgs
                + monitorROSChains
                + monitorROI
                + monitorL1
                + monitorFullEvent
                + monitorGlobals
                + monitorEventProfile
                + monitorRates
                + monitorRatesUpgrade
                + monitorSliceCPU
                + writeEBWeightXML;

    if (nMon == 0) {
      Warning("Config::parseCLI", "Not specified a monitor to use. Will default to --monitorAll.");
      monitorAll = 1;
    }

    // Check for rates only processing, and relax the LB requirement
    Int_t ratesOnly = 0;
    if ((monitorRates == 1 || monitorRatesUpgrade == 1) && nMon == (monitorRates + monitorRatesUpgrade)) {
      ratesOnly = 1;
      if (nLbPerHLTConfig != INT_MAX && summaryAll == 0 && summaryPerHLTConfig == 1) {
        Warning("Config::parseCLI",
                "We're in 'rates only' mode, --nLbPerHLTConfig is ignored and all LB are processed per HLT config.");
      }
    }
    set(kRatesOnly, ratesOnly, "RatesOnly");

    // We save monitorAll separately as it is picked up in a time saving loop later
    set(kDoAllMonitor, monitorAll, "MonitorAll"); // This one is magic, and will be picked up in a loop later - force
                                                   // enabling all others
    set(kDoChainMonitor, monitorChains, "ChainMonitor");
    set(kDoChainAlgorithmMonitor, monitorChainAlgs, "ChainAlgorithmMonitor");
    set(kDoSequenceMonitor, monitorSequences, "SequenceMonitor");
    set(kDoSequenceAlgorithmMonitor, monitorSequenceAlgs, "SequenceAlgorithmMonitor");
    set(kDoAlgorithmMonitor, monitorAlgs, "AlgorithmMonitor");
    set(kDoAlgorithmClassMonitor, monitorAlgClass, "AlgorithmClassMonitor");
    set(kDoSliceCPUMonitor, monitorSliceCPU, "SliceCPUMonitor");
    set(kDoROSMonitor, monitorROS, "ROSMonitor");
    set(kDoROBINMonitor, monitorROBIN, "ROBINMonitor");
    set(kDoROSAlgorithmMonitor, monitorROSAlgs, "ROSAlgorithmMonitor");
    set(kDoROSChainMonitor, monitorROSChains, "ROSChainMonitor");
    set(kDoROIMonitor, monitorROI, "ROIMonitor");
    set(kDoFullEventMonitor, monitorFullEvent, "FullEventMonitor");
    set(kDoGlobalsMonitor, monitorGlobals, "GlobalsMonitor");
    set(kDoEventProfileMonitor, monitorEventProfile, "EventProfileMonitor");
    set(kDoRatesMonitor, monitorRates, "RatesMonitor");
    set(kDoRatesUpgradeMonitor, monitorRatesUpgrade, "RatesUpgradeMonitor");

    // Number of passes, UpgradeMonitor needs 2 if we are doing upgrade pileup scaling
    if (monitorRatesUpgrade == kTRUE && noUpgradePileupScaling == kFALSE) {
      set(kNPasses, 2, "NPasses");
    } else {
      set(kNPasses, 1, "NPasses");
    }

    if (isZero(targetMu) &&
        (patternsNoLumiWeight.size() || patternsNoMuLumiWeight.size() || patternsNoBunchLumiWeight.size() ||
         patternsExpoMuLumiWeight.size())) {
      Error("Config::parseCLI",
            "If using --patternsNoLumiWeight, --patternsNoMuLumiWeight or --patternsNoBunchLumiWeight then --predictionMu must be specified too.");
      return kFALSE;
    }

    // Hard code some special cases
    if (useDefaultLumiScalingExceptions) {
      patternsNoLumiWeight.push_back("HLT_lumipeb_L1RD");
      patternsNoLumiWeight.push_back("HLT_lumipeb_vdm_L1RD");
      patternsNoLumiWeight.push_back("HLT_j0_perf_L1RD");
      patternsNoLumiWeight.push_back("HLT_ibllumi_L1RD");
      patternsNoLumiWeight.push_back("HLT_l1calocalib");
      patternsNoLumiWeight.push_back("HLT_sct_noise");
      patternsNoLumiWeight.push_back("HLT_noalg_L1RD");
    }

    if (useDefaultExponentialScalingList) {
      patternsExpoMuLumiWeight.push_back("HLT_noalg_L1XE");
      patternsExpoMuLumiWeight.push_back("HLT_xe");
      patternsExpoMuLumiWeight.push_back("L1_XE");
    }

    // String patterns to match when doing monitoring
    set(kPatternsMonitor, patternsMonitor, "PatternsMonitor", kUnlocked);
    set(kPatternsOutput, patternsOutput, "PatternsOutput", kUnlocked);
    set(kPatternsUnique, patternsUnique, "PatternsUnique");
    set(kPatternsExclude, patternsExclude, "PatternsExclude");
    set(kPatternsOverlap, patternsOverlap, "PatternsOverlap");
    set(kPatternsNoLumiWeight, patternsNoLumiWeight, "PatternsNoLumiWeight");
    set(kPatternsNoMuLumiWeight, patternsNoMuLumiWeight, "PatternsNoMuLumiWeight");
    set(kPatternsNoBunchLumiWeight, patternsNoBunchLumiWeight, "PatternsNoBunchLumiWeight");
    set(kPatternsExpoMuLumiWeight, patternsExpoMuLumiWeight, "PatternsExpoMuLumiWeight");


    // If there are patterns here, make sure we switch on unique rates
    if (patternsUnique.size() > 0 && doUniqueRates == kFALSE) {
      doUniqueRates = kTRUE;
      Info("Config::parseCLI", "Setting --doUniqueRates to TRUE because one or more --patternsUnique were supplied");
    }
    if (doUniqueRates && doExponentialMu) {
      Warning("Config::parseCLI", "Forcing doExponentialMu flag to FALSE otherwise can get negative unique rates.");
      doExponentialMu = kFALSE;
    }

    // Lumi block settings. Start, end, and number to fully summarise
    if (lbEnd < lbBegin) {
      Error("Config::parseCLI", "End lumi block number cannot be before starting lumi block number.");
      return kFALSE;
    }
    set(kLumiStart, lbBegin, "LumiBlockStart");
    set(kLumiEnd, lbEnd, "LumiBlockEnd");
    set(kNLbFullSummary, nLbFullSummary, "NLBFullSummary");
    set(kNLbFullSkip, nLbFullSkip, "NLBFullSkip");
    set(kNLbPerHLTConfig, nLbPerHLTConfig, "NLBPerHLTConfig");
    set(kNHLTConfigSummary, nHLTConfigSummary, "NHLTConfigSummary");

    // Are we doing time consuming ROS matching?
    Bool_t matching = kFALSE;
    if (monitorROS || monitorROBIN || monitorROSChains || monitorROSAlgs || monitorEventProfile) matching = kTRUE;

    set(kEnableROSToAlgMatching, (Int_t) matching, "RosToAlgMatching");

    //////////////////////////
    //        OUTPUT TO GENERATE
    //////////////////////////

    if (doOutputHist + doOutputCanvas + doOutputPng + doOutputPdf + doOutputCsv + doOutputXML == 0) {
      Warning("Config::parseCLI",
              "No output format specified. Will default to --doOutputHist --doOutputCSV --doOutputMenus --doOutputRatesGraph --doOutputRatesXML");
      doOutputHist = 1;
      doOutputCsv = 1;
      doOutputRatesGraph = 1;
      doOutputMenus = 1;
      doOutputXML = 1;
    }

    set(kOutputRootDirectory, outRootDirectory, "OutputRootFileSystemDirectory");
    // Combine the root output directory with the programs directory name here
    outDirectory = outRootDirectory + "/" + outDirectory;

    // See if we have been requested to get the AtlasVersion from Athena and use it in the tag
    if (outputTagFromAthena == kTRUE) {
      const Char_t* env = std::getenv("AtlasBuildStamp");
      if (env != NULL) {
        outTag = std::string(env);
      }
    }

    // Remove any "_" from the tag
    while (outTag.find("_") != std::string::npos) {
      outTag.replace(outTag.find("_"), 1, m_blankString);
    }

    set(kOutputPng, doOutputPng, "DoPNG");
    set(kOutputPdf, doOutputPdf, "DoPDF");
    set(kOutputImage, (Int_t) (doOutputPdf || doOutputPng), "DoOutputImage");
    set(kOutputRoot, (Int_t) (doOutputHist || doOutputCanvas), "DoOutputROOT");
    set(kOutputHist, doOutputHist, "DoOutputHist");
    set(kOutputCanvas, doOutputCanvas, "DoOutputCanvas");
    set(kOutputCsv, doOutputCsv, "DoOutputCSV");
    set(kOutputXML, doOutputXML, "DoOutputXML");
    set(kOutputRatesGraph, doOutputRatesGraph, "DoOutputRatesGraph");
    set(kOutputMenus, doOutputMenus, "DoOutputMenus");
    set(kOutputDirectory, outDirectory, "OutputDirectory", kUnlocked); // Do NOT lock this as we need to modify it to
                                                                        // add the run number in
    set(kOutputTag, outTag, "OutputTag");
    set(kOutputImageDirectory, outImageDirectory, "OutputImageDirectory");
    set(kOutputCSVDirectory, outCSVDirectory, "OutputCSVDirectory");
    set(kOutputXMLDirectory, outXMLDirectory, "OutputXMLDirectory");
    set(kOutputRatesGraphFilename, outRatesGraphFilename, "OutputRatesGraphFilename");
    set(kOutputRatesWarnings, outRatesWarningsFilename, "OutputRatesWarningsFilename");
    set(kOutputRootFilename, outRootFilename, "OutputROOTFileName");
    set(kLinkOutputDir, linkOutputDirectory, "LinkOutputDirectory");
    set(kWriteDummyPSXML, writeDummyPSXML, "WriteDummyPSXML");
    if (userDetails != "") {
      set(kUserDetails, userDetails, "Details", kUnlocked); // Modified later to include metadata
    }

    //////////////////////////
    //        MISC SETTINGS
    //////////////////////////

    // Cannot both use EB weights and export them
    if (doEBWeighting == kTRUE && writeEBWeightXML == kTRUE) {
      Error("Config::parseCLI", "Cannot use both --useEBWeight and --writeEBXML");
      abort();
    }

    if (doEBWeighting == kTRUE && ratesScaleByPS == kTRUE) {
      Error("Config::parseCLI",
            "--useEBWeight with --scaleRatesByPS is not valid. When using EB wieghts, all PS are supplied by the user. You can then set everything to PS=1.");
      Error("Config::parseCLI",
            "(this is the default, if no PS XML is supplied). The --scaleRatesByPS option should only be used to undo online prescales for chains.");
      abort();
    }

    setFloat(kBasicEventWeight, basicWeight, "BasicEventWeight");
    if (!isZero(predictionLumi)) {
      // Allow either notation here. Either explicit or in multiples of 10^30
      if (predictionLumi < 1e20) predictionLumi *= 1e30;
      setFloat(kPredictionLumi, predictionLumi, "PredictionLumi"); // The lumi we're currently predicting for (may be
                                                                    // set in XML)
    }
    if (!isZero(runLumi)) {
      if (runLumi < 1e20) runLumi *= 1e30;
      setFloat(kRunLumi, runLumi, "RunLumi"); // The lumi of the EB run online
    }
#ifdef MTHREAD
    set(kNThread, nThread, "NumberThreads");
#else
    set(kNThread, 1, "NumberThreads");
#endif
    set(kSlowThreshold, slowThreshold, "SlowThreshold");
    set(kSlowEvThreshold, slowEventThreshold, "SlowEventThreshold");
    set(kDoEBWeighting, doEBWeighting, "DoEBWeighting");
    set(kWriteEBWeightXML, writeEBWeightXML, "WriteEBWeightXML");
    set(kDefaultLBLength, defaultLBLength, "DefaultLBLength");
    set(kDebug, debug, "Debug");
    set(kCleanAll, cleanAll, "CleanAll"); //Specifies if the output directory is to be regenerated
    set(kRatesForcePass, ratesForcePass, "RatesForceAllChainsToPassRaw");
    set(kDoUniqueRates, doUniqueRates, "DoUniqueRates");
    set(kDoCPS, doCPS, "DoCPS");
    set(kDoGroupOverlap, doGroupOverlap, "DoGroupOverlap");
    set(kDoAllOverlap, doAllOverlap, "DoAllOverlap");
    set(kHistBins, histogramBins, "HistogramBins");
    set(kRatesOverlapWarning, ratesOverlapWarning, "RatesOverlapWarning");
    set(kRatesScaleByPS, ratesScaleByPS, "RatesScaleByPS");
    set(kExtrapolate8To13, extraplolate8To13, "Extrapolate8To13TeV");
    set(kExtrapolate13To5, extraplolate13To5, "Extrapolate13To5TeV");
    set(kDoNotWriteMetadata, doNotWriteMetadata, "DoNotWriteMetadata");
    set(kNoMsgSuppression, noMsgSup, "NoMessageSuppression");
    // If set to true, RD0/1/2/3 triggers will only be allowed to fire if the event was taken online with a random
    // trigger
    // This prevents the biased triggers inflating the rate of chains which are supposed to run off of the unbiased
    // triggers
    set(kMatchL1RandomToOnline, matchL1RandomToOnline, "MatchL1RandomToOnline");
    set(kNoOnlineDeadtimeCorrection, noOnlineDTCorrection, "NoOnlineDeadtimeCorrection");
    set(kUpgradeScenario, upgradeScenario, "UpgradeScenario");
    set(kNoUpgradePileupScaling, noUpgradePileupScaling, "NoUpgradePileupScaling");
    set(kNoUpgradeBunchScaling, noUpgradeBunchScaling, "NoUpgradeBunchScaling");
    set(kDoUpgradeRatesScan, doUpgradeRatesScan, "DoUpgradeRatesScan");
    set(kMessageWait, messageWait, "MessageWait");
    setFloat(kEventElapsed, 0., "EventElasped", kUnlocked);
    setFloat(kEventStartTime, 0., "EventStart", kUnlocked);
    set(kPatternsInvert, patternsInvert, "PatternsInvert");
    set(kIsCPUPrediction, isCPUPrediction, "IsCPUPrediction");
    set(kIgnoreRerun, 0, "IgnoreRerun", kUnlocked);
    set(kJIRA, jira, "JIRA");
    set(kPatternsExactMatch, patternsExactMatch, "PatternsExactMatch");
    set(kIgnoreNonPhysBunchGroups, ignoreNonPhysBunchGroups, "IgnoreNonPhyBunchgroups");
    set(kNoLBRescaling, noLBRescaling, "NoLBRescaling");
    setFloat(kTargetPeakMuAverage, targetMu, "TargetMu", kUnlocked);
    setFloat(kExpoRateScaleModifierL1, expoRateScaleModifierL1, "ExpoRateScaleModifierL1");
    setFloat(kExpoRateScaleModifierHLT, expoRateScaleModifierHLT, "ExpoRateScaleModifierHLT");
    set(kUpgradeMergeTOBOverlap, upgradeMergeTOBOverlap, "UpgradeMergeTOBOverlap");
    set(kDoExponentialMu, doExponentialMu, "DoExponentialMu");
    set(kInvertHighMuRunVeto, invertHighMuRunVeto, "InvertHighMuRunVeto");
    set(kUseOnlyTheseBCIDs, useOnlyTheseBCIDs, "UseOnlyTheseBCIDs");
    set(kIgnoreGRL, ignoreGRL, "IgnoreGRL");
    set(kIgnorePSGreaterThanOne, ignorePSGreaterThanOne, "IgnorePSGreaterThanOne");

    std::stringstream multiRunss(multiRun); // Comma separated
    std::string tempStr;
    std::vector<Int_t> multRunInts;
    while (std::getline(multiRunss, tempStr, ',')) multRunInts.push_back(stringToInt(tempStr));
    set(kMultiRun, multRunInts, "MultiRun");


    set(kMaxMultiSeed, maxMultiSeed, "MaxMultiSeed");
    set(kMaxMultiSeedForGroup, maxMultiSeedForGroup, "MaxMultiSeedForGroup");
    if (runNumber != 0) set(kRunNumber, runNumber, "RunNumber");

    // Load files to be accessed
    // File Names
    if (ROSXML != m_blankString) set(kROSXMLName, ROSXML, "ROSXML");
    if (prescaleXML1 != m_blankString) set(kPrescaleXMLName1, prescaleXML1, "PrescaleXML1");
    if (prescaleXML2 != m_blankString) set(kPrescaleXMLName2, prescaleXML2, "PrescaleXML2");

    // Choose sensible default PS behaviour. If XML supplied, default to -1, else to +1.
    if (prescaleXML1 != m_blankString || prescaleXML2 != m_blankString) {
      if (rateFallbackPrescaleL1 == FLT_MIN) rateFallbackPrescaleL1 = -1.;
      if (rateFallbackPrescaleHLT == FLT_MIN) rateFallbackPrescaleHLT = -1.;
    } else {
      if (rateFallbackPrescaleL1 == FLT_MIN) rateFallbackPrescaleL1 = 1.;
      if (rateFallbackPrescaleHLT == FLT_MIN) rateFallbackPrescaleHLT = 1.;
    }
    if (isZero(rateFallbackPrescaleL1 - 1.) == kFALSE) {
      Warning("Config::parseCLI", "PLEASE NOTE: Setting fall-back prescale value for unknown L1 items to %f.",
              rateFallbackPrescaleL1);
    }
    if (isZero(rateFallbackPrescaleHLT - 1.) == kFALSE) {
      Warning("Config::parseCLI", "PLEASE NOTE: Setting fall-back prescale value for unknown HLT items to %f.",
              rateFallbackPrescaleHLT);
    }
    setFloat(kRateFallbackPrescaleL1, rateFallbackPrescaleL1, "RateFallbackPrescaleL1");
    setFloat(kRateFallbackPrescaleHLT, rateFallbackPrescaleHLT, "RateFallbackPrescaleHLT");

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
    const Char_t* env = std::getenv("ROOTCOREBIN");
    if (env != NULL) {
      set(kIsRootCore, 1, "IsRootCore");
      set(kDataDir, std::string(env) + std::string("/data/TrigCostRootAnalysis/"), "DataDir");
      if (getIsSet(kROSXMLName)) set(kROSXMLPath, getStr(kDataDir) + getStr(kROSXMLName));
      if (getIsSet(kPrescaleXMLName1)) {
        if (getStr(kPrescaleXMLName1).substr(0, 1) == "/") set(kPrescaleXMLPath1, getStr(kPrescaleXMLName1));
        else set(kPrescaleXMLPath1, getStr(kDataDir) + getStr(kPrescaleXMLName1));
      }
      if (getIsSet(kPrescaleXMLName2)) {
        if (getStr(kPrescaleXMLName2).substr(0, 1) == "/") set(kPrescaleXMLPath2, getStr(kPrescaleXMLName2));
        else set(kPrescaleXMLPath2, getStr(kDataDir) + getStr(kPrescaleXMLName2));
      }
      set(kAutoMonXMLPath, getStr(kDataDir) + AutoMonXMLName, "AutoMonXMLPath");
    } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
      set(kIsRootCore, 0, "IsRootCore");
      if (getIsSet(kROSXMLName)) {
        std::string locAthena = PathResolverFindDataFile(getStr(kROSXMLName));
        if (locAthena ==
            m_blankString) Error("Config::parseCLI", "Athena cannot find ROS mapping file %s", getStr(
            kROSXMLName).c_str());

        else {
          set(kROSXMLPath, locAthena, "ROSXMLPath");
          Info("Config::parseCLI", "Athena has found the file: %s", getStr(kROSXMLPath).c_str());
        }
      }
      //
      if (getIsSet(kPrescaleXMLName1)) {
        std::string locAthena = PathResolverFindDataFile(getStr(kPrescaleXMLName1));
        if (locAthena ==
        m_blankString) Error("Config::parseCLI", "Athena cannot find prescale XML file #1 %s", getStr(
            kPrescaleXMLName1).c_str());
        else {
          set(kPrescaleXMLPath1, locAthena, "PrescaleXMLPath1");
          Info("Config::parseCLI", "Athena has found the file: %s", getStr(kPrescaleXMLPath1).c_str());
        }
      }
      if (getIsSet(kPrescaleXMLName2)) {
        std::string locAthena = PathResolverFindDataFile(getStr(kPrescaleXMLName2));
        if (locAthena == m_blankString) Error("Config::parseCLI", "Athena cannot find prescale XML file #2 %s", getStr(kPrescaleXMLName2).c_str());
        else {
          set(kPrescaleXMLPath2, locAthena, "PrescaleXMLPath2");
          Info("Config::parseCLI", "Athena has found the file: %s", getStr(kPrescaleXMLPath2).c_str());
        }
      }
      std::string locAthena = PathResolverFindDataFile(AutoMonXMLName);
      if (locAthena == m_blankString) Error("Config::parseCLI", "Athena cannot find AutoMonConfig XML file #1 %s", AutoMonXMLName.c_str());
      else {
        set(kAutoMonXMLPath, locAthena, "AutoMonXMLPath");
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
    if (binMin != FLT_MIN && binMax != FLT_MIN) {
      setFloat(kHistBinMin, binMin, "HistogramBinMin");
      setFloat(kHistBinMax, binMax, "HistogramBinMax");
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
    set(kVersionString, version, "Version");

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

    std::map<ConfKey_t, Int_t>::iterator itInt;
    std::map<ConfKey_t, Long64_t>::iterator itLong;
    std::map<ConfKey_t, Float_t>::iterator itFloat;
    std::map<ConfKey_t, std::string>::iterator itStr;
    std::map<ConfKey_t, std::vector<std::string> > ::iterator itStrVec;
    std::map<ConfKey_t, std::vector<Int_t> > ::iterator itIntVec;

    Int_t i = -1;
    for (itInt = m_settingsInt.begin(); itInt != m_settingsInt.end(); ++itInt) {
      const std::string name = m_settingsName[itInt->first];
      Info("Config::dump", " [INT:%i]\t[LOCK:%i]\t%s = %i", ++i, (Int_t) getIsLocked(itInt->first), name.c_str(), itInt->second);
    }
    i = -1;
    for (itLong = m_settingsLong.begin(); itLong != m_settingsLong.end(); ++itLong) {
      const std::string name = m_settingsName[itLong->first];
      Info("Config::dump", " [LONG:%i]\t[LOCK:%i]\t%s = %lli", ++i, (Int_t) getIsLocked(itLong->first), name.c_str(), itLong->second);
    }
    i = -1;
    for (itFloat = m_settingsFloat.begin(); itFloat != m_settingsFloat.end(); ++itFloat) {
      const std::string name = m_settingsName[itFloat->first];
      // Big or small?
      if (itFloat->second > 1e4) { //BIG
        Info("Config::dump", " [FLOAT:%i]\t[LOCK:%i]\t%s = %e", ++i, (Int_t) getIsLocked(itFloat->first), name.c_str(), itFloat->second);
      } else { // small
        Info("Config::dump", " [FLOAT:%i]\t[LOCK:%i]\t%s = %f", ++i, (Int_t) getIsLocked(itFloat->first), name.c_str(), itFloat->second);
      }
    }
    i = -1;
    for (itStr = m_settingsStr.begin(); itStr != m_settingsStr.end(); ++itStr) {
      const std::string name = m_settingsName[itStr->first];
      Info("Config::dump", " [STRING:%i]\t[LOCK:%i]\t%s = \"%s\"", ++i, (Int_t) getIsLocked(itStr->first), name.c_str(), itStr->second.c_str());
    }
    i = -1;
    for (itStrVec = m_settingsVecStr.begin(); itStrVec != m_settingsVecStr.end(); ++itStrVec) {
      const std::string name = m_settingsName[itStrVec->first];
      Info("Config::dump", " [VEC<STR>:%i]\t[LOCK:%i]\t%s, size %u", ++i, (Int_t) getIsLocked(itStrVec->first), name.c_str(), (UInt_t) itStrVec->second.size());
      for (UInt_t v = 0; v < itStrVec->second.size(); ++v) {
        Info("Config::dump", "\t\t\t\t  [%u] \"%s\"", v, itStrVec->second.at(v).c_str());
      }
    }
    i = -1;
    for (itIntVec = m_settingsVecInt.begin(); itIntVec != m_settingsVecInt.end(); ++itIntVec) {
      const std::string name = m_settingsName[itIntVec->first];
      Info("Config::dump", " [VEC<INT>:%i]\t[LOCK:%i]\t%s, size %u", ++i, (Int_t) getIsLocked(itIntVec->first), name.c_str(), (UInt_t) itIntVec->second.size());
      for (UInt_t v = 0; v < itIntVec->second.size(); ++v) {
        Info("Config::dump", "\t\t\t\t  [%u] \"%i\"", v, itIntVec->second.at(v));
      }
    }
  }

  void Config::dumpToMeta(std::ofstream& fout, JsonExport& json) {
    std::map<ConfKey_t, Int_t>::iterator itInt;
    std::map<ConfKey_t, Long64_t>::iterator itLong;
    std::map<ConfKey_t, Float_t>::iterator itFloat;
    std::map<ConfKey_t, std::string>::iterator itStr;
    std::map<ConfKey_t, std::vector<std::string> > ::iterator itStrVec;
    std::map<ConfKey_t, std::vector<Int_t> > ::iterator itIntVec;

    for (itInt = m_settingsInt.begin(); itInt != m_settingsInt.end(); ++itInt) {
      const std::string name = m_settingsName[itInt->first];
      json.addLeafCustom(fout, name, intToString(itInt->second));
    }
    for (itLong = m_settingsLong.begin(); itLong != m_settingsLong.end(); ++itLong) {
      const std::string name = m_settingsName[itLong->first];
      json.addLeafCustom(fout, name, intToString(itLong->second));
    }
    for (itFloat = m_settingsFloat.begin(); itFloat != m_settingsFloat.end(); ++itFloat) {
      json.addLeafCustom(fout, m_settingsName[itFloat->first], floatToString(itFloat->second));
    }
    for (itStr = m_settingsStr.begin(); itStr != m_settingsStr.end(); ++itStr) {
      if (m_settingsName[itStr->first] == itStr->second) continue;
      json.addLeafCustom(fout, m_settingsName[itStr->first], itStr->second);
    }
    for (itStrVec = m_settingsVecStr.begin(); itStrVec != m_settingsVecStr.end(); ++itStrVec) {
      for (UInt_t v = 0; v < itStrVec->second.size(); ++v) {
        json.addLeafCustom(fout, std::string(m_settingsName[itStrVec->first] + intToString(v)), itStrVec->second.at(v).c_str());
      }
    }
    for (itIntVec = m_settingsVecInt.begin(); itIntVec != m_settingsVecInt.end(); ++itIntVec) {
      for (UInt_t v = 0; v < itIntVec->second.size(); ++v) {
        json.addLeafCustom(fout, std::string(m_settingsName[itIntVec->first] + intToString(v)), intToString(itIntVec->second.at(v)));
      }
    }
  }

  void Config::messageSuppressionReport() {
    for (std::map<ConfKey_t, Int_t>::iterator itInt = m_settingsInt.begin(); itInt != m_settingsInt.end(); ++itInt) {
      ConfKey_t key = itInt->first;
      if (m_settingsMaxCalls[key] == 0) {
        continue; // If no max message calls
      }
      if (m_settingsInt[key] < m_settingsMaxCalls[key]) {
        continue; // If no overflow suppresion messages
      }
      const std::string name = m_settingsName[key];
      UInt_t suppressed = m_settingsInt[key] - m_settingsMaxCalls[key];
      Warning("Config::messageSuppressionReport", "There were %i additional instances of message '%s' which were suppressed.", suppressed, name.c_str());
    }
  }

  /**
   * Retrieve string configuration for given enum key.
   * @param key enum key for this config.
   * @returns Requested configuration string or blank string if key not found.
   */
  const std::string& Config::getStr(ConfKey_t key) {
    if (m_settingsStr.count(key) == 1) {
      return m_settingsStr[key];// Return the requested string
    } else if (m_settingsVecStr.count(key) == 1) {
      static std::string scratch; // singleton so this is OK
      std::stringstream ss;
      for (auto entry : m_settingsVecStr[key]) {
        ss << entry << ", ";
      }
      scratch = ss.str();
      return scratch;
    } else {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::getStr", "Unknown string-key %i. (name, if any:%s)", key, name.c_str());
    }
    return m_blankString;
  }

  /**
   * Retrieve string name of config enum
   * @param key enum key for this config.
   * @returns Requested configuration enum's na,me.
   */
  const std::string& Config::getName(ConfKey_t key) {
    if (m_settingsName.count(key) == 0) {
      Error("Config::getName", "Unknown key %i", key);
      return m_blankString;
    }
    // Return the requested string
    return m_settingsName[key];
  }

  /**
   * Retrieve integer configuration for given enum key.
   * @param key enum key for this config.
   * @return Requested configuration integer or -1 if key not found.
   */
  Int_t Config::getInt(ConfKey_t key) {
    if (m_settingsInt.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::getInt", "Unknown key %i %s", key, name.c_str());
      return -1;
    }
    return m_settingsInt[key];
  }

  /**
   * Increase by one the integer or long for given enum key.
   * @param key enum key for this config.
   */
  void Config::increment(ConfKey_t key) {
    if (getIsLocked(key, kTRUE) == kTRUE) return;

    if (m_settingsInt.count(key) == 1) {
      ++m_settingsInt[key];
    } else if (m_settingsLong.count(key) == 1) {
      ++m_settingsLong[key];
    } else {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::increment", "Unknown key %i %s", key, name.c_str());
      return;
    }
  }

  /**
   * Decrease by one the integer for given enum key.
   * @param key enum key for this config.
   */
  void Config::decrement(ConfKey_t key) {
    if (m_settingsInt.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::decrement", "Unknown key %i %s", key, name.c_str());
      return;
    }
    if (getIsLocked(key, kTRUE) == kTRUE) return;

    --m_settingsInt[key];
  }

  /**
   * Retrieve float configuration for given enum key.
   * @param key enum key for this config.
   * @return Requested configuration integer or -1 if key not found.
   */
  Float_t Config::getFloat(ConfKey_t key) {
    if (m_settingsFloat.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::getFloat", "Unknown key %i %s", key, name.c_str());
      return -1.;
    }
    return m_settingsFloat[key];
  }

  /**
   * Retrieve long int configuration for given enum key.
   * @param key enum key for this config.
   * @return Requested configuration integer or -1 if key not found.
   */
  Long64_t Config::getLong(ConfKey_t key) {
    if (m_settingsLong.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::getLong", "Unknown key %i %s", key, name.c_str());
      return -1;
    }
    return m_settingsLong[key];
  }

  /**
   * Retrieve size of vector storage for given enum key.
   * @param key enum key for this config.
   * @return Size of vector for given key, or INT_MAX if key not found.
   */
  UInt_t Config::getVecSize(ConfKey_t key) {
    if (m_settingsVecStr.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::getVecSize", "Unknown key %i %s", key, name.c_str());
      return INT_MAX;
    }
    return m_settingsVecStr[key].size();
  }

  /**
   * Retrieve vector configuration for given key.
   * @param key enum key for this config.
   * @param location Location within vector to return.
   * @return String at vector location for requested key or blank string if key not found or out of range.
   */
  const std::string& Config::getVecEntry(ConfKey_t key, UInt_t location) {
    if (m_settingsVecStr.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::getVecEntry", "Unknown key %i %s", key, name.c_str());
      return m_blankString;
    }
    if (location >= getVecSize(key)) {
      Error("Config::getVecEntry", "Location %i is out of range", location);
      return m_blankString;
    }
    return m_settingsVecStr[key].at(location);
  }

  /**
   * If a settings vector is registered with a given key, and it is unlocked, then append the given string.
   * @param key enum key for this config.
   * @param toAdd Const reference to string to add to the settings vector.
   * @return true on success.
   */
  Bool_t Config::addVecEntry(ConfKey_t key, const std::string& toAdd) {
    if (m_settingsVecStr.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::addVecEntry", "Unknown key %i %s", key, name.c_str());
      return kFALSE;
    }
    if (getIsLocked(key, kTRUE) == kTRUE) return kFALSE;

    m_settingsVecStr[key].push_back(toAdd);
    return kTRUE;
  }

  /**
   * If a settings vector is registered with a given key, and it is unlocked, then remove the given entry.
   * Note the test is for an exact match, whereas getVecMatches() checks for a fuzzy match.
   * @param key enum key for this config.
   * @param toRemove Const reference to string to remove from the settings vector.
   * @return true if one or more entries were removed.
   */
  Bool_t Config::removeVecEntry(ConfKey_t key, const std::string& toRemove) {
    Bool_t entriesRemoved = kFALSE;

    if (m_settingsVecStr.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::removeVecEntry", "Unknown key %i %s", key, name.c_str());
      return kFALSE;
    }
    if (getIsLocked(key, kTRUE) == kTRUE) return kFALSE;

    std::vector<std::string>::iterator it = m_settingsVecStr[key].begin();
    while (it != m_settingsVecStr[key].end()) {
      if (toRemove.compare((*it)) == 0) { // Compare == 0 for exact match only
        it = m_settingsVecStr[key].erase(it);
        entriesRemoved = kTRUE;
      } else {
        ++it;
      }
    }
    return entriesRemoved;
  }

  /**
   * Remove all entries in a config vector
   * @param key enum key for this config.
   * @return true if one or more entries were removed.
   */
  Bool_t Config::clearVec(ConfKey_t key) {
    Bool_t entriesRemoved = kFALSE;

    if (m_settingsVecStr.count(key) == 0 && m_settingsVecInt.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::clearVec", "Unknown key %i %s", key, name.c_str());
      return kFALSE;
    }
    if (getIsLocked(key, kTRUE) == kTRUE) return kFALSE;

    if (m_settingsVecStr[key].size()) entriesRemoved = kTRUE;
    m_settingsVecStr[key].clear();
    if (m_settingsVecInt[key].size()) entriesRemoved = kTRUE;
    m_settingsVecInt[key].clear();
    return entriesRemoved;
  }

  /**
   * Check if a vector of strings configuration contains a match to the given string.
   * Note: the pattern may match a sub-string, it does not need exactly equal one vector entry.
   * To be clear: the stored value must be a sub-string of entry
   * This behaviour can be changed based on the stored kPatternsExactMatch config
   * @param key enum key for this config.
   * @param entry The string to match.
   * @return kTRUE if the patter.
   */
  Bool_t Config::getVecMatches(ConfKey_t key, const std::string& entry) {
    if (m_settingsVecStr.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::getVecMatches", "Unknown key %i %s", key, name.c_str());
      return kFALSE;
    }

    for (UInt_t i = 0; i < m_settingsVecStr[key].size(); ++i) {
      if (m_settingsInt[kPatternsExactMatch] == 1) {
        // This chain is exactly the correct name
        if (entry == m_settingsVecStr[key].at(i)) return kTRUE;
      } else {
        // This chain contains the correct string name
        if (entry.find(m_settingsVecStr[key].at(i)) != std::string::npos) return kTRUE;
      }
    }
    return kFALSE;
  }

  /**
   * Check if a vector of ints configuration contains a match to the given int.
   * @param key enum key for this config.
   * @param entry The int to match.
   * @return kTRUE if matched.
   */
  Bool_t Config::getIntVecMatches(ConfKey_t key, const Int_t entry) {
    if (m_settingsVecInt.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::getIntVecMatches", "Unknown key %i %s", key, name.c_str());
      return kFALSE;
    }
    if (std::find(m_settingsVecInt[key].begin(), m_settingsVecInt[key].end(), entry) != m_settingsVecInt[key].end()) return kTRUE;

    return kFALSE;
  }

  /**
   * @param key enum key for this config.
   * @return the vector of ints.
   */
  const std::vector<Int_t>& Config::getIntVec(ConfKey_t key) {
    static std::vector<Int_t> empty;

    if (m_settingsVecInt.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::getIntVecMatches", "Unknown key %i %s", key, name.c_str());
      return empty;
    }
    return m_settingsVecInt[key];
  }

  /**
   * Get if there is a setting stored with the given key. Regardless of type.
   * @param key enum key for this config.
   * @return kTRUE if a setting is stored, with any type, for the given key.
   */
  Bool_t Config::getIsSet(ConfKey_t key) {
    if (m_settingsVecStr.count(key) == 1) return kTRUE;

    if (m_settingsVecInt.count(key) == 1) return kTRUE;

    if (m_settingsInt.count(key) == 1) return kTRUE;

    if (m_settingsLong.count(key) == 1) return kTRUE;

    if (m_settingsStr.count(key) == 1) return kTRUE;

    if (m_settingsFloat.count(key) == 1) return kTRUE;

    return kFALSE;
  }

  /**
   * Get if to display a message, this assumes that key has been preset to the maximum number of times the message
   *should be displayed before suppression.
   * @param key enum key for this config.
   * @return kTRUE if a setting is stored, with any type, for the given key.
   */
  Bool_t Config::getDisplayMsg(ConfKey_t key) {
    if (m_settingsInt.count(key) == 0) {
      std::string name;
      if (m_settingsName.count(key) == 1) name = m_settingsName[key];
      Error("Config::getVecEntry", "Unknown key %i %s", key, name.c_str());
      return kFALSE;
    }

    if (getInt(kNoMsgSuppression) == kTRUE) return kTRUE;

    m_settingsInt[key]++;
    if (m_settingsInt[key] == m_settingsMaxCalls[key]) { // Warn of last time
      Info("Config::getDisplayMsg", "The following message has reached its suppression limit of %i and will not be displayed any more.", m_settingsInt[key]);
    } else if (m_settingsInt[key] > m_settingsMaxCalls[key]) {
      return kFALSE;
    }
    return kTRUE;
  }

  /**
   * Store or overwrite a string configuration with a enum key.
   * If no name is given, then it is assumed it is its name is its value
   * @param key enum key for this config.
   * @param value Const string value to store.
   */
  void Config::set(ConfKey_t key, const std::string value, const std::string name, LockStatus_t lock) {
    if (getIsLocked(key, kTRUE) == kTRUE) return;

    m_settingsStr[key] = value;
    m_settingsName[key] = name;
    if (name == m_blankString) m_settingsName[key] = value;
    m_settingsLock[key] = lock;
  }

  /**
   * Store or overwrite a integer configuration with a enum key. Can be given an optional name.
   * @param key enum key for this config.
   * @param name Optional name for this entry.
   * @param value Const integer value to store.
   */
  void Config::set(ConfKey_t key, const Int_t value, const std::string name, LockStatus_t lock) {
    if (getIsLocked(key, kTRUE) == kTRUE) return;

    m_settingsInt[key] = value;
    m_settingsMaxCalls[key] = 0;
    m_settingsName[key] = name;
    m_settingsLock[key] = lock;
    // This is a special flag, set for easy access.
    if (key == kDebug) {
      m_debug = (Bool_t) value;
    }
  }

  /**
   * Store or overwrite a integer configuration with a enum key. This versions is without any method to name the var.
   * @param key enum key for this config.
   * @param value Const integer value to store.
   */
  void Config::set(ConfKey_t key, const Int_t value, LockStatus_t lock) {
    if (getIsLocked(key, kTRUE) == kTRUE) return;

    m_settingsInt[key] = value;
    m_settingsMaxCalls[key] = 0;
    m_settingsLock[key] = lock;
    // This is a special flag, set for easy access.
    if (key == kDebug) {
      m_debug = (Bool_t) value;
    }
  }

  /**
   * Store or overwrite a integer configuration with a enum key. Can be given an optional name.
   * This version is specific for config entries which will be used to restrict the display of error messages
   * @param key enum key for this config.
   * @param name Optional name for this entry.
   * @param value Maximum number of itmes to display this error/warning/info message.
   */
  void Config::setDisplayMsg(ConfKey_t key, const Int_t value, const std::string name) {
    m_settingsInt[key] = 0;
    m_settingsMaxCalls[key] = value;
    m_settingsName[key] = name;
    m_settingsLock[key] = kUnlocked; // Must not be locked as needs to be increased
  }

  /**
   * Store or overwrite a vector of strings configuration with a enum key.
   * @param key enum key for this config.
   * @param name Optional name for this entry.
   * @param value Const vector of strings to store.
   */
  void Config::set(ConfKey_t key, const std::vector< std::string > value, const std::string name, LockStatus_t lock) {
    if (getIsLocked(key, kTRUE) == kTRUE) return;

    m_settingsVecStr[key] = value;
    m_settingsName[key] = name;
    m_settingsLock[key] = lock;
  }

  /**
   * Store or overwrite a vector of ints configuration with a enum key.
   * @param key enum key for this config.
   * @param name Optional name for this entry.
   * @param value Const vector of ints to store.
   */
  void Config::set(ConfKey_t key, const std::vector< Int_t > value, const std::string name, LockStatus_t lock) {
    if (getIsLocked(key, kTRUE) == kTRUE) return;

    m_settingsVecInt[key] = value;
    m_settingsName[key] = name;
    m_settingsLock[key] = lock;
  }

  /**
   * Store or overwrite a long integer configuration with a enum key.
   * Note - this setter has a unique name to prevent ambiguity with the Int_t setter
   * @param key enum key for this config.
   * @param name Optional name for this entry.
   * @param value Long64_t to store.
   */
  void Config::setLong(ConfKey_t key, const Long64_t value, const std::string name, LockStatus_t lock) {
    if (getIsLocked(key, kTRUE) == kTRUE) return;

    m_settingsLong[key] = value;
    m_settingsName[key] = name;
    m_settingsLock[key] = lock;
  }

  /**
   * Store or overwrite a float configuration with a enum key.
   * Note - this setter has a unique name to prevent ambiguity with the Int_t setter
   * @param key enum key for this config.
   * @param name Optional name for this entry.
   * @param value Float to store.
   */
  void Config::setFloat(ConfKey_t key, const Float_t value, const std::string name, LockStatus_t lock) {
    if (getIsLocked(key, kTRUE) == kTRUE) return;

    m_settingsFloat[key] = value;
    m_settingsName[key] = name;
    m_settingsLock[key] = lock;
  }

  /**
   * Check if this key has been used before, and if so then whether the lock was set to true. Regardles of type.
   * @param key enum key for this config.
   * @param printErrorMsg Set to kTRUE if the intention was to modify this record, will display error message to user.
   * @return kTRUE if the key exists and the lock was set.
   */
  Bool_t Config::getIsLocked(ConfKey_t key, Bool_t printErrorMsg) {
    if (m_settingsLock.count(key) == 1) {
      if (m_settingsLock[key] == kLocked) {
        std::string name;
        if (m_settingsName.count(key) == 1) name = m_settingsName[key];
        if (printErrorMsg == kTRUE) Error("Config::getIsLocked", "The configuration key %i (name, if any:%s) is LOCKED and cannot be modified.", key, name.c_str());
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
