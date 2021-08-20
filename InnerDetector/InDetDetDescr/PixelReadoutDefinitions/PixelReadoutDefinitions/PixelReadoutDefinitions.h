/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELREADOUTDEFINITIONS_H
#define PIXELREADOUTDEFINITIONS_H

namespace InDetDD
{

enum class PixelModuleType
{
  DBM,
  IBL_PLANAR,
  IBL_3D,
  PIX_BARREL,
  PIX_ENDCAP,
  NONE
};

enum class PixelDiodeType
{
  NORMAL,
  LONG,
  GANGED,
  LARGE
};

enum class PixelReadoutTechnology
{
  FEI3,
  FEI4,
  RD53
};

}

#endif
