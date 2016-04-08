/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BLM_GeoModel/BLM_Wall.h"

#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"

double BLM_Wall::s_width = 18;
double BLM_Wall::s_length = 22;
double BLM_Wall::s_extended_length = 27;//must be more than 'length'
double BLM_Wall::s_hole_r = 1.85;
double BLM_Wall::s_hole_position = 2.5;
double BLM_Wall::s_holder_length = 33;
double BLM_Wall::s_holder_height = 10;
double BLM_Wall::s_holder_spacing = 4;
double BLM_Wall::s_holder_spacing_length = 4;
double BLM_Wall::s_holder_thickness = 3;
double BLM_Wall::s_clamp_thickness = 2;
double BLM_Wall::s_clamp_length = 6;

GeoPhysVol* BLM_Wall::BuildClamp(const GeoMaterial* material)
{
  const GeoBox* blmWallBox = new GeoBox(s_width/2, s_clamp_thickness/2, s_clamp_length/2);
  const GeoTube* blmWallHole = new GeoTube(0, s_hole_r, s_clamp_thickness);
  //rotations
  CLHEP::HepRotation rm;
  rm.rotateX(90.*CLHEP::deg);
  //position of holes
  CLHEP::Hep3Vector pos1(s_width/2-s_hole_position, 0, s_clamp_length/2-2.5);
  CLHEP::Hep3Vector pos2(s_hole_position-s_width/2, 0, s_clamp_length/2-2.5);
  
  //offsets
  HepGeom::Transform3D offset1(rm,pos1);
  HepGeom::Transform3D offset2(rm,pos2);
  const GeoShapeSubtraction& blm1 = blmWallBox->subtract(((*blmWallHole) << offset1));
  const GeoShapeSubtraction& blmFinal = blm1.subtract(((*blmWallHole)<< offset2));

  const GeoLogVol* blmWallLog = new GeoLogVol("blmWallLog", &blmFinal, material);
  GeoPhysVol* blmWallPhys = new GeoPhysVol(blmWallLog);
  
  return blmWallPhys;
}

GeoPhysVol* BLM_Wall::BuildBlock(double X, double Y, double Z, std::string logname, const GeoMaterial* material)
{
  const GeoBox* blmWallBox = new GeoBox(X/2, Y/2, Z/2);
  const GeoLogVol* blmWallLog = new GeoLogVol(logname, blmWallBox, material);
  GeoPhysVol* blmWallPhys = new GeoPhysVol(blmWallLog);
  
  return blmWallPhys;
}

GeoPhysVol* BLM_Wall::BuildScrew(double length, const GeoMaterial* material)
{
  const GeoTube* blmScrew = new GeoTube(0, 2.5, 1);
  const GeoTube* blmScrew1 = new GeoTube(0, 1.5, length/2);
  //positions
  CLHEP::HepRotation rm;
  CLHEP::Hep3Vector pos1(0, 0, length/2+1);
  HepGeom::Transform3D offset1(rm,pos1);
  const GeoShapeUnion& blmFinal = blmScrew->add(((*blmScrew1) << offset1));
  
  const GeoLogVol* blmWallLog = new GeoLogVol("blmWallLog", &blmFinal, material);
  GeoPhysVol* blmWallPhys = new GeoPhysVol(blmWallLog);
  
  return blmWallPhys;
}

