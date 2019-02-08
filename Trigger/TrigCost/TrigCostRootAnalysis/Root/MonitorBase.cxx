// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>
#include <iostream>
#include <sstream>
#include <utility> //std::pair
#include <iomanip> // std::setprecision
#include <assert.h>
#include <algorithm> //std::replace
#include <stdlib.h> //abort

//Local include(s):
#include "../TrigCostRootAnalysis/MonitorBase.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TableValueFunctions.h"

// ROOT include(s):
#include <TError.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TDirectory.h>
#include <TSystem.h>

namespace TrigCostRootAnalysis {
  /**
   * Constructor of pure virtual MonitorBase class.
   * Sets internal flags based on current configuration.
   * Called by derived implementation on construction.
   */
  MonitorBase::MonitorBase(const TrigCostData* costData, std::string name) :
    m_costData(costData),
    m_name(name),
    m_level(0),
    m_pass(1),
    m_detailLevel(10),
    m_dummyCounter(0),
    m_allowSameNamedCounters(kFALSE),
    m_allowSameIDCounters(kFALSE),
    m_filterOutput(kFALSE),
    m_filterOnDecorationKey(),
    m_filterOnDecorationValue(),
    m_counterCollections(),
    m_collectionsToProcess(),
    m_countersInEvent(),
    m_currentCollectionLBNumber(-1),
    m_currentDBKey(-1, -1, -1),
    m_timer("Monitor", name) {
    m_invertFilter = Config::config().getInt(kPatternsInvert);
    m_isCPUPrediction = (Bool_t) Config::config().getInt(kIsCPUPrediction);
    m_doKeySummary = (Bool_t) Config::config().getInt(kDoKeySummary);
    m_doLumiBlockSummary = (Bool_t) Config::config().getInt(kDoLumiBlockSummary);
    m_doAllSummary = (Bool_t) Config::config().getInt(kDoAllSummary);
    m_ratesOnly = (Bool_t) Config::config().getInt(kRatesOnly);
    m_allString = Config::config().getStr(kAllString);
    m_nLbFullSkip = Config::config().getInt(kNLbFullSkip);
    m_nHLTConfigSummary = Config::config().getInt(kNHLTConfigSummary);
    m_nLBPerHLTConfig = Config::config().getInt(kNLbPerHLTConfig);
    m_lumiBlockString = Config::config().getStr(kLumiBlockString);
  }

  /**
   * Base destructor for MonitorBase.
   * Deletes all stored counters.
   */
  MonitorBase::~MonitorBase() {
    // Loop over all collections
    if (Config::config().debug()) {
      Info("MonitorBase::~MonitorBase", "Destructing this monitor and all counters within %s L:%i",
           getName().c_str(), getLevel());
    }
    CounterCollectionIt_t colIt = m_counterCollections.begin();
    for (; colIt != m_counterCollections.end(); ++colIt) {
      // Loop over all counters in this counter map
      CounterMapIt_t counterIt = colIt->second.begin();
      for (; counterIt != colIt->second.end(); ++counterIt) {
        delete counterIt->second;
      }
    }
    m_counterCollections.clear();
    for (std::map<std::string, LumiCollector*>::iterator it = m_collectionLumiCollector.begin();
         it != m_collectionLumiCollector.end(); ++it) {
      delete it->second;
    }
    m_collectionLumiCollector.clear();
    delete m_dummyCounter;
  }

  /**
   * @ param pass Set which pass through the input files are we on.
   */
  void MonitorBase::setPass(UInt_t pass) {
    m_pass = pass;
  }

  /**
   * @return Which pass through the input files are we on.
   */
  UInt_t MonitorBase::getPass() {
    return m_pass;
  }

  /**
   * Virtual function, overloaded by at least the MonitorFullEvent
   * See how many CounterCollections are defined for this monitor to process for the current event.
   * This is based on LB number and keyset.
   * If all monitors report zero then event execution will be skipped.
   * @return Number of counter collections which will be processed for the current event
   */
  Int_t MonitorBase::getNCollectionsToProcess() {
    collateCounterCollectionsForEvent();
    return m_collectionsToProcess.size();
  }

