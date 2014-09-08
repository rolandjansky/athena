// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_DataVariable_H
#define TrigCostRootAnalysis_DataVariable_H

// STL include(s):
#include <string>
#include <map>
#include <limits.h>

// ROOT include(s):
#include <Rtypes.h>

//Forward declaration
class TH1F;
class TH2F;

namespace TrigCostRootAnalysis {

  /**
   * @class DataVariable
   * Hold the primitives and histograms associated with a monitored variable.
   * Buffer structure held within sub-class (I promise this is the bottom of the heiararchy now!)
   *
   */
  class DataVariable {
  
   public:
   
    DataVariable(DataStore* _parent);
    ~DataVariable();
    void registerSaveState(VariableOption_t _vo, std::string& _title);
    void store(Float_t _value, Float_t _weight = 1.);
    void setVariableDenominator(Float_t _denominator);
    void setBinLabels(VariableOption_t _vo, std::vector<std::string> _titles);
    void endEvent();
    void endPeriod();
    Float_t getValue(VariableOption_t _vo);
    Float_t getValueError(VariableOption_t _vo);
    Int_t getEntries(VariableOption_t _vo);
    void setValue(VariableOption_t _vo, Float_t _val);
    void setEntries(VariableOption_t _vo, UInt_t _val);
    TH1F* getHist(VariableOption_t _vo, Bool_t _silent = kFALSE);
    std::string* getHistTitle(VariableOption_t _vo);
    TH2F* getHist2D(VariableOption_t _vo);
    
    static UInt_t s_globalHistId;

   private:
   
    /**
     * @struct Data
     * This structure contains the generic primitives and pointers to more complicated possible datastores
     * which are used by the various VariableOption_t types as the raw underlying storage mechanism.
     */
    struct Data {
      Int_t m_entries;  //!< Number of entries
      Float_t m_data; //!< Value stored
      Float_t m_buffer; //!< Temporary buffer for tabulating quantites which are stored per event
      Float_t m_bufferWeight; //!< Weight of values in buffer
      Float_t m_denominator; //!< Denominator for averaged quantities
      Bool_t  m_usedInEvent;
      std::string* m_histoTitle; //!< Pointer to string which will hold the histogram title up to the point that we need to make a histo, then it's deleted
      TH1F* m_hist; //<! Internal histogram pointer
      TH2F* m_hist2D; //<! Internal histogram pointer
      Data();
      ~Data();
    };
    
    void makeHist(Data* _data);
    void dataSave(Data* _data, Float_t _value, Float_t _weight);
    void dataBuffer(Data* _data, Float_t _value, Float_t _weight);
    void dataSaveBuffer(Data* _data);
    void dataSaveFractionBuffer(Data* _data);
    Bool_t checkRegistered(VariableOption_t _vo, Bool_t _silent = kFALSE);
    DataVariable::Data* getData(VariableOption_t _vo);
    
    std::map< VariableOption_t, Data* > m_dataMap; //!< Map of Data structs for each option
    DataStore* m_parentDataStore; //!< Pointer to my parent, used mostly for debug
    
  }; //class DataVariable
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_DataVariable_H
