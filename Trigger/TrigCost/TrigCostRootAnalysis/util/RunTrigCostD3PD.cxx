/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// STL include(s):
#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <fstream>
#include <time.h>
#include <set>
#include <sys/resource.h> //system resource usage
#include <stdio.h>
#include <signal.h> // Catch

// ROOT include(s):
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TError.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TInterpreter.h>

// Local include(s):
#include "TrigCostRootAnalysis/Config.h"
#include "TrigCostRootAnalysis/ProcessEvent.h"
#include "TrigCostRootAnalysis/TrigConfInterface.h"
#include "TrigCostRootAnalysis/MonitorBase.h"
#include "TrigCostRootAnalysis/TrigCostData.h"
#include "TrigCostRootAnalysis/Utility.h"
#include "TrigCostRootAnalysis/TrigCostAtlasStyle.h"
#include "TrigCostRootAnalysis/DataVariable.h"
#include "TrigCostRootAnalysis/TrigXMLService.h"
#include "TrigCostRootAnalysis/Config.h"

/**
 *
 * Main executable for TrigCostRootAnalysis package
 *
 */

// This function is to intercept Ctrl-C
UInt_t _terminateCalls = 0;
void handlerFunctionTerm (Int_t _parameter) {
  if (_parameter == 2 && _terminateCalls++ == 0) {
    Warning("TrigCostD3PD","Received exit signal. Exiting event loop and writing output for events processed so far.");
  } else if (_parameter == 2 && _terminateCalls++ > 0) {
    Fatal("TrigCostD3PD","Received another exit signal. Aborting now.");
    abort();
  }
}

using namespace TrigCostRootAnalysis;

