/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Module.h"

#include "SCT_GeoModel/SCT_Identifier.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"

#include "SCT_GeoModel/SCT_Sensor.h"
#include "SCT_GeoModel/SCT_Hybrid.h"
#include "SCT_GeoModel/SCT_Pigtail.h"
#include "SCT_GeoModel/SCT_InnerSide.h"
#include "SCT_GeoModel/SCT_OuterSide.h"
#include "SCT_GeoModel/SCT_BaseBoard.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
// 8th Aug 2005 S.Mima modified.
#include "GeoModelKernel/GeoShapeSubtraction.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

#include <cmath>

SCT_Module::SCT_Module(const std::string & name) 
  : SCT_UniqueComponentFactory(name),
    //m_stereoSign(0),
    m_innerSide(0),
    m_outerSide(0),
    m_baseBoard(0)

{
  getParameters();
  m_logVolume = preBuild();
}


SCT_Module::~SCT_Module()
{
  // Clean up
  delete m_baseBoardPos; // 6th Apr 2005 S.Mima modified.
  delete m_innerSidePos;
  delete m_outerSidePos;

  //delete m_sensor; // 14:00 Thu 14th Jul 2005 D.Naito removed.
  delete m_innerSide;
  delete m_outerSide;
  delete m_baseBoard;

  delete m_env1RefPointVector;
  delete m_env2RefPointVector;
}


void
SCT_Module::getParameters()
{
  const SCT_BarrelModuleParameters       * parameters = geometryManager()->barrelModuleParameters();
  const SCT_GeneralParameters     * generalParameters = geometryManager()->generalParameters();

  //const SCT_OuterSide OuterSide("OuterSide"); // 14:00 Thu 14th Jul 2005 D.Naito removed.

  m_safety       = generalParameters->safety();

  m_sensorGap    = parameters->moduleSensorToSensorGap();
  
  m_stereoAngle = parameters->moduleStereoAngle();
  int      sign = parameters->moduleStereoUpperSign();
  m_upperSide   = parameters->moduleUpperSideNumber();
  m_stereoInner = -0.5*sign*m_stereoAngle;
  m_stereoOuter =  0.5*sign*m_stereoAngle;

  // 1st Feb 2005 D.Naito modified.
  //m_hybridAndBaseBoardOffsetDirection = (m_stereoSign == 0) ? -1 : +1;

  m_baseBoardOffsetY = parameters->baseBoardOffsetY();
  m_baseBoardOffsetZ = parameters->baseBoardOffsetZ();
}


