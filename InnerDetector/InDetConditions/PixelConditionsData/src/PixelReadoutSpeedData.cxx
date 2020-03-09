/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelReadoutSpeedData.h"

PixelReadoutSpeedData::PixelReadoutSpeedData():
  m_rodReadoutMap()
{
}

PixelReadoutSpeedData::~PixelReadoutSpeedData() { }

void PixelReadoutSpeedData::clear() {
  m_rodReadoutMap.clear();
}

