/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArShapeP2.h"
#include <cassert>


LArShapeP2::LArShapeP2(float timeOffset,
                       float timeBinWidth,
                       const std::vector<std::vector<float> >& vShape,
                       const std::vector<std::vector<float> >& vShapeDer)
  : LAr2DWaveBase (timeOffset, timeBinWidth, vShape, vShapeDer)
{
} 


LArShapeP2::LArShapeP2(float timeOffset,
                       float timeBinWidth,
                       unsigned int nPhases,
                       unsigned int nSamples,
                       const std::vector<float>& shape,
                       const std::vector<float>& shapeDer,
                       unsigned int index)
  : LAr2DWaveBase (timeOffset, timeBinWidth,
                   nPhases, nSamples,
                   shape, shapeDer, index)
{
}


LArShapeP2::LArShapeP2 (const LArCompactSubsetChannelProxy& other)
  : LAr2DWaveBase (other)
{
}


