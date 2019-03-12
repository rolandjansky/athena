/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// 1st Feb 2005 D.Naito modified.
// 28th Mar 2005 S.Mima modified.
//
#include "SCT_GeoModel/SCT_SkiAux.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"
#include "SCT_GeoModel/SCT_GeometryManager.h"

#include "SCT_GeoModel/SCT_Ski.h"
#include "SCT_GeoModel/SCT_Module.h"
// 14th Aug 2005 S.Mima modified.
#include "SCT_GeoModel/SCT_Bracket.h"
#include "SCT_GeoModel/SCT_Harness.h"
#include "SCT_GeoModel/SCT_SkiPowerTape.h"

#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"

#include <cmath>

inline double sqr(double x) {return x*x;}

// Small utility function to determine min and max angle of components.
void
calcMinMaxRatioS(double xCenter, double yCenter, 
                 double xWidth,  double yWidth, 
                 double & minRatio, double & maxRatio);

SCT_SkiAux::SCT_SkiAux(const std::string & name,
                       SCT_Ski * ski,
                       SCT_Bracket * bracket,
                       SCT_Harness * harness,
                       SCT_SkiPowerTape * skiPowerTape,
                       double innerRadius,
                       double bracketPhiOffset, 
                       double powerTapePhiOffset,
                       double divisionAngle,
                       InDetDD::SCT_DetectorManager* detectorManager,
                       const SCT_GeometryManager* geometryManager,
                       SCT_MaterialManager* materials) :
  SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
  m_innerRadius(innerRadius), 
  m_bracketPhiOffset(bracketPhiOffset),
  m_powerTapePhiOffset(powerTapePhiOffset),
  m_sectorAngle(divisionAngle),
  m_ski(ski), m_bracket(bracket), m_harness(harness), m_skiPowerTape(skiPowerTape)
{
  getParameters();
  m_physVolume = build();
}
    
void 
SCT_SkiAux::getParameters()
{
} 

