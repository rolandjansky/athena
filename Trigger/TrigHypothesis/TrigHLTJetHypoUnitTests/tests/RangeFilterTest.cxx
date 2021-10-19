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

TEST(RangeFilterTester, insufficient0) {
  auto rf = RangeFilter(0, 3);
  HypoJetVector tv{};

  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);
  
  auto fj = rf.filter(tv, deb);
  EXPECT_EQ(fj.size(), 0u);

  tv.push_back(make_jet(0, 2.));
	    
  fj = rf.filter(tv, deb);
  EXPECT_EQ(fj.size(), 0u);

  tv.push_back(make_jet(0, 1.));

  fj = rf.filter(tv, deb);
  EXPECT_EQ(fj.size(), 0u);

  tv.push_back(make_jet(0, 3.));

  fj = rf.filter(tv, deb);
  EXPECT_EQ(fj.size(), 3u);

  EXPECT_GT(fj[0]->pt(), fj[1]->pt());
  EXPECT_GT(fj[1]->pt(), fj[2]->pt());
}

TEST(RangeFilterTester, insufficient1) {
  auto rf = RangeFilter(1, 3);
  HypoJetVector tv{};
  tv.push_back(make_jet(0, 2.));
  tv.push_back(make_jet(0, 1.));

  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);
  
  auto fj = rf.filter(tv, deb);
  EXPECT_EQ(fj.size(), 0u);

}

TEST(RangeFilterTester, run2TLA) {
  auto rf = RangeFilter(1, 3);
  HypoJetVector tv{};
  tv.push_back(make_jet(0, 2.));
  tv.push_back(make_jet(0, 1.));
  tv.push_back(make_jet(0, 0.5));

  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);
  
  auto fj = rf.filter(tv, deb);
  EXPECT_EQ(fj.size(), 2u);

}