GeoPhysVol* BLM_Wall::BuildHolder(const GeoMaterial* material)
{
  const GeoBox* blmWallBox = new GeoBox(s_width/2, s_holder_thickness/2, s_holder_length/2);
  const GeoBox* blmWallBox1 = new GeoBox(s_width/2, s_holder_height/2, s_holder_thickness/2);
  const GeoTube* blmWallHole = new GeoTube(0, s_hole_r, s_holder_thickness);
  const GeoBox* blmWallHole1 = new GeoBox(s_holder_spacing/2, s_holder_height, (s_holder_spacing_length+s_holder_thickness)/2+1);
  //rotations
  CLHEP::HepRotation rm;
  CLHEP::HepRotation rm1;
  rm.rotateX(90.*CLHEP::deg);
  //position of holes
  CLHEP::Hep3Vector pos1(s_width/2-s_hole_position, 0, s_holder_length/2-s_hole_position);
  CLHEP::Hep3Vector pos2(s_hole_position-s_width/2, 0, s_holder_length/2-s_hole_position);
  CLHEP::Hep3Vector pos3(s_width/2-s_hole_position, 0, s_holder_length/2-s_length+s_hole_position);
  CLHEP::Hep3Vector pos4(s_hole_position-s_width/2, 0, s_holder_length/2-s_length+s_hole_position);
  CLHEP::Hep3Vector pos5(s_width/2-s_hole_position, 0, s_holder_length/2-s_extended_length+2.1);
  CLHEP::Hep3Vector pos6(s_hole_position-s_width/2, 0, s_holder_length/2-s_extended_length+2.1);
  CLHEP::Hep3Vector pos9(s_width/2-s_hole_position, 3.5+s_holder_thickness/2-s_holder_height, s_holder_thickness/2-s_holder_length/2);
  CLHEP::Hep3Vector pos10(s_hole_position-s_width/2, 3.5+s_holder_thickness/2-s_holder_height, s_holder_thickness/2-s_holder_length/2);
  //position of vertical boxes
  CLHEP::Hep3Vector pos7(0, s_holder_thickness/2-s_holder_height/2, s_holder_thickness/2-s_holder_length/2);
  //position of the hole
  CLHEP::Hep3Vector pos8(0, -s_holder_height/2, (s_holder_spacing_length+s_holder_thickness)/2-s_holder_length/2-2);
  //offsets
  HepGeom::Transform3D offset1(rm,pos1);
  HepGeom::Transform3D offset2(rm,pos2);
  HepGeom::Transform3D offset3(rm,pos3);
  HepGeom::Transform3D offset4(rm,pos4);
  HepGeom::Transform3D offset5(rm,pos5);
  HepGeom::Transform3D offset6(rm,pos6);
  HepGeom::Transform3D offset7(rm1,pos7);
  HepGeom::Transform3D offset8(rm1,pos8);
  HepGeom::Transform3D offset9(rm1,pos9);
  HepGeom::Transform3D offset10(rm1,pos10);
  const GeoShapeUnion& blm1 = blmWallBox->add(((*blmWallBox1) << offset7));
  const GeoShapeSubtraction& blm3 = blm1.subtract(((*blmWallHole1)<< offset8));
  const GeoShapeSubtraction& blm4 = blm3.subtract(((*blmWallHole) << offset1));
  const GeoShapeSubtraction& blm5 = blm4.subtract(((*blmWallHole) << offset2));
  const GeoShapeSubtraction& blm6 = blm5.subtract(((*blmWallHole) << offset3));
  const GeoShapeSubtraction& blm7 = blm6.subtract(((*blmWallHole) << offset4));
  const GeoShapeSubtraction& blm8 = blm7.subtract(((*blmWallHole) << offset5));
  const GeoShapeSubtraction& blm9 = blm8.subtract(((*blmWallHole) << offset9));
  const GeoShapeSubtraction& blm10 = blm9.subtract(((*blmWallHole) << offset10));
  const GeoShapeSubtraction& blmFinal = blm10.subtract(((*blmWallHole) << offset6));
  
  const GeoLogVol* blmWallLog = new GeoLogVol("blmWallLog", &blmFinal, material);
  GeoPhysVol* blmWallPhys = new GeoPhysVol(blmWallLog);
  
  return blmWallPhys;
}

