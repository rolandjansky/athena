/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/ProductGen.h"
#include <vector>

using res = std::pair<std::vector<unsigned int>, bool>;
using vec = std::vector<unsigned int>;

TEST(ProductGenTester, noargs) { 
  ProductGen gen;
  EXPECT_TRUE(gen.next().empty());
}

TEST(ProductGenTester, empty) {
  std::vector<std::size_t> ends;
  ProductGen gen(ends);
  EXPECT_TRUE(gen.next().empty());
}

TEST(ProductGenTester, zeroend) { 
  ProductGen gen(std::vector<std::size_t>({0,2}));

  // no values due to and end value of 0.
  EXPECT_TRUE(gen.next().empty());
}


TEST(ProductGenTester, ends12) { 
  ProductGen gen(std::vector<std::size_t>({2, 3}));

  std::vector<std::size_t> exp{0, 0};
  EXPECT_EQ(gen.next(), exp);

  exp = std::vector<std::size_t>{1, 0};
  EXPECT_EQ(gen.next(), exp);

  exp = std::vector<std::size_t>{0, 1};
  EXPECT_EQ(gen.next(), exp);

  exp = std::vector<std::size_t>{1, 1};
  EXPECT_EQ(gen.next(), exp);

  exp = std::vector<std::size_t>{0, 2};
  EXPECT_EQ(gen.next(), exp);
  
  exp = std::vector<std::size_t>{1, 2};
  EXPECT_EQ(gen.next(), exp);
  
  EXPECT_TRUE(gen.next().empty());
}

