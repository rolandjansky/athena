/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BLM_GeoModel/BLM_Module.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"

GeoPhysVol* BLM_Module::Build(const AbsMaterialManager* mat_mgr, const BLM_ModuleParameters* parameters, MsgStream* msg)
{
  double CuThick = 0.015;
  double LamelThick15 = 1;
  double LamelThick234 = 1.4;

  //module outside dimentions
  double ModWidth = BLM_Wall::s_width;
  double ModLength = BLM_Wall::s_holder_length;
  double ModHeight = BLM_Wall::s_holder_height + 10*CuThick + 2*LamelThick15 + 3*LamelThick234 +2;//2 stands for screw head dimensions
  double diamondThick = 0.5;
  double diamondSize = 8;

  //materials
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoMaterial* copper = mat_mgr->getMaterial("std::Copper");
  const GeoMaterial* g10 = mat_mgr->getMaterial("std::G10");
  const GeoMaterial* diamond = mat_mgr->getMaterial("pix::Diamond");
  if(diamond == NULL)
  {
  	diamond = mat_mgr->getMaterial("std::Carbon");
//  	if(msg)
//  		(*msg) << "BLM with graphite." << endreq; //commented out by D.Dobos on request by M.Elsing
  }
//  else
//  {
//  	if(msg)
//  		(*msg) << "BLM with diamond." << endreq; //commented out by D.Dobos on request by M.Elsing
//  }
  const GeoMaterial* stainless_steel = mat_mgr->getMaterial("std::Iron");
  const GeoMaterial* peek = mat_mgr->getMaterial("pix::Peek");
  if(peek == NULL)
  {     
  	if(msg)
  		(*msg) << "BLM _ PEEK _ MISSING." << endreq;
	GeoMaterial* peektmp = new GeoMaterial("PEEK", 1.3*CLHEP::gram/CLHEP::cm3);
	GeoElement* hydrogen = new GeoElement("Hydrogen", "H", 1.0, 1.010);
	GeoElement* oxygen = new GeoElement("Oxygen", "O", 8.0, 16.000);
	GeoElement* carbon = new GeoElement("Carbon", "C", 6.0, 12.010);
	peektmp->add(hydrogen, 0.042);
	peektmp->add(oxygen, 0.167);
	peektmp->add(carbon, 0.791);
	peektmp->lock();
	peek = peektmp;
  }
  //else 
  //	std::cout << "BLM _ PEEK _ FOUND" << std::endl;

  //PEEK

 /* //STAINLESS STEEL
  GeoMaterial* stainless_steel = new GeoMaterial("S_steel", 7.8*CLHEP::gram/CLHEP::cm3);
  GeoElement* iron = new GeoElement("Iron", "Fe", 26.0, 55.845);
  GeoElement* chromium = new GeoElement("Chromium", "Cr", 24.0, 51.996);
  stainless_steel->add(iron, 0.89);
  stainless_steel->add(chromium, 0.10);
  stainless_steel->add(carbon, 0.01);
  stainless_steel->lock();*/

  const GeoBox* blmModBox = new GeoBox(ModWidth/2, ModHeight/2, ModLength/2);
  const GeoLogVol* blmModLog=new GeoLogVol("blmModLog", blmModBox, air);
  GeoPhysVol* blmModPhys = new GeoPhysVol(blmModLog);

  BLM_Wall wall;

  //holder
  GeoPhysVol* holder = wall.BuildHolder(peek);
  CLHEP::Hep3Vector holderPos(0, BLM_Wall::s_holder_height-ModHeight/2-BLM_Wall::s_holder_thickness/2, 0);
  CLHEP::HepRotation holderRot;
  GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(holderRot, holderPos));
  GeoNameTag* tag = new GeoNameTag("Holder");
  blmModPhys->add(tag);
  blmModPhys->add(xform);
  blmModPhys->add(holder);

  //layer1
  GeoPhysVol* layer1a = wall.BuildLayerI(CuThick, copper, false);
  GeoPhysVol* layer1b = wall.BuildLayerI(LamelThick15, g10, false);
  GeoPhysVol* layer1c = wall.BuildLayerI(CuThick, copper, true);
  CLHEP::Hep3Vector layer1Posa(0, BLM_Wall::s_holder_height-ModHeight/2+CuThick/2, ModLength/2-BLM_Wall::s_extended_length/2);
  CLHEP::Hep3Vector layer1Posb(0, BLM_Wall::s_holder_height-ModHeight/2+CuThick+LamelThick15/2, ModLength/2-BLM_Wall::s_extended_length/2);
  CLHEP::Hep3Vector layer1Posc(0, BLM_Wall::s_holder_height-ModHeight/2+3*CuThick/2+LamelThick15, ModLength/2-BLM_Wall::s_extended_length/2);
  GeoTransform* yforma = new GeoTransform(HepGeom::Transform3D(holderRot, layer1Posa));
  GeoTransform* yformb = new GeoTransform(HepGeom::Transform3D(holderRot, layer1Posb));
  GeoTransform* yformc = new GeoTransform(HepGeom::Transform3D(holderRot, layer1Posc));
  tag = new GeoNameTag("Layer1");
  blmModPhys->add(tag);
  blmModPhys->add(yforma);
  blmModPhys->add(layer1a);
  blmModPhys->add(tag);
  blmModPhys->add(yformb);
  blmModPhys->add(layer1b);
  blmModPhys->add(tag);
  blmModPhys->add(yformc);
  blmModPhys->add(layer1c);

  //layer2
  GeoPhysVol* layer2a = wall.BuildLayerII(CuThick, copper);
  GeoPhysVol* layer2b = wall.BuildLayerII(LamelThick234, g10);
  GeoPhysVol* layer2c = wall.BuildLayerII(CuThick, copper);
  CLHEP::Hep3Vector layer2Posa(0, BLM_Wall::s_holder_height-ModHeight/2+5*CuThick/2+LamelThick15, ModLength/2-BLM_Wall::s_length/2);
  CLHEP::Hep3Vector layer2Posb(0, BLM_Wall::s_holder_height-ModHeight/2+3*CuThick+LamelThick15+LamelThick234/2, ModLength/2-BLM_Wall::s_length/2);
  CLHEP::Hep3Vector layer2Posc(0, BLM_Wall::s_holder_height-ModHeight/2+7*CuThick/2+LamelThick15+LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  yforma = new GeoTransform(HepGeom::Transform3D(holderRot, layer2Posa));
  yformb = new GeoTransform(HepGeom::Transform3D(holderRot, layer2Posb));
  yformc = new GeoTransform(HepGeom::Transform3D(holderRot, layer2Posc));
  tag = new GeoNameTag("Layer2");
  blmModPhys->add(tag);
  blmModPhys->add(yforma);
  blmModPhys->add(layer2a);
  blmModPhys->add(tag);
  blmModPhys->add(yformb);
  blmModPhys->add(layer2b);
  blmModPhys->add(tag);
  blmModPhys->add(yformc);
  blmModPhys->add(layer2c);

  //layer3
  GeoPhysVol* layer3a = wall.BuildLayerIII(CuThick, copper);
  GeoPhysVol* layer3b = wall.BuildLayerIII(LamelThick234, g10);
  GeoPhysVol* layer3c = wall.BuildLayerIII(CuThick, copper);
  CLHEP::Hep3Vector layer3Posa(0, BLM_Wall::s_holder_height-ModHeight/2+9*CuThick/2+LamelThick15+LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  CLHEP::Hep3Vector layer3Posb(0, BLM_Wall::s_holder_height-ModHeight/2+5*CuThick+LamelThick15+3*LamelThick234/2, ModLength/2-BLM_Wall::s_length/2);
  CLHEP::Hep3Vector layer3Posc(0, BLM_Wall::s_holder_height-ModHeight/2+11*CuThick/2+LamelThick15+2*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  yforma = new GeoTransform(HepGeom::Transform3D(holderRot, layer3Posa));
  yformb = new GeoTransform(HepGeom::Transform3D(holderRot, layer3Posb));
  yformc = new GeoTransform(HepGeom::Transform3D(holderRot, layer3Posc));
  tag = new GeoNameTag("Layer3");
  blmModPhys->add(tag);
  blmModPhys->add(yforma);
  blmModPhys->add(layer3a);
  blmModPhys->add(tag);
  blmModPhys->add(yformb);
  blmModPhys->add(layer3b);
  blmModPhys->add(tag);
  blmModPhys->add(yformc);
  blmModPhys->add(layer3c);

  //layer4
  GeoPhysVol* layer4a = wall.BuildLayerIV(CuThick, copper);
  GeoPhysVol* layer4b = wall.BuildLayerIV(LamelThick234, g10);
  GeoPhysVol* layer4c = wall.BuildLayerIV(CuThick, copper);
  CLHEP::Hep3Vector layer4Posa(0, BLM_Wall::s_holder_height-ModHeight/2+13*CuThick/2+LamelThick15+2*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  CLHEP::Hep3Vector layer4Posb(0, BLM_Wall::s_holder_height-ModHeight/2+7*CuThick+LamelThick15+5*LamelThick234/2, ModLength/2-BLM_Wall::s_length/2);
  CLHEP::Hep3Vector layer4Posc(0, BLM_Wall::s_holder_height-ModHeight/2+15*CuThick/2+LamelThick15+3*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  yforma = new GeoTransform(HepGeom::Transform3D(holderRot, layer4Posa));
  yformb = new GeoTransform(HepGeom::Transform3D(holderRot, layer4Posb));
  yformc = new GeoTransform(HepGeom::Transform3D(holderRot, layer4Posc));
  tag = new GeoNameTag("Layer4");
  blmModPhys->add(tag);
  blmModPhys->add(yforma);
  blmModPhys->add(layer4a);
  blmModPhys->add(tag);
  blmModPhys->add(yformb);
  blmModPhys->add(layer4b);
  blmModPhys->add(tag);
  blmModPhys->add(yformc);
  blmModPhys->add(layer4c);

  //layer5
  GeoPhysVol* layer5a = wall.BuildLayerV(CuThick, copper);
  GeoPhysVol* layer5b = wall.BuildLayerV(LamelThick15, g10);
  GeoPhysVol* layer5c = wall.BuildLayerV(CuThick, copper);
  CLHEP::Hep3Vector layer5Posa(0, BLM_Wall::s_holder_height-ModHeight/2+17*CuThick/2+LamelThick15+3*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  CLHEP::Hep3Vector layer5Posb(0, BLM_Wall::s_holder_height-ModHeight/2+9*CuThick+3*LamelThick15/2+3*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  CLHEP::Hep3Vector layer5Posc(0, BLM_Wall::s_holder_height-ModHeight/2+19*CuThick/2+2*LamelThick15+3*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  yforma = new GeoTransform(HepGeom::Transform3D(holderRot, layer5Posa));
  yformb = new GeoTransform(HepGeom::Transform3D(holderRot, layer5Posb));
  yformc = new GeoTransform(HepGeom::Transform3D(holderRot, layer5Posc));
  tag = new GeoNameTag("Layer5");
  blmModPhys->add(tag);
  blmModPhys->add(yforma);
  blmModPhys->add(layer5a);
  blmModPhys->add(tag);
  blmModPhys->add(yformb);
  blmModPhys->add(layer5b);
  blmModPhys->add(tag);
  blmModPhys->add(yformc);
  blmModPhys->add(layer5c);

  //screws
  GeoPhysVol* screw1 = wall.BuildScrew(10, stainless_steel);
  GeoPhysVol* screw2 = wall.BuildScrew(10, stainless_steel);
  GeoPhysVol* screw3 = wall.BuildScrew(10, stainless_steel);
  GeoPhysVol* screw4 = wall.BuildScrew(10, stainless_steel);
  GeoPhysVol* screw5 = wall.BuildScrew(10, stainless_steel);
  GeoPhysVol* screw6 = wall.BuildScrew(10, stainless_steel);
  GeoPhysVol* screw7 = wall.BuildScrew(BLM_Wall::s_holder_thickness, stainless_steel);
  GeoPhysVol* screw8 = wall.BuildScrew(BLM_Wall::s_holder_thickness, stainless_steel);
  CLHEP::HepRotation screwRot;
  screwRot.rotateX(90.*CLHEP::deg);
  CLHEP::HepRotation screwRot1;
  screwRot1.rotateX(180.*CLHEP::deg);
  CLHEP::Hep3Vector screwPos1(ModWidth/2-BLM_Wall::s_hole_position, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_hole_position);
  CLHEP::Hep3Vector screwPos2(BLM_Wall::s_hole_position-ModWidth/2, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_hole_position);
  CLHEP::Hep3Vector screwPos3(ModWidth/2-BLM_Wall::s_hole_position, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_length+BLM_Wall::s_hole_position);
  CLHEP::Hep3Vector screwPos4(BLM_Wall::s_hole_position-ModWidth/2, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_length+BLM_Wall::s_hole_position);
  CLHEP::Hep3Vector screwPos5(ModWidth/2-BLM_Wall::s_hole_position, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_extended_length+2.1);
  CLHEP::Hep3Vector screwPos6(BLM_Wall::s_hole_position-ModWidth/2, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_extended_length+2.1);
  CLHEP::Hep3Vector screwPos7(ModWidth/2-BLM_Wall::s_hole_position, 3.5-ModHeight/2, BLM_Wall::s_holder_thickness-ModLength/2+1);
  CLHEP::Hep3Vector screwPos8(BLM_Wall::s_hole_position-ModWidth/2, 3.5-ModHeight/2, BLM_Wall::s_holder_thickness-ModLength/2+1);
  GeoTransform* xform1 = new GeoTransform(HepGeom::Transform3D(screwRot, screwPos1));
  GeoTransform* xform2 = new GeoTransform(HepGeom::Transform3D(screwRot, screwPos2));
  GeoTransform* xform3 = new GeoTransform(HepGeom::Transform3D(screwRot, screwPos3));
  GeoTransform* xform4 = new GeoTransform(HepGeom::Transform3D(screwRot, screwPos4));
  GeoTransform* xform5 = new GeoTransform(HepGeom::Transform3D(screwRot, screwPos5));
  GeoTransform* xform6 = new GeoTransform(HepGeom::Transform3D(screwRot, screwPos6));
  GeoTransform* xform7 = new GeoTransform(HepGeom::Transform3D(screwRot1, screwPos7));
  GeoTransform* xform8 = new GeoTransform(HepGeom::Transform3D(screwRot1, screwPos8));
  tag = new GeoNameTag("Screw");
  blmModPhys->add(tag);
  blmModPhys->add(xform1);
  blmModPhys->add(screw1);
  blmModPhys->add(tag);
  blmModPhys->add(xform2);
  blmModPhys->add(screw2);
  blmModPhys->add(tag);
  blmModPhys->add(xform3);
  blmModPhys->add(screw3);
  blmModPhys->add(tag);
  blmModPhys->add(xform4);
  blmModPhys->add(screw4);
  blmModPhys->add(tag);
  blmModPhys->add(xform5);
  blmModPhys->add(screw5);
  blmModPhys->add(tag);
  blmModPhys->add(xform6);
  blmModPhys->add(screw6);
  blmModPhys->add(tag);
  blmModPhys->add(xform7);
  blmModPhys->add(screw7);
  blmModPhys->add(tag);
  blmModPhys->add(xform8);
  blmModPhys->add(screw8);

  //clamp
  GeoPhysVol* clamp = wall.BuildClamp(g10);
  CLHEP::Hep3Vector clampPos(0, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234-BLM_Wall::s_clamp_thickness/2, ModLength/2-BLM_Wall::s_extended_length+2.1+2.5-BLM_Wall::s_clamp_length/2);
  yforma = new GeoTransform(HepGeom::Transform3D(holderRot, clampPos));
  tag = new GeoNameTag("Clamp");
  blmModPhys->add(tag);
  blmModPhys->add(yforma);
  blmModPhys->add(clamp);

  //diamond//blmDiamondLog
  GeoPhysVol* DiamondVol = wall.BuildBlock(8, diamondThick, diamondSize, "blmDiamondLog", diamond);
  CLHEP::Hep3Vector DiamondPos(parameters->DiamondPosition_X(), BLM_Wall::s_holder_height-ModHeight/2+CuThick+LamelThick15+diamondThick/2, ModLength/2-diamondSize/2-5+parameters->DiamondPosition_Z());
  xform = new GeoTransform(HepGeom::Transform3D(holderRot,DiamondPos));
  tag = new GeoNameTag("Diamond");
  blmModPhys->add(tag);
  blmModPhys->add(new GeoIdentifierTag(2009));
  blmModPhys->add(xform);
  blmModPhys->add(DiamondVol);

  return blmModPhys;
}
