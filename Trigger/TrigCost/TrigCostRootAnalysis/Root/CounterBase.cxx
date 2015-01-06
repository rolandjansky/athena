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
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"

// ROOT include(s):
#include <TError.h>

namespace TrigCostRootAnalysis {

  /**
   * Base class constructor. Sets name and ID.
   */
  CounterBase::CounterBase(const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel) : 
    m_costData(_costData),
    m_dataStore(this),
    m_strDecorations(),
    m_decorations(),
    m_detailLevel(_detailLevel),
    m_name(_name),
    m_ID(_ID), 
    m_calls(0) {
  }

  /**
   * Base class destructor. Currently no base pointers to delete.
   */
  CounterBase::~CounterBase() {
  }
  
  /**
   * Return data for this counter from within the DataStore. 
   * @param _name The name of the variable required.
   * @param _vo The VariableOption_t requested for this variable.
   * @return The stored value requested.
   */
  Float_t CounterBase::getValue(ConfKey_t _name, VariableOption_t _vo) {
    return m_dataStore.getValue(_name, _vo);
  }

  /**
   * Return the error on data for this counter from within the DataStore. 
   * @param _name The name of the variable required.
   * @param _vo The VariableOption_t requested for this variable.
   * @return The stored value requested.
   */
  Float_t CounterBase::getValueError(ConfKey_t _name, VariableOption_t _vo) {
    return m_dataStore.getValueError(_name, _vo);
  }
  
  /**
   * Return data for this counter from within the DataStore, normalised to the number of entries.
   * @param _name The name of the variable required.
   * @param _vo The VariableOption_t requested for this variable.
   * @return The stored value divided by the number of entires.
   */
  Float_t CounterBase::getValueNormalised(ConfKey_t _name, VariableOption_t _vo) {
    Float_t _nom = getValue(_name, _vo);
    Float_t _denom = getEntries(_name, _vo);
    if (isZero(_denom) == kTRUE) {
      Error("CounterBase::getValueNormalised", "Cannot get normalised value for %s, no entries. [/0].", Config::config().getStr(_name).c_str());
      return 0.;
    }
    return ( _nom / _denom );
  }
  
  /**
   * Return data for this counter from within the DataStore.
   * @param _name The name of the variable required.
   * @param _vo The VariableOption_t requested for this variable.
   * @return The number of entries stored for this variable and VariableOption.
   */
  Int_t CounterBase::getEntries(ConfKey_t _name, VariableOption_t _vo) {
    return m_dataStore.getEntries(_name, _vo);
  }

  /**
   * Set data for this counter from within the DataStore.
   * @param _name The name of the variable required.
   * @param _vo The VariableOption_t requested for this variable.
   * @param _val The value store for this variable and VariableOption.
   */
  void CounterBase::setValue(ConfKey_t _name, VariableOption_t _vo, Float_t _val) {
    return m_dataStore.setValue(_name, _vo, _val);
  }

  /**
   * Set data entries for this counter from within the DataStore.
   * @param _name The name of the variable required.
   * @param _vo The VariableOption_t requested for this variable.
   * @param _val The number of entries to store for this variable and VariableOption.
   */
  void CounterBase::setEntries(ConfKey_t _name, VariableOption_t _vo, UInt_t _val) {
    return m_dataStore.setEntries(_name, _vo, _val);
  }

  /**
   * Overwrite the sumw2 error of this counter. Note that this should be the SQUARE of the error.
   * @param _name The name of the variable required.
   * @param _vo The VariableOption_t requested for this variable.
   * @param _val The SQUARE of the error for this variable
   */
  void CounterBase::setError(ConfKey_t _name, VariableOption_t _vo, Float_t _val) {
    return m_dataStore.setError(_name, _vo, _val);
  }
  
  /**
   * Return data for this counter from within the DataStore.
   * @param _name The name of the variable required.
   * @param _vo The VariableOption_t requested for this variable.
   * @return The corresponding histogram pointer for this variable and VariableOption.
   */
  TH1F* CounterBase::getHist(ConfKey_t _name, VariableOption_t _vo) {
    return m_dataStore.getHist(_name, _vo);
  }
  
  /**
   * Return data for this counter from within the DataStore.
   * NOTE: 2D historgams are currently not implimented. This will always return 0.
   * @param _name The name of the variable required.
   * @param _vo The VariableOption_t requested for this variable.
   * @return The corresponding 2D histogram pointer for this variable and VariableOption.
   */
  TH2F* CounterBase::getHist2D(ConfKey_t _name, VariableOption_t _vo) {
    return m_dataStore.getHist2D(_name, _vo);
  }

