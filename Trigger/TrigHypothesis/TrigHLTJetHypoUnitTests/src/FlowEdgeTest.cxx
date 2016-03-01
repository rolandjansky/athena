/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowEdge.h"


TEST(FlowEdgeTest, DefaultConstructor) { 
  FlowEdge fe;
    EXPECT_EQ (0, fe.from ());
}