GeoPhysVol* BLM_Wall::BuildLayerI(double thick, const GeoMaterial* material, bool hole)
{
  if(hole == true)
    {
      const GeoShape* blmWallBox = new GeoBox(s_width/2, thick/2, s_extended_length/2);
      const GeoShape* blmWallHole = new GeoTube(0, s_hole_r, thick);
      const GeoShape* blmWallHole1 = new GeoBox(s_width/2-3.5, thick, 5.9);
      const GeoShape* blmWallHole2 = new GeoBox(s_width/2-6.1, thick, 4);
      const GeoShape* blmWallHole3 = new GeoBox(3.89, thick, 3.89);
      CLHEP::HepRotation rm;
      CLHEP::HepRotation rm1;
      CLHEP::HepRotation rm2;
      rm.rotateX(90.*CLHEP::deg);
      rm1.rotateY(45.*CLHEP::deg);
      CLHEP::Hep3Vector pos1(s_width/2-s_hole_position, 0, s_extended_length/2-s_hole_position);
      CLHEP::Hep3Vector pos2(s_hole_position-s_width/2, 0, s_extended_length/2-s_hole_position);
      CLHEP::Hep3Vector pos3(s_width/2-s_hole_position, 0, s_extended_length/2-s_length+s_hole_position);
      CLHEP::Hep3Vector pos4(s_hole_position-s_width/2, 0, s_extended_length/2-s_length+s_hole_position);
      CLHEP::Hep3Vector pos9(s_width/2-s_hole_position, 0, 2.1-s_extended_length/2);
      CLHEP::Hep3Vector pos10(s_hole_position-s_width/2, 0, 2.1-s_extended_length/2);
      CLHEP::Hep3Vector pos5(0, 0, s_extended_length/2-s_length+12.55);
      CLHEP::Hep3Vector pos6(0, 0, s_extended_length/2-s_length+4);
      CLHEP::Hep3Vector pos7(0, 0, s_extended_length/2+6.6-s_length);
      HepGeom::Transform3D offset1(rm,pos1);
      HepGeom::Transform3D offset2(rm,pos2);
      HepGeom::Transform3D offset3(rm,pos3);
      HepGeom::Transform3D offset4(rm,pos4);
      HepGeom::Transform3D offset5(rm2,pos5);
      HepGeom::Transform3D offset6(rm2,pos6);
      HepGeom::Transform3D offset7(rm1,pos7);
      HepGeom::Transform3D offset9(rm,pos9);
      HepGeom::Transform3D offset10(rm,pos10);
      
      const GeoShapeSubtraction& blm1 = blmWallBox->subtract(((*blmWallHole3) << offset7));
      const GeoShapeSubtraction& blm2 = blm1.subtract(((*blmWallHole) << offset2));
      const GeoShapeSubtraction& blm3 = blm2.subtract(((*blmWallHole) << offset3));
      const GeoShapeSubtraction& blm4 = blm3.subtract(((*blmWallHole) << offset4));
      const GeoShapeSubtraction& blm5 = blm4.subtract(((*blmWallHole1) << offset5));
      const GeoShapeSubtraction& blm6 = blm5.subtract(((*blmWallHole2) << offset6));
      const GeoShapeSubtraction& blm7 = blm6.subtract(((*blmWallHole) << offset9));
      const GeoShapeSubtraction& blm8 = blm7.subtract(((*blmWallHole) << offset10));
      const GeoShapeSubtraction& blmFinal = blm8.subtract(((*blmWallHole) << offset1));
      
      const GeoLogVol* blmWallLog = new GeoLogVol("blmWallLog", &blmFinal, material);
      GeoPhysVol* blmWallPhys = new GeoPhysVol(blmWallLog);
      
      return blmWallPhys;
    }
  else
    {
      const GeoShape* blmWallBox = new GeoBox(s_width/2, thick/2, s_extended_length/2);
      const GeoShape* blmWallHole = new GeoTube(0, s_hole_r, thick);
      CLHEP::HepRotation rm;
      CLHEP::HepRotation rm1;
      rm.rotateX(90.*CLHEP::deg);
      CLHEP::Hep3Vector pos1(s_width/2-s_hole_position, 0, s_extended_length/2-s_hole_position);
      CLHEP::Hep3Vector pos2(s_hole_position-s_width/2, 0, s_extended_length/2-s_hole_position);
      CLHEP::Hep3Vector pos3(s_width/2-s_hole_position, 0, s_extended_length/2-s_length+s_hole_position);
      CLHEP::Hep3Vector pos4(s_hole_position-s_width/2, 0, s_extended_length/2-s_length+s_hole_position);
      CLHEP::Hep3Vector pos5(s_width/2-s_hole_position, 0, 2.1-s_extended_length/2);
      CLHEP::Hep3Vector pos6(s_hole_position-s_width/2, 0, 2.1-s_extended_length/2);
      HepGeom::Transform3D offset1(rm,pos1);
      HepGeom::Transform3D offset2(rm,pos2);
      HepGeom::Transform3D offset3(rm,pos3);
      HepGeom::Transform3D offset4(rm,pos4);
      HepGeom::Transform3D offset5(rm,pos5);
      HepGeom::Transform3D offset6(rm,pos6);
      const GeoShapeSubtraction& blm1 = blmWallBox->subtract(((*blmWallHole) << offset1));
      const GeoShapeSubtraction& blm2 = blm1.subtract(((*blmWallHole) << offset2));
      const GeoShapeSubtraction& blm3 = blm2.subtract(((*blmWallHole) << offset3));
      const GeoShapeSubtraction& blm4 = blm3.subtract(((*blmWallHole) << offset4));
      const GeoShapeSubtraction& blm5 = blm4.subtract(((*blmWallHole) << offset5));
      const GeoShapeSubtraction& blmFinal = blm5.subtract(((*blmWallHole) << offset6));
      
      const GeoLogVol* blmWallLog = new GeoLogVol("blmWallLog", &blmFinal, material);
      GeoPhysVol* blmWallPhys = new GeoPhysVol(blmWallLog);
      
      return blmWallPhys;
    }
}

