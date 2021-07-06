/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*
 */

#ifndef TILERECUTILS_ITILERAWCHANNELTOOL_H
#define TILERECUTILS_ITILERAWCHANNELTOOL_H


#include "TileEvent/TileMutableRawChannelContainer.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IAlgTool.h"


class ITileRawChannelTool: virtual public IAlgTool
{
public:
  DeclareInterfaceID (ITileRawChannelTool, 1, 0);

  // Update a TileMutableRawChannelContainer,
  // subtracting common mode noise for example.
  virtual StatusCode process (TileMutableRawChannelContainer& rchCont, const EventContext &ctx) const = 0;
};

#endif 