  /**
   * Each event may fall into multiple monitoring catagories.
   * Eg, we may want to monitor it at the per lumi block level, the per trigger key leve, or the whole-run level.
   * This function caclulates which of these catagories are applicable for this event and populates a set which can
   * then be iterated over.
   */
  void MonitorBase::collateCounterCollectionsForEvent() {
    // Do we already have this calculated
    Float_t lumiLength = m_costData->getLumiLength();
    Int_t lumiBlockNumber = m_costData->getLumi();
    DBKey key = TrigConfInterface::getCurrentDBKey();

    if (m_currentCollectionLBNumber == lumiBlockNumber && m_currentDBKey == key) {
      // We do not need to recalculate the list of counter collections, but do still need to account for the lumi
      // bookkeeping
      for (std::set<std::string>::const_iterator it = m_collectionsToProcessNames.begin();
           it != m_collectionsToProcessNames.end(); ++it) {
        recordLumi(*it, lumiBlockNumber, lumiLength);
      }
      return;
    }

    m_currentCollectionLBNumber = lumiBlockNumber;
    m_currentDBKey.set(key.SMK(), key.L1PSK(), key.HLTPSK());

    // Clear the set of collections to iterate over this event
    m_collectionsToProcess.clear();
    m_collectionsToProcessNames.clear();

    // Add the "All" collection. Simplist, just runs over all the events. There may be a lot of events
    if (getIfActive(kDoAllSummary) && m_doAllSummary) {
      addToCollectionsToProcess(m_allString, lumiBlockNumber, lumiLength, kDoAllSummary);
    }

    //Active for this monitor?
    if (getIfActive(kDoLumiBlockSummary) && m_doLumiBlockSummary) {
      // Do we look at this lumi block on its own?
      static Int_t lbSummaryStart = INT_MIN, lbSummaryEnd = INT_MIN;

      // Have we set yet which is the "first" LB we are doing detailed monitoring for.
      if (Config::config().getIsSet(kFullSummaryStartLb) == kFALSE) {
        // No, OK, well if we have been told a starting lumi block for this run, then use this for the start of detailed
        // monitoring too.
        if (Config::config().getInt(kLumiStart) > INT_MIN) {
          Config::config().set(kFullSummaryStartLb, Config::config().getInt(kLumiStart), kLocked);
        } else {
          // No lumiStart either... OK let's start from this LB. It may not be the first - but it's our best guess.
          Config::config().set(kFullSummaryStartLb, lumiBlockNumber, kLocked);
        }
      }

      if (lbSummaryStart == INT_MIN) {
        lbSummaryStart = Config::config().getInt(kFullSummaryStartLb);
        lbSummaryEnd = lbSummaryStart +
                        ((Config::config().getInt(kNLbFullSummary) - 1) * Config::config().getInt(kNLbFullSkip));
        Info("MonitorBase::collateCounterCollectionsForEvent",
             "Setting per-LB monitoring from LB %i to %i (every %i LB)", (Int_t) lbSummaryStart, (Int_t) lbSummaryEnd,
             (Int_t) Config::config().getInt(kNLbFullSkip));
        assert(lbSummaryEnd > lbSummaryStart);
      }

      // Does the current lumiblock fall in the range?
      if ((Int_t) lumiBlockNumber >= lbSummaryStart && (Int_t) lumiBlockNumber <= lbSummaryEnd) {
        // Is it a multiple of NLbFullSkip?
        if ((lumiBlockNumber - lbSummaryStart) % m_nLbFullSkip == 0) {
          std::string LBIdentifier;
          std::ostringstream ss;
          ss << std::setfill('0') << std::setw(5) << lumiBlockNumber;
          LBIdentifier = m_lumiBlockString + std::string("_") + ss.str();
          addToCollectionsToProcess(LBIdentifier, lumiBlockNumber, lumiLength, kDoLumiBlockSummary);
        }
      }
    }

    // Are we providing a summary per keyset?
    if (getIfActive(kDoKeySummary) && m_doKeySummary) {
      Bool_t doKeySummaryDecision = kTRUE;

      if (!m_ratesOnly) { // Only apply extra logic if this is not "rates mode"
        if (m_perKeySummaries.count(key) == 0) { // Do we not have this summary in the books?
          if (m_perKeySummaries.size() < (size_t) m_nHLTConfigSummary) {
            // We have not yet reached the max number of key summaries.
            m_perKeySummaries.insert(key);
            m_perKeySummaryLBStart[ key ] = lumiBlockNumber;
          } else {
            doKeySummaryDecision = kFALSE;
          }
        } else { // seen this one before
          // Check LB range
          Int_t diff = lumiBlockNumber - m_perKeySummaryLBStart[ key ];
          if (diff < 0 || diff >= m_nLBPerHLTConfig) {
            doKeySummaryDecision = kFALSE;
          }
        }
      }

      if (doKeySummaryDecision == kTRUE) addToCollectionsToProcess(
          key.name(), lumiBlockNumber, lumiLength, kDoKeySummary);
    }

    // m_collectionsToProcess has been populated, return
    return;
  }

  /**
   * @return A count of the number of counters in all counter collections.
   */
  UInt_t MonitorBase::getNCounters() {
    CounterCollectionIt_t colIt = m_counterCollections.begin();
    UInt_t n = 0;

    for (; colIt != m_counterCollections.end(); ++colIt) {
      // Loop over all counters in this counter map
      n += colIt->second.size();
    }
    return n;
  }

  /**
   * Virtual. Overloaded by at least FullEvent monitor
   * @return if it is safe to run this monitor in parallel with other monitors. If not, will be run after
   */
  Bool_t MonitorBase::isThreadable() {
    return kTRUE;
  }

  /**
   * Adds the counters for this monitor to be processed in this event
   * @param name reference to the name of counter collection to add (will be created if does not exist)
   * @param lumiBlockNumber Current LB, used for bookkeeping for this CounterCollection
   * @param lumiLength Length of current LB, used for bookkeeping for this CounterCollection
   * @param type Conf key specifying the type of this CounterCollection which may be queried later.
   */
  void MonitorBase::addToCollectionsToProcess(const std::string& name, UInt_t lumiBlockNumber, Float_t lumiLength,
                                              const ConfKey_t type) {
    m_collectionsToProcess.insert(getCounterCollection(name, type));
    m_collectionsToProcessNames.insert(name);
    recordLumi(name, lumiBlockNumber, lumiLength);
  }

