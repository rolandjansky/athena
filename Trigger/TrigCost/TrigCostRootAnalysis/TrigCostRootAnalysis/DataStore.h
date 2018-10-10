// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_DataStore_H
#define TrigCostRootAnalysis_DataStore_H

// STL include(s):
#include <string>
#include <map>
#include <limits.h>

// ROOT include(s):
#include <Rtypes.h>

// Local include(s):
#include "Utility.h"

//Forward declaration
class TH1F;
class TH2F;

namespace TrigCostRootAnalysis {
  //Forward declaration
  class DataVariable;
  class CounterBase;

  /**
   * @class DataStore
   *
   * Handles the tabulation of monitored quantities. These are registered by a counter and refered to by name.
   * The DataStore takes care of correctly keeping tabs on what quantities are stored per-event, normalisation,
   * and what quantities are also to be histogrammed. All recorded quantities are of Float_t(TH1F) type (due to the
   * possibility of weights). Options available per monitored variable:
   * - savePerCall: Value is stored for each call to DataStore
   * - savePerEvent: Each call to DataStore is added to an internal buffer, this is stored at the end of the event.
   * - savePerPeriod: Same as savePerEvent, except with manual flushing of the buffer with endPeriod(variableName).
   * - savePerEventFraction: Save a value which is divided by another quantity only available at the end of the event.
   *                         You must call setVariableDenominator(variableName,denominator) before endEvent() is called.
   * @see Utility.h
   */

  class DataStore {
  public:
    DataStore(CounterBase* parent = 0);
    ~DataStore();
    DataStore& newVariable(ConfKey_t name);
    void setSaveMode(ConfKey_t, VariableOption_t vo, std::string title = "");
    DataStore& setSavePerCall(std::string title = "");
    DataStore& setSavePerEvent(std::string title = "");
    DataStore& setSavePerPeriod(std::string title = "");
    DataStore& setSavePerEventFraction(std::string title = "");
    void setHistogramming(Bool_t histogrammingEnabled = kTRUE);
    void setBinLabels(ConfKey_t name, VariableOption_t vo, std::vector<std::string> titles);
    void endEvent();
    void setVariableDenominator(ConfKey_t name, Float_t denominator);
    void store(ConfKey_t name, Float_t value, Float_t weight = 1.);
    Float_t getValue(ConfKey_t name, VariableOption_t vo) const;
    Bool_t getValueExists(ConfKey_t name, VariableOption_t vo) const;
    Float_t getValueError(ConfKey_t name, VariableOption_t vo) const;
    Int_t getEntries(ConfKey_t name, VariableOption_t vo) const;
    void setValue(ConfKey_t name, VariableOption_t vo, Float_t val);
    void setEntries(ConfKey_t name, VariableOption_t vo, UInt_t val);
    void setErrorSquared(ConfKey_t name, VariableOption_t vo, Float_t val);
    TH1F* getHist(ConfKey_t name, VariableOption_t vo, Bool_t silent = kFALSE);
    void setHist(ConfKey_t name, VariableOption_t vo, TH1F* hist);
    // TH2F* getHist2D(ConfKey_t name, VariableOption_t vo);
    Bool_t getHistogramming() const {return m_histogrammingEnabled;}
    CounterBase* getParent() const {return m_parent;}
    VariableOptionVector_t getAllHistograms();
    const std::string& getNameOfMostRecentCall() const;
  private:
    Bool_t checkRegistered(ConfKey_t name, Bool_t silent = kFALSE) const;
    DataStore& setSaveInternal(VariableOption_t vo, std::string& title, ConfKey_t name = kBlankString);
    Bool_t m_histogrammingEnabled; //!< If histograming is to be unsed on newly created VariableOptions. Designed to be
                                   // toggled on and off.
    mutable DataVariable* m_mostRecent; //!< Pointer to most recently created/modified variable, used with the chaining
                                        // operators.
    std::map< ConfKey_t, DataVariable* > m_variableMap; //!< Internal map of DataVariable objects, these hold the
                                                        // primitive types and histograms.
    CounterBase* m_parent; //!< Pointer to the CounterBase which (may) own this object.
  }; //class DataStore
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_DataStore_H
