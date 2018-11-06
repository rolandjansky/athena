/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLTIMING_H
#define TILECONDITIONS_TILECONDTOOLTIMING_H

// Tile includes
#include "TileConditions/TileCalibData.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"


#include <vector>

class TileCondToolTiming : public AthAlgTool {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolTiming(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolTiming();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;


    float getSignalPhase(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;
    float getChannelOffset(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

  private:

    SG::ReadCondHandleKey<TileCalibDataFlt> m_calibTimingKey{this,
        "TileTiming", "TileTiming", "Input Tile timing calibration constants"};

};


//
//____________________________________________________________________
inline float TileCondToolTiming::getSignalPhase(unsigned int drawerIdx,
                                                unsigned int channel,
                                                unsigned int adc) const {
  return getChannelOffset(drawerIdx, channel, adc);
}

#endif
