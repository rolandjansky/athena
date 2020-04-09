/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

// Build detailed stave support : face plate + carbon foam + cable flex + cooling pipe + end blocks
// This will be built once per two layers

#include "BarrelInclinedRef/GeoPixelSlimStaveSupportInclRef.h"
#include "BarrelInclinedRef/PixelInclRefStaveXMLHelper.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
 
#include "GeoModelKernel/GeoTransform.h"
#include "PathResolver/PathResolver.h"

#include <algorithm>
using std::max;

GeoPixelSlimStaveSupportInclRef::GeoPixelSlimStaveSupportInclRef(const PixelGeoBuilderBasics* basics, int innerLayer, double gapPlanar, halfStaveType StaveType, int staveShapeIndex):
  PixelGeoBuilder(basics),								     
  m_innerLayer(innerLayer),
  m_planarGap(gapPlanar),
  m_staveType(StaveType),
  m_staveShapeIndex(staveShapeIndex)
{
  preBuild();
}


void GeoPixelSlimStaveSupportInclRef::preBuild() {
  // New longeron with corners
  // Access XML file
  PixelInclRefStaveXMLHelper staveDBHelper(m_innerLayer, getBasics());
  std::string matNameBrlWall   = staveDBHelper.getStaveSupportMaterial(m_staveShapeIndex);
  std::string matNameBrlCorner = staveDBHelper.getStaveSupportCornerMaterial(m_staveShapeIndex);
  std::string matNameEos       = staveDBHelper.getStaveSupportEOSMaterial   (m_staveShapeIndex);
  double matFudge = staveDBHelper.getMaterialFudge();

  double eosBaseWidth    = staveDBHelper.getBaseWidthAtEOS(m_staveShapeIndex);
  double eosTopWidth     = staveDBHelper.getTopWidthAtEOS(m_staveShapeIndex);
  double brlWidth        = staveDBHelper.getBarrelWidth(m_staveShapeIndex);

  // Outer = longeron half pointing outwards, to link with layer at higher R, 
  // therefore this is the "bottom", low-R half
  double brlZMax = (m_staveType == OUTER) ? 
    staveDBHelper.getBarrelZMax(m_staveShapeIndex):
    staveDBHelper.getBarrelZMaxHighR(m_staveShapeIndex);
  
  double radialMidpoint  = staveDBHelper.getRadialMidpointAtEOS();
  double radialLength    = staveDBHelper.getRadialLengthAtEOS(m_staveShapeIndex);
  double wallThickness   = staveDBHelper.getWallThickness(m_staveShapeIndex);
  double cornerThickness = staveDBHelper.getCornerThickness(m_staveShapeIndex);
  double brlStepLowR     = staveDBHelper.getXStepLowR(m_staveShapeIndex);
  double brlStepHighR    = staveDBHelper.getXStepHighR(m_staveShapeIndex);
  double eosLength       = (staveDBHelper.getStaveSupportLength() * 0.5) - (brlZMax + m_planarGap);
  double eosZPos         = brlZMax + m_planarGap + (eosLength *  0.5);
  
  double outerOffsetMid = 0.25*(eosBaseWidth - eosTopWidth);           // Dy>0 if top width < bottom width, i.e. sides point inwards
  double eosShellAngle  = 90*CLHEP::degree;//assume this as default value since this is the limit of atan(x) as x goes to infinity (such as when dividing by radial length by zero, which can happen for some pixel geometries)
  if(outerOffsetMid!=0) eosShellAngle  = atan( (0.5*radialLength) / outerOffsetMid);
  if (eosShellAngle < 0.0) eosShellAngle = eosShellAngle + 180*CLHEP::degree; // Ensure angle is always positive to avoid heaches with sin and tan

  double halfEosRadius = 0.5*radialLength;
  double halfBarrelRadius = 0.5*radialLength;
  halfBarrelRadius += (m_staveType == INNER) ? brlStepHighR : brlStepLowR;
  double halfBarrelWidth  = 0.5*brlWidth;
 
  double eosCornerThickness = cornerThickness;
  if (eosCornerThickness > halfEosRadius) eosCornerThickness = 0.6;

  // No idea what this correction does...
  double shapeCorr = (halfBarrelWidth + (eosBaseWidth + eosTopWidth)*0.25)*0.5;
  shapeCorr = radialMidpoint - sqrt(radialMidpoint*radialMidpoint - shapeCorr*shapeCorr);
  radialMidpoint -= shapeCorr;
  double safety = 0.25;
  radialMidpoint += (m_staveType == INNER) ? safety : -safety;

  halfBarrelRadius += (m_staveType == INNER) ? shapeCorr : -shapeCorr;
  halfEosRadius += (m_staveType == INNER) ? shapeCorr : -shapeCorr;
  
  // Construct an envelope volume - offset to enclose inner/outer stave
  // Length of volume = envelope length minus a 0.05mm safety factor
  double envelopeWidth  = std::max(std::max(eosTopWidth, eosBaseWidth), brlWidth);
  double envelopeOffset = (m_staveType == INNER) ? halfBarrelRadius/2.0 : -halfBarrelRadius/2.0;
  GeoBox* longeronEnvelope = new GeoBox(halfBarrelRadius/2.0, envelopeWidth/2.0, (staveDBHelper.getStaveSupportLength() * 0.5) - 0.05);
  const GeoShape* envelopeShape = &((*longeronEnvelope) << HepGeom::Translate3D(envelopeOffset, 0.0, 0.0));
  GeoLogVol* longeronLogVol = new GeoLogVol("Longeron", envelopeShape, matMgr()->getMaterial("special::Ether"));
  m_physVol = new GeoPhysVol(longeronLogVol);
  


  // Construct shapes that will make up the longeron
  GeoSimplePolygonBrep* brlWall1   = new GeoSimplePolygonBrep(brlZMax + m_planarGap);
  GeoSimplePolygonBrep* brlWall2   = new GeoSimplePolygonBrep(brlZMax + m_planarGap);
  GeoSimplePolygonBrep* brlWall3   = new GeoSimplePolygonBrep(brlZMax + m_planarGap);
  GeoSimplePolygonBrep* brlCorner1 = new GeoSimplePolygonBrep(brlZMax + m_planarGap);
  GeoSimplePolygonBrep* brlCorner2 = new GeoSimplePolygonBrep(brlZMax + m_planarGap);

  GeoSimplePolygonBrep* eosWall1   = new GeoSimplePolygonBrep(0.5*eosLength);
  GeoSimplePolygonBrep* eosWall2   = new GeoSimplePolygonBrep(0.5*eosLength);
  GeoSimplePolygonBrep* eosWall3   = new GeoSimplePolygonBrep(0.5*eosLength);
  GeoSimplePolygonBrep* eosCorner1 = new GeoSimplePolygonBrep(0.5*eosLength);
  GeoSimplePolygonBrep* eosCorner2 = new GeoSimplePolygonBrep(0.5*eosLength);

  if (m_staveType == INNER){
    // Barrel wall segments - could be done as simple cuboids, but this technique maintains consistency with the Eos staves
    brlWall1->addVertex(halfBarrelRadius - cornerThickness,  halfBarrelWidth);
    brlWall1->addVertex(0.0,                                 halfBarrelWidth);
    brlWall1->addVertex(0.0,                                 halfBarrelWidth-wallThickness);
    brlWall1->addVertex(halfBarrelRadius - cornerThickness,  halfBarrelWidth-wallThickness);
 
    brlWall2->addVertex(halfBarrelRadius,                  (halfBarrelWidth - cornerThickness));
    brlWall2->addVertex(halfBarrelRadius - wallThickness,  (halfBarrelWidth - cornerThickness));
    brlWall2->addVertex(halfBarrelRadius - wallThickness, -(halfBarrelWidth - cornerThickness));
    brlWall2->addVertex(halfBarrelRadius,                 -(halfBarrelWidth - cornerThickness));

    // As brlWall_1, but with -ve y (coords changed to have anticlockwise order)
    brlWall3->addVertex(halfBarrelRadius - cornerThickness,  -(halfBarrelWidth-wallThickness));
    brlWall3->addVertex(0.0,                                 -(halfBarrelWidth-wallThickness));
    brlWall3->addVertex(0.0,                                 -(halfBarrelWidth));
    brlWall3->addVertex(halfBarrelRadius - cornerThickness,  -(halfBarrelWidth));


    // Corner element
    brlCorner1->addVertex(halfBarrelRadius                , halfBarrelWidth);
    brlCorner1->addVertex(halfBarrelRadius-cornerThickness, halfBarrelWidth);
    brlCorner1->addVertex(halfBarrelRadius-cornerThickness, halfBarrelWidth - wallThickness);
    brlCorner1->addVertex(halfBarrelRadius-wallThickness  , halfBarrelWidth - wallThickness);
    brlCorner1->addVertex(halfBarrelRadius-wallThickness  , halfBarrelWidth - cornerThickness);
    brlCorner1->addVertex(halfBarrelRadius                , halfBarrelWidth - cornerThickness);
    
    // As above, but with -ve y (coords have anticlockwise order)
    brlCorner2->addVertex(halfBarrelRadius                , -(halfBarrelWidth - cornerThickness));
    brlCorner2->addVertex(halfBarrelRadius-wallThickness  , -(halfBarrelWidth - cornerThickness));
    brlCorner2->addVertex(halfBarrelRadius-wallThickness  , -(halfBarrelWidth - wallThickness));
    brlCorner2->addVertex(halfBarrelRadius-cornerThickness, -(halfBarrelWidth - wallThickness));
    brlCorner2->addVertex(halfBarrelRadius-cornerThickness, -(halfBarrelWidth));
    brlCorner2->addVertex(halfBarrelRadius                , -(halfBarrelWidth));

    // eos walls
    eosShellAngle = 180*CLHEP::degree - eosShellAngle;

    eosWall1->addVertex(0.0,                                                (0.5*eosBaseWidth) - outerOffsetMid);
    eosWall1->addVertex(0.0,                                                (0.5*eosBaseWidth) - outerOffsetMid - fabs(wallThickness/sin(eosShellAngle)) );
    eosWall1->addVertex(halfEosRadius - eosCornerThickness*sin(eosShellAngle), (0.5*eosTopWidth) - eosCornerThickness*cos(eosShellAngle) - wallThickness/sin(eosShellAngle)  );
    eosWall1->addVertex(halfEosRadius - eosCornerThickness*sin(eosShellAngle), (0.5*eosTopWidth) - eosCornerThickness*cos(eosShellAngle));

    eosWall2->addVertex(halfEosRadius - wallThickness,     -((0.5*eosTopWidth) - eosCornerThickness - wallThickness*cos(eosShellAngle)));
    eosWall2->addVertex(halfEosRadius,                     -((0.5*eosTopWidth) - eosCornerThickness));
    eosWall2->addVertex(halfEosRadius,                      (0.5*eosTopWidth) - eosCornerThickness);
    eosWall2->addVertex(halfEosRadius - wallThickness,      (0.5*eosTopWidth) - eosCornerThickness - wallThickness*cos(eosShellAngle));

    eosWall3->addVertex(halfEosRadius - eosCornerThickness*sin(eosShellAngle), -((0.5*eosTopWidth) - eosCornerThickness*cos(eosShellAngle)));
    eosWall3->addVertex(halfEosRadius - eosCornerThickness*sin(eosShellAngle), -((0.5*eosTopWidth) - eosCornerThickness*cos(eosShellAngle) - wallThickness/sin(eosShellAngle))  );
    eosWall3->addVertex(0.0,                                                -((0.5*eosBaseWidth) - outerOffsetMid - fabs(wallThickness/sin(eosShellAngle))) );
    eosWall3->addVertex(0.0,                                                -((0.5*eosBaseWidth) - outerOffsetMid));


    eosCorner1->addVertex(halfEosRadius - eosCornerThickness*sin(eosShellAngle),   (0.5*eosTopWidth) - eosCornerThickness*cos(eosShellAngle) - wallThickness/sin(eosShellAngle)  );
    eosCorner1->addVertex(halfEosRadius - wallThickness,                        (0.5*eosTopWidth) - ((wallThickness/sin(eosShellAngle)) + wallThickness/tan(eosShellAngle)  )  );
    eosCorner1->addVertex(halfEosRadius - wallThickness,                        (0.5*eosTopWidth) - eosCornerThickness - wallThickness*cos(eosShellAngle));
    eosCorner1->addVertex(halfEosRadius,                                        (0.5*eosTopWidth) - eosCornerThickness);
    eosCorner1->addVertex(halfEosRadius,                                        (0.5*eosTopWidth));
    eosCorner1->addVertex(halfEosRadius - eosCornerThickness*sin(eosShellAngle),   (0.5*eosTopWidth) - eosCornerThickness*cos(eosShellAngle));

    eosCorner2->addVertex(halfEosRadius - eosCornerThickness*sin(eosShellAngle),   -((0.5*eosTopWidth) - eosCornerThickness*cos(eosShellAngle)));
    eosCorner2->addVertex(halfEosRadius,                                        -((0.5*eosTopWidth)));
    eosCorner2->addVertex(halfEosRadius,                                        -((0.5*eosTopWidth) - eosCornerThickness));
    eosCorner2->addVertex(halfEosRadius - wallThickness,                        -((0.5*eosTopWidth) - eosCornerThickness - wallThickness*cos(eosShellAngle)));
    eosCorner2->addVertex(halfEosRadius - wallThickness,                        -((0.5*eosTopWidth) - ((wallThickness/sin(eosShellAngle)) + wallThickness/tan(eosShellAngle)  )  ));
    eosCorner2->addVertex(halfEosRadius - eosCornerThickness*sin(eosShellAngle),   -((0.5*eosTopWidth) - eosCornerThickness*cos(eosShellAngle) - wallThickness/sin(eosShellAngle)  ));


  }
  else {
    // Barrel wall segments - could be done as simple cuboids, but this technique maintains consistency with the Eos staves
    brlWall1->addVertex(0.0,                                  halfBarrelWidth);
    brlWall1->addVertex(-halfBarrelRadius + cornerThickness,  halfBarrelWidth);
    brlWall1->addVertex(-halfBarrelRadius + cornerThickness,  halfBarrelWidth-wallThickness);
    brlWall1->addVertex(0.0,                                  halfBarrelWidth-wallThickness);
    
    brlWall2->addVertex(-halfBarrelRadius + wallThickness,  (halfBarrelWidth - cornerThickness));
    brlWall2->addVertex(-halfBarrelRadius,                  (halfBarrelWidth - cornerThickness));
    brlWall2->addVertex(-halfBarrelRadius,                 -(halfBarrelWidth - cornerThickness));
    brlWall2->addVertex(-halfBarrelRadius + wallThickness, -(halfBarrelWidth - cornerThickness));
    
    // As brlWall_1, but with -ve y (coords have anticlockwise order)
    brlWall3->addVertex(0.0,                                  -(halfBarrelWidth-wallThickness));
    brlWall3->addVertex(-halfBarrelRadius + cornerThickness,  -(halfBarrelWidth-wallThickness));
    brlWall3->addVertex(-halfBarrelRadius + cornerThickness,  -(halfBarrelWidth));
    brlWall3->addVertex(0.0,                                  -(halfBarrelWidth));
    
    // Corner element
    brlCorner1->addVertex(-halfBarrelRadius + cornerThickness, halfBarrelWidth);
    brlCorner1->addVertex(-halfBarrelRadius,                   halfBarrelWidth);
    brlCorner1->addVertex(-halfBarrelRadius,                   halfBarrelWidth - cornerThickness);
    brlCorner1->addVertex(-halfBarrelRadius + wallThickness,   halfBarrelWidth - cornerThickness);
    brlCorner1->addVertex(-halfBarrelRadius + wallThickness,   halfBarrelWidth - wallThickness);
    brlCorner1->addVertex(-halfBarrelRadius + cornerThickness, halfBarrelWidth - wallThickness);
    
    // As above, but with  -ve y (coords have anticlockwise order)
    brlCorner2->addVertex(-halfBarrelRadius +cornerThickness, -(halfBarrelWidth - wallThickness));
    brlCorner2->addVertex(-halfBarrelRadius +wallThickness,   -(halfBarrelWidth - wallThickness));
    brlCorner2->addVertex(-halfBarrelRadius +wallThickness,   -(halfBarrelWidth - cornerThickness));
    brlCorner2->addVertex(-halfBarrelRadius,                  -(halfBarrelWidth - cornerThickness));
    brlCorner2->addVertex(-halfBarrelRadius,                  -(halfBarrelWidth));
    brlCorner2->addVertex(-halfBarrelRadius +cornerThickness, -(halfBarrelWidth));

    // eos walls
    eosWall1->addVertex((-1.0*halfEosRadius) + eosCornerThickness*sin(eosShellAngle),   (0.5*eosBaseWidth) - eosCornerThickness*cos(eosShellAngle));
    eosWall1->addVertex((-1.0*halfEosRadius) + eosCornerThickness*sin(eosShellAngle),   (0.5*eosBaseWidth) - eosCornerThickness*cos(eosShellAngle) - fabs(wallThickness/sin(eosShellAngle))  );
    eosWall1->addVertex(0.0,                                                 0.5*eosBaseWidth - outerOffsetMid - wallThickness/sin(eosShellAngle));
    eosWall1->addVertex(0.0,                                                 0.5*eosBaseWidth - outerOffsetMid);

    eosWall2->addVertex((-1.0*halfEosRadius) + wallThickness,      (0.5*eosBaseWidth) - eosCornerThickness - wallThickness*cos(eosShellAngle));
    eosWall2->addVertex((-1.0*halfEosRadius),                      (0.5*eosBaseWidth) - eosCornerThickness);
    eosWall2->addVertex((-1.0*halfEosRadius),                     -((0.5*eosBaseWidth) - eosCornerThickness));
    eosWall2->addVertex((-1.0*halfEosRadius) + wallThickness,     -((0.5*eosBaseWidth) - eosCornerThickness - wallThickness*cos(eosShellAngle)));

    // As eos wall 1, but flipped y-coord and order reversed
    eosWall3->addVertex(0.0,                                                         -((0.5*eosBaseWidth) - outerOffsetMid));
    eosWall3->addVertex(0.0,                                                         -((0.5*eosBaseWidth) - outerOffsetMid - wallThickness/sin(eosShellAngle)));
    eosWall3->addVertex((-1.0*halfEosRadius) + eosCornerThickness*sin(eosShellAngle),   -((0.5*eosBaseWidth) - eosCornerThickness*cos(eosShellAngle) - fabs(wallThickness/sin(eosShellAngle))  ));
    eosWall3->addVertex((-1.0*halfEosRadius) + eosCornerThickness*sin(eosShellAngle),   -((0.5*eosBaseWidth) - eosCornerThickness*cos(eosShellAngle)));

    eosCorner1->addVertex((-1.0*halfEosRadius) + eosCornerThickness*sin(eosShellAngle),   (0.5*eosBaseWidth) - eosCornerThickness*cos(eosShellAngle));
    eosCorner1->addVertex((-1.0*halfEosRadius),                                        (0.5*eosBaseWidth));
    eosCorner1->addVertex((-1.0*halfEosRadius),                                        (0.5*eosBaseWidth) - eosCornerThickness);
    eosCorner1->addVertex((-1.0*halfEosRadius) + wallThickness,                        (0.5*eosBaseWidth) - eosCornerThickness - wallThickness*cos(eosShellAngle));
    eosCorner1->addVertex((-1.0*halfEosRadius) + wallThickness,                        (0.5*eosBaseWidth) - ((wallThickness/sin(eosShellAngle)) + wallThickness/tan(eosShellAngle)  )  );
    eosCorner1->addVertex((-1.0*halfEosRadius) + eosCornerThickness*sin(eosShellAngle),   (0.5*eosBaseWidth) - eosCornerThickness*cos(eosShellAngle) - wallThickness/sin(eosShellAngle)  );

    eosCorner2->addVertex((-1.0*halfEosRadius) + eosCornerThickness*sin(eosShellAngle),   -((0.5*eosBaseWidth) - eosCornerThickness*cos(eosShellAngle) - wallThickness/sin(eosShellAngle)  ));
    eosCorner2->addVertex((-1.0*halfEosRadius) + wallThickness,                        -((0.5*eosBaseWidth) - ((wallThickness/sin(eosShellAngle)) + wallThickness/tan(eosShellAngle)  )  ));
    eosCorner2->addVertex((-1.0*halfEosRadius) + wallThickness,                        -((0.5*eosBaseWidth) - eosCornerThickness - wallThickness*cos(eosShellAngle)));
    eosCorner2->addVertex((-1.0*halfEosRadius),                                        -((0.5*eosBaseWidth) - eosCornerThickness));
    eosCorner2->addVertex((-1.0*halfEosRadius),                                        -((0.5*eosBaseWidth)));
    eosCorner2->addVertex((-1.0*halfEosRadius) + eosCornerThickness*sin(eosShellAngle),   -((0.5*eosBaseWidth) - eosCornerThickness*cos(eosShellAngle)));
  }

  



  // Compute total volume for material density
  std::string suffix = (m_staveType == INNER) ? "INNER" : "OUTER";

  double brlWallVolume = brlWall1->volume() + brlWall2->volume() + brlWall3->volume();
  const GeoMaterial* brlWallMaterial = matMgr()->getMaterialForVolume(matNameBrlWall, brlWallVolume, matNameBrlWall+suffix, matFudge);

  double brlCornerVolume = brlCorner1->volume() + brlCorner2->volume();
  const GeoMaterial* brlCornerMaterial = matMgr()->getMaterialForVolume(matNameBrlCorner, brlCornerVolume, matNameBrlCorner+suffix, matFudge);

  double eosVolume = 2.0*(eosWall1->volume() + eosWall2->volume() + eosWall3->volume()  + eosCorner1->volume() + eosCorner2->volume());
  
   
  // Logival and physical volumes of all solids
  GeoLogVol* brlWall1LogVol   = new GeoLogVol("brlWall1",   brlWall1,   brlWallMaterial);
  GeoLogVol* brlWall2LogVol   = new GeoLogVol("brlWall2",   brlWall2,   brlWallMaterial);
  GeoLogVol* brlWall3LogVol   = new GeoLogVol("brlWall3",   brlWall3,   brlWallMaterial);
  GeoLogVol* brlCorner1LogVol = new GeoLogVol("brlCorner1", brlCorner1, brlCornerMaterial);
  GeoLogVol* brlCorner2LogVol = new GeoLogVol("brlCorner2", brlCorner2, brlCornerMaterial);  

  GeoLogVol* eosWall1LogVol;
  GeoLogVol* eosWall2LogVol;
  GeoLogVol* eosWall3LogVol;
  GeoLogVol* eosCorner1LogVol;
  GeoLogVol* eosCorner2LogVol;

  if(matNameEos!=""){
    

    const GeoMaterial* eosMaterial = matMgr()->getMaterialForVolume(matNameEos, eosVolume, matNameEos+suffix, matFudge);
  
    eosWall1LogVol   = new GeoLogVol("eosWall1",   eosWall1,   eosMaterial);
    eosWall2LogVol   = new GeoLogVol("eosWall2",   eosWall2,   eosMaterial);
    eosWall3LogVol   = new GeoLogVol("eosWall3",   eosWall3,   eosMaterial);
    eosCorner1LogVol = new GeoLogVol("eosCorner1", eosCorner1, eosMaterial);
    eosCorner2LogVol = new GeoLogVol("eosCorner2", eosCorner2, eosMaterial);
  
  }
  else{

    ATH_MSG_DEBUG("###########################################################################");
    ATH_MSG_DEBUG("##Using approximations EOS material##");
   ATH_MSG_DEBUG("###########################################################################");

    eosWall1LogVol   = new GeoLogVol("eosWall1",   eosWall1,   brlWallMaterial);
    eosWall2LogVol   = new GeoLogVol("eosWall2",   eosWall2,   brlWallMaterial);
    eosWall3LogVol   = new GeoLogVol("eosWall3",   eosWall3,   brlWallMaterial);
    eosCorner1LogVol = new GeoLogVol("eosCorner1", eosCorner1, brlCornerMaterial);
    eosCorner2LogVol = new GeoLogVol("eosCorner2", eosCorner2, brlCornerMaterial);
  }

  GeoPhysVol* brlWall1PhysVol   = new GeoPhysVol(brlWall1LogVol);
  GeoPhysVol* brlWall2PhysVol   = new GeoPhysVol(brlWall2LogVol);
  GeoPhysVol* brlWall3PhysVol   = new GeoPhysVol(brlWall3LogVol);
  GeoPhysVol* brlCorner1PhysVol = new GeoPhysVol(brlCorner1LogVol);
  GeoPhysVol* brlCorner2PhysVol = new GeoPhysVol(brlCorner2LogVol);

  GeoPhysVol* eosWall1PhysVol   = new GeoPhysVol(eosWall1LogVol);
  GeoPhysVol* eosWall2PhysVol   = new GeoPhysVol(eosWall2LogVol);
  GeoPhysVol* eosWall3PhysVol   = new GeoPhysVol(eosWall3LogVol);
  GeoPhysVol* eosCorner1PhysVol = new GeoPhysVol(eosCorner1LogVol);
  GeoPhysVol* eosCorner2PhysVol = new GeoPhysVol(eosCorner2LogVol);

  
  // Add physical volumes to the longeron envelope
  GeoTransform* brlStaveTrf  = new GeoTransform(HepGeom::Translate3D(0.0, 0.0, 0.0));
  GeoTransform* eosAStaveTrf = new GeoTransform(HepGeom::Translate3D(0.0, 0.0, eosZPos));
  GeoTransform* eosCStaveTrf = new GeoTransform(HepGeom::Translate3D(0.0, 0.0, -eosZPos));


  m_physVol->add(brlStaveTrf);   m_physVol->add(brlWall1PhysVol);
  m_physVol->add(brlStaveTrf);   m_physVol->add(brlWall2PhysVol);
  m_physVol->add(brlStaveTrf);   m_physVol->add(brlWall3PhysVol);
  m_physVol->add(brlStaveTrf);   m_physVol->add(brlCorner1PhysVol);
  m_physVol->add(brlStaveTrf);   m_physVol->add(brlCorner2PhysVol);
  
  m_physVol->add(eosAStaveTrf);  m_physVol->add(eosWall1PhysVol);
  m_physVol->add(eosAStaveTrf);  m_physVol->add(eosWall2PhysVol);
  m_physVol->add(eosAStaveTrf);  m_physVol->add(eosWall3PhysVol);
  m_physVol->add(eosAStaveTrf);  m_physVol->add(eosCorner1PhysVol);
  m_physVol->add(eosAStaveTrf);  m_physVol->add(eosCorner2PhysVol);

  m_physVol->add(eosCStaveTrf);  m_physVol->add(eosWall1PhysVol);
  m_physVol->add(eosCStaveTrf);  m_physVol->add(eosWall2PhysVol);
  m_physVol->add(eosCStaveTrf);  m_physVol->add(eosWall3PhysVol);
  m_physVol->add(eosCStaveTrf);  m_physVol->add(eosCorner1PhysVol);
  m_physVol->add(eosCStaveTrf);  m_physVol->add(eosCorner2PhysVol);


  m_transform = HepGeom::Translate3D(radialMidpoint, 0.0, 0.0);
  return;
}



GeoVPhysVol* GeoPixelSlimStaveSupportInclRef::Build() {
  return m_physVol;
}