const GeoLogVol * 
SCT_Module::preBuild()
{
  SCT_MaterialManager materials;

  // Create child components
  m_outerSide       = new SCT_OuterSide("OuterSide");
  m_baseBoard = new SCT_BaseBoard("BaseBoard");
  m_innerSide       = new SCT_InnerSide("InnerSide");
  //m_sensor          = new SCT_Sensor("BRLSensor"); // 14:00 Thu 14th Jul 2005 D.Naito removed.

  //
  // We have 2 envelopes.
  // 1. It contains two sensors and baseBoard.
  // 2. It contains hybrid and pigtail.
  //
  // See "doc/cornerName.ps".
  //

  // We need to take into account the safety incorporated in the inner and outer side 
  // envelopes. 
  double sensorWidth   =  m_innerSide->sensor()->width() + m_safety;
  double sensorLength  =  m_innerSide->sensor()->length() + m_safety;
  double innerSideHybridWidth   =  m_innerSide->hybrid()->width() + m_safety;
  double innerSideHybridLength  =  m_innerSide->hybrid()->length() + m_safety;
  double outerSideHybridWidth   =  m_outerSide->hybrid()->width() + m_safety;
  double outerSideHybridLength  =  m_outerSide->hybrid()->length() + m_safety;
  double baseBoardWidth         =  m_baseBoard->width() + m_safety;
  double baseBoardLength        =  m_baseBoard->length() + m_safety;
  double outerSidePigtailLength =  m_outerSide->pigtail()->length() + m_safety;


  // Define constants for convenience.
  // for corner of outer side sensor.
  CLHEP::Hep3Vector a(0.0, 0.5 * sensorWidth, 0.5 * sensorLength);
  CLHEP::Hep3Vector b(0.0, -0.5 * sensorWidth, a.z());
  CLHEP::Hep3Vector c(0.0, b.y(),-0.5 * sensorLength);
  CLHEP::Hep3Vector d(0.0, a.y(), c.z());

  // for corner of inner side sensor.
  CLHEP::Hep3Vector e(0.0, a.y(), a.z());
  CLHEP::Hep3Vector f(0.0, b.y(), b.z());
  CLHEP::Hep3Vector g(0.0, c.y(), c.z());
  CLHEP::Hep3Vector h(0.0, d.y(), d.z());

  // for corner of base board.
  CLHEP::Hep3Vector u(0.0,
	       m_baseBoardOffsetY + 0.5*baseBoardWidth,
	       m_baseBoardOffsetZ + 0.5*baseBoardLength);
  CLHEP::Hep3Vector v(0.0, m_baseBoardOffsetY - 0.5*baseBoardWidth, u.z());
  CLHEP::Hep3Vector w(0.0, v.y(), m_baseBoardOffsetZ - 0.5*baseBoardLength);
  CLHEP::Hep3Vector x(0.0, u.y(),w.z());
  
  // for corner of hybrid, connectorouter and pigtail of outer side.
  //CLHEP::Hep3Vector i(0.0, 
  //	       0.5*outerSideHybridWidth,
  //	       m_outerSide->hybridOffsetZ() + 0.5*outerSideHybridLength);
  //  CLHEP::Hep3Vector k(0.0,
  //	       -0.5*outerSideHybridWidth,
  //	       m_outerSide->hybridOffsetZ() + 0.5*outerSidePigtailLength);
  //CLHEP::Hep3Vector l(0.0,
  //	       -0.5*outerSideHybridWidth - m_outerSide->pigtail()->width(), k.z());
  //CLHEP::Hep3Vector m(0.0, l.y(),
  //	       m_outerSide->hybridOffsetZ() - 0.5*outerSidePigtailLength);
  //CLHEP::Hep3Vector n(0.0, k.y(),m.z());
  //CLHEP::Hep3Vector p(0.0, i.y(),
  //	       m_outerSide->hybridOffsetZ() - 0.5*outerSideHybridLength);

  CLHEP::Hep3Vector i(0.0, 
  	       0.5*outerSideHybridWidth,
  	       m_outerSide->hybridOffsetZ() + 0.5*outerSidePigtailLength);
  CLHEP::Hep3Vector l(0.0,
  	       -0.5*outerSideHybridWidth - m_outerSide->pigtail()->width(), i.z());
  CLHEP::Hep3Vector m(0.0, l.y(),
  	       m_outerSide->hybridOffsetZ() - 0.5*outerSidePigtailLength);
  CLHEP::Hep3Vector p(0.0, i.y(),
  	       m.z());



  // for corner of hybrid and interConnect of inner side.
  CLHEP::Hep3Vector q(0.0, 0.5*outerSideHybridWidth, m_outerSide->hybridOffsetZ() + 0.5*outerSideHybridLength );
  CLHEP::Hep3Vector r(0.0, -0.5*innerSideHybridWidth, q.z());
  CLHEP::Hep3Vector s(0.0, r.y(), m_innerSide->hybridOffsetZ() - 0.5*innerSideHybridLength);
  CLHEP::Hep3Vector t(0.0, q.y(), s.z());

  // All points turn +-20 mCLHEP::rad around physical center of module.
  a.rotateX(m_stereoOuter/CLHEP::radian);
  b.rotateX(m_stereoOuter/CLHEP::radian);
  c.rotateX(m_stereoOuter/CLHEP::radian);
  d.rotateX(m_stereoOuter/CLHEP::radian);

  e.rotateX(m_stereoInner/CLHEP::radian);
  f.rotateX(m_stereoInner/CLHEP::radian);
  g.rotateX(m_stereoInner/CLHEP::radian);
  h.rotateX(m_stereoInner/CLHEP::radian);

  i.rotateX(m_stereoOuter/CLHEP::radian);
  //k.rotateX(m_stereoOuter/CLHEP::radian);
  l.rotateX(m_stereoOuter/CLHEP::radian);
  m.rotateX(m_stereoOuter/CLHEP::radian);
  //n.rotateX(m_stereoOuter/CLHEP::radian);
  p.rotateX(m_stereoOuter/CLHEP::radian);

  q.rotateX(m_stereoInner/CLHEP::radian);
  r.rotateX(m_stereoInner/CLHEP::radian);
  s.rotateX(m_stereoInner/CLHEP::radian);
  t.rotateX(m_stereoInner/CLHEP::radian);

  // Calculate demension of envelope1.
  const double z_ab = std::max(a.z(), b.z());
  const double z_ef = std::max(e.z(), f.z());
  const double z_cd = std::min(c.z(), d.z());
  const double z_gh = std::min(g.z(), h.z());

  const double y_ad = std::max(a.y(), d.y());
  const double y_eh = std::max(e.y(), h.y());
  const double y_bc = std::min(b.y(), c.y());
  const double y_fg = std::min(f.y(), g.y());

  //const double y_ux = std::max(u.y(), x.y());
  //const double y_vw = std::min(v.y(), w.y());

  const double zmaxEnv1 = std::max(z_ab, z_ef);
  const double zminEnv1 = std::min(z_cd, z_gh);

  const double ypre_maxEnv1 = std::max(y_ad, y_eh);
  const double ypre_minEnv1 = std::min(y_bc, y_fg);

  const double ymaxEnv1 = std::max(ypre_maxEnv1, u.y());
  const double yminEnv1 = std::min(ypre_minEnv1, v.y());

  // some temporary value.
  const double thicknessEnv1 = m_sensorGap + 2.0*m_outerSide->sensor()->thickness();
  const double lengthEnv1 = zmaxEnv1 - zminEnv1;
  const double widthEnv1  = ymaxEnv1 - yminEnv1;

  // Center of evnelope1.
  const double xCenterEnv1 = 0.0;
  const double yCenterEnv1 = yminEnv1 + 0.5*widthEnv1;
  const double zCenterEnv1 = zmaxEnv1 - 0.5*lengthEnv1;

  m_env1RefPointVector = new CLHEP::Hep3Vector(-xCenterEnv1, -yCenterEnv1, -zCenterEnv1);

  // Calculate demension of envelope2.
  const double z_ikl = std::max(i.z(), l.z());
  const double z_qr = std::max(q.z(), r.z());
  const double z_mnp = std::min(m.z(), p.z());
  const double z_st = std::min(s.z(), t.z());

  const double y_ip = std::max(i.y(), p.y());
  const double y_qt = std::max(q.y(), t.y());
  const double y_lm = std::min(l.y(), m.y());
  const double y_rs = std::min(r.y(), s.y());

  const double zmaxEnv2 = std::max(z_ikl, z_qr);
  const double zminEnv2 = std::min(z_mnp, z_st);

  const double ymaxEnv2 = std::max(y_ip, y_qt);
  const double yminEnv2 = std::min(y_lm, y_rs);

  const double thicknessEnv2 = 2.0*(m_outerSide->hybridOffsetX() + 
				    0.5*m_innerSide->sensor()->thickness() + 
				    0.5*m_sensorGap + 
				    0.5*m_outerSide->hybrid()->thickness());
  const double lengthEnv2 = zmaxEnv2 - zminEnv2;
  const double widthEnv2  = ymaxEnv2 - yminEnv2;

  // Center of envelope2.
  const double xCenterEnv2 = 0.0;
  const double yCenterEnv2 = ymaxEnv2 - 0.5*widthEnv2;
  const double zCenterEnv2 = zmaxEnv2 - 0.5*lengthEnv2;

  m_env2RefPointVector = new CLHEP::Hep3Vector(-xCenterEnv2, -yCenterEnv2, -zCenterEnv2);

  // 8th Aug 2005 S.Mima modified.
  // Calculate dimension of subbox 
  const double xmaxSubBox = - 0.5*m_baseBoard->thickness() - m_safety;
  const double xminSubBox = - 0.5*thicknessEnv2 - 2.0*m_safety;

  const double ymaxSubBox = std::min(r.y(), s.y()) - m_safety;
  const double yminSubBox = yminEnv2 - 2.0*m_safety;

  const double zmaxSubBox = zmaxEnv1 + 2.0*m_safety;
  const double zminSubBox = zminEnv1 - 2.0*m_safety; 
  
  const double thicknessSubBox = xmaxSubBox - xminSubBox;
  const double widthSubBox = ymaxSubBox - yminSubBox;
  const double lengthSubBox = zmaxSubBox - zminSubBox;

  // Center of subbox.
  const double xCenterSubBox = xmaxSubBox - 0.5*thicknessSubBox;
  const double yCenterSubBox = ymaxSubBox - 0.5*widthSubBox;
  const double zCenterSubBox = zmaxSubBox - 0.5*lengthSubBox;


  m_env1Thickness = thicknessEnv1 + 2*m_safety;
  m_env1Width     = widthEnv1     + 2*m_safety;
  m_env1Length    = lengthEnv1    + 2*m_safety;

  m_env2Thickness = thicknessEnv2 + 2*m_safety;
  m_env2Width     = widthEnv2     + 2*m_safety;
  m_env2Length    = lengthEnv2    + 2*m_safety;

  // Envelope 2 defines the overall thickness. NB. The ski assumes the 
  // the envelope "thickness" is centered on x, which it is.
  m_thickness   = m_env2Thickness;
  m_activeWidth = m_env2Width;
  m_width       = m_env2Width;
  m_length      = m_env1Length;

  // Calculate a few things.
  const double OSPosX =   0.5*m_sensorGap + 0.5*m_innerSide->sensor()->thickness();
  const double ISPosX = -(0.5*m_sensorGap + 0.5*m_innerSide->sensor()->thickness());

  //
  // Make an envelope for the whole module.
  //
  // 6th Feb 2005 D.Naito modified.
  const GeoBox * envelope1 = new GeoBox(0.5*m_env1Thickness, 0.5*m_env1Width, 0.5*m_env1Length);
  const GeoBox * envelope2 = new GeoBox(0.5*m_env2Thickness, 0.5*m_env2Width, 0.5*m_env2Length);

  // 8th Aug 2005 S.Mima modified.
  const GeoBox * subBox = new GeoBox(0.5*thicknessSubBox, 0.5*widthSubBox, 0.6*lengthSubBox);

  // In the following, envelope1 and envelope2 are added and SUBBOX is pulled. 
  const GeoShape & moduleEnvelope = (*envelope1 << HepGeom::Translate3D(xCenterEnv1, yCenterEnv1, zCenterEnv1)).
    add(*envelope2 << HepGeom::Translate3D(xCenterEnv2, yCenterEnv2, zCenterEnv2)).
    subtract(*subBox << HepGeom::Translate3D(xCenterSubBox, yCenterSubBox, zCenterSubBox));

  const GeoLogVol * moduleLog           = new GeoLogVol(getName(), &moduleEnvelope, materials.gasMaterial());
  
  //
  // inner side
  //
  CLHEP::HepRotation rotInner;
  rotInner.rotateZ(180*CLHEP::deg);
  rotInner.rotateX(m_stereoInner);
  m_innerSidePos = new HepGeom::Transform3D(rotInner,
				      CLHEP::Hep3Vector(ISPosX, 0.0, 0.0));

  //
  // outer side
  //
  CLHEP::HepRotation rotOuter;
  rotOuter.rotateX(m_stereoOuter);
  m_outerSidePos = new HepGeom::Transform3D(rotOuter,
				      CLHEP::Hep3Vector(OSPosX, 0.0, 0.0));

  //
  // base board
  // 6th Apr 2005 S.Mima modified.
  //
  const double baseBoardPosY = m_baseBoardOffsetY;
  const double baseBoardPosZ = m_baseBoardOffsetZ;
  m_baseBoardPos = new HepGeom::Translate3D(CLHEP::Hep3Vector( 0.0, baseBoardPosY, baseBoardPosZ));


  return moduleLog;
}


