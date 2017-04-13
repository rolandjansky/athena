// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <assert.h>

// Local include(s):
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/DataStore.h"
#include "../TrigCostRootAnalysis/DataVariable.h"
#include "../TrigCostRootAnalysis/CounterBase.h"

// ROOT include(s):
#include <TError.h>
#include <TH1.h>

namespace TrigCostRootAnalysis {
  /**
   * DataStore constructor, takes no arguments.
   */
  DataStore::DataStore(CounterBase* _parent) : m_histogrammingEnabled(kTRUE), m_mostRecent(0), m_variableMap(),
    m_parent(_parent) {
  }

  /**
   * DataStore destructor.
   */
  DataStore::~DataStore() {
    VariableMapIt_t _it = m_variableMap.begin();

    for (; _it != m_variableMap.end(); ++_it) {
      delete _it->second;
    }
    m_variableMap.clear();
  }

  /**
   * Register a new monitored variable by name.
   * @see setSaveMode()
   * @param _name Name of new variable. Use a confKey with a string name set for it.
   * @return A self reference for chaining.
   */
  DataStore& DataStore::newVariable(ConfKey_t _name) {
    if (m_variableMap.count(_name) == 1) {
      Warning("DataStore::newVariable", "Variable %s already registered. Replacing it.", Config::config().getStr(
                _name).c_str());
      delete m_variableMap[_name];
    }
    m_mostRecent = new DataVariable(this);
    m_variableMap[_name] = m_mostRecent;
    return *this;
  }

  /**
   * Must be called on all registered savePerEventFraction variables before endEvent() is called.
   * @see endEvent()
   * @param _name Name of variable to set denominator for.
   * @param _denominator Denominator to divide by.
   */
  void DataStore::setVariableDenominator(ConfKey_t _name, Float_t _denominator) {
    if (checkRegistered(_name) == kFALSE) return;

    m_mostRecent->setVariableDenominator(_denominator); // m_mostRecent is set by checkRegistered
  }

  /**
   * Store a new value for a variable according to the variable's configuration.
   * Optimisation note: this is a very high-call-rate function.
   * @param _name Name of variable which is being stored.
   * @param _value Value to store.
   * @param _weight Weight of entry.
   */
  void DataStore::store(ConfKey_t _name, Float_t _value, Float_t _weight) {
    if (checkRegistered(_name) == kFALSE) return;

    m_mostRecent->store(_value, _weight); // m_mostRecent is set by checkRegistered
  }

  /**
   * Flag to enable or disable histogramming for *new* save states of current or new variables.
   * @param _histogrammingEnabled True to enable histograms for future calls to setSave*bla*.
   */
  void DataStore::setHistogramming(Bool_t _histogrammingEnabled) {
    m_histogrammingEnabled = _histogrammingEnabled;
  }

  /**
   * Set custom X axis bin lables for a histogram.
   * @param _name Name of variable which is being accessed.
   * @param _vo Variable option to edit.
   * @param _titles Vector of histogram X axis bin titles
   */
  void DataStore::setBinLabels(ConfKey_t _name, VariableOption_t _vo, std::vector<std::string> _titles) {
    if (checkRegistered(_name) == kFALSE) {
      Error("DataStore::setBinLabels", "Cannot find entry for %s", Config::config().getName(_name).c_str());
      return;
    }
    m_mostRecent->setBinLabels(_vo, _titles); // m_mostRecent is set by checkRegistered
  }

  /**
   * Flush buffers for all per-event quantities
   */
  void DataStore::endEvent() {
    VariableMapIt_t _it = m_variableMap.begin();

    for (; _it != m_variableMap.end(); ++_it) {
      _it->second->endEvent();
    }
  }

