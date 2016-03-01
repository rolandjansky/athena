/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_MOCKJET_H
#define TRIGHLTJETHYPOUNITTESTS_MOCKJET_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "gmock/gmock.h"
#include <TLorentzVector.h>
#include <string>

class MockJet: public HypoJet::IJet {
public:
  MOCK_CONST_METHOD0(pt, double());
  MOCK_CONST_METHOD0(eta, double());
  MOCK_CONST_METHOD0(phi, double());
  MOCK_CONST_METHOD0(m, double ());
  MOCK_CONST_METHOD0(e, double());
  MOCK_CONST_METHOD0(et, double());
  MOCK_CONST_METHOD0(rapidity, double());
  MOCK_CONST_METHOD0(p4, const TLorentzVector&());
  MOCK_CONST_METHOD0(position, unsigned int());
  MOCK_CONST_METHOD2(getAttribute, bool (const std::string&, float&));
  MOCK_CONST_METHOD0(toString, std::string());
};
#endif
