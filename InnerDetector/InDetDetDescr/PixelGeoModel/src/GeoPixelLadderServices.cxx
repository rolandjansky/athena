/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds the Ladder services (Omega + Al tube + glue + part of pigtail + connector)
//
//
#include "PixelGeoModel/GeoPixelLadderServices.h"
#include "PixelGeoModel/GeoPixelFluid.h"
#include "PixelGeoModel/GeoPixelCable.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"

#include <cmath>
#include <sstream>
#include <vector>

GeoPixelLadderServices::GeoPixelLadderServices(int ladderType)
  : m_ladderType(ladderType)
{
  //std::cout << "Building GeoPixelLadderServices with ladder type : " << ladderType << std::endl; 
  //
  // Define the log volume in the constructor, so I do it only once.
  //   
  // Length and thickness of the Omega are in the db
  //
  double halflength =0.5* gmt_mgr->PixelLadderLength()+m_epsilon;
  //
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  //
  //const GeoBox* ladderSvcBox = new GeoBox(thickness/2.,width/2.,halflength);
  // Quick fix - we hardwire the numbers. Need to work out a way to extract this from the database numbers.
  double safety = 0.01*CLHEP::mm;
  double xBase = 0;
  
  // ConnA: Part to fit Connector
  // ConnC: Part to fit Cable

  double xOffsetConnA = xBase + gmt_mgr->PixelLadderCableOffsetX() - gmt_mgr->PixelLadderServicesX();
  double xOffsetConnC = xOffsetConnA;


  //double xMaxConnA = 1.250 * CLHEP::cm + xOffsetConnA + safety;
  // double xMaxConnC = 1.6575 * CLHEP::cm + xOffsetConnC + safety;
  // max offset is 12.5mm + 1/2 thickness of cables
  //  double xMaxConnA = 1.5075 * CLHEP::cm + 0.5* 0.15*CLHEP::cm + xOffsetConnA + safety;
  double xMaxConnA = gmt_mgr->PixelConnectorPosX(1) + 0.5*gmt_mgr->PixelConnectorWidthX(1)  + xOffsetConnA + safety;
  double xMaxConnC = 1.25 * CLHEP::cm + 0.5* 0.0125*CLHEP::cm + xOffsetConnC + safety;
  double xMaxOmegaBase = 0.055 * CLHEP::cm + xBase + 1*CLHEP::mm; // The 1 mm is just extra safety. 
  double yWidthConnA = 1.0 * CLHEP::cm;
  double yWidthConnC = 0.2 * CLHEP::cm;
  double yPosConnA =  gmt_mgr->PixelLadderCableOffsetY() - gmt_mgr->PixelLadderServicesY();
  double yPosConnC =  yPosConnA;
  double xCenter = 0;
  double xWidthOmegaBase = xMaxOmegaBase - xBase;
  double xWidthConnA = xMaxConnA - xBase;
  double xWidthConnC = xMaxConnC - xBase;
  double yWidthOmega = 1.2*CLHEP::cm + m_epsilon;

  const GeoBox* omegaBaseEnv = new GeoBox(0.5*xWidthOmegaBase, 0.5*yWidthOmega, halflength);
  const GeoBox* connAEnv     = new GeoBox(0.5*xWidthConnA, 0.5*yWidthConnA + safety, halflength);
  const GeoBox* connCEnv     = new GeoBox(0.5*xWidthConnC, 0.5*yWidthConnC + safety, halflength);
  HepGeom::Transform3D omegaBaseShift = HepGeom::Translate3D(xBase + 0.5*xWidthOmegaBase - xCenter, 0., 0.);
  HepGeom::Transform3D connAShift =     HepGeom::Translate3D(xBase + 0.5*xWidthConnA - xCenter, yPosConnA, 0.);
  HepGeom::Transform3D connCShift =     HepGeom::Translate3D(xBase + 0.5*xWidthConnC - xCenter, yPosConnC, 0.);

  const GeoShape & ladderSvcShape = ((*omegaBaseEnv) << omegaBaseShift)
    .add(*connAEnv << connAShift)
    .add(*connCEnv << connCShift);

  m_ladderServicesLV = new GeoLogVol("LadderSvc",&ladderSvcShape,air);

  // store coordinates of corners of envelope.
  m_envelopeCornerA1 = HepGeom::Point3D<double>(xMaxConnA - xCenter, yPosConnA + 0.5* yWidthConnA + safety, 0.);
  m_envelopeCornerA2 = HepGeom::Point3D<double>(xMaxConnA - xCenter, yPosConnA - 0.5* yWidthConnA + safety, 0.);
  m_envelopeCornerC1 = HepGeom::Point3D<double>(xMaxConnC - xCenter, yPosConnC + 0.5* yWidthConnC + safety, 0.);
  m_envelopeCornerC2 = HepGeom::Point3D<double>(xMaxConnC - xCenter, yPosConnC - 0.5* yWidthConnC + safety, 0.);


  m_xOffset = xBase;
  m_yOffset = 0;

}




