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
#include "../TrigCostRootAnalysis/CounterBase.h"
#include "../TrigCostRootAnalysis/MonitorBase.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"

// ROOT include(s):
#include <TError.h>

namespace TrigCostRootAnalysis {
  /**
   * Base class constructor. Sets name and ID.
   */
  CounterBase::CounterBase(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel,
                           MonitorBase* parent) :
    m_costData(costData),
    m_dataStore(this),
    m_parent(parent),
    m_strDecorations(),
    m_decorations(),
    m_detailLevel(detailLevel),
    m_name(name),
    m_calls(0) {
    decorate(kDecID, ID);
  }

  /**
   * Base class destructor. Currently no base pointers to delete.
   */
  CounterBase::~CounterBase() {
  }

  /**
   * @return Pointer to the parent monitor.
   */
  MonitorBase* CounterBase::getParent() const {
    return m_parent;
  }

  /**
   * Return data for this counter from within the DataStore.
   * @param name The name of the variable required.
   * @param vo The VariableOption_t requested for this variable.
   * @return The stored value requested.
   */
  Float_t CounterBase::getValue(ConfKey_t name, VariableOption_t vo) const {
    return m_dataStore.getValue(name, vo);
  }

  /**
   * Return if the key is registered in the data store.
   * @param name The name of the variable required.
   * @param vo The VariableOption_t requested for this variable.
   * @return If the key is present in the data store for the given VO.
   */
  Bool_t CounterBase::getValueExists(ConfKey_t name, VariableOption_t vo) const {
    return m_dataStore.getValueExists(name, vo);
  }

  /**
   * Return the error on data for this counter from within the DataStore.
   * @param name The name of the variable required.
   * @param vo The VariableOption_t requested for this variable.
   * @return The stored value requested.
   */
  Float_t CounterBase::getValueError(ConfKey_t name, VariableOption_t vo) const {
    return m_dataStore.getValueError(name, vo);
  }

  /**
   * Return data for this counter from within the DataStore, normalised to the number of entries.
   * @param name The name of the variable required.
   * @param vo The VariableOption_t requested for this variable.
   * @return The stored value divided by the number of entries.
   */
  Float_t CounterBase::getValueNormalised(ConfKey_t name, VariableOption_t vo) const {
    Float_t num = getValue(name, vo);
    Float_t denom = getEntries(name, vo);

    if (isZero(denom) == kTRUE) {
      Error("CounterBase::getValueNormalised", "Cannot get normalised value for %s, no entries. [/0].",
            Config::config().getStr(name).c_str());
      return 0.;
    }
    return(num / denom);
  }

  /**
   * Return data for this counter from within the DataStore.
   * @param name The name of the variable required.
   * @param vo The VariableOption_t requested for this variable.
   * @return The number of entries stored for this variable and VariableOption.
   */
  Int_t CounterBase::getEntries(ConfKey_t name, VariableOption_t vo) const {
    return m_dataStore.getEntries(name, vo);
  }

  /**
   * Set data for this counter from within the DataStore.
   * @param name The name of the variable required.
   * @param vo The VariableOption_t requested for this variable.
   * @param val The value store for this variable and VariableOption.
   */
  void CounterBase::setValue(ConfKey_t name, VariableOption_t vo, Float_t val) {
    return m_dataStore.setValue(name, vo, val);
  }

  /**
   * Set data entries for this counter from within the DataStore.
   * @param name The name of the variable required.
   * @param vo The VariableOption_t requested for this variable.
   * @param val The number of entries to store for this variable and VariableOption.
   */
  void CounterBase::setEntries(ConfKey_t name, VariableOption_t vo, UInt_t val) {
    return m_dataStore.setEntries(name, vo, val);
  }

  /**
   * Overwrite the sumw2 error of this counter. Note that this should be the SQUARE of the error.
   * @param name The name of the variable required.
   * @param vo The VariableOption_t requested for this variable.
   * @param val The SQUARE of the error for this variable
   */
  void CounterBase::setErrorSquared(ConfKey_t name, VariableOption_t vo, Float_t val) {
    return m_dataStore.setErrorSquared(name, vo, val);
  }

  /**
   * Return data for this counter from within the DataStore.
   * @param name The name of the variable required.
   * @param vo The VariableOption_t requested for this variable.
   * @return The corresponding histogram pointer for this variable and VariableOption.
   */
  TH1F* CounterBase::getHist(ConfKey_t name, VariableOption_t vo) {
    return m_dataStore.getHist(name, vo);
  }

  /**
   * Return data for this counter from within the DataStore.
   * NOTE: 2D historgams are currently not implemented. This will always return 0.
   * @param name The name of the variable required.
   * @param vo The VariableOption_t requested for this variable.
   * @return The corresponding 2D histogram pointer for this variable and VariableOption.
   */
  // TH2F* CounterBase::getHist2D(ConfKey_t name, VariableOption_t vo) {
  //   return m_dataStore.getHist2D(name, vo);
  // }

  /**
   * Passthrough function, see imp in DataStore
   * @return Vector< pair< name, VariableOption_t > > of all entries which have a != TH1F pointer
   */
  VariableOptionVector_t CounterBase::getAllHistograms() {
    return m_dataStore.getAllHistograms();
  }