  /**
   * Get a value from the data store.
   * @param _name Name of variable to retrieve.
   * @param _vo Variable option to retrieve .
   * @return The stored result.
   */
  Float_t DataStore::getValue(ConfKey_t _name, VariableOption_t _vo) const {
    if (checkRegistered(_name) == kFALSE) return 0.;

    return m_mostRecent->getValue(_vo); // m_mostRecent is set by checkRegistered
  }

  /**
   * Check if a value key and variable option are available
   * @param _name Name of variable to check for.
   * @param _vo Variable option to check for.
   * @return If the variable is stored with a given VO.
   */
  Bool_t DataStore::getValueExists(ConfKey_t _name, VariableOption_t _vo) const {
    if (checkRegistered(_name, /*silent*/ kTRUE) == kFALSE) return kFALSE;

    return m_mostRecent->getValueExists(_vo); // m_mostRecent is set by checkRegistered
  }

  /**
   * Get a value error from the data store.
   * @param _name Name of variable to retrieve.
   * @param _vo Variable option to retrieve .
   * @return The stored result.
   */
  Float_t DataStore::getValueError(ConfKey_t _name, VariableOption_t _vo) const {
    if (checkRegistered(_name) == kFALSE) return 0.;

    return m_mostRecent->getValueError(_vo); // m_mostRecent is set by checkRegistered
  }

  /**
   * Get the number of filled entries.
   * @param _name Name of variable.
   * @param _vo Variable option.
   * @return The number of entries stored.
   */
  Int_t DataStore::getEntries(ConfKey_t _name, VariableOption_t _vo) const {
    if (checkRegistered(_name) == kFALSE) {
      Error("DataStore::getEntries", "No such entry '%s' registered with VO %s", Config::config().getStr(
              _name).c_str(), VariableOptionStr[_vo].c_str());
      return 0;
    }
    return m_mostRecent->getEntries(_vo);  // m_mostRecent is set by checkRegistered
  }

  /**
   * Set a value from the data store.
   * @param _name Name of variable to retrieve.
   * @param _vo Variable option to retrieve .
   * @param _val The value to set.
   */
  void DataStore::setValue(ConfKey_t _name, VariableOption_t _vo, Float_t _val) {
    if (checkRegistered(_name) == kFALSE) return;

    m_mostRecent->setValue(_vo, _val); // m_mostRecent is set by checkRegistered
  }

  /**
   * Set the number of filled entries.
   * @param _name Name of variable.
   * @param _vo Variable option.
   * @param _val The value to set as the number of entries stored.
   */
  void DataStore::setEntries(ConfKey_t _name, VariableOption_t _vo, UInt_t _val) {
    if (checkRegistered(_name) == kFALSE) {
      Error("DataStore::setEntries", "No such entry '%s' registered with VO %s", Config::config().getStr(
              _name).c_str(), VariableOptionStr[_vo].c_str());
      return;
    }
    m_mostRecent->setEntries(_vo, _val);  // m_mostRecent is set by checkRegistered
  }

  /**
   * Set the sumw2 variable - this is the SQUARE of the error.
   * @param _name Name of variable.
   * @param _vo Variable option.
   * @param _val The SQUARE of the error to save.
   */
  void DataStore::setErrorSquared(ConfKey_t _name, VariableOption_t _vo, Float_t _val) {
    if (checkRegistered(_name) == kFALSE) {
      Error("DataStore::setError", "No such entry '%s' registered with VO %s", Config::config().getStr(
              _name).c_str(), VariableOptionStr[_vo].c_str());
      return;
    }
    m_mostRecent->setErrorSquared(_vo, _val);  // m_mostRecent is set by checkRegistered
  }

  /**
   * Get the histogram for a monitored quantity.
   * @param _name Name of variable.
   * @param _vo Variable option.
   * @param _silent set to true to suppress error messages
   * @return Histogram pointer.
   */
  TH1F* DataStore::getHist(ConfKey_t _name, VariableOption_t _vo, Bool_t _silent) {
    if (checkRegistered(_name, _silent) == kFALSE) {
      if (_silent == kFALSE) Error("DataStore::getHist",
                                   "Remember to give axis labels with newVariable() if you want histogramming.");
      return 0;
    }
    return m_mostRecent->getHist(_vo, _silent); // m_mostRecent is set by checkRegistered
  }

