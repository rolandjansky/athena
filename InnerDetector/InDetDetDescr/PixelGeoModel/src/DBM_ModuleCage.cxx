/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/DBM_ModuleCage.h"
#include "PixelGeoModel/DBM_Module.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTube.h"

#include "GeoModelKernel/GeoPhysVol.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"

GeoVPhysVol* DBM_ModuleCage::Build() {

  // safety, to make sure volumes don't overlap
  double safety = 0.005*CLHEP::mm;

  // Telescope dimension
  double layerUnitY = gmt_mgr->DBMModuleCageY();
  double layerUnitZ = gmt_mgr->DBMModuleCageZ();

  // layer spacing
  double Zspacing = gmt_mgr->DBMSpacingZ();
  double Rspacing = gmt_mgr->DBMSpacingRadial();
  // gap between V-slide and first main plate
  double layer1Space = gmt_mgr->DBMSpace();

  // main plate, on which is mounted the sensor module
  double mainPlateX = gmt_mgr->DBMMainPlateX(); //dimension in x-direction or width
  double mainPlateY = gmt_mgr->DBMMainPlateY(); //y-direction or height
  double mainPlateZ = gmt_mgr->DBMMainPlateZ(); //z-direction or thickness
  double mPlateWindowX = gmt_mgr->DBMMPlateWindowWidth(); // window width in the main plate
  double mPlateWindowY = gmt_mgr->DBMMPlateWindowHeight(); // window height
  double mPlateWindowPos = gmt_mgr->DBMMPlateWindowPos(); // window position from bottom of the main plate

  // DBM module
  double diamondZ = gmt_mgr->DBMDiamondZ(); 
  double FEI4Z = gmt_mgr->DBMFEI4Z();
  double ceramicY = gmt_mgr->DBMCeramicY(); 
  double ceramicZ = gmt_mgr->DBMCeramicZ();
  double moduleAirGap = gmt_mgr->DBMAirGap();   // air gap between diamond and FE-I4 chip
  double kaptonZ = gmt_mgr->DBMKaptonZ();

  // flex support
  double flexSupportX = gmt_mgr->DBMFlexSupportX();
  double flexSupportY = gmt_mgr->DBMFlexSupportY();
  double flexSupportZ = gmt_mgr->DBMFlexSupportZ();
  double flexSupportOffset = gmt_mgr->DBMFlexSupportOffset();

  // rods
  double rodRadius = gmt_mgr->DBMRodRadius();
  double mPlateRod2RodY = gmt_mgr->DBMMPlateRod2RodY(); // distance between center of top and bottom rods
  double mPlateRod2RodX = gmt_mgr->DBMMPlateRod2RodX(); // distance between center of left and right rods

  // materials
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoMaterial* aluminium = mat_mgr->getMaterial("std::Aluminium");
  const GeoMaterial* DBMRod_mat = mat_mgr->getMaterial("pix::DBMRod");



  //**** 3-layers unit volume forming the module cage
  const GeoBox* containerBox = new GeoBox(mainPlateX/2. + safety, layerUnitY/2. + safety, layerUnitZ/2.);
  const GeoLogVol* containerLog = new GeoLogVol("dbmTelescopeLog", containerBox, air);
  GeoPhysVol* containerPhys = new GeoPhysVol(containerLog);

  //**** blocks to form the main plate
  
  const GeoBox* mPSide = new GeoBox((mainPlateX - mPlateWindowX)/4., mainPlateY/2., mainPlateZ/2.);
  const GeoMaterial* dbmAluminium1 = mat_mgr->getMaterialForVolume("pix::DBMAluminium1", mPSide->volume());
  const GeoLogVol* mPSideLog = new GeoLogVol("dbmWallLogPlt", mPSide, dbmAluminium1);
  GeoPhysVol* mPSidePhys = new GeoPhysVol(mPSideLog);

  const GeoBox* mPCenterBot = new GeoBox(mPlateWindowX/2., mPlateWindowPos/2., mainPlateZ/2.);
  const GeoLogVol* mPCenterBotLog = new GeoLogVol("dbmWallLogPlt", mPCenterBot, aluminium);
  GeoPhysVol* mPCenterBotPhys = new GeoPhysVol(mPCenterBotLog);

  const GeoBox* mPCenterTop = new GeoBox(mPlateWindowX/2., (mainPlateY - mPlateWindowY - mPlateWindowPos)/2., mainPlateZ/2.);
  const GeoLogVol* mPCenterTopLog = new GeoLogVol("dbmWallLogPlt", mPCenterTop, aluminium);
  GeoPhysVol* mPCenterTopPhys = new GeoPhysVol(mPCenterTopLog);

  //**** telescope rods

  // rod in front of the first layer
  const GeoTube* rodA = new GeoTube(0., rodRadius, layer1Space/2. - safety);
  const GeoLogVol* rodALog = new GeoLogVol("dbmWallLogPlt", rodA, DBMRod_mat);
  GeoPhysVol* rodAPhys = new GeoPhysVol(rodALog);

  // top rods
  const GeoTube* topRod = new GeoTube(0., rodRadius, (Zspacing - mainPlateZ - flexSupportZ)/2. - safety);
  const GeoLogVol* topRodLog = new GeoLogVol("dbmWallLogPlt", topRod, DBMRod_mat);
  GeoPhysVol* topRodPhys = new GeoPhysVol(topRodLog);

  // Bottom rods
  const GeoTube* botRod = new GeoTube(0., rodRadius, (Zspacing - mainPlateZ)/2. - safety);
  const GeoLogVol* botRodLog = new GeoLogVol("dbmWallLogPlt", botRod, DBMRod_mat);
  GeoPhysVol* botRodPhys = new GeoPhysVol(botRodLog);

  //**** add volumes to the telescope box

  // position of the center of the main plates with respect to the front bottom telescope box
  double mainPlatePosZ[3];  // Z coordinates
  mainPlatePosZ[0] = -layerUnitZ/2. + layer1Space + mainPlateZ/2.;
  mainPlatePosZ[1] = -layerUnitZ/2. + layer1Space + Zspacing + mainPlateZ/2.;
  mainPlatePosZ[2] = -layerUnitZ/2. + layer1Space + 2.*Zspacing + mainPlateZ/2.;
  // Y coordinate
  double mainPlatePosY = -layerUnitY/2. + mainPlateY/2.;

  CLHEP::HepRotation rm;
 
  // store initial value Eta value 
  int tempLD = gmt_mgr->GetLD();

  // strings for volume tag
  std::string tag_mainPlate[3] = {"mainPlate1", "mainPlate2", "mainPlate3"};
  std::string tag_flex[3] = {"flexSupport1", "flexSupport2", "flexSupport3"};
  std::string tag_module[3] = {"module1", "module2", "module3"};
  std::string tag_rod[3] = {"rod1", "rod2", "rod3"};

  for (int i = 0; i < 3; i++) {

    //**** Add volume to form the module cage

    // add sensor module

    // set telescope layer number 
    gmt_mgr->SetCurrentLD(i);

    DBM_Module module;
    GeoVPhysVol* modulePhys = module.Build();

    Rspacing = gmt_mgr->DBMSpacingRadial();
    CLHEP::Hep3Vector modulePos(0, -layerUnitY/2. + Rspacing + ceramicY/2., mainPlatePosZ[i]-mainPlateZ/2.-(ceramicZ+FEI4Z+moduleAirGap+diamondZ + kaptonZ)/2.-2*safety);
    GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(rm,modulePos));
    GeoNameTag* tag = new GeoNameTag(tag_module[i].c_str());
    containerPhys->add(tag);
    containerPhys->add(new GeoIdentifierTag(i));
    containerPhys->add(xform);
    containerPhys->add(modulePhys);

    // add main plate

    CLHEP::Hep3Vector mPSideAPos((mainPlateX+mPlateWindowX)/4.+safety/2., mainPlatePosY, mainPlatePosZ[i]);
    xform = new GeoTransform(HepGeom::Transform3D(rm,mPSideAPos));
    tag = new GeoNameTag(tag_mainPlate[i].c_str());
    containerPhys->add(tag);
    containerPhys->add(xform);
    containerPhys->add(mPSidePhys);

    CLHEP::Hep3Vector mPSideBPos(-(mainPlateX+mPlateWindowX)/4.-safety/2., mainPlatePosY, mainPlatePosZ[i]);
    xform = new GeoTransform(HepGeom::Transform3D(rm,mPSideBPos));
    containerPhys->add(tag);
    containerPhys->add(xform);
    containerPhys->add(mPSidePhys);

    CLHEP::Hep3Vector mPBotPos(0.0, (-layerUnitY + mPlateWindowPos)/2.-safety/2., mainPlatePosZ[i]);
    xform = new GeoTransform(HepGeom::Transform3D(rm,mPBotPos));
    containerPhys->add(tag);
    containerPhys->add(xform);
    containerPhys->add(mPCenterBotPhys);

    CLHEP::Hep3Vector mPTopPos(0.0, (-layerUnitY + mainPlateY + mPlateWindowY + mPlateWindowPos)/2.+safety/2., mainPlatePosZ[i]);
    xform = new GeoTransform(HepGeom::Transform3D(rm,mPTopPos));
    containerPhys->add(tag);
    containerPhys->add(xform);
    containerPhys->add(mPCenterTopPhys);


    //**** component of the flex support
    // Additional material in the 3rd fex support from screws and the 4 rods 
    const GeoBox* flexSupp = new GeoBox(flexSupportX/2., flexSupportY/2., flexSupportZ/2.);
   
    // get mass of flex support material
    std::string flexSupp_matName = "";
    if (i == 0) {
      flexSupp_matName = "pix::DBMPeek5";
    } else if (i == 1) {
      flexSupp_matName = "pix::DBMPeek6";
    } else if (i == 2) {
      flexSupp_matName = "pix::DBMPeek7";
    }
    const GeoMaterial* dbmFlexSupp_mat = mat_mgr->getMaterialForVolume(flexSupp_matName.c_str(), flexSupp->volume());

    const GeoLogVol* flexSuppLog = new GeoLogVol("dbmWallLog", flexSupp, dbmFlexSupp_mat);
    GeoPhysVol* flexSuppPhys = new GeoPhysVol(flexSuppLog);

    // Y position of flex support block
    double flexSuppPosY = mainPlatePosY + mPlateRod2RodY/2. - flexSupportY/2. + flexSupportOffset; 
    // Z position of flex support blocks
    double flexSuppPosZ = mainPlatePosZ[i] + mainPlateZ/2. + flexSupportZ/2. + safety;
        
    //**** add flex support

    CLHEP::Hep3Vector flexSuppPos(0.0, flexSuppPosY, flexSuppPosZ);
    xform = new GeoTransform(HepGeom::Transform3D(rm,flexSuppPos));
    tag = new GeoNameTag(tag_flex[i].c_str());
    containerPhys->add(tag);
    containerPhys->add(xform);
    containerPhys->add(flexSuppPhys);

   //**** add rod

    // Y position of rods
    double topRodPosY = mainPlatePosY + mPlateRod2RodY/2.; 
    double botRodPosY = mainPlatePosY - mPlateRod2RodY/2.;

    if (gmt_mgr->GetLD() == 0) { //first layer

      double rodPosZ = mainPlatePosZ[i] - mainPlateZ/2.0 - layer1Space/2.; // Z position of rod

      CLHEP::Hep3Vector rodAPos(mPlateRod2RodX/2., topRodPosY, rodPosZ);
      xform = new GeoTransform(HepGeom::Transform3D(rm,rodAPos));
      tag = new GeoNameTag(tag_rod[i].c_str());
      containerPhys->add(tag);
      containerPhys->add(xform);
      containerPhys->add(rodAPhys);

      CLHEP::Hep3Vector rodBPos(-mPlateRod2RodX/2., topRodPosY, rodPosZ);
      xform = new GeoTransform(HepGeom::Transform3D(rm,rodBPos));
      containerPhys->add(tag);
      containerPhys->add(xform);
      containerPhys->add(rodAPhys);

      CLHEP::Hep3Vector rodCPos(mPlateRod2RodX/2., botRodPosY, rodPosZ);
      xform = new GeoTransform(HepGeom::Transform3D(rm,rodCPos));
      containerPhys->add(tag);
      containerPhys->add(xform);
      containerPhys->add(rodAPhys);

      CLHEP::Hep3Vector rodDPos(-mPlateRod2RodX/2., botRodPosY, rodPosZ);
      xform = new GeoTransform(HepGeom::Transform3D(rm,rodDPos));
      containerPhys->add(tag);
      containerPhys->add(xform);
      containerPhys->add(rodAPhys);

    } else if (gmt_mgr->GetLD() > 0 && gmt_mgr->GetLD() < 3) { //second and third layer

      //Z position of rods
      double topRodPosZ = mainPlatePosZ[i] - mainPlateZ/2. - (Zspacing - mainPlateZ - flexSupportZ)/2.;
      double botRodPosZ = mainPlatePosZ[i] - mainPlateZ/2. - (Zspacing - mainPlateZ)/2.;

      CLHEP::Hep3Vector rodAPos(mPlateRod2RodX/2., topRodPosY, topRodPosZ);
      xform = new GeoTransform(HepGeom::Transform3D(rm,rodAPos));
      tag = new GeoNameTag(tag_rod[i].c_str());
      containerPhys->add(tag);
      containerPhys->add(xform);
      containerPhys->add(topRodPhys);

      CLHEP::Hep3Vector rodBPos(-mPlateRod2RodX/2., topRodPosY, topRodPosZ);
      xform = new GeoTransform(HepGeom::Transform3D(rm,rodBPos));
      containerPhys->add(tag);
      containerPhys->add(xform);
      containerPhys->add(topRodPhys);

      CLHEP::Hep3Vector rodCPos(mPlateRod2RodX/2., botRodPosY, botRodPosZ);
      xform = new GeoTransform(HepGeom::Transform3D(rm,rodCPos));
      containerPhys->add(tag);
      containerPhys->add(xform);
      containerPhys->add(botRodPhys);

      CLHEP::Hep3Vector rodDPos(-mPlateRod2RodX/2., botRodPosY, botRodPosZ);
      xform = new GeoTransform(HepGeom::Transform3D(rm,rodDPos));
      containerPhys->add(tag);
      containerPhys->add(xform);
      containerPhys->add(botRodPhys);

    } else 
      gmt_mgr->msg(MSG::WARNING) << "Module cage   invalid layer value: " << gmt_mgr->GetLD() << endmsg; 

  
  }

  // restore Eta value
  gmt_mgr->SetCurrentLD(tempLD);

  return containerPhys;
}