  /**
   * Passthrough function, see imp in DataStore
   * @return Vector< pair< name, VariableOption_t > > of all entries which have a != TH!F pointer
   */
  VariableOptionVector_t CounterBase::getAllHistograms() {
    return m_dataStore.getAllHistograms();
  }
  
  /**
   * Statistics on how many times this Counter object has been called.
   * @return The number of calls.
   */
  UInt_t CounterBase::getCalls() {
    return m_calls;
  }
  
  /**
   * Get the unique (within its parent Monitor) name of the counter.
   * @return Const reference to name of this counter.
   */
  const std::string& CounterBase::getName() {
    return m_name;
  }
  
  /**
   * Get the ID number of the counter. Not used for bookkeeping.
   * @return Stored ID number of counter.
   */
  UInt_t CounterBase::getID() {
    return m_ID;
  }
  
  /**
   * Get the ID numbers of the counter. For counters with multiple IDs, like ROBINs with many ROBs.
   * @return Set of stored ID numbers of counter.
   */
  const std::set<UInt_t>& CounterBase::getMultiID() {
    return m_multiId;
  }
  
  /**
   * Records additional ID number for counter. For counters with multiple IDs, like ROBINs with many ROBs.
   * @param _multiId The (additional) ID to store. Ignores duplicates.
   */
  void CounterBase::addMultiID(UInt_t _multiId) {
    m_multiId.insert( _multiId );
  }
  
  /**
   * Alternate method format, passing both variable name and option in a pair.
   * @see getHist(std::string _name, VariableOption_t _vo)
   * @return The corresponding histogram pointer for this variable and VariableOption.
   */
  TH1F* CounterBase::getHist( ConfVariableOptionPair_t _pair ) {
    return getHist(_pair.first, _pair.second);
  }
  
  /**
   * For some counters, like algorithm ones, it is handy to decorate the counter with additional information.
   * Such as the algorithm class, the algorithm's chain.
   * @param _key ConfKey_t key to associate the data with.
   * @param _value Data to save
   */
  void CounterBase::decorate( ConfKey_t _key, const std::string _value ) {
    m_strDecorations[_key] = _value;
  }

  /**
   * For some counters, like algorithm ones, it is handy to decorate the counter with additional information.
   * @param _key ConfKey_t key to associate the data with.
   * @param _value Data to save
   */
  void CounterBase::decorate( ConfKey_t _key, const Float_t _value ) {
    m_decorations[_key] = _value;
  }

  /**
   * Additional decorate call for Int_t values
   * @param _key ConfKey_t key to associate the data with.
   * @param _value Data to save
   */
  void CounterBase::decorate( ConfKey_t _key, Int_t _value ) {
    m_intDecorations[_key] = _value;
  }
  
  /**
   * Get a string decoration.
   * @see decorate( ConfKey_t _key, const std::string _value ) 
   * @return Const reference to decoration.
   */
  const std::string& CounterBase::getStrDecoration( ConfKey_t _key ) {
    if ( m_strDecorations.count( _key ) == 0) { 
      Warning("CounterBase::getStrDecoration", "Unknown decoration %s requested in %s.", Config::config().getStr(_key).c_str(), getName().c_str());
      return Config::config().getStr(kUnknownString);
    }
    return m_strDecorations[_key];
  }

  /**
   * Get a float decoration.
   * @see decorate( ConfKey_t _key, Float_t _value ) 
   * @return Const reference to float decoration
   */
  Float_t CounterBase::getDecoration( ConfKey_t _key ) {
    if ( m_decorations.count( _key ) == 0) { 
      Warning("CounterBase::getDecoration", "Unknown decoration %s requested in %s.", Config::config().getStr(_key).c_str(), getName().c_str());
      return 0.;
    }
    return m_decorations[_key];
  }

  /**
   * Get a int decoration.
   * @see decorate( ConfKey_t _key, Int_t _value ) 
   * @return Const reference to int decoration
   */
  Int_t CounterBase::getIntDecoration( ConfKey_t _key ) {
    if ( m_intDecorations.count( _key ) == 0) { 
      Warning("CounterBase::getIntDecoration", "Unknown decoration %s requested in %s.", Config::config().getStr(_key).c_str(), getName().c_str());
      return 0.;
    }
    return m_intDecorations[_key];
  }
  
} // namespace TrigCostRootAnalysis
