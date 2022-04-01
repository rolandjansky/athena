/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/JetGroupReducer.h"
#include <vector>
#include <map>

using res = std::pair<std::vector<unsigned int>, bool>;
using vec = std::vector<unsigned int>;

TEST(JetGroupReducerTester, empty) {
  std::vector<std::size_t> siblings;
  CondInd2JetGroupsInds satisfiedBy;
  JetGroupInd2ElemInds jg2elem;

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupReducer jgr(siblings, satisfiedBy, jg2elem);

  EXPECT_TRUE(jgr.valid());
  EXPECT_TRUE(jgr.next(collector).empty());
}

TEST(JetGroupReducerTester, emptyCondition) {
  std::vector<std::size_t> siblings{1, 2, 3};
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[1] = std::vector<std::size_t> {};
  satisfiedBy[2] = std::vector<std::size_t> {3, 4, 5};
  JetGroupInd2ElemInds jg2elem;

  

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupReducer jgr(siblings, satisfiedBy, jg2elem);

  EXPECT_TRUE(jgr.valid());
  EXPECT_TRUE(jgr.next(collector).empty());
}


TEST(JetGroupReducerTester, one_condition_elem) {

  // condition is satisfied by an elementary jet group
  std::vector<std::size_t> siblings{1};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[1] = std::vector<std::size_t> {0};
  JetGroupInd2ElemInds jg2elem;
  jg2elem[0] = std::vector<std::size_t> {0};
  jg2elem[1] = std::vector<std::size_t> {1};
  jg2elem[2] = std::vector<std::size_t> {0, 1};


  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupReducer jgr(siblings, satisfiedBy, jg2elem);
  
  auto exp = std::vector<std::size_t>{0};
  EXPECT_EQ(jgr.next(collector), exp);
  EXPECT_TRUE(jgr.next(collector).empty());
}


TEST(JetGroupReducerTester, one_condition_non_elem) {

  // condition is satisfied by an non-elementary jet group
  std::vector<std::size_t> siblings{1};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[1] = std::vector<std::size_t> {2};
  JetGroupInd2ElemInds jg2elem;
  jg2elem[0] = std::vector<std::size_t> {0};
  jg2elem[1] = std::vector<std::size_t> {1};
  jg2elem[2] = std::vector<std::size_t> {0, 1};


  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupReducer jgr(siblings, satisfiedBy, jg2elem);
  
  auto exp = std::vector<std::size_t>{0, 1};
  EXPECT_EQ(jgr.next(collector), exp);
  EXPECT_TRUE(jgr.next(collector).empty());
}

TEST(JetGroupReducerTester, two_conditions_nooverlap_elem_elem) {
  std::vector<std::size_t> siblings{1, 2};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[1] = std::vector<std::size_t> {0, 1};
  satisfiedBy[2] = std::vector<std::size_t> {2};
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupInd2ElemInds jg2elem;

  jg2elem[0] = std::vector<std::size_t> {0};
  jg2elem[1] = std::vector<std::size_t> {1};
  jg2elem[2] = std::vector<std::size_t> {2};
  jg2elem[3] = std::vector<std::size_t> {3};
  jg2elem[4] = std::vector<std::size_t> {4};
  jg2elem[5] = std::vector<std::size_t> {5};
  jg2elem[6] = std::vector<std::size_t> {0, 1};
  jg2elem[7] = std::vector<std::size_t> {2, 3};
  jg2elem[8] = std::vector<std::size_t> {4, 5};


  JetGroupReducer jgr(siblings, satisfiedBy, jg2elem);
  
  auto exp = std::vector<std::size_t>{0, 1, 2};
  EXPECT_EQ(jgr.next(collector), exp);
  EXPECT_TRUE(jgr.next(collector).empty());
}

