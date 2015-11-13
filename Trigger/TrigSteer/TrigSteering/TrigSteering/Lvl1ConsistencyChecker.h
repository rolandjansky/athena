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
  StatusCode initialize(); 
  StatusCode updateConfig(const TrigConf::ILVL1ConfigSvc* config);
  HLT::ErrorCode check(const std::vector<const LVL1CTP::Lvl1Item*>& items,
		       const HLT::Navigation* nav);

private:
  bool m_printErrorMessages;
  bool m_returnFailure;
  TH1I* m_histogram;

  // aux data structures
  struct Threshold {
    std::string name;
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


  typedef std::map<ThresholdId, Threshold>::const_iterator thresholds_iterator;
  typedef std::map<ItemId, Item>::const_iterator items_iterator;
  
  void makeItemRoIMap(const TrigConf::TriggerItemNode *node, Item& item);
  bool thresholdToCheck(const std::string& );
};


#endif // TrigSteering_Lvl1ConsistencyChecker_h
