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
    DataStore(CounterBase* _parent = 0);
    ~DataStore();
    DataStore& newVariable(ConfKey_t _name);
    void setSaveMode(ConfKey_t, VariableOption_t _vo, std::string _title = "");
    DataStore& setSavePerCall(std::string _title = "");
    DataStore& setSavePerEvent(std::string _title = "");
    DataStore& setSavePerPeriod(std::string _title = "");
    DataStore& setSavePerEventFraction(std::string _title = "");
    void setHistogramming(Bool_t _histogrammingEnabled = kTRUE);
    void setBinLabels(ConfKey_t _name, VariableOption_t _vo, std::vector<std::string> _titles);
    void endEvent();
    void setVariableDenominator(ConfKey_t _name, Float_t _denominator);
    void store(ConfKey_t _name, Float_t _value, Float_t _weight = 1.);
    Float_t getValue(ConfKey_t _name, VariableOption_t _vo) const;
    Bool_t getValueExists(ConfKey_t _name, VariableOption_t _vo) const;
    Float_t getValueError(ConfKey_t _name, VariableOption_t _vo) const;
    Int_t getEntries(ConfKey_t _name, VariableOption_t _vo) const;
    void setValue(ConfKey_t _name, VariableOption_t _vo, Float_t _val);
    void setEntries(ConfKey_t _name, VariableOption_t _vo, UInt_t _val);
    void setErrorSquared(ConfKey_t _name, VariableOption_t _vo, Float_t _val);
    TH1F* getHist(ConfKey_t _name, VariableOption_t _vo, Bool_t _silent = kFALSE);
    void setHist(ConfKey_t _name, VariableOption_t _vo, TH1F* _hist);
    // TH2F* getHist2D(ConfKey_t _name, VariableOption_t _vo);
    Bool_t getHistogramming() const {return m_histogrammingEnabled;}
    CounterBase* getParent() const {return m_parent;}
    VariableOptionVector_t getAllHistograms();
    const std::string& getNameOfMostRecentCall() const;
  private:
    Bool_t checkRegistered(ConfKey_t _name, Bool_t _silent = kFALSE) const;
    DataStore& setSaveInternal(VariableOption_t _vo, std::string& _title, ConfKey_t _name = kBlankString);
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
