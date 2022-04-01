/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/JetGroupProductFactory.h"
#include <vector>

using vec = std::vector<std::size_t>;

TEST(JetGroupProductFactoryTester, nosiblings) {
 std::vector<std::size_t> siblings{};

  std::vector<bool> leaves{};
  
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t>();// root node

  std::vector<std::size_t> condMult{1};
  std::vector<unsigned int> condCap{0};
  std::vector<int> condClique{1};

  JetGroupInd2ElemInds jg2elemjgs;

  std::size_t parCapacity{0};

  bool simpleTree{true};
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  auto jgp = makeJetGroupProduct(siblings,
				 leaves,
				 satisfiedBy,
				 condMult,
				 condCap,
				 condClique,
				 jg2elemjgs,
				 parCapacity,
				 simpleTree,
				 collector);

  EXPECT_TRUE(jgp != nullptr);
  EXPECT_TRUE(jgp->valid());
  EXPECT_TRUE(jgp->next(collector).empty());
}


// Tests for simple trees - root is accept all, all other
// nodes are leaf nodes


TEST(JetGroupProductFactoryTester, one_clique_fail) {
  std::vector<std::size_t> siblings{1, 2};

  std::vector<bool> leaves{false, true, true};
  
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[2] = std::vector<std::size_t> {0, 1, 2};

  std::vector<std::size_t> condMult{1u, 5u, 1u};
  std::vector<unsigned int> condCap{0, 1, 1};
  std::vector<int> condClique{-1, 0, 0};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[2] = std::vector<std::size_t> {2};

  std::size_t parCapacity{0};
  bool simpleTree{true};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  auto jgp = makeJetGroupProduct(siblings,
				 leaves,
				 satisfiedBy,
				 condMult,
				 condCap,
				 condClique,
				 jg2elemjgs,
				 parCapacity,
				 simpleTree,
				 collector);


  EXPECT_TRUE(jgp != nullptr);
  EXPECT_TRUE(jgp->valid());

  EXPECT_TRUE(jgp->next(collector).empty());
  EXPECT_TRUE(jgp->next(collector).empty());
}

TEST(JetGroupProductFactoryTester, one_clique_pass) {
  std::vector<std::size_t> siblings{1, 2};

  std::vector<bool> leaves{true, true};


  // just enough jets to pass mono-clique, required = 5+1 = 6 jets
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2, 3, 4, 5};
  satisfiedBy[2] = std::vector<std::size_t> {0, 1, 2, 3, 4, 5};

  std::vector<std::size_t> condMult{1u, 5u, 1u};
  std::vector<unsigned int> condCap{0, 1, 1};
  std::vector<int> condClique{-1, 0, 0};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[2] = std::vector<std::size_t> {2};
  jg2elemjgs[3] = std::vector<std::size_t> {3};
  jg2elemjgs[4] = std::vector<std::size_t> {4};
  jg2elemjgs[5] = std::vector<std::size_t> {5};


  std::size_t parCapacity{0};
  bool simpleTree{true};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  auto jgp = makeJetGroupProduct(siblings,
				 leaves,
				 satisfiedBy,
				 condMult,
				 condCap,
				 condClique,
				 jg2elemjgs,
				 parCapacity,
				 simpleTree,
				 collector);

  EXPECT_TRUE(jgp != nullptr);
  EXPECT_TRUE(jgp->valid());

  vec exp0{0u, 1u, 2u, 3u, 4u, 5u};
  EXPECT_EQ(jgp->next(collector), exp0);
  EXPECT_TRUE(jgp->next(collector).empty());
  EXPECT_TRUE(jgp->next(collector).empty());
}

TEST(JetGroupProductFactoryTester, njets_eq_nsiblings) {
  std::vector<std::size_t> siblings{1, 2};

  std::vector<bool> leaves{false, true, true};


  // just enough jets to pass mono-clique, required = 5+1 = 6 jets
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0,1};
  satisfiedBy[2] = std::vector<std::size_t> {0,1};

  std::vector<std::size_t> condMult{1u, 1u, 1u};
  std::vector<unsigned int> condCap{0, 1, 1};
  std::vector<int> condClique{-1, 0, 1};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};

  std::size_t parCapacity{0};
  bool simpleTree{true};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  auto jgp = makeJetGroupProduct(siblings,
				 leaves,
				 satisfiedBy,
				 condMult,
				 condCap,
				 condClique,
				 jg2elemjgs,
				 parCapacity,
				 simpleTree,
				 collector);


  EXPECT_TRUE(jgp != nullptr);
  EXPECT_TRUE(jgp->valid());

  vec exp0{0u, 1u};
  EXPECT_EQ(jgp->next(collector), exp0);
  EXPECT_TRUE(jgp->next(collector).empty());
  EXPECT_TRUE(jgp->next(collector).empty());


}


