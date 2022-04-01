/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILESAMPLINGFRACTIONCONDALG_H
#define TILECONDITIONS_TILESAMPLINGFRACTIONCONDALG_H

#include "./TileCondAlg.h"
#include "TileConditions/TileSamplingFraction.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

/**
 * @class TileSamlingFractionCondAlg
 * @brief Condition algorithm to prepare Tile sampling fraction conditions object and put it into conditions store
 */
class TileSamplingFractionCondAlg: public TileCondAlg<TileSamplingFraction, TileCalibDrawerFlt> {

  public:

    using TileCondAlg<TileSamplingFraction, TileCalibDrawerFlt>::TileCondAlg;

  protected:

   /**
    * @brief Check compatibility of Geant4 version from input file with version used to calculate SF (DB)
    */
    virtual  StatusCode checkData(const TileCalibData<TileCalibDrawerFlt>& calibData) const override {

      if (m_g4Version > 0) {
        // G4 version used to calculate Tile sampling fraction is stored in AUX01 calib drawer in DB
        float g4Version = calibData.getCalibDrawer(1)->getData(0, 0, 0);
        if (m_g4Version != g4Version) {
          ATH_MSG_ERROR("Geant4 simulation version (" << m_g4Version / 100
                        << ") is not compatible with version used to calculate Tile SF (" << g4Version / 100 << ")");
          return StatusCode::FAILURE;
        }
      }
      return StatusCode::SUCCESS;
    };

  private:

   /**
    * @brief Geant4 simulation version to check compatibility with version used to calculate SF (DB)
    */
    Gaudi::Property<float> m_g4Version{this, "G4Version", -1,
        "G4 simulation version to check compatibility with version used to calculate Tile SF (DB), (-1 => not check)"};
};


#endif // TILECONDITIONS_TILESAMPLINGFRACTIONCONDALG_H
