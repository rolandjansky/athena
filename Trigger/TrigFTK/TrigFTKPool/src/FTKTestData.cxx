/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKPool/FTKTestData.h"

void FTKTestData::setValue(int ival)
{
  m_ival = ival;
}


int FTKTestData::getValue() const
{
  return m_ival;
}
