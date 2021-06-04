/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILESAMPLENOISE_H
#define TILECONDITIONS_TILESAMPLENOISE_H

// Tile includes
#include "TileConditions/TileCalibData.h"

class TileSampleNoise {

  public:

    TileSampleNoise(const TileCalibDataFlt* sampleNosie) : m_sampleNoise{sampleNosie} {};

    virtual ~TileSampleNoise() = default;

    float getPed(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    float getHfn(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    float getLfn(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    float getHfn1(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    float getHfn2(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    float getHfnNorm(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

  private:

    const TileCalibDataFlt* m_sampleNoise;
};


// inlines
inline
float TileSampleNoise::getPed(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  return m_sampleNoise->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
}

inline
float TileSampleNoise::getHfn(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  return m_sampleNoise->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);
}

inline
float TileSampleNoise::getLfn(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  const TileCalibDrawerFlt* calibDrawer = m_sampleNoise->getCalibDrawer(drawerIdx);
  //=== check if Lfn is stored already if not - return zero
  return (calibDrawer->getObjSizeUint32() < 3) ? 0. : calibDrawer->getData(channel, adc, 2);
}

inline
float TileSampleNoise::getHfn1(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  const TileCalibDrawerFlt* calibDrawer = m_sampleNoise->getCalibDrawer(drawerIdx);
  //=== check if Hfn1 is stored already, if not - return old Hfn
  return (calibDrawer->getObjSizeUint32() < 4) ? calibDrawer->getData(channel, adc, 1)
                                               : calibDrawer->getData(channel, adc, 3);
}

inline
float TileSampleNoise::getHfn2(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  const TileCalibDrawerFlt* calibDrawer = m_sampleNoise->getCalibDrawer(drawerIdx);
  //=== check if Hfn2 is stored already, if not - return zero
  return (calibDrawer->getObjSizeUint32() < 5) ? 0.0 : calibDrawer->getData(channel, adc, 4);
}

inline
float TileSampleNoise::getHfnNorm(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  const TileCalibDrawerFlt* calibDrawer = m_sampleNoise->getCalibDrawer(drawerIdx);
  //=== check if HfnNorm is stored already, if not - return zero
  return (calibDrawer->getObjSizeUint32() < 6) ? 0.0 : calibDrawer->getData(channel, adc, 5);
}


#endif
