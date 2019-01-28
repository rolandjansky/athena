// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Chris McNicol <christopher.john.mcnicol@cern.ch>
// -------------------------------------------------------------

#ifndef TrigCostRootAnalysis_AutoMonTest_H
#define TrigCostRootAnalysis_AutoMonTest_H

#include <map>
#include <string>
#include <vector>
#include <limits>


#include "TrigCostRootAnalysis/Utility.h"


namespace TrigCostRootAnalysis {
  struct Thresholds {
    Float_t Alert_threshold = static_cast<Float_t >(std::numeric_limits<float>::lowest());
    Float_t Warning_threshold = static_cast<Float_t >(std::numeric_limits<float>::lowest());
    Float_t Advisory_threshold = static_cast<Float_t >(std::numeric_limits<float>::lowest());
  };

  enum AlertStatus {
    ALERT = 1,
    WARNING = 2,
    ADVISORY = 3,
    NOMINAL = 4,
  };

  enum TestType {
    HIGH,
    LOW
  };


  const std::string getResultString(const AlertStatus& alert);

  class AutoMonResult; //Forward declaration

  class AutoMonTest {
  public:
    AutoMonTest(const std::string& TestName, const ConfKey_t& Varname, const VariableOption_t& VarOpt,
                bool doFractional, AutoMonResult* result);

    void runTest(const CounterMap_t& counter_map) const;
    void setThresholds(const Float_t& alert);
    void setThresholds(const Float_t& alert, const Float_t& warn);
    void setThresholds(const Float_t& alert, const Float_t& warn, const Float_t& advise);
    void applyThresholds(const CounterBase* counter, Float_t total_result) const;
    void printThresholds(std::ostream& outStream) const;
    Float_t sumCounters(const CounterMap_t& counter_map) const;
    const std::string& getTestName() const {return m_test_name;}
    Float_t getThreshold(const AlertStatus& alert) const;
    const TestType& getTestType() const {return m_test_type;}
    const ConfKey_t& getVarName() const {return m_Varname;}
    const VariableOption_t& getVarOption() const {return m_variable_option;}
  private:
    ConfKey_t m_Varname;
    VariableOption_t m_variable_option;
    bool m_doFractional;
    const std::string m_test_name;
    AutoMonResult* m_result;
    TestType m_test_type;
    Thresholds m_thresholds;
  };
}//namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_AutoMonTest_H
