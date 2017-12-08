/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetSimEvent/TRTHit.h"


TRTHit::TRTHit(unsigned first, unsigned second, unsigned third, unsigned fourth,
  unsigned fifth): firstWord(first), secondWord(second), thirdWord(third),
  fourthWord(fourth), fifthWord(fifth) {}
