/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEPULSEFORM_H
#define TILECONDITIONS_TILEPULSEFORM_H

// Tile includes
#include "TileConditions/TileCalibData.h"

class TilePulse {
  public:

    TilePulse(const TileCalibDataFlt* pulseShape) : m_pulseShape{pulseShape} {};
    virtual ~TilePulse() = default;

    bool getPulseShapeYDY(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                          float time, float &y, float &dy) const;

  private:

    //=== TileCalibData
    const TileCalibDataFlt* m_pulseShape;
};

// inlines
inline
bool TilePulse::getPulseShapeYDY(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                     float time, float &y, float &dy) const {
  return m_pulseShape->getCalibDrawer(drawerIdx)->getYDY(channel, adc, time, y, dy);
}

#endif