TEST(JetGroupProductFactoryTester, 3siblings_3jets_pass) {
  std::vector<std::size_t> siblings{1, 2, 3};

  std::vector<bool> leaves{true, true, true};

  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0,1,2};
  satisfiedBy[2] = std::vector<std::size_t> {0};
  satisfiedBy[3] = std::vector<std::size_t> {1};

  std::vector<std::size_t> condMult{1u, 1u, 1u, 1u};
  std::vector<unsigned int> condCap{0, 1, 1, 1};
  std::vector<int> condClique{-1, 0, 1, 2};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[2] = std::vector<std::size_t> {2};

  std::size_t parCapacity{0};
  bool simpleTree{true};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  auto jgp = makeJetGroupProduct(siblings,
				 leaves,
				 satisfiedBy,
				 condMult,
				 condCap,
				 condClique,
				 jg2elemjgs,
				 parCapacity,
				 simpleTree,
				 collector);
  EXPECT_TRUE(jgp != nullptr);
  EXPECT_TRUE(jgp->valid());
  
  vec exp0{0u, 1u, 2u};
  EXPECT_EQ(jgp->next(collector), exp0);
  EXPECT_TRUE(jgp->next(collector).empty());
  EXPECT_TRUE(jgp->next(collector).empty());
}

TEST(JetGroupProductFactoryTester, 3siblings_3jets_fail) {
  std::vector<std::size_t> siblings{1, 2, 3};

  std::vector<bool> leaves{false, true, true, true};


  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0,1,2};
  satisfiedBy[2] = std::vector<std::size_t> {1};
  satisfiedBy[3] = std::vector<std::size_t> {1};

  std::vector<std::size_t> condMult{1u, 1u, 1u, 1u};
  std::vector<unsigned int> condCap{0, 1, 1, 1};
  std::vector<int> condClique{-1, 0, 1, 2};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[2] = std::vector<std::size_t> {2};

  std::size_t parCapacity{0};
  bool simpleTree{true};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  auto jgp = makeJetGroupProduct(siblings,
				 leaves,
				 satisfiedBy,
				 condMult,
				 condCap,
				 condClique,
				 jg2elemjgs,
				 parCapacity,
				 simpleTree,
				 collector);
  EXPECT_TRUE(jgp != nullptr);
  EXPECT_TRUE(jgp->valid());

  EXPECT_TRUE(jgp->next(collector).empty());
  EXPECT_TRUE(jgp->next(collector).empty());
}


// Tests for non-AcceptAll parent

TEST(JetGroupProductFactoryTester, dijet_jets_overlap) {
  std::vector<std::size_t> siblings{1, 2};

  std::vector<bool> leaves{false, true, true};


  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1};
  satisfiedBy[2] = std::vector<std::size_t> {0, 1};

  std::vector<std::size_t> condMult{1u, 1u, 1u};
  std::vector<unsigned int> condCap{2, 1, 1};
  std::vector<int> condClique{-1, 0, 0};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};

  std::size_t parCapacity{2};
  bool simpleTree{false};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  auto jgp = makeJetGroupProduct(siblings,
				 leaves,
				 satisfiedBy,
				 condMult,
				 condCap,
				 condClique,
				 jg2elemjgs,
				 parCapacity,
				 simpleTree,
				 collector);

  EXPECT_TRUE(jgp != nullptr);
  EXPECT_TRUE(jgp->valid());

  vec e0{0u, 1u};
  EXPECT_EQ(jgp->next(collector), e0);

  EXPECT_TRUE(jgp->next(collector).empty());
  EXPECT_TRUE(jgp->next(collector).empty());
}

TEST(JetGroupProductFactoryTester, dijet_jets_no_overlap) {
  std::vector<std::size_t> siblings{1, 2};

  std::vector<bool> leaves{false, true, true};


  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1};
  satisfiedBy[2] = std::vector<std::size_t> {2, 3};

  std::vector<std::size_t> condMult{1u, 1u, 1u};
  std::vector<unsigned int> condCap{2, 1, 1};
  std::vector<int> condClique{-1, 0, 0};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[1] = std::vector<std::size_t> {2};
  jg2elemjgs[1] = std::vector<std::size_t> {3};

  std::size_t parCapacity{2};
  bool simpleTree{false};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  auto jgp = makeJetGroupProduct(siblings,
				 leaves,
				 satisfiedBy,
				 condMult,
				 condCap,
				 condClique,
				 jg2elemjgs,
				 parCapacity,
				 simpleTree,
				 collector);
  EXPECT_TRUE(jgp != nullptr);
  EXPECT_TRUE(jgp->valid());

  vec e0{0u, 2u};
  vec e1{1u, 2u};
  vec e2{0u, 3u};
  vec e3{1u, 3u};
  EXPECT_EQ(jgp->next(collector), e0);
  EXPECT_EQ(jgp->next(collector), e1);
  EXPECT_EQ(jgp->next(collector), e2);
  EXPECT_EQ(jgp->next(collector), e3);
  EXPECT_TRUE(jgp->next(collector).empty());
  EXPECT_TRUE(jgp->next(collector).empty());
}


