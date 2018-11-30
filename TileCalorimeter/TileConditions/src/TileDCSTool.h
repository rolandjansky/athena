//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEDCSTOOL_H
#define TILECONDITIONS_TILEDCSTOOL_H

// Tile includes
#include "TileConditions/ITileDCSTool.h"
#include "TileConditions/TileDCSState.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"

/**
 * @class TileDCSTool
 * @brief The tool to get Tile DCS information from DB
 */

class TileDCSTool: public extends<AthAlgTool, ITileDCSTool> {

  public:

    TileDCSTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileDCSTool();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
   /**
    * @copydoc ITileDCSTool::getChannelHV(unsigned int ros,  unsigned int drawer,  unsigned int channel) const
    */
    virtual float getChannelHV(unsigned int ros,  unsigned int drawer,  unsigned int channel) const override;

   /**
    * @copydoc ITileDCSTool::getChannelHVSet(unsigned int ros,  unsigned int drawer,  unsigned int channel) const
    */
    virtual float getChannelHVSet(unsigned int ros,  unsigned int drawer,  unsigned int channel) const override;

   /**
    * @copydoc ITileDCSTool::getDrawerStates(unsigned int ros,  unsigned int drawer) const
    */
    virtual int getDrawerStates(unsigned int ros,  unsigned int drawer) const override;

   /**
    * @copydoc ITileDCSTool::getDCSHVStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const
    */
    virtual TileDCSState::TileDCSStatus getDCSHVStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const override;

   /**
    * @copydoc ITileDCSTool::getDCSStatus(unsigned int ros, unsigned int drawer) const
    */
    virtual TileDCSState::TileDCSStatus getDCSStatus(unsigned int ros, unsigned int drawer) const override;

   /**
    * @copydoc ITileDCSTool::getDCSStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const
    */
    virtual TileDCSState::TileDCSStatus getDCSStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const override;

   /**
    * @copydoc ITileDCSTool::isStatusHVBad(unsigned int ros, unsigned int drawer, unsigned int channel) const
    */
    virtual bool isStatusHVBad(unsigned int ros, unsigned int drawer, unsigned int channel) const override;

   /**
    * @copydoc ITileDCSTool::isStatusBad (unsigned int ros, unsigned int drawer) const
    */
    virtual bool isStatusBad (unsigned int ros, unsigned int drawer) const override;

   /**
    * @copydoc ITileDCSTool::isStatusBad(unsigned int ros, unsigned int drawer, unsigned int channel) const
    */
    virtual bool isStatusBad(unsigned int ros, unsigned int drawer, unsigned int channel) const override;

  private:

   /**
    * @brief Name of TileDCSState object in condition store
    */
    SG::ReadCondHandleKey<TileDCSState> m_dcsStateKey{this,
        "TileDCS", "TileDCS", "Input Tile DCS status"};

};



#endif // TILECONDITIONS_TILEDCSTOOL_H
