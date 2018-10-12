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

//Automated Monitoring
#include "TrigCostRootAnalysis/AutoMonControl.h"


/**
 *
 * Main executable for TrigCostRootAnalysis package
 *
 */

// This function is to intercept Ctrl-C
UInt_t terminateCalls = 0;
void handlerFunctionTerm(Int_t parameter) {
  if (parameter == 2 && terminateCalls++ == 0) {
    Warning("TrigCostD3PD", "Received exit signal. Exiting event loop and writing output for events processed so far.");
  } else if (parameter == 2 && terminateCalls++ > 0) {
    Fatal("TrigCostD3PD", "Received another exit signal. Aborting now.");
    abort();
  }
}

using namespace TrigCostRootAnalysis;

int main(int argc, char* argv[]) {
  signal(SIGINT, handlerFunctionTerm);

  Timer timerSetup("Main", "Setup");
  Timer timerLoop("Main", "Event Loop");
  Timer timerFinalise("Main", "Finalise");
  Timer timerWrite("Main", "Writing");

  clock_t tStart, tEnd, tProgress;
  tStart = clock();
  timerSetup.start();

  SetTrigCostAtlasStyle();

  // Dictionary issues with EOS
  //gInterpreter->GenerateDictionary("vector<vector<float> >;vector<vector<vector<float> > >", "vector");

  Info("TrigCostD3PD", "Execute RunTrigCostD3PD");

  Config& config = Config::config();
  Bool_t isConfigured = config.parseCLI(argc, argv);

  if (isConfigured) {
    Info("TrigCostD3PD", "All program options parsed, preparing event loop.");
  } else {
    Error("TrigCostD3PD", "There were issues configuring this cost execution. Try --help. Aborting.");
    abort();
  }


  // Master chain of all files to analyse
  TChain chain(config.getStr(kTreeName).c_str());
  // Configuration tree chain

  // Add files to chain
  for (UInt_t f = 0; f < config.getVecSize(kInputFiles); ++f) {
    chain.Add(config.getVecEntry(kInputFiles, f).c_str());
  }

  // 'event' is very important, it's used pass-by-reference and dictates which entry in the tree we're reading from.
  Long64_t event = 0;

  // Create readers for cost branches and attach them to the TChain
  // These readers are wrapped in a data access class which provides a more intuitive interface.
  TrigCostData L2Data;
  TrigCostData EFData;
  TrigCostData HLTData; 
  

  // Need to be explicit here else coverity pipes up
  const Bool_t doL2 = config.getInt(kDoL2);
  const Bool_t doEF = config.getInt(kDoEF);
  const Bool_t doHLT = config.getInt(kDoHLT);

  if (doL2) L2Data.setup(event, config.getStr(kL2Prefix).c_str(), &chain);
  if (doEF) EFData.setup(event, config.getStr(kEFPrefix).c_str(), &chain);
  if (doHLT) HLTData.setup(event, config.getStr(kHLTPrefix).c_str(), &chain);

  // Create event processors. These hold individual monitors and distribute data in the event loop.
  ProcessEvent* processEventL2 = new ProcessEvent(&L2Data, 2, config.getStr(kL2String));
  ProcessEvent* processEventEF = new ProcessEvent(&EFData, 3, config.getStr(kEFString));
  ProcessEvent* processEventHLT = new ProcessEvent(&HLTData, 2, config.getStr(kHLTString));

  // Setup the monitors we'll be using within the ProcessEvents
  // Start at kAllMonitors - this will fire an internal loop in ProcessEvent if true to enable all monitors in the enum

  for (UInt_t mon = kDoAllMonitor; mon > kMonitorBegin; --mon) {
    ConfKey_t monKey = static_cast<ConfKey_t>(mon);
    if (doL2) processEventL2->setMonitoringMode(monKey, config.getInt(monKey));
    if (doEF) processEventEF->setMonitoringMode(monKey, config.getInt(monKey));
    if (doHLT) processEventHLT->setMonitoringMode(monKey, config.getInt(monKey));
    // If kAllMonitors was true we can stop here
    if (monKey == kDoAllMonitor && config.getInt(kDoAllMonitor) == kTRUE) break;
  }

  Long64_t eventsSinceLastPrint = 0, messageNumber = 0;
  UInt_t reportAfter = CLOCKS_PER_SEC * config.getInt(kMessageWait);
  time_t onlineTime = 0;
  Float_t effectivePrescale = 0.;
  std::set<UInt_t> lbProcessed;
  std::string onlineTimeStr;
  struct rusage resources;
  std::string outputProgressFile;
  const Bool_t writeEBWeightXML = config.getInt(kWriteEBWeightXML);
  const Int_t LBStart = config.getInt(kLumiStart);
  const Int_t LBEnd = config.getInt(kLumiEnd);
  // Begin event loop

  config.setLong(kEventsInFiles, chain.GetEntries(), "EventsInFiles");
  Info("D3PD", "Events in files: %lli", config.getLong(kEventsInFiles));
  if (!config.getLong(kEventsInFiles)) abort();

  tProgress = tStart;

  Int_t nPasses = config.getInt(kNPasses);
  for (Int_t pass = 1; pass <= nPasses; ++pass) {
    if (nPasses > 1) Info("TrigCostD3PD",
                           "|----------|---PASS----|--NUMBER--|-----%i------|----OF----|-----%i------|--------------|",
                           pass, nPasses);
    config.set(kCurrentPass, pass, "CurrentPass", kUnlocked);
    processEventL2->setPass(pass);
    processEventEF->setPass(pass);
    processEventHLT->setPass(pass);
    config.setLong(kEventsProcessed, 0, "EventsProcessed", kUnlocked);
    config.setLong(kEventsSkipped, 0, "EventsSkipped", kUnlocked);

    for (Long64_t masterEvent = 0; masterEvent < chain.GetEntries(); ++masterEvent) {
      // Load correct tree in chain into memory
      // Returns the local entry number within the tree
      // (D3PD reader doesn't distinguish between TTree or TChain)
      event = chain.LoadTree(masterEvent);
      if (event == 0) {
        Info("TrigCostD3PD", "Opening new file: %s", chain.GetFile()->GetName());
      }
      assert(event >= 0); // -1=Empty, -2=OutOfRange, -3=FileIOProblem, -4=TTreeMissing

      // If first event, perform configuration on loaded treesf
      if (masterEvent == 0 && pass == 1 && config.getInt(kWriteEBWeightXML) == kFALSE) {
        // Get the run number
        if (doL2 && config.getIsSet(kRunNumber) == kFALSE) {
          config.set(kRunNumber, L2Data.getRunNumber(), "RunNumber");
          onlineTime = L2Data.getCostRunSec();
        } else if (doEF && config.getIsSet(kRunNumber) == kFALSE) {
          config.set(kRunNumber, EFData.getRunNumber(), "RunNumber");
          onlineTime = EFData.getCostRunSec();
        } else if (doHLT && config.getIsSet(kRunNumber) == kFALSE) {
          config.set(kRunNumber, HLTData.getRunNumber(), "RunNumber");
          onlineTime = HLTData.getCostRunSec();
        }
        TrigXMLService::trigXMLService().init();
        struct tm* timeinfo = gmtime(&onlineTime);
        char buffer[80];
        strftime(buffer, 80, "%F %R UTC", timeinfo);
        onlineTimeStr = std::string(buffer);
        std::string outDirectory = config.getStr(kOutputDirectory);
        if (outDirectory.find("%r") != std::string::npos) { //If this string is present, replace it with the run number
          outDirectory.replace(outDirectory.find("%r"), 2, intToString(config.getInt(kRunNumber)));
        }
        if (outDirectory.find("%t") != std::string::npos) { //If this string is present, replace it with the tag
          outDirectory.replace(outDirectory.find("%t"), 2, config.getStr(kOutputTag));
        }
        config.set(kOutputDirectory, outDirectory, "OutputDirectory", kLocked); // Lock this time
        Info("TrigCostD3PD", "We are processing run %i, cost montioring has the timestamp %s", config.getInt(
               kRunNumber), onlineTimeStr.c_str());
        if (config.getInt(kCleanAll)) { // send rm command to get rid of current output directory
          std::string toCall = std::string("rm -r ") + config.getStr(kOutputDirectory) + std::string("/");
          Info("TrigCostD3PD", "Cleaning up from before, executing the command: '%s'", toCall.c_str());
          gSystem->Exec(toCall.c_str());
        }
        gSystem->mkdir(config.getStr(kOutputDirectory).c_str(), kTRUE);
        outputProgressFile = config.getStr(kOutputDirectory) + "/progress.json";
        // Do symlink
        if (config.getInt(kLinkOutputDir) == kTRUE) {
          std::string toCall = std::string("ln -sfn ") + config.getStr(kOutputDirectory) + std::string(" ") +
                                config.getStr(kLinkOutputDirName);
          Info("TrigCostD3PD", "Soft linking the output dir, executing the command: '%s'", toCall.c_str());
          gSystem->Exec(toCall.c_str());
        }
        // Setup the trigger configuration
        TrigConfInterface::configure(&chain);
        if (config.debug()) TrigConfInterface::debug();
        if (config.getInt(kWriteDummyPSXML) == kTRUE) {
          // Here the user has asked us to simple dump out the menu to a prescale XML file
          TrigXMLService::trigXMLService().writePrescaleXML();
          Info("TrigCostD3PD", "Written dummy XML, program will now terminate.");
          abort();
        }
        // Load the effective "prescale" that CostMon was run with online.
        Float_t doOperationalInfo = 0, execPrescale = 0.;
        std::string doOperationalInfoStr = TrigConfInterface::getMetaStringVal("doOperationalInfo");
        std::string execPrescaleStr = TrigConfInterface::getMetaStringVal("ExecPrescale");
        if (doOperationalInfoStr != config.getStr(kBlankString)) doOperationalInfo = stringToFloat(
            doOperationalInfoStr);
        if (execPrescaleStr != config.getStr(kBlankString)) execPrescale = stringToFloat(execPrescaleStr);
        effectivePrescale = doOperationalInfo * execPrescale;
        // This is for backward compatability for files without these data
        if (isZero(effectivePrescale) == kTRUE) effectivePrescale = 1.;
        config.setFloat(kEffectivePrescale, effectivePrescale, "EffectivePrescale");
        Float_t basicWeight = config.getFloat(kBasicEventWeight);
        Float_t lumiExtrapWeight = TrigXMLService::trigXMLService().getLumiExtrapWeight();
        Float_t eventWeight = basicWeight * effectivePrescale * lumiExtrapWeight;
        if (isZero(eventWeight - 1.) == kTRUE) { // Info if weight == 1
          Info("TrigCostD3PD",
               "Will apply global weight %f to all events (BasicWeight:%.2f, EffectiveCostPrescale:%.2f, LumiExtrapWeight:%.2f)",
               eventWeight, basicWeight, effectivePrescale, lumiExtrapWeight);
        } else { // If weight != 1, this should be bumped up to a warning
          Warning("TrigCostD3PD",
                  "Will apply global weight %f to all events (BasicWeight:%.2f, EffectiveCostPrescale:%.2f, LumiExtrapWeight:%.2f)",
                  eventWeight, basicWeight, effectivePrescale, lumiExtrapWeight);
        }
        config.setFloat(kEventWeight, eventWeight, "EventWeight");
        timerSetup.stop();
      }

      // Get trigger configuration for current event
      //TrigConfInterface::getEntry( event ); //not needed

      // Skip N events at beginning
      if (config.getLong(kEventsSkipped) < config.getLong(kNSkip)) {
        config.increment(kEventsSkipped);
        continue;
      }

      timerLoop.start();

      // Get event weight
      Float_t weight = 1;
      if (writeEBWeightXML == kFALSE) {
        weight = config.getFloat(kEventWeight); // This is our base weight that we can apply additional weights on top
                                                  // of
        TrigConfInterface::newEvent(HLTData.getLumi());       // Check / get menu configuration
      }

      Bool_t eventAccepted = kFALSE;

      // Execute L2 monitoring
      if (doL2 && !isZero(weight)) {
        // Check lumi block
        if (L2Data.getLumi() >= LBStart && L2Data.getLumi() <= LBEnd) {
          eventAccepted = processEventL2->newEvent(weight);
        }
      }

      // Execute EF monitoring
      if (doEF && !isZero(weight)) {
        // Check lumi block
        if (EFData.getLumi() >= LBStart && EFData.getLumi() <= LBEnd) {
          eventAccepted = processEventEF->newEvent(weight);
        }
      }

      // Execute HLT monitoring
      if (doHLT && !isZero(weight)) {
        if (writeEBWeightXML == kTRUE) {
          TrigXMLService::trigXMLService().exportEnhancedBiasXML(HLTData.getEventNumber(),
                                                                 HLTData.getEBWeight(),
            HLTData.getEBWeightBG(), HLTData.getEBUnbiased());
        } else if (HLTData.getLumi() >= LBStart && HLTData.getLumi() <= LBEnd &&
                   TrigXMLService::trigXMLService().getIsLBFlaggedBad(HLTData.getLumi()) == kFALSE) {
          lbProcessed.insert(HLTData.getLumi());
          eventAccepted = processEventHLT->newEvent(weight);
        }
      }

      if ((clock() - tProgress) >= reportAfter) {
        tProgress = clock();
        if (messageNumber++ % 20 == 0) {
          Info("TrigCostD3PD",
               "|----------|-----------|----------|------------|----------|------------|--------------|");
          Info("TrigCostD3PD",
               "| CURRENT  | PROCESSED | SKIPPED  | MEMORY     | CPU      | TIME PER   | APPROX. TIME |");
          Info("TrigCostD3PD",
               "| EVENT    | EVENTS    | EVENTS   | USAGE [Mb] | TIME [s] | EVENT [ms] | LEFT [h:m]   |");
          Info("TrigCostD3PD",
               "|----------|-----------|----------|------------|----------|------------|--------------|");
        }
        getrusage(RUSAGE_SELF, &resources);
        Float_t memoryUsage = (Float_t) resources.ru_maxrss / 1024.;
        Float_t timeSoFar = (Float_t) (tProgress - tStart) / CLOCKS_PER_SEC;
        Long64_t nEventsProcessedOrSkipped = config.getLong(kEventsProcessed) + config.getLong(kEventsSkipped);
        Long64_t nEventsLeft =
          std::min(
            config.getLong(kNEvents) - nEventsProcessedOrSkipped,
            config.getLong(kEventsInFiles) - nEventsProcessedOrSkipped);
        UInt_t minsLeft = 0;
        if (nEventsProcessedOrSkipped) minsLeft =
          (nEventsLeft * (timeSoFar / (Float_t) nEventsProcessedOrSkipped)) / 60.;
        UInt_t hoursLeft = minsLeft / 60;
        minsLeft = minsLeft % 60;
        Float_t timePerEventLastFewSecs = 0;
        if (nEventsProcessedOrSkipped - eventsSinceLastPrint) {
          timePerEventLastFewSecs = (reportAfter / CLOCKS_PER_SEC) /
          (Float_t) (nEventsProcessedOrSkipped - eventsSinceLastPrint) * 1000.;
        }
        Info("TrigCostD3PD", "| %-*lli | %-*lli | %-*lli | %-*.2f | %-*.2f | %-*.2f | %*d:%-*d |",
             8, masterEvent,
             9, config.getLong(kEventsProcessed),
             8, config.getLong(kEventsSkipped),
             10, memoryUsage,
             8, timeSoFar,
             10, timePerEventLastFewSecs,
             5, hoursLeft, 6, minsLeft);
        eventsSinceLastPrint = nEventsProcessedOrSkipped;
        // Save partial result
        std::ofstream fout(outputProgressFile.c_str());
        JsonExport json;
        json.addNode(fout, "progress");
        json.addLeafCustom(fout, "EventsProcessed", intToString(config.getLong(kEventsProcessed)));
        json.addLeafCustom(fout, "EventsInFiles", intToString(config.getLong(kEventsInFiles)));
        json.addLeafCustom(fout, "HoursLeft", intToString(hoursLeft));
        json.addLeafCustom(fout, "MinsLeft", intToString(minsLeft));
        json.endNode(fout);
        fout.close();
        config.set(kWroteProgressFile, 1, "WroteProgressFile", kUnlocked);
      }

      if (eventAccepted == kTRUE) {
        config.increment(kEventsProcessed);
      } else {
        config.increment(kEventsSkipped);
      }

      timerLoop.stop();

      //Early exit from enough events or ctrl-C
      if (config.getLong(kEventsProcessed) >= config.getLong(kNEvents) || terminateCalls) {
        break;
      }
    } // MasterEvent loop
  } // Pass loop

  ++terminateCalls; // Such that a press of Crtl-C from this point on will terminate on the first issue.
  Info("TrigCostD3PD", "|----------|-----------|----------|------------|----------|------------|--------------|");

  tEnd = clock();
  Float_t time = ((Float_t) tEnd - (Float_t) tStart) / CLOCKS_PER_SEC;
  getrusage(RUSAGE_SELF, &resources);
  Info("TrigCostD3PD", "End of Event Loop Processed:%lli Skipped:%lli", config.getLong(
    kEventsProcessed), config.getLong(kEventsSkipped));
  // Do breakdown?
  if (config.getInt(kDoEBWeighting) != 0) {
    IntIntMap_t bgMap = TrigXMLService::trigXMLService().getBGMap();
    IntIntMap_t bgUbMap = TrigXMLService::trigXMLService().getBGUnbiasedMap();
    for (IntIntMapIt_t it = bgMap.begin(); it != bgMap.end(); ++it) {
      Info("TrigCostD3PD", "  Events from Bunch Group %s: %i (of which unbiased %i)",
           BunchGroupNameStr[ (*it).first ].c_str(),
           (*it).second,
           bgUbMap[ (*it).first ]);
    }
  }
  if (config.getLong(kEventsProcessed) != 0) {
    Info("TrigCostD3PD", "Program execution CPU wall time:%.2f s, Per-Event:%.2f ms", time,
    (time / (Float_t) config.getLong(kEventsProcessed)) * 1000.);
  }
  Info("TrigCostD3PD", "Program execution final memory footprint: %.2f Mb with %u histograms.",
       (Float_t) resources.ru_maxrss / 1024.,
       DataVariable::s_globalHistId);

  ///  --- OUTPUT SECTION ---  ///
  timerFinalise.start();




  if (writeEBWeightXML == kTRUE) TrigXMLService::trigXMLService().saveExportedEnhancedBiasXML();

  TFile* outFile = 0;
  if (config.getInt(kOutputRoot) == kTRUE) {
    const std::string outputFileName = config.getStr(kOutputDirectory) + "/" + config.getStr(kOutputRootFilename);
    outFile = new TFile(outputFileName.c_str(), "UPDATE");
  }

  // Do output
  if (config.getInt(kDoHLT) == kTRUE) processEventHLT->saveOutput();
  if (config.getInt(kDoEF) == kTRUE) processEventEF->saveOutput();
  if (config.getInt(kDoL2) == kTRUE) processEventL2->saveOutput();

  //LumiService::lumiService().saveOutput();
  timerFinalise.stop();

  if (config.getInt(kOutputRoot) == kTRUE) {
    timerWrite.start();
    Info("TrigCostD3PD", "Closing output ROOT file. Now at the mercy of ROOT I/O. Hold tight.");
    outFile->Close();
    timerWrite.stop();
  }

  // Save metadata
  if (config.getInt(kDoNotWriteMetadata) == kFALSE) {
    // Update user details, fill in patterns
    if (config.getIsSet(kUserDetails) == kTRUE) {
      std::string userDetails = config.getStr(kUserDetails);
      if (userDetails.find("%l") != std::string::npos) { //If this string is present, replace it with N LB
        userDetails.replace(userDetails.find("%l"), 2, intToString((Int_t) lbProcessed.size()));
      }
      if (userDetails.find("%e") != std::string::npos) { //If this string is present, replace it with n events
        userDetails.replace(userDetails.find("%e"), 2,
        floatToString(Float_t(config.getLong(kEventsProcessed) * effectivePrescale)));
      }
      config.set(kUserDetails, userDetails, "Details", kLocked);
    }
    TrigConfInterface::populateLBPerKeysetStrings();

    const std::string outputMetadata = config.getStr(kOutputDirectory) + "/metadata.json";
    std::ofstream fout(outputMetadata.c_str());
    JsonExport json;
    json.addNode(fout, "metadata");
    json.addLeafCustom(fout, "Date of Processing", onlineTimeStr);
    Config::config().dumpToMeta(fout, json);
    // Get metadata from the last root file
    if (writeEBWeightXML == kFALSE) {
      for (UInt_t m = 0; m < TrigConfInterface::getMetaStringN(); ++m) {
        std::string metaKey = TrigConfInterface::getMetaStringKey(m);
        //Info("TrigCostD3PD","Exporting metadata %s = %s", metaKey.c_str(),
        // TrigConfInterface::getMetaStringVal(m).c_str());
        json.addLeafCustom(fout, metaKey, TrigConfInterface::getMetaStringVal(m));
      }
    }
    // Got metadata from the last root file
    std::string CLI;
    for (Int_t i = 0; i < argc; ++i) CLI += std::string(argv[i]) + std::string(" ");
    while (CLI.find("\"") != std::string::npos) CLI.replace(CLI.find("\""), 1, "'");                                                                                            // Only
                                                                                                                                                                                   // single
                                                                                                                                                                                   // quotes
    json.addLeafCustom(fout, "Command", CLI);
    json.endNode(fout);
    fout.close();
  }

  // Remove progress file
  if (config.getIsSet(kWroteProgressFile) == kTRUE) {
    const std::string rmProg = std::string("rm ") + outputProgressFile;
    Info("TrigCostD3PD", "Remove progress file, execute: %s", rmProg.c_str());
    gSystem->Exec(rmProg.c_str());
  }




  bool doAutoMon = true;

  if (doAutoMon) {
    std::cout << '\n';
    std::cout << "----------------------------------------------------------------------------------" << '\n';
    std::cout << '\n';

    std::cout << "Beginning Automated Cost Monitoring" << '\n';

    AutoMonControl auto_monitoring = AutoMonControl(Config::config().getStr(kAutoMonXMLPath).c_str(), processEventHLT);
    auto_monitoring.loadXml();
    auto_monitoring.runAutoMonitoring();
    std::cout << "Finished Automated Cost Monitoring" << '\n';

    std::cout << '\n';
    std::cout << "----------------------------------------------------------------------------------" << '\n';
    std::cout << '\n';
  }

  ///  --- OUTPUT SECTION ---  ///

  Timer::printAll();

  config.messageSuppressionReport();

  // Cleanup
  TrigConfInterface::reset();

  if (config.getInt(kSloppyExit) == kFALSE) {
    Info("TrigCostD3PD",
         "Cleaning all counters and histograms from the heap before program terminates, this can be slow.");
    delete processEventL2;
    delete processEventEF;
    delete processEventHLT;
  } else {
    // This is very naughty, but the memory will be bulk-freed upon return. Saves lots of time.
  }
  Info("TrigCostD3PD", "Terminating");
  return 0;
}
