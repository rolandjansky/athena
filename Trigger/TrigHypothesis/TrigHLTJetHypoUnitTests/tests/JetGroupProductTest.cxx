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

  EXPECT_TRUE((jgp.next(collector)).empty());
}



TEST(JetGroupProductTester, onecond) {
  std::vector<std::size_t> siblings{0};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {0, 1, 2};

  std::vector<std::size_t> condMult{1};

  JetGroupProduct jgp(siblings, satisfiedBy, condMult);

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
  
  auto exp = std::vector<std::size_t>{0};
  EXPECT_EQ(jgp.next(collector), exp);

  exp = std::vector<std::size_t>{1};
  EXPECT_EQ(jgp.next(collector), exp);

  exp = std::vector<std::size_t>{2};
  EXPECT_EQ(jgp.next(collector), exp);

  EXPECT_TRUE(jgp.next(collector).empty());
}

TEST(JetGroupProductTester, repeatedcond) {
  std::vector<std::size_t> siblings{0};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {0, 1, 2};

  std::vector<std::size_t> condMult{2};

  JetGroupProduct jgp(siblings, satisfiedBy, condMult);

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
  
  auto exp = std::vector<std::size_t>{0, 1};
  EXPECT_EQ(jgp.next(collector), exp);

  exp = std::vector<std::size_t>{0, 2};
  EXPECT_EQ(jgp.next(collector), exp);

 
  exp = std::vector<std::size_t>{1, 2};
  EXPECT_EQ(jgp.next(collector), exp);

  EXPECT_TRUE(jgp.next(collector).empty());
}

TEST(JetGroupProductTester, twocond) {
  std::vector<std::size_t> siblings{0, 1};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};

  std::vector<std::size_t> condMult{1, 1};

  JetGroupProduct jgp(siblings, satisfiedBy, condMult);

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
  
  auto exp = std::vector<std::size_t>{0, 1};
  EXPECT_EQ(jgp.next(collector), exp);

  exp = std::vector<std::size_t>{0, 2};
  EXPECT_EQ(jgp.next(collector), exp);
  
  exp = std::vector<std::size_t>{1, 2};
  EXPECT_EQ(jgp.next(collector), exp);

  EXPECT_TRUE(jgp.next(collector).empty());
  if(collector) {collector->write();}

}

