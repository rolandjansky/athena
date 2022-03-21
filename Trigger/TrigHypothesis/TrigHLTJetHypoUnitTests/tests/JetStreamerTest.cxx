/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/JetStreamer.h"
#include "TrigHLTJetHypo/../src/IJetStream.h"
#include <vector>
#include <memory>

using vec = std::vector<std::size_t>;

std::unique_ptr<JetStreamer> make_streamer(int test){
  
  auto null_stream  = std::unique_ptr<IJetStream>{nullptr};

  auto streamer = std::unique_ptr<JetStreamer> (nullptr);

  if (test == 0) {
    std::vector<std::size_t> jets0 {1, 2};

    auto stream0 = std::make_unique<SimpleJetStream>(jets0,
						     std::move(null_stream),
						     0
						     );
    streamer = std::make_unique<JetStreamer>(std::move(stream0));
    
    return streamer;
  } else if (test == 1) {

    std::vector<std::size_t> jets_cs {5, 6, 7};
    std::size_t k2{2};

    auto stream0 =
      std::make_unique<CombinationsJetStream>(jets_cs,
					      std::move(null_stream),
					      k2,
					      0
					      );
    streamer.reset(new JetStreamer(std::move(stream0)));
    return streamer;
  } else if (test == 2) {

    std::vector<std::size_t> jets0 {1, 2};
    std::vector<std::size_t> jets1 {3, 4};
    auto stream1 =
      std::make_unique<SimpleJetStream>(jets1,
					std::move(null_stream),
					1
					);

          
    auto stream0 =
      std::make_unique<SimpleJetStream>(jets0,
					std::move(stream1),
					0
					);
    
    streamer.reset(new JetStreamer(std::move(stream0)));
    return streamer;
  } else if (test == 3) {
    
    std::vector<std::size_t> jets_cs {5, 6, 7};
    std::size_t k2{2};
    std::vector<std::size_t> jets0 {1, 2};


      
    auto stream1 =
      std::make_unique<CombinationsJetStream>(jets_cs,
					      std::move(null_stream),
					      k2,
					      1
					      );

          
    auto stream0 =
      std::make_unique<SimpleJetStream>(jets0,
					std::move(stream1),
					0
					);
    
    streamer.reset(new JetStreamer(std::move(stream0)));
    return streamer;
    } else if (test == 4) {
        
    std::vector<std::size_t> jets_cs {5, 6, 7};
    std::size_t k2{2};
    std::vector<std::size_t> jets0 {1, 2};

    
    auto stream1 =
      std::make_unique<SimpleJetStream>(jets0,
					std::move(null_stream),
					1
					);
    
    
    auto stream0 =
      std::make_unique<CombinationsJetStream>(jets_cs,
					      std::move(stream1),
					      k2,
					      0
					      );
    
    streamer.reset(new JetStreamer(std::move(stream0)));
    return streamer;
  } else if (test == 5) {

            
    std::vector<std::size_t> jets_cs {5, 6, 7};
    std::size_t k2{2};
    std::vector<std::size_t> jets0 {1, 2};
    std::vector<std::size_t> jets1 {3, 4};

    auto stream2 =
      std::make_unique<SimpleJetStream>(jets1,
					std::move(null_stream),
					2
					);
      
    auto stream1 =
      std::make_unique<CombinationsJetStream>(jets_cs,
					      std::move(stream2),
					      k2,
					      1
					      );

          
    auto stream0 =
      std::make_unique<SimpleJetStream>(jets0,
					std::move(stream1),
					0
					);
    
    streamer.reset(new JetStreamer(std::move(stream0)));
  
  return streamer;
   } else if (test == 6) {

            
    std::vector<std::size_t> jets_cs {};
    std::size_t k2{2};
    std::vector<std::size_t> jets0 {1, 2};
    std::vector<std::size_t> jets1 {3, 4};

    auto stream2 =
      std::make_unique<SimpleJetStream>(jets1,
					std::move(null_stream),
					2
					);
      
    auto stream1 =
      std::make_unique<CombinationsJetStream>(jets_cs,
					      std::move(stream2),
					      k2,
					      1
					      );

          
    auto stream0 =
      std::make_unique<SimpleJetStream>(jets0,
					std::move(stream1),
					0
					);
    
    streamer.reset(new JetStreamer(std::move(stream0)));
  
    return streamer;
 } else if (test == 7) {

            
    std::vector<std::size_t> jets_cs {5, 6, 7};
    std::size_t k4{4};
    std::vector<std::size_t> jets0 {1, 2};
    std::vector<std::size_t> jets1 {3, 4};

    auto stream2 =
      std::make_unique<SimpleJetStream>(jets1,
					std::move(null_stream),
					2
					);
      
    auto stream1 =
      std::make_unique<CombinationsJetStream>(jets_cs,
					      std::move(stream2),
					      k4,
					      1
					      );

          
    auto stream0 =
      std::make_unique<SimpleJetStream>(jets0,
					std::move(stream1),
					0
					);
    
    streamer.reset(new JetStreamer(std::move(stream0)));
  
    return streamer;
    } else if (test == 8) {

    auto stream0 = std::unique_ptr<IJetStream>(nullptr);
    streamer.reset(new JetStreamer(std::move(stream0)));
    return streamer;
  } else {
  std::cerr << "unknown test << " << test << '\n';
  return streamer;
  }
}

