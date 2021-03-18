/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/JetGroupUnion.h"
#include <vector>
#include <map>

using res = std::pair<std::vector<unsigned int>, bool>;
using vec = std::vector<unsigned int>;

TEST(JetGroupUnionTester, empty) {
  std::vector<std::size_t> siblings;
  CondInd2JetGroupsInds satisfiedBy;
  JetGroupInd2ElemInds jg2elemjgs;
  
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupUnion jgu(siblings, satisfiedBy, jg2elemjgs, collector);

  EXPECT_TRUE(jgu.next(collector).empty());
}


TEST(JetGroupUnionTester, one_elementary_cond) {
  std::vector<std::size_t> siblings{0};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {0, 1, 2};
  std::vector<std::size_t> condMult{1};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[2] = std::vector<std::size_t> {2};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupUnion jgu(siblings, satisfiedBy, jg2elemjgs, collector);
  
  auto exp = std::vector<std::size_t>{0, 1, 2};
  EXPECT_EQ(jgu.next(collector), exp);
  EXPECT_TRUE(jgu.next(collector).empty());
}


TEST(JetGroupUnionTester, two_elem_nooverlap) {
  std::vector<std::size_t> siblings{0, 1};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[1] = std::vector<std::size_t> {3, 4, 5};
  std::vector<std::size_t> condMult{1};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[2] = std::vector<std::size_t> {2};
  jg2elemjgs[3] = std::vector<std::size_t> {3};
  jg2elemjgs[4] = std::vector<std::size_t> {4};
  jg2elemjgs[5] = std::vector<std::size_t> {5};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupUnion jgu(siblings, satisfiedBy, jg2elemjgs, collector);
  
  auto exp = std::vector<std::size_t>{0, 1, 2, 3, 4, 5};
  EXPECT_EQ(jgu.next(collector), exp);
  EXPECT_TRUE(jgu.next(collector).empty());
}

TEST(JetGroupUnionTester, two_elem_withoverlap) {
  std::vector<std::size_t> siblings{0, 1};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[1] = std::vector<std::size_t> {3, 1, 5};
  std::vector<std::size_t> condMult{1};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[2] = std::vector<std::size_t> {2};
  jg2elemjgs[3] = std::vector<std::size_t> {3};
  jg2elemjgs[4] = std::vector<std::size_t> {4};
  jg2elemjgs[5] = std::vector<std::size_t> {5};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupUnion jgu(siblings, satisfiedBy, jg2elemjgs, collector);
  
  auto exp = std::vector<std::size_t>{0, 1, 2, 3, 5};
  EXPECT_EQ(jgu.next(collector), exp);
  EXPECT_TRUE(jgu.next(collector).empty());
}


TEST(JetGroupUnionTester, two_comp_nooverlap) {
  std::vector<std::size_t> siblings{0, 1};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {10};
  satisfiedBy[1] = std::vector<std::size_t> {20};
  std::vector<std::size_t> condMult{1};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[2] = std::vector<std::size_t> {2};
  jg2elemjgs[3] = std::vector<std::size_t> {3};
  jg2elemjgs[4] = std::vector<std::size_t> {4};
  jg2elemjgs[5] = std::vector<std::size_t> {5};
  jg2elemjgs[10] = std::vector<std::size_t> {0, 1, 2};
  jg2elemjgs[20] = std::vector<std::size_t> {3, 4, 5};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupUnion jgu(siblings, satisfiedBy, jg2elemjgs, collector);
  
  auto exp = std::vector<std::size_t>{0, 1, 2, 3, 4, 5};
  EXPECT_EQ(jgu.next(collector), exp);
  EXPECT_TRUE(jgu.next(collector).empty());
}


TEST(JetGroupUnionTester, two_comp_withoverlap) {
  std::vector<std::size_t> siblings{0, 1};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {10};
  satisfiedBy[1] = std::vector<std::size_t> {20};
  std::vector<std::size_t> condMult{1};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[2] = std::vector<std::size_t> {2};
  jg2elemjgs[3] = std::vector<std::size_t> {3};
  jg2elemjgs[4] = std::vector<std::size_t> {4};
  jg2elemjgs[5] = std::vector<std::size_t> {5};
  jg2elemjgs[10] = std::vector<std::size_t> {0, 1, 2};
  jg2elemjgs[20] = std::vector<std::size_t> {0, 1, 5};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  JetGroupUnion jgu(siblings, satisfiedBy, jg2elemjgs, collector);
  
  auto exp = std::vector<std::size_t>{0, 1, 2, 5};
  EXPECT_EQ(jgu.next(collector), exp);
  EXPECT_TRUE(jgu.next(collector).empty());
}
