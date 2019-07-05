/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/PartitionsGen.h"
#include <vector>
#include <set>

using res = std::vector<std::vector<std::size_t>>;
using vec = std::vector<std::size_t>;

TEST(PartitionsGenTester, zero0) {

  vec v;
  std::size_t toAllocate{0};
  PartitionsGen gen(toAllocate, v);


  EXPECT_FALSE (gen.next().has_value());
}

TEST(PartitionsGenTester, m2m1) {
  using vst = std::vector<std::size_t>;
  std::vector<res> expected{
    res{vst{1, 2}, vst{3}},
    res{vst{1, 3}, vst{2}},
    res{vst{2, 3}, vst{1}},
    res{vst{0, 2}, vst{3}},
    res{vst{0, 3}, vst{2}},
    res{vst{0, 1}, vst{3}},
    res{vst{0, 1}, vst{2}},
    res{vst{0, 3}, vst{1}},
    res{vst{0, 2}, vst{1}},
    res{vst{2, 3}, vst{0}},
    res{vst{1, 3}, vst{0}},
    res{vst{1, 2}, vst{0}}
  };
  
  vec v{2, 1};
  std::size_t to_allocate{4};
  PartitionsGen gen(to_allocate, v);

  std::set<res> results_set;

  // check values without considering order, and total number of
  // different partitions.
  while(true){
    auto next = gen.next();
    if (!next.has_value()){break;}
    EXPECT_NE(std::find(expected.begin(),
			expected.end(),
			*next),
	      expected.end());
    results_set.insert(*next);
  }

  EXPECT_EQ (results_set.size(), expected.size());
}

TEST(PartitionsGenTester, m2m3) {
  
  vec v{2, 3};
  std::size_t to_allocate{6};
  PartitionsGen gen(to_allocate, v);

  // check values without considering order, and total number of
  // different partitions.
  std::set<res> results_set;

  while(true){
    auto next = gen.next();
    if (!next.has_value()){break;}
    results_set.insert(*next);
  }

  //60 = 6.5.4.3.2/(2! 3!)
  EXPECT_EQ (results_set.size(), 60);
}