GeoPhysVol* BLM_Wall::BuildLayerII(double thick, const GeoMaterial* material)
{
  const GeoShape* blmWallBox = new GeoBox(s_width/2, thick/2, s_length/2);
  const GeoShape* blmWallHole = new GeoTube(0, s_hole_r, thick);
  const GeoShape* blmWallHole1 = new GeoBox(s_width/2-3.5, thick, 5.9);
  const GeoShape* blmWallHole2 = new GeoBox(s_width/2-6.1, thick, 4);
  //const GeoShape* blmWallHole3 = new GeoBox(1.76777, thick, 1.76777);
  const GeoShape* blmWallHole3 = new GeoBox(3.9, thick, 3.9);
  CLHEP::HepRotation rm;
  CLHEP::HepRotation rm1;
  CLHEP::HepRotation rm2;
  rm.rotateX(90.*CLHEP::deg);
  rm1.rotateY(45.*CLHEP::deg);
  CLHEP::Hep3Vector pos1(s_width/2-s_hole_position, 0, s_length/2-s_hole_position);
  CLHEP::Hep3Vector pos2(s_hole_position-s_width/2, 0, s_length/2-s_hole_position);
  CLHEP::Hep3Vector pos3(s_width/2-s_hole_position, 0, s_hole_position-s_length/2);
  CLHEP::Hep3Vector pos4(s_hole_position-s_width/2, 0, s_hole_position-s_length/2);
  CLHEP::Hep3Vector pos5(0, 0, 1.5);
  CLHEP::Hep3Vector pos6(0, 0, -7.175);
  CLHEP::Hep3Vector pos7(s_width/2-6.1, 0, 6.6-s_length/2);
  //CLHEP::Hep3Vector pos8(6.1-s_width/2, 0, 6.6-s_length/2);
  CLHEP::Hep3Vector pos8(0, 0, 6.6-s_length/2);
  HepGeom::Transform3D offset1(rm,pos1);
  HepGeom::Transform3D offset2(rm,pos2);
  HepGeom::Transform3D offset3(rm,pos3);
  HepGeom::Transform3D offset4(rm,pos4);
  HepGeom::Transform3D offset5(rm2,pos5);
  HepGeom::Transform3D offset6(rm2,pos6);
  HepGeom::Transform3D offset7(rm1,pos7);
  HepGeom::Transform3D offset8(rm1,pos8);

  const GeoShapeSubtraction& blm1 = blmWallBox->subtract(((*blmWallHole3) << offset8));
  const GeoShapeSubtraction& blm2 = blm1.subtract(((*blmWallHole) << offset2));
  const GeoShapeSubtraction& blm3 = blm2.subtract(((*blmWallHole) << offset3));
  const GeoShapeSubtraction& blm4 = blm3.subtract(((*blmWallHole) << offset4));
  const GeoShapeSubtraction& blm5 = blm4.subtract(((*blmWallHole1) << offset5));
  const GeoShapeSubtraction& blm6 = blm5.subtract(((*blmWallHole2) << offset6));
  //const GeoShapeSubtraction& blm7 = blm6.subtract(((*blmWallHole3) << offset7));
  const GeoShapeSubtraction& blmFinal = blm6.subtract(((*blmWallHole) << offset1));

  const GeoLogVol* blmWallLog = new GeoLogVol("blmWallLog", &blmFinal, material);
  GeoPhysVol* blmWallPhys = new GeoPhysVol(blmWallLog);
  
  return blmWallPhys;
}