  /**
   * Keep track on the event lumi
   */
  void MonitorBase::recordLumi(const std::string& name, UInt_t lumiBlockNumber, Float_t lumiLength) {
    if (m_pass != 1) return; // Only do lumi on the first pass

    if (m_collectionLumiCollector.count(name) == 0) {
      m_collectionLumiCollector.insert(std::make_pair(name, new LumiCollector()));
    }
    m_collectionLumiCollector[ name ]->recordEventLumi(lumiBlockNumber, lumiLength);
  }

  /**
   * Sets the level of detail for this monitor to use, this is passed on to all counters which are created.
   * @param detailLevel Level of detail nominally in the range 0 - 10
   */
  void MonitorBase::setDetailLevel(UInt_t detailLevel) {
    m_detailLevel = detailLevel;
  }

  /**
   * Call to change monitor behaviour to always return a new counter on the getCounter call.
   * The default behaviour is to retrieve a Counter if one is registered with the same name, or create a new one
   *otherwise.
   */
  void MonitorBase::allowSameNamedCounters() {
    m_allowSameNamedCounters = kTRUE;
  }

  /**
   * Call to change monitor behaviour to not care if two counters are reg'd with the same IDs (the string, not the ID is
   *the actual key)
   */
  void MonitorBase::allowSameIDCounters() {
    m_allowSameIDCounters = kTRUE;
  }

  /**
   * Apply a check on a counter's string decoration and only export it if an exact match
   * @param decoration Key of string decoration to check
   * @param value Value of string to match exactly
   */
  void MonitorBase::filterOutputOnStrDecoration(ConfKey_t decoration, const std::string value) {
    m_filterOnDecorationValue = value;
    m_filterOnDecorationKey = decoration;
  }

  /**
   * Sends the startEvent call to one_ counter - assuming that only static counters need resetting.
   * If a counter map is optionlly suplied, the call is sent to all counters in the map
   * @param counters Optional map of counters to call startEvent on, if not supplied only the dummy counter is called.
   */
  void MonitorBase::startEvent(CounterMap_t* counters) {
    if (counters == 0 && m_dummyCounter != 0) {
      m_dummyCounter->startEvent();
    } else if (counters != 0) {
      CounterMapIt_t it = counters->begin();
      for (; it != counters->end(); ++it) {
        it->second->startEvent();
      }
    }
  }

  /**
   * Sends the endEvent call to all counters which have run in the event (denoted by inserting them into the set
   *m_countersInEvent)
   */
  void MonitorBase::endEvent(Float_t weight) {
    for (CounterSetIt_t it = m_countersInEvent.begin(); it != m_countersInEvent.end(); ++it) {
      // it is an iterator over a set of pointers to CounterBase objects
      (**it).endEvent(weight);
    }
    m_countersInEvent.clear();
  }

  /**
   * This must be inherited to be used
   */
  Bool_t MonitorBase::getIfActive(ConfKey_t /*mode*/) {
    Error("MonitorBase::getIfActive", "Function called on base class, you must impliment this in the derived class.");
    return kFALSE;
  }

