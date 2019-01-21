// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterBase.h"
#include "../TrigCostRootAnalysis/DataStore.h"
#include "../TrigCostRootAnalysis/DataVariable.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"

// ROOT include(s):
#include <TH1.h>
#include <TH2.h>
#include <TError.h>
#include <TMath.h>

namespace TrigCostRootAnalysis {
  /**
   * DataVariable constructor, takes a pointer to its owner
   */
  DataVariable::DataVariable(DataStore* parent) : m_parentDataStore(parent) {
    // Initialise pointers to NULL
    for (UInt_t i = 0; i < kVariableOption_SIZE; ++i) {
      m_dataMap[ (VariableOption_t) i ] = NULL;
    }
  }

  /**
   * DataVariable destructor. Frees memory from all Data*
   */
  DataVariable::~DataVariable() {
    std::map< VariableOption_t, Data* >::iterator it = m_dataMap.begin();
    for (; it != m_dataMap.end(); ++it) {
      delete it->second;
    }
    m_dataMap.clear();
  }

  /**
   * Data constructor, takes no arguments. Initialises to empty.
   */
  DataVariable::Data::Data() : m_entries(0), m_data(0), m_bufferVal(0), m_bufferValW(0), m_bufferValW2(0),
    m_sumw2(0), m_denominator(0), m_usedInEvent(kFALSE), m_histoTitle(), m_hist(0) {
    // Nothing here
  }

  /**
   * Data destructor. Deletes internal histograms and string.
   */
  DataVariable::Data::~Data() {
    delete m_hist;
    // delete m_hist2D;
    delete m_histoTitle;
  }

  /**
   * Add a new save state (VariableOption) to this DataVariable.
   * @param vo Variable option defining which save state to add and enable.
   * @param title If not "", then enable histogramming.
   */
  void DataVariable::registerSaveState(VariableOption_t vo, std::string& title) {
    Data* data = new Data;

    // Only create histograms if we are going to output a ROOT file.
    // Actual memory allocation hapens just before first fill - as many declared histos don't actually ever get used
    data->m_hist = 0;
    // //TODO make use of the TH2F pointer
    // data->m_hist2D = 0;
    data->m_entries = 0.;
    data->m_data = 0.;
    data->m_sumw2 = 0.;
    data->m_bufferVal = 0.;
    data->m_bufferValW = 0.;
    data->m_bufferValW2 = 0.;
    data->m_denominator = 0;
    data->m_usedInEvent = kFALSE;
    data->m_histoTitle = 0;
    if (title != Config::config().getStr(kBlankString)
        && m_parentDataStore->getHistogramming() == kTRUE
        && (Config::config().getInt(kOutputRoot) || Config::config().getInt(kOutputImage))) {
      data->m_histoTitle = new std::string(title);
      // If this is the "dummy" counter, then we must make all of its histograms now.
      // This is because the dummy counter is used to determine later what output histograms are to be expected.
      CounterBase* holdingCounter = m_parentDataStore->getParent();
      if (holdingCounter != 0 && holdingCounter->getName() == Config::config().getStr(kDummyString)) {
        makeHist(data);
      }
    }
    if (m_dataMap.count(vo) == 1 && m_dataMap[vo] != NULL) {
      delete m_dataMap[vo];
      Warning("DataVariable::registerSaveState",
              "DataVariable %s in %s already exists. Replacing this DataVariable with a new one",
              VariableOptionStr[vo].c_str(),
              m_parentDataStore->getNameOfMostRecentCall().c_str());
    }
    m_dataMap[vo] = data;
  }

