/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDALG_H
#define TILECONDITIONS_TILECONDALG_H

#include "TileConditions/TileCalibData.h"
#include "TileConditions/TileSamplingFraction.h"
#include "TileConditions/ITileCondProxy.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

/**
 * @class TileCondAlg
 * @brief Condition algorithm to prepare Tile conditions object and put it into conditions store
 */
template<class CONDDATA, class CALIBDATA>
class TileCondAlg: public AthReentrantAlgorithm {
  public:

    using AthReentrantAlgorithm::AthReentrantAlgorithm;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  protected:

    virtual StatusCode checkData(const TileCalibData<CALIBDATA>& /*calibData*/) const { return StatusCode::SUCCESS; };

  private:

  /**
   * @brief Proxy tool to get Tile calibrations data from source (file, DB)
   */
    ToolHandle<ITileCondProxy<CALIBDATA> > m_conditionsProxy{this,
        "ConditionsProxy", "","Tile conditions proxy"};

  /**
   * @brief Name of output TileCondData
   */
    SG::WriteCondHandleKey<CONDDATA> m_condDataKey{this,
        "TileCondData", "", "Output Tile conditions object"};

};

#include "TileCondAlg.icc"

#endif // TILECONDITIONS_TILECONDALG_H
