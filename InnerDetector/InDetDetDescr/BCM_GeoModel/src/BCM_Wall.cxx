/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BCM_GeoModel/BCM_Wall.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"

GeoPhysVol* BCM_Wall::Build(double halfX, double halfY, double thick, std::string logname,const  GeoMaterial* material)
{
  const GeoBox* bcmWallBox = new GeoBox(halfX, halfY, thick/2);
  const GeoLogVol* bcmWallLog = new GeoLogVol(logname, bcmWallBox, material);
  GeoPhysVol* bcmWallPhys = new GeoPhysVol(bcmWallLog);
  
  return bcmWallPhys;
}

GeoPhysVol* BCM_Wall::Build(double halfX, double halfY, double thick_one, double thick_two, const GeoMaterial* material_one, const GeoMaterial* material_two, const AbsMaterialManager* mat_mgr)
{
  double wallthick = thick_one + thick_two;
  
  //outside box
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoBox* bcmWallBox = new GeoBox(halfX, halfY, wallthick/2);
  const GeoLogVol* bcmWallLog=new GeoLogVol("bcmWallLog", bcmWallBox, air);
  GeoPhysVol* bcmWallPhys = new GeoPhysVol(bcmWallLog);

  //adding first layer (material one)
  
  const GeoBox* bcmWallOneBox = new GeoBox(halfX, halfY, thick_one/2);
  const GeoLogVol* bcmWallOneLog = new GeoLogVol("bcmWallLog", bcmWallOneBox, material_one);
  GeoPhysVol* bcmWallOnePhys = new GeoPhysVol(bcmWallOneLog);
 
  CLHEP::Hep3Vector WallOnePos(0,0,thick_one/2 - wallthick/2);
  CLHEP::HepRotation rm;
  GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(rm,WallOnePos));
  bcmWallPhys->add(xform);
  bcmWallPhys->add(bcmWallOnePhys);
 
  //adding second layer (material two)

  const GeoBox* bcmWallTwoBox = new GeoBox(halfX, halfY, thick_two/2);
  const GeoLogVol* bcmWallTwoLog = new GeoLogVol("bcmWallLog", bcmWallTwoBox, material_two);
  GeoPhysVol* bcmWallTwoPhys = new GeoPhysVol(bcmWallTwoLog);

  CLHEP::Hep3Vector WallTwoPos(0,0,wallthick/2 - thick_two/2);
  xform = new GeoTransform(HepGeom::Transform3D(rm,WallTwoPos));
  bcmWallPhys->add(xform);
  bcmWallPhys->add(bcmWallTwoPhys);

  return bcmWallPhys;
}

GeoPhysVol* BCM_Wall::Build(double halfX, double halfY, double thick_one, double thick_two, double thick_three, const GeoMaterial* material_one, const GeoMaterial* material_two, const GeoMaterial* material_three, const AbsMaterialManager* mat_mgr)
{
  double wallthick = thick_one + thick_two + thick_three;
  
  //outside box
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoBox* bcmWallBox = new GeoBox(halfX, halfY, wallthick/2);
  const GeoLogVol* bcmWallLog=new GeoLogVol("bcmWallLog", bcmWallBox, air);
  GeoPhysVol* bcmWallPhys = new GeoPhysVol(bcmWallLog);

  //adding first layer (material one)
  
  const GeoBox* bcmWallOneBox = new GeoBox(halfX, halfY, thick_one/2);
  const GeoLogVol* bcmWallOneLog = new GeoLogVol("bcmWallLog", bcmWallOneBox, material_one);
  GeoPhysVol* bcmWallOnePhys = new GeoPhysVol(bcmWallOneLog);
 
  CLHEP::Hep3Vector WallOnePos(0,0,thick_one/2 - wallthick/2);
  CLHEP::HepRotation rm;
  GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(rm,WallOnePos));
  bcmWallPhys->add(xform);
  bcmWallPhys->add(bcmWallOnePhys);
 
  //adding second layer (material two)

  const GeoBox* bcmWallTwoBox = new GeoBox(halfX, halfY, thick_two/2);
  const GeoLogVol* bcmWallTwoLog = new GeoLogVol("bcmWallLog", bcmWallTwoBox, material_two);
  GeoPhysVol* bcmWallTwoPhys = new GeoPhysVol(bcmWallTwoLog);

  CLHEP::Hep3Vector WallTwoPos(0,0,thick_one -wallthick/2 + thick_two/2);
  xform = new GeoTransform(HepGeom::Transform3D(rm,WallTwoPos));
  bcmWallPhys->add(xform);
  bcmWallPhys->add(bcmWallTwoPhys);

  //adding third layer (material three)

  const GeoBox* bcmWallThreeBox = new GeoBox(halfX, halfY, thick_three/2);
  const GeoLogVol* bcmWallThreeLog = new GeoLogVol("bcmWallLog", bcmWallThreeBox, material_three);
  GeoPhysVol* bcmWallThreePhys = new GeoPhysVol(bcmWallThreeLog);

  CLHEP::Hep3Vector WallThreePos(0,0,wallthick/2 - thick_three/2);
  xform = new GeoTransform(HepGeom::Transform3D(rm,WallThreePos));
  bcmWallPhys->add(xform);
  bcmWallPhys->add(bcmWallThreePhys);

  return bcmWallPhys;
}
