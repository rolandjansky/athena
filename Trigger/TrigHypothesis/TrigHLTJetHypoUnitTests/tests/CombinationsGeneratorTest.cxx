/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/CombinationsGenerator.h"
#include <vector>

using vec = std::vector<size_t>;

TEST(CombinationsGeneratorTester, n3k1) { 
  CombinationsGenerator gen(3,1);
  // return value from bump() says whether the generator has cycled.
  EXPECT_EQ (vec{0}, gen.get());
  EXPECT_EQ (false, gen.bump());
  EXPECT_EQ (vec{1}, gen.get());
  EXPECT_EQ (false, gen.bump());
  EXPECT_EQ (vec{2}, gen.get());
  EXPECT_EQ (true, gen.bump());
}

TEST(CombinationsGeneratorTester, n3k2) { 
  CombinationsGenerator gen(3,2);
  vec v0 {0,1};
  vec v1 {0,2};
  vec v2 {1,2};
  EXPECT_EQ (v0, gen.get());
  EXPECT_EQ (false, gen.bump());
  EXPECT_EQ (v1, gen.get());
  EXPECT_EQ (false, gen.bump());
  EXPECT_EQ (v2, gen.get());
  EXPECT_EQ (true, gen.bump());
}

TEST(CombinationsGeneratorTester, n3k3) { 
  CombinationsGenerator gen(3,3);
  vec v0 {0,1,2};
  EXPECT_EQ (v0, gen.get());
  EXPECT_EQ (true, gen.bump());
}

TEST(CombinationsGeneratorTester, n3k0) { 
  CombinationsGenerator gen(3,0);
  vec v0 {};
  EXPECT_EQ (v0, gen.get());
  EXPECT_EQ (true, gen.bump());
}

TEST(CombinationsGeneratorTester, n3k4) { 
  CombinationsGenerator gen(3,4);
  vec v0 {};
  EXPECT_EQ (v0, gen.get());
  EXPECT_EQ (true, gen.bump());
}



