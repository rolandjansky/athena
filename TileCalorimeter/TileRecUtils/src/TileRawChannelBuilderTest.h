// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/src/TileRawChannelBuilderTest.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Helper for testing TileRawChannelBuilder.
 */


#ifndef TILERECUTILS_TILERAWCHANNELBUILDERTEST_H
#define TILERECUTILS_TILERAWCHANNELBUILDERTEST_H


#include "TileRecUtils/TileRawChannelBuilder.h"


class TileRawChannelBuilderTest
  : public TileRawChannelBuilder
{
public:
  using TileRawChannelBuilder::TileRawChannelBuilder;


  /**
   * Builder virtual method to be implemented by subclasses
   * @param digits Pointer to TileDigitsContainer
   *
   */
  virtual TileRawChannel* rawChannel (const TileDigits* digits) override;
};


#endif // not TILERECUTILS_TILERAWCHANNELBUILDERTEST_H