TEST(JetStreamerTester, oneSimpleJetStream) {
  // Cycle around the input vector
  auto streamer = make_streamer(0);
  EXPECT_EQ (vec{1}, streamer->next());
  EXPECT_EQ (vec{2}, streamer->next());
  EXPECT_EQ (vec{}, streamer->next());
}


TEST(JetStreamerTester, oneCombinationsJetStream) {
  // Cycle around the input vector
  auto streamer = make_streamer(1);
  vec v0{5, 6};
  vec v1{5, 7};
  vec v2{6, 7};
  EXPECT_EQ (v0, streamer->next());
  EXPECT_EQ (v1, streamer->next());
  EXPECT_EQ (v2, streamer->next());
  EXPECT_EQ (vec{}, streamer->next());
}


TEST(JetStreamerTester, twoSimpleJetStreams) {
  // Cycle around the input vector
  auto streamer = make_streamer(2);
  vec v0{3, 1};
  vec v1{4, 1};
  vec v2{3, 2};
  vec v3{4, 2};

  EXPECT_EQ (v0, streamer->next());
  EXPECT_EQ (v1, streamer->next());
  EXPECT_EQ (v2, streamer->next());
  EXPECT_EQ (v3, streamer->next());
  EXPECT_EQ (vec{}, streamer->next());
}

TEST(JetStreamerTester, SimpleThenCombinationsJetStreams) {
  // Cycle around the input vector
  auto streamer = make_streamer(3);
  vec v0{5, 6, 1};
  vec v1{5, 7, 1};
  vec v2{6, 7, 1};
  vec v3{5, 6, 2};
  vec v4{5, 7, 2};
  vec v5{6, 7, 2};

  EXPECT_EQ (v0, streamer->next());
  EXPECT_EQ (v1, streamer->next());
  EXPECT_EQ (v2, streamer->next());
  EXPECT_EQ (v3, streamer->next());
  EXPECT_EQ (v4, streamer->next());
  EXPECT_EQ (v5, streamer->next());
  EXPECT_EQ (vec{}, streamer->next());
}

TEST(JetStreamerTester, CombinationsThenSimpleJetStreams) {
  // Cycle around the input vector
  auto streamer = make_streamer(4);
  vec v0{1, 5, 6};
  vec v1{2, 5, 6};
  vec v2{1, 5, 7};
  vec v3{2, 5, 7};
  vec v4{1, 6, 7};
  vec v5{2, 6, 7};

  EXPECT_EQ (v0, streamer->next());
  EXPECT_EQ (v1, streamer->next());
  EXPECT_EQ (v2, streamer->next());
  EXPECT_EQ (v3, streamer->next());
  EXPECT_EQ (v4, streamer->next());
  EXPECT_EQ (v5, streamer->next());
  EXPECT_EQ (vec{}, streamer->next());
}

TEST(JetStreamerTester, SimmpleThenCombinationsThenSimpleJetStreams) {
  // Cycle around the input vector
  auto streamer = make_streamer(5);
  vec v0{3, 5, 6, 1};
  vec v1{4, 5, 6, 1};
  vec v2{3, 5, 7, 1};
  vec v3{4, 5, 7, 1};
  vec v4{3, 6, 7, 1};
  vec v5{4, 6, 7, 1};
  vec v6{3, 5, 6, 2};
  vec v7{4, 5, 6, 2};
  vec v8{3, 5, 7, 2};
  vec v9{4, 5, 7, 2};
  vec v10{3, 6, 7, 2};
  vec v11{4, 6, 7, 2};

  EXPECT_EQ (v0, streamer->next());
  EXPECT_EQ (v1, streamer->next());
  EXPECT_EQ (v2, streamer->next());
  EXPECT_EQ (v3, streamer->next());
  EXPECT_EQ (v4, streamer->next());
  EXPECT_EQ (v5, streamer->next());
  EXPECT_EQ (v6, streamer->next());
  EXPECT_EQ (v7, streamer->next());
  EXPECT_EQ (v8, streamer->next());
  EXPECT_EQ (v9, streamer->next());
  EXPECT_EQ (v10, streamer->next());
  EXPECT_EQ (v11, streamer->next());
  EXPECT_EQ (vec{}, streamer->next());
}

TEST(JetStreamerTester, InvalidJetIndices) {
  // Cycle around the input vector
  auto streamer = make_streamer(6);
  EXPECT_EQ (vec{}, streamer->next());
}


TEST(JetStreamerTester, InvalidNchooseK) {
  // Cycle around the input vector
  auto streamer = make_streamer(7);
  EXPECT_EQ (vec{}, streamer->next());
}

TEST(JetStreamerTester, InvalidJetStream) {
  // Cycle around the input vector
  auto streamer = make_streamer(8);
  EXPECT_EQ (vec{}, streamer->next());
}
