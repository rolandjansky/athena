/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetSimEvent/TRTHit.h"


TRTHit::TRTHit(unsigned first, unsigned second, unsigned third, unsigned fourth,
  unsigned fifth): m_firstWord(first), m_secondWord(second), m_thirdWord(third),
  m_fourthWord(fourth), m_fifthWord(fifth) {}
