/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_OuterSide.h"

#include "SCT_GeoModel/SCT_Identifier.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"
#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"

//  module parts.
#include "SCT_GeoModel/SCT_Sensor.h"
#include "SCT_GeoModel/SCT_Hybrid.h"
#include "SCT_GeoModel/SCT_Pigtail.h"

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

SCT_OuterSide::SCT_OuterSide(const std::string & name) 
  : SCT_UniqueComponentFactory(name),
    m_hybrid(0),
    m_pigtail(0),
    m_sensor(0)
{
  getParameters();
  m_logVolume = preBuild();
}


SCT_OuterSide::~SCT_OuterSide()
{
  delete m_hybrid;
  delete m_pigtail;
  delete m_sensor;
  delete m_env1RefPointVector;
  delete m_env2RefPointVector;
  if (m_hybridPos) m_hybridPos->unref();
  if (m_pigtailPos) m_pigtailPos->unref();
  if (m_sensorPos) m_sensorPos->unref();
}


void
SCT_OuterSide::getParameters()
{
  const SCT_GeneralParameters     * generalParameters = geometryManager()->generalParameters();
  const SCT_BarrelModuleParameters * parameters = geometryManager()->barrelModuleParameters();

  m_safety           = generalParameters->safety();
  m_hybridOffsetZ    = parameters->hybridOffsetZ();
  m_hybridOffsetX    = parameters->hybridOffsetX();
}


const GeoLogVol * 
SCT_OuterSide::preBuild()
{
  SCT_MaterialManager materials;

  // Create child components
  m_sensor             = new SCT_Sensor("BRLSensor");
  m_hybrid             = new SCT_Hybrid("Hybrid");
  m_pigtail            = new SCT_Pigtail("Pigtail");

  //
  // Define constants for convenience.
  // t_XXX: thickness of XXX.
  // w_XXX: width of XXX.
  // l_XXX: length of XXX.
  //
  const double t_hybrid = m_hybrid->thickness();
  const double w_hybrid = m_hybrid->width();
  //const double l_hybrid = m_hybrid->length();

  const double t_pigtail = m_pigtail->thickness();
  const double w_pigtail = m_pigtail->width();
  const double l_pigtail = m_pigtail->length();

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

  const double pigtailPosX = hybridPosX + 0.5*t_hybrid - 0.5*t_pigtail;
  const double pigtailPosY = -0.5*w_hybrid - 0.5*w_pigtail;
  const double pigtailPosZ = hybridPosZ;

  //
  // ose : OuterSideEnvelope
  // Reference: sct_module_geometry.ps
  //
  const double w_ose1 = w_sensor + m_safety;
  const double t_ose1 = t_sensor + m_safety;
  const double l_ose1 = l_sensor + m_safety;

  const double t_ose2 = t_hybrid + m_safety;
  const double w_ose2 = w_hybrid + w_pigtail + m_safety;
  const double l_ose2 = l_pigtail + m_safety;

  const double ose2PosX = hybridPosX;
  const double ose2PosY = hybridPosY - 0.5*w_pigtail;
  const double ose2PosZ = hybridPosZ;

  // *** 16:30 Wed 15th Jun 2005 D.Naito modified. (02)*********************************
  // *** -->>                                      (02)*********************************
  m_env1RefPointVector = new GeoTrf::Vector3D(0.0, 0.0, 0.0);
  m_env2RefPointVector = new GeoTrf::Vector3D(-ose2PosX, -ose2PosY, -ose2PosZ);
  // *** End of modified lines. ------------------ (02)*********************************

  m_hybridPos             = new GeoTransform(GeoTrf::Translate3D(hybridPosX, hybridPosY, hybridPosZ));
  m_hybridPos->ref();
  m_pigtailPos            = new GeoTransform(GeoTrf::Translate3D(pigtailPosX, pigtailPosY, pigtailPosZ));
  m_pigtailPos->ref();

  // The depth axis goes from the backside to the implant side 
  // and so point to away from the  module center.
  // The two sensor+hybrid combinations are built in a similar way.
  //
 //                      ^ 
  //        ---   hybrid  | 
  //      ------- sensor  | x-axis
  //
  // Shown is the outer side. The inner side is the same but with a rotation of 180 Gaudi::Units::deg around the z-axis.       
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
  const GeoBox * ose1Shape = new GeoBox(0.5 * t_ose1,
                                        0.5 * w_ose1,
                                        0.5 * l_ose1);
  const GeoBox * ose2Shape = new GeoBox(0.5 * t_ose2,
                                        0.5 * w_ose2,
                                        0.5 * l_ose2);

  const GeoShape & OuterSideEnvelopeShape = (*ose1Shape).
    add(*ose2Shape << GeoTrf::Translate3D(ose2PosX, ose2PosY, ose2PosZ));

  const GeoLogVol * OuterSideEnvelopeLog = new GeoLogVol("OuterSideEnvelope",
                                                         &OuterSideEnvelopeShape,
                                                         materials.gasMaterial());

  // 28th Mar S.Mima modified.
  // *** 16:30 Wed 15th Jun 2005 D.Naito modified. (03)*********************************
  //m_thickness = 0.5*t_sensor + m_hybridOffsetX + 0.5*t_ose2;
  // *** -->>                                      (03)*********************************
  m_thickness = 0.5*t_ose1 + m_hybridOffsetX + 0.5*t_ose2;
  // *** End of modified lines. ------------------ (03)*********************************
  m_width     = w_ose2;
  m_length    = l_ose1;

  return OuterSideEnvelopeLog;
}


GeoVPhysVol * 
SCT_OuterSide::build(SCT_Identifier id) const
{
  GeoFullPhysVol * outerSide = new GeoFullPhysVol(m_logVolume);

  //
  // Build the outerSide
  //
  // Add Sensor
  outerSide->add(new GeoIdentifierTag(1000));
  outerSide->add(m_sensorPos);
  outerSide->add(m_sensor->build(id));

  // Add Hybrid
  outerSide->add(m_hybridPos);
  outerSide->add(m_hybrid->getVolume());

  // Add Pigtail
  outerSide->add(m_pigtailPos);
  outerSide->add(m_pigtail->getVolume());

  return outerSide;
}
