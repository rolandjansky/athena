//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECALIBCONDALG_H
#define TILECONDITIONS_TILECALIBCONDALG_H

#include "TileConditions/TileCalibData.h"
#include "TileConditions/ITileCondProxy.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

/**
 * @class TileCalibCondAlg
 * @brief Condition algorithm to prepare TileCalibData object and put it into condition store
 *
 */
template<class T>
class TileCalibCondAlg: public AthAlgorithm {
  public:

    TileCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~TileCalibCondAlg();

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:

  /**
   * @brief Proxy tool to get Tile conditions data from source (file, DB)
   */
    ToolHandle<ITileCondProxy<T> > m_conditionsProxy{this, "ConditionsProxy", "",
                                                     "Tile conditions proxy"};

  /**
   * @brief Name of output TileCalibData
   */
    SG::WriteCondHandleKey<TileCalibData<T> > m_calibDataKey{this, "TileCalibData", "",
                                                            "Output Tile calibration constants"};
};

#include "TileCalibCondAlg.icc"

#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
typedef TileCalibCondAlg<TileCalibDrawerFlt> TileCalibFltCondAlg;

#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"
typedef TileCalibCondAlg<TileCalibDrawerOfc> TileCalibOfcCondAlg;


#endif // TILECONDITIONS_TILECALIBCONDALG_H
