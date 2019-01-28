/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// 13rd Mar 2005 D.Naito
// 28th Mar 2005 S.Mima
// Wed 15th Jun 2005 D.Naito modified.
//
#include "SCT_GeoModel/SCT_InnerSide.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"
#include "SCT_GeoModel/SCT_Identifier.h"

//  module parts.
#include "SCT_GeoModel/SCT_Sensor.h"
#include "SCT_GeoModel/SCT_Hybrid.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>

SCT_InnerSide::SCT_InnerSide(const std::string & name) 
  : SCT_UniqueComponentFactory(name),
    m_hybrid(0),
    m_sensor(0)
{
  getParameters();
  m_logVolume = preBuild();
}


SCT_InnerSide::~SCT_InnerSide()
{
  delete m_hybrid;
  delete m_sensor;
  delete m_env1RefPointVector;
  delete m_env2RefPointVector;
  if (m_hybridPos) m_hybridPos->unref();
  if (m_sensorPos) m_sensorPos->unref();
}


void
SCT_InnerSide::getParameters()
{
  const SCT_BarrelModuleParameters       * parameters = geometryManager()->barrelModuleParameters();
  const SCT_GeneralParameters     * generalParameters = geometryManager()->generalParameters();

  m_safety           = generalParameters->safety();
  m_hybridOffsetZ    = parameters->hybridOffsetZ();
  m_hybridOffsetX    = parameters->hybridOffsetX();
}


const GeoLogVol * 
SCT_InnerSide::preBuild()
{
  SCT_MaterialManager materials;

  // Create child components
  m_sensor             = new SCT_Sensor("BRLSensor");
  m_hybrid             = new SCT_Hybrid("Hybrid");

  //
  // Define constants for convenience.
  // t_XXX: thickness of XXX.
  // w_XXX: width of XXX.
  // l_XXX: length of XXX.
  //
  const double t_hybrid = m_hybrid->thickness();
  const double w_hybrid = m_hybrid->width();
  const double l_hybrid = m_hybrid->length();

  const double t_sensor = m_sensor->thickness();
  const double w_sensor = m_sensor->width();
  const double l_sensor = m_sensor->length();

  //   
  // Calculate a few things.
  //
  const double sensorPosX = 0.0;
  const double sensorPosY = 0.0;
  const double sensorPosZ = 0.0;

  const double hybridPosX = m_hybridOffsetX;
  const double hybridPosY = 0.0;
  const double hybridPosZ = m_hybridOffsetZ;

  //
  // ise : InnerSideEnvelope
  // Reference: sct_module_geometry.ps
  //
  // 28th Mar S.Mima modified
  // Wed 15th Jun 2005 D.Naito modified.
  const double w_ise1 = w_sensor + m_safety;
  const double t_ise1 = t_sensor + m_safety;
  const double l_ise1 = l_sensor + m_safety;

  const double t_ise2 = t_hybrid + m_safety;
  const double w_ise2 = w_hybrid + m_safety;
  const double l_ise2 = l_hybrid + m_safety;

  const double ise2PosX = hybridPosX;
  const double ise2PosY = hybridPosY;
  const double ise2PosZ = hybridPosZ;

  // *** 16:30 Wed 15th Jun 2005 D.Naito modified. (00)*********************************
  // *** -->>                                      (00)*********************************
  m_env1RefPointVector = new GeoTrf::Vector3D(0.0, 0.0, 0.0);
  m_env2RefPointVector = new GeoTrf::Vector3D(-ise2PosX, -ise2PosY, -ise2PosZ);
  // *** End of modified lines. ------------------ (00)*********************************

  m_hybridPos             = new GeoTransform(GeoTrf::Translate3D(hybridPosX, hybridPosY, hybridPosZ));
  m_hybridPos->ref();

  // The depth axis goes from the backside to the implant side 
  // and so point away from the module center.
  // The two sensor+hybrid combinations are built in a similar way.
  //
  //                      ^ 
  //        ---   hybrid  | 
  //      ------- sensor  | x-axis
  //
  // Shown is the outer side. The inner side is the same but with a rotation of 180 deg around the z-axis.       
  // 
  //Gaudi::Units::HepRotation rotSensor;
  //rotSensor.rotateZ(180*Gaudi::Units::deg);
  //m_outerSidePos = new GeoTrf::Transform3D(rotOuter, GeoTrf::Vector3D(0.5 * (m_sensorGap + sectThickness), 0., 0.));
  //m_sensorPos = new GeoTransform(GeoTrf::Transform3D(rotSensor, GeoTrf::Vector3D(sensorPosX, sensorPosY, sensorPosZ)));
  m_sensorPos             = new GeoTransform(GeoTrf::Translate3D(sensorPosX, sensorPosY, sensorPosZ));
  m_sensorPos->ref();

  //
  // Make an envelope for the whole module.
  //
  const GeoBox * ise1Shape = new GeoBox(0.5 * t_ise1,
                                        0.5 * w_ise1,
                                        0.5 * l_ise1);
  const GeoBox * ise2Shape = new GeoBox(0.5 * t_ise2,
                                        0.5 * w_ise2,
                                        0.5 * l_ise2);

  const GeoShape & InnerSideEnvelopeShape = (*ise1Shape).
    add(*ise2Shape << GeoTrf::Translate3D(ise2PosX, ise2PosY, ise2PosZ));

  const GeoLogVol * InnerSideEnvelopeLog = new GeoLogVol("InnerSideEnvelope",
                                                         &InnerSideEnvelopeShape,
                                                         materials.gasMaterial());
  // 28th Mar S.Mima modified
  // *** 16:30 Wed 15th Jun 2005 D.Naito modified. (00)*********************************
  //m_thickness = 0.5*t_sensor + hybridPosX + 0.5*t_ise2;
  // *** -->>                                      (00)*********************************
  m_thickness = 0.5*t_ise1 + hybridPosX + 0.5*t_ise2;
  // *** End of modified lines. ------------------ (00)*********************************
  m_width     = w_ise1;
  m_length    = l_ise1;

  return InnerSideEnvelopeLog;
}


GeoVPhysVol * 
SCT_InnerSide::build(SCT_Identifier id) const
{
  GeoFullPhysVol * innerSide = new GeoFullPhysVol(m_logVolume);

  //
  // Build the innerSide
  //
  // Add Sensor
  innerSide->add(new GeoIdentifierTag(1000));
  innerSide->add(m_sensorPos);
  innerSide->add(m_sensor->build(id));

  // Add Hybrid
  innerSide->add(m_hybridPos);
  innerSide->add(m_hybrid->getVolume());

  return innerSide;
}
