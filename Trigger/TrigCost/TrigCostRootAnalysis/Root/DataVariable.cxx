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
   * DataVariable constructor, takes no arguments.
   */
  DataVariable::DataVariable(DataStore* _parent) : m_parentDataStore(_parent) {
    // Initialise pointers to NULL
    for (UInt_t i = 0; i < kVariableOption_SIZE; ++i) {
      m_dataMap[ (VariableOption_t) i ] = NULL;
    }
  }
  
  /**
   * DataVariable destructor. Frees memory from all Data*
   */
  DataVariable::~DataVariable() {
    std::map< VariableOption_t, Data* >::iterator _it = m_dataMap.begin();
    for (; _it != m_dataMap.end(); ++_it) {
      delete _it->second;
    }
    m_dataMap.clear();
  }
  
  /**
   * Data constructor, takes no arguments. Initialises to empty.
   */
  DataVariable::Data::Data() : m_hist(0), m_hist2D(0) {
    // Nothing here
  }
  
  /**
   * Data destructor. Deletes internal histograms and map.
   */
  DataVariable::Data::~Data() {
    delete m_hist;
    delete m_hist2D;
    delete m_histoTitle;
  }
  
  /**
   * Add a new save state (VariableOption) to this DataVariable.
   * @param _vo Variable option defining which save state to add and enable.
   * @param _title If not "", then enable histogramming.
   */
  void DataVariable::registerSaveState(VariableOption_t _vo, std::string& _title) {
    Data* _data = new Data;
    // Only create histograms if we are going to output a ROOT file.
    // Actual memory allocation hapens just before first fill - as many declared histos don't actually ever get used
    _data->m_hist = 0;
    //TODO make use of the TH2F pointer
    _data->m_hist2D = 0;
    _data->m_entries = 0.;
    _data->m_data = 0.;
    _data->m_buffer = 0.;
    _data->m_bufferWeight = 0.;
    _data->m_denominator = 0;
    _data->m_usedInEvent = kFALSE;
    _data->m_histoTitle = 0;
    if (_title != "" 
      && m_parentDataStore->getHistogramming() == kTRUE 
      && ( Config::config().getInt(kOutputRoot) || Config::config().getInt(kOutputImage) ) )
    {
      _data->m_histoTitle = new std::string( _title );
      // If this is the "dummy" counter, then we must make all of its histograms now.
      // This is because the dummy counter is used to determine later what output histograms are to be expected.
      CounterBase* _holdingCounter = m_parentDataStore->getParent();
      if (_holdingCounter != 0 && _holdingCounter->getName() == Config::config().getStr(kDummyString) ) {
        makeHist(_data);
      }
    }
    if (m_dataMap.count(_vo) == 1 && m_dataMap[_vo] != NULL) {
      delete m_dataMap[_vo];
      Warning("DataVariable::registerSaveState", "DataVariable %s in %s already exists. Replacing this DataVariable with a new one", 
        VariableOptionStr[_vo].c_str(),
        m_parentDataStore->getNameOfMostRecentCall().c_str());
    }
    m_dataMap[_vo] = _data;
  }



  /**
   * Set custom X axis bin lables for a histogram.
   * @param _vo Variable option to edit.
   * @param _titles Vector of histogram X axis bin titles
   */
  void DataVariable::setBinLabels(VariableOption_t _vo, std::vector<std::string> _titles) {
    if ( getHist(_vo) == 0 && getHistTitle(_vo) != 0) {
      // If the histogram hasn't been made yet, then we need to make it now to set its bin properties.
      makeHist( getData(_vo) );
    } else if ( getHist(_vo) == 0) {
      Error("DataVariable::setBinLabels","Cannot find a histogram for %s / %s", 
        m_parentDataStore->getNameOfMostRecentCall().c_str(),
        VariableOptionStr[_vo].c_str() );
      return;
    }
    TH1F* _h = getHist(_vo);
    _h->SetBit(TH1::kCanRebin, kFALSE);
    _h->SetBins( (Int_t) _titles.size(), 0., (Float_t) _titles.size() );
    for (UInt_t _i = 0; _i < _titles.size(); ++_i) {
      _h->GetXaxis()->SetBinLabel( (Int_t) _i + 1, _titles.at(_i).c_str() );
    }
  }
  
  /**
   * Record a value (see DataStore for what can be monitored).
   * @see DataStore()
   * @param _value Value to save.
   * @param _weight Weight to be applied to value.
   */
  void DataVariable::store(Float_t _value, Float_t _weight) {
    // Here the logic of the different types of storage come in.

    // SavePerCall. Every call of store records this variable.
    dataSave( m_dataMap[kSavePerCall], _value, _weight );

    // SavePerEvent. Every call of store buffers this variable.
    dataBuffer( m_dataMap[kSavePerEvent], _value, _weight );

    // SavePerPeriod. Every call of store buffers this variable.
    dataBuffer( m_dataMap[kSavePerPeriod], _value, _weight );

    // SavePerEventFraction. Every call of store buffers this variable.
    dataBuffer( m_dataMap[kSavePerEventFraction], _value, _weight );

  }
  
  /**
   * Used specifically with kSavePerEventFraction to set the denominator for this event.
   * @see DataStore()
   * @see kSavePerEventFraction
   * @param _denominator Denominator to divide through by at end of event.
   */
  void DataVariable::setVariableDenominator(Float_t _denominator) {
    if (m_dataMap[kSavePerEventFraction] == 0) {
      Error("DataVariable::setVariableDenominator", "Per event fractional output not setup for this variable, %s, call savePerEventFraction() first.", 
        m_parentDataStore->getNameOfMostRecentCall().c_str() );
      return;
    }
    m_dataMap[kSavePerEventFraction]->m_denominator = _denominator;
  }
  
  /**
   * Record per event values from internall buffers and clear buffers
   */
  void DataVariable::endEvent() {
  
    // SavePerCall, nothing to do.
    // ~nothing here~
    
    // SavePerEvent, save what's in the buffer
    dataSaveBuffer( m_dataMap[kSavePerEvent] );
    
    // SavePerPeriod. nothing to do - invoked manually
    // ~nothing here~
    
    // SavePerEventFraction. save what's in the buffer divided by the denominator
    dataSaveFractionBuffer( m_dataMap[kSavePerEventFraction] );
  }
  
  /**
   * Record per-period (general use) values from internall buffers and clear buffers.
   */
  void DataVariable::endPeriod() {
    // Like endEvent, but specific for user specified per-period monitor rather than generic per-event one
    dataSaveBuffer( m_dataMap[kSavePerPeriod] );
  }
  
  /**
   * Return the stored data for a VariableOption. 
   * @param _vo Which VariableOption_t to retrieve the stored data from.
   * @return The stored value.
   */
  Float_t DataVariable::getValue(VariableOption_t _vo) {
    if ( checkRegistered(_vo) == kFALSE) return 0;
    return m_dataMap[_vo]->m_data;
  }

  /**
   * Return the error on a stored data for a VariableOption. 
   * @param _vo Which VariableOption_t to retrieve the stored data from.
   * @return The stored value.
   */
  Float_t DataVariable::getValueError(VariableOption_t _vo) {
    if ( checkRegistered(_vo) == kFALSE) return 0; ///XXX TODO THIS IS WRONG - PUT IN CORRECT FORMULA!!!
    Float_t _val = m_dataMap[_vo]->m_data;
    //Float_t _entries = m_dataMap[_vo]->m_entries;
    return (Float_t) TMath::Sqrt( _val );
  }

  /**
   * Set the stored data for a VariableOption (does not affect entries). 
   * @param _vo Which VariableOption_t to retrieve the stored data from.
   * @param _val The value to set for the stored value.
   */
  void DataVariable::setValue(VariableOption_t _vo, Float_t _val) {
    if ( checkRegistered(_vo) == kFALSE) return;
    m_dataMap[_vo]->m_data = _val;
  }

  /**
   * Set the number of entries for a VariableOption. Be careful with this one. 
   * @param _vo Which VariableOption_t to retrieve the stored data from.
   * @param _val The value to set for the numer of entries.
   */
  void DataVariable::setEntries(VariableOption_t _vo, UInt_t _val) {
    if ( checkRegistered(_vo) == kFALSE) return; 
    m_dataMap[_vo]->m_entries = _val;
  }
  
  /**
   * Return the number of stored entries VariableOption.
   * @param _vo Which VariableOption_t to retrieve the stored data from.
   * @return The numer of stored entries.
   */
  Int_t DataVariable::getEntries(VariableOption_t _vo) {
    if ( checkRegistered(_vo) == kFALSE) return 0;
    return m_dataMap[_vo]->m_entries;
  }
  
  /**
   * Get pointer to stored histogram for export.
   * @param _vo Which VariableOption_t to retrieve the stored data from.
   * @return TH1F histogram pointer.
   */
  TH1F* DataVariable::getHist(VariableOption_t _vo, Bool_t _silent) {
    if ( checkRegistered(_vo, _silent) == kFALSE) return 0;
    return m_dataMap[_vo]->m_hist;
  }
 
   /**
   * Get pointer to histo title string. This is !=0 if a histogram has been requested but has not yet
   * been made on the heap becasue no Fill call has propagated to it.
   * @param _vo Which VariableOption_t to retrieve the stored data from.
   * @return Pointer to std::string of title or 0.
   */
  std::string* DataVariable::getHistTitle(VariableOption_t _vo) {
    if ( checkRegistered(_vo, /*_silent*/ kTRUE) == kFALSE) return 0;
    return m_dataMap[_vo]->m_histoTitle;
  }


  /**
   * Get pointer to stored histogram for export.
   * @param _vo Which VariableOption_t to retrieve the stored data from.
   * @return TH2F histogram pointer.
   */
  TH2F* DataVariable::getHist2D(VariableOption_t _vo) {
    if ( checkRegistered(_vo) == kFALSE) return 0;
    return m_dataMap[_vo]->m_hist2D;
  }
  
  /**
   * Internal call to save a value.
   * @param _data Pointer to data struct.
   * @param _value Value to save.
   * @param _weight Value weight.
   */
  void DataVariable::dataSave(Data* _data, Float_t _value, Float_t _weight) {
    if (_data == 0) return;
    _data->m_entries++;
    _data->m_data += ( _value * _weight);
    if (_data->m_hist == 0 && _data->m_histoTitle != 0) makeHist(_data);
    if (_data->m_hist != 0) {
      _data->m_hist->Fill( _value, _weight );
    }
  }
  
  /**
   * Internal call to buffered a value.
   * @param _data Pointer to data struct.
   * @param _value Value to save.
   * @param _weight Value weight.
   */
  void DataVariable::dataBuffer(Data* _data, Float_t _value, Float_t _weight) {
    if (_data == 0) return;
    _data->m_buffer += _value; //TODO check
    _data->m_bufferWeight = _weight; //TODO check
    _data->m_usedInEvent = kTRUE;
  }
  
  /**
   * Internal call flush the buffered values.
   * @param _data Pointer to data struct.
   */
  void DataVariable::dataSaveBuffer(Data* _data) {
    if (_data == 0 || _data->m_usedInEvent == kFALSE) return;
    _data->m_entries++;
    _data->m_data += ( _data->m_buffer * _data->m_bufferWeight ); //TODO check
    if (_data->m_hist == 0 && _data->m_histoTitle != 0) makeHist(_data);
    if (_data->m_hist != 0) {
      _data->m_hist->Fill( _data->m_buffer, _data->m_bufferWeight ); //TODO check
    }
    _data->m_buffer = 0.;
    _data->m_bufferWeight = 0.;
    _data->m_usedInEvent = kFALSE;
  }
  
  /**
   * Internal call flush the buffered values for averaged quantity.
   * @param _data Pointer to data struct.
   */
  void DataVariable::dataSaveFractionBuffer(Data* _data) {
    if (_data == 0 || _data->m_usedInEvent == kFALSE) return;
    if (isZero(_data->m_denominator) == kTRUE) {
      Warning("DataVariable::dataSaveFractionBuffer", "Denominator of zero for per event fraction of %s [/0]."
        "Make sure you call !=0 setVariableDenominator() before endEvent(). Save skipped.", 
        m_parentDataStore->getNameOfMostRecentCall().c_str() );
    } else {
      _data->m_entries++;
      _data->m_data += ( (_data->m_buffer * _data->m_bufferWeight) / _data->m_denominator);
      if (_data->m_hist == 0 && _data->m_histoTitle != 0) makeHist(_data);
      if (_data->m_hist != 0) {
        _data->m_hist->Fill( (_data->m_buffer * _data->m_bufferWeight) / _data->m_denominator ); //TODO Think about this
      }
    }
    _data->m_buffer = 0.;
    _data->m_bufferWeight = 0.;
    _data->m_denominator = 0.;
    _data->m_usedInEvent = kFALSE;
  }
  
  /**
   * Static member variable to count the total number of histograms we have made.
   * Top tip: it's lots of histograms.
   */
  UInt_t DataVariable::s_globalHistId = 0;

  /**
   * Only create a new histogram on the heap once we're sure it's actually going to be used.
   * @param _data Pointer to data struct.
   */
  void DataVariable::makeHist(Data* _data) {
    std::string _name;
    CounterBase* _holdingCounter = m_parentDataStore->getParent();
    if (_holdingCounter != 0) _name += _holdingCounter->getName();
    if (_name != "") _name = _name.substr(0,10); // Restrict to 10 characters
    _name += std::string("_h") + intToString( s_globalHistId++, 6);
    _data->m_hist = new TH1F(_name.c_str(), _data->m_histoTitle->c_str() , Config::config().getInt(kHistBins), 0, 0);
    _data->m_hist->SetBit(TH1::kCanRebin, kTRUE);
    delete _data->m_histoTitle; //No longer need to keep this title floating about, it's stored in this histogram
    _data->m_histoTitle = 0;
  }

  /**
   * Check if map contains Data pointer for given VariableOption.
   * Optimisation note: this is a very high-call-rate function.
   * @param _vo VariableOption_t to check.
   * @param _silent Set to true to suppress error messages.
   * @return kTRUE if pointer is not null.
   */
  Bool_t DataVariable::checkRegistered(VariableOption_t _vo, Bool_t _silent) {
    if (m_dataMap[_vo] == 0) {
      if (_silent == kFALSE) {
        Error("DataVariable::checkRegistered", "VariableOption_t %s is not registered for this DataStore entry: %s.",
          VariableOptionStr[_vo].c_str(),
          m_parentDataStore->getNameOfMostRecentCall().c_str());
      }
      return kFALSE;
    }
    return kTRUE;
  }

  /**
   * Get data pointer for a given VO
   * @param _vo VariableOption_t to check.
   * @return Data pointer or 0.
   */
  DataVariable::Data* DataVariable::getData(VariableOption_t _vo) {
    if ( checkRegistered(_vo) == kFALSE) return 0;
    return m_dataMap[_vo];
  }

  
} // namespace TrigCostRootAnalysis
