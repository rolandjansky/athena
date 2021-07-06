/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/../src/JetTrigTimer.h"
#include "gtest/gtest.h"
#include <math.h>
#include <tuple>


double delay(std::size_t d){
  auto sumNum{0.};
  for (std::size_t i = 0; i != d; ++i) {
    sumNum  += i* atan(i);
  }
  return sumNum;
}

TEST(JetTrigTimerTest, exerciser) {

  auto timer = JetTrigTimer(false);
  EXPECT_EQ(timer.units(), "us");
  
  timer = JetTrigTimer(true);
  EXPECT_EQ(timer.units(), "ns");

  std::size_t delay_par(10000);

  auto tup = timer.read_bare();
  EXPECT_EQ(std::get<0>(tup), 0.);  // elapsed time = 0
  EXPECT_EQ(std::get<1>(tup), 0);  // no of calls = 0
  EXPECT_EQ(std::get<2>(tup), "ns");  // units are ns.

  timer.start();
  delay(delay_par);
  timer.update();

  tup = timer.read_bare();
  EXPECT_GE(std::get<0>(tup), 0.0);  // elapsed time
  EXPECT_EQ(std::get<1>(tup), 1);  // no of calls
  EXPECT_EQ(std::get<2>(tup), "ns");  // units are ns.

  timer.reset();
  timer.start();
  double e0 = timer.elapsed_to_now();
  delay(10);
  double e1 = timer.elapsed_to_now();
  delay(10);
  double e2 = timer.elapsed_to_update();
  

  EXPECT_GT(e1, e0);
  EXPECT_EQ(e2, e1);

  timer.reset();
  tup = timer.read_bare();
  EXPECT_EQ(std::get<0>(tup), 0.0);  // elapsed time = 0
  EXPECT_EQ(std::get<1>(tup), 0);  // no of calls = 0
  EXPECT_EQ(std::get<2>(tup), "ns");  // units are ns.
}
