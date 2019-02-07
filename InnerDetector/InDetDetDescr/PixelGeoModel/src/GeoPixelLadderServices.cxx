/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds the Ladder services (Omega + Al tube + glue + part of pigtail + connector)
//
//
#include "GeoPixelLadderServices.h"
#include "GeoPixelFluid.h"
#include "GeoPixelCable.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <cmath>
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
  double halflength =0.5* m_gmt_mgr->PixelLadderLength()+m_epsilon;
  //
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  //
  //const GeoBox* ladderSvcBox = new GeoBox(thickness/2.,width/2.,halflength);
  // Quick fix - we hardwire the numbers. Need to work out a way to extract this from the database numbers.
  double safety = 0.01*Gaudi::Units::mm;
  double xBase = 0;
  
  // ConnA: Part to fit Connector
  // ConnC: Part to fit Cable

  double xOffsetConnA = xBase + m_gmt_mgr->PixelLadderCableOffsetX() - m_gmt_mgr->PixelLadderServicesX();
  double xOffsetConnC = xOffsetConnA;


  //double xMaxConnA = 1.250 * Gaudi::Units::cm + xOffsetConnA + safety;
  // double xMaxConnC = 1.6575 * Gaudi::Units::cm + xOffsetConnC + safety;
  // max offset is 12.5mm + 1/2 thickness of cables
  //  double xMaxConnA = 1.5075 * Gaudi::Units::cm + 0.5* 0.15*Gaudi::Units::cm + xOffsetConnA + safety;
  double xMaxConnA = m_gmt_mgr->PixelConnectorPosX(1) + 0.5*m_gmt_mgr->PixelConnectorWidthX(1)  + xOffsetConnA + safety;
  double xMaxConnC = 1.25 * Gaudi::Units::cm + 0.5* 0.0125*Gaudi::Units::cm + xOffsetConnC + safety;
  double xMaxOmegaBase = 0.055 * Gaudi::Units::cm + xBase + 1*Gaudi::Units::mm; // The 1 mm is just extra safety. 
  double yWidthConnA = 1.0 * Gaudi::Units::cm;
  double yWidthConnC = 0.2 * Gaudi::Units::cm;
  double yPosConnA =  m_gmt_mgr->PixelLadderCableOffsetY() - m_gmt_mgr->PixelLadderServicesY();
  double yPosConnC =  yPosConnA;
  double xCenter = 0;
  double xWidthOmegaBase = xMaxOmegaBase - xBase;
  double xWidthConnA = xMaxConnA - xBase;
  double xWidthConnC = xMaxConnC - xBase;
  double yWidthOmega = 1.2*Gaudi::Units::cm + m_epsilon;

  const GeoBox* omegaBaseEnv = new GeoBox(0.5*xWidthOmegaBase, 0.5*yWidthOmega, halflength);
  const GeoBox* connAEnv     = new GeoBox(0.5*xWidthConnA, 0.5*yWidthConnA + safety, halflength);
  const GeoBox* connCEnv     = new GeoBox(0.5*xWidthConnC, 0.5*yWidthConnC + safety, halflength);
  GeoTrf::Transform3D omegaBaseShift = GeoTrf::Translate3D(xBase + 0.5*xWidthOmegaBase - xCenter, 0., 0.);
  GeoTrf::Transform3D connAShift =     GeoTrf::Translate3D(xBase + 0.5*xWidthConnA - xCenter, yPosConnA, 0.);
  GeoTrf::Transform3D connCShift =     GeoTrf::Translate3D(xBase + 0.5*xWidthConnC - xCenter, yPosConnC, 0.);

  const GeoShape & ladderSvcShape = ((*omegaBaseEnv) << omegaBaseShift)
    .add(*connAEnv << connAShift)
    .add(*connCEnv << connCShift);

  m_ladderServicesLV = new GeoLogVol("LadderSvc",&ladderSvcShape,air);
  m_ladderServicesLV->ref();
  // store coordinates of corners of envelope.
  m_envelopeCornerA1 = GeoTrf::Vector3D(xMaxConnA - xCenter, yPosConnA + 0.5* yWidthConnA + safety, 0.);
  m_envelopeCornerA2 = GeoTrf::Vector3D(xMaxConnA - xCenter, yPosConnA - 0.5* yWidthConnA + safety, 0.);
  m_envelopeCornerC1 = GeoTrf::Vector3D(xMaxConnC - xCenter, yPosConnC + 0.5* yWidthConnC + safety, 0.);
  m_envelopeCornerC2 = GeoTrf::Vector3D(xMaxConnC - xCenter, yPosConnC - 0.5* yWidthConnC + safety, 0.);


  m_xOffset = xBase;
  m_yOffset = 0;

}
GeoPixelLadderServices::~GeoPixelLadderServices(){
  m_ladderServicesLV->unref();
}




