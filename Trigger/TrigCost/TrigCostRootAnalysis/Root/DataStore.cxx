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
  DataStore::DataStore(CounterBase* parent) : m_histogrammingEnabled(kTRUE), m_mostRecent(0), m_variableMap(),
    m_parent(parent) {
  }

  /**
   * DataStore destructor.
   */
  DataStore::~DataStore() {
    VariableMapIt_t it = m_variableMap.begin();

    for (; it != m_variableMap.end(); ++it) {
      delete it->second;
    }
    m_variableMap.clear();
  }

  /**
   * Register a new monitored variable by name.
   * @see setSaveMode()
   * @param name Name of new variable. Use a confKey with a string name set for it.
   * @return A self reference for chaining.
   */
  DataStore& DataStore::newVariable(ConfKey_t name) {
    if (m_variableMap.count(name) == 1) {
      Warning("DataStore::newVariable", "Variable %s already registered. Replacing it.", Config::config().getStr(
                name).c_str());
      delete m_variableMap[name];
    }
    m_mostRecent = new DataVariable(this);
    m_variableMap[name] = m_mostRecent;
    return *this;
  }

  /**
   * Must be called on all registered savePerEventFraction variables before endEvent() is called.
   * @see endEvent()
   * @param name Name of variable to set denominator for.
   * @param denominator Denominator to divide by.
   */
  void DataStore::setVariableDenominator(ConfKey_t name, Float_t denominator) {
    if (checkRegistered(name) == kFALSE) return;

    m_mostRecent->setVariableDenominator(denominator); // m_mostRecent is set by checkRegistered
  }

  /**
   * Store a new value for a variable according to the variable's configuration.
   * Optimisation note: this is a very high-call-rate function.
   * @param name Name of variable which is being stored.
   * @param value Value to store.
   * @param weight Weight of entry.
   */
  void DataStore::store(ConfKey_t name, Float_t value, Float_t weight) {
    if (checkRegistered(name) == kFALSE) return;

    m_mostRecent->store(value, weight); // m_mostRecent is set by checkRegistered
  }

  /**
   * Flag to enable or disable histogramming for *new* save states of current or new variables.
   * @param histogrammingEnabled True to enable histograms for future calls to setSave*bla*.
   */
  void DataStore::setHistogramming(Bool_t histogrammingEnabled) {
    m_histogrammingEnabled = histogrammingEnabled;
  }

  /**
   * Set custom X axis bin lables for a histogram.
   * @param name Name of variable which is being accessed.
   * @param vo Variable option to edit.
   * @param titles Vector of histogram X axis bin titles
   */
  void DataStore::setBinLabels(ConfKey_t name, VariableOption_t vo, std::vector<std::string> titles) {
    if (checkRegistered(name) == kFALSE) {
      Error("DataStore::setBinLabels", "Cannot find entry for %s", Config::config().getName(name).c_str());
      return;
    }
    m_mostRecent->setBinLabels(vo, titles); // m_mostRecent is set by checkRegistered
  }

  /**
   * Flush buffers for all per-event quantities
   */
  void DataStore::endEvent() {
    VariableMapIt_t it = m_variableMap.begin();

    for (; it != m_variableMap.end(); ++it) {
      it->second->endEvent();
    }
  }

  /**
   * Get a value from the data store.
   * @param name Name of variable to retrieve.
   * @param vo Variable option to retrieve .
   * @return The stored result.
   */
  Float_t DataStore::getValue(ConfKey_t name, VariableOption_t vo) const {
    if (checkRegistered(name) == kFALSE) return 0.;

    return m_mostRecent->getValue(vo); // m_mostRecent is set by checkRegistered
  }

  /**
   * Check if a value key and variable option are available
   * @param name Name of variable to check for.
   * @param vo Variable option to check for.
   * @return If the variable is stored with a given VO.
   */
  Bool_t DataStore::getValueExists(ConfKey_t name, VariableOption_t vo) const {
    if (checkRegistered(name, /*silent*/ kTRUE) == kFALSE) return kFALSE;

    return m_mostRecent->getValueExists(vo); // m_mostRecent is set by checkRegistered
  }

  /**
   * Get a value error from the data store.
   * @param name Name of variable to retrieve.
   * @param vo Variable option to retrieve .
   * @return The stored result.
   */
  Float_t DataStore::getValueError(ConfKey_t name, VariableOption_t vo) const {
    if (checkRegistered(name) == kFALSE) return 0.;

    return m_mostRecent->getValueError(vo); // m_mostRecent is set by checkRegistered
  }

  /**
   * Get the number of filled entries.
   * @param name Name of variable.
   * @param vo Variable option.
   * @return The number of entries stored.
   */
  Int_t DataStore::getEntries(ConfKey_t name, VariableOption_t vo) const {
    if (checkRegistered(name) == kFALSE) {
      Error("DataStore::getEntries", "No such entry '%s' registered with VO %s", Config::config().getStr(
              name).c_str(), VariableOptionStr[vo].c_str());
      return 0;
    }
    return m_mostRecent->getEntries(vo);  // m_mostRecent is set by checkRegistered
  }

  /**
   * Set a value from the data store.
   * @param name Name of variable to retrieve.
   * @param vo Variable option to retrieve .
   * @param _val The value to set.
   */
  void DataStore::setValue(ConfKey_t name, VariableOption_t vo, Float_t val) {
    if (checkRegistered(name) == kFALSE) return;

    m_mostRecent->setValue(vo, val); // m_mostRecent is set by checkRegistered
  }

  /**
   * Set the number of filled entries.
   * @param name Name of variable.
   * @param vo Variable option.
   * @param val The value to set as the number of entries stored.
   */
  void DataStore::setEntries(ConfKey_t name, VariableOption_t vo, UInt_t val) {
    if (checkRegistered(name) == kFALSE) {
      Error("DataStore::setEntries", "No such entry '%s' registered with VO %s", Config::config().getStr(
              name).c_str(), VariableOptionStr[vo].c_str());
      return;
    }
    m_mostRecent->setEntries(vo, val);  // m_mostRecent is set by checkRegistered
  }

  /**
   * Set the sumw2 variable - this is the SQUARE of the error.
   * @param name Name of variable.
   * @param vo Variable option.
   * @param val The SQUARE of the error to save.
   */
  void DataStore::setErrorSquared(ConfKey_t name, VariableOption_t vo, Float_t val) {
    if (checkRegistered(name) == kFALSE) {
      Error("DataStore::setError", "No such entry '%s' registered with VO %s", Config::config().getStr(
              name).c_str(), VariableOptionStr[vo].c_str());
      return;
    }
    m_mostRecent->setErrorSquared(vo, val);  // m_mostRecent is set by checkRegistered
  }

  /**
   * Get the histogram for a monitored quantity.
   * @param name Name of variable.
   * @param vo Variable option.
   * @param silent set to true to suppress error messages
   * @return Histogram pointer.
   */
  TH1F* DataStore::getHist(ConfKey_t name, VariableOption_t vo, Bool_t silent) {
    if (checkRegistered(name, silent) == kFALSE) {
      if (silent == kFALSE) Error("DataStore::getHist",
                                   "Remember to give axis labels with newVariable() if you want histogramming.");
      return 0;
    }
    return m_mostRecent->getHist(vo, silent); // m_mostRecent is set by checkRegistered
  }

  /**
   * Set the histogram for a monitored quantity.
   * This overwrites any current hist! Use with care!
   * Object assumes ownership of the histogram
   * @param name Name of variable.
   * @param vo Variable option.
   * @param hist histogram pointer
   */
  void DataStore::setHist(ConfKey_t name, VariableOption_t vo, TH1F* hist) {
    if (checkRegistered(name, kFALSE) == kFALSE) {
      Error("DataStore::setHist", "Cannot find entry");
      return;
    }
    return m_mostRecent->setHist(vo, hist); // m_mostRecent is set by checkRegistered
  }

  /**
   * Get the histogram for a 2D monitored quantity.
   * @param name Name of variable.
   * @param vo Variable option.
   * @return 2D histogram pointer.
   */
  // TH2F* DataStore::getHist2D(ConfKey_t name, VariableOption_t vo) {
  //   if ( checkRegistered(name) == kFALSE ) return 0;
  //   return m_mostRecent->getHist2D(vo); // m_mostRecent is set by checkRegistered
  // }

  /**
   * Iterate over the data store, record all entries which have a histogram.
   * @return Vector< pair< name, VariableOption_t > > of all entries which have a != TH!F pointer
   */
  VariableOptionVector_t DataStore::getAllHistograms() {
    VariableOptionVector_t vec;
    VariableMapIt_t dsIt = m_variableMap.begin();

    for (; dsIt != m_variableMap.end(); ++dsIt) {
      ConfKey_t dvNameEnum = dsIt->first;

      for (Int_t vo = 0; vo < kVariableOption_SIZE; ++vo) {
        VariableOption_t voEnum = static_cast<VariableOption_t>(vo);

        if (getHist(dvNameEnum, voEnum, /*silent = */ kTRUE) == 0) continue;
        if (getHist(dvNameEnum, voEnum, /*silent = */ kTRUE)->GetEntries() == 0) continue;

        if (Config::config().debug()) {
          Info("DataStore::getAllHistograms", "Adding output histogram %s / %s", Config::config().getName(
                 dvNameEnum).c_str(), VariableOptionStr[voEnum].c_str());
        }

        vec.push_back(std::make_pair(dvNameEnum, voEnum));
      }
    }
    //Info("","returning vec of size %i", (int)vec.size());
    return vec;
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
    VariableMapIt_t it = m_variableMap.begin();

    // One-to-one mapping is enforced by newVariable, so this reverse-lookup can exit on first success.
    for (; it != m_variableMap.end(); ++it) {
      if (it->second == m_mostRecent) { // Check pointers
        return Config::config().getStr(it->first);
      }
    }
    return Config::config().getStr(kUnknownString);
  }

  /**
   * Check if map contains variable with name.
   * If found, pointer to the DataVariable stored in member pointer m_mostRecent (to save us having to do another find
   *to use it!)
   * Optimisation note: this is a very high-call-rate function.
   * Current implementation means that the map is only searched once per Store call.
   * @param name Name of variable.
   * @return kTRUE if contains name, m_mostRecent then points to the counter. Else kFALSE and message to error stream.
   */
  Bool_t DataStore::checkRegistered(ConfKey_t name, Bool_t silent) const {
    VariableMapIt_t location = m_variableMap.find(name);

    if (location == m_variableMap.end()) {
      if (silent == kFALSE && Config::config().getDisplayMsg(kMsgCannotFindVar) == kTRUE) {
        Error("DataStore::checkRegistered", "Unable to find variable with name %s", Config::config().getStr(
                name).c_str());
      }
      return kFALSE;
    }
    m_mostRecent = location->second; //Cache the pointer to the DataVariable
    return kTRUE;
  }

  /**
   * Internal function used to request new save option in DataVariable.
   * One name to be supplied or pointer m_mostRecent used.
   * @see m_mostRecent
   * @param vo Variable option.
   * @param title Name of variable.
   * @param name Name of variable to change, or "" to use m_mostRecent.
   * @return A self reference for chaining.
   */
  DataStore& DataStore::setSaveInternal(VariableOption_t vo, std::string& title, ConfKey_t name) {
    // Function can be called with or without name. If called with, set m_mostRecent to match the name
    // supplied. Otherwise using chained function calls so check that m_mostRecent is valid.
    if (name != kBlankString) {
      if (checkRegistered(name) == kFALSE) return *this;

      m_mostRecent = m_variableMap[name];
    } else if (m_mostRecent == 0) {
      Error("DataStore::setSaveInternal", "No variable to set save state on. Did you call newVariable() first?");
      return *this;
    }
    m_mostRecent->registerSaveState(vo, title);
    return *this;
  }

  /**
   * Non-chain version of SetSave which accepts a name argument.
   * @param name Name of variable to modify.
   * @param vo Variable option to enable monitoring for.
   * @param title String, if filled enables histograming, format "XAxisLabel;YAxisLabel"
   */
  void DataStore::setSaveMode(ConfKey_t name, VariableOption_t vo, std::string title) {
    // Manual method, pass to internal function.
    setSaveInternal(vo, title, name);
  }

  /**
   * Chain version of SetSave which enables saving per-call on most recently added variable.
   * @param title Passing a non-empty string enables histograming, format "XAxisLabel;YAxisLabel"
   * @return A self reference for chaining.
   */
  DataStore& DataStore::setSavePerCall(std::string title) {
    return setSaveInternal(kSavePerCall, title);
  }

  /**
   * Chain version of SetSave for most recently added variable which enables per-call addition to an
   * internal buffer which is stored at the end of the event.
   * @param title Passing a non-empty string enables histograming, format "XAxisLabel;YAxisLabel"
   * @return A self reference for chaining.
   */
  DataStore& DataStore::setSavePerEvent(std::string title) {
    return setSaveInternal(kSavePerEvent, title);
  }

  /**
   * Chain version of SetSave for most recently added variable which enables per-call addition to an
   * internal buffer which is stored at the end of the user-defined period.
   * @param title Passing a non-empty string enables histogramming, format "XAxisLabel;YAxisLabel"
   * @return A self reference for chaining.
   */
  DataStore& DataStore::setSavePerPeriod(std::string title) {
    return setSaveInternal(kSavePerPeriod, title);
  }

  /**
   * Chain version of SetSave for most recently added variable which enables per-call addition to an
   * internal buffer which is divided by a denominator before being stored at the end of the event.
   * @see setVariableDenominator(std::string name, Float_t denominator)
   * @param title Passing a non-empty string enables histograming, format "XAxisLabel;YAxisLabel"
   * @return A self reference for chaining.
   */
  DataStore& DataStore::setSavePerEventFraction(std::string title) {
    return setSaveInternal(kSavePerEventFraction, title);
  }
} // namespace TrigCostRootAnalysis
