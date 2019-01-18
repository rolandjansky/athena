// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Chris McNicol <christopher.john.mcnicol@cern.ch>
// -------------------------------------------------------------



    #include "TrigCostRootAnalysis/AutoMonTest.h"
    #include "TrigCostRootAnalysis/CounterBase.h"
    #include "TrigCostRootAnalysis/MonitorBase.h"
    #include "TrigCostRootAnalysis/AutoMonResult.h"


namespace TrigCostRootAnalysis {
  AutoMonTest::AutoMonTest(const std::string& TestName, const ConfKey_t& Varname, const VariableOption_t& VarOpt,
                           bool doFractional, AutoMonResult* result) :
    m_Varname(Varname),
    m_variable_option(VarOpt),
    m_doFractional(doFractional),
    m_test_name(TestName),
    m_result(result) {
    //Currently the tests are set to this type by default.
    // TODO allow user to configure this.
    m_test_type = TestType::HIGH;
  }

  /**
   * Set the thresholds to be used.
   * @param alert The ALERT threshold.
   * @param warn The WARNING threshold.
   * @param advise The ADVISORY threshold.
   */

  void AutoMonTest::setThresholds(const Float_t& alert) {
    m_thresholds.Alert_threshold = alert;
  }

  void AutoMonTest::setThresholds(const Float_t& alert, const Float_t& warn) {
    m_thresholds.Alert_threshold = alert;
    m_thresholds.Warning_threshold = warn;
  }

  void AutoMonTest::setThresholds(const Float_t& alert, const Float_t& warn, const Float_t& advise) {
    m_thresholds.Alert_threshold = alert;
    m_thresholds.Warning_threshold = warn;
    m_thresholds.Advisory_threshold = advise;
  }

  /**
   * Check to see if the counter value passes one of the thresholds
   * @param counter Pointer to the counter to be tested.
   */

  void AutoMonTest::applyThresholds(const CounterBase* counter, const Float_t total_result) const {
    Float_t counterValue;

    if (counter->getValueExists(m_Varname, m_variable_option)) {
      counterValue = counter->getValue(m_Varname, m_variable_option);
    } else {
      std::string errorMSG = "Counter " + counter->getName() +
                             " does not have a variable with requested ConfKey or VariableOption";
      Error("AutoMonTest::applyThresholds", "%s", errorMSG.c_str());
      return;
    }

    if (m_doFractional) {
      counterValue = counterValue / total_result;
    }

    bool testBelowThreshold = (m_test_type == TestType::LOW);

    // ^ is Logical XOR, when testing when Below threshold we want to reverse comparison.

    if (m_result->getInterestLevel() >= AlertStatus::ALERT &&
        m_thresholds.Alert_threshold != std::numeric_limits<float>::lowest()) {
      if ((testBelowThreshold) ^ (counterValue > m_thresholds.Alert_threshold)) {
        m_result->addToList(counter, this, AlertStatus::ALERT);
        return;
      }
    }

    if (m_result->getInterestLevel() >= AlertStatus::WARNING &&
        m_thresholds.Warning_threshold != std::numeric_limits<float>::lowest()) {
      if ((testBelowThreshold) ^ (counterValue > m_thresholds.Warning_threshold)) {
        m_result->addToList(counter, this, AlertStatus::WARNING);
        return;
      }
    }

    if (m_result->getInterestLevel() >= AlertStatus::ADVISORY &&
        m_thresholds.Advisory_threshold != std::numeric_limits<float>::lowest()) {
      if ((testBelowThreshold) ^ (counterValue > m_thresholds.Advisory_threshold)) {
        m_result->addToList(counter, this, AlertStatus::ADVISORY);
        return;
      }
    }

    return;
  }

  /**
   * Run Test over the counter map.
   * @param counter_map CounterMap the test will run over.
   */

  void AutoMonTest::runTest(const CounterMap_t& counter_map) const {
    Float_t result;

    if (m_doFractional) {
      result = sumCounters(counter_map);
    } else {
      result = static_cast<Float_t>(1.0);
    }
    const Float_t total_result = result;

    for (auto counter_iter = counter_map.begin(); counter_iter != counter_map.end(); ++counter_iter) {
      applyThresholds(counter_iter->second, total_result);
    }
  }

  /**
   * Sum the counters.
   * @param counter_map CounterMap the test will run over.
   */

  Float_t AutoMonTest::sumCounters(const CounterMap_t& counter_map) const {
    Float_t toReturn = 0.0;

    for (auto counter_iter = counter_map.begin(); counter_iter != counter_map.end(); ++counter_iter) {
      toReturn += (counter_iter->second)->getValue(m_Varname, m_variable_option);
    }//end for loop over countermap
    return toReturn;
  }//end sumCounters

  /**
   * Get the threshold for a particular level.
   * @param alert The alert level requested.
   * @returns The threshold .
   */

  Float_t AutoMonTest::getThreshold(const AlertStatus& alert) const {
    switch (alert) {
    case ALERT:
      return m_thresholds.Alert_threshold;

    case WARNING:
      return m_thresholds.Warning_threshold;

    case ADVISORY:
      return m_thresholds.Advisory_threshold;

    default:
      return std::numeric_limits<float>::lowest();
    } // end switch
  } // end getThreshold

  /**
   * Print the thresholds.
   * @param outStream The ouput stream.
   */

  void AutoMonTest::printThresholds(std::ostream& outStream) const {
    outStream << "Alert Threshold : " << m_thresholds.Alert_threshold;
    outStream << " Warning Threshold : " << m_thresholds.Warning_threshold;
    outStream << " Advisory Threshold : " << m_thresholds.Advisory_threshold << '\n';
    outStream << " Do Fractional ? " << std::boolalpha << m_doFractional << '\n';
  } // end printThresholds

  /**
   * Print the thresholds.
   * @param alert The alert level requested.
   * @returns A string that corresponds to the alert level.
   */

  const std::string getResultString(const AlertStatus& alert) {
    switch (alert) {
    case ALERT:
      return "ALERT";

    case WARNING:
      return "WARNING";

    case ADVISORY:
      return "ADVISORY";

    case NOMINAL:
      return "NOMINAL";

    default:
      return "UNKNOWN";
    }//end switch
  }//end getResultString
} //namespace TrigCostRootAnalysis
