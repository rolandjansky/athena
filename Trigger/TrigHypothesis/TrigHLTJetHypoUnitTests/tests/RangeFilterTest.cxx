/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/RangeFilter.h"
#include "TrigHLTJetHypo/../src/ITrigJetHypoInfoCollector.h"


#include "../src/TLorentzVectorFactory.h"
#include "../src/TLorentzVectorAsIJet.h"

#include <vector>
#include <memory>

HypoJetVector::value_type make_jet(double eta, double pt){
  // TLorentzVectorFactory makes massless vecotrs, so e = p
  TLorentzVector tlv = TLorentzVectorFactory().make(eta, pt);
  pHypoJet hjet(nullptr);
  hjet.reset(new TLorentzVectorAsIJet(tlv));
  return hjet;
}   

TEST(RangeFilterTester, construction) { 
  // constructor throws id begin index > end index
  EXPECT_NO_THROW({RangeFilter(0,0);});
  EXPECT_NO_THROW({RangeFilter(1,1);});
  EXPECT_THROW({RangeFilter(1,0);}, std::out_of_range);
}

TEST(CombinationsGenTester, insufficient) {
  auto rf = RangeFilter(0, 3);
  HypoJetVector tv{};

  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);
  
  auto pair = rf.filter(tv.begin(), tv.end(), deb);
  EXPECT_EQ((pair.second - pair.first), 0);

  tv.push_back(make_jet(0, 2.));

  pair = rf.filter(tv.begin(), tv.end(), deb);
  EXPECT_EQ((pair.second - pair.first), 0);

  tv.push_back(make_jet(0, 1.));

  pair = rf.filter(tv.begin(), tv.end(), deb);
  EXPECT_EQ((pair.second - pair.first), 0);

  tv.push_back(make_jet(0, 3.));

  pair = rf.filter(tv.begin(), tv.end(), deb);
  EXPECT_EQ((pair.second - pair.first), 3);

  const auto& fiter = pair.first;
  const auto& siter = pair.second;
  
  EXPECT_GT( (*fiter)->pt(), (*(fiter+1))->pt());
  EXPECT_GT((*(fiter+1))->pt(), (*(fiter+2))->pt());
  EXPECT_EQ((fiter+3), siter); 
}