GeoVPhysVol* GeoPixelLadderServices::Build() {
  GeoPhysVol* ladderSvcPhys = new GeoPhysVol(m_ladderServicesLV);
  //double thickness = m_gmt_mgr->PixelLadderThickness()+m_gmt_mgr->PixelCableThickness();
  //double thickness = m_gmt_mgr->PixelLadderThickness() + m_gmt_mgr->PixelCableThickness() + 0.25*Gaudi::Units::cm; // plus 0.25 cm New DC3 ???
  //double thickness = m_gmt_mgr->PixelLadderThickness()+ 6.5;  // m_gmt_mgr->PixelCableThickness() was 0.4 cm, plus 0.25 cm New DC3
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
  GeoTransform* xform   = new GeoTransform(GeoTrf::Translate3D(xpos,ypos,zpos));
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
    double xpos = m_xOffset + cable.getStackOffset() + m_gmt_mgr->PixelLadderCableOffsetX() - m_gmt_mgr->PixelLadderServicesX();
    double ypos = m_yOffset + m_gmt_mgr->PixelLadderCableOffsetY() - m_gmt_mgr->PixelLadderServicesY();
    double zpos = cable.zpos();
    GeoTransform* xform = new GeoTransform(GeoTrf::Translate3D(xpos,ypos,zpos));
    ladderSvcPhys->add(xform);
    ladderSvcPhys->add(phys);
  }

  return ladderSvcPhys;
}

