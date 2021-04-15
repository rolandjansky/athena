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
  
  auto pair = rf.filter(tv.begin(), tv.end(), deb);
  EXPECT_EQ((pair.second - pair.first), 0);

  EXPECT_EQ(pair.first, tv.begin());
  EXPECT_EQ(pair.second, tv.end());
}

TEST(PassThroughFilterTester, twojets) {

  auto rf = PassThroughFilter();
  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);

  std::vector<double> eta{2., 1.};
  HypoJetVector tv = makeHypoJets(eta);

  std::pair<HypoJetCIter, HypoJetCIter> iters =
    std::make_pair(tv.begin(), tv.end());
  
  iters = rf.filter(tv.begin(), tv.end(), deb);
  EXPECT_EQ((iters.second - iters.first), 2);

  const auto& fiter = iters.first;
  const auto& siter = iters.second;

  double f_eta = (*fiter)->eta();
  double s_eta = (*(siter-1))->eta();
  
  EXPECT_EQ( f_eta, (*(tv.begin()))->eta());
  EXPECT_EQ( s_eta, (*(tv.end()-1))->eta());

}
