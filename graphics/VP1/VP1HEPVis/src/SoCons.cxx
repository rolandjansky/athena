/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*-----------------------------HEPVis---------------------------------------*/
/*                                                                          */
/* Node:             SoCons                                                 */
/* Description:      Represents the G4Cons Geant Geometry entity            */
/* Author:           Joe Boudreau Nov 11 1996                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/

// this :
#include <VP1HEPVis/nodes/SoCons.h>
#include "RevolutionSurfaceUtil.h"

#include <cassert>
#include <cmath>
#include <Inventor/SbBox.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>

#include "SbMath.h"

// This statement is required
SO_NODE_SOURCE(SoCons)

// Constructor
SoCons::SoCons() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoCons);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(fRmin1,               (0.0));
  SO_NODE_ADD_FIELD(fRmin2,               (0.0));
  SO_NODE_ADD_FIELD(fRmax1,               (1.0));
  SO_NODE_ADD_FIELD(fRmax2,               (1.0));
  SO_NODE_ADD_FIELD(fDz,                 (10.0));
  SO_NODE_ADD_FIELD(fSPhi,                (0.0));
  SO_NODE_ADD_FIELD(fDPhi,             ((float)(2*M_PI)));
  SO_NODE_ADD_FIELD(smoothDraw,          (TRUE));
  SO_NODE_ADD_FIELD(pOverrideNPhi,       (0));
  SO_NODE_ADD_FIELD(alternateRep,        (NULL));
  m_children = new SoChildList(this);

  setNodeType(EXTENSION);
}

// Destructor
SoCons::~SoCons() {
  delete m_children;
}

//____________________________________________________________________
bool SoCons::s_didInit = false;
void SoCons::initClass()
{
  if ( !s_didInit ) {
    SO_NODE_INIT_CLASS(SoCons,SoShape,"Shape");
    s_didInit = true;
  }
}


// generatePrimitives
void SoCons::generatePrimitives(SoAction *action) {
  // This variable is used to store each vertex
  SoPrimitiveVertex pv;

  // Access the stat from the action
  SoState *state = action->getState();
  if (!state)
    return;

  // See if we have to use a texture coordinate function,
  // rather than generating explicit texture coordinates.
  SbBool useTexFunction=
    (SoTextureCoordinateElement::getType(state) ==
     SoTextureCoordinateElement::FUNCTION);

  // If we need to generate texture coordinates with a function,
  // we'll need an SoGLTextureCoordinateElement.  Otherwise, we'll
  // set up the coordinates directly.
  const SoTextureCoordinateElement *tce = NULL;
  SbVec4f texCoord;
  if (useTexFunction) {
    tce = SoTextureCoordinateElement::getInstance(state);
  } else {
    texCoord[2] = 0.0;
    texCoord[3] = 1.0;
  }
  SbVec3f point, normal;

  ///////////////////////////////////////////////////////
  //-----------------------------------------------------
#define GEN_VERTEX(pv,x,y,z,s,t,nx,ny,nz)               \
  point.setValue((float)(x),(float)(y),(float)(z));     \
  normal.setValue((float)(nx),(float)(ny),(float)(nz)); \
  if (useTexFunction) {                                 \
    texCoord=tce->get(point,normal);                    \
  } else {                                              \
    texCoord[0]=(float)(s);                             \
    texCoord[1]=(float)(t);                             \
  }                                                     \
  pv.setPoint(point);                                   \
  pv.setNormal(normal);                                 \
  pv.setTextureCoords(texCoord);                        \
  shapeVertex(&pv);
  //-----------------------------------------------------
  ///////////////////////////////////////////////////////

  int NPHI = RevolutionSurfaceUtil::nphiDivisions( fDPhi.getValue(), this->getComplexityValue(action), pOverrideNPhi.getValue() );

  double deltaPhi = fDPhi.getValue()/NPHI;
  double phi0     = fSPhi.getValue();
  double phi1     = phi0 + fDPhi.getValue();
  double rMax1    = fRmax1.getValue();
  double rMin1    = fRmin1.getValue();
  double rMax2    = fRmax2.getValue();
  double rMin2    = fRmin2.getValue();
  double zMax     = fDz.getValue();
  double zMin     = -zMax;
  double cosPhi0  = cos(phi0);
  double sinPhi0  = sin(phi0);
  double cosPhi1  = cos(phi1);
  double sinPhi1  = sin(phi1);
  double cosDeltaPhi = cos(deltaPhi);
  double sinDeltaPhi = sin(deltaPhi);
  //
  // The outer surface!
  //
  int    i;
  double sinPhi=sinPhi0;
  double cosPhi=cosPhi0;

  {
    double dR =rMax2-rMax1;
    double dZ =zMax-zMin;
    double cosTheta = -dR/sqrt(dR*dR+dZ*dZ);
    double sinTheta =  dZ/sqrt(dR*dR+dZ*dZ);

    beginShape(action,TRIANGLE_STRIP);
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMax2*cosPhi,rMax2*sinPhi,zMax,0.0,0.0,sinTheta*cosPhi,sinTheta*sinPhi,cosTheta);
      GEN_VERTEX(pv,rMax1*cosPhi,rMax1*sinPhi,zMin,1.0,1.0,sinTheta*cosPhi,sinTheta*sinPhi,cosTheta);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }
    endShape();
  }
  //
  // The inner surface!
  //

  sinPhi=sinPhi0;
  cosPhi=cosPhi0;

  {
    double dR =rMin2-rMin1;
    double dZ =zMax-zMin;
    double cosTheta = -dR/sqrt(dR*dR+dZ*dZ);
    double sinTheta =  dZ/sqrt(dR*dR+dZ*dZ);
    beginShape(action,TRIANGLE_STRIP);
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMin2*cosPhi,rMin2*sinPhi,zMax,0.0,0.0,-sinTheta*cosPhi,-sinTheta*sinPhi,-cosTheta);
      GEN_VERTEX(pv,rMin1*cosPhi,rMin1*sinPhi,zMin,1.0,1.0,-sinTheta*cosPhi,-sinTheta*sinPhi,-cosTheta);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }
    endShape();
  }
  if (fabs(deltaPhi)<2.0*M_PI) {
    //
    // The end
    //
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    GEN_VERTEX(pv,rMax2*cosPhi,rMax2*sinPhi,zMax,0.0,0.0,sinPhi,-cosPhi,0);
    GEN_VERTEX(pv,rMax1*cosPhi,rMax1*sinPhi,zMin,1.0,1.0,sinPhi,-cosPhi,0);
    GEN_VERTEX(pv,rMin2*cosPhi,rMin2*sinPhi,zMax,1.0,0.0,sinPhi,-cosPhi,0);
    GEN_VERTEX(pv,rMin1*cosPhi,rMin1*sinPhi,zMin,0.0,1.0,sinPhi,-cosPhi,0);
    endShape();
    //
    // The other end
    //
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi1;
    cosPhi=cosPhi1;
    GEN_VERTEX(pv,rMax2*cosPhi,rMax2*sinPhi, zMax,0.0,0.0,-sinPhi,+cosPhi,0);
    GEN_VERTEX(pv,rMax1*cosPhi,rMax1*sinPhi, zMin,1.0,1.0,-sinPhi,+cosPhi,0);
    GEN_VERTEX(pv,rMin2*cosPhi,rMin2*sinPhi, zMax,1.0,0.0,-sinPhi,+cosPhi,0);
    GEN_VERTEX(pv,rMin1*cosPhi,rMin1*sinPhi, zMin,0.0,1.0,-sinPhi,+cosPhi,0);
    endShape();
  }

  //
  // The outer surface at z=+PDZ
  //
  beginShape(action,TRIANGLE_STRIP);
  sinPhi=sinPhi0;
  cosPhi=cosPhi0;
  for (i = 0; i<=NPHI; i++) {
    GEN_VERTEX(pv,rMin2*cosPhi,rMin2*sinPhi,zMax,0.0,0.0,0,0,1);
    GEN_VERTEX(pv,rMax2*cosPhi,rMax2*sinPhi,zMax,1.0,1.0,0,0,1);
    inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
  }
  endShape();
  //
  // The outer surface at z=-PDZ
  //
  beginShape(action,TRIANGLE_STRIP);
  sinPhi=sinPhi0;
  cosPhi=cosPhi0;
  for (i = 0; i<=NPHI; i++) {
    GEN_VERTEX(pv,rMin1*cosPhi,rMin1*sinPhi,zMin,0.0,0.0,0,0,-1);
    GEN_VERTEX(pv,rMax1*cosPhi,rMax1*sinPhi,zMin,1.0,1.0,0,0,-1);
    inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
  }
  endShape();

  if (state&&state->isElementEnabled(SoGLCacheContextElement::getClassStackIndex())) {
    //Encourage auto caching
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DO_AUTO_CACHE);
#if ((COIN_MAJOR_VERSION>=3)||((COIN_MAJOR_VERSION==2)&&(COIN_MINOR_VERSION>=5)))
    SoGLCacheContextElement::incNumShapes(state);
#endif
  }
}

// getChildren
SoChildList *SoCons::getChildren() const {
  return m_children;
}


// computeBBox
void SoCons::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  RevolutionSurfaceUtil::setBBoxPars(fSPhi.getValue(), fDPhi.getValue(),
				     std::min(fRmin1.getValue(),fRmin1.getValue()),
				     std::max(fRmax1.getValue(),fRmax1.getValue()),
				     -fDz.getValue(),fDz.getValue(),
				     box, center );
}




// updateChildren
void SoCons::updateChildren() {


  // Redraw the G4Cons....

  assert(m_children->getLength()==1);
  SoSeparator       *sep                = (SoSeparator *)  ( *m_children)[0];
  SoCoordinate3     *theCoordinates     = (SoCoordinate3 *)      ( sep->getChild(0));
  SoNormal          *theNormals         = (SoNormal *)           ( sep->getChild(1));
  SoNormalBinding   *theNormalBinding   = (SoNormalBinding *)    ( sep->getChild(2));
  SoIndexedFaceSet  *theFaceSet         = (SoIndexedFaceSet *)   ( sep->getChild(3));

  const int NPHI=96, NPOINTS=2*(2*NPHI+2), NFACES=4*NPHI+2, NINDICES = NFACES*5;
  float points[NPOINTS][3], normals[NFACES][3];
#ifdef INVENTOR2_0
  static long     indices[NINDICES];
#else
  static int32_t  indices[NINDICES];
#endif
  static int init=0;
  double phi, pp, DeltaPhi;

  // Indices need to be generated once! This is here to keep it close to the point
  // generation, since otherwise it will be confusing.

  int i;
  if (!init) {
    init = 1;
    // Outer face
    for (i = 0; i< NPHI; i++) {
      // 0 1 3 2;
      indices[5*i+0] =  2*i+0;
      indices[5*i+1] =  2*i+1;
      indices[5*i+2] =  2*i+3;
      indices[5*i+3] =  2*i+2;
      indices[5*i+4] = SO_END_FACE_INDEX;
    }
    // the inner face
    for (i=0;i<NPHI;i++) {
      indices[5*1*NPHI + 5*i+0] = 2*NPHI+2 + 2*i+0;
      indices[5*1*NPHI + 5*i+1] = 2*NPHI+2 + 2*i+1;
      indices[5*1*NPHI + 5*i+2] = 2*NPHI+2 + 2*i+3;
      indices[5*1*NPHI + 5*i+3] = 2*NPHI+2 + 2*i+2;
      indices[5*1*NPHI + 5*i+4] = SO_END_FACE_INDEX;
    }
    // the top side
    for (i=0;i<NPHI;i++) {
      indices[5*2*NPHI + 5*i+0] = 2*i+0;
      indices[5*2*NPHI + 5*i+1] = 2*i+2;
      indices[5*2*NPHI + 5*i+2] = NPOINTS - (2*i+4);
      indices[5*2*NPHI + 5*i+3] = NPOINTS - (2*i+2);
      indices[5*2*NPHI + 5*i+4] = SO_END_FACE_INDEX;
    }
    // the bottom side
    for (i=0;i<NPHI;i++) {
      indices[5*3*NPHI + 5*i+0] = 2*i+1;
      indices[5*3*NPHI + 5*i+1] = NPOINTS - (2*i+1);
      indices[5*3*NPHI + 5*i+2] = NPOINTS - (2*i+3);
      indices[5*3*NPHI + 5*i+3] = 2*i+3;
      indices[5*3*NPHI + 5*i+4] = SO_END_FACE_INDEX;
    }
    // the odd side
    indices[5*4*NPHI +0] = 2*NPHI;
    indices[5*4*NPHI +1] = 2*NPHI+1;
    indices[5*4*NPHI +2] = 2*NPHI+3;
    indices[5*4*NPHI +3] = 2*NPHI+2;
    indices[5*4*NPHI +4] = SO_END_FACE_INDEX;
    // aother odd side
    indices[5*4*NPHI +5 +0] = 0;
    indices[5*4*NPHI +5 +1] = NPOINTS-2;
    indices[5*4*NPHI +5 +2] = NPOINTS-1;
    indices[5*4*NPHI +5 +3] = 1;
    indices[5*4*NPHI +5 +4] = SO_END_FACE_INDEX;
  }
  // Points need to be generated each time:
  // The outer surface
  DeltaPhi = fDPhi.getValue()/NPHI, phi = fSPhi.getValue();
  float  t,st,ct;
  t = FATAN((fRmax2.getValue()-fRmax1.getValue())/(2*fDz.getValue()));
  st = FSIN(t);
  ct = FCOS(t);
  for (i = 0; i<=NPHI; i++) {
    points[2*i+0][0] = fRmax2.getValue()*FCOS(phi);
    points[2*i+0][1] = fRmax2.getValue()*FSIN(phi);
    points[2*i+0][2] = +fDz.getValue();
    points[2*i+1][0] = fRmax1.getValue()*FCOS(phi);
    points[2*i+1][1] = fRmax1.getValue()*FSIN(phi);
    points[2*i+1][2] = -fDz.getValue();
    pp = phi+DeltaPhi/2.0;
    if (i!=NPHI) {
      normals[i][0] = ct * FCOS(pp);
      normals[i][1] = ct * FSIN(pp);
      normals[i][2] = -st;
    }
    phi+=DeltaPhi;
  }
  // The inner surface
  phi = fSPhi.getValue() + fDPhi.getValue();
  t = FATAN((fRmin2.getValue()-fRmin1.getValue())/(2*fDz.getValue()));
  st = FSIN(t);
  ct = FCOS(t);
  for (i = 0; i<=NPHI; i++) {
    points[2*NPHI+2+2*i+0][0] = fRmin2.getValue()*FCOS(phi);
    points[2*NPHI+2+2*i+0][1] = fRmin2.getValue()*FSIN(phi);
    points[2*NPHI+2+2*i+0][2] = +fDz.getValue();
    points[2*NPHI+2+2*i+1][0] = fRmin1.getValue()*FCOS(phi);
    points[2*NPHI+2+2*i+1][1] = fRmin1.getValue()*FSIN(phi);
    points[2*NPHI+2+2*i+1][2] = -fDz.getValue();
    pp = phi-DeltaPhi/2.0;
    if (i!=NPHI) {
      normals[NPHI+i][0] = -ct*FCOS(pp);
      normals[NPHI+i][1] = -ct*FSIN(pp);
      normals[NPHI+i][2] = st;
    }
    phi-=DeltaPhi;
  }
  // The top side
  for (i=0;i<NPHI;i++) {
    normals[2*NPHI+i][0]=normals[2*NPHI+i][1]=0;
    normals[2*NPHI+i][2]=  1.0;
  }
  // The bottom side
  for (i=0;i<NPHI;i++) {
    normals[3*NPHI+i][0]=normals[3*NPHI+i][1]=0;
    normals[3*NPHI+i][2]= -1.0;
  }
  // The odd side
  phi = fSPhi.getValue();
  normals[4*NPHI+0][0]=  FSIN(phi);
  normals[4*NPHI+0][1]= -FCOS(phi);
  normals[4*NPHI+0][2]= 0;

  // Another odd side
  phi = fSPhi.getValue()+fDPhi.getValue();
  normals[4*NPHI+1][0]= -FSIN(phi);
  normals[4*NPHI+1][1]= +FCOS(phi);
  normals[4*NPHI+1][2]=0;

  for (int np=0;np<NPOINTS;np++) theCoordinates->point.set1Value(np,points[np][0],points[np][1],points[np][2]);
  theFaceSet->coordIndex.setValues(0,NINDICES,indices);
  if (smoothDraw.getValue()) {
    //    This Line is replaced by the next one because of an apparent Bug in Inventor (mem. leak).
    //    theNormals->vector.deleteValues(0);
    for (int nf=0;nf<NFACES;nf++) theNormals->vector.set1Value(nf,normals[nf][0],normals[nf][1],normals[nf][2]);
    theNormalBinding->value=SoNormalBinding::PER_FACE;
  }
  else {
    for (int nf=0;nf<NFACES;nf++) theNormals->vector.set1Value(nf,normals[nf][0],normals[nf][1],normals[nf][2]);
    theNormalBinding->value=SoNormalBinding::PER_FACE;
  }
}

// generateChildren
void SoCons::generateChildren() {

  // This routines creates one SoSeparator, one SoCoordinate3, and
  // one SoLineSet, and puts it in the child list.  This is done only
  // once, whereas redrawing the position of the coordinates occurs each
  // time an update is necessary, in the updateChildren routine.

  assert(m_children->getLength() ==0);
  SoSeparator      *sep              = new SoSeparator();
  SoCoordinate3    *theCoordinates   = new SoCoordinate3();
  SoNormal         *theNormals       = new SoNormal();
  SoNormalBinding  *theNormalBinding = new SoNormalBinding();
  SoIndexedFaceSet *theFaceSet       = new SoIndexedFaceSet();
  //
  // This line costs some in render quality! but gives speed.
  //
  sep->addChild(theCoordinates);
  sep->addChild(theNormals);
  sep->addChild(theNormalBinding);
  sep->addChild(theFaceSet);
  m_children->append(sep);
}

// generateAlternateRep
void SoCons::generateAlternateRep() {

  // This routine sets the alternate representation to the child
  // list of this mode.

  if (m_children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *m_children)[0]);
}

// clearAlternateRep
void SoCons::clearAlternateRep() {
  alternateRep.setValue(NULL);
}