GeoPhysVol* BLM_Wall::BuildLayerIII(double thick, const GeoMaterial* material)
{
  const GeoShape* blmWallBox = new GeoBox(s_width/2, thick/2, s_length/2);
  const GeoShape* blmWallHole = new GeoTube(0, s_hole_r, thick);
  const GeoShape* blmWallHole1 = new GeoBox(s_width/2-3.5, thick, 5.425);
  CLHEP::HepRotation rm;
  CLHEP::HepRotation rm1;
  rm.rotateX(90.*CLHEP::deg);
  CLHEP::Hep3Vector pos1(s_width/2-s_hole_position, 0, s_length/2-s_hole_position);
  CLHEP::Hep3Vector pos2(s_hole_position-s_width/2, 0, s_length/2-s_hole_position);
  CLHEP::Hep3Vector pos3(s_width/2-s_hole_position, 0, s_hole_position-s_length/2);
  CLHEP::Hep3Vector pos4(s_hole_position-s_width/2, 0, s_hole_position-s_length/2);
  CLHEP::Hep3Vector pos5(0, 0, 2.075);
  HepGeom::Transform3D offset1(rm,pos1);
  HepGeom::Transform3D offset2(rm,pos2);
  HepGeom::Transform3D offset3(rm,pos3);
  HepGeom::Transform3D offset4(rm,pos4);
  HepGeom::Transform3D offset5(rm1,pos5);
  const GeoShapeSubtraction& blm1 = blmWallBox->subtract(((*blmWallHole) << offset1));
  const GeoShapeSubtraction& blm2 = blm1.subtract(((*blmWallHole) << offset2));
  const GeoShapeSubtraction& blm3 = blm2.subtract(((*blmWallHole) << offset3));
  const GeoShapeSubtraction& blm4 = blm3.subtract(((*blmWallHole) << offset4));;
  const GeoShapeSubtraction& blmFinal = blm4.subtract(((*blmWallHole1) << offset5));

  const GeoLogVol* blmWallLog = new GeoLogVol("blmWallLog", &blmFinal, material);
  GeoPhysVol* blmWallPhys = new GeoPhysVol(blmWallLog);
  
  return blmWallPhys;
}

