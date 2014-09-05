/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileConditions/TileCondProxyWrapper.h"
#include "TileCondProxyCool.icc"
#include "TileCondProxyFile.icc"

template class TileCondProxyCool< TileCalibDrawerFlt >;
template class TileCondProxyCool< TileCalibDrawerBch >;
template class TileCondProxyCool< TileCalibDrawerOfc >;

template class TileCondProxyFile< TileCalibDrawerFlt >;
template class TileCondProxyFile< TileCalibDrawerBch >;
