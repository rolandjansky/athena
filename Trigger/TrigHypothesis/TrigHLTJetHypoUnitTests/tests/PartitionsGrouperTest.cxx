/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/PartitionsGrouper.h"

#include "../src/TLorentzVectorFactory.h"
#include "../src/TLorentzVectorAsIJet.h"

#include <TLorentzVector.h>

#include "gtest/gtest.h"


// using ::testing::Return;
// using ::testing::_;
// using ::testing::SetArgReferee;


TEST(PartitionsGrouperTest, zeroJets){
  HypoJetVector jets;
  std::vector<std::size_t> mults{2, 3};
  PartitionsGrouper pg(mults);
  auto jb = jets.begin();
  auto je = jets.end();
  auto groupVectors = pg.group(jb, je);
  EXPECT_TRUE(groupVectors.size() == 0);
}


TEST(PartitionsGrouperTest, test0){
  /* basic PartitionsGrouper testing 
   */

  auto factory = TLorentzVectorFactory();
  
  HypoJetVector jets;

  constexpr double eta{0.5}; 
  for(int i = 1; i < 6; ++i){
    auto tlv = factory.make(eta, i);
    auto tl_j = std::make_shared<TLorentzVectorAsIJet>(tlv);
    jets.push_back(tl_j);
  }

  EXPECT_TRUE(jets.size() == 5);  // started at 1, not 0...
  std::vector<std::size_t> mults{2, 3};
  PartitionsGrouper pg(mults);
  auto jb = jets.begin();
  auto je = jets.end();
  auto groupVectors = pg.group(jb, je);

  // need to place 5 jets in 2 conditions
  // 5.4.3.2/(2!3!) ways ot to this (j1j2 and j2j1 in c1 is counted once).
  EXPECT_TRUE(groupVectors.size() == 10);
  for(const auto& groupVector: groupVectors){
    EXPECT_TRUE(groupVector.size()== 2); // two conditions objects
    EXPECT_TRUE(groupVector[0].size()== 2); // two jets first group
    EXPECT_TRUE(groupVector[1].size()== 3); // three jets second group
  }
}

TEST(PartitionsGrouperTest, test1){
  /* basic PartitionsGrouper testing 
   */

  auto factory = TLorentzVectorFactory();
  
  HypoJetVector jets;

  constexpr double eta{0.5}; 
  for(int i = 1; i < 10; ++i){
    auto tlv = factory.make(eta, i);
    auto tl_j = std::make_shared<TLorentzVectorAsIJet>(tlv);
    jets.push_back(tl_j);
  }

  EXPECT_TRUE(jets.size() == 9);  // started at 1, not 0...
  std::vector<std::size_t> mults{2, 3, 1};
  PartitionsGrouper pg(mults);
  auto jb = jets.begin();
  auto je = jets.end();
  auto groupVectors = pg.group(jb, je);

  // need to place 9 jets in 3 conditions requiring 6 jets
  // 9.8.7.6.5.4/(2!3!1!) ways ot to this (j1j2 and j2j1 in c1 is counted once).
  EXPECT_TRUE(groupVectors.size() == 5040);
  for(const auto& groupVector: groupVectors){
    EXPECT_TRUE(groupVector.size()== 3); // two conditions objects
    EXPECT_TRUE(groupVector[0].size()== 2); // two jets first group
    EXPECT_TRUE(groupVector[1].size()== 3); // three jets second group
    EXPECT_TRUE(groupVector[2].size()== 1); // one jet third group
  }
}


TEST(PartitionsGrouperTest, test2){
  /* too few jets 
   */

  auto factory = TLorentzVectorFactory();
  
  HypoJetVector jets;

  constexpr double eta{0.5}; 
  for(int i = 1; i < 5; ++i){
    auto tlv = factory.make(eta, i);
    auto tl_j = std::make_shared<TLorentzVectorAsIJet>(tlv);
    jets.push_back(tl_j);
  }

  EXPECT_TRUE(jets.size() == 4);  // started at 1, not 0...
  std::vector<std::size_t> mults{2, 3};
  PartitionsGrouper pg(mults);
  auto jb = jets.begin();
  auto je = jets.end();
  auto groupVectors = pg.group(jb, je);

  // need to place 5 jets in 2 conditions
  // 5.4.3.2/(2!3!) ways to do this (j1j2 and j2j1 in c1 is counted once).
  EXPECT_TRUE(groupVectors.size() == 0);
}


