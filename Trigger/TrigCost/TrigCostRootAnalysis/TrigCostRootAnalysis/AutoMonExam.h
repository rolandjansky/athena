// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Chris McNicol <christopher.john.mcnicol@cern.ch>
// -------------------------------------------------------------

#ifndef TrigCostRootAnalysis_AutoMonExam_H
#define TrigCostRootAnalysis_AutoMonExam_H

#include <map>
#include <string>
#include <vector>
#include "AutoMonTest.h"

namespace TrigCostRootAnalysis {
  class AutoMonControl;

  class AutoMonExam {
  public:
    AutoMonExam(const std::string& name, AutoMonControl* control, MonitorBase* monitor,
                std::vector<const AutoMonTest*> testList);
    AutoMonExam(const std::string& name, AutoMonControl* control, MonitorBase* monitor,
                std::vector<const AutoMonTest*> testList, const std::string& range);
    AutoMonExam(const std::string& name, AutoMonControl* control, MonitorBase* monitor,
                std::vector<std::string> exclude, std::vector<std::string> specify,
                std::vector< const AutoMonTest*> testList);
    AutoMonExam(const std::string& name, AutoMonControl* control, MonitorBase* monitor,
                std::vector<std::string> exclude, std::vector<std::string> specify,
                std::vector< const AutoMonTest*> testList, const std::string& range);
    void runExam();

    void addTest(const std::string& TestName);
    void addTest(AutoMonTest* test);
    void printCountersToExam() const;
    void setupSpecify();
    void setupExclude();
    void setupCounterList();
    void retrieveCounterCollection();
    const std::string& getExamName() const {return m_exam_name;}
    const AutoMonTest* getTest(const std::string& testName) const;
    void printTests();
    MonitorBase* getMonitor() const {return m_monitor;}
  private:
    const std::string m_exam_name;
    AutoMonControl* m_control;
    MonitorBase* m_monitor;
    std::vector<std::string> m_exclude_list;
    std::vector<std::string> m_specify_list;
    std::vector< const AutoMonTest* > m_test_list;
    const std::string m_range;

    CounterMap_t m_counters_to_exam;
    CounterMap_t* m_all_counters_in_monitor;

    std::vector<double> m_results;
  };
}// namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_AutoMonExam_H