  /**
   * Much of the code for writing histograms (and canvases) is shared between different monitors.
   * This code assumes a ROOT file has been opened for writing and uses gDirectory to navigate within it.
   * It will create directories and fill them with histograms or canvases.
   * If setup to do so, it can also mimic this hierarchy in the file system. Here, canvases are saved as either PDF or
   *PNG (or both).
   * Be warned, this can create thousands of files and directories if output is not slimmed using --chainPatternsOutput.
   * @param toSave a vector of pairs of <string name, VariableOption> where each pair corresponds to a histogram type
   *to export.
   */
  void MonitorBase::sharedHistogramOutputRoutine(VariableOptionVector_t& toSave) {
    Bool_t doRoot = Config::config().getInt(kOutputRoot);
    Bool_t doCanv = Config::config().getInt(kOutputCanvas);
    Bool_t doHist = Config::config().getInt(kOutputHist);
    Bool_t doImage = Config::config().getInt(kOutputImage);
    Bool_t doPng = Config::config().getInt(kOutputPng);
    Bool_t doPdf = Config::config().getInt(kOutputPdf);

    if (doRoot == kFALSE && doImage == kFALSE) return;

    Bool_t checkEachCounter = kFALSE;
    if (toSave.size() == 0) {
      checkEachCounter = kTRUE;
    }

    TCanvas* c = new TCanvas();
    c->SetLogy(kTRUE);
    c->SetBatch(kTRUE);

    const std::string outputFolder = Config::config().getStr(kOutputDirectory);
    const std::string imgFolder = Config::config().getStr(kOutputImageDirectory);

    // Loop over all counter collections
    CounterCollectionIt_t colIt = m_counterCollections.begin();
    for (; colIt != m_counterCollections.end(); ++colIt) {
      CounterMapIt_t mapIt = colIt->second.begin();
      std::string counterCollectionName = colIt->first;

      // For the counter collection
      disableROOTMsg();
      if (doRoot) gDirectory->mkdir(counterCollectionName.c_str());
      if (doRoot) gDirectory->cd(counterCollectionName.c_str());
      enableROOTMsg();

      // For the monitor
      if (doRoot) gDirectory->mkdir(std::string(getName() + "_" + getLevelStr()).c_str());
      if (doRoot) gDirectory->cd(std::string(getName() + "_" + getLevelStr()).c_str());

      for (; mapIt != colIt->second.end(); ++mapIt) {
        CounterBase* TCCB = mapIt->second;

        // Check if we are saving this
        if (m_filterOutput && checkPatternNameOutput(TCCB->getName(), m_invertFilter) == kFALSE) continue;

        // Check if there is any content
        if (TCCB->getValueExists(kVarCalls, kSavePerEvent) == kTRUE) {
          if (TCCB->getValue(kVarCalls, kSavePerEvent) == 0) continue;
        }

        const std::string outputPath = outputFolder + "/"
                                        + imgFolder + "/"
                                        + counterCollectionName + "/"
                                        + getName() + "_" + getLevelStr() + "/"
                                        + TCCB->getName();

        std::string outputName = TCCB->getName();
        checkForIllegalCharacters(outputName, /* , */ kTRUE, /* ' */ kTRUE, /* : */ kTRUE);

        if (checkEachCounter == kTRUE) { // Query each counter
          toSave = TCCB->getAllHistograms();
        }
        if (toSave.size() == 0) {
          //Info("MonitorBase::sharedHistogramOutputRoutine","No filled hist for %s", outputPath.c_str());
          continue; // Nothing to save here
        }

        if (doImage) gSystem->mkdir(outputPath.c_str(), kTRUE);
        if (doRoot) gDirectory->mkdir(outputName.c_str());
        if (doRoot) gDirectory->cd(outputName.c_str());

        for (UInt_t i = 0; i < toSave.size(); ++i) {
          const std::string plotName = constructPlotName(mapIt->second, toSave[i]);
          TH1F* h = TCCB->getHist(toSave[i]);

          if (h == 0 || h->GetEntries() == 0) {
            if (Config::config().debug()) {
              Info("MonitorBase::sharedHistogramOutputRoutine", "Skipping save of empty histogram %s",
                   plotName.c_str());
            }
            continue; // Do not save empty histos
          }

          plotHistogram(h);
          // Rename for better finding later
          h->SetName(std::string("h_" + plotName).c_str());
          plotText(0.20, 0.85, std::string(m_name + ": " + TCCB->getName()).c_str());

          disableROOTMsg();
          if (doPng) c->Print(std::string(outputPath + "/" + plotName + ".png").c_str());
          if (doPdf) c->Print(std::string(outputPath + "/" + plotName + ".pdf").c_str());
          enableROOTMsg();
          if (doCanv) c->Write(std::string("c_" + plotName).c_str());
          // Important - if we do not set the range here - the java web display will mess up
          Float_t min = h->GetBinContent(h->GetMinimumBin()) * 0.9;
          h->GetYaxis()->SetRangeUser(min, h->GetBinContent(h->GetMaximumBin()) * 1.1);
          // Float_t max = h->GetBinContent(h->GetMaximumBin());
          // Float_t min = h->GetBinContent(h->GetMaximumBin());
          // if (!isEqual(max, min) && !isZero(min) && min > 0 && (max/min) > 100. ) {
          //   c->SetLogy(kTRUE); // For large Y scales
          // }

          if (doHist) h->Write(std::string("h_" + plotName).c_str());
        }

        if (doRoot) gDirectory->cd("..");
      }

      if (doRoot) gDirectory->cd("..");
      if (doRoot) gDirectory->cd("..");
    }

    if (checkEachCounter == kTRUE) toSave.clear();
    delete c;
  }

  /**
   * This base code will output a .csv table from a set of counters. It with either fetch data directly from the
   *counter's data stores
   * with the option of some basic predefined post-processings specified by enums (FormatterOption).
   * Or for complicated/bespoke output it will supply the counter pointers to custom functions which callculate the
   *values to be saved in the table.
   * @see FormatterOption
   * @see VariableOption
   * @param toSave a vector of TableColumnFormatter objects which have been setup to give the desired table output.
   */
  void MonitorBase::sharedTableOutputRoutine(const std::vector<TableColumnFormatter>& toSave) {
    if (Config::config().getInt(kOutputCsv) == kFALSE) return;

    // Save tables. Loop over counter collections.
    CounterCollectionNonConstIt_t colIt = m_counterCollections.begin();
    for (; colIt != m_counterCollections.end(); ++colIt) {
      std::string counterCollectionName = colIt->first;
      CounterMap_t* counterMap = &(colIt->second);
      CounterMapIt_t counterMapIt = colIt->second.begin();

      // Skip if there are no counters to process
      if (counterMap->size() == 0) continue;
      // Skip if there are no counters passing decoration requirement
      if (m_filterOutput && m_filterOnDecorationValue != Config::config().getStr(kBlankString)) {
        Bool_t stuffToWrite = kFALSE;
        for (; counterMapIt != colIt->second.end(); ++counterMapIt) {
          if (counterMapIt->second->getStrDecoration(m_filterOnDecorationKey) == m_filterOnDecorationValue) {
            stuffToWrite = kTRUE;
            break;
          }
        }
        if (stuffToWrite == kFALSE) continue;
      }

      const std::string outputFolder = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(
        kOutputCSVDirectory);
      gSystem->mkdir(outputFolder.c_str(), kTRUE);

      const std::string tableName = outputFolder
                                     + "/Table_"
                                     + getName() + "_"
                                     + getLevelStr() + "_"
                                     + counterCollectionName
                                     + ".csv";
      std::ofstream fout;
      fout.open(tableName.c_str());
      fout << std::fixed; // Use fixed width output

      if (Config::config().debug()) {
        Info("MonitorBase::sharedTableOutputRoutine",
             "Doing table output with path %s, %u columns, %u (potential) rows.",
             tableName.c_str(), (UInt_t) toSave.size(), (UInt_t) colIt->second.size());
      }

      // Do the title line. First column is always the counter's name.
      fout << "Name,";
      for (UInt_t i = 0; i < toSave.size(); ++i) {
        checkForIllegalCharacters(toSave[i].m_columnName, /* , */ kTRUE, /* ' */ kTRUE, /* : */ kFALSE);
        fout << toSave[i].m_columnName;
        if (i != toSave.size() - 1) fout << ",";
        else fout << std::endl;
      }

      // Do the second line, this contains the tooltip descriptions
      fout << ",";
      for (UInt_t i = 0; i < toSave.size(); ++i) {
        checkForIllegalCharacters(toSave[i].m_tooltip, /* , */ kTRUE, /* ' */ kTRUE, /* : */ kFALSE);
        fout << toSave[i].m_tooltip;
        if (i != toSave.size() - 1) fout << ",";
        else fout << std::endl;
      }

      // Loop over all counters and add their row to the table.
      for (counterMapIt = colIt->second.begin(); counterMapIt != colIt->second.end(); ++counterMapIt) {
        CounterBase* TCCB = counterMapIt->second;
        outputTableRow(TCCB, fout, toSave, counterMap, counterCollectionName);
      }

      fout.close();
    }
  }

