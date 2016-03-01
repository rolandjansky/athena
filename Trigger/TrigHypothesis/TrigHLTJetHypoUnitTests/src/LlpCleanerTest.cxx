/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/LlpCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/UncleanableJet.h"  // exception class
#include "./MockJet.h"
#include "gtest/gtest.h"
// #include <TLorentzVector.h>
#include "gmock/gmock.h"
#include <array>

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;

LlpCleaner makeLlpCleaner(float fSampMaxLlpThreshold,
			  float negELlpThreshold,
			  float hecfLlpThreshold,
			  float hecqLlpThreshold,
			  float avLarQFLlpThreshold){
  
  return LlpCleaner(fSampMaxLlpThreshold,
		    negELlpThreshold,
		    hecfLlpThreshold,
		    hecqLlpThreshold,
		    avLarQFLlpThreshold);
}


TEST(LlpCleanerTest, SimpleThresholds) {

  MockJet jet;
  
  float FracSamplingMax{1};
  float NegativeE{1};
  float HECFrac{1};
  float HECQuality{1};
  float AverageLArQF{1};

  EXPECT_CALL(jet, getAttribute("FracSamplingMax", _))
    .WillRepeatedly(DoAll(SetArgReferee<1>(FracSamplingMax), Return(true)));

  EXPECT_CALL(jet,
	      getAttribute("NegativeE", _)
	      ).WillRepeatedly(DoAll(SetArgReferee<1>(NegativeE),
				     Return(true)));
  EXPECT_CALL(jet,
	      getAttribute("HECFrac", _)
	      ).WillRepeatedly(DoAll(SetArgReferee<1>(HECFrac),
				     Return(true)));

  EXPECT_CALL(jet,
	      getAttribute("HECQuality", _)
	      ).WillRepeatedly(DoAll(SetArgReferee<1>(HECQuality),
				     Return(true)));

  EXPECT_CALL(jet,
	      getAttribute("AverageLArQF", _)
	      ).WillRepeatedly(DoAll(SetArgReferee<1>(AverageLArQF),
				     Return(true)));

  
  // jet on accept side of all boundaries
  {
    std::array<float, 5> args{1., 1., 1., 1., 1.};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  constexpr float eps = 0.00001;


  // jet above  fSampMaxTightThreshold, below NegativeE
  {
    std::array<float, 5> args{1.-eps, 1., 1,  1., 1.};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  // jet below  fSampMaxTightThreshold, above NegativeE
  {
    std::array<float, 5> args{1., 1.-eps, 1,  1., 1.};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  // jet above  fSampMaxTightThreshold, above NegativeE
  {
    std::array<float, 5> args{1-eps, 1.-eps, 1,  1., 1.};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_FALSE(cleaner(&jet));
  }

  // jet above HECFrac below HECQuality below AverageLArQF
  {
    std::array<float, 5> args{1., 1., 1-eps,  1., 1.};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  // jet below HECFrac above HECQuality below AverageLArQF
  {
    std::array<float, 5> args{1., 1., 1,  1.-eps, 1.};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  // jet below HECFrac below HECQuality above AverageLArQF
  {
    std::array<float, 5> args{1., 1., 1,  1., 1.-eps};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  // jet above HECFrac above HECQuality below AverageLArQF
  {
    std::array<float, 5> args{1., 1., 1.-eps,  1.-eps, 1.};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  // jet above HECFrac below HECQuality above AverageLArQF
  {
    std::array<float, 5> args{1., 1., 1.-eps,  1., 1.-eps};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  // jet below HECFrac above HECQuality above AverageLArQF
  {
    std::array<float, 5> args{1., 1., 1.,  1.-eps, 1.-eps};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_TRUE(cleaner(&jet));
  }

  // jet above HECFrac above HECQuality above AverageLArQF
  {
    std::array<float, 5> args{1., 1., 1.-eps,  1.-eps, 1.-eps};
    auto cleaner = makeLlpCleaner(args[0], args[1], args[2], args[3], args[4]);
    EXPECT_FALSE(cleaner(&jet));
  }

}

TEST(LlpCleanerTest, ThrowsOnUncleanableJet) {
  MockJet jet;

  LlpCleaner cleaner(1., 1., 1., 1., 1.);

  EXPECT_CALL(jet, getAttribute("FracSamplingMax", _))
    .WillOnce(DoAll(SetArgReferee<1>(0.9), Return(false)));

  

  EXPECT_THROW(cleaner(&jet), UncleanableJet);
}