GeoVPhysVol* GeoPixelLadderServices::BuildOmega() {

  double xOffset = m_xOffset;
  double yOffset = m_yOffset;
  /*
  double xUpperBend = xOffset + 2.9*Gaudi::Units::mm;
  double yUpperBend = yOffset + 0;
  double radUpperBend = 2.3*Gaudi::Units::mm; 
  double xLowerBend = xOffset + 0.9;
  double yLowerBend = yOffset + 3.35*Gaudi::Units::mm;
  double radLowerBend = 0.8*Gaudi::Units::mm; 
  double yStart= yOffset + (4.675+0.5*2.65)*Gaudi::Units::mm;
  double yEnd =  yOffset -yStart;
  double thick = 0.3*Gaudi::Units::mm;
  double length = 816*Gaudi::Units::mm;
  double zOffset = 0;
  */
  double xUpperBend = xOffset + m_gmt_mgr->PixelOmegaUpperBendX();
  double yUpperBend = yOffset + m_gmt_mgr->PixelOmegaUpperBendY();
  double radUpperBend = m_gmt_mgr->PixelOmegaUpperBendRadius(); 
  double xLowerBend = xOffset + m_gmt_mgr->PixelOmegaLowerBendX();
  double yLowerBend = yOffset + m_gmt_mgr->PixelOmegaLowerBendY();
  double radLowerBend = m_gmt_mgr->PixelOmegaLowerBendRadius(); 
  double yStart= yOffset + m_gmt_mgr->PixelOmegaStartY(); 
  double yEnd =  yOffset + m_gmt_mgr->PixelOmegaEndY(); 
  double thick = m_gmt_mgr->PixelOmegaWallThickness();
  double length = m_gmt_mgr->PixelOmegaLength();
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
  GeoTubs * upperBendShape = new GeoTubs(radUpperBend - thick, radUpperBend, 0.5* length, alpha-0.5*Gaudi::Units::pi, Gaudi::Units::pi - 2*alpha);

  // Tube sector for lower bend (+y)
  GeoTubs * lowerBendShapeP = new GeoTubs(radLowerBend - thick, radLowerBend, 0.5* length, Gaudi::Units::pi, 0.5*Gaudi::Units::pi-alpha);

  // Tube sector for lower bend (-y)
  GeoTubs * lowerBendShapeM = new GeoTubs(radLowerBend - thick, radLowerBend, 0.5* length, 0.5*Gaudi::Units::pi + alpha, 0.5*Gaudi::Units::pi-alpha);
 
  // Lower Straight section (+y)
  GeoBox * lowerStraightBoxP = new GeoBox(0.5*thick, 0.5*(yStart - yLowerBend), 0.5*length);

  // Lower Straight section (-y)
  GeoBox * lowerStraightBoxM = new GeoBox(0.5*thick, 0.5*(-yLowerBend - yEnd), 0.5*length);

  // Upper Straight section (+-y)
  GeoBox * upperStraightBox = new GeoBox(0.5*thick, 0.5*upperStraightLength, 0.5*length);

  // Put it all together
  const GeoShape & omegaShape = 
    (*lowerStraightBoxP     << GeoTrf::Translate3D(xLowerBend-radLowerBend+0.5*thick,0.5*(yLowerBend+yStart),zOffset) )
    .add(*lowerBendShapeP   << GeoTrf::Translate3D(xLowerBend,yLowerBend,zOffset) )
    .add(*upperStraightBox  << GeoTrf::Translate3D(0.5*(xLowerStraight+xUpperStraight),0.5*(yLowerStraight+yUpperStraight),zOffset)*GeoTrf::RotateZ3D(0.5*Gaudi::Units::pi-alpha) )
    .add(*upperBendShape    << GeoTrf::Translate3D(xUpperBend,yUpperBend,zOffset) )
    .add(*upperStraightBox  << GeoTrf::Translate3D(0.5*(xLowerStraight+xUpperStraight),-0.5*(yLowerStraight+yUpperStraight),zOffset)*GeoTrf::RotateZ3D(0.5*Gaudi::Units::pi+alpha) )
    .add(*lowerBendShapeM   << GeoTrf::Translate3D(xLowerBend,-yLowerBend,zOffset) )
    .add(*lowerStraightBoxM << GeoTrf::Translate3D(xLowerBend-radLowerBend+0.5*thick,0.5*(-yLowerBend+yEnd),zOffset) );

  double totVolume = 
    lowerStraightBoxP->volume() 
    + lowerBendShapeP->volume()
    + 2*upperStraightBox->volume()
    + upperBendShape->volume()
    + lowerBendShapeM->volume()
    + lowerStraightBoxM->volume();

  std::string matName = m_gmt_mgr->getMaterialName("Omega", m_gmt_mgr->GetLD());
  const GeoMaterial* omegaMat = m_mat_mgr->getMaterialForVolume(matName,totVolume);
  GeoLogVol* omegaLV = new GeoLogVol("Omega",&omegaShape,omegaMat);
  return new GeoPhysVol(omegaLV);

}

