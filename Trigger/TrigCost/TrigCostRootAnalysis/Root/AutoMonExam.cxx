// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Chris McNicol <christopher.john.mcnicol@cern.ch>
// -------------------------------------------------------------


#include "TrigCostRootAnalysis/AutoMonControl.h"
#include "TrigCostRootAnalysis/AutoMonExam.h"
#include "TrigCostRootAnalysis/MonitorBase.h"
#include "TrigCostRootAnalysis/CounterBase.h"
#include <TError.h>


namespace TrigCostRootAnalysis {
  AutoMonExam::AutoMonExam(const std::string& name, AutoMonControl* control, MonitorBase* monitor,
                           std::vector<std::string> exclude, std::vector<std::string> specify,
                           std::vector< const AutoMonTest*> testList) :
    m_exam_name(name),
    m_control(control),
    m_monitor(monitor),
    m_exclude_list(exclude),
    m_specify_list(specify),
    m_test_list(testList),
    m_range("All") {
    retrieveCounterCollection();
    setupCounterList();
  }

  AutoMonExam::AutoMonExam(const std::string& name, AutoMonControl* control, MonitorBase* monitor,
                           std::vector< const AutoMonTest*> testList) :
    m_exam_name(name),
    m_control(control),
    m_monitor(monitor),
    m_test_list(testList),
    m_range("All") {
    retrieveCounterCollection();
    setupCounterList();
  }

  AutoMonExam::AutoMonExam(const std::string& name, AutoMonControl* control, MonitorBase* monitor,
                           std::vector<std::string> exclude, std::vector<std::string> specify,
                           std::vector< const AutoMonTest*> testList, const std::string& range) :
    m_exam_name(name),
    m_control(control),
    m_monitor(monitor),
    m_exclude_list(exclude),
    m_specify_list(specify),
    m_test_list(testList),
    m_range(range) {
    retrieveCounterCollection();
    setupCounterList();
  }

  AutoMonExam::AutoMonExam(const std::string& name, AutoMonControl* control, MonitorBase* monitor,
                           std::vector<const AutoMonTest*> testList, const std::string& range) :
    m_exam_name(name),
    m_control(control),
    m_monitor(monitor),
    m_test_list(testList),
    m_range(range) {
    retrieveCounterCollection();
    setupCounterList();
  }

  void AutoMonExam::retrieveCounterCollection() {
    // This is slightly superfluous as we could just getCounterCollection("All") but this allows some flexibility if the
    // Config is changed
    // This is default and it is expected most exams will use the default range
    if (m_range == "All" || m_range == "all") {
      m_all_counters_in_monitor = m_monitor->getCounterCollection(Config::config().getStr(kAllString), kDoAllSummary);
    } else if (m_monitor->doesCounterCollectionExist(m_range)) {
      m_all_counters_in_monitor =
        m_monitor->getCounterCollection(m_range,
                                        kDoAllSummary /*Not used, only used when creating new CounterCollection*/);
    } else {
      const std::string rangeError = "Unable to find/use range : " + m_range;
      const std::string infoString = "Exam " + m_exam_name + " will run over zero counters!";
      Error("AutoMonExam::retrieveCounterCollection", "%s", rangeError.c_str());
      Info("AutoMonExam::retrieveCounterCollection", "%s", infoString.c_str());
    }

    for (auto iter = m_all_counters_in_monitor->begin(); iter != m_all_counters_in_monitor->end(); ++iter) {
      const std::string counterString = "Counter in monitor : " + (iter->second)->getName();
      Info("AutoMonExam::retrieveCounterCollection", "%s", counterString.c_str());
    }
    printTests();
  } // end retrieveCounterCollection

  /**
   * Add test to exam after finding in AutoMonControl test list
   * @param TestName Name of test to be found in AutoMonControl
   */

  void AutoMonExam::addTest(const std::string& TestName) {
    const AutoMonTest* test = m_control->getTest(TestName);

    if (test) {
      m_test_list.push_back(test);
    }
  } //end addTest

  void AutoMonExam::addTest(AutoMonTest* test) {
    m_test_list.push_back(test);
  } //end addTest

  /**
   * Print name and information of tests in this exam
   */

