// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/ITileDQstatusTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief Interface for building TileDQstatus object.
 */


#ifndef TILERECUTILS_ITILEDQSTATUSTOOL_H
#define TILERECUTILS_ITILEDQSTATUSTOOL_H


#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"


class TileRawChannelContainer;
class TileDigitsContainer;
class TileBeamElemContainer;
class TileDQstatus;
class EventContext;


/***
 * @brief Interface for building TileDQstatus object.
 */
class ITileDQstatusTool
  : virtual public IAlgTool
{
public:
  DeclareInterfaceID (ITileDQstatusTool, 1, 0);


  /**
   * @brief Make a new TileDQstatus object.
   * @param ctx Event context.
   * @param rawChannelContainer The TileRawChannelContainer (may be null).
   * @param tileDigitsContainer The TileDigitsContainer (may be null).
   * @param tileBeamElemContainer The TileBeamElemContainer (may be null).
   * @param[out] dqstatus TileDQstatus object to fill.
   */
  virtual
  StatusCode
  makeStatus (const EventContext& ctx,
              const TileRawChannelContainer* rawChannelContainer,
              const TileDigitsContainer* tileDigitsContainer,
              const TileBeamElemContainer* tileBeamElemContainer,
              TileDQstatus& dqstatus) const = 0;
};


#endif // not TILERECUTILS_ITILEDQSTATUSTOOL_H
