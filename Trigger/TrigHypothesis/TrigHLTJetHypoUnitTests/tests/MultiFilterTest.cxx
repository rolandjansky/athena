/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/ConditionFilter.h"
#include "TrigHLTJetHypo/../src/ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/../src/EtaConditionAbsMT.h"
#include "TrigHLTJetHypo/../src/RepeatedCondition.h"
#include "TrigHLTJetHypo/../src/ITrigHypoJetVectorFilterConfig.h"


#include "../src/TLorentzVectorFactory.h"
#include "../src/TLorentzVectorAsIJet.h"
#include "../src/makeHypoJets.h"
#include "../src/makeRepeatedConditions.h"

#include <vector>
#include <memory>



TEST(MultiFilterTester, fourjets_twofilters) {

  EtaPairs cond_etas;
  cond_etas.push_back(std::make_pair(0., 2.5));
  cond_etas.push_back(std::make_pair(0., 1.5));

  // vector of two ConditionPtr s
  // ConditionPtrs conditionPtrs = makeRepeatedConditions(cond_etas);

  std::vector<FilterPtr> filters{};

  for(const auto& ce : cond_etas) {
    filters.push_back(std::make_unique<ConditionFilter>(makeRepeatedConditions(ce)));
  }
  
  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);

  std::vector<double> jet_eta{1.0, 0.5, 2.0, 4.0};
  HypoJetVector tv = makeHypoJets(jet_eta);

  std::pair<HypoJetCIter, HypoJetCIter> iters =
    std::make_pair(tv.begin(), tv.end());

  EXPECT_EQ(filters.size(), 2);

  // apply the filters in a chain
  for (const auto& fp : filters) {
    iters = fp->filter(iters.first, iters.second, deb);
  }

  EXPECT_EQ((iters.second - iters.first), 2);

  const auto& fiter = iters.first;
  const auto& siter = iters.second;

  auto etas = std::vector<double> {};
  etas.reserve(siter-fiter);
  std::transform(fiter,
		 siter,
		 std::back_inserter(etas),
		 [](const pHypoJet& hj){return hj->eta();});
  std::sort(etas.begin(), etas.end());

  EXPECT_EQ(etas.size(), 2);
  EXPECT_DOUBLE_EQ(etas[0], 0.5);
  EXPECT_DOUBLE_EQ(etas[1], 1.0);

}
