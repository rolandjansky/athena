/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILECONDTOOLTMDB_H
#define TILECONDITIONS_ITILECONDTOOLTMDB_H

#include "GaudiKernel/IAlgTool.h"

#include <array>


namespace TMDB {
  using Weights = std::array<float, 7>;
}


class ITileCondToolTMDB: virtual public IAlgTool {

  public:

    virtual ~ITileCondToolTMDB() {};

    static const InterfaceID& interfaceID() {
      static const InterfaceID IID_TileCondToolTMDB("ITileCondToolTMDB", 1, 0);    
      return IID_TileCondToolTMDB;
    };

    virtual float getThreshold(unsigned int drawerIdx, unsigned int threshold) const = 0;
    virtual float getDelay(unsigned int drawerIdx, unsigned int channel) const = 0;

    virtual void getCalib(unsigned int drawerIdx, unsigned int channel, float& a, float& b) const = 0;
    virtual unsigned int getWeights(unsigned int drawerIdx, unsigned int channel, TMDB::Weights& weights) const = 0;
    
    virtual float channelCalib(unsigned int drawerIdx, unsigned int channel, const std::vector<float>& samples) const = 0;
    virtual float channelCalib(unsigned int drawerIdx, unsigned int channel, float amplitude) const = 0;

};

#endif