GeoVPhysVol* GeoPixelLadderServices::Build() {
  GeoPhysVol* ladderSvcPhys = new GeoPhysVol(m_ladderServicesLV);
  //double thickness = gmt_mgr->PixelLadderThickness()+gmt_mgr->PixelCableThickness();
  //double thickness = gmt_mgr->PixelLadderThickness() + gmt_mgr->PixelCableThickness() + 0.25*CLHEP::cm; // plus 0.25 cm New DC3 ???
  //double thickness = gmt_mgr->PixelLadderThickness()+ 6.5;  // gmt_mgr->PixelCableThickness() was 0.4 cm, plus 0.25 cm New DC3
  //
  // The Glue
  BuildGlue(ladderSvcPhys);

  // The aluminium tube
  // Positioned at 0,0,0 relative to parent.
  ladderSvcPhys->add(this->BuildAlTube()); 

  // The carbon-carbon Omega
  // Positioned at 0,0,0 relative to parent.
  ladderSvcPhys->add(this->BuildOmega()); 

  //
  // Put the fluid/gaz mixture
  //
  GeoPixelFluid fluid(m_ladderType);
  double xpos = fluid.posX() + m_xOffset;
  double ypos = fluid.posY() + m_yOffset;
  double zpos = fluid.posZ();

  GeoVPhysVol* fluidPhys = fluid.Build();
  GeoTransform* xform   = new GeoTransform(HepGeom::Translate3D(xpos,ypos,zpos));
  ladderSvcPhys->add(xform);
  ladderSvcPhys->add(fluidPhys);

  //
  // Place Pigtail + connector
  //
  BuildPigtailAndConnector(ladderSvcPhys);


  //
  // Cables
  //
  GeoPixelCable cable;
  for (int ii = 0; ii < cable.numElements(); ii++) {
    cable.setElement(ii);
    GeoVPhysVol* phys = cable.Build();
    double xpos = m_xOffset + cable.getStackOffset() + gmt_mgr->PixelLadderCableOffsetX() - gmt_mgr->PixelLadderServicesX();
    double ypos = m_yOffset + gmt_mgr->PixelLadderCableOffsetY() - gmt_mgr->PixelLadderServicesY();
    double zpos = cable.zpos();
    GeoTransform* xform = new GeoTransform(HepGeom::Translate3D(xpos,ypos,zpos));
    ladderSvcPhys->add(xform);
    ladderSvcPhys->add(phys);
  }

  return ladderSvcPhys;
}

