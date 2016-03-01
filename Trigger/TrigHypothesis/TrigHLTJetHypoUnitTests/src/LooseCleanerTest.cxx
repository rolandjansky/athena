/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/LooseCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/UncleanableJet.h"  // exception class
#include "./MockJet.h"
#include "gtest/gtest.h"
// #include <TLorentzVector.h>
#include "gmock/gmock.h"
#include <array>

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;

LooseCleaner makeLooseCleaner(float fSampMaxTightThreshold,
			      float etaTightThreshold,
			      float emfLowTightThreshold,
			      float emfHighTightThreshold,
			      float hecfTightThreshold){

  return LooseCleaner(fSampMaxTightThreshold,
		      etaTightThreshold,
		      emfLowTightThreshold,
		      emfHighTightThreshold,
		      hecfTightThreshold);
}


TEST(LooseCleanerTest, SimpleThresholds) {

  MockJet jet;
  
  float FracSamplingMax{1};
  float EMFrac{1};
  float HECFrac{1};

  EXPECT_CALL(jet, getAttribute("FracSamplingMax", _))
    .WillRepeatedly(DoAll(SetArgReferee<1>(FracSamplingMax), Return(true)));

  EXPECT_CALL(jet,
	      getAttribute("EMFrac", _)
	      ).WillRepeatedly(DoAll(SetArgReferee<1>(EMFrac),
				     Return(true)));
  EXPECT_CALL(jet,
	      getAttribute("HECFrac", _)
	      ).WillRepeatedly(DoAll(SetArgReferee<1>(HECFrac),
				     Return(true)));

  EXPECT_CALL(jet, eta()).WillRepeatedly(Return(0.5));


  
  // jet on accept side of all boundaries
  {
    std::array<float, 5> args{1., 1., 1., 2., 1.};
    auto cleaner = makeLooseCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  constexpr float eps = 0.00001;


  // jet above  fSampMaxTightThreshold, inside applicable eta range
  {
    std::array<float, 5> args{1.-eps, 1+ eps, 1,  2., 1.};
    auto cleaner = makeLooseCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_FALSE(cleaner(&jet));
  }

  // jet above  fSampMaxTightThreshold, outside applicable eta range
  {
    std::array<float, 5> args{1.-eps, 0.5- eps, 1,  2., 1.};
    auto cleaner = makeLooseCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  // jet below _emfLowTightThreshold
  {
    std::array<float, 5> args{1., 1., 1+eps,  2., 1.};
    auto cleaner = makeLooseCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_FALSE(cleaner(&jet));
  }


  // jet above _emfHighTightThreshold
  {
    std::array<float, 5> args{1., 1., 0.5,  1.-eps, 1.};
    auto cleaner = makeLooseCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_FALSE(cleaner(&jet));
  }


  // jet above _emfLowTightThreshold
  {
    std::array<float, 5> args{1., 1., 1.0,  1.0, 1. -eps};
    auto cleaner = makeLooseCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_FALSE(cleaner(&jet));
  }

}

TEST(LooseCleanerTest, ThrowsOnUncleanableJet) {
  MockJet jet;

  LooseCleaner cleaner(1., 1., 1., 2., 1.);

  EXPECT_CALL(jet, getAttribute("FracSamplingMax", _))
    .WillOnce(DoAll(SetArgReferee<1>(0.9), Return(false)));

  

  EXPECT_THROW(cleaner(&jet), UncleanableJet);
}
