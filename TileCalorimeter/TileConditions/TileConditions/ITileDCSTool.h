//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILEDCSTOOL_H
#define TILECONDITIONS_ITILEDCSTOOL_H

// Tile includes
#include "TileConditions/TileDCSState.h"

// Gaudi includes
#include "GaudiKernel/IAlgTool.h"

/**
 * @class ITileDCSTool
 * @brief An abstract interface to get Tile DCS information from DB
 */

class ITileDCSTool: virtual public IAlgTool {

  public:

    virtual ~ITileDCSTool() {};

   /**
    * @copydoc TileDCSState::getChannelHV(unsigned int ros,  unsigned int drawer,  unsigned int channel) const
    */
    virtual float getChannelHV(unsigned int ros,  unsigned int drawer,  unsigned int channel) const = 0;

   /**
    * @copydoc TileDCSState::getChannelHVSet(unsigned int ros,  unsigned int drawer,  unsigned int channel) const
    */
    virtual float getChannelHVSet(unsigned int ros,  unsigned int drawer,  unsigned int channel) const = 0;

   /**
    * @copydoc TileDCSState::getDrawerStates(unsigned int ros,  unsigned int drawer) const
    */
    virtual int getDrawerStates(unsigned int ros,  unsigned int drawer) const = 0;

   /**
    * @copydoc TileDCSState::getDCSHVStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const
    */
    virtual TileDCSState::TileDCSStatus getDCSHVStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const = 0;

   /**
    * @copydoc TileDCSState::getDCSStatus(unsigned int ros, unsigned int drawer) const
    */
    virtual TileDCSState::TileDCSStatus getDCSStatus(unsigned int ros, unsigned int drawer) const = 0;

   /**
    * @copydoc TileDCSState::getDCSStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const
    */
    virtual TileDCSState::TileDCSStatus getDCSStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const = 0;

   /**
    * @copydoc TileDCSState::isStatusHVBad(unsigned int ros, unsigned int drawer, unsigned int channel) const
    */
    virtual bool isStatusHVBad(unsigned int ros, unsigned int drawer, unsigned int channel) const = 0;

   /**
    * @copydoc TileDCSState::isStatusBad (unsigned int ros, unsigned int drawer) const
    */
    virtual bool isStatusBad (unsigned int ros, unsigned int drawer) const = 0;

   /**
    * @copydoc TileDCSState::isStatusBad(unsigned int ros, unsigned int drawer, unsigned int channel) const
    */
    virtual bool isStatusBad(unsigned int ros, unsigned int drawer, unsigned int channel) const = 0;

  DeclareInterfaceID(ITileDCSTool, 1, 0);

};



#endif // TILECONDITIONS_ITILEDCSTOOL_H
