// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterBase_H
#define TrigCostRootAnalysis_CounterBase_H

// STL include(s):
#include <string>
#include <set>
#include <utility>
#include <limits.h>

// ROOT include(s):
#include <Rtypes.h>

// Local include(s):
#include "DataStore.h"
#include "TrigCostData.h"
#include "Utility.h"

//Forward declaration
class TH1F;
class TH2F;

namespace TrigCostRootAnalysis {
  //Forward declaration
  class TrigCostData;
  class MonitorBase;

  /**
   * @class CounterBase
   * Base class for counter objects. A counter will in general correspond to a monitored object, such as a chain
   * or alg. It is responsible for monitoring that object throughout the run.
   * A counter must have name and ID. These will usually be related (e.g. chain ID and chain name) but they do not
   * have to be. The string part must be unique within the parent.
   *
   * Objects of this type are owned by their parent MonitorBase derived class.
   *
   * This base class pure is virtual. Only derived implimentations can be instantiated.
   */
  class CounterBase {
  public:
    CounterBase(const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel = 10,
                MonitorBase* _parent = 0);
    virtual ~CounterBase();

    virtual void startEvent() = 0;
    virtual void processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight = 1) = 0;
    virtual void endEvent(Float_t _weight = 1) = 0;

    Float_t getValue(ConfKey_t _name, VariableOption_t _vo) const;
    Bool_t getValueExists(ConfKey_t _name, VariableOption_t _vo) const;
    Float_t getValueError(ConfKey_t _name, VariableOption_t _vo) const;
    Float_t getValueNormalised(ConfKey_t _name, VariableOption_t _vo) const;
    Int_t getEntries(ConfKey_t _name, VariableOption_t _vo) const;
    void setValue(ConfKey_t _name, VariableOption_t _vo, Float_t _val);
    void setEntries(ConfKey_t _name, VariableOption_t _vo, UInt_t _val);
    void setErrorSquared(ConfKey_t _name, VariableOption_t _vo, Float_t _val);
    TH1F* getHist(ConfKey_t _name, VariableOption_t _vo);
    TH1F* getHist(ConfVariableOptionPair_t _pair);
    // TH2F* getHist2D(ConfKey_t _name, VariableOption_t _vo);
    UInt_t getCalls() const;
    VariableOptionVector_t getAllHistograms();
    const std::string& getName() const;
    Int_t getID() const;
    void addMultiID(UInt_t _multiId);
    const std::set<UInt_t>& getMultiID() const;
    void decorate(ConfKey_t _key, const std::string _value);
    void decorate(ConfKey_t _key, const Float_t _value);
    void decorate(ConfKey_t _key, const Int_t _value);
    const std::string& getStrDecoration(ConfKey_t _key) const;
    Int_t getIntDecoration(ConfKey_t _key) const;
    Float_t getDecoration(ConfKey_t _key) const;
    Bool_t hasDecoration(ConfKey_t _key) const;
    MonitorBase* getParent() const;
    const std::string getRange() const;
  protected:
    virtual Double_t getPrescaleFactor(UInt_t _e = INT_MAX) = 0;


    const TrigCostData* m_costData; //!< The raw data
    DataStore m_dataStore; //!< Structured storage of primatives and histograms for use by counters.
    MonitorBase* m_parent; //!< Monitor which ownes me
    confStringMap_t m_strDecorations; //!<< Storage for additional string,string pairs of details.
    confFloatMap_t m_decorations; //!<< Storage for additional string,string pairs of details.
    confIntMap_t m_intDecorations; //!<< Storage for additional string,int pairs of details.
    UInt_t m_detailLevel; //!< Level of detail to use when saving data. Lower valus use fewer histograms, less space and
                          // less time. 10=max
    const std::string m_name; //!< Counter's name. Mapped quantity. Must be unique within parent Monitor
    std::set<UInt_t> m_multiId; //!< Records IDs for counters which may have many IDs mapping to one counter (E.g. ROBs
                                // to a ROBIN)
    UInt_t m_calls; //!< Record of how many times counter has been called
  }; //class CounterBase
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterBase_H