  /**
   * Function to sanitise text to be displayed on the website. The string is passed by reference and is modified if
   *problems are found.
   * Commas - , - are not allowed as the data are saved as a CSV file and this is the delimator. They are replaced with
   *spaces.
   * Single quotes - ' - are not allowed as these strings are echo'd into javascript fields. They are replaced with -
   *(not ideal).
   * @param toCheck String to parse for illegal characters. Note - this is modified.
   * @param checkComma True to replace comma's with spaces
   * @param checkApostrophe True to replace single quotes with hyphens
   * @param checkColon True to replace colons with underscrores
   */
  void MonitorBase::checkForIllegalCharacters(std::string& toClean, Bool_t checkComma, Bool_t checkApostrophe,
                                              Bool_t checkColon) {
    std::string before = toClean;
    if (checkComma && toClean.find(",") != std::string::npos) {
      std::replace(toClean.begin(), toClean.end(), ',', ' ');
      if (Config::config().getDisplayMsg(kMsgIllegalCharacters) == kTRUE) {
        Warning("MonitorBase::checkForIllegalCharacters",
                "Titles, tooltips and data are stored in CSV, they cannot contain ',' changing %s[%s -> %s]",
                getName().c_str(), before.c_str(), toClean.c_str());
      }
    }
    if (checkApostrophe && toClean.find("'") != std::string::npos) {
      std::replace(toClean.begin(), toClean.end(), '\'', '-');
      if (Config::config().getDisplayMsg(kMsgIllegalCharacters) == kTRUE) {
        Warning("MonitorBase::checkForIllegalCharacters",
                "Titles, tooltips and data cannot contain single quotes, messes up latter javascript, changing %s[%s -> %s]",
                getName().c_str(), before.c_str(), toClean.c_str());
      }
    }
    if (checkColon && toClean.find(":") != std::string::npos) {
      std::replace(toClean.begin(), toClean.end(), ':', '_');
      if (Config::config().getDisplayMsg(kMsgIllegalCharacters) == kTRUE) {
        Warning("MonitorBase::checkForIllegalCharacters", "TDirectories, cannot contain \":\" changing %s[%s -> %s]",
                getName().c_str(), before.c_str(), toClean.c_str());
      }
    }
  }

