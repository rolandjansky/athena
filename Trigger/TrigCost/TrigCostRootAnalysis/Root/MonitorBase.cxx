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
  MonitorBase::MonitorBase(const TrigCostData* _costData, std::string _name) :
    m_costData(_costData),
    m_name(_name),
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
    m_timer("Monitor",_name) {
    m_invertFilter = Config::config().getInt(kPatternsInvert);
    m_isCPUPrediction = (Bool_t) Config::config().getInt(kIsCPUPrediction);
  }

  /**
   * Base destructor for MonitorBase.
   * Deletes all stored counters.
   */
  MonitorBase::~MonitorBase() {
    // Loop over all collections
    if ( Config::config().debug() ) {
      Info("MonitorBase::~MonitorBase", "Destructing this monitor and all counters within %s L:%i", getName().c_str(), getLevel());
    }
    CounterCollectionIt_t _colIt = m_counterCollections.begin();
    for (; _colIt != m_counterCollections.end(); ++_colIt) {
      // Loop over all counters in this counter map
      CounterMapIt_t _counterIt = _colIt->second.begin();
      for (; _counterIt != _colIt->second.end(); ++_counterIt) {
        delete _counterIt->second;
      }
    }
    m_counterCollections.clear();
    for (std::map<std::string, LumiCollector*>::iterator _it = m_collectionLumiCollector.begin(); _it != m_collectionLumiCollector.end(); ++_it) {
      delete _it->second;
    }
    m_collectionLumiCollector.clear();
    delete m_dummyCounter;
  }

  /**
   * @ param _pass Set which pass through the input files are we on.
   */ 
  void MonitorBase::setPass(UInt_t _pass) {
    m_pass = _pass;
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
    Float_t _lumiLength = m_costData->getLumiLength();
    Int_t _lumiBlockNumber = m_costData->getLumi();
    DBKey _key = TrigConfInterface::getCurrentDBKey();

    if (m_currentCollectionLBNumber == _lumiBlockNumber && m_currentDBKey == _key) {
      // We do not need to recalculate the list of counter collections, but do still need to account for the lumi bookkeeping
      for (std::set<std::string>::const_iterator _it = m_collectionsToProcessNames.begin(); _it != m_collectionsToProcessNames.end(); ++_it) {
        recordLumi( *_it, _lumiBlockNumber, _lumiLength);
      }
      return;
    }

    m_currentCollectionLBNumber = _lumiBlockNumber;
    m_currentDBKey.set(_key.SMK(), _key.L1PSK(), _key.HLTPSK());

    // Clear the set of collections to iterate over this event
    m_collectionsToProcess.clear();
    m_collectionsToProcessNames.clear();

    // Add the "All" collection. Simplist, just runs over all the events. There may be a lot of events
    if ( Config::config().getInt(kDoAllSummary) && getIfActive(kDoAllSummary)) {
      addToCollectionsToProcess( Config::config().getStr(kAllString), _lumiBlockNumber, _lumiLength, kDoAllSummary );
    }

    //Active for this monitor?
    if ( Config::config().getInt(kDoLumiBlockSummary) && getIfActive(kDoLumiBlockSummary) ) {

      // Do we look at this lumi block on its own?
      static Int_t _lbSummaryStart = INT_MIN, _lbSummaryEnd = INT_MIN;

      // Have we set yet which is the "first" LB we are doing detailed monitoring for.
      if ( Config::config().getIsSet(kFullSummaryStartLb) == kFALSE  ) {
        // No, OK, well if we have been told a starting lumi block for this run, then use this for the start of detailed monitoring too.
        if ( Config::config().getInt(kLumiStart) > INT_MIN ) {
          Config::config().set(kFullSummaryStartLb, Config::config().getInt(kLumiStart), kLocked );
        } else {
          // No lumiStart either... OK let's start from this LB. It may not be the first - but it's our best guess.
          Config::config().set(kFullSummaryStartLb, _lumiBlockNumber, kLocked );
        }
      }

      if (_lbSummaryStart == INT_MIN) {
        _lbSummaryStart = Config::config().getInt(kFullSummaryStartLb);
        _lbSummaryEnd = _lbSummaryStart + ((Config::config().getInt(kNLbFullSummary) - 1) * Config::config().getInt(kNLbFullSkip));
        Info("MonitorBase::collateCounterCollectionsForEvent", "Setting per-LB monitoring from LB %i to %i (every %i LB)", (Int_t)_lbSummaryStart, (Int_t)_lbSummaryEnd, (Int_t)Config::config().getInt(kNLbFullSkip));
        assert(_lbSummaryEnd > _lbSummaryStart);
      }

      // Does the current lumiblock fall in the range?
      if ( (Int_t) _lumiBlockNumber >= _lbSummaryStart && (Int_t) _lumiBlockNumber <= _lbSummaryEnd ) {
        // Is it a multiple of NLbFullSkip?
        if ( (_lumiBlockNumber - _lbSummaryStart) % Config::config().getInt(kNLbFullSkip) == 0) {
          std::string _LBIdentifier;
          std::ostringstream _ss;
          _ss << std::setfill('0') << std::setw(5)  << _lumiBlockNumber;
          _LBIdentifier = Config::config().getStr(kLumiBlockString) + std::string("_") + _ss.str();
          addToCollectionsToProcess( _LBIdentifier, _lumiBlockNumber, _lumiLength, kDoLumiBlockSummary );
        }
      }
    }

    // Are we providing a summary per keyset?
    if ( Config::config().getInt(kDoKeySummary) && getIfActive(kDoKeySummary) ) {
      Bool_t _doKeySummaryDesicion = kTRUE;

      // Do we not have this summary in the books?
      if (m_perKeySummaries.count(_key) == 0) {

        if (m_perKeySummaries.size() < (size_t) Config::config().getInt(kNHLTConfigSummary)) {
          // We have not yet reached the max number of key summaries.
          m_perKeySummaries.insert( _key );
          m_perKeySummaryLBStart[ _key ] = _lumiBlockNumber;
        } else {
          _doKeySummaryDesicion = kFALSE;
        }

      } else { // seen this one before

        // Check LB range
        Int_t _diff = _lumiBlockNumber - m_perKeySummaryLBStart[ _key ];
        if (_diff < 0 || _diff >= Config::config().getInt(kNLbPerHLTConfig)) {
          _doKeySummaryDesicion = kFALSE;
        }

      }

      if (_doKeySummaryDesicion == kTRUE) addToCollectionsToProcess( _key.name(), _lumiBlockNumber, _lumiLength, kDoKeySummary );
    }

    // m_collectionsToProcess has been populated, return
    return;
  }

  /**
   * @return A count of the number of counters in all counter collections.
   */
  UInt_t MonitorBase::getNCounters() {
    CounterCollectionIt_t _colIt = m_counterCollections.begin();
    UInt_t _n = 0;
    for (; _colIt != m_counterCollections.end(); ++_colIt) {
      // Loop over all counters in this counter map
      _n += _colIt->second.size();
    }
    return _n;
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
   * @param _name reference to the name of counter collection to add (will be created if does not exist)
   * @param _lumiBlockNumber Current LB, used for bookkeeping for this CounterCollection
   * @param _lumiLength Length of current LB, used for bookkeeping for this CounterCollection
   * @param _type Conf key specifying the type of this CounterCollection which may be queried later.
   */
  void MonitorBase::addToCollectionsToProcess( const std::string &_name, UInt_t _lumiBlockNumber, Float_t _lumiLength, const ConfKey_t _type ) {
    m_collectionsToProcess.insert( getCounterCollection( _name, _type ) );
    m_collectionsToProcessNames.insert( _name );
    recordLumi(_name, _lumiBlockNumber, _lumiLength);
  }

  /**
   * Keep track on the event lumi
   */
  void MonitorBase::recordLumi( const std::string &_name, UInt_t _lumiBlockNumber, Float_t _lumiLength) {
    if (m_pass != 1) return; // Only do lumi on the first pass
    if ( m_collectionLumiCollector.count( _name ) == 0)  {
      m_collectionLumiCollector.insert( std::make_pair( _name, new LumiCollector()) );
    }
    m_collectionLumiCollector[ _name ]->recordEventLumi( _lumiBlockNumber, _lumiLength );
  }

  /**
   * Sets the level of detail for this monitor to use, this is passed on to all counters which are created.
   * @param _detailLevel Level of detail nominally in the range 0 - 10
   */
  void MonitorBase::setDetailLevel(UInt_t _detailLevel) {
    m_detailLevel = _detailLevel;
  }

  /**
   * Call to change monitor behaviour to always return a new counter on the getCounter call.
   * The default behaviour is to retrieve a Counter if one is registered with the same name, or create a new one otherwise.
   */
  void MonitorBase::allowSameNamedCounters() {
    m_allowSameNamedCounters = kTRUE;
  }

  /**
   * Call to change monitor behaviour to not care if two counters are reg'd with the same IDs (the string, not the ID is the actual key)
   */
  void MonitorBase::allowSameIDCounters() {
    m_allowSameIDCounters = kTRUE;
  }

  /**
   * Apply a check on a counter's string decoration and only export it if an exact match
   * @param _decoration Key of string decoration to check
   * @param _value Value of string to match exactly
   */
  void MonitorBase::filterOutputOnStrDecoration(ConfKey_t _decoration, const std::string _value) {
    m_filterOnDecorationValue = _value;
    m_filterOnDecorationKey = _decoration;
  }



  /**
   * Sends the startEvent call to _one_ counter - assuming that only static counters need resetting.
   * If a counter map is optionlly suplied, the call is sent to all counters in the map
   * @param _counters Optional map of counters to call startEvent on, if not supplied only the dummy counter is called.
   */
  void MonitorBase::startEvent(CounterMap_t* _counters) {
    if (_counters == 0 && m_dummyCounter != 0) {
      m_dummyCounter->startEvent();
    } else if (_counters != 0) {
      CounterMapIt_t _it = _counters->begin();
      for (; _it != _counters->end(); ++ _it) {
        _it->second->startEvent();
      }
    }
  }

  /**
   * Sends the endEvent call to all counters which have run in the event (denoted by inserting them into the set m_countersInEvent)
   */
  void MonitorBase::endEvent(Float_t _weight) {
    for (CounterSetIt_t _it = m_countersInEvent.begin(); _it != m_countersInEvent.end(); ++_it) {
      // _it is an iterator over a set of pointers to CounterBase objects
      (**_it).endEvent(_weight);
    }
    m_countersInEvent.clear();
  }

  /**
   * This must be inherited to be used
   */
  Bool_t MonitorBase::getIfActive(ConfKey_t _mode) {
    UNUSED(_mode);
    Error("MonitorBase::getIfActive", "Function called on base class, you must impliment this in the derived class.");
    return kFALSE;
  }


  /**
   * Much of the code for writing histograms (and canvases) is shared between different monitors.
   * This code assumes a ROOT file has been opened for writing and uses gDirectory to navigate within it.
   * It will create directories and fill them with histograms or canvases.
   * If setup to do so, it can also mimic this hierarchy in the file system. Here, canvases are saved as either PDF or PNG (or both).
   * Be warned, this can create thousands of files and directories if output is not slimmed using --chainPatternsOutput.
   * @param _toSave a vector of pairs of <string name, VariableOption> where each pair corresponds to a histogram type to export.
   */
  void MonitorBase::sharedHistogramOutputRoutine(VariableOptionVector_t& _toSave) {

    Bool_t _doRoot = Config::config().getInt(kOutputRoot);
    Bool_t _doCanv = Config::config().getInt(kOutputCanvas);
    Bool_t _doHist = Config::config().getInt(kOutputHist);
    Bool_t _doImage = Config::config().getInt(kOutputImage);
    Bool_t _doPng = Config::config().getInt(kOutputPng);
    Bool_t _doPdf = Config::config().getInt(kOutputPdf);

    if (_doRoot == kFALSE && _doImage == kFALSE) return;

    Bool_t _checkEachCounter = kFALSE;
    if (_toSave.size() == 0) {
      _checkEachCounter = kTRUE;
    }

    TCanvas* _c = new TCanvas();
    _c->SetLogy(kTRUE);
    _c->SetBatch(kTRUE);

    const std::string _outputFolder = Config::config().getStr(kOutputDirectory);
    const std::string _imgFolder = Config::config().getStr(kOutputImageDirectory);

    // Loop over all counter collections
    CounterCollectionIt_t _colIt = m_counterCollections.begin();
    for (; _colIt != m_counterCollections.end(); ++_colIt) {
      CounterMapIt_t _mapIt = _colIt->second.begin();
      std::string _counterCollectionName = _colIt->first;

      // For the counter collection
      disableROOTMsg();
      if (_doRoot) gDirectory->mkdir( _counterCollectionName.c_str() );
      if (_doRoot) gDirectory->cd   ( _counterCollectionName.c_str() );
      enableROOTMsg();

      // For the monitor
      if (_doRoot) gDirectory->mkdir( std::string(getName() + "_" + getLevelStr()).c_str() );
      if (_doRoot) gDirectory->cd   ( std::string(getName() + "_" + getLevelStr()).c_str() );

      for (; _mapIt != _colIt->second.end(); ++_mapIt) {

        CounterBase* _TCCB =  _mapIt->second;

        // Check if we are saving this
        if ( m_filterOutput && checkPatternNameOutput( _TCCB->getName(), m_invertFilter ) == kFALSE ) continue;

        // Check if there is any content
        if ( _TCCB->getValueExists(kVarCalls, kSavePerEvent) == kTRUE ) {
          if ( _TCCB->getValue(kVarCalls, kSavePerEvent) == 0 ) continue;
        }

        const std::string _outputPath = _outputFolder + "/"
          + _imgFolder + "/"
          + _counterCollectionName + "/"
          + getName() + "_" + getLevelStr() + "/"
          + _TCCB->getName();

        std::string _outputName = _TCCB->getName();
        checkForIllegalCharacters(_outputName, /* , */ kTRUE, /* ' */ kTRUE, /* : */ kTRUE);

        if (_checkEachCounter == kTRUE) { // Query each counter
          _toSave = _TCCB->getAllHistograms();
        }
        if (_toSave.size() == 0) {
          //Info("MonitorBase::sharedHistogramOutputRoutine","No filled hist for %s", _outputPath.c_str());
          continue; // Nothing to save here
        }

        if (_doImage) gSystem->mkdir( _outputPath.c_str(), kTRUE);
        if (_doRoot) gDirectory->mkdir( _outputName.c_str() );
        if (_doRoot) gDirectory->cd   ( _outputName.c_str() );

        for (UInt_t _i = 0; _i < _toSave.size(); ++_i) {

          const std::string _plotName = constructPlotName(_mapIt->second, _toSave[_i]);
          TH1F* _h = _TCCB->getHist(_toSave[_i]);

          if (_h == 0 || _h->GetEntries() == 0) {
            if (Config::config().debug()) {
              Info("MonitorBase::sharedHistogramOutputRoutine","Skiping save of empty histogram %s", _plotName.c_str());
            }
            continue; // Do not save empty histos
          }

          plotHistogram( _h );
          // Rename for better finding later
          _h->SetName( std::string( "h_" + _plotName).c_str() );
          plotText(0.20, 0.85, std::string(m_name + ": " + _TCCB->getName()).c_str() );

          disableROOTMsg();
          if (_doPng) _c->Print(std::string(_outputPath + "/" + _plotName + ".png").c_str());
          if (_doPdf) _c->Print(std::string(_outputPath + "/" + _plotName + ".pdf").c_str());
          enableROOTMsg();
          if (_doCanv) _c->Write( std::string( "c_" + _plotName).c_str() );
          // Important - if we do not set the range here - the java web display will mess up
          Float_t _min = _h->GetBinContent(_h->GetMinimumBin()) * 0.9;
          _h->GetYaxis()->SetRangeUser(_min, _h->GetBinContent(_h->GetMaximumBin()) * 1.1);
          // Float_t _max = _h->GetBinContent(h->GetMaximumBin());
          // Float_t _min = _h->GetBinContent(h->GetMaximumBin());
          // if (!isEqual(_max, _min) && !isZero(_min) && _min > 0 && (_max/_min) > 100. ) {
          //   _c->SetLogy(kTRUE); // For large Y scales
          // }

          if (_doHist) _h->Write( std::string( "h_" + _plotName).c_str() );
        }

        if (_doRoot) gDirectory->cd( ".." );
      }

      if (_doRoot) gDirectory->cd( ".." );
      if (_doRoot) gDirectory->cd( ".." );

    }

    if (_checkEachCounter == kTRUE) _toSave.clear();
    delete _c;
  }

  /**
   * This base code will output a .csv table from a set of counters. It with either fetch data directly from the counter's data stores
   * with the option of some basic predefined post-processings specified by enums (FormatterOption).
   * Or for complicated/bespoke output it will supply the counter pointers to custom functions which callculate the values to be saved in the table.
   * @see FormatterOption
   * @see VariableOption
   * @param _toSave a vector of TableColumnFormatter objects which have been setup to give the desired table output.
   */
  void MonitorBase::sharedTableOutputRoutine( const std::vector<TableColumnFormatter>& _toSave ) {

    if (Config::config().getInt(kOutputCsv) == kFALSE) return;

    // Save tables. Loop over counter collections.
    CounterCollectionNonConstIt_t _colIt = m_counterCollections.begin();
    for (; _colIt != m_counterCollections.end(); ++_colIt) {
      std::string _counterCollectionName = _colIt->first;
      CounterMap_t* _counterMap = &(_colIt->second);
      CounterMapIt_t _counterMapIt = _colIt->second.begin();

      // Skip if there are no counters to process
      if ( _counterMap->size() == 0) continue;
      // Skip if there are no counters passing decoration requirement
      if ( m_filterOutput && m_filterOnDecorationValue != Config::config().getStr(kBlankString) ) {
        Bool_t _stuffToWrite = kFALSE;
        for (; _counterMapIt != _colIt->second.end(); ++_counterMapIt) {
          if( _counterMapIt->second->getStrDecoration(m_filterOnDecorationKey) == m_filterOnDecorationValue ) {
            _stuffToWrite = kTRUE;
            break;
          }
        }
        if (_stuffToWrite == kFALSE) continue;
      }

      const std::string _outputFolder = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(kOutputCSVDirectory);
      gSystem->mkdir( _outputFolder.c_str(), kTRUE);

      const std::string _tableName = _outputFolder
        + "/Table_"
        + getName() + "_"
        + getLevelStr() + "_"
        + _counterCollectionName
        + ".csv";
      std::ofstream _fout;
      _fout.open( _tableName.c_str() );
      _fout << std::fixed; // Use fixed width output

      if (Config::config().debug()) {
        Info("MonitorBase::sharedTableOutputRoutine","Doing table output with path %s, %u columns, %u (potential) rows.",
          _tableName.c_str(), (UInt_t) _toSave.size(), (UInt_t) _colIt->second.size());
      }

      // Do the title line. First column is always the counter's name.
      _fout << "Name,";
      for (UInt_t _i = 0; _i < _toSave.size(); ++_i) {
        checkForIllegalCharacters(_toSave[_i].m_columnName, /* , */ kTRUE, /* ' */ kTRUE, /* : */ kFALSE);
        _fout << _toSave[_i].m_columnName;
        if (_i != _toSave.size() - 1) _fout << ",";
        else _fout << std::endl;
      }

      // Do the second line, this contains the tooltip descriptions
      _fout << ",";
      for (UInt_t _i = 0; _i < _toSave.size(); ++_i) {
        checkForIllegalCharacters(_toSave[_i].m_tooltip, /* , */ kTRUE, /* ' */ kTRUE, /* : */ kFALSE);
        _fout << _toSave[_i].m_tooltip;
        if (_i != _toSave.size() - 1) _fout << ",";
        else _fout << std::endl;
      }

      // Loop over all counters and add their row to the table.
      for (_counterMapIt = _colIt->second.begin(); _counterMapIt != _colIt->second.end(); ++_counterMapIt) {
        CounterBase* _TCCB = _counterMapIt->second;
        outputTableRow(_TCCB, _fout, _toSave, _counterMap, _counterCollectionName);
      }

      _fout.close();
    }
  }

  /**
   * Function to sanitise text to be displayed on the website. The string is passed by reference and is modified if problems are found.
   * Commas - , - are not allowed as the data are saved as a CSV file and this is the delimator. They are replaced with spaces.
   * Single quotes - ' - are not allowed as these strings are echo'd into javascript fields. They are replaced with - (not ideal).
   * @param _toCheck String to parse for illegal characters. Note - this is modified.
   * @param _checkComma True to replace comma's with spaces
   * @param _checkApostrophe True to replace single quotes with hyphens
   * @param _checkColon True to replace colons with underscrores
   */
  void MonitorBase::checkForIllegalCharacters(std::string &_toClean, Bool_t _checkComma, Bool_t _checkApostrophe, Bool_t _checkColon) {
    std::string _before = _toClean;
    if (_checkComma && _toClean.find(",") != std::string::npos) {
      std::replace( _toClean.begin(), _toClean.end(), ',', ' ');
      if (Config::config().getDisplayMsg(kMsgIllegalCharacters) == kTRUE) {
        Warning("MonitorBase::checkForIllegalCharacters","Titles, tooltips and data are stored in CSV, they cannot contain ',' changing %s[%s -> %s]",
          getName().c_str(), _before.c_str(), _toClean.c_str());
      }
    }
    if (_checkApostrophe && _toClean.find("'") != std::string::npos) {
      std::replace( _toClean.begin(), _toClean.end(), '\'', '-');
      if (Config::config().getDisplayMsg(kMsgIllegalCharacters) == kTRUE) {
        Warning("MonitorBase::checkForIllegalCharacters","Titles, tooltips and data cannot contain single quotes, messes up latter javascript, changing %s[%s -> %s]",
          getName().c_str(), _before.c_str(), _toClean.c_str());
      }
    }
    if (_checkColon && _toClean.find(":") != std::string::npos) {
      std::replace( _toClean.begin(), _toClean.end(), ':', '_');
      if (Config::config().getDisplayMsg(kMsgIllegalCharacters) == kTRUE) {
        Warning("MonitorBase::checkForIllegalCharacters","TDirectories, cannot contain \":\" changing %s[%s -> %s]",
          getName().c_str(), _before.c_str(), _toClean.c_str());
      }
    }
  }

  /**
   * Function to actually extract, format and export a row
   * @see sharedTableOutputRoutine
   * @see FormatterOption
   * @see VariableOption
   * @param _TCCB Pointer to counter object
   */
  void MonitorBase::outputTableRow(CounterBase* _TCCB,
    std::ofstream& _fout,
    const std::vector<TableColumnFormatter>& _toSave,
    CounterMap_t* _counterMap,
    std::string &_counterCollectionName) {

    std::ios::fmtflags _foutFlags( _fout.flags() );

    // Check if we are saving this
    if ( m_filterOutput && checkPatternNameOutput( _TCCB->getName(), m_invertFilter ) == kFALSE ) return;

    if ( m_filterOutput && m_filterOnDecorationValue != Config::config().getStr(kBlankString) ) {
      if ( _TCCB->getStrDecoration(m_filterOnDecorationKey) != m_filterOnDecorationValue ) return;
    }

    // Check if there is any content
    if ( _TCCB->getValueExists(kVarCalls, kSavePerEvent) == kTRUE ) {
      if ( _TCCB->getValue(kVarCalls, kSavePerEvent) == 0 ) return;
    }

    // Output name
    std::string _rowName = _TCCB->getName();
    checkForIllegalCharacters(_rowName, /* , */ kTRUE, /* ' */ kTRUE, /* : */ kTRUE);
    _fout << _rowName << ",";
    // Output data
    for (UInt_t _i = 0; _i < _toSave.size(); ++_i) {

      std::string _stringValue = "";
      Float_t _value = 0., _entries = 0.;

      // If using a Float_t function pointer
      if (_toSave[_i].m_functionPtr != 0) {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow","\tDoing table cell %s via Float_t FnPtr %lu",
            _toSave[_i].m_columnName.c_str(),
            (ULong_t) _toSave[_i].m_functionPtr );
        }
        _value = _toSave[_i].m_functionPtr( _counterMap, _TCCB ); // Use this function pointer to get the value to save.
      // If using a std::string function pointer
      } else if (_toSave[_i].m_functionPtrStr != 0) {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow","\tDoing table cell %s via std::string FnPtr %lu",
            _toSave[_i].m_columnName.c_str(),
            (ULong_t) _toSave[_i].m_functionPtrStr );
        }
        _stringValue = _toSave[_i].m_functionPtrStr( _counterMap, _TCCB );
      // If using a std::string decoration
      } else if (_toSave[_i].m_formatOption == kFormatOptionUseStringDecoration) {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow","\t\tDoing table cell %s via std::string decoration %s",
            _toSave[_i].m_columnName.c_str(),
            Config::config().getStr(_toSave[_i].m_dataVariable).c_str() );
        }
        _stringValue = _TCCB->getStrDecoration( _toSave[_i].m_dataVariable );
      // If using Float_t decoration
      } else if ( _toSave[_i].m_formatOption == kFormatOptionUseFloatDecoration ) {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow","\tDoing table cell %s via Float_t decoration %s",
            _toSave[_i].m_columnName.c_str(),
            Config::config().getStr(_toSave[_i].m_dataVariable).c_str() );
        }
        _value =  _TCCB->getDecoration( _toSave[_i].m_dataVariable );
      // If using Int_t decoration
      } else if ( _toSave[_i].m_formatOption == kFormatOptionUseIntDecoration ) {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow","\tDoing table cell %s via Int_t decoration %s",
            _toSave[_i].m_columnName.c_str(),
            Config::config().getStr(_toSave[_i].m_dataVariable).c_str() );
        }
        _value =  _TCCB->getIntDecoration( _toSave[_i].m_dataVariable );
      // Otherwise do a direct fetch of stored data
      } else {
        if (Config::config().debug()) {
          Info("MonitorBase::outputTableRow","\tDoing table cell %s via Float_t DataVariable %s",
            _toSave[_i].m_columnName.c_str(),
            Config::config().getStr(_toSave[_i].m_dataVariable).c_str() );
        }
        _value = _TCCB->getValue( _toSave[_i].m_dataVariable, _toSave[_i].m_dataVO );
        _entries = _TCCB->getEntries( _toSave[_i].m_dataVariable, _toSave[_i].m_dataVO );

        // Check if we are supposed to divide through by a denominator
        if (_toSave[_i].m_dataVariableDenominator != kNoneString) {
          Float_t _valueDenom = _TCCB->getValue( _toSave[_i].m_dataVariableDenominator, _toSave[_i].m_dataVODenominator );
          if (isZero(_valueDenom)) {
            _value = 0.;
          } else {
            _value /= _valueDenom;
          }
        }
      }

      // See if there are any other modifiers requested via enum
      if (_toSave[_i].m_formatOption != kFormatOptionNone) {
        switch (_toSave[_i].m_formatOption) {
          case kFormatOptionNormaliseEntriesWallTime:
            _value = _entries;
            // NOTE: Explicit fall-through of case logic
          case kFormatOptionNormaliseWallTime:
            if (isZero( m_collectionLumiCollector[ _counterCollectionName ]->getTotalLumiBlockTime() )) {
              _value = 0.;
            } else {
              _value /= m_collectionLumiCollector[ _counterCollectionName ]->getTotalLumiBlockTime();
            }
            break;
          case kFormatOptionNormaliseLBTimeDec:
            if ( isZero(_TCCB->getDecoration(kDecLbLength)) ) _value = 0;
            else _value /= _TCCB->getDecoration(kDecLbLength);
            break;
          case kFormatOptionUseWallTime:
            _value = m_collectionLumiCollector[ _counterCollectionName ]->getTotalLumiBlockTime();
            break;
          case kFormatOptionNormaliseEntries:
            if ( isZero(_entries) == kTRUE ) {
              _value = 0.;
            } else {
              _value /= _entries;
            }
            break;
          case kFormatOptionMiliSecToSec:
            _value /= 1000.;
            break;
          case kFormatOptionToPercentage:
            _value *= 100.;
            break;
          case kFormatOptionUseEntries:
            // Overwrite the value field, we are after the number of entries rather than the value
            _value = _entries;
            break;
          // Deliberately ignored cases
          case kFormatOptionUseStringDecoration: case kFormatOptionUseFloatDecoration: case kFormatOptionUseIntDecoration:
            break;
          default:
            Error("MonitorBase::outputTableRow", "Table formatting option enum %i was not recognised.", _toSave[_i].m_formatOption);
            break;
        }
      }

      // Do the output
      if (_stringValue != "") {
        checkForIllegalCharacters(_stringValue, /* , */ kTRUE, /* ' */ kTRUE, /* : */ kFALSE);
        _fout << _stringValue;
      } else {
        _fout << std::setprecision( _toSave[_i].m_precision ) << _value;
      }

      // Comma separate
      if (_i != _toSave.size() - 1) _fout << ",";
    }

    _fout << std::endl;
    _fout.flags( _foutFlags );
  }

  /**
   * Get the collection of counters for a given identifier, create a new one if none currently exists.
   * @param _identifier The name of the collection, "All" is used for the collection which runs on each event,
   *                    "LB:xxx" is used for individual lumi blocks, "SM:xxx_L1:xxx_HLT:xxx" for individual keysets.
   *                    Others can be added as and when needed.
   * @param _type A key to specify what sort of counter collection this is rather than having to decude from the string
   * @return A pointer to the requested counter map.
   */
  CounterMap_t* MonitorBase::getCounterCollection(const std::string& _identifier, const ConfKey_t _type) {
    if ( m_counterCollections.count(_identifier) == 0) {
      m_counterCollections[_identifier] = CounterMap_t();
      m_counterMapType[ &m_counterCollections[_identifier] ] = _type;
    }
    return &m_counterCollections[_identifier];
  }


  /**
   * Request counter for a supplied name and ID. The name must be unique within this monitor.
   * If a counter with this name already exists, a base class pointer is returned.
   * If a counter with this name does not exist, a new one is first created before its pointer returned.
   * @param _name Const reference to unique counter name (within this monitor)
   * @param _ID Reference to the ID number for this counter.
   */
  CounterBase* MonitorBase::getCounter( CounterMap_t* _counterMap, const std::string &_name, Int_t _ID ) {
    CounterMapIt_t _it = _counterMap->find( _name );
    // Do we have a counter for this string?
    if ( _it != _counterMap->end() ) {
      // Also check IDs match (unless this has been explicitly disabled due to having many ID's)
      if ( m_allowSameIDCounters == kFALSE && _it->second->getID() != _ID) {
        // This is technically allowed by the architecture of the program, but we will warn the user anyway as they have
        // not explicitly said that it is OK to have many counters with the same ID
        Warning("MonitorBase::getCounter", "Name clash for %s with IDs %i and %i.", _name.c_str(), _ID, _it->second->getID() );
      }
      m_countersInEvent.insert( _it->second );
      return _it->second;
    } else {
      // If we don't yet then add a new one and return a reference
      CounterBase* _TCCB = addCounter( _counterMap, _name, _ID );
      m_countersInEvent.insert( _TCCB );
      return _TCCB;
    }
  }

  /**
   * Base class wrapper to handle the map related aspects of adding a new counter.
   * The newCounter call is pure virtual and must be implemented by the derived class such that
   * an object of the correct type is created then passed back as a polymorphic base class pointer.
   * @param _name Const reference to the name of the new counter.
   * @param _ID Reference to the ID number of the new counter.
   * @result Base class pointer to new counter.
   */
  CounterBase* MonitorBase::addCounter( CounterMap_t* _counterMap, const std::string &_name, Int_t _ID ) {
    CounterBase* _tcc = newCounter( _name,  _ID );
    // If we are after a new counter each call - then change the name (but *ONLY* in the map) to a simple number
    // such that it will never match a counter name upon a call to newCounter
    if (m_allowSameNamedCounters == kTRUE) {
      static Int_t _counterNumber = 0;
      const std::string _uniqueName = std::string( "counter_" + intToString( _counterNumber++ ) );
      _counterMap->insert( std::make_pair( _uniqueName, _tcc ) );
    } else {
      _counterMap->insert( std::make_pair( _name, _tcc ) );
    }
    return _tcc;
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
    Error("MonitorBase::saveOutput", "This monitor which saveOutput has been called on does not have any save functionality... yet");
  }

  /**
   * Helper function. For a given counter, and a given variable plus VariableOption pair, construct the name of the plot
   * to be used as the file name or the histogram name in any output.
   * @param _counter Pointer to counter object, the name of the counter will be used.
   * @param _variable Pair of variable name and VariableOption enum, used in constructiong plot name.
   * @returns Formatted plot name string.
   */
  std::string MonitorBase::constructPlotName(CounterBase* _counter, ConfVariableOptionPair_t _variable) {
    std::string _name = _counter->getName();
    _name += "_";
    _name += Config::config().getStr(_variable.first); // What is being monitored (e.g. time, calls).
    _name += "_";
    _name += VariableOptionStr[ _variable.second ]; // How it's being monitored (e.g. per event, per call)
    return _name;
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
   * @param _l Sets the level.
   */
  void MonitorBase::setLevel( UInt_t _l ) {
    m_level = _l;
  }

  /**
   * Sets the name of this monitor, used when formatting output.
   * @param _name Sets the name.
   */
  void MonitorBase::setName( const std::string& _name ) {
    m_name = _name;
  }

  /**
   * Construct a TableColumnFormatter to output a column of data from all counters.
   * This version will directly fetch a variable from the counter's storage.
   * @param _title The title of the column in the table.
   * @param _variableName The name of the variable saved in the counter to export for this column.
   * @param _vo The corresponding variable option for this variable (saved per call, per event, etc.)
   * @param _precision The number of decimal places to include for this column.
   */
  MonitorBase::TableColumnFormatter::TableColumnFormatter(const std::string& _title,
      const std::string& _tooltip,
      ConfKey_t _variableName,
      VariableOption_t _vo,
      UInt_t _precision, FormatterOption_t _fo) :
    m_columnName(_title),
    m_tooltip(_tooltip),
    m_dataVariable(_variableName),
    m_dataVO(_vo),
    m_dataVariableDenominator(kNoneString),
    m_dataVODenominator(),
    m_precision(_precision),
    m_functionPtr(0),
    m_functionPtrStr(0),
    m_formatOption(_fo) {}

  /**
   * Construct a TableColumnFormatter to output a column of data from all counters.
   * This version will directly fetch two variable from the counter's storage and divide one by the other.
   * @param _title The title of the column in the table.
   * @param _variableNameNominator The name of the variable saved in the counter to export for this column.
   * @param _voNominator The corresponding variable option for this variable (saved per call, per event, etc.)
   * @param _variableNameDenominator The name of the variable saved in the counter to use as denominator.
   * @param _voDenominator The corresponding variable option for this denominator
   * @param _precision The number of decimal places to include for this column.
   */
  MonitorBase::TableColumnFormatter::TableColumnFormatter(const std::string& _title,
      const std::string& _tooltip,
      ConfKey_t _dataVarialbeNominator,
      VariableOption_t _voNominator,
      ConfKey_t _dataVarialbeDenominator,
      VariableOption_t _voDenominator,
      UInt_t _precision, FormatterOption_t _fo) :
    m_columnName(_title),
    m_tooltip(_tooltip),
    m_dataVariable(_dataVarialbeNominator),
    m_dataVO(_voNominator),
    m_dataVariableDenominator(_dataVarialbeDenominator),
    m_dataVODenominator(_voDenominator),
    m_precision(_precision),
    m_functionPtr(0),
    m_functionPtrStr(0),
    m_formatOption(_fo) {}

  /**
   * Construct a TableColumnFormatter to output a column of data from all counters.
   * This version takes a function pointer which can do an arbitrary operation using the counter and return the value to plot.
   * The CounterMap_t* input is considered optional. This is only needed when all counters are needed to calculate a quantity, such as how much time a
   * chain used averaged over all chains.
   * @param _title The title of the column in the table.
   * @param _functionPrt Pointer to function which takes a counterMap* and CounterBase* and then returns a float value to include in the column.
   * @param _precision The number of decimal places to include for this column.
   */
  MonitorBase::TableColumnFormatter::TableColumnFormatter(const std::string& _title,
      const std::string& _tooltip,
      Float_t (*_functionPtr)(CounterMap_t*, CounterBase*),
      UInt_t _precision ) :
    m_columnName(_title),
    m_tooltip(_tooltip),
    m_dataVariable(),
    m_dataVO(),
    m_dataVariableDenominator(),
    m_dataVODenominator(),
    m_precision(_precision),
    m_functionPtr(_functionPtr),
    m_functionPtrStr(0),
    m_formatOption(kFormatOptionNone) {}

  /**
   * Construct a TableColumnFormatter to output a column of data from all counters.
   * This version takes a function pointer which can do an arbitrary operation using the counter and return a string to plot.
   * The CounterMap_t* input is considered optional. This is only needed when all counters are needed to calculate a quantity, such as how much time a
   * chain used averaged over all chains.
   * @param _title The title of the column in the table.
   * @param _functionPrtStr Pointer to function which takes a CounterMap_t* and CounterBase* and then returns a string value to include in the column.
   */
  MonitorBase::TableColumnFormatter::TableColumnFormatter(const std::string& _title,
    const std::string& _tooltip,
    std::string (*_functionPtrStr)(CounterMap_t*, CounterBase*) ) :
    m_columnName(_title),
    m_tooltip(_tooltip),
    m_dataVariable(),
    m_dataVO(),
    m_dataVariableDenominator(),
    m_dataVODenominator(),
    m_precision(0),
    m_functionPtr(0),
    m_functionPtrStr(_functionPtrStr),
    m_formatOption(kFormatOptionNone) {}

} // namespace TrigCostRootAnalysis