  /**
   * Set the histogram for a monitored quantity.
   * This overwrites any current hist! Use with care!
   * Object assumes ownership of the histogram
   * @param _name Name of variable.
   * @param _vo Variable option.
   * @param _hist histogram pointer
   */
  void DataStore::setHist(ConfKey_t _name, VariableOption_t _vo, TH1F* _hist) {
    if (checkRegistered(_name, kFALSE) == kFALSE) {
      Error("DataStore::setHist", "Cannot find entry");
      return;
    }
    return m_mostRecent->setHist(_vo, _hist); // m_mostRecent is set by checkRegistered
  }

  /**
   * Get the histogram for a 2D monitored quantity.
   * @param _name Name of variable.
   * @param _vo Variable option.
   * @return 2D histogram pointer.
   */
  // TH2F* DataStore::getHist2D(ConfKey_t _name, VariableOption_t _vo) {
  //   if ( checkRegistered(_name) == kFALSE ) return 0;
  //   return m_mostRecent->getHist2D(_vo); // m_mostRecent is set by checkRegistered
  // }

  /**
   * Iterate over the data store, record all entries which have a histogram.
   * @return Vector< pair< name, VariableOption_t > > of all entries which have a != TH!F pointer
   */
  VariableOptionVector_t DataStore::getAllHistograms() {
    VariableOptionVector_t _vec;
    VariableMapIt_t _dsIt = m_variableMap.begin();

    for (; _dsIt != m_variableMap.end(); ++_dsIt) {
      ConfKey_t _dvNameEnum = _dsIt->first;

      for (Int_t _vo = 0; _vo < kVariableOption_SIZE; ++_vo) {
        VariableOption_t _voEnum = static_cast<VariableOption_t>(_vo);

        if (getHist(_dvNameEnum, _voEnum, /*_silent = */ kTRUE) == 0) continue;
        if (getHist(_dvNameEnum, _voEnum, /*_silent = */ kTRUE)->GetEntries() == 0) continue;

        if (Config::config().debug()) {
          Info("DataStore::getAllHistograms", "Adding output histogram %s / %s", Config::config().getName(
                 _dvNameEnum).c_str(), VariableOptionStr[_voEnum].c_str());
        }

        _vec.push_back(std::make_pair(_dvNameEnum, _voEnum));
      }
    }
    //Info("","returning vec of size %i", (int)_vec.size());
    return _vec;
  }

  /**
   * If a problem happens downstream in DataVariable, it is handy to know what call DataStore made which caused it.
   * As DataVariables take up enough memory already and this is only generally needed if things go wrong,
   * this function will manually search the map to reverse-lookup the name from the pointer to the DataVariable
   * saved in m_mostRecent.
   * This is to make the vast majority of calls (where DataVariable doesn't care about it's name) as fast as possible.
   * @return Const reference to string with the the name (key) corresponding to m_mostRecent
   */
  const std::string& DataStore::getNameOfMostRecentCall() const {
    VariableMapIt_t _it = m_variableMap.begin();

    // One-to-one mapping is enforced by newVariable, so this reverse-lookup can exit on first success.
    for (; _it != m_variableMap.end(); ++_it) {
      if (_it->second == m_mostRecent) { // Check pointers
        return Config::config().getStr(_it->first);
      }
    }
    return Config::config().getStr(kUnknownString);
  }

