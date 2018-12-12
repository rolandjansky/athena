/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILECONDPROXY_H
#define TILECONDITIONS_ITILECONDPROXY_H

#include "TileConditions/TileCalibData.h"

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventIDRange.h"

/**
 * @class ITileCondProxy
 * @brief An abstract interface to get Tile conditions data from source (file, DB)
 *
 */
template<typename T>
class ITileCondProxy : virtual public IAlgTool {
  public:

    virtual ~ITileCondProxy() {}

    static const InterfaceID& interfaceID() {
      static const InterfaceID IID_ITileCondProxy("ITileCondProxy", 1 , 0);
      return IID_ITileCondProxy;
    }

   /**
    * @brief Fill TileCalibData with Tile conditions data from source (file, DB)
    * @param[out] calibData TileCalibData to fill with Tile conditions data
    * @param[out] eventRange EventIDRange during wich Tile conditions data are valid
    */
    virtual StatusCode fillCalibData(TileCalibData<T>& calibData, EventIDRange& eventRange) const = 0;

};

#endif
