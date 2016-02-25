// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorBase_H
#define TrigCostRootAnalysis_MonitorBase_H

// STL include(s):
#include <map>
#include <set>
#include <string>
#include <vector>

// ROOT include(s):
#include <TNamed.h>

//Local include(s):
#include "CounterBase.h"
#include "DataStore.h"
#include "TableValueFunctions.h"
#include "LumiCollector.h"
#include "Utility.h"
#include "Timer.h"
#include "TrigConfInterface.h"

namespace TrigCostRootAnalysis {

  //Forward declaration
  class TrigCostData;

  /**
   * @class MonitorBase
   * Base collection class to hold monitors,
   * specific implimentation handles the counter object etc. used for monitoring
   * different types. Base implimentation is pure virtual.
   * This base class also contains much of hte heavy lifting for data output.
   */
  class MonitorBase {

   public:

    MonitorBase(const TrigCostData* _costData, std::string _name);
    virtual ~MonitorBase();
    virtual void newEvent(Float_t _weight = 1.) = 0;
    virtual Bool_t getIfActive(ConfKey_t _mode);
    virtual void saveOutput();
    virtual Int_t getNCollectionsToProcess();
    virtual Bool_t isThreadable();
    const std::string& getName();
    UInt_t getLevel();
    const std::string& getLevelStr();
    void setName( const std::string& _name );
    void setLevel( UInt_t _l );
    CounterMap_t* getCounterCollection(const std::string& _identifier, const ConfKey_t _type);
    UInt_t getNCounters();
    void setPass(UInt_t _pass);
    UInt_t getPass();

    struct TableColumnFormatter {
      TableColumnFormatter(const std::string& _title,
        const std::string& _tooltip,
        ConfKey_t _dataVarialbe,
        VariableOption_t _vo,
        UInt_t _precision = 4,
        FormatterOption_t _fo = kFormatOptionNone);
      TableColumnFormatter(const std::string& _title,
        const std::string& _tooltip,
        ConfKey_t _dataVarialbeNominator,
        VariableOption_t _voNominator,
        ConfKey_t _dataVarialbeDenominator,
        VariableOption_t _voDenominator,
        UInt_t _precision = 4,
        FormatterOption_t _fo = kFormatOptionNone);
      TableColumnFormatter(const std::string& _title,
        const std::string& _tooltip,
        Float_t (*_functionPtr)(CounterMap_t*, CounterBase*),
        UInt_t _precision);
      TableColumnFormatter(const std::string& _title,
        const std::string& _tooltip,
        std::string (*_functionPtr)(CounterMap_t*, CounterBase*) );
      mutable std::string m_columnName; //!< The name of the column, mutable as this may need to be cleaned of illegal characters
      mutable std::string m_tooltip; //!< The hover tooltip for the column, mutable as this may need to be cleaned of illegal characters
      ConfKey_t m_dataVariable; //!< For a direct fetch, the name of the DataVariable to use in this column.
      VariableOption_t m_dataVO; //!< For a direct fetch, the variable option to use for the DataVariable chosen for this column.
      ConfKey_t m_dataVariableDenominator; //!< For a direct fetch, the name of the DataVariable to use as a denominator.
      VariableOption_t m_dataVODenominator; //!< For a direct fetch, the variable option to use for the DataVariable chosen as a denominator for this column.
      UInt_t m_precision; //!< How many digits after the decimal place to save.
      Float_t (*m_functionPtr)(CounterMap_t*, CounterBase*); //!< For more complicated quantities, a pointer to a function to return a float
      std::string (*m_functionPtrStr)(CounterMap_t*, CounterBase*); //!< For more complicated quantities, a pointer to a function to return a string
      FormatterOption_t m_formatOption; //!< Additional post-processing or behaviour changing options are defined here. @see FormatterOption
    };

   protected:

