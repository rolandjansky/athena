/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGen.h"
#include <vector>

using res = std::pair<std::vector<unsigned int>, bool>;
using vec = std::vector<unsigned int>;

TEST(CombinationsGenTester, n3k1) { 
  CombinationsGen gen(3,1);
  EXPECT_EQ (res(vec{0}, true), gen.next());
  EXPECT_EQ (res(vec{1}, true), gen.next());
  EXPECT_EQ (res(vec{2}, true), gen.next());
  res r =  gen.next();
  EXPECT_EQ (r.second, false);
}

TEST(CombinationsGenTester, n3k2) { 
  CombinationsGen gen(3,2);
  EXPECT_EQ (res(vec{0,1}, true), gen.next());
  EXPECT_EQ (res(vec{0,2}, true), gen.next());
  EXPECT_EQ (res(vec{1,2}, true), gen.next());
  res r =  gen.next();
  EXPECT_EQ (r.second, false);
}


TEST(CombinationsGenTester, n3k3) { 
  CombinationsGen gen(3,3);
  EXPECT_EQ (res(vec{0,1, 2}, true), gen.next());
  res r =  gen.next();
  EXPECT_EQ (r.second, false);
}

TEST(CombinationsGenTester, n3k0) { 
  CombinationsGen gen(3,0);
  EXPECT_EQ (res(vec{}, true), gen.next());
  res r =  gen.next();
  EXPECT_EQ (r.second, false);
}

TEST(CombinationsGenTester, n3kgtn) { 
  CombinationsGen gen(3,4);
  EXPECT_FALSE(gen.next().second);
}