GeoVPhysVol* GeoPixelLadderServices::BuildOmega() {

  double xOffset = m_xOffset;
  double yOffset = m_yOffset;
  /*
  double xUpperBend = xOffset + 2.9*CLHEP::mm;
  double yUpperBend = yOffset + 0;
  double radUpperBend = 2.3*CLHEP::mm; 
  double xLowerBend = xOffset + 0.9;
  double yLowerBend = yOffset + 3.35*CLHEP::mm;
  double radLowerBend = 0.8*CLHEP::mm; 
  double yStart= yOffset + (4.675+0.5*2.65)*CLHEP::mm;
  double yEnd =  yOffset -yStart;
  double thick = 0.3*CLHEP::mm;
  double length = 816*CLHEP::mm;
  double zOffset = 0;
  */
  double xUpperBend = xOffset + gmt_mgr->PixelOmegaUpperBendX();
  double yUpperBend = yOffset + gmt_mgr->PixelOmegaUpperBendY();
  double radUpperBend = gmt_mgr->PixelOmegaUpperBendRadius(); 
  double xLowerBend = xOffset + gmt_mgr->PixelOmegaLowerBendX();
  double yLowerBend = yOffset + gmt_mgr->PixelOmegaLowerBendY();
  double radLowerBend = gmt_mgr->PixelOmegaLowerBendRadius(); 
  double yStart= yOffset + gmt_mgr->PixelOmegaStartY(); 
  double yEnd =  yOffset + gmt_mgr->PixelOmegaEndY(); 
  double thick = gmt_mgr->PixelOmegaWallThickness();
  double length = gmt_mgr->PixelOmegaLength();
  double zOffset = 0;

  double sepX = (xUpperBend - xLowerBend);
  double sepY = (yUpperBend - yLowerBend);
  double sep2 = sepX*sepX+sepY*sepY;
  double sep  = sqrt(sep2);
  double radDist = radUpperBend+radLowerBend-thick;
  double alpha =  atan(-sepY/sepX) - asin(radDist/sep);
  double upperStraightLength = sqrt(sep2 - radDist*radDist);
  double xLowerStraight = xLowerBend - (radLowerBend-0.5*thick)*sin(alpha);
  double yLowerStraight = yLowerBend - (radLowerBend-0.5*thick)*cos(alpha);
  double xUpperStraight = xUpperBend + (radUpperBend-0.5*thick)*sin(alpha);
  double yUpperStraight = yUpperBend + (radUpperBend-0.5*thick)*cos(alpha);


  // Tube sector for upper bend
  GeoTubs * upperBendShape = new GeoTubs(radUpperBend - thick, radUpperBend, 0.5* length, alpha-0.5*CLHEP::pi, CLHEP::pi - 2*alpha);

  // Tube sector for lower bend (+y)
  GeoTubs * lowerBendShapeP = new GeoTubs(radLowerBend - thick, radLowerBend, 0.5* length, CLHEP::pi, 0.5*CLHEP::pi-alpha);

  // Tube sector for lower bend (-y)
  GeoTubs * lowerBendShapeM = new GeoTubs(radLowerBend - thick, radLowerBend, 0.5* length, 0.5*CLHEP::pi + alpha, 0.5*CLHEP::pi-alpha);
 
  // Lower Straight section (+y)
  GeoBox * lowerStraightBoxP = new GeoBox(0.5*thick, 0.5*(yStart - yLowerBend), 0.5*length);

  // Lower Straight section (-y)
  GeoBox * lowerStraightBoxM = new GeoBox(0.5*thick, 0.5*(-yLowerBend - yEnd), 0.5*length);

  // Upper Straight section (+-y)
  GeoBox * upperStraightBox = new GeoBox(0.5*thick, 0.5*upperStraightLength, 0.5*length);

  // Put it all together
  const GeoShape & omegaShape = 
    (*lowerStraightBoxP     << HepGeom::Translate3D(xLowerBend-radLowerBend+0.5*thick,0.5*(yLowerBend+yStart),zOffset) )
    .add(*lowerBendShapeP   << HepGeom::Translate3D(xLowerBend,yLowerBend,zOffset) )
    .add(*upperStraightBox  << HepGeom::Translate3D(0.5*(xLowerStraight+xUpperStraight),0.5*(yLowerStraight+yUpperStraight),zOffset)*HepGeom::RotateZ3D(0.5*CLHEP::pi-alpha) )
    .add(*upperBendShape    << HepGeom::Translate3D(xUpperBend,yUpperBend,zOffset) )
    .add(*upperStraightBox  << HepGeom::Translate3D(0.5*(xLowerStraight+xUpperStraight),-0.5*(yLowerStraight+yUpperStraight),zOffset)*HepGeom::RotateZ3D(0.5*CLHEP::pi+alpha) )
    .add(*lowerBendShapeM   << HepGeom::Translate3D(xLowerBend,-yLowerBend,zOffset) )
    .add(*lowerStraightBoxM << HepGeom::Translate3D(xLowerBend-radLowerBend+0.5*thick,0.5*(-yLowerBend+yEnd),zOffset) );

  double totVolume = 
    lowerStraightBoxP->volume() 
    + lowerBendShapeP->volume()
    + 2*upperStraightBox->volume()
    + upperBendShape->volume()
    + lowerBendShapeM->volume()
    + lowerStraightBoxM->volume();

  std::string matName = gmt_mgr->getMaterialName("Omega", gmt_mgr->GetLD());
  const GeoMaterial* omegaMat = mat_mgr->getMaterialForVolume(matName,totVolume);
  GeoLogVol* omegaLV = new GeoLogVol("Omega",&omegaShape,omegaMat);
  return new GeoPhysVol(omegaLV);

}

