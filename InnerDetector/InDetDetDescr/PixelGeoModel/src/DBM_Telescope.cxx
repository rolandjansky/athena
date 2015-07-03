/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/DBM_Telescope.h"
#include "PixelGeoModel/DBM_ModuleCage.h"

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

#include "InDetGeoModelUtils/InDetMaterialManager.h"

GeoVPhysVol* DBM_Telescope::Build() {

  double safety = 0.005*CLHEP::mm;

  // telescope tilting angle in degree
  double angle = gmt_mgr->DBMAngle();

  // Telescope dimension
  double telescopeX = gmt_mgr->DBMTelescopeX();
  double telescopeY = gmt_mgr->DBMTelescopeY();
  double telescopeZ = gmt_mgr->DBMTelescopeZ();

  // module cage forming of three layers of plates 
  // on which is mounted a DBM module
  double layerUnitY = gmt_mgr->DBMModuleCageY();
  double layerUnitZ = gmt_mgr->DBMModuleCageZ();

  // bracket unit dimension
  double bracketX = gmt_mgr->DBMBracketX(); // width of the bracket unit
  double bracketY = gmt_mgr->DBMBracketY(); // total height of the bracket unit
  double bracketZ = gmt_mgr->DBMBracketZ(); // total thickness of the bracket unit,
  // back trapezoid block with window
  double trapBack_theta = gmt_mgr->DBMTrapezBackTheta();
  double trapBackX = gmt_mgr->DBMTrapezBackX();
  double trapBackY = gmt_mgr->DBMTrapezBackY();
  double trapBackShortZ = gmt_mgr->DBMTrapezBackShortZ();
  // bracket window
  double brcktWindowX = gmt_mgr->DBMBrcktWindowX();
  double brcktWindowY = gmt_mgr->DBMBrcktWindowY();
  double brcktWindow_offset = gmt_mgr->DBMBrcktWindowOffset();
  double brcktWindow_centerZ = gmt_mgr->DBMBrcktWindowCenterZ();
  // bracket front block
  double brcktTopBlockZ = gmt_mgr->DBMBrcktTopBlockZ();
  double brcktSideBlockX = gmt_mgr->DBMBrcktSideBlockX();
  double brcktSideBlockY = gmt_mgr->DBMBrcktSideBlockY();
  //// locking block
  double brcktLockZ = gmt_mgr->DBMBrcktLockZ();
  double brcktLockY = gmt_mgr->DBMBrcktLockY();
 
  // cooling side plate
  double coolingSidePlateX = gmt_mgr->DBMCoolingSidePlateX();
  double coolingSidePlateY = gmt_mgr->DBMCoolingSidePlateY();
  double coolingSidePlateZ = gmt_mgr->DBMCoolingSidePlateZ();
  // position of side plate, parallel to side plate axis,
  // measured from the back of the V-slide to the front of the side plate
  double coolingSidePlatePos = gmt_mgr->DBMCoolingSidePlatePos();
  // cooling plates next to bracket 
  double coolingFinLongZ = gmt_mgr->DBMBrcktFinLongZ();
  double coolingFinHeight = gmt_mgr->DBMBrcktFinHeight();
  double coolingFinThick = gmt_mgr->DBMBrcktFinThick();
  double coolingFinPos = gmt_mgr->DBMBrcktFinPos();

  double mainPlateX = gmt_mgr->DBMMainPlateX(); //dimension in x-direction or width

  // materials
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");

  // DBM telescope volume
  const GeoBox* telescopeBox = new GeoBox(telescopeX/2. + 3*safety, telescopeY/2.+ safety, telescopeZ/2.);
  const GeoLogVol* telescopeLog = new GeoLogVol("dbmTelescopeLog", telescopeBox, air);
  GeoPhysVol* telescopePhys = new GeoPhysVol(telescopeLog);

  CLHEP::HepRotation rm;
  CLHEP::HepRotation rmX10;
  rmX10.rotateX(-10.*CLHEP::deg);

  DBM_ModuleCage moduleCage;
  GeoVPhysVol* moduleCagePhys = moduleCage.Build();

  // parameters for rotating the 3-layer unit
  double lyRadius = sqrt(layerUnitY*layerUnitY/4 + layerUnitZ*layerUnitZ/4);
  double lyAngle = atan(layerUnitY/layerUnitZ);//21.6444*CLHEP::deg; // arctan(DBM3LayersY / DBM3LayersZ)
  // position of bottom tip of the 3-layers unit, which is the rotation point
  double layerUnitPos_Y = (trapBackY/cos(angle) - coolingSidePlateY)*cos(angle);
  double layerUnitPos_Z = coolingSidePlateY*sin(angle) + trapBackShortZ + bracketZ - brcktLockZ; 

  CLHEP::Hep3Vector layerUnitPos( 0.0, -telescopeY/2. + layerUnitPos_Y + lyRadius * sin(lyAngle+angle), -telescopeZ/2. + layerUnitPos_Z + lyRadius * cos(lyAngle+angle) + 3*safety);
  GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(rmX10,layerUnitPos));
  GeoNameTag* tag = new GeoNameTag("dbm3layers");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(moduleCagePhys);


  //**** build bracket unit
  
  // back trapezoid block with window, will be rotated by 90 degree along the x-axis

  const GeoTrap* trapBack = new GeoTrap(trapBackY/2., trapBack_theta, 90.0*CLHEP::deg, trapBackShortZ/2., trapBackX/2., trapBackX/2, 0.0, (trapBackShortZ+trapBackY*tan(angle))/2., trapBackX/2., trapBackX/2., 0.0);

  double brWindowPosY = brcktWindow_offset + brcktWindow_centerZ * tan(angle) + brcktWindowY/(2 * cos(angle));
  const GeoBox* brWindow = new GeoBox(brcktWindowX/2., trapBackShortZ, brcktWindowY/2.);
  CLHEP::Hep3Vector brWindowPos(0., 0., trapBackY/2. - brWindowPosY);
  HepGeom::Transform3D brWindowShift(rmX10,brWindowPos);

  const GeoShapeSubtraction& trapBack1 = trapBack->subtract(((*brWindow) << brWindowShift));

  const GeoMaterial* dbmPeek4 = mat_mgr->getMaterialForVolume("pix::DBMPeek4",trapBack1.volume());
  const GeoLogVol* trapBackLog = new GeoLogVol("bracketLog", &trapBack1, dbmPeek4);
  GeoPhysVol* trapBackPhys = new GeoPhysVol(trapBackLog);

  CLHEP::HepRotation rmX90; 
  rmX90.rotateX(90.*CLHEP::deg); // rotate along x-axis by 90 degree
  double trapBackPos_Z = -telescopeZ/2. + bracketZ - brcktLockZ + ( (trapBackShortZ+trapBackY*tan(angle))/2. + trapBackY/2.*sin(trapBack_theta) - trapBackY*tan(trapBack_theta) );
  CLHEP::Hep3Vector trapBackPos(0.0, -telescopeY/2. + trapBackY/2. + safety, trapBackPos_Z + 3*safety);
  xform = new GeoTransform(HepGeom::Transform3D(rmX90,trapBackPos));
  tag = new GeoNameTag("trapBack");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(trapBackPhys);

  // bracket front top plate
  const GeoBox* brcktTopBlock = new GeoBox(bracketX/2., (bracketY - brcktSideBlockY)/2., brcktTopBlockZ/2.);
  const GeoMaterial* dbmPeekAluminium = mat_mgr->getMaterialForVolume("pix::DBMPeekAl",brcktTopBlock->volume());
  const GeoLogVol* brcktTopBlockLog = new GeoLogVol("bracketLog", brcktTopBlock, dbmPeekAluminium);
  GeoPhysVol* brcktTopBlockPhys = new GeoPhysVol(brcktTopBlockLog);

  CLHEP::Hep3Vector brcktTopBlockPos( 0., -telescopeY/2. + brcktSideBlockY + (bracketY - brcktSideBlockY)/2.+2*safety, -telescopeZ/2. + brcktTopBlockZ/2.);
  xform = new GeoTransform(HepGeom::Transform3D(rm,brcktTopBlockPos));
  tag = new GeoNameTag("brcktTopBlock");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(brcktTopBlockPhys);

  // bracket front side blocks
  const GeoBox* brcktSideBlock = new GeoBox(brcktSideBlockX/2., brcktSideBlockY/2., brcktTopBlockZ/2.);
  const GeoMaterial* dbmPeek2 = mat_mgr->getMaterialForVolume("pix::DBMPeek2",brcktSideBlock->volume());
  const GeoLogVol* brcktSideLog = new GeoLogVol("brcktSideLog", brcktSideBlock, dbmPeek2);
  GeoPhysVol* brcktSidePhys = new GeoPhysVol(brcktSideLog);

  CLHEP::Hep3Vector brcktSidePos1( bracketX/2. - brcktSideBlockX/2., -telescopeY/2. + brcktSideBlockY/2.+safety, -telescopeZ/2. + brcktTopBlockZ/2.);
  xform = new GeoTransform(HepGeom::Transform3D(rm,brcktSidePos1));
  tag = new GeoNameTag("brcktSideBlock");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(brcktSidePhys);

  CLHEP::Hep3Vector brcktSidePos2( -bracketX/2. + brcktSideBlockX/2., -telescopeY/2. + brcktSideBlockY/2. + safety, -telescopeZ/2. + brcktTopBlockZ/2.);
  xform = new GeoTransform(HepGeom::Transform3D(rm,brcktSidePos2));
  tag = new GeoNameTag("brcktSideBlock");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(brcktSidePhys);

  // bracket Locking block
  const GeoBox* brcktLock = new GeoBox(bracketX/2., brcktLockY/2.-safety, brcktLockZ/2.-safety);
  const GeoMaterial* dbmPeek3 = mat_mgr->getMaterialForVolume("pix::DBMPeek3",brcktLock->volume());
  const GeoLogVol* brcktLockLog = new GeoLogVol("bracketLog", brcktLock, dbmPeek3);
  GeoPhysVol* brcktLockPhys = new GeoPhysVol(brcktLockLog);

  CLHEP::Hep3Vector brcktLockPos( 0., -telescopeY/2. + trapBackY + brcktLockY/2. + 2*safety, -telescopeZ/2. + bracketZ - brcktLockZ/2. + safety);
  xform = new GeoTransform(HepGeom::Transform3D(rm,brcktLockPos));
  tag = new GeoNameTag("brcktLock");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(brcktLockPhys);

  // rectangular aluminium side plates

  const GeoBox* sidePlate = new GeoBox(coolingSidePlateX/2., coolingSidePlateY/2., coolingSidePlateZ/2.);
  const GeoMaterial* dbmAluminium2 = mat_mgr->getMaterialForVolume("pix::DBMAluminium2", sidePlate->volume());
  const GeoLogVol* sidePlateLog = new GeoLogVol("sidePlateLog", sidePlate, dbmAluminium2);
  GeoPhysVol* sidePlatePhys = new GeoPhysVol(sidePlateLog);

  double spAngle = angle + 17.57126*CLHEP::deg;
  double spRadius = 1/2. * sqrt(coolingSidePlateZ*coolingSidePlateZ + coolingSidePlateY*coolingSidePlateY);

  CLHEP::Hep3Vector sidePlatePos1( mainPlateX/2. + coolingSidePlateX/2. + 2*safety, - telescopeY/2. + spRadius * sin(spAngle), -telescopeZ/2. + layerUnitPos_Z - coolingSidePlatePos*cos(angle) + spRadius * cos(spAngle));
  xform = new GeoTransform(HepGeom::Transform3D(rmX10, sidePlatePos1));
  tag = new GeoNameTag("sidePlate");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(sidePlatePhys);

  CLHEP::Hep3Vector sidePlatePos2( -mainPlateX/2. - coolingSidePlateX/2. - 2*safety, - telescopeY/2. + spRadius * sin(spAngle), -telescopeZ/2. + layerUnitPos_Z - coolingSidePlatePos*cos(angle) + spRadius * cos(spAngle));
  xform = new GeoTransform(HepGeom::Transform3D(rmX10, sidePlatePos2));
  tag = new GeoNameTag("sidePlate");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(sidePlatePhys);

  //cooling plates next to the bracket unit
  const GeoTrap* coolingFin = new GeoTrap(coolingFinHeight/2., trapBack_theta, 90.0*CLHEP::deg, (coolingFinLongZ - coolingFinHeight*tan(angle))/2., coolingFinThick/2., coolingFinThick/2, 0.0, coolingFinLongZ/2., coolingFinThick/2., coolingFinThick/2., 0.0);
  const GeoMaterial* dbmAluminium3 = mat_mgr->getMaterialForVolume("pix::DBMAluminium3", coolingFin->volume());
  const GeoLogVol* finLog = new GeoLogVol("finLog", coolingFin, dbmAluminium3);
  GeoPhysVol* coolingFinPhys = new GeoPhysVol(finLog);

  CLHEP::Hep3Vector finPos1( mainPlateX/2. - coolingFinThick/2. + safety, -telescopeY/2. +  coolingFinHeight/2. + 4*safety, -telescopeZ/2. + coolingFinPos + 0.05*CLHEP::mm);
  xform = new GeoTransform(HepGeom::Transform3D(rmX90, finPos1));
  tag = new GeoNameTag("finPlate");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(coolingFinPhys);

  CLHEP::Hep3Vector finPos2( -mainPlateX/2. + coolingFinThick/2. - safety, -telescopeY/2. +  coolingFinHeight/2. + 4*safety, -telescopeZ/2. + coolingFinPos + 0.05*CLHEP::mm);
  xform = new GeoTransform(HepGeom::Transform3D(rmX90, finPos2));
  tag = new GeoNameTag("finPlate");
  telescopePhys->add(tag);
  telescopePhys->add(xform);
  telescopePhys->add(coolingFinPhys);


  return telescopePhys;

}