TEST(PartitionsGrouperTest, SingleJetGrouperBehavioiur){
  /* Test SingleJetGrouper bevaiour - split incoming jet collection
   * of n jets int n groups of 1 jet.
   */

  auto factory = TLorentzVectorFactory();
  
  HypoJetVector jets;

  constexpr double eta{0.5}; 
  for(int i = 1; i < 7; ++i){
    auto tlv = factory.make(eta, i);
    auto tl_j = std::make_shared<TLorentzVectorAsIJet>(tlv);
    jets.push_back(tl_j);
  }

  EXPECT_TRUE(jets.size() == 6);  // started at 1, not 0...

  //------------
  std::vector<std::size_t> mults{1}; // determines "single jet" behaviour
  //------------

  PartitionsGrouper pg(mults);
  auto jb = jets.begin();
  auto je = jets.end();
  auto groupVectors = pg.group(jb, je);

  // need to place 1 jet in 1 conditions
  // 6  ways to do this

  // grouper returns {[(1)], [(2)], [(3)], [(4)], [(5)], [(6)]}
  // "{" delimits vector<HypoJetGroupsVector<
  // "[" delimits HypoJetGroupVector (with one HypoJetVector entry)
  // "(" delimits HypoJetVector (with one jet entry)
  
  EXPECT_TRUE(groupVectors.size() ==6);
  for (const auto& gv : groupVectors){
    EXPECT_TRUE(gv.size() == 1);
    for(const auto & jv : gv){
      EXPECT_TRUE(jv.size() == 1);
    }
  }
}




TEST(PartitionsGrouperTest, CombinationsGrouperBehavioiur){
  /* Test SingleJetGrouper bevaiour - split incoming jet collection
   * of n jets int n groups of 1 jet.
   */

  auto factory = TLorentzVectorFactory();
  
  HypoJetVector jets;

  constexpr double eta{0.5}; 
  for(int i = 1; i < 7; ++i){
    auto tlv = factory.make(eta, i);
    auto tl_j = std::make_shared<TLorentzVectorAsIJet>(tlv);
    jets.push_back(tl_j);
  }

  EXPECT_TRUE(jets.size() == 6);  // started at 1, not 0...

  //------------
  std::vector<std::size_t> mults{2};  // "combinations" behaviour
  //------------

  PartitionsGrouper pg(mults);
  auto jb = jets.begin();
  auto je = jets.end();
  auto groupVectors = pg.group(jb, je);

  // need to place 1 jet in 1 conditions
  // 6  ways to do this

  // grouper returns {[(1, 2)], [(1, 3)], [(1, 4)], [(1, 5)], [(1, 6)],
  //                  [(2, 3)], [(2, 4)], [(2, 5)], [(2,6)]
  //                  [(3, 4)], [(3, 5)], [(3, 6)],
  //                  [(4, 5)], [(4, 6)],
  //                  [(5, 6)]
  //                 }
  // "{" delimits vector<HypoJetGroupsVector<
  // "[" delimits HypoJetGroupVector (with one HypoJetVector entry)
  // "(" delimits HypoJetVector (with one jet entry)
  
 

  EXPECT_TRUE(groupVectors.size() == 15);
  for (const auto& gv : groupVectors){
    EXPECT_TRUE(gv.size() == 1);
    for(const auto & jv : gv){
      EXPECT_TRUE(jv.size() == 2);
    }
  }
}

TEST(PartitionsGrouperTest, FullPartitionGrouperBehavioiur){
  /* Test SingleJetGrouper bevaiour - split incoming jet collection
   * of n jets int n groups of 1 jet.
   */

  auto factory = TLorentzVectorFactory();
  
  HypoJetVector jets;

  constexpr double eta{0.5}; 
  for(int i = 1; i < 7; ++i){
    auto tlv = factory.make(eta, i);
    auto tl_j = std::make_shared<TLorentzVectorAsIJet>(tlv);
    jets.push_back(tl_j);
  }

  EXPECT_TRUE(jets.size() == 6);  // started at 1, not 0...

  //------------
  std::vector<std::size_t> mults{1, 1};
  //------------

  
  PartitionsGrouper pg(mults);
  auto jb = jets.begin();
  auto je = jets.end();
  auto groupVectors = pg.group(jb, je);

  // need to place 1 jet in 1 conditions
  // 6  ways to do this

  // grouper returns
  // {
  //   [(1), (2)], [(1), (3)], [(1), (4)], [(1), (5)], [(1), (6)],
  //   [(2), (1)], [(2), (3)], [(2), (4)], [(2), (5)], [(2), (6)],
  //   [(3), (1)], [(3), (2)], [(3), (4)], [(3), (5)], [(3), (6)],
  //   [(4), (1)], [(4), (2)], [(4), (3)], [(4), (5)], [(4), (6)],
  //   [(5), (1)], [(5), (2)], [(5), (3)], [(5), (4)], [(5), (6)],
  //   [(6), (1)], [(6), (2)], [(6), (3)], [(6), (4)], [(6), (5)],
  // }
  //
  // "{" delimits vector<HypoJetGroupsVector<
  // "[" delimits HypoJetGroupVector (with one HypoJetVector entry)
  // "(" delimits HypoJetVector (with one jet entry)
  
 

  EXPECT_TRUE(groupVectors.size() == 30);
  for (const auto& gv : groupVectors){
    EXPECT_TRUE(gv.size() == 2);
    for(const auto & jv : gv){
      EXPECT_TRUE(jv.size() == 1);
    }
  }
}

