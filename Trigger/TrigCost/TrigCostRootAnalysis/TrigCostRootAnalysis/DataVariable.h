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
    DataVariable(DataStore* parent);
    ~DataVariable();
    void registerSaveState(VariableOption_t vo, std::string& title);
    void store(Float_t value, Float_t weight = 1.);
    void setVariableDenominator(Float_t denominator);
    void setBinLabels(VariableOption_t vo, std::vector<std::string> titles);
    void endEvent();
    void endPeriod();
    Float_t getValue(VariableOption_t vo) const;
    Bool_t getValueExists(VariableOption_t vo) const;
    Float_t getValueError(VariableOption_t vo) const;
    Int_t getEntries(VariableOption_t vo) const;
    void setValue(VariableOption_t vo, Float_t val);
    void setEntries(VariableOption_t vo, UInt_t val);
    void setErrorSquared(VariableOption_t vo, Float_t val);
    TH1F* getHist(VariableOption_t vo, Bool_t silent = kFALSE);
    std::string* getHistTitle(VariableOption_t vo) const;
    void setHist(VariableOption_t vo, TH1F* hist);
    // TH2F* getHist2D(VariableOption_t vo);

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
      Float_t m_bufferVal; //!< Temporary buffer for tabulating quantites which are stored per event
      Float_t m_bufferValW; //!< Temporary buffer for tabulating value * weight of quantites which are stored per event
      Float_t m_bufferValW2; //!< Temp buffer for tabulatign the value * weight^2 of qunatities which are stored per
                             // event
      Float_t m_sumw2; // Sum of square of weights (scaled by values)
      Float_t m_denominator; //!< Denominator for averaged quantities
      Bool_t m_usedInEvent;
      std::string* m_histoTitle; //!< Pointer to string which will hold the histogram title up to the point that we need
                                 // to make a histo, then it's deleted
      TH1F* m_hist; //<! Internal histogram pointer
      // TH2F* m_hist2D; //<! Internal histogram pointer
      Data();
      ~Data();
    };

    void makeHist(Data* data);
    void dataSave(Data* data, Float_t value, Float_t weight);
    void dataBuffer(Data* data, Float_t value, Float_t weight);
    void dataSaveBuffer(Data* data);
    void dataSaveFractionBuffer(Data* data);
    Bool_t checkRegistered(VariableOption_t vo, Bool_t silent = kFALSE) const;
    DataVariable::Data* getData(VariableOption_t vo);

    std::map< VariableOption_t, Data* > m_dataMap; //!< Map of Data structs for each option
    DataStore* m_parentDataStore; //!< Pointer to my parent, used mostly for debug
  }; //class DataVariable
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_DataVariable_H