GeoVPhysVol* GeoPixelLadderServices::BuildAlTube() {

  double xOffset = m_xOffset;
  double yOffset = m_yOffset;

  /*
  double xUpperBend = xOffset + 2.7*Gaudi::Units::mm;
  double yUpperBend = yOffset;
  double radUpperBend = 2.0*Gaudi::Units::mm; 
  double xLowerBend = xOffset + 0.55;
  double yLowerBend = yOffset+1.925*Gaudi::Units::mm;
  double radLowerBend = 0.5*Gaudi::Units::mm; 
  double thick = 0.2*Gaudi::Units::mm;
  double length = 838*Gaudi::Units::mm;
  double zOffset = 0;
  */

  double xUpperBend = xOffset + m_gmt_mgr->PixelAlTubeUpperBendX();
  double yUpperBend = yOffset + m_gmt_mgr->PixelAlTubeUpperBendY();
  double radUpperBend =  m_gmt_mgr->PixelAlTubeUpperBendRadius(); 
  double xLowerBend = xOffset + m_gmt_mgr->PixelAlTubeLowerBendX();
  double yLowerBend = yOffset + m_gmt_mgr->PixelAlTubeLowerBendY();
  double radLowerBend = m_gmt_mgr->PixelAlTubeLowerBendRadius(); 
  double thick = m_gmt_mgr->PixelAlTubeWallThickness();
  double length = m_gmt_mgr->PixelAlTubeLength();
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
  GeoTubs * upperBendShape = new GeoTubs(radUpperBend - thick, radUpperBend, 0.5* length, alpha-0.5*Gaudi::Units::pi, Gaudi::Units::pi - 2*alpha);

  // Tube sector for lower bend (+y)
  GeoTubs * lowerBendShapeP = new GeoTubs(radLowerBend - thick, radLowerBend, 0.5* length, 0.5*Gaudi::Units::pi-alpha, 0.5*Gaudi::Units::pi+alpha);

  // Tube sector for lower bend (-y)
  GeoTubs * lowerBendShapeM = new GeoTubs(radLowerBend - thick, radLowerBend, 0.5* length, Gaudi::Units::pi, 0.5*Gaudi::Units::pi+alpha);
 
  // Lower Straight section 
  GeoBox * lowerStraightBox = new GeoBox(0.5*thick, yLowerBend, 0.5*length);

  // Upper Straight section (+-y)
  GeoBox * upperStraightBox = new GeoBox(0.5*thick, 0.5*upperStraightLength, 0.5*length);

  // Put it all together
  const GeoShape & alTubeShape = 
    (*lowerStraightBox      << GeoTrf::Translate3D(xLowerBend-radLowerBend+0.5*thick,0,zOffset) )
    .add(*lowerBendShapeP   << GeoTrf::Translate3D(xLowerBend,yLowerBend,zOffset) )
    .add(*upperStraightBox  << GeoTrf::Translate3D(0.5*(xLowerStraight+xUpperStraight),0.5*(yLowerStraight+yUpperStraight),zOffset)*GeoTrf::RotateZ3D(0.5*Gaudi::Units::pi-alpha) )
    .add(*upperBendShape    << GeoTrf::Translate3D(xUpperBend,yUpperBend,zOffset) )
    .add(*upperStraightBox  << GeoTrf::Translate3D(0.5*(xLowerStraight+xUpperStraight),-0.5*(yLowerStraight+yUpperStraight),zOffset)*GeoTrf::RotateZ3D(0.5*Gaudi::Units::pi+alpha) )
    .add(*lowerBendShapeM   << GeoTrf::Translate3D(xLowerBend,-yLowerBend,zOffset) );

  double totVolume = 
    lowerStraightBox->volume() 
    + lowerBendShapeP->volume()
    + 2*upperStraightBox->volume()
    + upperBendShape->volume()
    + lowerBendShapeM->volume();


  std::string matName = m_gmt_mgr->getMaterialName("AlTube", m_gmt_mgr->GetLD());
  const GeoMaterial* alMat = m_mat_mgr->getMaterialForVolume(matName,totVolume);
  GeoLogVol* tubeLV = new GeoLogVol("AlTube",&alTubeShape,alMat);
  return new GeoPhysVol(tubeLV);


}

void GeoPixelLadderServices::BuildGlue(GeoPhysVol * parent) {


  int nGlueElements = m_gmt_mgr->PixelNumOmegaGlueElements();
  for (int i = 0; i < nGlueElements; ++i) {
    double x1 = m_gmt_mgr->PixelOmegaGlueStartX(i); 
    double thickness = m_gmt_mgr->PixelOmegaGlueThickness(i); 
    double y1 = m_gmt_mgr->PixelOmegaGlueStartY(i); 
    double y2 = m_gmt_mgr->PixelOmegaGlueEndY(i);  
    double length =  m_gmt_mgr->PixelOmegaGlueLength(i); 
    double zOffset =  m_gmt_mgr->PixelOmegaGluePosZ(i);
    int typeNum = m_gmt_mgr->PixelOmegaGlueTypeNum(i);
 
    GeoBox * glueShape = new GeoBox(0.5*thickness, 0.5*std::abs(y1-y2), 0.5*length);
    std::string matName = m_gmt_mgr->getMaterialName("GlueOmegaStave", m_gmt_mgr->GetLD(), typeNum);
    const GeoMaterial* glueMat = m_mat_mgr->getMaterialForVolume(matName, glueShape->volume());
    GeoLogVol* glueLV   = new GeoLogVol("Glue",glueShape,glueMat);
    GeoPhysVol* gluePV = new GeoPhysVol(glueLV);
 
    GeoTransform * xform = new GeoTransform(GeoTrf::Translate3D(m_xOffset+x1+0.5*thickness, m_yOffset-0.5*(y1+y2), zOffset));

    parent->add(xform);
    parent->add(gluePV);
  } 
}


