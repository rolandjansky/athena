/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/JetGroupProduct.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include <vector>
#include <map>

using res = std::pair<std::vector<unsigned int>, bool>;
using vec = std::vector<unsigned int>;

TEST(JetGroupProductTester, empty) {
  std::vector<std::size_t> siblings;
  CondInd2JetGroupsInds satisfiedBy;
  std::vector<std::size_t> condMult;
  
  JetGroupProduct jgp(siblings, satisfiedBy, condMult);
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  EXPECT_FALSE(jgp.valid());
  EXPECT_TRUE((jgp.next(collector)).empty());
}



TEST(JetGroupProductTester, onecondition) {
  std::vector<std::size_t> siblings{1};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t>();
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};

  std::vector<std::size_t> condMult{0, 1};

  JetGroupProduct jgp(siblings, satisfiedBy, condMult);

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
  
  EXPECT_TRUE(jgp.valid());

  auto exp = std::vector<std::size_t>{0};
  EXPECT_EQ(jgp.next(collector), exp);

  exp = std::vector<std::size_t>{1};
  EXPECT_EQ(jgp.next(collector), exp);

  exp = std::vector<std::size_t>{2};
  EXPECT_EQ(jgp.next(collector), exp);

  EXPECT_TRUE(jgp.next(collector).empty());
}

TEST(JetGroupProductTester, repeatedcond) {
  std::vector<std::size_t> siblings{1};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t>();
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};

  std::vector<std::size_t> condMult{1, 2};

  JetGroupProduct jgp(siblings, satisfiedBy, condMult);

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  EXPECT_TRUE(jgp.valid());

  auto exp = std::vector<std::size_t>{0, 1};
  EXPECT_EQ(jgp.next(collector), exp);

  exp = std::vector<std::size_t>{0, 2};
  EXPECT_EQ(jgp.next(collector), exp);

 
  exp = std::vector<std::size_t>{1, 2};
  EXPECT_EQ(jgp.next(collector), exp);

  EXPECT_TRUE(jgp.next(collector).empty());
}

TEST(JetGroupProductTester, twocond) {
  std::vector<std::size_t> siblings{1, 2};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t>();
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[2] = std::vector<std::size_t> {0, 1, 2};

  std::vector<std::size_t> condMult{1, 1, 1};

  JetGroupProduct jgp(siblings, satisfiedBy, condMult);

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  EXPECT_TRUE(jgp.valid());

  auto exp = std::vector<std::size_t>{0, 1};
  EXPECT_EQ(jgp.next(collector), exp);

  exp = std::vector<std::size_t>{0, 2};
  EXPECT_EQ(jgp.next(collector), exp);
  
  exp = std::vector<std::size_t>{1, 2};
  EXPECT_EQ(jgp.next(collector), exp);

  EXPECT_TRUE(jgp.next(collector).empty());
  if(collector) {collector->write();}

}

TEST(JetGroupProductTester, reallife) {
  std::vector<std::size_t> siblings{1, 2};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t>();
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[2] = std::vector<std::size_t> {
    0, 1, 2, 3, 4, 6, 7, 9, 10, 11, 12, 13,
    14, 15, 17, 19, 21, 25, 26, 29, 33};

  std::vector<std::size_t> condMult{1, 1, 3};

  JetGroupProduct jgp(siblings, satisfiedBy, condMult);

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  EXPECT_TRUE(jgp.valid());
  
  std::size_t ipass{0};
  
  while (true) {
    auto indices = jgp.next(collector);
    if (indices.empty()){break;}
    ++ipass;
  }

  // 2925 = 3*C(21,3) - overlaps and duplicates
  EXPECT_EQ (ipass, 2925u);
  
}