    const TrigCostData* m_costData; //!< Source of all data
    virtual CounterBase* newCounter( const std::string &_name, Int_t _ID ) = 0;
    std::string constructPlotName(CounterBase* _counter, ConfVariableOptionPair_t _variable);
    CounterBase* getCounter( CounterMap_t* _counterMap, const std::string &_name, Int_t _ID );
    CounterBase* addCounter( CounterMap_t* _counterMap, const std::string &_name, Int_t _ID );
    void collateCounterCollectionsForEvent();
    void addToCollectionsToProcess(const std::string &_name, UInt_t _lumiBlockNumber, Float_t _lumiLength, const ConfKey_t _type);
    void recordLumi(const std::string &_name, UInt_t _lumiBlockNumber, Float_t _lumiLength);
    void sharedHistogramOutputRoutine(VariableOptionVector_t& _toSave);
    void sharedTableOutputRoutine( const std::vector<TableColumnFormatter> &_toSave );
    void outputTableRow(CounterBase* _TCCB, std::ofstream& _fout, const std::vector<TableColumnFormatter> &_toSave, CounterMap_t *_counterMap, std::string &_counterCollectionName);
    void setDetailLevel(UInt_t _detailLevel);
    void allowSameNamedCounters();
    void allowSameIDCounters();
    void filterOutputOnStrDecoration(ConfKey_t _decoration, const std::string _value);
    void startEvent(CounterMap_t* _counters = 0);
    void endEvent(Float_t _weight = 1.);
    void enableROOTMsg();
    void disableROOTMsg();
    void checkForIllegalCharacters(std::string &_toClean, Bool_t _checkComma = kTRUE, Bool_t _checkApostrophe = kTRUE, Bool_t _checkColon = kFALSE);

    std::string m_name; //<! Name of this monitor, for use in output.
    UInt_t m_level; //<! Level of monitor (2 or 3)
    UInt_t m_pass; //<! On rare occasions we may need more than one pass through the file, this holds which pass we're on
    UInt_t m_detailLevel; //!<< How detailed this monitor is to be, passed on to created Counters
    CounterBase* m_dummyCounter; //!< Counter of appropriate type which is not used for monitoring, but can be used to reset static tabulators & generate output patterns.
    Bool_t m_allowSameNamedCounters; //!< If true, one counter is registered per unique name. If false, every call to getCounter will assign a new counter.
    Bool_t m_allowSameIDCounters; //<! For counters where there may be a many->one mapping of ID->name. Disable warning.
    Bool_t m_filterOutput; //!< Set if the user's supplied chain name filter is to be applied to this monitor's counters during the output routine.
    Bool_t m_invertFilter; //!< Set true to have the filter specify items *not* to be saved.
    Bool_t m_isCPUPrediction; //!< Flag implies prescales were applied to EB on the grid. Monitors should use after prescale bits rather than before
    ConfKey_t m_filterOnDecorationKey; //!< If the output is filtered by decoration, holds which decoration to use
    std::string m_filterOnDecorationValue; //!< If output is filtered by decoration, holds string must be an exact match

    CounterCollection_t   m_counterCollections; //!< Map of string referenced collections of counters.
    CounterMapSet_t       m_collectionsToProcess; //!< Pointers to the counter maps to be included in a given event.
    std::set<std::string> m_collectionsToProcessNames; //!< Names of the counter maps being processed
    std::map< std::string, LumiCollector* > m_collectionLumiCollector; //!< Map of string referenced collections to the lumi counter for that collection
    CounterMapType_t m_counterMapType; //!< Map of a CounterMap pointer (i.e. the business end of a CounterCollection) to it's CounterCollection type

    std::set< CounterBase* > m_countersInEvent; //!< Set of counters processed in an event. We need to run endEvent on these.

    Int_t m_currentCollectionLBNumber; //!< Remember LB number that m_collectionsToProcess is currently calculated for
    DBKey m_currentDBKey; //!< Remember database key that m_collectionsToProcess is currently calculated for

    std::set<DBKey>         m_perKeySummaries; //!< Set of keys we are at least part way through monitoring
    std::map<DBKey, UInt_t> m_perKeySummaryLBStart; //!< Map of keys we are monitoring to the initial (seen) LB, so we can stop after kNLbPerHLTConfig LB

    Timer m_timer; //<! Every monitor gets a timer to see how long it took


  }; //class MonitorBase

} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorBase_H