  /**
   * Check if map contains variable with _name.
   * If found, pointer to the DataVariable stored in member pointer m_mostRecent (to save us having to do another find
   *to use it!)
   * Optimisation note: this is a very high-call-rate function.
   * Current implementation means that the map is only searched once per Store call.
   * @param _name Name of variable.
   * @return kTRUE if contains _name, m_mostRecent then points to the counter. Else kFALSE and message to error stream.
   */
  Bool_t DataStore::checkRegistered(ConfKey_t _name, Bool_t _silent) const {
    VariableMapIt_t _location = m_variableMap.find(_name);

    if (_location == m_variableMap.end()) {
      if (_silent == kFALSE && Config::config().getDisplayMsg(kMsgCannotFindVar) == kTRUE) {
        Error("DataStore::checkRegistered", "Unable to find variable with name %s", Config::config().getStr(
                _name).c_str());
      }
      return kFALSE;
    }
    m_mostRecent = _location->second; //Cache the pointer to the DataVariable
    return kTRUE;
  }

  /**
   * Internal function used to request new save option in DataVariable.
   * One name to be supplied or pointer m_mostRecent used.
   * @see m_mostRecent
   * @param _vo Variable option.
   * @param _title Name of variable.
   * @param _name Name of variable to change, or "" to use m_mostRecent.
   * @return A self reference for chaining.
   */
  DataStore& DataStore::setSaveInternal(VariableOption_t _vo, std::string& _title, ConfKey_t _name) {
    // Function can be called with or without name. If called with, set m_mostRecent to match the name
    // supplied. Otherwise using chained function calls so check that m_mostRecent is valid.
    if (_name != kBlankString) {
      if (checkRegistered(_name) == kFALSE) return *this;

      m_mostRecent = m_variableMap[_name];
    } else if (m_mostRecent == 0) {
      Error("DataStore::setSaveInternal", "No variable to set save state on. Did you call newVariable() first?");
      return *this;
    }
    m_mostRecent->registerSaveState(_vo, _title);
    return *this;
  }

  /**
   * Non-chain version of SetSave which accepts a name argument.
   * @param _name Name of variable to modify.
   * @param _vo Variable option to enable monitoring for.
   * @param _title String, if filled enables histograming, format "XAxisLabel;YAxisLabel"
   */
  void DataStore::setSaveMode(ConfKey_t _name, VariableOption_t _vo, std::string _title) {
    // Manual method, pass to internal function.
    setSaveInternal(_vo, _title, _name);
  }

  /**
   * Chain version of SetSave which enables saving per-call on most recently added variable.
   * @param _title Passing a non-empty string enables histograming, format "XAxisLabel;YAxisLabel"
   * @return A self reference for chaining.
   */
  DataStore& DataStore::setSavePerCall(std::string _title) {
    return setSaveInternal(kSavePerCall, _title);
  }

  /**
   * Chain version of SetSave for most recently added variable which enables per-call addition to an
   * internal buffer which is stored at the end of the event.
   * @param _title Passing a non-empty string enables histograming, format "XAxisLabel;YAxisLabel"
   * @return A self reference for chaining.
   */
  DataStore& DataStore::setSavePerEvent(std::string _title) {
    return setSaveInternal(kSavePerEvent, _title);
  }

  /**
   * Chain version of SetSave for most recently added variable which enables per-call addition to an
   * internal buffer which is stored at the end of the user-defined period.
   * @param _title Passing a non-empty string enables histogramming, format "XAxisLabel;YAxisLabel"
   * @return A self reference for chaining.
   */
  DataStore& DataStore::setSavePerPeriod(std::string _title) {
    return setSaveInternal(kSavePerPeriod, _title);
  }

  /**
   * Chain version of SetSave for most recently added variable which enables per-call addition to an
   * internal buffer which is divided by a denominator before being stored at the end of the event.
   * @see setVariableDenominator(std::string _name, Float_t denominator)
   * @param _title Passing a non-empty string enables histograming, format "XAxisLabel;YAxisLabel"
   * @return A self reference for chaining.
   */
  DataStore& DataStore::setSavePerEventFraction(std::string _title) {
    return setSaveInternal(kSavePerEventFraction, _title);
  }
} // namespace TrigCostRootAnalysis
