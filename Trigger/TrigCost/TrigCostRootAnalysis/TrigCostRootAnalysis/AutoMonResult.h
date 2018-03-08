// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Chris McNicol <christopher.john.mcnicol@cern.ch>
// -------------------------------------------------------------

#ifndef TrigCostRootAnalysis_AutoMonResult_H
#define TrigCostRootAnalysis_AutoMonResult_H

#include "AutoMonTest.h"
#include "TrigCostRootAnalysis/Config.h"
#include <iostream>

namespace TrigCostRootAnalysis {
  class AutoMonResult {
  public:
    AutoMonResult();
    AutoMonResult(const AlertStatus& interest_level);

    void saveOutput() const;
    void saveList(JsonExport json, std::map < const CounterBase*, const AutoMonTest* > list, std::ofstream& fout,
                  const AlertStatus& alert) const;
    void printList(std::map < const CounterBase*, const AutoMonTest* > list, const AlertStatus& alert) const;
    void setInterestLevel(const AlertStatus& level) {m_interest_level = level;}
    const AlertStatus& getInterestLevel() const {return m_interest_level;}
    void addToList(const CounterBase* counter, const AutoMonTest* test, const AlertStatus& level);
    void printResults() const;
  private:
    AlertStatus m_interest_level;
    std::map< const CounterBase*, const AutoMonTest* > m_alert_list;
    std::map< const CounterBase*, const AutoMonTest* > m_warning_list;
    std::map< const CounterBase*, const AutoMonTest* > m_advisory_list;
  };
} //namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_AutoMonResult_H