GeoVPhysVol * 
SCT_SkiAux::build()
{
  // SkiAux is a composition with all the elements that go along with
  // ski but are not tilted with the ski. Currently the brackets, harness
  // and powertapes. Harness is not present in older geometries.
  // The reference point is the beam axis.
  // The envelope is a tube sector.
 
  //
  // Calculate Position of Brackets. Z position is calculated when it is positioned
  //
  // phi offset is calculated in SCT_Layer. For AGE compatible version it is positioned 
  // such that the lower edge of the bracket is lined up with the upper 
  // edge of the next module below it. 

  //  std::cout << "m_bracketPhiOffset = " << m_bracketPhiOffset << std::endl;
  //  std::cout << "m_powerTapePhiOffset = " << m_powerTapePhiOffset << std::endl;
  //  std::cout << "m_sectorAngle = " << m_sectorAngle << std::endl;

  // Define small distance for avoiding overlaps.
  double radiusBracket = m_innerRadius + 0.5*m_bracket->thickness() + epsilon();
  double xBracketPos = radiusBracket * cos(m_bracketPhiOffset);
  double yBracketPos = radiusBracket * sin(m_bracketPhiOffset);
 
  //  std::cout << "Bracket x,y = " << xBracketPos << "  " << yBracketPos << std::endl;

  // Calculate position of harness, if present. Phi offset is
  // same as power tapes, and we leave a gap equal to one tape
  // width
  double xHarnessPos = 0;
  double yHarnessPos = 0;
  if(m_harness != 0) {
    double radiusHarness = m_innerRadius +
      0.5 * m_harness->thickness() + 
      m_skiPowerTape->powerTapeThickness();
    xHarnessPos = radiusHarness * cos(m_powerTapePhiOffset);
    yHarnessPos = radiusHarness * sin(m_powerTapePhiOffset);
  }
  //  std::cout << "Harness x,y = " << xHarnessPos << "  " << yHarnessPos << std::endl;
    
  //
  // Calculate Position of PowerTapes
  //
  // Calculate x position of stack of tapes. 
  // If no harness is present, this is on the outer surface of the support 
  // cylinder with a gap equal to a tape width. In AGE there is a bug at the 
  // moment so this is not identical to AGE.
  // If the harness is present, the power tapes are moved beyond the harness

  double radiusTape = m_innerRadius +
    0.5 * m_skiPowerTape->thickness() + 
    m_skiPowerTape->powerTapeThickness();
  if(m_harness != 0) {radiusTape += m_harness->thickness();}
  double xTapePos = radiusTape * cos(m_powerTapePhiOffset);
  double yTapePos = radiusTape * sin(m_powerTapePhiOffset);

  //  std::cout << "Tape x,y = " << xTapePos << "  " << yTapePos << std::endl;

  //
  // Calculate envelope.
  //
  // Calculate the max radius that the components extend to.
  // N.B. harness is inside power tape, so don't need to check it
  double rMaxExtentOfPowerTape = 
    sqrt( sqr(radiusTape + 0.5 * m_skiPowerTape->thickness()) +
          sqr(0.5 * m_skiPowerTape->width()) );

  double rMaxExtentOfBracket = 
    sqrt( sqr(radiusBracket + 0.5 * m_bracket->thickness()) +
          sqr(0.5 * m_bracket->width()) );

  m_outerRadius = std::max(rMaxExtentOfBracket,rMaxExtentOfPowerTape) + epsilon();


  // calculate min and max tan(alpha) = y/x in order to work out 
  // maximum and minimum angle for the sector.
  // Assumes angles are between -pi/2 and pi/2 and don't straddle pi/2 boarder.
  double minAngle=0.;
  double maxAngle=0.;

  double halfAngleBracket   = atan(0.5 * m_bracket->width()/m_innerRadius);
  double halfAnglePowerTape = atan(0.5 * m_skiPowerTape->width()/m_innerRadius);
  // Harness and power tape are at same phi
  if(m_harness != 0) { 
    double halfAngleHarness   = atan(0.5 * m_harness->width()/m_innerRadius);
    halfAnglePowerTape = std::max(halfAnglePowerTape, halfAngleHarness);
  }

  if (m_powerTapePhiOffset > m_bracketPhiOffset) {
    minAngle = m_bracketPhiOffset   - halfAngleBracket;
    maxAngle = m_powerTapePhiOffset + halfAnglePowerTape;
  } else {
    minAngle = m_powerTapePhiOffset - halfAnglePowerTape;
    maxAngle = m_bracketPhiOffset   + halfAngleBracket;
  }    


  // Calculate extent so that min and max angle are equal distant to sector edge
  double midAngle = 0.5 * (minAngle+maxAngle);
  m_sectorStartAngle = midAngle - 0.5 * m_sectorAngle;

  // Length is same as power tape length
  m_length = m_skiPowerTape->length();

  const GeoTubs * skiAuxShape = new GeoTubs(m_innerRadius, m_outerRadius, 0.5*m_length, 
                                            m_sectorStartAngle, m_sectorAngle);
  const GeoLogVol *skiAuxLog = 
    new GeoLogVol(getName(), skiAuxShape, m_materials->gasMaterial());
  GeoPhysVol * skiAux = new GeoPhysVol(skiAuxLog);
  //  std::cout << "SCT_SkiAux: m_sectorStartAngle = " <<  m_sectorStartAngle
  //            << ", m_sectorAngle = " << m_sectorAngle << std::endl;
  //
  //  std::cout << "minangle, maxangle, m_sectorStartAngle, m_sectorAngle = " 
  //         << std::endl;
  //  std::cout << minAngle << ","  << maxAngle << ", " 
  //          << m_sectorStartAngle << "," 
  //         << m_sectorAngle << std::endl;

  //
  // Position brackets
  //

  // Loop over modules in ski as we need their z positions.
  for (int iModule = 0; iModule < m_ski->modulesPerSki(); iModule++) {
    
    // Z Position is position of the center of the baseBoard:
    // 15th Aug 2005 S.Mima modified.
    //    double baseBoardPosZ =  m_ski->zPos(iModule) + m_ski->module()->baseBoardCenter();
    double coolingCenterPosZ =  m_ski->zPos(iModule) + m_ski->coolingBlockOffsetZ();
    GeoTrf::Translation3D posBracket(xBracketPos, yBracketPos, coolingCenterPosZ);
    GeoTrf::RotateZ3D rotBracket(m_bracketPhiOffset);
    skiAux->add(new GeoTransform(GeoTrf::Transform3D(posBracket*rotBracket)));
    skiAux->add(m_bracket->getVolume());
  }

  // Position the harness
  if(m_harness != 0) {
    GeoTrf::Translation3D posHarness(xHarnessPos, yHarnessPos, 0);
    GeoTrf::RotateZ3D rotHarness(m_powerTapePhiOffset);
    skiAux->add(new GeoTransform(GeoTrf::Transform3D(posHarness*rotHarness)));
    skiAux->add(m_harness->getVolume());
  }

  //
  // Position the power tapes
  //
  GeoTrf::Translation3D posTape(xTapePos, yTapePos, 0);
  GeoTrf::RotateZ3D rotTape(m_powerTapePhiOffset);
  
  skiAux->add(new GeoTransform(GeoTrf::Transform3D(posTape*rotTape)));
  skiAux->add(m_skiPowerTape->getVolume());


  // return the volume pointer of the top volume
  return skiAux;

}

void
calcMinMaxRatioS(double xCenter, double yCenter, 
                 double xWidth,  double yWidth, 
                 double & minRatio, double & maxRatio)
{
  // Returns minRatio and maxRatio

  // This function assumes x is always positive.
  double x = 0.5 * xWidth;
  double y = 0.5 * yWidth;
  
  double r00,r01,r10,r11;

  r11 = (yCenter + y) / (xCenter + x);
  r00 = (yCenter - y) / (xCenter - x);
  r10 = (yCenter + y) / (xCenter - x);
  r01 = (yCenter - y) / (xCenter + x);

  //std::cout << "r11,r00,r10,r02: " 
  //        << r11 << " "
  //       << r00 << " "
  //       << r10 << " "
  //       << r01 << " " << std::endl;
    
  minRatio = std::min(r11 , std::min(r00, std::min(r10, r01)));
  maxRatio = std::max(r11 , std::max(r00, std::max(r10, r01)));

}
