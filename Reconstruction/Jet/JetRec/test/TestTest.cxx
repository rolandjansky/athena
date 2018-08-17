/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"

class TestTest: public ::testing::Test {
public:
    virtual void SetUp() {
      
    }

};



TEST_F(TestTest, test) {
  
  EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
