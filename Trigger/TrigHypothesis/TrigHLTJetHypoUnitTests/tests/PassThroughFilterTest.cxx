/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/PassThroughFilter.h"
#include "TrigHLTJetHypo/../src/ITrigJetHypoInfoCollector.h"


#include "../src/TLorentzVectorFactory.h"
#include "../src/TLorentzVectorAsIJet.h"
#include "../src/makeHypoJets.h"

#include <vector>
#include <memory>

TEST(PassThroughFilterTester, construction) { 
  // constructor throws id begin index > end index
  EXPECT_NO_THROW({PassThroughFilter();});
}

TEST(PassThroughFilterTester, zerojets) {
  auto rf = PassThroughFilter();
  HypoJetVector tv{};

  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);
  
  auto fj = rf.filter(tv, deb);
  EXPECT_EQ(fj.size(), 0u);
}

TEST(PassThroughFilterTester, twojets) {

  auto rf = PassThroughFilter();
  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);

  std::vector<double> eta{2., 1.};
  HypoJetVector tv = makeHypoJets(eta);

  auto fj = rf.filter(tv, deb);
  EXPECT_EQ(fj.size(), 2u);
  
  EXPECT_EQ(fj.front()->eta(), tv.front()->eta());
  EXPECT_EQ(fj.back()->eta(), tv.back()->eta());


}
