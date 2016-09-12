/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILECONDTOOLTMDB_H
#define TILECONDITIONS_ITILECONDTOOLTMDB_H

#include "GaudiKernel/IAlgTool.h"

#include <array>


namespace TMDB {
  enum CHANNEL { D0 = 0, D1L = 1, D1R = 2, D2L = 3, D2R = 4, D3L = 5, D3R = 6, 
                 D5L = 0, D5R = 1, D6L = 2, D6R = 3,
  };

  enum THRESHOLD { D6LOW = 0, D6HIGH = 1, D5D6LOW = 2, D5D6HIGH = 3
  };

  using Weights = std::array<float, 7>;
}


class ITileCondToolTMDB: virtual public IAlgTool {

  public:

    virtual ~ITileCondToolTMDB() {};

    static const InterfaceID& interfaceID() {
      static const InterfaceID IID_TileCondToolTMDB("ITileCondToolTMDB", 1, 0);    
      return IID_TileCondToolTMDB;
    };

    virtual float getThreshold(unsigned int drawerIdx, TMDB::THRESHOLD threshold) const = 0;
    virtual float getDelay(unsigned int drawerIdx, TMDB::CHANNEL channel) const = 0;

    virtual void getCalib(unsigned int drawerIdx, TMDB::CHANNEL channel, float& a, float& b) const = 0;
    virtual unsigned int getWeights(unsigned int drawerIdx, TMDB::CHANNEL channel, TMDB::Weights& weights) const = 0;
    
    virtual float channelCalib(unsigned int drawerIdx, TMDB::CHANNEL channel, const std::vector<float>& samples) const = 0;
    virtual float channelCalib(unsigned int drawerIdx, TMDB::CHANNEL channel, float amplitude) const = 0;

};

#endif