GeoVPhysVol* GeoPixelLadderServices::BuildAlTube() {

  double xOffset = m_xOffset;
  double yOffset = m_yOffset;

  /*
  double xUpperBend = xOffset + 2.7*CLHEP::mm;
  double yUpperBend = yOffset;
  double radUpperBend = 2.0*CLHEP::mm; 
  double xLowerBend = xOffset + 0.55;
  double yLowerBend = yOffset+1.925*CLHEP::mm;
  double radLowerBend = 0.5*CLHEP::mm; 
  double thick = 0.2*CLHEP::mm;
  double length = 838*CLHEP::mm;
  double zOffset = 0;
  */

  double xUpperBend = xOffset + gmt_mgr->PixelAlTubeUpperBendX();
  double yUpperBend = yOffset + gmt_mgr->PixelAlTubeUpperBendY();
  double radUpperBend =  gmt_mgr->PixelAlTubeUpperBendRadius(); 
  double xLowerBend = xOffset + gmt_mgr->PixelAlTubeLowerBendX();
  double yLowerBend = yOffset + gmt_mgr->PixelAlTubeLowerBendY();
  double radLowerBend = gmt_mgr->PixelAlTubeLowerBendRadius(); 
  double thick = gmt_mgr->PixelAlTubeWallThickness();
  double length = gmt_mgr->PixelAlTubeLength();
  double zOffset = 0;

  double sepX = (xUpperBend - xLowerBend);
  double sepY = (yUpperBend - yLowerBend);
  double sep2 = sepX*sepX+sepY*sepY;
  double sep  = sqrt(sep2);
  double radDist = radUpperBend-radLowerBend;
  double alpha =  atan(-sepY/sepX) - asin(radDist/sep);
  double upperStraightLength = sqrt(sep2 - radDist*radDist);
  double xLowerStraight = xLowerBend + (radLowerBend-0.5*thick)*sin(alpha);
  double yLowerStraight = yLowerBend + (radLowerBend-0.5*thick)*cos(alpha);
  double xUpperStraight = xUpperBend + (radUpperBend-0.5*thick)*sin(alpha);
  double yUpperStraight = yUpperBend + (radUpperBend-0.5*thick)*cos(alpha);


  // Tube sector for upper bend
  GeoTubs * upperBendShape = new GeoTubs(radUpperBend - thick, radUpperBend, 0.5* length, alpha-0.5*CLHEP::pi, CLHEP::pi - 2*alpha);

  // Tube sector for lower bend (+y)
  GeoTubs * lowerBendShapeP = new GeoTubs(radLowerBend - thick, radLowerBend, 0.5* length, 0.5*CLHEP::pi-alpha, 0.5*CLHEP::pi+alpha);

  // Tube sector for lower bend (-y)
  GeoTubs * lowerBendShapeM = new GeoTubs(radLowerBend - thick, radLowerBend, 0.5* length, CLHEP::pi, 0.5*CLHEP::pi+alpha);
 
  // Lower Straight section 
  GeoBox * lowerStraightBox = new GeoBox(0.5*thick, yLowerBend, 0.5*length);

  // Upper Straight section (+-y)
  GeoBox * upperStraightBox = new GeoBox(0.5*thick, 0.5*upperStraightLength, 0.5*length);

  // Put it all together
  const GeoShape & alTubeShape = 
    (*lowerStraightBox      << HepGeom::Translate3D(xLowerBend-radLowerBend+0.5*thick,0,zOffset) )
    .add(*lowerBendShapeP   << HepGeom::Translate3D(xLowerBend,yLowerBend,zOffset) )
    .add(*upperStraightBox  << HepGeom::Translate3D(0.5*(xLowerStraight+xUpperStraight),0.5*(yLowerStraight+yUpperStraight),zOffset)*HepGeom::RotateZ3D(0.5*CLHEP::pi-alpha) )
    .add(*upperBendShape    << HepGeom::Translate3D(xUpperBend,yUpperBend,zOffset) )
    .add(*upperStraightBox  << HepGeom::Translate3D(0.5*(xLowerStraight+xUpperStraight),-0.5*(yLowerStraight+yUpperStraight),zOffset)*HepGeom::RotateZ3D(0.5*CLHEP::pi+alpha) )
    .add(*lowerBendShapeM   << HepGeom::Translate3D(xLowerBend,-yLowerBend,zOffset) );

  double totVolume = 
    lowerStraightBox->volume() 
    + lowerBendShapeP->volume()
    + 2*upperStraightBox->volume()
    + upperBendShape->volume()
    + lowerBendShapeM->volume();


  std::string matName = gmt_mgr->getMaterialName("AlTube", gmt_mgr->GetLD());
  const GeoMaterial* alMat = mat_mgr->getMaterialForVolume(matName,totVolume);
  GeoLogVol* tubeLV = new GeoLogVol("AlTube",&alTubeShape,alMat);
  return new GeoPhysVol(tubeLV);


}

