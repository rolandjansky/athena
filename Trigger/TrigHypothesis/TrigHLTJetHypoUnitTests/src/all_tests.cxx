/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "gmock/gmock.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleMock( &argc, argv );
  return RUN_ALL_TESTS();
}
