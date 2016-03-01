/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"


TEST(FlowNetworkTest, SimpleTest) { 
  FlowNetwork fn(0);
    EXPECT_EQ (0, fn.V());
}