GeoPhysVol* BLM_Wall::BuildLayerIV(double thick, const GeoMaterial* material)
{
  const GeoShape* blmWallBox = new GeoBox(s_width/2, thick/2, s_length/2);
  const GeoShape* blmWallHole = new GeoTube(0, s_hole_r, thick);
  const GeoShape* blmWallHole1 = new GeoBox(s_width/2-3.5, thick, 5.425);
  const GeoShape* blmWallHole2 = new GeoBox(s_width/2-8.1, thick, 4);
  CLHEP::HepRotation rm;
  CLHEP::HepRotation rm1;
  rm.rotateX(90.*CLHEP::deg);
  CLHEP::Hep3Vector pos1(s_width/2-s_hole_position, 0, s_length/2-s_hole_position);
  CLHEP::Hep3Vector pos2(s_hole_position-s_width/2, 0, s_length/2-s_hole_position);
  CLHEP::Hep3Vector pos3(s_width/2-s_hole_position, 0, s_hole_position-s_length/2);
  CLHEP::Hep3Vector pos4(s_hole_position-s_width/2, 0, s_hole_position-s_length/2);
  CLHEP::Hep3Vector pos5(0, 0, 2.075);
  CLHEP::Hep3Vector pos6(0, 0, -7.175);
  HepGeom::Transform3D offset1(rm,pos1);
  HepGeom::Transform3D offset2(rm,pos2);
  HepGeom::Transform3D offset3(rm,pos3);
  HepGeom::Transform3D offset4(rm,pos4);
  HepGeom::Transform3D offset5(rm1,pos5);
  HepGeom::Transform3D offset6(rm1,pos6);
  const GeoShapeSubtraction& blm1 = blmWallBox->subtract(((*blmWallHole) << offset1));
  const GeoShapeSubtraction& blm2 = blm1.subtract(((*blmWallHole) << offset2));
  const GeoShapeSubtraction& blm3 = blm2.subtract(((*blmWallHole) << offset3));
  const GeoShapeSubtraction& blm4 = blm3.subtract(((*blmWallHole) << offset4));;
  const GeoShapeSubtraction& blm5 = blm4.subtract(((*blmWallHole1) << offset5));
  const GeoShapeSubtraction& blmFinal = blm5.subtract(((*blmWallHole2) << offset6));

  const GeoLogVol* blmWallLog = new GeoLogVol("blmWallLog", &blmFinal, material);
  GeoPhysVol* blmWallPhys = new GeoPhysVol(blmWallLog);
  
  return blmWallPhys;
}

GeoPhysVol* BLM_Wall::BuildLayerV(double thick, const GeoMaterial* material)
{
  const GeoShape* blmWallBox = new GeoBox(s_width/2, thick/2, s_length/2);
  const GeoShape* blmWallHole = new GeoTube(0, s_hole_r, thick);
  CLHEP::HepRotation rm;
  rm.rotateX(90.*CLHEP::deg);
  CLHEP::Hep3Vector pos1(s_width/2-s_hole_position, 0, s_length/2-s_hole_position);
  CLHEP::Hep3Vector pos2(s_hole_position-s_width/2, 0, s_length/2-s_hole_position);
  CLHEP::Hep3Vector pos3(s_width/2-s_hole_position, 0, s_hole_position-s_length/2);
  CLHEP::Hep3Vector pos4(s_hole_position-s_width/2, 0, s_hole_position-s_length/2);
  HepGeom::Transform3D offset1(rm,pos1);
  HepGeom::Transform3D offset2(rm,pos2);
  HepGeom::Transform3D offset3(rm,pos3);
  HepGeom::Transform3D offset4(rm,pos4);
  const GeoShapeSubtraction& blm1 = blmWallBox->subtract(((*blmWallHole) << offset1));
  const GeoShapeSubtraction& blm2 = blm1.subtract(((*blmWallHole) << offset2));
  const GeoShapeSubtraction& blm3 = blm2.subtract(((*blmWallHole) << offset3));
  const GeoShapeSubtraction& blmFinal = blm3.subtract(((*blmWallHole) << offset4));;

  const GeoLogVol* blmWallLog = new GeoLogVol("blmWallLog", &blmFinal, material);
  GeoPhysVol* blmWallPhys = new GeoPhysVol(blmWallLog);
  
  return blmWallPhys;
}
