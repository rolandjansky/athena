/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLNOISERAWCHN_H
#define TILECONDITIONS_TILECONDTOOLNOISERAWCHN_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Tile includes
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileConditions/ITileCondToolNoise.h"
#include "TileConditions/ITileCondProxy.h"

// Forward declaration
class TileCalibDrawerFlt;
class TileCondToolEmscale;

class TileCondToolNoiseRawChn: public AthAlgTool
                             , virtual public ITileCondToolNoise {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolNoiseRawChn(const std::string& type, const std::string& name,
        const IInterface* parent);
    virtual ~TileCondToolNoiseRawChn();

    StatusCode initialize();
    StatusCode finalize();

    float getElectronicNoise(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
        TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts) const;

    float getPileUpNoise(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    float getNoise(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
        TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts) const {
      return getElectronicNoise(drawerIdx, channel, adc, unit);
    }

  private:

    //=== used tools
    ToolHandle<TileCondToolEmscale> m_tileToolEms;

    //=== TileCondProxy
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryNoiseRawChn;
};

#endif
