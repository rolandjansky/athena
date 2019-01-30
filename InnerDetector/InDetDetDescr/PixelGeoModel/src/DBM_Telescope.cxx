/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DBM_Telescope.h"
#include "DBM_ModuleCage.h"

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoTube.h"

#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"

GeoVPhysVol* DBM_Telescope::Build() {

  double safety = 0.005*Gaudi::Units::mm;

  // telescope tilting angle in degree
  double angle = m_gmt_mgr->DBMAngle();

  // Telescope dimension
  double telescopeX = m_gmt_mgr->DBMTelescopeX();
  double telescopeY = m_gmt_mgr->DBMTelescopeY();
  double telescopeZ = m_gmt_mgr->DBMTelescopeZ();

  // module cage forming of three layers of plates 
  // on which is mounted a DBM module
  double layerUnitY = m_gmt_mgr->DBMModuleCageY();
  double layerUnitZ = m_gmt_mgr->DBMModuleCageZ();

  // bracket unit dimension
  double bracketX = m_gmt_mgr->DBMBracketX(); // width of the bracket unit
  double bracketY = m_gmt_mgr->DBMBracketY(); // total height of the bracket unit
  double bracketZ = m_gmt_mgr->DBMBracketZ(); // total thickness of the bracket unit,
  // back trapezoid block with window
  double trapBack_theta = m_gmt_mgr->DBMTrapezBackTheta();
  double trapBackX = m_gmt_mgr->DBMTrapezBackX();
  double trapBackY = m_gmt_mgr->DBMTrapezBackY();
  double trapBackShortZ = m_gmt_mgr->DBMTrapezBackShortZ();
  // bracket window
  double brcktWindowX = m_gmt_mgr->DBMBrcktWindowX();
  double brcktWindowY = m_gmt_mgr->DBMBrcktWindowY();
  double brcktWindow_offset = m_gmt_mgr->DBMBrcktWindowOffset();
  double brcktWindow_centerZ = m_gmt_mgr->DBMBrcktWindowCenterZ();
  // bracket front block
  double brcktTopBlockZ = m_gmt_mgr->DBMBrcktTopBlockZ();
  double brcktSideBlockX = m_gmt_mgr->DBMBrcktSideBlockX();
  double brcktSideBlockY = m_gmt_mgr->DBMBrcktSideBlockY();
  //// locking block
  double brcktLockZ = m_gmt_mgr->DBMBrcktLockZ();
  double brcktLockY = m_gmt_mgr->DBMBrcktLockY();
 
  // cooling side plate
  double coolingSidePlateX = m_gmt_mgr->DBMCoolingSidePlateX();
  double coolingSidePlateY = m_gmt_mgr->DBMCoolingSidePlateY();
  double coolingSidePlateZ = m_gmt_mgr->DBMCoolingSidePlateZ();
  // position of side plate, parallel to side plate axis,
  // measured from the back of the V-slide to the front of the side plate
  double coolingSidePlatePos = m_gmt_mgr->DBMCoolingSidePlatePos();
  // cooling plates next to bracket 
  double coolingFinLongZ = m_gmt_mgr->DBMBrcktFinLongZ();
  double coolingFinHeight = m_gmt_mgr->DBMBrcktFinHeight();
  double coolingFinThick = m_gmt_mgr->DBMBrcktFinThick();
  double coolingFinPos = m_gmt_mgr->DBMBrcktFinPos();

  double mainPlateX = m_gmt_mgr->DBMMainPlateX(); //dimension in x-direction or width

  // materials
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");

  // DBM telescope volume
  const GeoBox* telescopeBox = new GeoBox(telescopeX/2. + 3*safety, telescopeY/2.+ safety, telescopeZ/2.);
  const GeoLogVol* telescopeLog = new GeoLogVol("dbmTelescopeLog", telescopeBox, air);
  GeoPhysVol* telescopePhys = new GeoPhysVol(telescopeLog);

  GeoTrf::RotateX3D rmX10(-10.*Gaudi::Units::deg);

  DBM_ModuleCage moduleCage;
  GeoVPhysVol* moduleCagePhys = moduleCage.Build();

  // parameters for rotating the 3-layer unit
  double lyRadius = sqrt(layerUnitY*layerUnitY/4 + layerUnitZ*layerUnitZ/4);
  double lyAngle = atan(layerUnitY/layerUnitZ);//21.6444*Gaudi::Units::deg; // arctan(DBM3LayersY / DBM3LayersZ)
  // position of bottom tip of the 3-layers unit, which is the rotation point
  double layerUnitPos_Y = (trapBackY/cos(angle) - coolingSidePlateY)*cos(angle);
  double layerUnitPos_Z = coolingSidePlateY*sin(angle) + trapBackShortZ + bracketZ - brcktLockZ; 

  GeoTrf::Translation3D layerUnitPos( 0.0, -telescopeY/2. + layerUnitPos_Y + lyRadius * sin(lyAngle+angle), -telescopeZ/2. + layerUnitPos_Z + lyRadius * cos(lyAngle+angle) + 3*safety);
  GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(layerUnitPos*rmX10));
  GeoNameTag* tag = new GeoNameTag("dbm3layers");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(moduleCagePhys);


  //**** build bracket unit
  
  // back trapezoid block with window, will be rotated by 90 degree along the x-axis

  const GeoTrap* trapBack = new GeoTrap(trapBackY/2., trapBack_theta, 90.0*Gaudi::Units::deg, trapBackShortZ/2., trapBackX/2., trapBackX/2, 0.0, (trapBackShortZ+trapBackY*tan(angle))/2., trapBackX/2., trapBackX/2., 0.0);

  double brWindowPosY = brcktWindow_offset + brcktWindow_centerZ * tan(angle) + brcktWindowY/(2 * cos(angle));
  const GeoBox* brWindow = new GeoBox(brcktWindowX/2., trapBackShortZ, brcktWindowY/2.);
  GeoTrf::Translation3D brWindowPos(0., 0., trapBackY/2. - brWindowPosY);
  GeoTrf::Transform3D brWindowShift(brWindowPos*rmX10);

  const GeoShapeSubtraction& trapBack1 = trapBack->subtract(((*brWindow) << brWindowShift));

  const GeoMaterial* dbmPeek4 = m_mat_mgr->getMaterialForVolume("pix::DBMPeek4",trapBack1.volume());
  const GeoLogVol* trapBackLog = new GeoLogVol("bracketLog", &trapBack1, dbmPeek4);
  GeoPhysVol* trapBackPhys = new GeoPhysVol(trapBackLog);

  GeoTrf::RotateX3D rmX90(90.*Gaudi::Units::deg);
  double trapBackPos_Z = -telescopeZ/2. + bracketZ - brcktLockZ + ( (trapBackShortZ+trapBackY*tan(angle))/2. + trapBackY/2.*sin(trapBack_theta) - trapBackY*tan(trapBack_theta) );
  GeoTrf::Translation3D trapBackPos(0.0, -telescopeY/2. + trapBackY/2. + safety, trapBackPos_Z + 3*safety);
  xform = new GeoTransform(GeoTrf::Transform3D(trapBackPos*rmX90));
  tag = new GeoNameTag("trapBack");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(trapBackPhys);

  // bracket front top plate
  const GeoBox* brcktTopBlock = new GeoBox(bracketX/2., (bracketY - brcktSideBlockY)/2., brcktTopBlockZ/2.);
  const GeoMaterial* dbmPeekAluminium = m_mat_mgr->getMaterialForVolume("pix::DBMPeekAl",brcktTopBlock->volume());
  const GeoLogVol* brcktTopBlockLog = new GeoLogVol("bracketLog", brcktTopBlock, dbmPeekAluminium);
  GeoPhysVol* brcktTopBlockPhys = new GeoPhysVol(brcktTopBlockLog);

  GeoTrf::Translate3D brcktTopBlockPos( 0., -telescopeY/2. + brcktSideBlockY + (bracketY - brcktSideBlockY)/2.+2*safety, -telescopeZ/2. + brcktTopBlockZ/2.);
  xform = new GeoTransform(brcktTopBlockPos);
  tag = new GeoNameTag("brcktTopBlock");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(brcktTopBlockPhys);

  // bracket front side blocks
  const GeoBox* brcktSideBlock = new GeoBox(brcktSideBlockX/2., brcktSideBlockY/2., brcktTopBlockZ/2.);
  const GeoMaterial* dbmPeek2 = m_mat_mgr->getMaterialForVolume("pix::DBMPeek2",brcktSideBlock->volume());
  const GeoLogVol* brcktSideLog = new GeoLogVol("brcktSideLog", brcktSideBlock, dbmPeek2);
  GeoPhysVol* brcktSidePhys = new GeoPhysVol(brcktSideLog);

  GeoTrf::Translate3D brcktSidePos1( bracketX/2. - brcktSideBlockX/2., -telescopeY/2. + brcktSideBlockY/2.+safety, -telescopeZ/2. + brcktTopBlockZ/2.);
  xform = new GeoTransform(brcktSidePos1);
  tag = new GeoNameTag("brcktSideBlock");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(brcktSidePhys);

  GeoTrf::Translate3D brcktSidePos2( -bracketX/2. + brcktSideBlockX/2., -telescopeY/2. + brcktSideBlockY/2. + safety, -telescopeZ/2. + brcktTopBlockZ/2.);
  xform = new GeoTransform(brcktSidePos2);
  tag = new GeoNameTag("brcktSideBlock");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(brcktSidePhys);

  // bracket Locking block
  const GeoBox* brcktLock = new GeoBox(bracketX/2., brcktLockY/2.-safety, brcktLockZ/2.-safety);
  const GeoMaterial* dbmPeek3 = m_mat_mgr->getMaterialForVolume("pix::DBMPeek3",brcktLock->volume());
  const GeoLogVol* brcktLockLog = new GeoLogVol("bracketLog", brcktLock, dbmPeek3);
  GeoPhysVol* brcktLockPhys = new GeoPhysVol(brcktLockLog);

  GeoTrf::Translate3D brcktLockPos( 0., -telescopeY/2. + trapBackY + brcktLockY/2. + 2*safety, -telescopeZ/2. + bracketZ - brcktLockZ/2. + safety);
  xform = new GeoTransform(brcktLockPos);
  tag = new GeoNameTag("brcktLock");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(brcktLockPhys);

  // rectangular aluminium side plates

  const GeoBox* sidePlate = new GeoBox(coolingSidePlateX/2., coolingSidePlateY/2., coolingSidePlateZ/2.);
  const GeoMaterial* dbmAluminium2 = m_mat_mgr->getMaterialForVolume("pix::DBMAluminium2", sidePlate->volume());
  const GeoLogVol* sidePlateLog = new GeoLogVol("sidePlateLog", sidePlate, dbmAluminium2);
  GeoPhysVol* sidePlatePhys = new GeoPhysVol(sidePlateLog);

  double spAngle = angle + 17.57126*Gaudi::Units::deg;
  double spRadius = 1/2. * sqrt(coolingSidePlateZ*coolingSidePlateZ + coolingSidePlateY*coolingSidePlateY);

  GeoTrf::Translation3D sidePlatePos1( mainPlateX/2. + coolingSidePlateX/2. + 2*safety, - telescopeY/2. + spRadius * sin(spAngle), -telescopeZ/2. + layerUnitPos_Z - coolingSidePlatePos*cos(angle) + spRadius * cos(spAngle));
  xform = new GeoTransform(GeoTrf::Transform3D(sidePlatePos1*rmX10));
  tag = new GeoNameTag("sidePlate");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(sidePlatePhys);

  GeoTrf::Translation3D sidePlatePos2( -mainPlateX/2. - coolingSidePlateX/2. - 2*safety, - telescopeY/2. + spRadius * sin(spAngle), -telescopeZ/2. + layerUnitPos_Z - coolingSidePlatePos*cos(angle) + spRadius * cos(spAngle));
  xform = new GeoTransform(GeoTrf::Transform3D(sidePlatePos2*rmX10));
  tag = new GeoNameTag("sidePlate");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(sidePlatePhys);

  //cooling plates next to the bracket unit
  const GeoTrap* coolingFin = new GeoTrap(coolingFinHeight/2., trapBack_theta, 90.0*Gaudi::Units::deg, (coolingFinLongZ - coolingFinHeight*tan(angle))/2., coolingFinThick/2., coolingFinThick/2, 0.0, coolingFinLongZ/2., coolingFinThick/2., coolingFinThick/2., 0.0);
  const GeoMaterial* dbmAluminium3 = m_mat_mgr->getMaterialForVolume("pix::DBMAluminium3", coolingFin->volume());
  const GeoLogVol* finLog = new GeoLogVol("finLog", coolingFin, dbmAluminium3);
  GeoPhysVol* coolingFinPhys = new GeoPhysVol(finLog);

  GeoTrf::Translation3D finPos1( mainPlateX/2. - coolingFinThick/2. + safety, -telescopeY/2. +  coolingFinHeight/2. + 4*safety, -telescopeZ/2. + coolingFinPos + 0.05*Gaudi::Units::mm);
  xform = new GeoTransform(GeoTrf::Transform3D(finPos1*rmX90));
  tag = new GeoNameTag("finPlate");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(coolingFinPhys);

  GeoTrf::Translation3D finPos2( -mainPlateX/2. + coolingFinThick/2. - safety, -telescopeY/2. +  coolingFinHeight/2. + 4*safety, -telescopeZ/2. + coolingFinPos + 0.05*Gaudi::Units::mm);
  xform = new GeoTransform(GeoTrf::Transform3D(finPos2*rmX90));
  tag = new GeoNameTag("finPlate");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(coolingFinPhys);


  return telescopePhys;

}

