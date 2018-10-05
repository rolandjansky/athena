/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/HECLongBlock.h"

HECLongBlock::HECLongBlock(const HECLongBlock &right)
  : m_blockNumber(right.m_blockNumber),
    m_innerRadius(right.m_innerRadius),
    m_outerRadius(right.m_outerRadius),
    m_depth(right.m_depth),
    m_numLArGaps(right.m_numLArGaps),
    m_frontPlateThickness(right.m_frontPlateThickness),
    m_backPlateThickness(right.m_backPlateThickness)
{
  for (int i=0;i<14;i++) m_RadialSeg.push_back( right.m_RadialSeg[i] ? (HECRadialSegment *) 0 : new HECRadialSegment(*right.m_RadialSeg[i]));
}

HECLongBlock::HECLongBlock (unsigned int blockNum
			    , double innerRad
			    , double outerRad
			    , double totalDepth
			    , unsigned int nLarGaps
			    , double frontPlateThick
			    , double backPlateThick
			    , const double etaRange[15])
  : m_blockNumber(blockNum),
    m_innerRadius(innerRad),
    m_outerRadius(outerRad),
    m_depth(totalDepth),
    m_numLArGaps(nLarGaps),
    m_frontPlateThickness(frontPlateThick),
    m_backPlateThickness(backPlateThick)
{
  for (int i=0;i<14;i++) m_RadialSeg.push_back(nullptr);
  bool started=false;
  for (int i=0;i<14;i++) {
    if (etaRange[i]!=etaRange[i+1]) {
      started=true;
      m_RadialSeg[i] = new HECRadialSegment(etaRange[i],etaRange[i+1]);
    }
    else {
      if (started) break;
    }
  }
}


HECLongBlock::~HECLongBlock()
{
  for (int i=0;i<14;i++) delete m_RadialSeg[i];
}

HECLongBlock & HECLongBlock::operator=(const HECLongBlock &right)
{
  if (this != & right) {
    m_blockNumber=right.m_blockNumber;
    m_innerRadius=right.m_innerRadius;
    m_outerRadius=right.m_outerRadius;
    m_depth=right.m_depth;
    m_numLArGaps=right.m_numLArGaps;
    m_frontPlateThickness=right.m_frontPlateThickness;
    m_backPlateThickness=right.m_backPlateThickness;
    
    for (int i=0;i<14;i++) m_RadialSeg.push_back( right.m_RadialSeg[i] ? (HECRadialSegment *) 0 : new HECRadialSegment(*right.m_RadialSeg[i]));
    
  }
  return *this;
}