TEST(JetGroupProductFactoryTester, dijet_jets_some_overlap) {
  std::vector<std::size_t> siblings{1, 2};

  std::vector<bool> leaves{false, true, true};


  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[2] = std::vector<std::size_t> {2, 3, 4};

  std::vector<std::size_t> condMult{1u, 1u, 1u};
  std::vector<unsigned int> condCap{2, 1, 1};
  std::vector<int> condClique{-1, 0, 0};

  JetGroupInd2ElemInds jg2elemjgs;
  jg2elemjgs[0] = std::vector<std::size_t> {0};
  jg2elemjgs[1] = std::vector<std::size_t> {1};
  jg2elemjgs[1] = std::vector<std::size_t> {2};
  jg2elemjgs[1] = std::vector<std::size_t> {3};
  jg2elemjgs[1] = std::vector<std::size_t> {4};

  std::size_t parCapacity{2};
  bool simpleTree{false};

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  auto jgp = makeJetGroupProduct(siblings,
				 leaves,
				 satisfiedBy,
				 condMult,
				 condCap,
				 condClique,
				 jg2elemjgs,
				 parCapacity,
				 simpleTree,
				 collector);
  EXPECT_TRUE(jgp != nullptr);
  EXPECT_TRUE(jgp->valid());

  vec e0{0u, 2u};
  vec e1{1u, 2u};
  vec e2{0u, 3u};
  vec e3{1u, 3u};
  vec e4{2u, 3u};
  vec e5{0u, 4u};
  vec e6{1u, 4u};
  vec e7{2u, 4u};

  EXPECT_EQ(jgp->next(collector), e0);
  EXPECT_EQ(jgp->next(collector), e1);
  EXPECT_EQ(jgp->next(collector), e2);
  EXPECT_EQ(jgp->next(collector), e3);
  EXPECT_EQ(jgp->next(collector), e4);
  EXPECT_EQ(jgp->next(collector), e5);
  EXPECT_EQ(jgp->next(collector), e6);
  EXPECT_EQ(jgp->next(collector), e7);

  EXPECT_TRUE(jgp->next(collector).empty());
  EXPECT_TRUE(jgp->next(collector).empty());
}



TEST(JetGroupProductFactoryTester, max_jet_0) {
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0};
  satisfiedBy[2] = std::vector<std::size_t> {0};

  std::vector<std::size_t> siblings{1, 2};

  EXPECT_EQ(0, max_jet(siblings, satisfiedBy));
}

TEST(JetGroupProductFactoryTester, max_jet_1) {
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[2] = std::vector<std::size_t> {2, 3, 4};

  std::vector<std::size_t> siblings{1, 2};

  EXPECT_EQ(4, max_jet(siblings, satisfiedBy));
}


TEST(JetGroupProductFactoryTester, willpass_fail_0) {
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[2] = std::vector<std::size_t> {1};
  satisfiedBy[3] = std::vector<std::size_t> {1};

  std::vector<std::size_t> condMult{1, 1, 1, 1};

  std::vector<std::size_t> siblings{1, 2, 3};

  EXPECT_EQ(willPassSimpleTree(siblings, satisfiedBy, condMult), false);
}

TEST(JetGroupProductFactoryTester, willpass_fail_1) {
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[2] = std::vector<std::size_t> {2};
  satisfiedBy[3] = std::vector<std::size_t> {1};

  std::vector<std::size_t> condMult{1, 2, 1, 1};

  std::vector<std::size_t> siblings{1, 2, 3};

  EXPECT_EQ(willPassSimpleTree(siblings, satisfiedBy, condMult), false);
}


TEST(JetGroupProductFactoryTester, willpass_fail_2) {
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {};
  satisfiedBy[2] = std::vector<std::size_t> {};
  satisfiedBy[3] = std::vector<std::size_t> {};

  std::vector<std::size_t> condMult{1, 2, 1, 1};

  std::vector<std::size_t> siblings{1, 2, 3};

  EXPECT_EQ(willPassSimpleTree(siblings, satisfiedBy, condMult), false);
}

TEST(JetGroupProductFactoryTester, willpass_pass_0) {
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2};
  satisfiedBy[2] = std::vector<std::size_t> {2};
  satisfiedBy[3] = std::vector<std::size_t> {1};

  std::vector<std::size_t> condMult{1, 1, 1, 1};

  std::vector<std::size_t> siblings{1, 2, 3};

  EXPECT_EQ(willPassSimpleTree(siblings, satisfiedBy, condMult), true);
}

TEST(JetGroupProductFactoryTester, willpass_pass_1) {
  CondInd2JetGroupsInds satisfiedBy;
  satisfiedBy[0] = std::vector<std::size_t> {};
  satisfiedBy[1] = std::vector<std::size_t> {0, 1, 2, 3};
  satisfiedBy[2] = std::vector<std::size_t> {0, 1, 2};

  std::vector<std::size_t> condMult{1, 2, 2};

  std::vector<std::size_t> siblings{1, 2};

  EXPECT_EQ(willPassSimpleTree(siblings, satisfiedBy, condMult), true);
}
