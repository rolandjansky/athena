/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/src/TileRawChannelBuilderTest.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Helper for testing TileRawChannelBuilder.
 */


#include "TileRawChannelBuilderTest.h"
#include "AthAllocators/DataPool.h"


/**
 * Builder virtual method to be implemented by subclasses
 * @param digits Pointer to TileDigitsContainer
 *
 */
TileRawChannel*
TileRawChannelBuilderTest::rawChannel (const TileDigits* digits)
{
  const std::vector<float>& samples = digits->samples();
  float amp  = samples.size() > 0 ? samples[0] : 0;
  float time = samples.size() > 1 ? samples[1] : 0;
  float qual = samples.size() > 2 ? samples[2] : 0;
  float ped  = samples.size() > 3 ? samples[3] : 0;

  DataPool<TileRawChannel> tileRchPool (100);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  rawCh->assign (digits->adc_HWID(), amp, time, qual, ped);
  return rawCh;
}

