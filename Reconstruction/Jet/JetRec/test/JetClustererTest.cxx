/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODBase/IParticleContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "JetRec/JetClusterer.h"

#include "gtest/gtest.h"

//#define SC_CHECK( expr ) ASSERT_TRUE( (expr).isSuccess() )

TEST(JetClustererTest, setPropertyTest) {
  JetClusterer clusterer("Test");
  StatusCode sc= clusterer.setProperty("JetAlgorithm" , "AntiKt");
  ASSERT_TRUE( sc ) << "failed to set property JetAlgorithm" ; 
  ASSERT_TRUE(clusterer.setProperty("JetRadius" , 0.4 ) ) << "failed to set property JetRadius" ; 
  ASSERT_TRUE(clusterer.setProperty("PtMin" , 8000 ) ) << "failed to set property PtMin" ; 

  ASSERT_TRUE(clusterer.initialize());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  
  return RUN_ALL_TESTS();
}