void GeoPixelLadderServices::BuildGlue(GeoPhysVol * parent) {


  int nGlueElements = gmt_mgr->PixelNumOmegaGlueElements();
  for (int i = 0; i < nGlueElements; ++i) {
    double x1 = gmt_mgr->PixelOmegaGlueStartX(i); 
    double thickness = gmt_mgr->PixelOmegaGlueThickness(i); 
    double y1 = gmt_mgr->PixelOmegaGlueStartY(i); 
    double y2 = gmt_mgr->PixelOmegaGlueEndY(i);  
    double length =  gmt_mgr->PixelOmegaGlueLength(i); 
    double zOffset =  gmt_mgr->PixelOmegaGluePosZ(i);
    int typeNum = gmt_mgr->PixelOmegaGlueTypeNum(i);
 
    GeoBox * glueShape = new GeoBox(0.5*thickness, 0.5*std::abs(y1-y2), 0.5*length);
    std::string matName = gmt_mgr->getMaterialName("GlueOmegaStave", gmt_mgr->GetLD(), typeNum);
    const GeoMaterial* glueMat = mat_mgr->getMaterialForVolume(matName, glueShape->volume());
    GeoLogVol* glueLV   = new GeoLogVol("Glue",glueShape,glueMat);
    GeoPhysVol* gluePV = new GeoPhysVol(glueLV);
 
    GeoTransform * xform = new GeoTransform(HepGeom::Translate3D(m_xOffset+x1+0.5*thickness, m_yOffset-0.5*(y1+y2), zOffset));

    parent->add(xform);
    parent->add(gluePV);
  } 
}


