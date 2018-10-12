/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_ITILERAWCHANNELTOOL_H
#define TILERECUTILS_ITILERAWCHANNELTOOL_H


class TileRawChannelContainer ;

// Includes for Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IAlgTool.h"


class ITileRawChannelTool: virtual public IAlgTool
{
public:
  DeclareInterfaceID (ITileRawChannelTool, 1, 0);

  // update TileRawChannelContainer, subtract common mode noise for example
  virtual StatusCode process(TileRawChannelContainer * rchCnt) const = 0;
};

#endif 