TEST(JetGroupReducerTester, two_conditions_nooverlap_elem_non_elem) {
  std::vector<std::size_t> siblings{1, 2};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[1] = std::vector<std::size_t> {0, 1};
  satisfiedBy[2] = std::vector<std::size_t> {7};
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupInd2ElemInds jg2elem;

  jg2elem[0] = std::vector<std::size_t> {0};
  jg2elem[1] = std::vector<std::size_t> {1};
  jg2elem[2] = std::vector<std::size_t> {2};
  jg2elem[3] = std::vector<std::size_t> {3};
  jg2elem[4] = std::vector<std::size_t> {4};
  jg2elem[5] = std::vector<std::size_t> {5};
  jg2elem[6] = std::vector<std::size_t> {0, 1};
  jg2elem[7] = std::vector<std::size_t> {2, 3};
  jg2elem[8] = std::vector<std::size_t> {4, 5};


  JetGroupReducer jgr(siblings, satisfiedBy, jg2elem);
  
  auto exp = std::vector<std::size_t>{0, 1, 2, 3};
  EXPECT_EQ(jgr.next(collector), exp);
  EXPECT_TRUE(jgr.next(collector).empty());
}

TEST(JetGroupReducerTester, two_conditions_overlap_elem_non_elem) {
  std::vector<std::size_t> siblings{1, 2};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[1] = std::vector<std::size_t> {0, 1};
  satisfiedBy[2] = std::vector<std::size_t> {6};
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupInd2ElemInds jg2elem;

  jg2elem[0] = std::vector<std::size_t> {0};
  jg2elem[1] = std::vector<std::size_t> {1};
  jg2elem[2] = std::vector<std::size_t> {2};
  jg2elem[3] = std::vector<std::size_t> {3};
  jg2elem[4] = std::vector<std::size_t> {4};
  jg2elem[5] = std::vector<std::size_t> {5};
  jg2elem[6] = std::vector<std::size_t> {0, 1};
  jg2elem[7] = std::vector<std::size_t> {2, 3};
  jg2elem[8] = std::vector<std::size_t> {4, 5};


  JetGroupReducer jgr(siblings, satisfiedBy, jg2elem);
  
  auto exp = std::vector<std::size_t>{0, 1};
  EXPECT_EQ(jgr.next(collector), exp);
  EXPECT_TRUE(jgr.next(collector).empty());
}

TEST(JetGroupReducerTester, two_conditions_no_overlap_non_elem_non_elem) {
  std::vector<std::size_t> siblings{1, 2};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[1] = std::vector<std::size_t> {6, 7};
  satisfiedBy[2] = std::vector<std::size_t> {8};
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupInd2ElemInds jg2elem;

  jg2elem[0] = std::vector<std::size_t> {0};
  jg2elem[1] = std::vector<std::size_t> {1};
  jg2elem[2] = std::vector<std::size_t> {2};
  jg2elem[3] = std::vector<std::size_t> {3};
  jg2elem[4] = std::vector<std::size_t> {4};
  jg2elem[5] = std::vector<std::size_t> {5};
  jg2elem[6] = std::vector<std::size_t> {0, 1};
  jg2elem[7] = std::vector<std::size_t> {2, 3};
  jg2elem[8] = std::vector<std::size_t> {4, 5};


  JetGroupReducer jgr(siblings, satisfiedBy, jg2elem);
  
  auto exp = std::vector<std::size_t>{0, 1, 2, 3, 4, 5};
  EXPECT_EQ(jgr.next(collector), exp);
  EXPECT_TRUE(jgr.next(collector).empty());
}

TEST(JetGroupReducerTester, two_conditions_overlap_non_elem_non_elem) {
  std::vector<std::size_t> siblings{1, 2};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[1] = std::vector<std::size_t> {6, 7};
  satisfiedBy[2] = std::vector<std::size_t> {9};
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupInd2ElemInds jg2elem;

  jg2elem[0] = std::vector<std::size_t> {0};
  jg2elem[1] = std::vector<std::size_t> {1};
  jg2elem[2] = std::vector<std::size_t> {2};
  jg2elem[3] = std::vector<std::size_t> {3};
  jg2elem[4] = std::vector<std::size_t> {4};
  jg2elem[5] = std::vector<std::size_t> {5};
  jg2elem[6] = std::vector<std::size_t> {0, 1};
  jg2elem[7] = std::vector<std::size_t> {2, 3};
  jg2elem[8] = std::vector<std::size_t> {4, 5};
  jg2elem[9] = std::vector<std::size_t> {1, 5};


  JetGroupReducer jgr(siblings, satisfiedBy, jg2elem);
  
  auto exp = std::vector<std::size_t>{0, 1, 2, 3, 5};
  EXPECT_EQ(jgr.next(collector), exp);
  EXPECT_TRUE(jgr.next(collector).empty());
}

