//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECALIBEMSCALECONDALG_H
#define TILECONDITIONS_TILECALIBEMSCALECONDALG_H

#include "TileConditions/TileEMScale.h"
#include "TileConditions/ITileCondProxy.h"
#include "TileConditions/TileCablingSvc.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

/**
 * @brief Condition algorithm to produce TileEMScale and put it into condition store
 */

class TileEMScaleCondAlg: public AthAlgorithm {
  public:

    TileEMScaleCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TileEMScaleCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:

   /**
    * @brief Online cache units as string, see TileIdentifier/TileRawChannelUnit.h
    */
    Gaudi::Property<std::string> m_onlCacheUnitString{this,
        "OnlCacheUnit", "Invalid", "Online cache unit string"};

   /**
    * @brief Tool to get offline CIS calibration constants
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_oflCisLinProxy{this,
        "OflCisLinProxy", "", "Tile offline liner CIS conditions proxy tool"};

   /**
    * @brief Tool to get offline non-linear CIS calibration constants
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_oflCisNlnProxy{this,
        "OflCisNlnProxy", "", "Tile offline non-linerar CIS conditions proxy tool"};

   /**
    * @brief Tool to get offline linear Laser correction factors
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_oflLasLinProxy{this,
        "OflLasLinProxy", "", "Tile offline linear Laser conditions proxy tool"};

   /**
    * @brief Tool to get non-linear Laser corection factors
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_oflLasNlnProxy{this,
        "OflLasNlnProxy", "", "Tile offline non-linear Laser conditions proxy tool"};

   /**
    * @brief Tool to get offline Laser fiber correction factors
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_oflLasFibProxy{this,
        "OflLasFibProxy", "", "Tile offline Laser fiber conditions proxy tool"};

   /**
    * @brief Tool to get offline Cesium calibration constants
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_oflCesProxy{this,
        "OflCesProxy", "", "Tile offline CES conditions proxy tool"};

   /**
    * @brief Tool to get offline EMS factors
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_oflEmsProxy{this,
        "OflEmsProxy", "", "Tile offline EMS conditions proxy tool"};



   /**
    * @brief Tool to get online CIS calibration constants
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_onlCisProxy{this,
        "OnlCisProxy", "", "Tile online CIS conditions proxy tool"};

   /**
    * @brief Tool to get Tile online Laser correction factors
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_onlLasProxy{this,
        "OnlLasProxy", "", "Tile online Laser conditions proxy tool"};

   /**
    * @brief Tool to get Tile online Cesium calibration constants
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_onlCesProxy{this,
        "OnlCesProxy", "", "Tile online CES conditions proxy tool"};

   /**
    * @brief Tool to get Tile online EMS factors
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_onlEmsProxy{this,
        "OnlEmsProxy", "", "Tile online EMS conditions proxy tool"};


   /**
    * @brief Name of output TileEMScale
    */
    SG::WriteCondHandleKey<TileEMScale> m_calibEmsKey{this,
        "TileEMScale", "TileEMScale", "Output Tile EMS calibration constants"};

   /**
    * @brief Name of conditions service
    */
    ServiceHandle<ICondSvc> m_condSvc{this,
        "CondSvc", "CondSvc", "The conditions service"};

   /**
    * @brief Name of Tile cabling service
    */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service" };


    bool m_useOflLasFib;
    unsigned int m_maxChannels;
    unsigned int m_maxGains;
    TileRawChannelUnit::UNIT m_onlCacheUnit;
};


#endif // TILECONDITIONS_TILECALIBEMSCALECONDALG_H
