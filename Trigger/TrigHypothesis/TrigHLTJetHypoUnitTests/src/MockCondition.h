/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_MOCKCONDITION_H
#define TRIGHLTJETHYPOUNITTESTS_MOCKCONDITION_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"
#include "gmock/gmock.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

class MockCondition: public ICondition {
public:
  MOCK_CONST_METHOD1(isSatisfied, bool(const HypoJetVector&));

  //supply kludges needed due to no google mock support for noexcept
  // std::string toString() const noexcept {return toString_kludge();}
  std::string toString() const noexcept {return "MockCondition";}
  // MOCK_CONST_METHOD0(toString_kludge, std::string());

  double orderingParameter() const noexcept {return -999;}
  // double orderingParameter() const noexcept {return orderingParameter_kludge();}
  // MOCK_CONST_METHOD0(orderingParameter_kludge, double());
};
#endif
