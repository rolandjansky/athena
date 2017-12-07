/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigSteering_Lvl1ConsistencyChecker_h
#define TrigSteering_Lvl1ConsistencyChecker_h

#include "AthenaBaseComps/AthAlgTool.h"

class TH1I;

namespace TrigConf {
  class ILVL1ConfigSvc;
  class TriggerItemNode;
}

namespace HLT {
  class Navigation;
  class ErrorCode;
}

namespace LVL1CTP {
  class Lvl1Item;  
}

static const InterfaceID IID_Lvl1ConsistencyChecker("Lvl1ConsistencyChecker", 1, 0);

/**
 * @brief tool to check if L1 decision is consistent
 * It checks following: if l1 item is fired it checks that there is sufficient RoIs for this to be the case.
 * 
 * @author Brian Petersen
 * @author Tomasz Bold
 **/

class Lvl1ConsistencyChecker : public AthAlgTool {
public:


  Lvl1ConsistencyChecker(const std::string& name, const std::string& type,
                         const IInterface* parent);
  static const InterfaceID& interfaceID() { return IID_Lvl1ConsistencyChecker; } 
  
  StatusCode start(); 

  /**
   * Initialize with L1 configuration. Needs to be called before start().
   */
  StatusCode updateConfig(const TrigConf::ILVL1ConfigSvc* config);

  /**
   * Check if all L1 items have a corresponding threshold/RoI
   *
   * The tool will create two histograms. One with all missing thresholds and
   * one by ignoring the listed threshold types. This is useful to ignore known
   * mismatches for events with e.g. overflows.
   *
   * @param items            Trigger items
   * @param nav              Pointer to navigation
   * @param ignoreThresholds thresholds to ignore (e.g. 'EM','TAU')
   */
  HLT::ErrorCode check(const std::vector<const LVL1CTP::Lvl1Item*>& items,
                       const HLT::Navigation* nav, 
                       const std::vector<std::string>& ignoreThresholds);

private:
  bool m_printWarnings;
  bool m_returnFailure;
  std::vector<std::string> m_thresholdsToCheck;

  TH1I* m_hist{0};
  TH1I* m_histAll{0};

  // aux data structures
  struct Threshold {
    std::string name;
    std::string type;     // L1DataDef::TriggerType as string
    uint8_t multiplicity;
  };
  typedef uint32_t ThresholdId;

  struct Item {
    std::map<ThresholdId, Threshold> thresholds;
    std::string name;
    void addThreshold(const Threshold& t);
  };
  typedef uint32_t ItemId;
  std::map<ItemId, Item> m_configuredItems; 
  
  void makeItemRoIMap(const TrigConf::TriggerItemNode *node, Item& item);
};


#endif // TrigSteering_Lvl1ConsistencyChecker_h
