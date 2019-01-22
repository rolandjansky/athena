// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Chris McNicol <christopher.john.mcnicol@cern.ch>
// -------------------------------------------------------------

#include "TrigCostRootAnalysis/AutoMonControl.h"
#include "TrigCostRootAnalysis/AutoMonResult.h"
#include "TrigCostRootAnalysis/MonitorBase.h"
#include "TrigCostRootAnalysis/CounterBase.h"

namespace TrigCostRootAnalysis {
  AutoMonResult::AutoMonResult() {
    m_interest_level = AlertStatus::ALERT;
  }

  AutoMonResult::AutoMonResult(const AlertStatus& interest_level) :
    m_interest_level(interest_level) {
  }

  /*
   * Print all lists above the interest level
   */

  void AutoMonResult::printResults() const {
    printList(m_alert_list, AlertStatus::ALERT);
    if (m_interest_level == AlertStatus::ALERT) {
      return;
    }

    printList(m_warning_list, AlertStatus::WARNING);
    if (m_interest_level == AlertStatus::WARNING) {
      return;
    }

    printList(m_advisory_list, AlertStatus::ADVISORY);
    return;
  } //end printResults

  /*
   * Save output of all relevant lists to JSON file.
   */

  void AutoMonResult::saveOutput() const {
    const std::string output = Config::config().getStr(kOutputDirectory) + "/AutoMonResult.json";

    Info("AutoMonResult::saveOutput", "Outputting AutoMonResult to %s", output.c_str());
    std::ofstream fout(output.c_str());

    JsonExport json;
    json.addNode(fout, "AutoMonResults");

    saveList(json, m_alert_list, fout, AlertStatus::ALERT);

    if (m_interest_level >= AlertStatus::WARNING) {
      saveList(json, m_warning_list, fout, AlertStatus::WARNING);
    }
    if (m_interest_level >= AlertStatus::ADVISORY) {
      saveList(json, m_advisory_list, fout, AlertStatus::ADVISORY);
    }

    json.endNode(fout);
    fout.close();
  } //end saveOutput

  /*
   * Save output of a particular list to the JSON file.
   */

  void AutoMonResult::saveList(JsonExport json, std::map < const CounterBase*, const AutoMonTest* > list,
                               std::ofstream& fout, const AlertStatus& alert) const {
    for (auto list_iter = list.begin(); list_iter != list.end(); ++list_iter) {
      const std::string alertLevel = getResultString(alert);
      const std::string testName = (list_iter->second)->getTestName();
      const std::string counterName = (list_iter->first)->getName();
      const std::string MonitorName = (list_iter->first)->getParent()->getName();
      const std::string range = (list_iter->first)->getRange(); //"UNKNOWN";
      const std::string AboveOrBelow = ((list_iter->second)->getTestType() == TestType::HIGH) ? " > " : " < ";
      const std::string threshold = std::to_string((list_iter->second)->getThreshold(alert));
      const std::string value =
        std::to_string((list_iter->first)->getValue((list_iter->second)->getVarName(),
                                                    (list_iter->second)->getVarOption()));

      json.addNode(fout, counterName);
      json.addLeafCustom(fout, "alertLevel", alertLevel);
      json.addLeafCustom(fout, "testName", testName);
      json.addLeafCustom(fout, "counterName", counterName);
      json.addLeafCustom(fout, "MonitorName", MonitorName);
      json.addLeafCustom(fout, "range", range);
      json.addLeafCustom(fout, "AboveOrBelow", AboveOrBelow);
      json.addLeafCustom(fout, "threshold", threshold);
      json.addLeafCustom(fout, "value", value);
      json.endNode(fout);
    }// end of loop over list
  } // end saveList

  /**
   * Print List using Info .
   * @param list The list to be printed.
   * @param alert The alert level the list corresponds to.
   */

  void AutoMonResult::printList(std::map < const CounterBase*, const AutoMonTest* > list,
                                const AlertStatus& alert) const {
    for (auto list_iter = list.begin(); list_iter != list.end(); ++list_iter) {
      const std::string alertLevel = getResultString(alert);
      const std::string testName = (list_iter->second)->getTestName();
      const std::string counterName = (list_iter->first)->getName();
      const std::string MonitorName = (list_iter->first)->getParent()->getName();
      const std::string range = (list_iter->first)->getRange(); //"UNKNOWN";
      const std::string upOrDown = ((list_iter->second)->getTestType() == TestType::HIGH) ? " > " : " < ";
      const std::string threshold = std::to_string((list_iter->second)->getThreshold(alert));
      const std::string value =
        std::to_string((list_iter->first)->getValue((list_iter->second)->getVarName(),
                                                    (list_iter->second)->getVarOption()));
      const std::string outputString = alertLevel + " " + testName + " " + counterName + " " + MonitorName + " " +
                                       range + " " + " " + value + " " + upOrDown + " " + threshold;
      Info("AutoMonResult::printList", "%s", outputString.c_str());
    }// end of loop over list
  } // end printList

  /**
   * Add a counter that fails a test to the map of counters to tests.
   * @param counter Pointer to counter that fails test.
   * @param test Pointer to test.
   * @param level Alert Level that counter has failed test at.
   */

  void AutoMonResult::addToList(const CounterBase* counter, const AutoMonTest* test, const AlertStatus& level) {
    if (level == AlertStatus::ALERT) {
      m_alert_list[counter] = test;
      return;
    }
    if (level == AlertStatus::WARNING) {
      m_warning_list[counter] = test;
      return;
    }
    if (level == AlertStatus::ADVISORY) {
      m_advisory_list[counter] = test;
      return;
    }
  } //end addToList
}// namespace TrigCostRootAnalysis