  /**
   * Set custom X axis bin lables for a histogram.
   * @param vo Variable option to edit.
   * @param titles Vector of histogram X axis bin titles
   */
  void DataVariable::setBinLabels(VariableOption_t vo, std::vector<std::string> titles) {
    if (getHist(vo) == 0 && getHistTitle(vo) != 0) {
      // If the histogram hasn't been made yet, then we need to make it now to set its bin properties.
      makeHist(getData(vo));
    } else if (getHist(vo) == 0) {
      Error("DataVariable::setBinLabels", "Cannot find a histogram for %s / %s",
            m_parentDataStore->getNameOfMostRecentCall().c_str(),
            VariableOptionStr[vo].c_str());
      return;
    }
    TH1F* h = getHist(vo);
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 0, 0)
    h->SetCanExtend(TH1::kNoAxis);
#else
    h->SetBit(TH1::kCanRebin, kFALSE);
#endif
    h->SetBins((Int_t) titles.size(), 0., (Float_t) titles.size());
    for (UInt_t i = 0; i < titles.size(); ++i) {
      h->GetXaxis()->SetBinLabel((Int_t) i + 1, titles.at(i).c_str());
    }
  }

  /**
   * Record a value (see DataStore for what can be monitored).
   * @see DataStore()
   * @param value Value to save.
   * @param weight Weight to be applied to value.
   */
  void DataVariable::store(Float_t value, Float_t weight) {
    // Here the logic of the different types of storage come in.

    // SavePerCall. Every call of store records this variable.
    dataSave(m_dataMap[kSavePerCall], value, weight);

    // SavePerEvent. Every call of store buffers this variable.
    dataBuffer(m_dataMap[kSavePerEvent], value, weight);

    // SavePerPeriod. Every call of store buffers this variable.
    dataBuffer(m_dataMap[kSavePerPeriod], value, weight);

    // SavePerEventFraction. Every call of store buffers this variable.
    dataBuffer(m_dataMap[kSavePerEventFraction], value, weight);
  }

  /**
   * Used specifically with kSavePerEventFraction to set the denominator for this event.
   * @see DataStore()
   * @see kSavePerEventFraction
   * @param denominator Denominator to divide through by at end of event.
   */
  void DataVariable::setVariableDenominator(Float_t denominator) {
    if (m_dataMap[kSavePerEventFraction] == 0) {
      Error("DataVariable::setVariableDenominator",
            "Per event fractional output not setup for this variable, %s, call savePerEventFraction() first.",
            m_parentDataStore->getNameOfMostRecentCall().c_str());
      return;
    }
    m_dataMap[kSavePerEventFraction]->m_denominator = denominator;
  }

  /**
   * Record per event values from internall buffers and clear buffers
   */
  void DataVariable::endEvent() {
    // SavePerCall, nothing to do.
    // ~nothing here~

    // SavePerEvent, save what's in the buffer
    dataSaveBuffer(m_dataMap[kSavePerEvent]);

    // SavePerPeriod. nothing to do - invoked manually
    // ~nothing here~

    // SavePerEventFraction. save what's in the buffer divided by the denominator
    dataSaveFractionBuffer(m_dataMap[kSavePerEventFraction]);
  }

  /**
   * Record per-period (general use) values from internall buffers and clear buffers.
   */
  void DataVariable::endPeriod() {
    // Like endEvent, but specific for user specified per-period monitor rather than generic per-event one
    dataSaveBuffer(m_dataMap[kSavePerPeriod]);
  }

  /**
   * Return the stored data for a VariableOption.
   * @param vo Which VariableOption_t to retrieve the stored data from.
   * @return The stored value.
   */
  Float_t DataVariable::getValue(VariableOption_t vo) const {
    if (checkRegistered(vo) == kFALSE) return 0;

    return m_dataMap.at(vo)->m_data;
  }

  /**
   * Return if there is stored data for a VariableOption.
   * @param vo Which VariableOption_t to check
   * @return If there is stored data
   */
  Bool_t DataVariable::getValueExists(VariableOption_t vo) const {
    return checkRegistered(vo, /*silent*/ kTRUE);
  }

  /**
   * Return the error on a stored data for a VariableOption.
   * @param vo Which VariableOption_t to retrieve the stored data from.
   * @return The stored value.
   */
  Float_t DataVariable::getValueError(VariableOption_t vo) const {
    if (checkRegistered(vo) == kFALSE) return 0;                                                      
    ///XXX TODO FIRST PASS - MAY NOT BE CORRECT
    Float_t sumw2 = m_dataMap.at(vo)->m_sumw2;
    //Float_t entries = m_dataMap[vo]->m_entries;
    return (Float_t) TMath::Sqrt(sumw2);
  }

  /**
   * Set the stored data for a VariableOption (does not affect entries).
   * @param vo Which VariableOption_t to retrieve the stored data from.
   * @param val The value to set for the stored value.
   */
  void DataVariable::setValue(VariableOption_t vo, Float_t val) {
    if (checkRegistered(vo) == kFALSE) return;

    m_dataMap[vo]->m_data = val;
  }

  /**
   * Set the number of entries for a VariableOption. Be careful with this one.
   * @param vo Which VariableOption_t to retrieve the stored data from.
   * @param val The value to set for the numer of entries.
   */
  void DataVariable::setEntries(VariableOption_t vo, UInt_t val) {
    if (checkRegistered(vo) == kFALSE) return;

    m_dataMap[vo]->m_entries = val;
  }

  /**
   * Set the SQUARE of the error (the internal sumw2 vale). Be cafeful with this one.
   * @param vo Which VariableOption_t to retrieve the stored data from.
   * @param val The value to use for sumw2.
   */
  void DataVariable::setErrorSquared(VariableOption_t vo, Float_t val) {
    if (checkRegistered(vo) == kFALSE) return;

    m_dataMap[vo]->m_sumw2 = val;
  }

  /**
   * Return the number of stored entries VariableOption.
   * @param vo Which VariableOption_t to retrieve the stored data from.
   * @return The numer of stored entries.
   */
  Int_t DataVariable::getEntries(VariableOption_t vo) const {
    if (checkRegistered(vo) == kFALSE) return 0;

    return m_dataMap.at(vo)->m_entries;
  }

  /**
   * Get pointer to stored histogram for export.
   * @param vo Which VariableOption_t to retrieve the stored data from.
   * @return TH1F histogram pointer.
   */
  TH1F* DataVariable::getHist(VariableOption_t vo, Bool_t silent) {
    if (checkRegistered(vo, silent) == kFALSE) return 0;

    return m_dataMap[vo]->m_hist;
  }

  /**
   * Set pointer to stored histogram for export.
   * @param vo Which VariableOption_t to retrieve the stored data from.
   * @param hist TH1F histogram pointer.
   */
  void DataVariable::setHist(VariableOption_t vo, TH1F* hist) {
    if (checkRegistered(vo, kFALSE) == kFALSE) return;

    if (m_dataMap[vo]->m_hist != 0) {
      Warning("DataVariable::setHist", "Overwriting existing hist!");
      delete m_dataMap[vo]->m_hist;
    }
    m_dataMap[vo]->m_hist = hist;
  }

  /**
   * Get pointer to histo title string. This is !=0 if a histogram has been requested but has not yet
   * been made on the heap becasue no Fill call has propagated to it.
   * @param vo Which VariableOption_t to retrieve the stored data from.
   * @return Pointer to std::string of title or 0.
   */
  std::string* DataVariable::getHistTitle(VariableOption_t vo) const {
    if (checkRegistered(vo, /*silent*/ kTRUE) == kFALSE) return 0;

    return m_dataMap.at(vo)->m_histoTitle;
  }

  /**
   * Get pointer to stored histogram for export.
   * @param vo Which VariableOption_t to retrieve the stored data from.
   * @return TH2F histogram pointer.
   */
  // TH2F* DataVariable::getHist2D(VariableOption_t vo) {
  //   if ( checkRegistered(vo) == kFALSE) return 0;
  //   return m_dataMap[vo]->m_hist2D;
  // }

  /**
   * Internal call to save a value.
   * @param data Pointer to data struct.
   * @param value Value to save.
   * @param weight Value weight.
   */
  void DataVariable::dataSave(Data* data, Float_t value, Float_t weight) {
    if (data == 0) return;

    data->m_entries++;
    data->m_data += (value * weight);
    data->m_sumw2 += (value * weight * weight);
    if (data->m_hist == 0 && data->m_histoTitle != 0) makeHist(data);
    if (data->m_hist != 0) {
#ifdef MTHREAD
      static std::mutex mutex;
#endif
      MUTEX_ON
      data->m_hist->Fill(value, weight);
      MUTEX_OFF
    }
  }

  /**
   * Internal call to buffered a value.
   * @param data Pointer to data struct.
   * @param value Value to save.
   * @param weight Value weight.
   */
  void DataVariable::dataBuffer(Data* data, Float_t value, Float_t weight) {
    if (data == 0) return;

    // Now we need to keep track of the values and their errors
    //
    // bufferValW is the sum of weighted value to add to m_data (the total weighted value)
    // bufferValW2 is the sum of [value times square of weight] to add to m_sumw2 (the error on the total weighted value
    // squared)
    //
    // bufferVal is the unweighed sum, it is needed as the raw value to fill the histogram with.
    // (bufferValW / bufferVal) can be used to get the average weight to use when filling the histogram
    //
    data->m_bufferVal += value;
    data->m_bufferValW += value * weight;
    data->m_bufferValW2 += value * weight * weight;

    data->m_usedInEvent = kTRUE;
  }

  /**
   * Internal call flush the buffered values.
   * @param data Pointer to data struct.
   */
  void DataVariable::dataSaveBuffer(Data* data) {
    if (data == 0 || data->m_usedInEvent == kFALSE) return;

    data->m_entries++;

    data->m_data += data->m_bufferValW;
    data->m_sumw2 += data->m_bufferValW2;

    if (data->m_hist == 0 && data->m_histoTitle != 0) makeHist(data);
    if (data->m_hist != 0) {
      Float_t effectiveWeight = 0;
      if (!isZero(data->m_bufferVal)) effectiveWeight = data->m_bufferValW / data->m_bufferVal;
      //TODO what if m_bufferVal is zero? Could still have a large weight we want on the histogram?
#ifdef MTHREAD
       static std::mutex mutex;
#endif
      MUTEX_ON
      data->m_hist->Fill(data->m_bufferVal, effectiveWeight);  //TODO double check
      MUTEX_OFF
    }

    data->m_bufferVal = 0.;
    data->m_bufferValW = 0.;
    data->m_bufferValW2 = 0.;
    data->m_usedInEvent = kFALSE;
  }

  /**
   * Internal call flush the buffered values for averaged quantity.
   * @param data Pointer to data struct.
   */
  void DataVariable::dataSaveFractionBuffer(Data* data) {
    if (data == 0 || data->m_usedInEvent == kFALSE) return;

    if (isZero(data->m_denominator) == kTRUE) {
      if (Config::config().getDisplayMsg(kMsgDivZero) == kTRUE) {
        Warning("DataVariable::dataSaveFractionBuffer", "Denominator of zero for per event fraction of %s [/0]."
                                                        "Make sure you call !=0 setVariableDenominator() before endEvent(). Save skipped.",
                m_parentDataStore->getNameOfMostRecentCall().c_str());
      }
    } else {
      data->m_entries++;

      data->m_data += data->m_bufferValW / data->m_denominator;
      data->m_sumw2 += data->m_bufferValW2 / data->m_denominator;

      if (data->m_hist == 0 && data->m_histoTitle != 0) makeHist(data);
      if (data->m_hist != 0) {
        Float_t effectiveWeight = 0;
        if (!isZero(data->m_bufferVal)) effectiveWeight = data->m_bufferValW / data->m_bufferVal;
        //TODO what if m_bufferVal is zero? Could still have a large weight we want on the histogram?
        //NOTE We assume here that the demoninator is weighted hence take the weighted numerator too.
        //If this is ever not the case, may want to put a flag in
#ifdef MTHREAD
        static std::mutex mutex;
#endif
        MUTEX_ON
        data->m_hist->Fill(data->m_bufferValW / data->m_denominator, effectiveWeight); //TODO check, do we divide
                                                                                           // effW by denom?
        MUTEX_OFF
      }
    }

    data->m_bufferVal = 0.;
    data->m_bufferValW = 0.;
    data->m_bufferValW2 = 0.;
    data->m_denominator = 0.;
    data->m_usedInEvent = kFALSE;
  }

  /**
   * Static member variable to count the total number of histograms we have made.
   * Top tip: it's lots of histograms.
   */
  UInt_t DataVariable::s_globalHistId = 0;

  /**
   * Only create a new histogram on the heap once we're sure it's actually going to be used.
   * @param data Pointer to data struct.
   */
  void DataVariable::makeHist(Data* data) {
#ifdef MTHREAD
    static std::mutex mutex;
#endif
    MUTEX_ON
    std::string name;
    CounterBase* holdingCounter = m_parentDataStore->getParent();

    if (holdingCounter != 0) name += holdingCounter->getName();
    if (name != "") name = name.substr(0, 10); // Restrict to 10 characters
    name += std::string("h") + intToString(s_globalHistId++, 6);
    if (Config::config().getIsSet(kHistBinMin) == kTRUE && Config::config().getIsSet(kHistBinMax) == kTRUE) {
      // Special mode - just for making custom plots - specify axis range used for every histo
      data->m_hist = new TH1F(name.c_str(), data->m_histoTitle->c_str(),
                               Config::config().getInt(kHistBins),
                               Config::config().getFloat(kHistBinMin),
                               Config::config().getFloat(kHistBinMax));
    } else {
      // Standard mode - automatic bin widths
      data->m_hist = new TH1F(name.c_str(), data->m_histoTitle->c_str(), Config::config().getInt(kHistBins), 0, 0);
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 0, 0)
      data->m_hist->SetCanExtend(TH1::kAllAxes);
#else
      data->m_hist->SetBit(TH1::kCanRebin, kTRUE);
#endif
    }
    MUTEX_OFF
    delete data->m_histoTitle; //No longer need to keep this title floating about, it's stored in this histogram
    data->m_histoTitle = 0;
  }

  /**
   * Check if map contains Data pointer for given VariableOption.
   * Optimisation note: this is a very high-call-rate function.
   * @param vo VariableOption_t to check.
   * @param silent Set to true to suppress error messages.
   * @return kTRUE if pointer is not null.
   */
  Bool_t DataVariable::checkRegistered(VariableOption_t vo, Bool_t silent) const {
    if (m_dataMap.at(vo) == 0) {
      if (silent == kFALSE && Config::config().getDisplayMsg(kMsgCannotFindVO) == kTRUE) {
        Error("DataVariable::checkRegistered", "VariableOption_t %s is not registered for this DataStore entry: %s.",
              VariableOptionStr[vo].c_str(),
              m_parentDataStore->getNameOfMostRecentCall().c_str());
      }
      return kFALSE;
    }
    return kTRUE;
  }

  /**
   * Get data pointer for a given VO
   * @param vo VariableOption_t to check.
   * @return Data pointer or 0.
   */
  DataVariable::Data* DataVariable::getData(VariableOption_t vo) {
    if (checkRegistered(vo) == kFALSE) return 0;

    return m_dataMap[vo];
  }
} // namespace TrigCostRootAnalysis
