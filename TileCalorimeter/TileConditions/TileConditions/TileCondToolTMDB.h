/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLTMDB_H
#define TILECONDITIONS_TILECONDTOOLTMDB_H

// Tile includes
#include "TileConditions/ITileCondToolTMDB.h"
#include "TileConditions/TileCalibData.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"

class TileCondToolTMDB: public AthAlgTool, virtual public ITileCondToolTMDB {

  public:

    static const InterfaceID& interfaceID() {
      static const InterfaceID IID_TileCondToolTMDB("TileCondToolTMDB", 1, 0);
      return IID_TileCondToolTMDB;
    };

    TileCondToolTMDB(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolTMDB();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual float getThreshold(unsigned int drawerIdx, unsigned int threshold) const override;
    virtual float getDelay(unsigned int drawerIdx, unsigned int channel) const override;

    virtual void getCalib(unsigned int drawerIdx, unsigned int channel, float& a, float& b) const override;
    virtual unsigned int getWeights(unsigned int drawerIdx, unsigned int channel, TMDB::Weights& weights) const override;

    virtual float channelCalib(unsigned int drawerIdx, unsigned int channel, const std::vector<float>& samples) const override;
    virtual float channelCalib(unsigned int drawerIdx, unsigned int channel, float amplitude) const override;

  private:

    SG::ReadCondHandleKey<TileCalibDataFlt> m_calibThresholdKey{this,
        "TileTMDBThreshold", "TileTMDBThreshold", "Input Tile TMDB threshold calibration constants"};

    SG::ReadCondHandleKey<TileCalibDataFlt> m_calibDelayKey{this,
        "TileTMDBDelay", "TileTMDBDelay", "Input Tile TMDB delay calibration constants"};

    SG::ReadCondHandleKey<TileCalibDataFlt> m_calibTmfKey{this,
        "TileTMDBTMF", "TileTMDBTMF", "Input Tile TMDB TMF calibration constants"};

    SG::ReadCondHandleKey<TileCalibDataFlt> m_calibDataKey{this,
        "TileTMDBCalib", "TileTMDBCalib", "Input Tile TMDB calibration constants"};



};

#endif