int main(int argc, char* argv[]) {

  signal(SIGINT, handlerFunctionTerm);

  Timer _timerSetup("Main","Setup");
  Timer _timerLoop("Main","Event Loop");
  Timer _timerFinalise("Main","Finalise");
  Timer _timerWrite("Main","Writing");

  clock_t _tStart, _tEnd, _tProgress;
  _tStart = clock();
  _timerSetup.start();

  SetTrigCostAtlasStyle();

  // Dictionary issues with EOS
  //gInterpreter->GenerateDictionary("vector<vector<float> >;vector<vector<vector<float> > >", "vector");

  Info("TrigCostD3PD", "Execute RunTrigCostD3PD");

  Config& _config = Config::config();
  Bool_t _isConfigured = _config.parseCLI(argc, argv);

  if (_isConfigured) {
    Info("TrigCostD3PD","All program options parsed, preparing event loop.");
  } else {
    Error("TrigCostD3PD","There were issues configuring this cost execution. Try --help. Aborting.");
    abort();
  }


  // Master chain of all files to analyse
  TChain _chain( _config.getStr(kTreeName).c_str() );
  // Configuration tree chain

  // Add files to chain
  for (UInt_t _f = 0; _f < _config.getVecSize(kInputFiles); ++_f) {
    _chain.Add(       _config.getVecEntry(kInputFiles, _f).c_str() );
  }

  // 'event' is very important, it's used pass-by-reference and dictates which entry in the tree we're reading from.
  Long64_t _event = 0;

  // Create readers for cost branches and attach them to the TChain
  // These readers are wrapped in a data access class which provides a more intuitive interface.
  TrigCostData _L2Data;
  TrigCostData _EFData;
  TrigCostData _HLTData;

  if ( _config.getInt(kDoL2) == kTRUE ) _L2Data.setup( _event, _config.getStr(kL2Prefix).c_str(), &_chain );
  if ( _config.getInt(kDoEF) == kTRUE ) _EFData.setup( _event, _config.getStr(kEFPrefix).c_str(), &_chain );
  if ( _config.getInt(kDoHLT) == kTRUE ) _HLTData.setup( _event, _config.getStr(kHLTPrefix).c_str(), &_chain );

  // Create event processors. These hold individual monitors and distribute data in the event loop.
  ProcessEvent* _processEventL2  = new ProcessEvent(&_L2Data,  2, _config.getStr(kL2String));
  ProcessEvent* _processEventEF  = new ProcessEvent(&_EFData,  3, _config.getStr(kEFString));
  ProcessEvent* _processEventHLT = new ProcessEvent(&_HLTData, 2, _config.getStr(kHLTString));

  // Setup the monitors we'll be using within the ProcessEvents
  // Start at kAllMonitors - this will fire an internal loop in ProcessEvent if true to enable all monitors in the enum

  for (UInt_t _mon = kDoAllMonitor; _mon > kMonitorBegin; --_mon) {
    ConfKey_t _monKey = static_cast<ConfKey_t>(_mon);
    if ( _config.getInt(kDoL2) == kTRUE ) _processEventL2->setMonitoringMode(   _monKey, _config.getInt(_monKey) );
    if ( _config.getInt(kDoEF) == kTRUE ) _processEventEF->setMonitoringMode(   _monKey, _config.getInt(_monKey) );
    if ( _config.getInt(kDoHLT) == kTRUE ) _processEventHLT->setMonitoringMode( _monKey, _config.getInt(_monKey) );
    // If kAllMonitors was true we can stop here
    if (_monKey == kDoAllMonitor && _config.getInt(kDoAllMonitor) == kTRUE) break;
  }

  UInt_t _eventsSinceLastPrint = 0, _messageNumber = 0;
  UInt_t _reportAfter = CLOCKS_PER_SEC * _config.getInt(kMessageWait);
  time_t _onlineTime = 0;
  Float_t _effectivePrescale = 0.;
  std::set<UInt_t> _lbProcessed;
  std::string _onlineTimeStr;
  struct rusage _resources;
  std::string _outputProgressFile;
  // Begin event loop

  _config.set(kEventsInFiles, (Int_t) _chain.GetEntries(), "EventsInFiles");
  Info("D3PD","Events in files: %i", _config.getInt(kEventsInFiles) );
  if (!_config.getInt(kEventsInFiles)) abort();

  _tProgress = _tStart;

  Int_t _nPasses = _config.getInt(kNPasses);
  for (Int_t _pass = 1; _pass <= _nPasses; ++_pass) {
    if (_nPasses > 1) Info("TrigCostD3PD","|----------|---PASS----|--NUMBER--|-----%i------|----OF----|-----%i------|--------------|", _pass, _nPasses);
    _config.set(kCurrentPass, _pass, "CurrentPass", kUnlocked);
    _processEventL2->setPass( _pass );
    _processEventEF->setPass( _pass );
    _processEventHLT->setPass( _pass );
    _config.set(kEventsProcessed, 0, "EventsProcessed", kUnlocked);
    _config.set(kEventsSkipped, 0, "EventsSkipped", kUnlocked);

    for(Long64_t _masterEvent = 0; _masterEvent < _chain.GetEntries(); ++_masterEvent ) {

      // Load correct tree in chain into memory
      // Returns the local entry number within the tree
      // (D3PD reader doesn't distinguish between TTree or TChain)
      _event = _chain.LoadTree( _masterEvent );
      if (_event == 0) {
        Info("TrigCostD3PD","Opening new file: %s", _chain.GetFile()->GetName() );
      }
      assert( _event >= 0 ); // -1=Empty, -2=OutOfRange, -3=FileIOProblem, -4=TTreeMissing

      // If first event, perform configuration on loaded treesf
      if (_masterEvent == 0 && _pass == 1 && _config.getInt(kWriteEBWeightXML) == kFALSE)  {
        // Get the run number
        if ( _config.getInt(kDoL2) && _config.getIsSet(kRunNumber) == kFALSE ) {
          _config.set(kRunNumber, _L2Data.getRunNumber(), "RunNumber" );
          _onlineTime = _L2Data.getCostRunSec();
        } else if ( _config.getInt(kDoEF) && _config.getIsSet(kRunNumber) == kFALSE ) {
          _config.set(kRunNumber, _EFData.getRunNumber(), "RunNumber" );
          _onlineTime = _EFData.getCostRunSec();
        } else if ( _config.getInt(kDoHLT) && _config.getIsSet(kRunNumber) == kFALSE )  {
          _config.set(kRunNumber, _HLTData.getRunNumber(), "RunNumber" );
          _onlineTime = _HLTData.getCostRunSec();
        }
        struct tm* _timeinfo = gmtime( &_onlineTime );
        char _buffer[80];
        strftime(_buffer,80,"%F %R UTC",_timeinfo);
        _onlineTimeStr = std::string(_buffer);
        std::string _outDirectory = _config.getStr(kOutputDirectory);
        if ( _outDirectory.find("%r") != std::string::npos ) { //If this string is present, replace it with the run number
          _outDirectory.replace( _outDirectory.find("%r"), 2, intToString(_config.getInt(kRunNumber)) );
        }
        if ( _outDirectory.find("%t") != std::string::npos ) { //If this string is present, replace it with the tag
          _outDirectory.replace( _outDirectory.find("%t"), 2, _config.getStr(kOutputTag) );
        }
        _config.set(kOutputDirectory, _outDirectory, "OutputDirectory", kLocked); // Lock this time
        Info("TrigCostD3PD","We are processing run %i, cost montioring has the timestamp %s", _config.getInt(kRunNumber), _onlineTimeStr.c_str() );
        if (_config.getInt(kCleanAll)) { // send rm command to get rid of current output directory
          std::string _toCall = std::string("rm -r ") + _config.getStr(kOutputDirectory) + std::string("/");
          Info("TrigCostD3PD","Cleaning up from before, executing the command: '%s'", _toCall.c_str());
          gSystem->Exec( _toCall.c_str() );
        }
        gSystem->mkdir( _config.getStr(kOutputDirectory).c_str(), kTRUE );
        _outputProgressFile = _config.getStr(kOutputDirectory) + "/progress.json";
        // Do symlink
        if (_config.getInt(kLinkOutputDir) == kTRUE) {
          std::string _toCall = std::string("ln -sfn ") + _config.getStr(kOutputDirectory) + std::string(" ") + _config.getStr(kLinkOutputDirName);
          Info("TrigCostD3PD","Soft linking the output dir, executing the command: '%s'", _toCall.c_str());
          gSystem->Exec( _toCall.c_str() );
        }
        // Setup the trigger configuration
        TrigConfInterface::configure( &_chain );
        if ( _config.debug() ) TrigConfInterface::debug();
        if ( _config.getInt(kWriteDummyPSXML) == kTRUE) {
          // Here the user has asked us to simple dump out the menu to a prescale XML file
          TrigXMLService::trigXMLService().writePrescaleXML();
          Info("TrigCostD3PD","Written dummy XML, program will now terminate.");
          abort();
        }
        // Load the effective "prescale" that CostMon was run with online.
        Float_t _doOperationalInfo = 0, _execPrescale = 0.;
        std::string _doOperationalInfoStr = TrigConfInterface::getMetaStringVal("doOperationalInfo");
        std::string _execPrescaleStr = TrigConfInterface::getMetaStringVal("ExecPrescale");
        if (_doOperationalInfoStr != _config.getStr(kBlankString)) _doOperationalInfo = stringToFloat( _doOperationalInfoStr );
        if (_execPrescaleStr != _config.getStr(kBlankString)) _execPrescale = stringToFloat( _execPrescaleStr );
        _effectivePrescale = _doOperationalInfo * _execPrescale;
        // This is for backward compatability for files without these data
        if ( isZero(_effectivePrescale) == kTRUE ) _effectivePrescale = 1.;
        _config.setFloat(kEffectivePrescale, _effectivePrescale, "EffectivePrescale");
        Float_t _basicWeight = _config.getFloat( kBasicEventWeight );
        Float_t _lumiExtrapWeight = TrigXMLService::trigXMLService().getLumiExtrapWeight();
        Float_t _eventWeight = _basicWeight * _effectivePrescale * _lumiExtrapWeight;
        if ( isZero(_eventWeight - 1.) == kTRUE) { // Info if weight == 1
          Info("TrigCostD3PD","Will apply global weight %f to all events (BasicWeight:%.2f, EffectiveCostPrescale:%.2f, LumiExtrapWeight:%.2f)",
           _eventWeight, _basicWeight, _effectivePrescale, _lumiExtrapWeight);
        } else { // If weight != 1, this should be bumped up to a warning
          Warning("TrigCostD3PD","Will apply global weight %f to all events (BasicWeight:%.2f, EffectiveCostPrescale:%.2f, LumiExtrapWeight:%.2f)",
           _eventWeight, _basicWeight, _effectivePrescale, _lumiExtrapWeight);
        }
        _config.setFloat(kEventWeight, _eventWeight, "EventWeight");
        _timerSetup.stop();
      }

      // Get trigger configuration for current event
      //TrigConfInterface::getEntry( _event ); //not needed

      // Skip N events at beginning
      if ( _config.getInt(kEventsSkipped) < _config.getInt(kNSkip)) {
        _config.increment(kEventsSkipped);
        continue;
      }

      _timerLoop.start();

      // Get event weight
      Float_t _weight = 1;
      if (_config.getInt(kWriteEBWeightXML) == kFALSE) {
        _weight = _config.getFloat( kEventWeight ); // This is our base weight that we can apply additional weights on top of
      }

      // Check / get menu configuration
      if (_config.getInt(kWriteEBWeightXML) == kFALSE) TrigConfInterface::newEvent();

      Bool_t _eventAccepted = kFALSE;

      // Execute L2 monitoring
      if (_config.getInt(kDoL2) && !isZero(_weight)) {
        // Check lumi block
        if ( _L2Data.getLumi() >= _config.getInt(kLumiStart) && _L2Data.getLumi() <= _config.getInt(kLumiEnd) ) {
          _eventAccepted = _processEventL2->newEvent( _weight );
        }
      }

      // Execute EF monitoring
      if (_config.getInt(kDoEF) && !isZero(_weight)) {
        // Check lumi block
        if ( _EFData.getLumi() >= _config.getInt(kLumiStart) && _EFData.getLumi() <= _config.getInt(kLumiEnd) ) {
          _eventAccepted = _processEventEF->newEvent( _weight );
        }
      }

      // Execute HLT monitoring
      if (_config.getInt(kDoHLT)  && !isZero(_weight) ) {
        if (_config.getInt(kWriteEBWeightXML) == kTRUE) {
          TrigXMLService::trigXMLService().exportEnhancedBiasXML( _HLTData.getEventNumber(), _HLTData.getEBWeight(), _HLTData.getEBWeightBG(), _HLTData.getEBUnbiased() );
        } else if ( _HLTData.getLumi() >= _config.getInt(kLumiStart) && _HLTData.getLumi() <= _config.getInt(kLumiEnd) && TrigXMLService::trigXMLService().getIsLBFlaggedBad( _HLTData.getLumi() ) == kFALSE ) {
          _lbProcessed.insert( _HLTData.getLumi() );
          _eventAccepted = _processEventHLT->newEvent( _weight );
        }
      }

      if ( (clock() - _tProgress) >= _reportAfter ) {
        _tProgress = clock();
        if (_messageNumber++ % 20 == 0) {
          Info("TrigCostD3PD","|----------|-----------|----------|------------|----------|------------|--------------|");
          Info("TrigCostD3PD","| CURRENT  | PROCESSED | SKIPPED  | MEMORY     | CPU      | TIME PER   | APPROX. TIME |");
          Info("TrigCostD3PD","| EVENT    | EVENTS    | EVENTS   | USAGE [Mb] | TIME [s] | EVENT [ms] | LEFT [h:m]   |");
          Info("TrigCostD3PD","|----------|-----------|----------|------------|----------|------------|--------------|");
        }
        getrusage(RUSAGE_SELF, &_resources);
        Float_t _memoryUsage = (Float_t)_resources.ru_maxrss / 1024.;
        Float_t _timeSoFar = (Float_t)(_tProgress - _tStart)/CLOCKS_PER_SEC;
        Int_t _nEventsProcessedOrSkipped = _config.getInt(kEventsProcessed)+_config.getInt(kEventsSkipped);
        UInt_t _nEventsLeft =
          std::min(
            _config.getInt(kNEvents) - _nEventsProcessedOrSkipped,
            _config.getInt(kEventsInFiles) - _nEventsProcessedOrSkipped);
        UInt_t _minsLeft = 0;
        if (_nEventsProcessedOrSkipped) _minsLeft = (_nEventsLeft * ( _timeSoFar/(Float_t)_nEventsProcessedOrSkipped )) / 60.;
        UInt_t _hoursLeft = _minsLeft / 60;
        _minsLeft = _minsLeft % 60;
        Float_t _timePerEventLastFewSecs = 0;
        if (_nEventsProcessedOrSkipped - _eventsSinceLastPrint) {
          _timePerEventLastFewSecs = (_reportAfter/CLOCKS_PER_SEC)/(Float_t)(_nEventsProcessedOrSkipped - _eventsSinceLastPrint) * 1000.;
        }
        Info("TrigCostD3PD","| %-*lli | %-*d | %-*d | %-*.2f | %-*.2f | %-*.2f | %*d:%-*d |",
        8, _masterEvent,
        9, _config.getInt(kEventsProcessed),
        8, _config.getInt(kEventsSkipped),
        10, _memoryUsage,
        8, _timeSoFar,
        10, _timePerEventLastFewSecs,
        5, _hoursLeft, 6, _minsLeft);
        _eventsSinceLastPrint = _nEventsProcessedOrSkipped;
        // Save partial result
        std::ofstream _fout(_outputProgressFile.c_str());
        JsonExport _json;
        _json.addNode(_fout, "progress");
        _json.addLeafCustom(_fout, "EventsProcessed", intToString(_config.getInt(kEventsProcessed)) );
        _json.addLeafCustom(_fout, "EventsInFiles", intToString(_config.getInt(kEventsInFiles)) );
        _json.addLeafCustom(_fout, "HoursLeft", intToString(_hoursLeft) );
        _json.addLeafCustom(_fout, "MinsLeft", intToString(_minsLeft) );
        _json.endNode(_fout);
        _fout.close();
        _config.set(kWroteProgressFile, 1, "WroteProgressFile", kUnlocked);
      }

      if (_eventAccepted == kTRUE) {
        _config.increment(kEventsProcessed);
      } else {
        _config.increment(kEventsSkipped);
      }

      _timerLoop.stop();

      //Early exit from enough events or ctrl-C
      if (_config.getInt(kEventsProcessed) >= _config.getInt(kNEvents) || _terminateCalls) {
        break;
      }
    } // MassterEvent loop
  } // Pass loop

  ++_terminateCalls; // Such that a press of Crtl-C from this point on will terminate on the first issue.
  Info("TrigCostD3PD","|----------|-----------|----------|------------|----------|------------|--------------|");

  _tEnd = clock();
  Float_t _time = ((Float_t)_tEnd-(Float_t)_tStart)/CLOCKS_PER_SEC;
  getrusage(RUSAGE_SELF, &_resources);
  Info("TrigCostD3PD","End of Event Loop Processed:%i Skipped:%i", _config.getInt(kEventsProcessed), _config.getInt(kEventsSkipped));
  // Do breakdown?
  if (_config.getInt(kDoEBWeighting) != 0) {
    IntIntMap_t _bgMap = TrigXMLService::trigXMLService().getBGMap();
    IntIntMap_t _bgUbMap = TrigXMLService::trigXMLService().getBGUnbiasedMap();
    for (IntIntMapIt_t _it = _bgMap.begin(); _it != _bgMap.end(); ++_it) {
      Info("TrigCostD3PD","  Events from Bunch Group %s: %i (of which unbiased %i)",
        BunchGroupNameStr[ (*_it).first ].c_str(),
        (*_it).second,
        _bgUbMap[ (*_it).first ]);
    }
  }
  if (_config.getInt(kEventsProcessed) != 0) {
    Info("TrigCostD3PD","Program execution CPU wall time:%.2f s, Per-Event:%.2f ms", _time, ( _time/(Float_t)_config.getInt(kEventsProcessed) ) * 1000. );
  }
  Info("TrigCostD3PD","Program execution final memory footprint: %.2f Mb with %u histograms.",
    (Float_t)_resources.ru_maxrss / 1024.,
    DataVariable::s_globalHistId);

  ///  --- OUTPUT SECTION ---  ///
  _timerFinalise.start();

  if (_config.getInt(kWriteEBWeightXML) == kTRUE) TrigXMLService::trigXMLService().saveExportedEnhancedBiasXML();

  TFile* _outFile = 0;
  if (_config.getInt(kOutputRoot) == kTRUE) {
    const std::string _outputFileName =  _config.getStr(kOutputDirectory) + "/" + _config.getStr(kOutputRootFilename);
    _outFile = new TFile( _outputFileName.c_str() ,"UPDATE");
  }

  // Do output
  if (_config.getInt(kDoHLT) == kTRUE ) _processEventHLT->saveOutput();
  if (_config.getInt(kDoEF) == kTRUE ) _processEventEF->saveOutput();
  if (_config.getInt(kDoL2) == kTRUE ) _processEventL2->saveOutput();

  //LumiService::lumiService().saveOutput();
  _timerFinalise.stop();

  if (_config.getInt(kOutputRoot) == kTRUE) {
    _timerWrite.start();
    Info("TrigCostD3PD","Closing output ROOT file. Now at the mercy of ROOT I/O. Hold tight.");
    _outFile->Close();
    _timerWrite.stop();
  }

  // Save metadata
  if (_config.getInt(kDoNotWriteMetadata) == kFALSE) {
    // Update user details, fill in patterns
    if (_config.getIsSet(kUserDetails) == kTRUE) {
      std::string _userDetails = _config.getStr(kUserDetails);
      if ( _userDetails.find("%l") != std::string::npos ) { //If this string is present, replace it with N LB
        _userDetails.replace( _userDetails.find("%l"), 2, intToString( (Int_t) _lbProcessed.size() ) );
      }
      if ( _userDetails.find("%e") != std::string::npos ) { //If this string is present, replace it with n events
        _userDetails.replace( _userDetails.find("%e"), 2, floatToString( Float_t (_config.getInt(kEventsProcessed) * _effectivePrescale) ) );
      }
      _config.set(kUserDetails, _userDetails, "Details", kLocked);
    }

    const std::string _outputMetadata = _config.getStr(kOutputDirectory) + "/metadata.json";
    std::ofstream _fout(_outputMetadata.c_str());
    JsonExport _json;
    _json.addNode(_fout, "metadata");
    _json.addLeafCustom(_fout, "Date of Processing", _onlineTimeStr );
    Config::config().dumpToMeta(_fout, _json);
    // Get metadata from the last root file
    if (_config.getInt(kWriteEBWeightXML) == kFALSE) {
      for (UInt_t _m = 0; _m < TrigConfInterface::getMetaStringN(); ++_m) {
        std::string _metaKey = TrigConfInterface::getMetaStringKey(_m);
        //Info("TrigCostD3PD","Exporting metadata %s = %s", _metaKey.c_str(), TrigConfInterface::getMetaStringVal(_m).c_str());
        _json.addLeafCustom(_fout, _metaKey, TrigConfInterface::getMetaStringVal(_m));
      }
    }
    // Got metadata from the last root file
    std::string _CLI;
    for(Int_t _i = 0; _i < argc; ++_i) _CLI += std::string(argv[_i]) + std::string(" ");
    while (_CLI.find("\"") != std::string::npos) _CLI.replace( _CLI.find("\""), 1, "'"); // Only single quotes
    _json.addLeafCustom(_fout, "Command", _CLI);
    _json.endNode(_fout);
    _fout.close();
  }

  // Remove progress file
  if ( _config.getIsSet(kWroteProgressFile) == kTRUE) {
    const std::string _rmProg = std::string("rm ") + _outputProgressFile;
    Info("TrigCostD3PD","Remove progress file, execute: %s", _rmProg.c_str());
    gSystem->Exec( _rmProg.c_str() );
  }

  ///  --- OUTPUT SECTION ---  ///

  Timer::printAll();

  _config.messageSuppressionReport();

  // Cleanup
  TrigConfInterface::reset();

 if (_config.getInt(kSloppyExit) == kFALSE) {
    Info("TrigCostD3PD","Cleaning all counters and histograms from the heap before program terminates, this can be slow.");
    delete _processEventL2;
    delete _processEventEF;
    delete _processEventHLT;
  } else {
    // This is very naughty, but the memory will be bulk-freed upon return. Saves lots of time.
  }
  Info("TrigCostD3PD","Terminating");
  return 0;
}
