/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

// Google Test
#include <gtest/gtest.h>

// The class to test
#include "PileUpHashHelper.h"

namespace PileUpTesting
{

class PileUpHashHelper_test : public ::testing::Test {};

TEST_F(PileUpHashHelper_test, empty_mixture) {
  unsigned long long reference = 0;
  xAOD::EventInfo::PileUpMixtureID test{};

  ASSERT_EQ(reference, test.lowBits);
  ASSERT_EQ(reference, test.highBits);
}

TEST_F(PileUpHashHelper_test, uuid_to_long) {
  uuid_t source{'a','a','a','a','a','a','a','b','1','0','0','0','0','0','0','0'};

  xAOD::EventInfo::PileUpMixtureID reference;
  reference.lowBits = 7089054359331365217;
  reference.highBits = 3472328296227680305;

  xAOD::EventInfo::PileUpMixtureID test = PileUpHashHelper::uuidToPileUpMixtureId(source);

  ASSERT_EQ(reference, test);
  ASSERT_EQ(reference.lowBits, test.lowBits);
  ASSERT_EQ(reference.highBits, test.highBits);
}

TEST_F(PileUpHashHelper_test, long_to_uuid) {
  uuid_t reference{'a','a','a','a','a','a','a','b','1','0','0','0','0','0','0','0'};
  uuid_t test{};

  xAOD::EventInfo::PileUpMixtureID source;
  source.lowBits = 7089054359331365217;
  source.highBits = 3472328296227680305;

  PileUpHashHelper::pileUpMixtureIdToUuid(source, test);

  ASSERT_EQ(uuid_compare(reference, test), 0);
}

} // namespace PileUpTesting

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
