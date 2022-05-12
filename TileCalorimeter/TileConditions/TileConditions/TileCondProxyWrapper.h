/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDPROXYWRAPPER_H
#define TILECONDITIONS_TILECONDPROXYWRAPPER_H

#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileCalibBlobObjs/TileCalibDrawerBch.h"
#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"
#include "TileConditions/TileCondProxyCool.h"
#include "TileConditions/TileCondProxyFile.h"

typedef TileCondProxyCool< TileCalibDrawerFlt > TileCondProxyCoolFlt;
typedef TileCondProxyCool< TileCalibDrawerBch > TileCondProxyCoolBch;
typedef TileCondProxyCool< TileCalibDrawerOfc > TileCondProxyCoolOfc;

typedef TileCondProxyFile< TileCalibDrawerFlt > TileCondProxyFileFlt;
typedef TileCondProxyFile< TileCalibDrawerBch > TileCondProxyFileBch;

#endif