void GeoPixelLadderServices::BuildPigtailAndConnector(GeoPhysVol * parent) {
 
  double xOffset = m_xOffset + m_gmt_mgr->PixelLadderCableOffsetX() - m_gmt_mgr->PixelLadderServicesX();
  double yOffset = m_yOffset + m_gmt_mgr->PixelLadderCableOffsetY() - m_gmt_mgr->PixelLadderServicesY();
  
  // Pigtail flat section
  double xPosPigtail = xOffset + m_gmt_mgr->PixelPigtailPosX(); 
  double yPosPigtail = yOffset + 0.5*(m_gmt_mgr->PixelPigtailStartY() + m_gmt_mgr->PixelPigtailEndY()); 
  double zPosPigtail = m_gmt_mgr->PixelPigtailPosZ();       
  double xWidthPigtail = m_gmt_mgr->PixelPigtailThickness(); 
  double yWidthPigtail = std::abs(m_gmt_mgr->PixelPigtailStartY() - m_gmt_mgr->PixelPigtailEndY());
  double zWidthPigtail = m_gmt_mgr->PixelPigtailFlatWidthZ();       
  std::string matNamePigtail = m_gmt_mgr->getMaterialName("PigtailFlat", m_gmt_mgr->GetLD());
  GeoBox * pigtailShape = new GeoBox(0.5*xWidthPigtail, 0.5*yWidthPigtail, 0.5*zWidthPigtail);
  const GeoMaterial* pigtailMat = m_mat_mgr->getMaterialForVolume(matNamePigtail,pigtailShape->volume());
  GeoLogVol  * pigtailLV   = new GeoLogVol("PigtailFlat",pigtailShape,pigtailMat);
  GeoPhysVol * pigtailPhys = new GeoPhysVol(pigtailLV);
 
  // Connector
  int numConn = m_gmt_mgr->PixelNumConnectorElements();
  std::vector<GeoPhysVol *> connectorPhysVols(numConn);
  std::vector<GeoTrf::Transform3D> connectorTransforms(numConn);
  for (int iConn = 0; iConn < numConn; iConn++) {
    double xPosConnector = xOffset + m_gmt_mgr->PixelConnectorPosX(iConn); 
    double yPosConnector = yOffset + m_gmt_mgr->PixelConnectorPosY(iConn); 
    double zPosConnector = m_gmt_mgr->PixelConnectorPosZ(iConn);       
    double xWidthConnector =  m_gmt_mgr->PixelConnectorWidthX(iConn); 
    double yWidthConnector =  m_gmt_mgr->PixelConnectorWidthY(iConn); 
    double zWidthConnector =  m_gmt_mgr->PixelConnectorWidthZ(iConn); 
    std::string matNameConnector = m_gmt_mgr->getMaterialName("Connector", m_gmt_mgr->GetLD(), iConn);
    GeoBox * connectorShape = new GeoBox(0.5*xWidthConnector, 0.5*yWidthConnector, 0.5*zWidthConnector);
    const GeoMaterial* connectorMat = m_mat_mgr->getMaterialForVolume(matNameConnector,connectorShape->volume());
    GeoLogVol  * connectorLV   = new GeoLogVol("Connector",connectorShape,connectorMat);
    connectorPhysVols[iConn] = new GeoPhysVol(connectorLV);
    connectorTransforms[iConn] = GeoTrf::Translate3D(xPosConnector, yPosConnector, zPosConnector);
  }
   
  for (int iModule = 0; iModule<m_gmt_mgr->PixelNModule(); iModule++) {
    int moduleEta =  m_gmt_mgr->PixelModuleEtaFromIndex(iModule);
    double zShift = m_gmt_mgr->PixelModuleZPosition(moduleEta);
    
    // Place pigtail
    GeoTransform * xformPigtail = new GeoTransform(GeoTrf::Translate3D(xPosPigtail, yPosPigtail, zShift+zPosPigtail));
    parent->add(xformPigtail);
    parent->add(pigtailPhys);

    // Place connector
    for (int iConn = 0; iConn < numConn; iConn++) {
      GeoTransform * xformConnector = new GeoTransform(GeoTrf::TranslateZ3D(zShift)*connectorTransforms[iConn]);
      parent->add(xformConnector);
      parent->add(connectorPhysVols[iConn]);
    }
  }
}
