/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/Tree.h"
#include <vector>
#include <map>

using res = std::pair<std::vector<unsigned int>, bool>;
using vec = std::vector<std::size_t>;

TEST(TreeTester, empty) {
  std::vector<std::size_t> parents;
  
  Tree tree(parents);

  EXPECT_EQ(tree.size(), 0u);
}


TEST(TreeTester, simple) {
  std::vector<std::size_t> parents {0u, 0u, 0u};
  
  Tree tree(parents);

  vec leaves {1u, 2u};
  EXPECT_EQ(tree.size(), 3u);
  EXPECT_EQ(tree.leaves(), leaves);
  EXPECT_FALSE(tree.is_leaf(0u));
  EXPECT_TRUE(tree.is_leaf(1u));
  EXPECT_TRUE(tree.is_leaf(1u));

  EXPECT_TRUE(tree.is_simple());
  
}


TEST(TreeTester, notsimple) {
  std::vector<std::size_t> parents {0u, 0u, 1u};
  
  Tree tree(parents);

  vec leaves {2u};
  EXPECT_EQ(tree.size(), 3u);
  EXPECT_EQ(tree.leaves(), leaves);
  EXPECT_FALSE(tree.is_leaf(0u));
  EXPECT_FALSE(tree.is_leaf(1u));
  EXPECT_TRUE(tree.is_leaf(2u));

  EXPECT_FALSE(tree.is_simple());
  
}