  /**
   * Function to actually extract, format and export a row
   * @see sharedTableOutputRoutine
   * @see FormatterOption
   * @see VariableOption
   * @param TCCB Pointer to counter object
   */
  void MonitorBase::outputTableRow(CounterBase* TCCB,
                                   std::ofstream& fout,
                                   const std::vector<TableColumnFormatter>& toSave,
                                   CounterMap_t* counterMap,
                                   std::string& counterCollectionName) {
    std::ios::fmtflags foutFlags(fout.flags());

    // Check if we are saving this
    if (m_filterOutput && checkPatternNameOutput(TCCB->getName(), m_invertFilter) == kFALSE) return;

    if (m_filterOutput && m_filterOnDecorationValue != Config::config().getStr(kBlankString)) {
      if (TCCB->getStrDecoration(m_filterOnDecorationKey) != m_filterOnDecorationValue) return;
    }

    // Check if there is any content
    if (TCCB->getValueExists(kVarCalls, kSavePerEvent) == kTRUE) {
      if (TCCB->getValue(kVarCalls, kSavePerEvent) == 0) return;
    }

    // Output name
    std::string rowName = TCCB->getName();
    checkForIllegalCharacters(rowName, /* , */ kTRUE, /* ' */ kTRUE, /* : */ kTRUE);
    fout << rowName << ",";
    // Output data
    for (UInt_t i = 0; i < toSave.size(); ++i) {
      std::string stringValue = "";
      Float_t value = 0., entries = 0.;

      // If using a Float_t function pointer
      if (toSave[i].m_functionPtr != 0) {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow", "\tDoing table cell %s via Float_t FnPtr %lu",
               toSave[i].m_columnName.c_str(),
               (ULong_t) toSave[i].m_functionPtr);
        }
        value = toSave[i].m_functionPtr(counterMap, TCCB); // Use this function pointer to get the value to save.
        // If using a std::string function pointer
      } else if (toSave[i].m_functionPtrStr != 0) {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow", "\tDoing table cell %s via std::string FnPtr %lu",
               toSave[i].m_columnName.c_str(),
               (ULong_t) toSave[i].m_functionPtrStr);
        }
        stringValue = toSave[i].m_functionPtrStr(counterMap, TCCB);
        // If using a std::string decoration
      } else if (toSave[i].m_formatOption == kFormatOptionUseStringDecoration) {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow", "\t\tDoing table cell %s via std::string decoration %s",
               toSave[i].m_columnName.c_str(),
               Config::config().getStr(toSave[i].m_dataVariable).c_str());
        }
        stringValue = TCCB->getStrDecoration(toSave[i].m_dataVariable);
        // If using Float_t decoration
      } else if (toSave[i].m_formatOption == kFormatOptionUseFloatDecoration) {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow", "\tDoing table cell %s via Float_t decoration %s",
               toSave[i].m_columnName.c_str(),
               Config::config().getStr(toSave[i].m_dataVariable).c_str());
        }
        value = TCCB->getDecoration(toSave[i].m_dataVariable);
        // If using Int_t decoration
      } else if (toSave[i].m_formatOption == kFormatOptionUseIntDecoration) {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow", "\tDoing table cell %s via Int_t decoration %s",
               toSave[i].m_columnName.c_str(),
               Config::config().getStr(toSave[i].m_dataVariable).c_str());
        }
        value = TCCB->getIntDecoration(toSave[i].m_dataVariable);
        // Otherwise do a direct fetch of stored data
      } else {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow", "\tDoing table cell %s via Float_t DataVariable %s",
               toSave[i].m_columnName.c_str(),
               Config::config().getStr(toSave[i].m_dataVariable).c_str());
        }
        value = TCCB->getValue(toSave[i].m_dataVariable, toSave[i].m_dataVO);
        entries = TCCB->getEntries(toSave[i].m_dataVariable, toSave[i].m_dataVO);

        // Check if we are supposed to divide through by a denominator
        if (toSave[i].m_dataVariableDenominator != kNoneString) {
          Float_t valueDenom = TCCB->getValue(toSave[i].m_dataVariableDenominator, toSave[i].m_dataVODenominator);
          if (isZero(valueDenom)) {
            value = 0.;
          } else {
            value /= valueDenom;
          }
        }
      }

      // See if there are any other modifiers requested via enum
      if (toSave[i].m_formatOption != kFormatOptionNone) {
        switch (toSave[i].m_formatOption) {
        case kFormatOptionNormaliseEntriesWallTime:
          value = entries;
          // NOTE: Explicit fall-through of case logic
          /* FALLTHROUGH */
        case kFormatOptionNormaliseWallTime:
          if (isZero(m_collectionLumiCollector[ counterCollectionName ]->getTotalLumiBlockTime())) {
            value = 0.;
          } else {
            value /= m_collectionLumiCollector[ counterCollectionName ]->getTotalLumiBlockTime();
          }
          break;

        case kFormatOptionNormaliseLBTimeDec:
          if (isZero(TCCB->getDecoration(kDecLbLength))) value = 0;
          else value /= TCCB->getDecoration(kDecLbLength);
          break;

        case kFormatOptionUseWallTime:
          value = m_collectionLumiCollector[ counterCollectionName ]->getTotalLumiBlockTime();
          break;

        case kFormatOptionNormaliseEntries:
          if (isZero(entries) == kTRUE) {
            value = 0.;
          } else {
            value /= entries;
          }
          break;

        case kFormatOptionMiliSecToSec:
          value /= 1000.;
          break;

        case kFormatOptionToPercentage:
          value *= 100.;
          break;

        case kFormatOptionUseEntries:
          // Overwrite the value field, we are after the number of entries rather than the value
          value = entries;
          break;

        // Deliberately ignored cases
        case kFormatOptionUseStringDecoration:
        case kFormatOptionUseFloatDecoration:
        case kFormatOptionUseIntDecoration:
          break;

        default:
          Error("MonitorBase::outputTableRow", "Table formatting option enum %i was not recognised.",
                toSave[i].m_formatOption);
          break;
        }
      }

      // Do the output
      if (stringValue != "") {
        checkForIllegalCharacters(stringValue, /* , */ kTRUE, /* ' */ kTRUE, /* : */ kFALSE);
        fout << stringValue;
      } else {
        fout << std::setprecision(toSave[i].m_precision) << value;
      }

      // Comma separate
      if (i != toSave.size() - 1) fout << ",";
    }

    fout << std::endl;
    fout.flags(foutFlags);
  }

  Bool_t MonitorBase::doesCounterCollectionExist(const std::string& identifier) {
    if (m_counterCollections.count(identifier) != 0) {
      return (Bool_t) true;
    }
    return (Bool_t) false;
  }

  /**
   * Get the collection of counters for a given identifier, create a new one if none currently exists.
   * @param identifier The name of the collection, "All" is used for the collection which runs on each event,
   *                    "LB:xxx" is used for individual lumi blocks, "SM:xxx_L1:xxx_HLT:xxx" for individual keysets.
   *                    Others can be added as and when needed.
   * @param type A key to specify what sort of counter collection this is rather than having to decude from the string
   * @return A pointer to the requested counter map.
   */
  CounterMap_t* MonitorBase::getCounterCollection(const std::string& identifier, const ConfKey_t type) {
    if (m_counterCollections.count(identifier) == 0) {
      m_counterCollections[identifier] = CounterMap_t();
      m_counterMapType[ &m_counterCollections[identifier] ] = type;
    }
    return &m_counterCollections[identifier];
  }

  //TIM M - INCOMPLETE! Get name of every range
  std::vector<std::string> MonitorBase::getCounterCollectionKeys() const {
    std::vector<std::string> toReturn;
    for (const auto& element : m_counterCollections) {
      toReturn.push_back(element.first);
    }
    return toReturn;
  }

  ConfKey_t MonitorBase::getCounterCollectionType(const std::string& identifier) {
    return m_counterMapType.at(&m_counterCollections.at(identifier));
  }

  /**
   * Request counter for a supplied name and ID. The name must be unique within this monitor.
   * If a counter with this name already exists, a base class pointer is returned.
   * If a counter with this name does not exist, a new one is first created before its pointer returned.
   * @param name Const reference to unique counter name (within this monitor)
   * @param ID Reference to the ID number for this counter.
   */
  CounterBase* MonitorBase::getCounter(CounterMap_t* counterMap, const std::string& name, Int_t ID) {
    CounterMapIt_t it = counterMap->find(name);

    // Do we have a counter for this string?
    if (it != counterMap->end()) {
      // Also check IDs match (unless this has been explicitly disabled due to having many ID's)
      if (m_allowSameIDCounters == kFALSE && it->second->getID() != ID) {
        // This is technically allowed by the architecture of the program, but we will warn the user anyway as they have
        // not explicitly said that it is OK to have many counters with the same ID
        Warning("MonitorBase::getCounter", "Name clash for %s with IDs %i and %i.",
                name.c_str(), ID, it->second->getID());
      }
      m_countersInEvent.insert(it->second);
      return it->second;
    } else {
      // If we don't yet then add a new one and return a reference
      CounterBase* TCCB = addCounter(counterMap, name, ID);
      m_countersInEvent.insert(TCCB);
      return TCCB;
    }
  }

  std::vector<std::string> MonitorBase::getRanges() {
    std::vector<std::string> result;
    for (auto coll_iter = m_counterCollections.begin(); coll_iter != m_counterCollections.end(); ++coll_iter) {
      result.push_back(coll_iter->first);
    }
    return result;
  }

  /**
   * Base class wrapper to handle the map related aspects of adding a new counter.
   * The newCounter call is pure virtual and must be implemented by the derived class such that
   * an object of the correct type is created then passed back as a polymorphic base class pointer.
   * @param name Const reference to the name of the new counter.
   * @param ID Reference to the ID number of the new counter.
   * @result Base class pointer to new counter.
   */
  CounterBase* MonitorBase::addCounter(CounterMap_t* counterMap, const std::string& name, Int_t ID) {
    CounterBase* tcc = newCounter(name, ID);

    // If we are after a new counter each call - then change the name (but *ONLY* in the map) to a simple number
    // such that it will never match a counter name upon a call to newCounter
    if (m_allowSameNamedCounters == kTRUE) {
      static Int_t counterNumber = 0;
      const std::string uniqueName = std::string("counter_" + intToString(counterNumber++));
      counterMap->insert(std::make_pair(uniqueName, tcc));
    } else {
      counterMap->insert(std::make_pair(name, tcc));
    }
    return tcc;
  }

  /**
   * Helper call to disable ROOT's messages temporarily to avoid flooding when saving lots of images.
   */
  void MonitorBase::disableROOTMsg() {
    gErrorIgnoreLevel = 10000; // Suppress ROOT messages
  }

  /**
   * Re-enable ROOT's messages, or at least restore the previous reporting level.
   */
  void MonitorBase::enableROOTMsg() {
    gErrorIgnoreLevel = Config::config().getInt(kErrorIgnore);
  }

  /*
   * Output function. Called at the end of the run. This virtual function should be overridden in the derived class
   * if there is actual output to save.
   */
  void MonitorBase::saveOutput() {
    Error("MonitorBase::saveOutput",
          "This monitor which saveOutput has been called on does not have any save functionality... yet");
  }

  /**
   * Helper function. For a given counter, and a given variable plus VariableOption pair, construct the name of the plot
   * to be used as the file name or the histogram name in any output.
   * @param counter Pointer to counter object, the name of the counter will be used.
   * @param variable Pair of variable name and VariableOption enum, used in constructiong plot name.
   * @returns Formatted plot name string.
   */
  std::string MonitorBase::constructPlotName(CounterBase* counter, ConfVariableOptionPair_t variable) {
    std::string name = counter->getName();
    name += "_";
    name += Config::config().getStr(variable.first); // What is being monitored (e.g. time, calls).
    name += "_";
    name += VariableOptionStr[ variable.second ]; // How it's being monitored (e.g. per event, per call)
    return name;
  }

  /**
   * Get name of this monitor.
   * @returns Const reference to monitor name.
   */
  const std::string& MonitorBase::getName() {
    return m_name;
  }

  /**
   * Get level this monitor is operating at.
   * @returns Monitor level.
   */
  UInt_t MonitorBase::getLevel() {
    return m_level;
  }

  /**
   * Get a string version of the level this monitor is operating at.
   * @returns Monitor level as string, "L2, "EF" or "HLT".
   */
  const std::string& MonitorBase::getLevelStr() {
    return getLevelString(getLevel());
  }

  /**
   * Sets the level this monitor is operating at.
   * @param l Sets the level.
   */
  void MonitorBase::setLevel(UInt_t l) {
    m_level = l;
  }

  /**
   * Sets the name of this monitor, used when formatting output.
   * @param name Sets the name.
   */
  void MonitorBase::setName(const std::string& name) {
    m_name = name;
  }

  /**
   * Construct a TableColumnFormatter to output a column of data from all counters.
   * This version will directly fetch a variable from the counter's storage.
   * @param title The title of the column in the table.
   * @param variableName The name of the variable saved in the counter to export for this column.
   * @param vo The corresponding variable option for this variable (saved per call, per event, etc.)
   * @param precision The number of decimal places to include for this column.
   */
  MonitorBase::TableColumnFormatter::TableColumnFormatter(const std::string& title,
                                                          const std::string& tooltip,
                                                          ConfKey_t variableName,
                                                          VariableOption_t vo,
                                                          UInt_t precision, FormatterOption_t fo) :
    m_columnName(title),
    m_tooltip(tooltip),
    m_dataVariable(variableName),
    m_dataVO(vo),
    m_dataVariableDenominator(kNoneString),
    m_dataVODenominator(),
    m_precision(precision),
    m_functionPtr(0),
    m_functionPtrStr(0),
    m_formatOption(fo) {}

  /**
   * Construct a TableColumnFormatter to output a column of data from all counters.
   * This version will directly fetch two variable from the counter's storage and divide one by the other.
   * @param title The title of the column in the table.
   * @param variableNameNominator The name of the variable saved in the counter to export for this column.
   * @param voNominator The corresponding variable option for this variable (saved per call, per event, etc.)
   * @param variableNameDenominator The name of the variable saved in the counter to use as denominator.
   * @param voDenominator The corresponding variable option for this denominator
   * @param precision The number of decimal places to include for this column.
   */
  MonitorBase::TableColumnFormatter::TableColumnFormatter(const std::string& title,
                                                          const std::string& tooltip,
                                                          ConfKey_t dataVarialbeNominator,
                                                          VariableOption_t voNominator,
                                                          ConfKey_t dataVarialbeDenominator,
                                                          VariableOption_t voDenominator,
                                                          UInt_t precision, FormatterOption_t fo) :
    m_columnName(title),
    m_tooltip(tooltip),
    m_dataVariable(dataVarialbeNominator),
    m_dataVO(voNominator),
    m_dataVariableDenominator(dataVarialbeDenominator),
    m_dataVODenominator(voDenominator),
    m_precision(precision),
    m_functionPtr(0),
    m_functionPtrStr(0),
    m_formatOption(fo) {}

  /**
   * Construct a TableColumnFormatter to output a column of data from all counters.
   * This version takes a function pointer which can do an arbitrary operation using the counter and return the value to
   *plot.
   * The CounterMap_t* input is considered optional. This is only needed when all counters are needed to calculate a
   *quantity, such as how much time a
   * chain used averaged over all chains.
   * @param title The title of the column in the table.
   * @param functionPrt Pointer to function which takes a counterMap* and CounterBase* and then returns a float value
   *to include in the column.
   * @param precision The number of decimal places to include for this column.
   */
  MonitorBase::TableColumnFormatter::TableColumnFormatter(const std::string& title,
                                                          const std::string& tooltip,
                                                          Float_t(*functionPtr)(CounterMap_t*, CounterBase*),
                                                          UInt_t precision) :
    m_columnName(title),
    m_tooltip(tooltip),
    m_dataVariable(),
    m_dataVO(),
    m_dataVariableDenominator(),
    m_dataVODenominator(),
    m_precision(precision),
    m_functionPtr(functionPtr),
    m_functionPtrStr(0),
    m_formatOption(kFormatOptionNone) {}

  /**
   * Construct a TableColumnFormatter to output a column of data from all counters.
   * This version takes a function pointer which can do an arbitrary operation using the counter and return a string to
   *plot.
   * The CounterMap_t* input is considered optional. This is only needed when all counters are needed to calculate a
   *quantity, such as how much time a
   * chain used averaged over all chains.
   * @param title The title of the column in the table.
   * @param functionPrtStr Pointer to function which takes a CounterMap_t* and CounterBase* and then returns a string
   *value to include in the column.
   */
  MonitorBase::TableColumnFormatter::TableColumnFormatter(const std::string& title,
                                                          const std::string& tooltip,
                                                          std::string(*functionPtrStr)(CounterMap_t*, CounterBase*)) :
    m_columnName(title),
    m_tooltip(tooltip),
    m_dataVariable(),
    m_dataVO(),
    m_dataVariableDenominator(),
    m_dataVODenominator(),
    m_precision(0),
    m_functionPtr(0),
    m_functionPtrStr(functionPtrStr),
    m_formatOption(kFormatOptionNone) {}




  CounterCollection_t* MonitorBase::getAllCounterCollections() {
    return &m_counterCollections;
  }
} // namespace TrigCostRootAnalysis
