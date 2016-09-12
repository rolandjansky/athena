/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLTMDB_H
#define TILECONDITIONS_TILECONDTOOLTMDB_H

// Tile includes
#include "TileConditions/ITileCondToolTMDB.h"
#include "TileConditions/ITileCondProxy.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

class TileCalibDrawerFlt;

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

    virtual float getThreshold(unsigned int drawerIdx, TMDB::THRESHOLD threshold) const override;
    virtual float getDelay(unsigned int drawerIdx, TMDB::CHANNEL channel) const override;

    virtual void getCalib(unsigned int drawerIdx, TMDB::CHANNEL channel, float& a, float& b) const override;
    virtual unsigned int getWeights(unsigned int drawerIdx, TMDB::CHANNEL channel, TMDB::Weights& weights) const override;
    
    virtual float channelCalib(unsigned int drawerIdx, TMDB::CHANNEL channel, const std::vector<float>& samples) const override;
    virtual float channelCalib(unsigned int drawerIdx, TMDB::CHANNEL channel, float amplitude) const override;

  private:

    //=== TileCondProxies
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryThreshold;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryDelay;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryTMF;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryCalib;

};

#endif
