/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BCM_GeoModel/BCM_Module.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h" 

GeoPhysVol* BCM_Module::Build(const AbsMaterialManager* mat_mgr, const BCM_ModuleParameters* parameters, MsgStream* /*msg*/)
{
  //module outside dimensions
  
  double ModLength = 132.969 - 0.4 + parameters->BackWallThickness();
  double ModWidth = 33.02;
  double ModHeight = 21.717;
  double ModTailHeight = 7.747;
  double ModHeadLength = 49.657;

  double CuThick = 0.035;
  double G10Thick = 0.4;
  double WallThick = CuThick + G10Thick; //caution, CuThick, G10Thick, WallThick only valid for outside wall
  double ReducedModWidth = ModWidth - 2 * WallThick;
  double MainWallCuThick = 0.035;
  double MainWallBackCuThick = 0.035;
  double MainWallG10Thick = 1.4;
  double MainWallThick = MainWallCuThick + MainWallG10Thick;

  double LamelHoffset = parameters->LamelOffset_H();//34.860;//distance from center of lamel H to edge of module
  double LamelIoffset = parameters->LamelOffset_I();//29.509;//distance from center of lamel I to edge of module
  double LamelJoffset = parameters->LamelOffset_J();//20.205;//diatance form center of lamel J to edge of module
  double LamelG10Thick = 1.4;
  double LamelCuThick = 0.035;
  double LamelHThick = LamelG10Thick + 2*LamelCuThick;

  double diamondThick = 0.5;
  double diamondSize = 10;
  double safety = 0.001;  
  //outside box

  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoMaterial* ether = mat_mgr->getMaterial("special::Ether");
  const GeoMaterial* copper = mat_mgr->getMaterial("std::Copper");
  const GeoMaterial* g10 = mat_mgr->getMaterial("std::G10");
  //first try the Diamond
  const GeoMaterial* diamond = mat_mgr->getMaterial("pix::Diamond");
  if(diamond == NULL)
  {
  	diamond = mat_mgr->getMaterial("std::Carbon");
//  	if(msg)
//  		(*msg) << "BCM with graphite." << endreq;   //commented out by D.Dobos on request by M.Elsing
  }
//  else
//  {
//  	if(msg)
//  		(*msg) << "BCM with diamond." << endreq;   //commented out by D.Dobos on request by M.Elsing
//  }


//  RUN2 IBL/pixel/DBM/beam pipe geometry upgrade
//
//  Complex shape to avoid overlap with IST
//     1 - keep the default encompassing box (as intermediate volume), but change material to ether  
//          . box is not built by Geant4
//          . default volume and positions of sub-volumes are not modified 
//     2 - define a new complex encompassing box to avoid overlap with IST  
//          . add the box defined in 1. as a child volume 
//

// ----------------------------------------------------------------------------------
// New complex envelope volume defined to avoid overlap with IST
// ----------------------------------------------------------------------------------

  GeoSimplePolygonBrep* complexEnvShape = new GeoSimplePolygonBrep(ModWidth/2+safety);
  std::vector<double> xShape, yShape;

  double width = ModHeight*.5+safety;
  double length = ModLength*.5+safety;
  xShape.push_back(length); yShape.push_back(width);
  xShape.push_back(-length); yShape.push_back(width);
  xShape.push_back(-length); yShape.push_back(-width+10.);
  xShape.push_back(-length+10.); yShape.push_back(-width);
  xShape.push_back(length); yShape.push_back(-width);
  
  for(unsigned int iPt=0; iPt<xShape.size(); iPt++)
      complexEnvShape->addVertex(xShape[iPt],yShape[iPt]);

  const GeoLogVol* bcmModLog=new GeoLogVol("bcmModLog", complexEnvShape, air);
  GeoPhysVol* bcmModPhys = new GeoPhysVol(bcmModLog);


// ----------------------------------------------------------------------------------
// Intermediate Ether envelope volume (same as the one defined in previous release)
// ----------------------------------------------------------------------------------

  const GeoBox* bcmModBox = new GeoBox(ModHeight/2+safety, ModWidth/2+safety, ModLength/2+safety);
  const GeoLogVol* env_bcmModLog=new GeoLogVol("EnvBcmModLog", bcmModBox, ether);
  GeoPhysVol* env_bcmModPhys = new GeoPhysVol(env_bcmModLog);


  BCM_Wall wall;
  
  //wall C  
  GeoPhysVol* WallC = wall.Build(ModLength/2, ReducedModWidth/2, G10Thick, CuThick, g10, copper, mat_mgr);
		
  CLHEP::Hep3Vector WallCPos(ModHeight/2 - WallThick/2, 0, 0);
  CLHEP::HepRotation rmC;
  rmC.rotateY(90.*CLHEP::deg);
  GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(rmC,WallCPos));
  GeoNameTag* tag = new GeoNameTag("Wall C");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallC);

  //wall D
  GeoPhysVol* WallD = wall.Build((ModTailHeight - WallThick - MainWallThick)/2, ReducedModWidth/2, CuThick, G10Thick, copper, g10, mat_mgr);
  
  CLHEP::Hep3Vector WallDPos(ModHeight/2 - WallThick - (ModTailHeight - WallThick - MainWallThick)/2, 0, WallThick/2 - ModLength/2);
  CLHEP::HepRotation rmD;
  xform = new GeoTransform(HepGeom::Transform3D(rmD,WallDPos));
  tag = new GeoNameTag("Wall D");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallD);

  //wall H
  GeoPhysVol* WallH = wall.Build((ModTailHeight - WallThick - MainWallThick)/2, ReducedModWidth/2, LamelCuThick, LamelG10Thick, LamelCuThick, copper, g10, copper, mat_mgr);
  
  CLHEP::Hep3Vector WallHPos(ModHeight/2 -WallThick - (ModTailHeight - WallThick - MainWallThick)/2, 0, ModLength/2 - LamelHoffset);
  xform = new GeoTransform(HepGeom::Transform3D(rmD,WallHPos));
  tag = new GeoNameTag("Wall H");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallH);

  //wall J
  GeoPhysVol* WallJ = wall.Build((ModTailHeight - WallThick - MainWallThick - MainWallBackCuThick)/2, ReducedModWidth/2, LamelCuThick, LamelG10Thick, LamelCuThick, copper, g10, copper, mat_mgr);
		
  CLHEP::Hep3Vector WallJPos(ModHeight/2 -WallThick - (ModTailHeight - WallThick - MainWallThick - MainWallBackCuThick)/2, 0, ModLength/2 - LamelJoffset);
  xform = new GeoTransform(HepGeom::Transform3D(rmD,WallJPos));
  tag = new GeoNameTag("Wall J");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallJ);

  //wall E //if changing thickness of E one must also change length and position of MainWall and wallK (extra copper)
  GeoPhysVol* WallE = wall.Build(ModHeight/2 - WallThick, ReducedModWidth/2, parameters->BackWallThickness(), CuThick, g10, copper, mat_mgr);
		
  CLHEP::Hep3Vector WallEPos(0, 0,ModLength/2 - (parameters->BackWallThickness()+CuThick)/2);
  xform = new GeoTransform(HepGeom::Transform3D(rmD,WallEPos));
  tag = new GeoNameTag("Wall E");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallE);

  //wall I
  double heightI = ModHeight - ModTailHeight - WallThick;
  GeoPhysVol* WallI = wall.Build(heightI/2, ReducedModWidth/2, LamelG10Thick, "bcmWallLog", g10);
		
  CLHEP::Hep3Vector WallIPos(WallThick + heightI/2 - ModHeight/2, 0, ModLength/2 - LamelIoffset);
  xform = new GeoTransform(HepGeom::Transform3D(rmD,WallIPos));
  tag = new GeoNameTag("Wall I");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallI);

  //wall F
  GeoPhysVol* WallF = wall.Build(heightI/2, ReducedModWidth/2, CuThick, G10Thick, copper, g10, mat_mgr);
		
  CLHEP::Hep3Vector WallFPos(WallThick + heightI/2 - ModHeight/2, 0, ModLength/2 - ModHeadLength + WallThick/2);
  xform = new GeoTransform(HepGeom::Transform3D(rmD,WallFPos));
  tag = new GeoNameTag("Wall F");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallF);

  //wall G
  GeoPhysVol* WallG = wall.Build(ModHeadLength/2, ReducedModWidth/2, CuThick, G10Thick, copper, g10, mat_mgr);
  
  CLHEP::Hep3Vector WallGPos(WallThick/2 - ModHeight/2, 0, ModLength/2 - ModHeadLength/2);
  xform = new GeoTransform(HepGeom::Transform3D(rmC,WallGPos));
  tag = new GeoNameTag("Wall G");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallG);

  //main wall
  GeoPhysVol* WallMain = wall.Build((ModLength - parameters->BackWallThickness() - CuThick)/2, ReducedModWidth/2, MainWallCuThick, MainWallG10Thick, copper, g10, mat_mgr);
  
  CLHEP::Hep3Vector WallMainPos(ModHeight/2 + MainWallThick/2 - ModTailHeight, 0, -(parameters->BackWallThickness() + CuThick)/2);
  xform = new GeoTransform(HepGeom::Transform3D(rmC,WallMainPos));
  tag = new GeoNameTag("Wall Main");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallMain);

  //copper on the main wall (top side) wall K
  //wall K
  double lengthK = LamelHoffset - LamelHThick/2 - (parameters->BackWallThickness() + CuThick);
  GeoPhysVol* WallK = wall.Build(lengthK/2, ReducedModWidth/2, CuThick, "bcmWallLog",copper);
		
  CLHEP::Hep3Vector WallKPos(ModHeight/2 + MainWallThick + CuThick/2 - ModTailHeight, 0, ModLength/2 - lengthK/2 - (parameters->BackWallThickness() + CuThick));
  xform = new GeoTransform(HepGeom::Transform3D(rmC,WallKPos));
  tag = new GeoNameTag("Wall extra cupper");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallK);

  //diamond //bcmDiamondLog
  GeoPhysVol* DiamondVolA = wall.Build(diamondSize/2, diamondSize/2, diamondThick, "bcmDiamondLog", diamond);
  
  double diamond_x = ModHeight/2 + MainWallThick + diamondThick/2 - ModTailHeight;
  CLHEP::Hep3Vector DiamondPosA(diamond_x, parameters->DiamondPosition_Y(), parameters->DiamondPosition_Z());
  xform = new GeoTransform(HepGeom::Transform3D(rmC,DiamondPosA));
  tag = new GeoNameTag("Diamond");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(new GeoIdentifierTag(11950));
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(DiamondVolA);

  GeoPhysVol* DiamondVolB = wall.Build(diamondSize/2, diamondSize/2, diamondThick, "bcmDiamondLog", diamond);
  
  CLHEP::Hep3Vector DiamondPosB(diamond_x + diamondThick + parameters->DiamondDelta_X(), parameters->DiamondPosition_Y() + parameters->DiamondDelta_Y(), parameters->DiamondPosition_Z() + parameters->DiamondDelta_Z());
  xform = new GeoTransform(HepGeom::Transform3D(rmC,DiamondPosB));
  tag = new GeoNameTag("Diamond");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(new GeoIdentifierTag(11951));
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(DiamondVolB);

  //wall A
  GeoPhysVol* WallA = wall.Build(ModTailHeight/2, (ModLength - ModHeadLength)/2, CuThick, G10Thick, copper, g10, mat_mgr);
  
  CLHEP::Hep3Vector WallAPos((ModHeight - ModTailHeight)/2 , (ReducedModWidth + WallThick)/2, -ModHeadLength/2);
  CLHEP::HepRotation rmA;
  rmA.rotateX(90.*CLHEP::deg);
  xform = new GeoTransform(HepGeom::Transform3D(rmA,WallAPos));
  tag = new GeoNameTag("Wall A");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallA);
  
  //wall AA
  GeoPhysVol* WallAA = wall.Build(ModTailHeight/2, (ModLength - ModHeadLength)/2, G10Thick, CuThick, g10, copper, mat_mgr);
  
  CLHEP::Hep3Vector WallAAPos((ModHeight - ModTailHeight)/2 , -(ReducedModWidth + WallThick)/2, -ModHeadLength/2);
  xform = new GeoTransform(HepGeom::Transform3D(rmA,WallAAPos));
  tag = new GeoNameTag("Wall AA");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallAA);
  
  //wall B
  GeoPhysVol* WallB = wall.Build(ModHeight/2, ModHeadLength/2, CuThick, G10Thick, copper, g10, mat_mgr);
  
  CLHEP::Hep3Vector WallBPos(0, (ReducedModWidth + WallThick)/2, (ModLength - ModHeadLength)/2);
  xform = new GeoTransform(HepGeom::Transform3D(rmA,WallBPos));
  tag = new GeoNameTag("Wall B");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallB);
  
  //wall BB
  GeoPhysVol* WallBB = wall.Build(ModHeight/2, ModHeadLength/2, G10Thick, CuThick, g10, copper, mat_mgr);
  
  CLHEP::Hep3Vector WallBBPos(0, -(ReducedModWidth + WallThick)/2, (ModLength - ModHeadLength)/2);
  xform = new GeoTransform(HepGeom::Transform3D(rmA,WallBBPos));
  tag = new GeoNameTag("Wall BB");
  env_bcmModPhys->add(tag);
  env_bcmModPhys->add(xform);
  env_bcmModPhys->add(WallBB);


  // --------------------------------------------------------------------------------------
  // Add the BCM envelop inside the new complex encompassing volume
  // --------------------------------------------------------------------------------------

  CLHEP::HepRotation rmEnv;
  rmEnv.rotateZ(90.*CLHEP::deg);
  rmEnv.rotateY(90.*CLHEP::deg);

  CLHEP::Hep3Vector WallEnvPos(0., 0., 0.);
  xform = new GeoTransform(HepGeom::Transform3D(rmEnv,WallEnvPos));
  tag = new GeoNameTag("EnvBcmWallLog");
  bcmModPhys->add(tag);
  bcmModPhys->add(xform);
  bcmModPhys->add(env_bcmModPhys);
  
  return bcmModPhys;
}