GeoVPhysVol * 
SCT_Module::build(SCT_Identifier id) const
{
  GeoFullPhysVol * module = new GeoFullPhysVol(m_logVolume); 

  // We make these fullPhysVols for the alignment code.
  // We probably should make the transform for the sensor 
  // alignable rather than the "side" to save making an extra full phys volume.

  //
  // Build the module
  //
  // Add Baseboard
  GeoTransform * baseBoardTransform = new GeoTransform(*m_baseBoardPos);
  module->add(baseBoardTransform);
  module->add(m_baseBoard->getVolume());

  // Add innerside
  GeoAlignableTransform * innerTransform = new GeoAlignableTransform(*m_innerSidePos);
  module->add(innerTransform);
  int innerSideNumber = (m_upperSide) ? 0 : 1;
  module->add(new GeoNameTag("Side#"+intToString(innerSideNumber))); // Identifier side=0
  module->add(new GeoIdentifierTag(innerSideNumber));
  id.setSide(innerSideNumber);
  Identifier innerId = id.getWaferId();
  GeoVPhysVol * innerSide = m_innerSide->build(id);
  module->add(innerSide);  
  // Store alignable transform
  detectorManager()->addAlignableTransform(0, innerId, innerTransform, innerSide);
  				    
  // Add outerside
  GeoAlignableTransform * outerTransform = new GeoAlignableTransform(*m_outerSidePos);
  module->add(outerTransform);
  int outerSideNumber = m_upperSide;
  module->add(new GeoNameTag("Side#"+intToString(outerSideNumber))); // Identifier side=1
  module->add(new GeoIdentifierTag(outerSideNumber));
  id.setSide(outerSideNumber);
  Identifier outerId = id.getWaferId();
  GeoVPhysVol * outerSide = m_outerSide->build(id);
  module->add(outerSide);
  // Store alignable transform
  detectorManager()->addAlignableTransform(0, outerId, outerTransform, outerSide);
  
  return module;
}
