/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/make_jetstream.h"
#include <vector>
#include <memory>

using vec = std::vector<std::size_t>;

TEST(make_jetstreamTester, SimpleSimpleSimpleChain) {

  std::vector<std::vector<std::size_t>> v;
  v.push_back(std::vector<std::size_t>{1,2});
  v.push_back(std::vector<std::size_t>{3,4, 5});
  v.push_back(std::vector<std::size_t>{6,7});

  std::vector<std::size_t> repeats{1,1,1};

  std::size_t sid{0};

  auto stream = make_jetstream(v, repeats, sid);

  JetStreamer streamer(std::move(stream));

  vec v0{1, 3, 6}; 
  vec v1{2, 3, 6}; 
  vec v2{1, 4, 6}; 
  vec v3{2, 4, 6}; 
  vec v4{1, 5, 6}; 
  vec v5{2, 5, 6}; 
  vec v6{1, 3, 7}; 
  vec v7{2, 3, 7}; 
  vec v8{1, 4, 7}; 
  vec v9{2, 4, 7}; 
  vec v10{1, 5, 7}; 
  vec v11{2, 5, 7}; 


  EXPECT_EQ (v0, streamer.next());
  EXPECT_EQ (v1, streamer.next());
  EXPECT_EQ (v2, streamer.next());
  EXPECT_EQ (v3, streamer.next());
  EXPECT_EQ (v4, streamer.next());
  EXPECT_EQ (v5, streamer.next());
  EXPECT_EQ (v6, streamer.next());
  EXPECT_EQ (v7, streamer.next());
  EXPECT_EQ (v8, streamer.next());
  EXPECT_EQ (v9, streamer.next());
  EXPECT_EQ (v10, streamer.next());
  EXPECT_EQ (v11, streamer.next());
  
  EXPECT_EQ (vec{}, streamer.next());
}


TEST(make_jetstreamTester, SimpleCompoundSimpleChain) {
 std::vector<std::vector<std::size_t>> v;
  v.push_back(std::vector<std::size_t>{1,2});
  v.push_back(std::vector<std::size_t>{3,4, 5});
  v.push_back(std::vector<std::size_t>{6,7});

  std::vector<std::size_t> repeats{1,2,1};

  std::size_t sid{0};

  auto stream = make_jetstream(v, repeats, sid);

  JetStreamer streamer(std::move(stream));

  vec v0{1, 3, 4, 6}; 
  vec v1{2, 3, 4, 6}; 
  vec v2{1, 3, 5, 6}; 
  vec v3{2, 3, 5, 6}; 
  vec v4{1, 4, 5, 6}; 
  vec v5{2, 4, 5, 6}; 
  vec v6{1, 3, 4, 7}; 
  vec v7{2, 3, 4, 7}; 
  vec v8{1, 3, 5, 7}; 
  vec v9{2, 3, 5, 7}; 
  vec v10{1, 4, 5, 7}; 
  vec v11{2, 4, 5, 7}; 

  EXPECT_EQ (v0, streamer.next());
  EXPECT_EQ (v1, streamer.next());
  EXPECT_EQ (v2, streamer.next());
  EXPECT_EQ (v3, streamer.next());
  EXPECT_EQ (v4, streamer.next());
  EXPECT_EQ (v5, streamer.next());
  EXPECT_EQ (v6, streamer.next());
  EXPECT_EQ (v7, streamer.next());
  EXPECT_EQ (v8, streamer.next());
  EXPECT_EQ (v9, streamer.next());
  EXPECT_EQ (v10, streamer.next());
  EXPECT_EQ (v11, streamer.next());
  
  EXPECT_EQ (vec{}, streamer.next());
}

