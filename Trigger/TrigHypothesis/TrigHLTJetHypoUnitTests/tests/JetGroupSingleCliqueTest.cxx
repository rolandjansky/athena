/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/JetGroupSingleClique.h"
#include <vector>
#include <map>

using vec = std::vector<size_t>;

TEST(JetGroupSingleCliqueTester, empty) {
  std::vector<std::size_t> satisfyingJets;
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  std::size_t n_required{1};
  JetGroupSingleClique jgsc(satisfyingJets, n_required);
  EXPECT_TRUE(jgsc.next(collector).empty());
}

TEST(JetGroupSingleCliqueTester, insufficient) {
  std::vector<std::size_t> satisfyingJets {0, 1};
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  std::size_t n_required{3};
  JetGroupSingleClique jgsc(satisfyingJets, n_required);
  EXPECT_TRUE(jgsc.next(collector).empty());
}


TEST(JetGroupSingleCliqueTester, sufficient) {
  std::vector<std::size_t> satisfyingJets {0u, 1u};
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  std::size_t n_required{2};
  JetGroupSingleClique jgsc(satisfyingJets, n_required);

  vec exp0 {0u, 1u};
  EXPECT_EQ(jgsc.next(collector), exp0);
}
