/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.7%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.7%.codegen_version

//## begin module%4457C98D0025.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4457C98D0025.CLHEP::cm

//## begin module%4457C98D0025.cp preserve=no
//## end module%4457C98D0025.cp

//## Module: HECLongBlock%4457C98D0025; Pseudo Package body
//## Source file: /home/atlas/TEST-12.0.2-00/LArCalorimeter/LArGeoModel/LArReadoutGeometry/src/HECLongBlock.cxx

//## begin module%4457C98D0025.additionalIncludes preserve=no
//## end module%4457C98D0025.additionalIncludes

//## begin module%4457C98D0025.includes preserve=yes
//## end module%4457C98D0025.includes

// HECLongBlock
#include "LArReadoutGeometry/HECLongBlock.h"
//## begin module%4457C98D0025.additionalDeclarations preserve=yes
//## end module%4457C98D0025.additionalDeclarations

#include <cstdlib>  // For NULL


// Class HECLongBlock 

HECLongBlock::HECLongBlock(const HECLongBlock &right)
  //## begin HECLongBlock::HECLongBlock%4457C98D0025_copy.hasinit preserve=no
  //## end HECLongBlock::HECLongBlock%4457C98D0025_copy.hasinit
  //## begin HECLongBlock::HECLongBlock%4457C98D0025_copy.initialization preserve=yes
  : m_blockNumber(right.m_blockNumber),
    m_innerRadius(right.m_innerRadius),
    m_outerRadius(right.m_outerRadius),
    m_depth(right.m_depth),
    m_numLArGaps(right.m_numLArGaps),
    m_frontPlateThickness(right.m_frontPlateThickness),
    m_backPlateThickness(right.m_backPlateThickness)
  //## end HECLongBlock::HECLongBlock%4457C98D0025_copy.initialization
{
  //## begin HECLongBlock::HECLongBlock%4457C98D0025_copy.body preserve=yes
  for (int i=0;i<14;i++) m_RadialSeg.push_back( right.m_RadialSeg[i] ? (HECRadialSegment *) 0 : new HECRadialSegment(*right.m_RadialSeg[i]));
  //## end HECLongBlock::HECLongBlock%4457C98D0025_copy.body
}

HECLongBlock::HECLongBlock (unsigned int blockNum, double innerRad, double outerRad, double totalDepth, unsigned int nLarGaps, double frontPlateThick, double backPlateThick, const double etaRange[15])
  //## begin HECLongBlock::HECLongBlock%4457DF0A0112.hasinit preserve=no
  //## end HECLongBlock::HECLongBlock%4457DF0A0112.hasinit
  //## begin HECLongBlock::HECLongBlock%4457DF0A0112.initialization preserve=yes
  : m_blockNumber(blockNum),
    m_innerRadius(innerRad),
    m_outerRadius(outerRad),
    m_depth(totalDepth),
    m_numLArGaps(nLarGaps),
    m_frontPlateThickness(frontPlateThick),
    m_backPlateThickness(backPlateThick)
  
  //## end HECLongBlock::HECLongBlock%4457DF0A0112.initialization
{
  //## begin HECLongBlock::HECLongBlock%4457DF0A0112.body preserve=yes
  for (int i=0;i<14;i++) m_RadialSeg.push_back(NULL);
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
  //## end HECLongBlock::HECLongBlock%4457DF0A0112.body
}


HECLongBlock::~HECLongBlock()
{
  //## begin HECLongBlock::~HECLongBlock%4457C98D0025_dest.body preserve=yes
  for (int i=0;i<14;i++) delete m_RadialSeg[i];
  //## end HECLongBlock::~HECLongBlock%4457C98D0025_dest.body
}


HECLongBlock & HECLongBlock::operator=(const HECLongBlock &right)
{
  //## begin HECLongBlock::operator=%4457C98D0025_assign.body preserve=yes
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
  //## end HECLongBlock::operator=%4457C98D0025_assign.body
}


// Additional Declarations
  //## begin HECLongBlock%4457C98D0025.declarations preserve=yes
  //## end HECLongBlock%4457C98D0025.declarations

//## begin module%4457C98D0025.epilog preserve=yes
//## end module%4457C98D0025.epilog
