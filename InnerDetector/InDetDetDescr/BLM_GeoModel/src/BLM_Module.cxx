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

#include "GeoModelKernel/Units.h"
#include "GaudiKernel/SystemOfUnits.h"

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
//  		(*msg) << "BLM with graphite." << endmsg; //commented out by D.Dobos on request by M.Elsing
  }
//  else
//  {
//  	if(msg)
//  		(*msg) << "BLM with diamond." << endmsg; //commented out by D.Dobos on request by M.Elsing
//  }
  const GeoMaterial* stainless_steel = mat_mgr->getMaterial("std::Iron");
  const GeoMaterial* peek = mat_mgr->getMaterial("pix::Peek");
  if(peek == NULL)
  {     
  	if(msg)
  		(*msg) << "BLM _ PEEK _ MISSING." << endmsg;
	GeoMaterial* peektmp = new GeoMaterial("PEEK", 1.3*GeoModelKernelUnits::gram/Gaudi::Units::cm3);
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
  GeoMaterial* stainless_steel = new GeoMaterial("S_steel", 7.8*GeoModelKernelUnits::gram/GeoModelKernelUnits::cm3);
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
  GeoTrf::Translate3D holderPos(0, BLM_Wall::s_holder_height-ModHeight/2-BLM_Wall::s_holder_thickness/2, 0);
  GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(holderPos));
  GeoNameTag* tag = new GeoNameTag("Holder");
  blmModPhys->add(tag);
  blmModPhys->add(xform);
  blmModPhys->add(holder);

  //layer1
  GeoPhysVol* layer1a = wall.BuildLayerI(CuThick, copper, false);
  GeoPhysVol* layer1b = wall.BuildLayerI(LamelThick15, g10, false);
  GeoPhysVol* layer1c = wall.BuildLayerI(CuThick, copper, true);
  GeoTrf::Translate3D layer1Posa(0, BLM_Wall::s_holder_height-ModHeight/2+CuThick/2, ModLength/2-BLM_Wall::s_extended_length/2);
  GeoTrf::Translate3D layer1Posb(0, BLM_Wall::s_holder_height-ModHeight/2+CuThick+LamelThick15/2, ModLength/2-BLM_Wall::s_extended_length/2);
  GeoTrf::Translate3D layer1Posc(0, BLM_Wall::s_holder_height-ModHeight/2+3*CuThick/2+LamelThick15, ModLength/2-BLM_Wall::s_extended_length/2);
  GeoTransform* yforma = new GeoTransform(GeoTrf::Transform3D(layer1Posa));
  GeoTransform* yformb = new GeoTransform(GeoTrf::Transform3D(layer1Posb));
  GeoTransform* yformc = new GeoTransform(GeoTrf::Transform3D(layer1Posc));
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
  GeoTrf::Translate3D layer2Posa(0, BLM_Wall::s_holder_height-ModHeight/2+5*CuThick/2+LamelThick15, ModLength/2-BLM_Wall::s_length/2);
  GeoTrf::Translate3D layer2Posb(0, BLM_Wall::s_holder_height-ModHeight/2+3*CuThick+LamelThick15+LamelThick234/2, ModLength/2-BLM_Wall::s_length/2);
  GeoTrf::Translate3D layer2Posc(0, BLM_Wall::s_holder_height-ModHeight/2+7*CuThick/2+LamelThick15+LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  yforma = new GeoTransform(GeoTrf::Transform3D(layer2Posa));
  yformb = new GeoTransform(GeoTrf::Transform3D(layer2Posb));
  yformc = new GeoTransform(GeoTrf::Transform3D(layer2Posc));
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
  GeoTrf::Translate3D layer3Posa(0, BLM_Wall::s_holder_height-ModHeight/2+9*CuThick/2+LamelThick15+LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  GeoTrf::Translate3D layer3Posb(0, BLM_Wall::s_holder_height-ModHeight/2+5*CuThick+LamelThick15+3*LamelThick234/2, ModLength/2-BLM_Wall::s_length/2);
  GeoTrf::Translate3D layer3Posc(0, BLM_Wall::s_holder_height-ModHeight/2+11*CuThick/2+LamelThick15+2*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  yforma = new GeoTransform(GeoTrf::Transform3D(layer3Posa));
  yformb = new GeoTransform(GeoTrf::Transform3D(layer3Posb));
  yformc = new GeoTransform(GeoTrf::Transform3D(layer3Posc));
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
  GeoTrf::Translate3D layer4Posa(0, BLM_Wall::s_holder_height-ModHeight/2+13*CuThick/2+LamelThick15+2*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  GeoTrf::Translate3D layer4Posb(0, BLM_Wall::s_holder_height-ModHeight/2+7*CuThick+LamelThick15+5*LamelThick234/2, ModLength/2-BLM_Wall::s_length/2);
  GeoTrf::Translate3D layer4Posc(0, BLM_Wall::s_holder_height-ModHeight/2+15*CuThick/2+LamelThick15+3*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  yforma = new GeoTransform(GeoTrf::Transform3D(layer4Posa));
  yformb = new GeoTransform(GeoTrf::Transform3D(layer4Posb));
  yformc = new GeoTransform(GeoTrf::Transform3D(layer4Posc));
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
  GeoTrf::Translate3D layer5Posa(0, BLM_Wall::s_holder_height-ModHeight/2+17*CuThick/2+LamelThick15+3*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  GeoTrf::Translate3D layer5Posb(0, BLM_Wall::s_holder_height-ModHeight/2+9*CuThick+3*LamelThick15/2+3*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  GeoTrf::Translate3D layer5Posc(0, BLM_Wall::s_holder_height-ModHeight/2+19*CuThick/2+2*LamelThick15+3*LamelThick234, ModLength/2-BLM_Wall::s_length/2);
  yforma = new GeoTransform(GeoTrf::Transform3D(layer5Posa));
  yformb = new GeoTransform(GeoTrf::Transform3D(layer5Posb));
  yformc = new GeoTransform(GeoTrf::Transform3D(layer5Posc));
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
  GeoTrf::RotateX3D screwRot(90.*Gaudi::Units::deg);
  GeoTrf::RotateX3D screwRot1(180.*Gaudi::Units::deg);  
  GeoTrf::Translation3D screwPos1(ModWidth/2-BLM_Wall::s_hole_position, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_hole_position);
  GeoTrf::Translation3D screwPos2(BLM_Wall::s_hole_position-ModWidth/2, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_hole_position);
  GeoTrf::Translation3D screwPos3(ModWidth/2-BLM_Wall::s_hole_position, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_length+BLM_Wall::s_hole_position);
  GeoTrf::Translation3D screwPos4(BLM_Wall::s_hole_position-ModWidth/2, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_length+BLM_Wall::s_hole_position);
  GeoTrf::Translation3D screwPos5(ModWidth/2-BLM_Wall::s_hole_position, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_extended_length+2.1);
  GeoTrf::Translation3D screwPos6(BLM_Wall::s_hole_position-ModWidth/2, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234+1, ModLength/2-BLM_Wall::s_extended_length+2.1);
  GeoTrf::Translation3D screwPos7(ModWidth/2-BLM_Wall::s_hole_position, 3.5-ModHeight/2, BLM_Wall::s_holder_thickness-ModLength/2+1);
  GeoTrf::Translation3D screwPos8(BLM_Wall::s_hole_position-ModWidth/2, 3.5-ModHeight/2, BLM_Wall::s_holder_thickness-ModLength/2+1);
  GeoTransform* xform1 = new GeoTransform(GeoTrf::Transform3D(screwPos1*screwRot));
  GeoTransform* xform2 = new GeoTransform(GeoTrf::Transform3D(screwPos2*screwRot));
  GeoTransform* xform3 = new GeoTransform(GeoTrf::Transform3D(screwPos3*screwRot));
  GeoTransform* xform4 = new GeoTransform(GeoTrf::Transform3D(screwPos4*screwRot));
  GeoTransform* xform5 = new GeoTransform(GeoTrf::Transform3D(screwPos5*screwRot));
  GeoTransform* xform6 = new GeoTransform(GeoTrf::Transform3D(screwPos6*screwRot));
  GeoTransform* xform7 = new GeoTransform(GeoTrf::Transform3D(screwPos7*screwRot1));
  GeoTransform* xform8 = new GeoTransform(GeoTrf::Transform3D(screwPos8*screwRot1));
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
  GeoTrf::Translate3D clampPos(0, BLM_Wall::s_holder_height-ModHeight/2+10*CuThick+2*LamelThick15+3*LamelThick234-BLM_Wall::s_clamp_thickness/2, ModLength/2-BLM_Wall::s_extended_length+2.1+2.5-BLM_Wall::s_clamp_length/2);
  yforma = new GeoTransform(GeoTrf::Transform3D(clampPos));
  tag = new GeoNameTag("Clamp");
  blmModPhys->add(tag);
  blmModPhys->add(yforma);
  blmModPhys->add(clamp);

  //diamond//blmDiamondLog
  GeoPhysVol* DiamondVol = wall.BuildBlock(8, diamondThick, diamondSize, "blmDiamondLog", diamond);
  GeoTrf::Translate3D DiamondPos(parameters->DiamondPosition_X(), BLM_Wall::s_holder_height-ModHeight/2+CuThick+LamelThick15+diamondThick/2, ModLength/2-diamondSize/2-5+parameters->DiamondPosition_Z());
  xform = new GeoTransform(GeoTrf::Transform3D(DiamondPos));
  tag = new GeoNameTag("Diamond");
  blmModPhys->add(tag);
  blmModPhys->add(new GeoIdentifierTag(2009));
  blmModPhys->add(xform);
  blmModPhys->add(DiamondVol);

  return blmModPhys;
}
