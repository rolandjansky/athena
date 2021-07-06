/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/../src/JetGroupRegister.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include "../src/TLorentzVectorFactory.h"
#include "../src/TLorentzVectorAsIJet.h"

#include <vector>
#include <map>

using res = std::pair<std::vector<unsigned int>, bool>;
using vec = std::vector<unsigned int>;

TEST(JetGroupRegisterTester, empty) {

  double eta = 2.1;
  double et = 101.;

  TLorentzVector tlv = TLorentzVectorFactory().make(eta, et);
  pHypoJet hjet(nullptr);
  hjet.reset(new TLorentzVectorAsIJet(tlv));
  HypoJetVector jets {hjet};
    
  JetGroupRegister jg_register;

  std::size_t idx = jg_register.record(jets);

  EXPECT_EQ(idx, 0u);

  idx = jg_register.record(jets);

  EXPECT_EQ(idx, 0u);

  HypoJetVector jets2 = jets;

  jets2.push_back(hjet);

  idx = jg_register.record(jets2);

  EXPECT_EQ(idx, 1u);

  idx = jg_register.record(jets);
  EXPECT_EQ(idx, 0u);
}