void GeoPixelLadderServices::BuildPigtailAndConnector(GeoPhysVol * parent) {
 
  double xOffset = m_xOffset + gmt_mgr->PixelLadderCableOffsetX() - gmt_mgr->PixelLadderServicesX();
  double yOffset = m_yOffset + gmt_mgr->PixelLadderCableOffsetY() - gmt_mgr->PixelLadderServicesY();
  
  // Pigtail flat section
  double xPosPigtail = xOffset + gmt_mgr->PixelPigtailPosX(); 
  double yPosPigtail = yOffset + 0.5*(gmt_mgr->PixelPigtailStartY() + gmt_mgr->PixelPigtailEndY()); 
  double zPosPigtail = gmt_mgr->PixelPigtailPosZ();       
  double xWidthPigtail = gmt_mgr->PixelPigtailThickness(); 
  double yWidthPigtail = std::abs(gmt_mgr->PixelPigtailStartY() - gmt_mgr->PixelPigtailEndY());
  double zWidthPigtail = gmt_mgr->PixelPigtailFlatWidthZ();       
  std::string matNamePigtail = gmt_mgr->getMaterialName("PigtailFlat", gmt_mgr->GetLD());
  GeoBox * pigtailShape = new GeoBox(0.5*xWidthPigtail, 0.5*yWidthPigtail, 0.5*zWidthPigtail);
  const GeoMaterial* pigtailMat = mat_mgr->getMaterialForVolume(matNamePigtail,pigtailShape->volume());
  GeoLogVol  * pigtailLV   = new GeoLogVol("PigtailFlat",pigtailShape,pigtailMat);
  GeoPhysVol * pigtailPhys = new GeoPhysVol(pigtailLV);
 
  // Connector
  int numConn = gmt_mgr->PixelNumConnectorElements();
  std::vector<GeoPhysVol *> connectorPhysVols(numConn);
  std::vector<HepGeom::Transform3D> connectorTransforms(numConn);
  for (int iConn = 0; iConn < numConn; iConn++) {
    double xPosConnector = xOffset + gmt_mgr->PixelConnectorPosX(iConn); 
    double yPosConnector = yOffset + gmt_mgr->PixelConnectorPosY(iConn); 
    double zPosConnector = gmt_mgr->PixelConnectorPosZ(iConn);       
    double xWidthConnector =  gmt_mgr->PixelConnectorWidthX(iConn); 
    double yWidthConnector =  gmt_mgr->PixelConnectorWidthY(iConn); 
    double zWidthConnector =  gmt_mgr->PixelConnectorWidthZ(iConn); 
    std::string matNameConnector = gmt_mgr->getMaterialName("Connector", gmt_mgr->GetLD(), iConn);
    GeoBox * connectorShape = new GeoBox(0.5*xWidthConnector, 0.5*yWidthConnector, 0.5*zWidthConnector);
    const GeoMaterial* connectorMat = mat_mgr->getMaterialForVolume(matNameConnector,connectorShape->volume());
    GeoLogVol  * connectorLV   = new GeoLogVol("Connector",connectorShape,connectorMat);
    connectorPhysVols[iConn] = new GeoPhysVol(connectorLV);
    connectorTransforms[iConn] = HepGeom::Translate3D(xPosConnector, yPosConnector, zPosConnector);
  }
   
  for (int iModule = 0; iModule<gmt_mgr->PixelNModule(); iModule++) {
    int moduleEta =  gmt_mgr->PixelModuleEtaFromIndex(iModule);
    double zShift = gmt_mgr->PixelModuleZPosition(moduleEta);
    
    // Place pigtail
    GeoTransform * xformPigtail = new GeoTransform(HepGeom::Translate3D(xPosPigtail, yPosPigtail, zShift+zPosPigtail));
    parent->add(xformPigtail);
    parent->add(pigtailPhys);

    // Place connector
    for (int iConn = 0; iConn < numConn; iConn++) {
      GeoTransform * xformConnector = new GeoTransform(HepGeom::TranslateZ3D(zShift)*connectorTransforms[iConn]);
      parent->add(xformConnector);
      parent->add(connectorPhysVols[iConn]);
    }
  }
}