  /**
   * Statistics on how many times this Counter object has been called.
   * @return The number of calls.
   */
  UInt_t CounterBase::getCalls() const {
    return m_calls;
  }

  /**
   * Get the unique (within its parent Monitor) name of the counter.
   * @return Const reference to name of this counter.
   */
  const std::string& CounterBase::getName() const {
    return m_name;
  }

  /**
   * Get the ID number of the counter. Not used for bookkeeping.
   * @return Stored ID number of counter.
   */
  Int_t CounterBase::getID() const {
    return getIntDecoration(kDecID);
  }

  /**
   * Get the ID numbers of the counter. For counters with multiple IDs, like ROBINs with many ROBs.
   * @return Set of stored ID numbers of counter.
   */
  const std::set<UInt_t>& CounterBase::getMultiID() const {
    return m_multiId;
  }

  /**
   * Records additional ID number for counter. For counters with multiple IDs, like ROBINs with many ROBs.
   * @param multiId The (additional) ID to store. Ignores duplicates.
   */
  void CounterBase::addMultiID(UInt_t multiId) {
    m_multiId.insert(multiId);
  }

  /**
   * Alternate method format, passing both variable name and option in a pair.
   * @see getHist(std::string name, VariableOption_t vo)
   * @return The corresponding histogram pointer for this variable and VariableOption.
   */
  TH1F* CounterBase::getHist(ConfVariableOptionPair_t pair) {
    return getHist(pair.first, pair.second);
  }

  /**
   * For some counters, like algorithm ones, it is handy to decorate the counter with additional information.
   * Such as the algorithm class, the algorithm's chain.
   * @param key ConfKey_t key to associate the data with.
   * @param value Data to save
   */
  void CounterBase::decorate(ConfKey_t key, const std::string value) {
    m_strDecorations[key] = value;
  }

  /**
   * For some counters, like algorithm ones, it is handy to decorate the counter with additional information.
   * @param key ConfKey_t key to associate the data with.
   * @param value Data to save
   */
  void CounterBase::decorate(ConfKey_t key, const Float_t value) {
    m_decorations[key] = value;
  }

  /**
   * Additional decorate call for Int_t values
   * @param key ConfKey_t key to associate the data with.
   * @param value Data to save
   */
  void CounterBase::decorate(ConfKey_t key, Int_t value) {
    m_intDecorations[key] = value;
  }

  /**
   * Get a string decoration.
   * @see decorate( ConfKey_t key, const std::string value )
   * @return Const reference to decoration.
   */
  const std::string& CounterBase::getStrDecoration(ConfKey_t key) const {
    if (m_strDecorations.count(key) == 0) {
      if (Config::config().getDisplayMsg(kMsgUnknownDecoration) == kTRUE) {
        Warning("CounterBase::getStrDecoration", "Unknown decoration %s requested in %s.", Config::config().getStr(
                  key).c_str(), getName().c_str());
      }
      return Config::config().getStr(kUnknownString);
    }
    return m_strDecorations.at(key);
  }

  /**
   * Get a float decoration.
   * @see decorate( ConfKey_t key, Float_t value )
   * @return Const reference to float decoration
   */
  Float_t CounterBase::getDecoration(ConfKey_t key) const {
    if (m_decorations.count(key) == 0) {
      if (Config::config().getDisplayMsg(kMsgUnknownDecoration) == kTRUE) {
        Warning("CounterBase::getDecoration", "Unknown decoration %s requested in %s.", Config::config().getStr(
                  key).c_str(), getName().c_str());
      }
      return 0.;
    }
    return m_decorations.at(key);
  }

  /**
   * Get a int decoration.
   * @see decorate( ConfKey_t key, Int_t value )
   * @return Const reference to int decoration
   */
  Int_t CounterBase::getIntDecoration(ConfKey_t key) const {
    if (m_intDecorations.count(key) == 0) {
      if (Config::config().getDisplayMsg(kMsgUnknownDecoration) == kTRUE) {
        Warning("CounterBase::getIntDecoration", "Unknown decoration %s requested in %s.", Config::config().getStr(
                  key).c_str(), getName().c_str());
      }
      return 0.;
    }
    return m_intDecorations.at(key);
  }

  /**
   * @return true if a decoration of any type is stored for the given key
   */
  Bool_t CounterBase::hasDecoration(ConfKey_t key) const {
    if (m_decorations.count(key) > 0) return kTRUE;

    if (m_intDecorations.count(key) > 0) return kTRUE;

    if (m_strDecorations.count(key) > 0) return kTRUE;

    return kFALSE;
  }

  const std::string CounterBase::getRange() const {
    auto collection_iter = this->getParent()->getAllCounterCollections()->begin();

    for (; collection_iter != this->getParent()->getAllCounterCollections()->end(); ++collection_iter) {
      auto counter_iter = collection_iter->second.begin();

      for (; counter_iter != collection_iter->second.end(); ++counter_iter) {
        if (counter_iter->second == this) {
          return collection_iter->first;
        }
      }
    }

    return "UNKNOWN RANGE";
  }
} // namespace TrigCostRootAnalysis
