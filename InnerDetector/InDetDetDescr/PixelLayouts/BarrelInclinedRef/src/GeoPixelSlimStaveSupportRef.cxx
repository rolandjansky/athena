/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  // Access XML file
  PixelInclRefStaveXMLHelper staveDBHelper(m_innerLayer, getBasics());
  std::string matName = staveDBHelper.getStaveSupportMaterial(m_staveShapeIndex);

  // Print parameters to screen
  //msg(MSG::DEBUG)<<"Stave sizes LxWxT: " << m_length << "  " << m_width << "  " << m_thickness << "   " << matName << endreq;
  //msg(MSG::DEBUG)<<"   EC radial pos : " << m_endcapMinRadialPos << " " << m_endcapMaxRadialPos << " -> " << ecRadialPos << "   " << m_svcRouting << endreq;

  const GeoShape * staveShape = NULL;

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
  double shellThickness  = staveDBHelper.getShellThickness(m_staveShapeIndex);
  double brlStepLowR     = staveDBHelper.getXStepLowR(m_staveShapeIndex);
  double brlStepHighR    = staveDBHelper.getXStepHighR(m_staveShapeIndex);
  double eosLength       = (staveDBHelper.getStaveSupportLength() * 0.5) - (brlZMax + m_planarGap);
  double eosZPos         = brlZMax + m_planarGap + (eosLength *  0.5);
  double staveXOffset    = 0.0;
  double eosStaveXOffset = 0.0;
  // msg(MSG::DEBUG)  << eosBaseWidth  << ", " << eosTopWidth   << ", " << brlWidth  << ", " <<  brlZMax   << ", " << radialLength   << ", " << shellThickness   << ", " << eosXOffset  << ", " << endreq;

  // Validate parameters
  //if (eosTopWidth > eosBaseWidth)  msg(MSG::DEBUG) << "Stave eos element is upside down.  Check code/xml" << endreq;
  
  GeoSimplePolygonBrep *brlStaveShape = new GeoSimplePolygonBrep(brlZMax + m_planarGap);
  GeoSimplePolygonBrep *eosStaveShape = new GeoSimplePolygonBrep( 0.5*eosLength );  

  double outerOffsetMid  = 0.25*(eosTopWidth - eosBaseWidth);           // Dy<0 if top width < bottom width, i.e. sides point inwards
  double eosShellAngle   = atan( (0.5*radialLength) / outerOffsetMid);  // if Dy < 0, theta < 0
  double innerOffsetMid  = outerOffsetMid - fabs(shellThickness / sin(eosShellAngle)); 
  double innerOffsetBase = (outerOffsetMid < 0) ? fabs(shellThickness/ tan(eosShellAngle/2.0)) : shellThickness*tan(eosShellAngle/2.0); 
  double innerOffsetTop  = (outerOffsetMid < 0) ? fabs(shellThickness*tan(eosShellAngle/2.0))  : shellThickness/ tan(eosShellAngle/2.0); 

  if (m_staveType == INNER){
    staveXOffset = (0.5*radialLength + brlStepHighR);

    // Barrel outer stave profile
    brlStaveShape->addVertex(0.0,                                0.5*brlWidth                  ); // 8
    brlStaveShape->addVertex(-0.5*radialLength - brlStepHighR,   0.5*brlWidth                  ); // 7
    brlStaveShape->addVertex(-0.5*radialLength - brlStepHighR,   0.5*brlWidth  - shellThickness); // 6
    brlStaveShape->addVertex(-shellThickness,                    0.5*brlWidth  - shellThickness); // 5
    brlStaveShape->addVertex(-shellThickness,                  (-0.5*brlWidth) + shellThickness); // 4
    brlStaveShape->addVertex(-0.5*radialLength - brlStepHighR, (-0.5*brlWidth) + shellThickness); // 3
    brlStaveShape->addVertex(-0.5*radialLength - brlStepHighR,  -0.5*brlWidth                  ); // 2
    brlStaveShape->addVertex(0.0,                               -0.5*brlWidth                  ); // 1

    eosStaveShape->addVertex( 0.0,               (0.5*eosTopWidth)                     ); // 8
    eosStaveShape->addVertex(-0.5*radialLength,  (0.5*eosBaseWidth) + outerOffsetMid   ); // 7
    eosStaveShape->addVertex(-0.5*radialLength,  (0.5*eosBaseWidth) + innerOffsetMid   ); // 6
    eosStaveShape->addVertex(-shellThickness,     (0.5*eosTopWidth) - innerOffsetTop   ); // 5
    eosStaveShape->addVertex(-shellThickness,   -((0.5*eosTopWidth) - innerOffsetTop)  ); // 4
    eosStaveShape->addVertex(-0.5*radialLength, -((0.5*eosBaseWidth) + innerOffsetMid) ); // 3
    eosStaveShape->addVertex(-0.5*radialLength, -((0.5*eosBaseWidth) + outerOffsetMid) ); // 2
    eosStaveShape->addVertex( 0.0,              - (0.5*eosTopWidth)                    ); // 1
    eosStaveXOffset = 0.0 - brlStepHighR;
  }
  else {
    // offset to ensure that radial length is divided equally between layers)
    staveXOffset =  -(0.5*radialLength + brlStepLowR); // offset so that barrel outer edge sits at layer mid
    brlStaveShape->addVertex(0.0,                            (-0.5*brlWidth)                 ); // 8
    brlStaveShape->addVertex(0.5*radialLength + brlStepLowR, (-0.5*brlWidth)                 ); // 7
    brlStaveShape->addVertex(0.5*radialLength + brlStepLowR, (-0.5*brlWidth) + shellThickness); // 6
    brlStaveShape->addVertex(shellThickness,                 (-0.5*brlWidth) + shellThickness); // 5
    brlStaveShape->addVertex(shellThickness,                  0.5*brlWidth  - shellThickness); // 4
    brlStaveShape->addVertex(0.5*radialLength + brlStepLowR,  0.5*brlWidth  - shellThickness); // 3
    brlStaveShape->addVertex(0.5*radialLength + brlStepLowR,  0.5*brlWidth                  ); // 2
    brlStaveShape->addVertex(0.0,                             0.5*brlWidth                  ); // 1



    // EOS stave profile - modified form of barrel stave profile
    // Coordinates run anticlockwise from 1
    //    7/ /6   3\ \2      ^ x
    //    / /5_____4\ \      |
    //  8/_____________\1    |---> y
    eosStaveShape->addVertex(0.0,              - (0.5*eosBaseWidth)                    ); // 8
    eosStaveShape->addVertex(0.5*radialLength, -((0.5*eosBaseWidth) + outerOffsetMid)  ); // 7
    eosStaveShape->addVertex(0.5*radialLength, -((0.5*eosBaseWidth) + innerOffsetMid)  ); // 6
    eosStaveShape->addVertex(shellThickness,   -((0.5*eosBaseWidth) - innerOffsetBase) ); // 5
    eosStaveShape->addVertex(shellThickness,     (0.5*eosBaseWidth) - innerOffsetBase  ); // 4
    eosStaveShape->addVertex(0.5*radialLength,   (0.5*eosBaseWidth) + innerOffsetMid   ); // 3
    eosStaveShape->addVertex(0.5*radialLength,   (0.5*eosBaseWidth) + outerOffsetMid   ); // 2
    eosStaveShape->addVertex(0.0,                (0.5*eosBaseWidth)                    ); // 1
    eosStaveXOffset = brlStepLowR;
  }

  HepGeom::Transform3D brlStaveTrf = HepGeom::Translate3D(0,0,0);
  staveShape = addShape(staveShape, brlStaveShape, brlStaveTrf); 
  
  HepGeom::Transform3D eosAStaveTrf = HepGeom::Translate3D(eosStaveXOffset, 0.0,  eosZPos);
  staveShape = addShape(staveShape, eosStaveShape, eosAStaveTrf); 

  HepGeom::Transform3D eosCStaveTrf = HepGeom::Translate3D(eosStaveXOffset, 0.0, -eosZPos);
  staveShape = addShape(staveShape, eosStaveShape, eosCStaveTrf); 
  
  // Identify material and construct logical volume
  const GeoMaterial* material = matMgr()->getMaterialForVolume(matName, brlStaveShape->volume() + (2*eosStaveShape->volume()));
  // const GeoMaterial* material = matMgr()->getMaterialForVolume(matName, staveShape->volume());
  // const GeoMaterial* material = matMgr()->getMaterial("std::Air");

  GeoLogVol* logVol = new GeoLogVol("StaveSupport", staveShape, material);
  m_transform = HepGeom::Translate3D(radialMidpoint + staveXOffset, 0.0, 0.0);
  m_physVol = new GeoPhysVol(logVol);
  

  return;
}



GeoVPhysVol* GeoPixelSlimStaveSupportInclRef::Build() {
  return m_physVol;
}



const GeoShape* GeoPixelSlimStaveSupportInclRef::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans)
{
  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}