  void AutoMonExam::printTests() {
    const std::string header = "Printing tests found in exam : " + m_exam_name + '\n';

    Info("AutoMonExam::printTests", "%s", header.c_str());
    for (auto test_iter = m_test_list.begin(); test_iter != m_test_list.end(); ++test_iter) {
      const std::string testString = "Test : " + (*test_iter)->getTestName() + '\n';
      Info("AutoMonExam::printTests", "%s", testString.c_str());
      (*test_iter)->printThresholds(std::cout);
    }//end for loop over test list
  } //end printTests

  /**
   * Add test to exam after finding in AutoMonControl test list
   * @param TestName Name of test to be found in AutoMonControl
   */

  const AutoMonTest* AutoMonExam::getTest(const std::string& testName) const {
    const AutoMonTest* result = nullptr;

    for (auto test_iter = m_test_list.begin(); test_iter != m_test_list.end(); ++test_iter) {
      std::cout << (*test_iter)->getTestName() << '\n';
      if ((*test_iter)->getTestName() == testName) {
        result = (*test_iter);
        break;
      }
    }
    return result;
  }

  /**
   * Keep only counters that the user has specified
   */

  void AutoMonExam::setupSpecify() {
    if (m_specify_list.size() != 0) {
      m_counters_to_exam.clear();
      for (auto iter = m_specify_list.begin(); iter != m_specify_list.end(); ++iter) {
        auto find_counter_iter = m_all_counters_in_monitor->find(*iter);

        if (find_counter_iter != m_all_counters_in_monitor->end()) {
          m_counters_to_exam[ (*iter) ] = find_counter_iter->second;
        } else {
          std::string errorMSG = "AutoMonExam >> Counter with name: " + (*iter) +
                                 " specified but not found in Counter Map";
          Error("AutoMonExam::setupSpecify", "%s", errorMSG.c_str());
        }
      }//end forloop over m_specify_list
    } //end if no counters specified
  }//end setupSpecify()

  /**
   * Exclude counters that the user has specified
   */

  void AutoMonExam::setupExclude() {
    for (auto iter = m_exclude_list.begin(); iter != m_exclude_list.end(); ++iter) {
      auto search_iter = m_counters_to_exam.find(*iter);

      if (search_iter != m_counters_to_exam.end()) {
        m_counters_to_exam.erase(search_iter);
      } else {
        const std::string errorMSG = "AutoMonExam >> Counter with name " + (*iter) +
                                     "requested to be excluded - not found in Counter Map";
        Error("AutoMonExam::setupExclude", "%s", errorMSG.c_str());
      }
    } //end forloop over m_exclude_list
  }//end setupExclude()

  /**
   * Copy all counters in the monitor then exclude or specify counters based on user request.
   */

  void AutoMonExam::setupCounterList() {
    m_counters_to_exam = *(m_all_counters_in_monitor);

    if (m_exclude_list.size() != 0) {
      setupExclude();
    }
    if (m_specify_list.size() != 0) {
      setupSpecify();
    }
  } //end setupCounterList

  /**
   * Print all counters that are to be examined
   */

  void AutoMonExam::printCountersToExam() const {
    const std::string header = "Counters to Examine are :";

    Info("AutoMonExam::printCountersToExam", "%s", header.c_str());
    int i = 1;
    for (auto counter_iter = m_counters_to_exam.begin(); counter_iter != m_counters_to_exam.end(); ++counter_iter) {
      const std::string counterInfo = "Counter " + std::to_string(i) + " to examine: " +
                                      (counter_iter->second)->getName();
      Info("AutoMonExam::printCountersToExam", "%s", counterInfo.c_str());
      ++i;
    }
    const std::string footer = "---------------Finished List ";
    Info("AutoMonExam::printCountersToExam", "%s", footer.c_str());
  } //end printCountersToExam

  /**
   * Run all of the tests in the exam
   */

  void AutoMonExam::runExam() {
    const std::string header = "Running exam : " + m_exam_name;

    Info("AutoMonExam::runExam", "%s", header.c_str());

    for (auto test_iter = m_test_list.begin(); test_iter != m_test_list.end(); ++test_iter) {
      const std::string testInfo = "Running Test : " + (*test_iter)->getTestName();
      Info("AutoMonExam::runExam", "%s", testInfo.c_str());
      (*test_iter)->runTest(m_counters_to_exam);
    } //end for loop over test list
  } //end runExam
}//namespace TrigCostRootAnalysis
