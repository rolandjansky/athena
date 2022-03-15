/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILESAMPLINGFRACTION_H
#define TILECONDITIONS_TILESAMPLINGFRACTION_H

// Tile includes
#include "TileConditions/TileCalibData.h"

/**
 * @class TileSamplingFraction
 * @brief Condition object to keep and provide Tile Calorimeter sampling fraction and number of photoelectrons
 */

class TileSamplingFraction {
  public:


    TileSamplingFraction(std::unique_ptr<TileCalibDataFlt> samplingFraction)
      : m_samplingFraction{std::move(samplingFraction)} {};

    virtual ~TileSamplingFraction() = default;

   /**
    * @brief Return Tile Calorimeter sampling fraction
    * @returns Tile Calorimeter sampling fraction
    * @param drawerIdx Tile drawer index in range [0..275]
    * @param channel Tile channel number
    */
    float getSamplingFraction(unsigned int drawerIdx, unsigned int channel) const;

   /**
    * @brief Return number of photoelectrons per 1 GeV in Tile Calorimeter scintilator
    * @returns number of photoelectrons
    * @param drawerIdx Tile drawer index in range [0..275]
    * @param channel Tile channel number
    */
    float getNumberOfPhotoElectrons(unsigned int drawerIdx, unsigned int channel) const;

    TileSamplingFraction(const TileSamplingFraction&) = delete;
    TileSamplingFraction& operator= (const TileSamplingFraction&) = delete;

  private:

    //=== TileCalibData
    std::unique_ptr<TileCalibDataFlt> m_samplingFraction;

};

// inlines
inline
float TileSamplingFraction::getSamplingFraction(unsigned int drawerIdx, unsigned int channel) const {
  return m_samplingFraction->getCalibDrawer(drawerIdx)->getData(channel, 0, 0);
}

inline
float TileSamplingFraction::getNumberOfPhotoElectrons(unsigned int drawerIdx, unsigned int channel) const {
  return m_samplingFraction->getCalibDrawer(drawerIdx)->getData(channel, 0, 1);
}

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

CLASS_DEF(TileSamplingFraction, 102415689, 0)
CONDCONT_DEF(TileSamplingFraction, 239120355);

#endif
