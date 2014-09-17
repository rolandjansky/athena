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
#include <stdio.h>
#include <stdlib.h>
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
    static Int_t _monitorAllChainSeqAlgs = kFALSE;
    static Int_t _monitorROS = kFALSE;
    static Int_t _monitorL1 = kFALSE;
    static Int_t _monitorFullEvent = kFALSE;
    static Int_t _monitorGlobals = kFALSE;
    static Int_t _monitorROI = kFALSE;
    static Int_t _monitorRates = kFALSE;
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
    // Misc
    static Int_t _outputTagFromAthena = kFALSE;
    static Int_t _cleanAll = kFALSE;
    static Int_t _doEBWeighting = kFALSE;
    static Int_t _debug = kFALSE;
    static Int_t _ratesForcePass = kFALSE;
    static Int_t _doUniqueRates = kFALSE;
    static Int_t _showVersion = kFALSE;

    // User options
    std::vector< std::string > _inputFiles;
    std::vector< std::string > _patternsMonitor;
    std::vector< std::string > _patternsOutput;
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
    std::string _outDirectory = "costMonitoring_%t_%r";
    std::string _outRootDirectory = ".";
    std::string _outTag = "LOCAL";
    std::string _userDetails = "";
    std::string _menuXML = "";//"HLTconfig_Physics_pp_v5_19.2.0.xml";
    std::string _prescaleXML = "";//"cool_208354_366_366.xml"; // This is an old XML for test purposes
    std::string _ROSXML = "rob-ros-robin-2012.xml";
    std::string _version = "TrigCostRootAnalysis-00-05-09";
    Int_t _lbBegin = INT_MIN;
    Int_t _lbEnd = INT_MAX;
    UInt_t _nEvents = INT_MAX;
    UInt_t _nSkip = 0;
    UInt_t _nLbFullSummary = 1;
    UInt_t _defaultLBLength = 60;
    UInt_t _slowThreshold = 500;
    UInt_t _histogramBins = 100;
    UInt_t _maxNumberFullEvents = 10;
    UInt_t _ratesOverlapWarning = 80;
    UInt_t _maxMultiSeed = 1;
    
    // Parse CLI
    Int_t _status = 0;
    while (kTRUE) {
      static struct option _longOptions[] = {
        {"help",                   no_argument,       0,                        0},
        {"cleanAll",               no_argument,       &_cleanAll,               1},
        {"debug",                  no_argument,       &_debug,                  1},
        {"doL2",                   no_argument,       &_doL2,                   1},
        {"doEF",                   no_argument,       &_doEF,                   1},
        {"doL2EF",                 no_argument,       &_doL2EF,                 1},
        {"doHLT",                  no_argument,       &_doHLT,                  1},
        {"doOutputCanvas",         no_argument,       &_doOutputCanvas,         1},
        {"doOutputHist",           no_argument,       &_doOutputHist,           1},
        {"doOutputPNG",            no_argument,       &_doOutputPng,            1},
        {"doOutputPDF",            no_argument,       &_doOutputPdf,            1},
        {"doOutputCSV",            no_argument,       &_doOutputCsv,            1},
        {"doOutputXML",            no_argument,       &_doOutputXML,            1},
        {"doOutputRatesGraph",     no_argument,       &_doOutputRatesGraph,     1},
        {"doOutputMenus",          no_argument,       &_doOutputMenus,          1},
        {"outputTagFromAthena",    no_argument,       &_outputTagFromAthena,    1},
        {"monitorAll",             no_argument,       &_monitorAll,             1},
        {"monitorChains",          no_argument,       &_monitorChains,          1},
        {"monitorChainAlgs",       no_argument,       &_monitorChainAlgs,       1},
        {"monitorSequences",       no_argument,       &_monitorSequences,       1},
        {"monitorSequenceAlgs",    no_argument,       &_monitorSequenceAlgs,    1},
        {"monitorAlgs",            no_argument,       &_monitorAlgs,            1},
        {"monitorAllChainSeqAlgs", no_argument,       &_monitorAllChainSeqAlgs, 1},        
        {"monitorROS",             no_argument,       &_monitorROS,             1},
        {"monitorROI",             no_argument,       &_monitorROI,             1},
        {"monitorL1",              no_argument,       &_monitorL1,              1},
        {"monitorFullEvent",       no_argument,       &_monitorFullEvent,       1},
        {"monitorGlobals",         no_argument,       &_monitorGlobals,         1},
        {"monitorRates",           no_argument,       &_monitorRates,           1},
        {"useEBWeight",            no_argument,       &_doEBWeighting,          1},
        {"summaryPerHLTConfig",    no_argument,       &_summaryPerHLTConfig,    1},
        {"summaryPerLumiBlock",    no_argument,       &_summaryPerLumiBlock,    1},
        {"summaryAll",             no_argument,       &_summaryAll,             1},
        {"forceAllPass",           no_argument,       &_ratesForcePass,         1},
        {"doUniqueRates",          no_argument,       &_doUniqueRates,          1},
        {"version",                no_argument,       &_showVersion,            1},
        {"linkOutputDirectory",    no_argument,       &_linkOutputDirectory,    1},
        {"treeName",               required_argument, 0,                      't'},
        {"menuXML",                required_argument, 0,                      'm'},
        {"prescaleXML",            required_argument, 0,                      'M'},
        {"ROSXML",                 required_argument, 0,                      'R'},
        {"outputRootFile",         required_argument, 0,                      'r'},
        {"outputImageDirectory",   required_argument, 0,                      'i'},
        {"outputCSVDirectory",     required_argument, 0,                      'C'},
        {"outputXMLDirectory",     required_argument, 0,                      'x'},
        {"outputDirectory",        required_argument, 0,                      'o'},
        {"outputRootDirectory",    required_argument, 0,                      'O'},
        {"outputTag",              required_argument, 0,                      'T'},
        {"histogramBins",          required_argument, 0,                      'h'},
        {"prefixL2",               required_argument, 0,                      'l'},
        {"prefixEF",               required_argument, 0,                      'E'},
        {"prefixHLT",              required_argument, 0,                      'H'},
        {"prefixConfig",           required_argument, 0,                      'c'},
        {"lbBegin",                required_argument, 0,                      'b'},
        {"lbEnd",                  required_argument, 0,                      'e'},
        {"nEvents",                required_argument, 0,                      'n'},
        {"nSkip",                  required_argument, 0,                      's'},
        {"nFullEventSummaries",    required_argument, 0,                      'F'},
        {"slowThreshold",          required_argument, 0,                      'S'},
        {"defaultLBLength",        required_argument, 0,                      'd'},
        {"nLbFullSummary",         required_argument, 0,                      'L'},
        {"ratesOverlapWarning",    required_argument, 0,                      'w'},
        {"files",                  required_argument, 0,                      'f'},
        {"patternsMonitor",        required_argument, 0,                      'p'},
        {"patternsOutput",         required_argument, 0,                      'P'},
        {"userDetails",            required_argument, 0,                      'u'},
        {"maxMultiSeed",           required_argument, 0,                      'X'},
        {0, 0, 0, 0}
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
        if (std::string( _longOptions[_optionIndex].name ) == "help") {
          std::cout << "INFO: Trig Cost Root Analysis Program Usage:- " << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ HLT LEVELS TO PROCESS ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--doL2\t\t\t\t\t\tProcess just Level 2 cost data if present in ntuple." << std::endl;
          std::cout << "--doEF\t\t\t\t\t\tProcess just Event Filter cost data if present in ntuple." << std::endl;
          std::cout << "--doL2EF\t\t\t\t\tProcess Level 2 and Event Filter cost data if present in ntuple." << std::endl;
          std::cout << "--doHLT\t\t\t\t\t\t[Default] Process HLT (unified L2 and EF) cost data if present in ntuple." << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ TIME RANGES TO MONITOR  ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--summaryAll\t\t\t\t\tOutput a summary for all events processed." << std::endl;
          std::cout << "--summaryPerHLTConfig\t\t\t\t[Default] Output a summary for each set of trigger keys processed." << std::endl;
          std::cout << "--summaryPerLumiBlock\t\t\t\tOutput a summary for each for the first " << _nLbFullSummary << " lumi block(s) (see --nLbFullSummary)." << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ AREAS TO MONITOR ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--monitorAll\t\t\t\t\t[Default] Enable all the following monitors." << std::endl;
          std::cout << "--monitorChains\t\t\t\t\tPerform cost monitoring for HLT chains." << std::endl;
          std::cout << "--monitorChainAlgs\t\t\t\tPerform cost monitoring for Algorithms associated to HLT Chains." << std::endl;
          std::cout << "--monitorSequences\t\t\t\tPerform cost monitoring for HLT sequences." << std::endl;
          std::cout << "--monitorSequenceAlgs\t\t\t\tPerform cost monitoring for Algorithms associated to HLT sequences." << std::endl;
          std::cout << "--monitorAlgs\t\t\t\t\tPerform cost monitoring for individual HLT algorithms." << std::endl;
          std::cout << "--monitorAllChainSeqAlg\t\t\t\tEnables all of monitorChains, monitorChainAlgs, monitorSequences, monitorSequenceAlgs and monitorAlgs." << std::endl;
          std::cout << "--monitorROS\t\t\t\t\tPerform cost monitoring for data flow and time in the Read Out System and enables ROS data association with algs." << std::endl;
          std::cout << "--monitorROI\t\t\t\t\tPerform cost monitoring for Regions of Interest in the trigger." << std::endl;
          std::cout << "--monitorL1\t\t\t\t\t(Deprecated) Assess cost monitoring impact on HLT from L1 menu and prescales." << std::endl;
          std::cout << "--monitorGlobals\t\t\t\tRecord global monitoring quantities for the whole HLT per lumi block." << std::endl;
          std::cout << "--monitorFullEvent\t\t\t\tFor select events, save detailed record of algorithm execution." << std::endl;
          std::cout << "--monitorRates\t\t\t\t\tPerform rate monitoring on chains and overlaps with other chains in the same group." << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ OUTPUT TO GENERATE ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--doOutputCanvas\t\t\t\tSave results as decorated canvases in output ROOT file." << std::endl;
          std::cout << "--doOutputHist\t\t\t\t\t[Default] Save results as raw histograms in output ROOT file." << std::endl;
          std::cout << "--histogramBins "<< _histogramBins <<"\t\t\t\tNumber of bins to use per histogram." << std::endl;
          std::cout << "--doOutputPNG\t\t\t\t\tSave results as PNG images in folder structure (warning, best used with --patternsOutput)." << std::endl;
          std::cout << "--doOutputPDF\t\t\t\t\tSave results as PDF images in folder structure (warning, best used with --patternsOutput)." << std::endl;
          std::cout << "--doOutputCSV\t\t\t\t\t[Default] Save results as CSV table summaries." << std::endl;
          std::cout << "--doOutputRatesGraph\t\t\t\t[Default] Save rates chain-graphs in JSON format." << std::endl;
          std::cout << "--doOutputRatesXML\t\t\t\t[Default] Save rates XML files for the Rulebook etc." << std::endl;
          std::cout << "--doOutputMenus\t\t\t\t\t[Default] Save menu configurations in JSON format." << std::endl;
          std::cout << "--linkOutputDirectory\t\t\t\tSoft link the output directory to 'costMon' in the run directory for automated scripts to use the data."<< std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ RUN CONFIGURATION ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--useEBWeight\t\t\t\t\tCalculate and use per-event Enhanced Bias weighting factor." << std::endl;
          std::cout << "--lbBegin INT_MIN\t\t\t\tLowest value luminosity block from input to use." << std::endl;
          std::cout << "--lbEnd INT_MAX\t\t\t\t\tHighest value luminosity block from input to use." << std::endl;
          std::cout << "--nEvents INT_MAX\t\t\t\tNumber of input events from all files to run over." << std::endl;
          std::cout << "--nSkip 0\t\t\t\t\tNumber of events to skip." << std::endl;
          std::cout << "--slowThreshold "<< _slowThreshold <<"\t\t\t\tTime in milliseconds. Execution times greater than this are flagged as slow." << std::endl;
          std::cout << "--nLbFullSummary " << _nLbFullSummary << "\t\t\t\tNumber of luminosity blocks, starting from lbBegin, to produce a full summary for." << std::endl;
          std::cout << "--nFullEventSummaries " << _maxNumberFullEvents << "\t\t\tMax number of events (picked at random) to save full executions records for." << std::endl;
          std::cout << "--defaultLBLength " << _defaultLBLength << "\t\t\t\tDefault lumi block length in seconds. Used as an approximate fallback for inputs without LB length data." << std::endl;
          std::cout << "--patternsMonitor patt1 patt2 ...\t\tPatterns to match in names when running. Regex currently NOT supported. Partial matched allowed. Only entries which match will be analysed." << std::endl;
          std::cout << "--patternsOutput patt1 patt2 ...\t\tPatterns to match in names when saving results. Regex currently NOT supported. Partial matched allowed. Only entries which match will be included in the output." << std::endl;
          std::cout << "--debug\t\t\t\t\t\tEnable debug output." << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ TRIGGER RATES CONFIGURATION ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--menuXML \"" << _menuXML << "\"\t\t\t\t\tMenu XML file from which to read custom prescales for rates calculations (place in /data or current dir for Athena use)." << std::endl;
          std::cout << "--prescaleXML \"" << _prescaleXML << "\"\t\t\t\tPrescale XML file from which to read custom prescales for rates calculation (place in /data or current dir for Athena use)." << std::endl;      
          std::cout << "--forceAllPass\t\t\t\t\tForce all L1 and HLT chains to pass-raw in every event. Use to isolate the effect of prescales." << std::endl;
          std::cout << "--doUniqueRates\t\t\t\t\tCalculate unique rates for chains. Warning, this is slow." << std::endl;
          std::cout << "--ratesOverlapWarning "<<_ratesOverlapWarning<<"%\t\t\tValue in percent (0-100) above which to warn about chain overlaps within rates groups." << std::endl;
          std::cout << "--maxMultiSeed "<< _maxMultiSeed <<"\t\t\t\tMaximum number of L1 seeds a chain can have before it is dropped from Union rate groups due to exploding (2^nL1) computational complexity." << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ I/O CONFIGURATION ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--files file1 file2 ...\t\t\t\tInput ntuple files." << std::endl;
          std::cout << "--ROSXML \"" << _ROSXML << "\"\t\tROS ID mapping file from which to map ROBIN IDs to subdetectors (place in /data)." << std::endl;
          std::cout << "--treeName \"" << _treeName << "\"\t\t\t\tName of TTree in ntuples containing cost data." << std::endl;
          std::cout << "--prefixL2 \"" << _L2Prefix << "\"\t\t\tPrefix of L2 cost branches." << std::endl;
          std::cout << "--prefixEF \"" << _EFPrefix << "\"\t\t\tPrefix of EF cost branches." << std::endl;
          std::cout << "--prefixHLT \"" << _HLTPrefix << "\"\t\t\tPrefix of HLT cost branches." << std::endl;
          std::cout << "--prefixConfig \"" << _configPrefix << "\"\tName of TTree in ntuple containing trigger configuration metadata." << std::endl;
          std::cout << "--outputRootDirectory \"" << _outRootDirectory << "\"\t\t\tFolder in which to place this programs output directory." << std::endl;
          std::cout << "--outputDirectory \"" << _outDirectory << "\"\tFolder to contain all program output, %r is replaced with the run number, %t with the tag." << std::endl;
          std::cout << "--outputTag \"" << _outTag << "\"\t\t\t\tTag identifying this processing to be used in the output folder name (any underscores will be removed)." << std::endl;
          std::cout << "--outputTagFromAthena\t\t\t\tWhen running in Athena, this option will use the $AtlasVersion environment variable." << std::endl;
          std::cout << "--outputRootFile \"" << _outRootFilename << "\"\tName of ROOT file for results." << std::endl;
          std::cout << "--outputImageDirectory \"" << _outImageDirectory << "\"\t\t\tName of folder for image results." << std::endl;
          std::cout << "--outputCSVDirectory \"" << _outCSVDirectory << "\"\t\t\tName of folder for CSV results." << std::endl;
          std::cout << "--outputXMLDirectory \"" << _outXMLDirectory << "\"\t\t\tName of folder for XML results." << std::endl;
          std::cout << "--cleanAll\t\t\t\t\tRemove any old output directory before saving output." << std::endl;
          std::cout << "--userDetails \t\t\t\t\tUser supplied metadata string giving any extra details about this run." << std::endl;
          std::cout << "\t~~~~~~~~~~~~~~~ OTHER ~~~~~~~~~~~~~~~" << std::endl;
          std::cout << "--help\t\t\t\t\t\tDisplays this very message and exits." << std::endl;
          std::cout << "--version\t\t\t\t\tDisplays the application version and exits." << std::endl;
          abort();
        }
        // I did not set a flag, but was also not caught above. Require implementation
        Error("Config::parseCLI", "The following option was not recognised: %s",_longOptions[_optionIndex].name);
        abort();
        break;
      case 'f':
        // I'm setting the input files
        // Do secondary loop to identify all input files
        _inputFiles.push_back( std::string( optarg ) );
        while (optind < argc) {
          if ( std::string( argv[optind] ).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _inputFiles.push_back( std::string( argv[optind++] ) );
        }
        break;
      case 'p':
        // I'm setting the chain pattern monitor
        // Do secondary loop to identify all patterns
        _patternsMonitor.push_back( std::string( optarg ) );
        while (optind < argc) {
          if ( std::string( argv[optind] ).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsMonitor.push_back( std::string( argv[optind++] ) );
        }
        break;
      case 'P':
        // I'm setting the chain pattern output
        _patternsOutput.push_back( std::string( optarg ) );
        while (optind < argc) {
          if ( std::string( argv[optind] ).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _patternsOutput.push_back( std::string( argv[optind++] ) );
        }
        break;
      case 'u':
        // I'm setting the user details string
        _userDetails = std::string( optarg );
        while (optind < argc) {
          if ( std::string( argv[optind] ).substr(0, 1) == "-") { //We're back to arguments
            break;
          }
          _userDetails += std::string(" ") + std::string( argv[optind++] );
        }
        break;
      case 'n':
        // N Events
        _ss << optarg;
        _ss >> _nEvents;
        break;
      case 'h':
        // N histogram bins
        _ss << optarg;
        _ss >> _histogramBins;
        break;
      case 's':
        // N Events to skip
        _ss << optarg;
        _ss >> _nSkip;
        break;
      case 'S':
        // Slow threshold
        _ss << optarg;
        _ss >> _slowThreshold;
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
      case 'L':
        // Number of lumi blocks to do a full summary for
        _ss << optarg;
        _ss >> _nLbFullSummary;
        break;
      case 'd':
        // Default lumiblock length
        _ss << optarg;
        _ss >> _defaultLBLength;
        break;
      case 'X':
        // Default lumiblock length
        _ss << optarg;
        _ss >> _maxMultiSeed;
        break;
      case 't':
        // Different tree name
        _treeName = std::string( optarg );
        break;
      case 'T':
        // Different tag name
        _outTag = std::string( optarg );
        break;
      case 'm':
        // Different menu XML
        _menuXML = std::string( optarg );
        break;
      case 'M':
        // Different prescale menu XML
        _prescaleXML = std::string( optarg );
        break;
      case 'R':
        // Different ROS mapping
        _ROSXML = std::string( optarg );
        break;
      case 'r':
        // Different output root
        _outRootFilename = std::string( optarg );
        break;
      case 'i':
        // Different output image
        _outImageDirectory = std::string( optarg );
        break;
      case 'C':
        // Different output csv dir
        _outCSVDirectory = std::string( optarg );
        break;
      case 'x':
        // Different output xml dir
        _outXMLDirectory = std::string( optarg );
        break;
      case 'o':
        // Different output directory
        _outDirectory = std::string( optarg );
        break;
      case 'O':
        // Different output ROOT directory
        _outRootDirectory = std::string( optarg );
        break;
      case 'l':
        // Different L2 prefix
        _L2Prefix = std::string( optarg );
        break;
      case 'E':
        // Different EF prefix
        _EFPrefix = std::string( optarg );
        break;
      case 'w':
        // different overlap percent
        _ss << optarg;
        _ss >> _ratesOverlapWarning;
        break;
      case 'H':
        // Different HLT prefix
        _HLTPrefix = std::string( optarg );
        break;
      case 'c':
        // Different HLT prefix
        _configPrefix = std::string( optarg );
        break;
      default:
        Error("Config::parseCLI", "Supplied argument '%c' not recognised.", Char_t(_status));
        abort ();
      }
    }
    
    // Settings read. Now to check them and store them for use

    if ( _showVersion == kTRUE ) {
      Info("Config::parseCLI","Version %s", _version.c_str());
      abort();
    }

    //////////////////////////
    //        ROOT FILES, CHAIN AND TREE NAMES
    //////////////////////////

    if ( _inputFiles.size() == 0 ) {
      Error("Config::parseCLI", "Please supply at least one input file with --file.");
      return kFALSE;
    }
    set(kInputFiles, _inputFiles, "InputFiles");
    set(kNEvents, _nEvents, "NEvents");
    set(kNSkip, _nSkip, "NSkip");
    set(kTreeName, _treeName, "TreeName");
    set(kL2Prefix, _L2Prefix, "L2Prefix");
    set(kEFPrefix, _EFPrefix, "EFPrefix");
    set(kHLTPrefix, _HLTPrefix, "HLTPrefix");
    set(kConfigPrefix, _configPrefix, "ConfigPrefix"); 

    //////////////////////////
    //        HLT LEVELS AND RUN RANGES TO PROCESS
    //////////////////////////

    // Trigger Level settings
    if ( _doL2 + _doEF + _doL2EF + _doHLT != 1 ) {
      //  Allow use of doL2 and doEF instead of --doL2EF
      if ( ! (_doL2 == 1 && _doEF == 1 && _doHLT == 0) ) {
        Warning("Config::parseCLI", "Not specified a HLT level to run (--doL2, --doEF, --doL2EF or --doHLT). Will default to --doHLT.");
        _doHLT = 1;
      }
    }
    set(kDoL2, (Int_t) (_doL2 || _doL2EF), "DoL2");
    set(kDoEF, (Int_t) (_doEF || _doL2EF), "DoEF");
    set(kDoHLT, _doHLT, "DoHLT");
    set(kDoHLTOnline, 0, "DoHLTOnline"); // If unified HLT was used online - not yet ever true

    // Summary range settings
    if ( _summaryAll + _summaryPerHLTConfig + _summaryPerLumiBlock == 0 ) {
      Warning("Config::parseCLI", "Not specified a time range to summaries. Will default to --summaryPerHLTConfig.");
      _summaryPerHLTConfig = 1;
    }
    set(kDoAllSummary, _summaryAll, "DoSummaryAll"); 
    set(kDoKeySummary, _summaryPerHLTConfig, "DoKeySummary");
    set(kDoLumiBlockSummary, _summaryPerLumiBlock, "DoLBSummary");

    // Lumi block settings. Start, end, and number to fully summarise
    if ( _lbEnd < _lbBegin ) {
      Error("Config::parseCLI", "End lumi block number cannot be before starting lumi block number.");
      return kFALSE;
    }
    set(kLumiStart, _lbBegin, "LumiBlockStart");
    set(kLumiEnd, _lbEnd, "LumiBlockEnd");
    set(kNLbFullSummary, _nLbFullSummary, "NLBFullSummary");

    // Number of events to save full information for
    set(kFullEventMaxNumToSave, _maxNumberFullEvents, "MaxNumberFullEvents"); 
    if (_nEvents != INT_MAX) {
      set(kFullEventSaveOnePer, (Int_t)( 1 + ( _nEvents / getInt(kFullEventMaxNumToSave) ) ), "FullEventSaveOncePer");
    } else {
      set(kFullEventSaveOnePer, 1000, "FullEventSaveOncePer" );//This is just a generic fallback. We can update it later in the event loop if need be
    } 
    
    //////////////////////////
    //        MONITORS TO INCLUDE
    //////////////////////////

    // This flag just enables others
    if (_monitorAllChainSeqAlgs == 1) {
      _monitorChains = 1;
      _monitorChainAlgs = 1;
      _monitorSequences  = 1;
      _monitorSequenceAlgs = 1;
      _monitorAlgs = 1;
    }

    if ( _monitorAll 
      + _monitorChains 
      + _monitorChainAlgs
      + _monitorSequences 
      + _monitorSequenceAlgs
      + _monitorAlgs 
      + _monitorROS 
      + _monitorROI
      + _monitorL1 
      + _monitorFullEvent
      + _monitorGlobals
      + _monitorRates == 0 ) {
      Warning("Config::parseCLI", "Not specified a monitor to use. Will default to --monitorAll.");
      _monitorAll = 1;
    }

    // We save monitorAll separately as it is picked up in a time saving loop later
    set(kDoAllMonitor,               _monitorAll, "MonitorAll"); // This one is magic, and will be picked up in a loop later - force enabling all others
    set(kDoChainMonitor,             _monitorChains, "ChainMonitor");
    set(kDoChainAlgorithmMonitor,    _monitorChainAlgs, "ChainAlgorithmMonitor");
    set(kDoSequenceMonitor,          _monitorSequences, "SequenceMonitor");
    set(kDoSequenceAlgorithmMonitor, _monitorSequenceAlgs, "SequenceAlgorithmMonitor");
    set(kDoAlgorithmMonitor,         _monitorAlgs, "AlgorithmMonitor");
    set(kDoROSMonitor,               _monitorROS, "ROSMonitor");
    set(kDoROIMonitor,               _monitorROI, "ROIMonitor");
    set(kDoL1ChainMapMonitor,        _monitorL1, "L1ChainMapMonitor");
    set(kDoFullEventMonitor,         _monitorFullEvent, "FullEventMonitor");
    set(kDoGlobalsMonitor,           _monitorGlobals, "GlobalsMonitor");
    set(kDoRatesMonitor,             _monitorRates, "RatesMonitor");

    // String patterns to match when doing monitoring
    set(kPatternsMonitor, _patternsMonitor, "PatternsMonitor", kUnlocked);
    set(kPatternsOutput, _patternsOutput, "PatternsOutput", kUnlocked);

    //////////////////////////
    //        OUTPUT TO GENERATE
    //////////////////////////

    if ( _doOutputHist + _doOutputCanvas + _doOutputPng + _doOutputPdf + _doOutputCsv + _doOutputXML == 0) {
      Warning("Config::parseCLI", "No output format specified. Will default to --doOutputHist --doOutputCSV --doOutputMenus --doOutputRatesGraph --doOutputRatesXML");
      _doOutputHist = 1;
      _doOutputCsv = 1;
      _doOutputRatesGraph = 1;
      _doOutputMenus = 1;
      _doOutputXML = 1;
    }

    //Note - outputRootDir is not saved on its own, it is combined here
    _outDirectory = _outRootDirectory + "/" + _outDirectory;

    // See if we have been requested to get the AtlasVersion from Athena and use it in the tag
    if (_outputTagFromAthena == kTRUE) {
      const Char_t* _env = std::getenv("AtlasVersion");
      if (_env != NULL) {
        _outTag = std::string( _env );
      }
    }

    // Remove any "_" from the tag
    while (_outTag.find("_") != std::string::npos) {
      _outTag.replace( _outTag.find("_"), 1, m_blankString);
    }

    set(kOutputPng, _doOutputPng, "DoPNG");
    set(kOutputPdf, _doOutputPdf, "DoPDF");
    set(kOutputImage, (Int_t) ( _doOutputPdf || _doOutputPng), "DoOutputImage" );
    set(kOutputRoot, (Int_t) (_doOutputHist || _doOutputCanvas), "DoOutputROOT");
    set(kOutputHist, _doOutputHist, "DoOutputHist");
    set(kOutputCanvas, _doOutputCanvas, "DoOutputCanvas");
    set(kOutputCsv, _doOutputCsv, "DoOutputCSV");
    set(kOutputXML, _doOutputXML, "DoOutputXML");
    set(kOutputRatesGraph, _doOutputRatesGraph, "DoOutputRatesGraph");
    set(kOutputMenus, _doOutputMenus, "DoOutputMenus");    
    set(kOutputDirectory, _outDirectory, "OutputDirectory", kUnlocked); // Do NOT lock this as we need to modify it to add the run number in
    set(kOutputTag, _outTag, "OutputTag");
    set(kOutputImageDirectory, _outImageDirectory, "OutputImageDirectory");
    set(kOutputCSVDirectory, _outCSVDirectory, "OutputCSVDirectory");
    set(kOutputXMLDirectory, _outXMLDirectory, "OutputXMLDirectory");
    set(kOutputRatesGraphFilename, _outRatesGraphFilename, "OutputRatesGraphFilename");
    set(kOutputRootFilename, _outRootFilename, "OutputROOTFileName");
    set(kLinkOutputDir, _linkOutputDirectory, "LinkOutputDirectory");
    set(kLinkOutputDirName, "costMon");
    if (_userDetails != "") {
      set(kUserDetails, _userDetails, "UserMetadata");
    }
    
    //////////////////////////
    //        MISC SETTINGS
    //////////////////////////

    set(kSlowEventThreshold, _slowThreshold, "SlowThreshold"); 
    set(kDoEBWeighting, _doEBWeighting, "DoEBWeighting");
    set(kDefaultLBLength, _defaultLBLength, "DefaultLBLength"); 
    set(kDebug, _debug, "Debug");
    set(kCleanAll, _cleanAll, "CleanAll"); //Specifies if the output directory is to be regenerated
    set(kRatesForcePass, _ratesForcePass, "RatesForceAllChainsToPassRaw");
    set(kDoUniqueRates, _doUniqueRates, "DoUniqueRates");
    set(kHistBins, _histogramBins, "HistogramBins");
    set(kRatesOverlapWarning, _ratesOverlapWarning, "RatesOverlapWarning");
    set(kMaxMultiSeed, _maxMultiSeed, "MaxMultiSeed");

    // Check we only have one of these two
    if (_menuXML != m_blankString && _prescaleXML != m_blankString) {
      Error("Config::parseCLI", "Please supply only one of prescaleXML (%s) OR menuXML (%s).", _prescaleXML.c_str(), _menuXML.c_str());
      abort();
    }

    // Load files to be accessed
    // File Names
    if (_ROSXML != m_blankString) set(kROSXMLName, _ROSXML, "ROSXML");
    if (_menuXML != m_blankString) set(kMenuXMLName, _menuXML, "MenuXML");
    if (_prescaleXML != m_blankString) set(kPrescaleXMLName, _prescaleXML, "PrescaleXML");

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

    //Check if we're in ROOT CORE
    const Char_t* _env = std::getenv("ROOTCOREBIN");
    if (_env != NULL) {
      set(kDataDir, std::string(_env) + std::string("/data/TrigCostRootAnalysis/") );
      if (getIsSet(kROSXMLName)) set(kROSXMLPath, getStr(kDataDir) + getStr(kROSXMLName));
      if (getIsSet(kMenuXMLName)) set(kMenuXMLPath, getStr(kDataDir) + getStr(kMenuXMLName));
      if (getIsSet(kPrescaleXMLName)) set(kPrescaleXMLPath, getStr(kDataDir) + getStr(kPrescaleXMLName));      
    } else {
// CAUTION - THIS IS THE ONLY "ATHENA ONLY" CODE IN THIS PACKAGE
#ifndef ROOTCORE
      if (getIsSet(kROSXMLName)) {
        std::string _locAthena = PathResolverFindDataFile( getStr(kROSXMLName) );
        if (_locAthena == m_blankString) Error("Config::parseCLI","Athena cannot find ROS mapping file %s", getStr(kROSXMLName).c_str());
        else {
          set(kROSXMLPath, _locAthena);
          Info("Config::parseCLI","Athena has found the file: %s", getStr(kROSXMLPath).c_str());
        }
      }
      //
      if (getIsSet(kMenuXMLName)) {
        std::string _locAthena  = PathResolverFindDataFile( getStr(kMenuXMLName) );
        if (_locAthena == m_blankString) Error("Config::parseCLI","Athena cannot find menu XML file %s", getStr(kMenuXMLName).c_str());
        else {
          set(kMenuXMLPath, _locAthena);
          Info("Config::parseCLI","Athena has found the file: %s", getStr(kMenuXMLPath).c_str());
        } 
      }
      //
      if (getIsSet(kPrescaleXMLName)) {
        std::string _locAthena  = PathResolverFindDataFile( getStr(kPrescaleXMLName) );
        if (_locAthena == m_blankString) Error("Config::parseCLI","Athena cannot find prescale XML file %s", getStr(kPrescaleXMLName).c_str());
        else {
          set(kPrescaleXMLPath, _locAthena);
          Info("Config::parseCLI","Athena has found the file: %s", getStr(kPrescaleXMLPath).c_str());
        }
      }
#endif // not ROOTCORE
    }
    
    // Internally used parameters - do not need to be included in dump (hence do not need to be named either)
    set(kSloppyExit, 1, "SloppyExit");
    set(kOutputSummaryDirectory, "summary");
    set(kErrorIgnore, (Int_t) gErrorIgnoreLevel, "ErrorIgnoreLevel"); //Cache this internal ROOT variable
    set(kDirectlyApplyPrescales, 1, "DirectlyApplyPrescales");

    // Const strings literals to be referenced throughout
    set(kL1String,"L1");
    set(kL2String,"L2");
    set(kEFString, "EF");
    set(kHLTString, "HLT");
    set(kAllString, "All");
    set(kDelimatorString, "____");
    set(kLumiBlockString,"LumiBlock");
    set(kUnknownString, "UNKNOWN");
    set(kBlankString, m_blankString);  
    set(kDummyString, "dummy");  
    set(kMultipleString, "Multiple");
    set(kAndString,"__AND__");  
    set(kRateGlobalHLTString,"RATE_GLOBAL_HLT");  
    set(kRateGlobalL1String,"RATE_GLOBAL_L1"); 
    set(kRateUniqueString,"UNIQUE_");  
    set(kCachedString, "Cached");
    set(kCalledString, "Called");
    set(kAllROIString, "All_ROIs");
    set(kNoneString, "None");
    set(kMuonString, "Muon");
    set(kEmTauString, "EMTau");
    set(kJetString, "Jet");
    set(kJetEtString, "JetEt");
    set(kEnergyString, "Energy");
    set(kROBINString, "ROBIN");
    set(kROSString, "ROS");
    set(kAlwaysPassString, "UNSEEDED");

    set(kVarTime, "Time");
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
    set(kVarCalls, "Calls");
    set(kVarCallsSlow, "CallsSlow");
    set(kVarEventsPassed, "EventsPassed");
    set(kVarEventsPassedDP, "EventsPassedDirectPrescale");
    set(kVarEventsPassedRaw, "EventsPassedRaw");
    set(kVarEventsPassthrough, "EventsPassthrough");
    set(kVarEventsSlow, "EventsSlow");
    set(kVarTotalPrescale, "TotalPrescale");
    set(kVarL1PassEvents, "L1PassEvents");
    set(kVarHLTEvents, "HLTEvents");
    set(kVarHLTPassEvents, "HLTPassEvents");
    set(kVarHLTPUs, "HLTPUs");
    set(kVarROI, "ROI");
    set(kVarType, "Type");
    set(kVarEta, "Eta");
    set(kVarPhi, "Phi");
    set(kVarArea, "Area");
    set(kVarL1Thresh, "L1Thresholds");
    set(kVarEventsPerLumiblock, "EventsPerLB");
    set(kVarOverlap, "Overlap");
    set(kVarOverlapDP, "OverlapDirectPrescale");
    // Decoration ENUMs
    set(kDecStartTime, "StartTime");
    set(kDecCallOrCache, "CallOrCache");
    set(kDecROIString, "ROIString");
    set(kDecROSString, "ROSString");
    set(kDecAlgClassName, "AlgClassName");
    set(kDecChainName, "ChainName");
    set(kDecSeqName, "SequenceName");
    set(kDecLbLength, "LBLength");
    set(kDecType, "Type");
    set(kDecRatesGroupName, "RatesGroupName");
    set(kDecPrescale, "Prescale");
    // End of option parsing
    
    return kTRUE;
  }
  
  /**
   * Prints the contents of the program configuration.
   */
  void Config::dump() {
    Info("Config::dump", "TrigCostD3PD has %i configuration settings saved for this processing.", Int_t (m_settingsStr.size() + m_settingsVecStr.size() + m_settingsInt.size()) );
    
    std::map<ConfKey_t, Int_t>::iterator _itInt;
    std::map<ConfKey_t, std::string>::iterator _itStr;
    std::map<ConfKey_t, std::vector<std::string> > ::iterator _itStrVec;
    Int_t _i = -1;
    for (_itInt = m_settingsInt.begin(); _itInt != m_settingsInt.end(); ++_itInt) {
      const std::string _name = m_settingsName[_itInt->first];
      Info("Config::dump", " [INT:%i]\t[LOCK:%i]\t%s = %i", ++_i, (Int_t) getIsLocked(_itInt->first), _name.c_str(), _itInt->second  );
    }
    _i = -1;
    for (_itStr = m_settingsStr.begin(); _itStr != m_settingsStr.end(); ++_itStr) {
      const std::string _name = m_settingsName[_itStr->first];
      Info("Config::dump", " [STRING:%i]\t[LOCK:%i]\t%s = \"%s\"", ++_i, (Int_t) getIsLocked(_itStr->first), _name.c_str(), _itStr->second.c_str()  );
    }
    _i = -1;
    for (_itStrVec = m_settingsVecStr.begin(); _itStrVec != m_settingsVecStr.end(); ++_itStrVec) {
      const std::string _name = m_settingsName[_itStrVec->first];
      Info("Config::dump", " [VEC<STR>:%i]\t[LOCK:%i]\t%s, size %u", ++_i, (Int_t) getIsLocked(_itStrVec->first), _name.c_str(), (UInt_t) _itStrVec->second.size() );
      for (UInt_t _v = 0; _v < _itStrVec->second.size(); ++_v) {
        Info("Config::dump", "\t\t\t\t  [%u] \"%s\"", _v, _itStrVec->second.at(_v).c_str() );
      }
    }
    
  }
  
  /**
   * Retrieve string configuration for given enum key.
   * @param _key enum key for this config.
   * @returns Requested configuration string or blank string if key not found.
   */
  const std::string& Config::getStr( ConfKey_t _key ) {
    if ( m_settingsStr.count( _key ) == 0 ) {
      Error("Config::getStr", "Unknown string-key %i. (name, if any:%s)", _key, m_settingsName[_key].c_str() );
      return m_blankString;
    }
    // Return the requested string
    return m_settingsStr[_key];
  }

  /**
   * Retrieve string name of config enum
   * @param _key enum key for this config.
   * @returns Requested configuration enum's na,me.
   */
  const std::string& Config::getName( ConfKey_t _key ) {
    if ( m_settingsName.count( _key ) == 0 ) {
      Error("Config::getName", "Unknown key %i", _key );
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
  Int_t Config::getInt( ConfKey_t _key ) {
    if ( m_settingsInt.count( _key ) == 0 ) {
      Error("Config::getInt", "Unknown key %i", _key );
      return -1;
    }
    return m_settingsInt[_key];
  }
  
  /**
   * Retrieve size of vector storage for given enum key.
   * @param _key enum key for this config.
   * @return Size of vector for given key, or INT_MAX if key not found.
   */
  UInt_t Config::getVecSize( ConfKey_t _key ) {
    if ( m_settingsVecStr.count( _key ) == 0 ) {
      Error("Config::getVecSize", "Unknown key %i", _key );
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
  const std::string& Config::getVecEntry( ConfKey_t _key, UInt_t _location ) {
    if ( m_settingsVecStr.count( _key ) == 0 ) {
      Error("Config::getVecEntry", "Unknown key %i", _key);
      return m_blankString;
    }
    if (_location >= getVecSize( _key )) {
      Error("Config::getVecEntry", "Location %i is out of range", _location );
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
  Bool_t Config::addVecEntry( ConfKey_t _key, const std::string& _toAdd ) {
    if ( m_settingsVecStr.count( _key ) == 0 ) {
      Error("Config::getVecEntry", "Unknown key %i", _key );
      return kFALSE;
    }
    if ( getIsLocked( _key, kTRUE ) == kTRUE ) return kFALSE;
    m_settingsVecStr[_key].push_back( _toAdd );
    return kTRUE;
  }

  /**
   * If a settings vector is registered with a given key, and it is unlocked, then remove the given entry.
   * Note the test is for an exact match, whereas getVecMatches() checks for a fuzzy match.
   * @param _key enum key for this config.
   * @param _toRemove Const reference to string to remove from the settings vector.
   * @return true if one or more entries were removed.
   */
  Bool_t Config::removeVecEntry( ConfKey_t _key, const std::string& _toRemove ) {
    Bool_t _entriesRemoved = kFALSE;
    if ( m_settingsVecStr.count( _key ) == 0 ) {
      Error("Config::getVecEntry", "Unknown key %i", _key );
      return kFALSE;
    }
    if ( getIsLocked( _key, kTRUE ) == kTRUE ) return kFALSE;
    std::vector<std::string>::iterator _it = m_settingsVecStr[_key].begin();
    while(_it != m_settingsVecStr[_key].end()) {
      if ( _toRemove.compare( (*_it) ) == 0 ) { // Compare == 0 for exact match only
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
  Bool_t Config::clearVec( ConfKey_t _key ) {
    Bool_t _entriesRemoved = kFALSE;
    if ( m_settingsVecStr.count( _key ) == 0 ) {
      Error("Config::getVecEntry", "Unknown key %i", _key );
      return kFALSE;
    }
    if ( getIsLocked( _key, kTRUE ) == kTRUE ) return kFALSE;
    if ( m_settingsVecStr[_key].size() ) _entriesRemoved = kTRUE;
    m_settingsVecStr[_key].clear();
    return _entriesRemoved;
  }

  /**
   * Check if a vector of strings configuration contains a match to the given string.
   * Note: the pattern may match a sub-string, it does not need exactly equal one vector entry.
   * To be clear: the stored value must be a sub-string of _entry
   * @param _key enum key for this config.
   * @param _entry The string to match.
   * @return kTRUE if the patter.
   */
  Bool_t Config::getVecMatches( ConfKey_t _key, const std::string& _entry ) {
    if ( m_settingsVecStr.count( _key ) == 0 ) {
      Error("Config::getVecEntry", "Unknown key %i", _key );
      return kFALSE;
    }
    
    for (UInt_t _i = 0; _i < m_settingsVecStr[_key].size(); ++_i) {
      if ( _entry.find( m_settingsVecStr[_key].at(_i) ) != std::string::npos ) {
        // This chain contains the correct string name
        return kTRUE;
      }
    }
    return kFALSE;
  }
  
  /**
   * Get if there is a setting stored with the given key. Regardless of type.
   * @param _key enum key for this config.
   * @return kTRUE if a setting is stored, with any type, for the given key.
   */
  Bool_t Config::getIsSet( ConfKey_t _key ) {
    if ( m_settingsVecStr.count( _key ) == 1 ) return kTRUE;
    if ( m_settingsInt.count( _key ) == 1 ) return kTRUE;
    if ( m_settingsStr.count( _key ) == 1 ) return kTRUE;
    return kFALSE;
  }
  
  /**
   * Store or overwrite a string configuration with a enum key.
   * If no name is given, then it is assumed it is its name is its value
   * @param _key enum key for this config.
   * @param _value Const string value to store.
   */
  void Config::set( ConfKey_t _key, const std::string _value, const std::string _name, LockStatus_t _lock ) {
    if ( getIsLocked( _key, kTRUE ) == kTRUE ) return;
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
  void Config::set ( ConfKey_t _key, const Int_t _value, const std::string _name, LockStatus_t _lock ) {
    if ( getIsLocked( _key, kTRUE ) == kTRUE ) return;
    m_settingsInt[_key] = _value;
    m_settingsName[_key] = _name;
    m_settingsLock[_key] = _lock;
    // This is a special flag, set for easy access.
    if (_key == kDebug) {
      m_debug = (Bool_t) _value;
    }
  }
  
  /**
   * Store or overwrite a vector of strings configuration with a enum key.
   * @param _key enum key for this config.
   * @param _name Optional name for this entry.
   * @param _value Const vector of strings to store.
   */
  void Config::set ( ConfKey_t _key, const std::vector< std::string > _value, const std::string _name, LockStatus_t _lock ) {
    if ( getIsLocked( _key, kTRUE ) == kTRUE ) return;
    m_settingsVecStr[_key] = _value;
    m_settingsName[_key] = _name;
    m_settingsLock[_key] = _lock;
  }
  
  /**
   * Check if this key has been used before, and if so then whether the lock was set to true. Regardles of type.
   * @param _key enum key for this config.
   * @param _printErrorMsg Set to kTRUE if the intention was to modify this record, will display error message to user.
   * @return kTRUE if the key exists and the lock was set.
   */
  Bool_t Config::getIsLocked( ConfKey_t _key, Bool_t _printErrorMsg ) {
    if ( m_settingsLock.count( _key ) == 1 ) {
      if ( m_settingsLock[_key] == kLocked ) {
        if (_printErrorMsg == kTRUE) Error("Config::getIsLocked", "The configuration key %i is LOCKED and cannot be modified.", _key );
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
  
} // namespace TrigCostRootAnalysis
