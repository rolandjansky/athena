/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef IOVSVC_CALLBACK_ARGS_K
#define IOVSVC_CALLBACK_ARGS_K(K) int&,std::list<std::string>& K
#endif

#endif

