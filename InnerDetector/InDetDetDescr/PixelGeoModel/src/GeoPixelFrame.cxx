/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Build the global support frame

#include "GeoPixelFrame.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GaudiKernel/PhysicalConstants.h"
#include <algorithm>

GeoPixelFrame::GeoPixelFrame()
{
}

void GeoPixelFrame::BuildAndPlace(GeoFullPhysVol * parent, int section)
{
  double rminSide = m_gmt_mgr->PixelFrameRMinSide(section);
  double rmaxSide = m_gmt_mgr->PixelFrameRMaxSide(section);
  double sideWidth = m_gmt_mgr->PixelFrameSideWidth(section);
  double zmin = m_gmt_mgr->PixelFrameZMin(section);
  double zmax = m_gmt_mgr->PixelFrameZMax(section);
  double phiLoc = m_gmt_mgr->PixelFramePhiStart(section);
  int numSides = m_gmt_mgr->PixelFrameNumSides(section);
  bool mirrorSides = m_gmt_mgr->PixelFrameMirrorSides(section);

  // No frame defined in the DB table
  if(numSides==0)return;

  double midRadius = 0.5*(rminSide+rmaxSide);
  double zCenter = 0.5*(zmin+zmax);

  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");


  /////////////////////////
  // The Frame envelope
  ////////////////////////
  
  // Make envelope to hold the frame
  //double safety = 0.001 * Gaudi::Units::mm;
  double epsilon = 0.00001 * Gaudi::Units::mm;
  double halflength = 0.5*std::abs(zmax - zmin); 

  double alpha = Gaudi::Units::pi/numSides;
  double cosalpha = cos(alpha);
  double sinalpha = sin(alpha);
  
  /*
  double rminEnv = (rminSide-safety)/cosalpha;
  double rmaxEnv = (rmaxSide+safety)/cosalpha;
  GeoPgon * frameEnvShape = new GeoPgon(phiLoc-alpha,2*Gaudi::Units::pi,numSides);
  frameEnvShape->addPlane(zCenter-halflength-0.5*epsilon,rminEnv,rmaxEnv);
  frameEnvShape->addPlane(zCenter+halflength+0.5*epsilon,rminEnv,rmaxEnv);

  GeoLogVol * frameEnvLV = new GeoLogVol("Frame",frameEnvShape,air);
  GeoPhysVol * frameEnvPV = new GeoPhysVol(frameEnvLV);
  */

  ////////////////////////
  // The Corners
  ///////////////////////

  // Calculate corner parameters. This is made using a BREP by defining the vertices.
  double rmin = rminSide/cosalpha;
  double rmax = rmaxSide/cosalpha;
  double innerFullSideHalfWidth = rmin * sinalpha; 
  double outerFullSideHalfWidth = rmax * sinalpha; 
  double sideHalfWidth = 0.5 * sideWidth;
  if (innerFullSideHalfWidth - sideHalfWidth < 0) {
    std::cout << "Error. innerFullSideWidth is smaller than sideWidth" << std::endl;
    // ERROR
  }

  double v1x = rmax;
  double v1y = 0;
  double v2x = rmax - (outerFullSideHalfWidth - sideHalfWidth) * sinalpha; 
  double v2y = (outerFullSideHalfWidth - sideHalfWidth) * cosalpha; 
  double v3x = rmin - (innerFullSideHalfWidth - sideHalfWidth) * sinalpha;
  double v3y = (innerFullSideHalfWidth - sideHalfWidth) * cosalpha;
  double v4x = rmin;
  double v4y = 0;
  double v5x = v3x;
  double v5y = -v3y;
  double v6x = v2x;
  double v6y = -v2y;
  
  //std::cout << "Point 1: (" << v1x << ", " << v1y << ")" << std::endl; 
  //std::cout << "Point 2: (" << v2x << ", " << v2y << ")" << std::endl; 
  //std::cout << "Point 3: (" << v3x << ", " << v3y << ")" << std::endl; 
  //std::cout << "Point 4: (" << v4x << ", " << v4y << ")" << std::endl; 
  //std::cout << "Point 5: (" << v5x << ", " << v5y << ")" << std::endl; 
  //std::cout << "Point 6: (" << v6x << ", " << v6y << ")" << std::endl; 

   // Make the Corner shape
  GeoSimplePolygonBrep * cornerShape = new GeoSimplePolygonBrep(halflength-epsilon);
  cornerShape->addVertex(v1x,v1y);
  cornerShape->addVertex(v2x,v2y);
  cornerShape->addVertex(v3x,v3y);
  cornerShape->addVertex(v4x,v4y);
  cornerShape->addVertex(v5x,v5y);
  cornerShape->addVertex(v6x,v6y);

  // Test GeoModel volume calculation. OK.
  // double volumeBrepTest = (innerFullSideHalfWidth + outerFullSideHalfWidth - 2*sideHalfWidth)
  //                          * (rmaxSide-rminSide)*2*(halflength-epsilon);
  // std::cout << "Volume BREP: GeoModel: " << cornerShape->volume() << "   My calc: " << volumeBrepTest << std::endl;

  std::string cornerMatName =  m_gmt_mgr->PixelFrameCornerMaterial(section);
  const GeoMaterial* cornerMat = m_mat_mgr->getMaterialForVolume(cornerMatName,numSides*cornerShape->volume()); // CHeck volume calc.
  //const GeoMaterial* cornerMat = m_mat_mgr->getMaterial("std::Carbon");
  GeoLogVol * cornerLV = new GeoLogVol("FrameCorner",cornerShape,cornerMat);
  

  ////////////////////////
  // The Sides
  ///////////////////////

  // Make the side
  // Put the side in a box
  GeoPhysVol * sideEnvelopePV = 0;
  double zSideCenter = 0;
  int numElements = m_gmt_mgr->PixelFrameNumSideElements(section);

  if (numElements) {
    double sideThick = rmaxSide - rminSide;

    std::vector<GeoTrf::Transform3D> sideTransVec;
    std::vector<GeoShape *> sideElementShapeVec;
    sideTransVec.reserve(numElements);
    sideElementShapeVec.reserve(numElements);

    bool first = true;
    double zSideMin = 0;
    double zSideMax = 0;
    double totSideVolume = 0;
    for (int iElement = 0; iElement < numElements; iElement++) {
      // 1, 2 (ie zMin1, zMin2 and  zMax1 and zMax2 refers to the lower and upper (in phi) edges of the side element of the frame. 
      double zMin1 = m_gmt_mgr->PixelFrameElementZMin1(section, iElement)+2*epsilon;
      double zMax1 = m_gmt_mgr->PixelFrameElementZMax1(section, iElement)-2*epsilon;
      double zMin2 = m_gmt_mgr->PixelFrameElementZMin2(section, iElement)+2*epsilon;
      double zMax2 = m_gmt_mgr->PixelFrameElementZMax2(section, iElement)-2*epsilon;
      double zminInput = std::min(zMin1,std::min(zMax1,std::min(zMin2,zMax2)));
      double zmaxInput = std::max(zMin1,std::max(zMax1,std::max(zMin2,zMax2)));
      if (first) {
	zSideMin = zminInput;
	zSideMax = zmaxInput;
	first = false;
      } else {
	zSideMin = std::min(zSideMin, zminInput);
	zSideMax = std::max(zSideMax, zmaxInput);
      }      
      GeoShape * sideElementShape = 0;
      GeoTrf::Transform3D rotateShape(GeoTrf::Transform3D::Identity());
      double shapeVolume = 0;
      if (same(zMin1,zMin2) && same(zMax1,zMax2)) { 
	// Use a box
	sideElementShape = new GeoBox(0.5*sideThick,  0.5*sideWidth-2*epsilon, 0.5*std::abs(zMax1-zMin1));
	shapeVolume = sideElementShape->volume();
      } else if (same(zMax1-zMin1, zMax2-zMin2)) {
	// Use a parallelepiped
	double alphaPara = atan2(zMin2-zMin1,sideWidth);
	double thetaPara = 0; 
	double phiPara = 0;
	sideElementShape = new GeoPara(0.5*std::abs(zMax1-zMin1),  0.5*sideWidth-epsilon, 0.5*sideThick, alphaPara, thetaPara, phiPara);
	rotateShape = GeoTrf::RotateY3D(-90*Gaudi::Units::deg);
	shapeVolume =  std::abs(zMax1-zMin1) * (sideWidth-2*epsilon) * sideThick;
      } else {// 
	      // other cases not implemented. Should not occur for the frame.
	      std::cout << "GeoPixelFrame: This case is not handled for building the frame" << std::endl;
      }
      sideTransVec.push_back(GeoTrf::TranslateZ3D(0.25*(zMin1+zMin2+zMax1+zMax2))*rotateShape);
      sideElementShapeVec.push_back(sideElementShape);
      //totSideVolume += sideElementShape->volume();
      totSideVolume += shapeVolume;
    }

    // Make envelope
    GeoBox * sideEnvelope = new GeoBox(0.5*sideThick+epsilon, 0.5*sideWidth-epsilon, 0.5*std::abs(zSideMax-zSideMin)+epsilon);
    GeoLogVol * sideEnvelopeLV = new GeoLogVol("PixelFrameSideEnv", sideEnvelope, air);
    sideEnvelopePV = new GeoPhysVol(sideEnvelopeLV);

    zSideCenter =  0.5*(zSideMin+zSideMax);

    std::string sideMatName = m_gmt_mgr->PixelFrameSideMaterial(section);
    const GeoMaterial* sideMat = m_mat_mgr->getMaterialForVolume(sideMatName,numSides*totSideVolume);  
    for (int iElement = 0; iElement < numElements; iElement++) {
      GeoTransform * transSideElement = new GeoTransform(GeoTrf::TranslateZ3D(-zSideCenter)*sideTransVec[iElement]);
      std::ostringstream frameSideName;
      frameSideName << "FrameSide" << iElement;
      GeoLogVol  * sideElementLV = new GeoLogVol(frameSideName.str(), sideElementShapeVec[iElement], sideMat);
      GeoPhysVol * sideElementPV = new GeoPhysVol(sideElementLV);
      sideEnvelopePV->add(transSideElement);
      sideEnvelopePV->add(sideElementPV);
     }
  }

  /////////////////////////
  // Put it together
  /////////////////////////
  GeoPhysVol * cornerPV = new GeoPhysVol(cornerLV);

  // place the corners and sides.
  for (int iSide = 0; iSide<numSides; iSide++) {
    double angleCorner = phiLoc + alpha * (2*iSide - 1);
    GeoTransform * cornerTrans = new GeoTransform(GeoTrf::TranslateZ3D(zCenter)*GeoTrf::RotateZ3D(angleCorner));
    // Place the corners
    parent->add(cornerTrans);
    parent->add(cornerPV);
    if (sideEnvelopePV) {
      double angleSide   = phiLoc + alpha * (2*iSide);
      GeoTrf::Transform3D oddEvenRotate(GeoTrf::Transform3D::Identity());
      if (iSide%2 && mirrorSides) {
	      oddEvenRotate = GeoTrf::RotateZ3D(Gaudi::Units::pi); // Every 2nd side we mirror the side. 
      }
      GeoTransform * sideTrans = new GeoTransform(GeoTrf::TranslateZ3D(zSideCenter)*GeoTrf::RotateZ3D(angleSide)
						  *GeoTrf::TranslateX3D(midRadius)*oddEvenRotate);

      // Place the sides
      parent->add(sideTrans);
      parent->add(sideEnvelopePV);
    }
  }
  
}

bool GeoPixelFrame::same(double v1, double v2) 
{
  const double tol = 1e-9; 
  return (std::abs(v1-v2) < tol);
}

