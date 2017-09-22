/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*-----------------------------Hepvis---------------------------------------*/
/*                                                                          */
/* Node:             SoPcons                                                */
/* Description:      Represents the G4Pcons Geant Geometry entity           */
/* Author:           Joe Boudreau Nov 11 1996                               */
/* Update:  Giorgi   Gvaberidze (ggvaberi@cern.ch)                          */ 
/*          April    2011                                                   */
/*          Fixed:   Generating of alternate rep,  for export in *.iv file  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#include <VP1HEPVis/nodes/SoPcons.h>
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
#include <Inventor/C/glue/gl.h>


// This statement is required
SO_NODE_SOURCE(SoPcons)

// Pconstructor
SoPcons::SoPcons() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoPcons);
  
  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(fRmin,                (0.0));
  SO_NODE_ADD_FIELD(fRmax,                (0.0));
  SO_NODE_ADD_FIELD(fDz,                  (0.0));
  SO_NODE_ADD_FIELD(fSPhi,                (0.0));
  SO_NODE_ADD_FIELD(fDPhi,             ((float)(2*M_PI)));
  SO_NODE_ADD_FIELD(smoothDraw,          (TRUE));
  SO_NODE_ADD_FIELD(pOverrideNPhi,       (0));
  SO_NODE_ADD_FIELD(alternateRep,        (NULL));
  SO_NODE_ADD_FIELD(drawEdgeLines,       (false));

  m_children = new SoChildList(this);
  
  float rMinDef[]={10.0,  15.0, 10.0};
  float rMaxDef[]={11.0,  17.0, 12.0};
  float zDef   []={-10.0,  0.0, 10.0};
  
  fRmin.setValues(0,2,rMinDef);
  fRmax.setValues(0,2,rMaxDef);
  fDz.setValues(0,2,zDef);
  setNodeType(EXTENSION);
}

// Destructor
SoPcons::~SoPcons() {
  delete m_children;
}


//____________________________________________________________________
bool SoPcons::s_didInit = false;
void SoPcons::initClass()
{
  if ( !s_didInit ) {
    SO_NODE_INIT_CLASS(SoPcons,SoShape,"Shape");
    s_didInit = true;
  }
}

// generatePrimitives
void SoPcons::generatePrimitives(SoAction *action) {
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
  
  if (fRmin.getNum()!=fRmax.getNum() || fRmax.getNum() != fDz.getNum()) {
    return;
  }
  
  int nSeg = fRmin.getNum()-1;
  if (nSeg<1)  {
    return;
  }
  
  for (int p=0;p<nSeg;p++) {
    
    double rMin1= fRmin[p];
    double rMin2= fRmin[p+1];
    
    double rMax1= fRmax[p];
    double rMax2= fRmax[p+1];
    
    double zMin = fDz[p];
    double zMax= fDz[p+1];
    
    int NPHI = RevolutionSurfaceUtil::nphiDivisions( fDPhi.getValue(), this->getComplexityValue(action), pOverrideNPhi.getValue() );
    
    double deltaPhi = fDPhi.getValue()/NPHI;
    double phi0     = fSPhi.getValue();
    double phi1     = phi0 + fDPhi.getValue();
    double cosPhi0  = cos(phi0);
    double sinPhi0  = sin(phi0);
    double cosPhi1  = cos(phi1);
    double sinPhi1  = sin(phi1);
    double cosDeltaPhi = cos(deltaPhi);
    double sinDeltaPhi = sin(deltaPhi);
    
    int    i;
    double sinPhi;
    double cosPhi;
    
    const bool noPhiCutout=fabs(fDPhi.getValue())==0.F || fabs(fabs(fDPhi.getValue())-2.0*M_PI)<0.01; // FIXME - better way to do this?
    const bool disableLighting(glIsEnabled(GL_LIGHTING));
    const bool transparencyOn(glIsEnabled(GL_BLEND));
    
    
    //
    // The outer surface!
    //
    //    if (1)
    //    {
    double dR =rMax2-rMax1;
    double dZ =zMax-zMin;
    double cosTheta = -dR/sqrt(dR*dR+dZ*dZ);
    double sinTheta =  dZ/sqrt(dR*dR+dZ*dZ);
    
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMax2*cosPhi,rMax2*sinPhi,zMax,0.0,0.0,sinTheta*cosPhi,sinTheta*sinPhi,cosTheta);
      GEN_VERTEX(pv,rMax1*cosPhi,rMax1*sinPhi,zMin,1.0,1.0,sinTheta*cosPhi,sinTheta*sinPhi,cosTheta);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }
    endShape();
    
    if (drawEdgeLines.getValue()) {      
      if (disableLighting) glDisable(GL_LIGHTING);
      if (transparencyOn) glDisable(GL_BLEND);
      glBegin(GL_LINE_STRIP);      
      sinPhi=sinPhi0;
      cosPhi=cosPhi0;
      if (!noPhiCutout && p==0) glVertex3f(rMin1*cosPhi,rMin1*sinPhi,zMin); // only draw if phi range
      for (i = 0; i<=NPHI; i++) {
        glVertex3f(rMax1*cosPhi,rMax1*sinPhi,zMin);
        inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
      }
      if (!noPhiCutout && p==0) glVertex3f(rMin1*cosPhi1,rMin1*sinPhi1,zMin); // only draw if phi range
      glEnd();
      if (disableLighting) glEnable(GL_LIGHTING);
      if (transparencyOn) glEnable(GL_BLEND);
    }
    
    //}
    //    if (1)
    //
    // The inner surface!
    //
    //    {
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    
     dR =rMin2-rMin1;
     dZ =zMax-zMin;
     cosTheta = -dR/sqrt(dR*dR+dZ*dZ);
     sinTheta =  dZ/sqrt(dR*dR+dZ*dZ);
    
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMin2*cosPhi,rMin2*sinPhi,zMax,0.0,0.0,-cosPhi*sinTheta,-sinPhi*sinTheta,-cosTheta);
      GEN_VERTEX(pv,rMin1*cosPhi,rMin1*sinPhi,zMin,1.0,1.0,-cosPhi*sinTheta,-sinPhi*sinTheta,-cosTheta);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }
    endShape();
    
    if (drawEdgeLines.getValue()) {      
      if (disableLighting) glDisable(GL_LIGHTING);
      if (transparencyOn) glDisable(GL_BLEND);
      glBegin(GL_LINE_STRIP);      
      sinPhi=sinPhi0;
      cosPhi=cosPhi0;
//      if (!noPhiCutout) glVertex3f(rMin1*cosPhi,rMin1*sinPhi,zMin); // only draw if phi range
      for (i = 0; i<=NPHI; i++) {
        glVertex3f(rMin1*cosPhi,rMin1*sinPhi,zMin);
        inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
      }
//      if (!noPhiCutout) glVertex3f(rMin1*cosPhi1,rMin1*sinPhi1,zMin); // only draw if phi range
      glEnd();
      if (disableLighting) glEnable(GL_LIGHTING);
      if (transparencyOn) glEnable(GL_BLEND);
    }
    
    //    }
    
//    if (1) {
      if (!noPhiCutout) {
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
        
        if (drawEdgeLines.getValue()) {
          if (disableLighting) glDisable(GL_LIGHTING);
          if (transparencyOn) glDisable(GL_BLEND);
          
          glBegin(GL_LINES);
          glVertex3f(rMax2*cosPhi0,rMax2*sinPhi0, zMax);
          glVertex3f(rMax1*cosPhi0,rMax1*sinPhi0, zMin);
          glVertex3f(rMin2*cosPhi0,rMin2*sinPhi0, zMax);
          glVertex3f(rMin1*cosPhi0,rMin1*sinPhi0, zMin);
          glVertex3f(rMax2*cosPhi1,rMax2*sinPhi1, zMax);
          glVertex3f(rMax1*cosPhi1,rMax1*sinPhi1, zMin);
          glVertex3f(rMin2*cosPhi1,rMin2*sinPhi1, zMax);
          glVertex3f(rMin1*cosPhi1,rMin1*sinPhi1, zMin);
          glEnd();
          if (disableLighting) glEnable(GL_LIGHTING);
          if (transparencyOn) glEnable(GL_BLEND);
        }
        
      }
//    }
    if (p==(nSeg-1)) {
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
      
      if (drawEdgeLines.getValue()) {      
        if (disableLighting) glDisable(GL_LIGHTING);
        if (transparencyOn) glDisable(GL_BLEND);
        glBegin(GL_LINE_STRIP);      
        sinPhi=sinPhi0;
        cosPhi=cosPhi0;
        if (!noPhiCutout) glVertex3f(rMin1*cosPhi,rMin1*sinPhi,zMax); // only draw if phi range
        for (i = 0; i<=NPHI; i++) {
          glVertex3f(rMax1*cosPhi,rMax1*sinPhi,zMax);
          inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
        }
        if (!noPhiCutout) glVertex3f(rMin1*cosPhi1,rMin1*sinPhi1,zMax); // only draw if phi range
        glEnd();
        if (disableLighting) glEnable(GL_LIGHTING);
        if (transparencyOn) glEnable(GL_BLEND);
      }
      
    }
    if (p==0) {
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
    }
  }
  
  if (state&&state->isElementEnabled(SoGLCacheContextElement::getClassStackIndex())) {
    //Encourage auto caching
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DO_AUTO_CACHE);
#if ((COIN_MAJOR_VERSION>=3)||((COIN_MAJOR_VERSION==2)&&(COIN_MINOR_VERSION>=5)))
    SoGLCacheContextElement::incNumShapes(state);
#endif
  }
}

// getChildren
SoChildList *SoPcons::getChildren() const {
  return m_children;
}


// computeBBox
void SoPcons::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  if (fRmax.getNum()< 2) return;
  if (fRmin.getNum()< 2) return;
  if (fDz.getNum()  < 2) return;
  
  double MinMin = fRmin[0];
  double MaxMax = fRmax[0];
  
  double ZMin = fDz[0];
  double ZMax = fDz[0];
  
  
  for (int i=1;i<fRmin.getNum();++i) {
    if (fRmin[i]<MinMin) MinMin=fRmin[i];
  }
  
  for (int i=1;i<fRmax.getNum();++i) {
    if (fRmax[i]>MaxMax) MaxMax=fRmax[i];
  }
  
  for (int i=1;i<fDz.getNum();++i) {
    if (fDz[i]>ZMax) ZMax=fDz[i];
    if (fDz[i]<ZMin) ZMin=fDz[i];
  }
  
  RevolutionSurfaceUtil::setBBoxPars(fSPhi.getValue(), fDPhi.getValue(),
                                     MinMin, MaxMax,
                                     ZMin,ZMax,
                                     box, center );
}




// updateChildren
void SoPcons::updateChildren() {
  
#ifdef IMPLEMENTED
  
  // (Not implemented so you do not get an alternate rep.  Too bad.)
  
  // Redraw the G4Pcons....
  
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
  
  double fRmax1, 
         fRmax2, 
         fRMin1, 
         fRmin2;
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
#endif
}

// generateChildren
void SoPcons::generateChildren() {
#ifdef IMPLEMENTED
  // (Not implemented so you do not get an alternate rep.  Too bad.)
  
  
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
#endif
}

// generateAlternateRep
#include <vector>

void SoPcons::generateAlternateRep() {
  
  // This routine sets the alternate representation to the child
  // list of this mode.
  
  //if (m_children->getLength() == 0) generateChildren();
  //updateChildren();
  //  alternateRep.setValue((SoSeparator *)  ( *m_children)[0]);


  /*         updated for generate fullfill alternaterep 
   ***********************************************
   As First we collect point and triangle strip 
   bounds same as in draw function.
   there is copied draw function contents and 
   replased unused part;

   Source code was taked from render part, and modified
   for collect nessesary information, and build alternate
   geometry.
   **********************************************
   */
  const int NPHI = 96; 

  // This variable is used to store each vertex
  SoPrimitiveVertex pv;

  //For collect points in Pcon geometry chunk
  std::vector<SoPrimitiveVertex> vls; //Vertex List

  //For collect all triangles
  std::vector<SoPrimitiveVertex> fls; //Face list. face here is qudrangle

  // See if we have to use a texture coordinate function,
  // rather than generating explicit texture coordinates.

  SbVec4f texCoord;
  texCoord[2] = 0.0;
  texCoord[3] = 1.0;
  SbVec3f point, normal;

  ///////////////////////////////////////////////////////
  //-----------------------------------------------------
#define N_GEN_VERTEX(ls, pv,x,y,z,s,t,nx,ny,nz)		\
  point.setValue((float)(x),(float)(y),(float)(z));     \
  normal.setValue((float)(nx),(float)(ny),(float)(nz)); \
  texCoord[0]=(float)(s);                               \
  texCoord[1]=(float)(t);                               \
  pv.setPoint(point);                                   \
  pv.setNormal(normal);                                 \
  pv.setTextureCoords(texCoord);                        \
  ls.push_back(pv);
  //-----------------------------------------------------
  ///////////////////////////////////////////////////////
  if (fRmin.getNum()!=fRmax.getNum() || fRmax.getNum() != fDz.getNum()) {
    return;
  }
  
  int nSeg = fRmin.getNum()-1;
  if (nSeg<1)  {
    return;
  }
  
  for (int p=0;p<nSeg;p++) {
    
    double rMin1= fRmin[p];
    double rMin2= fRmin[p+1];
    
    double rMax1= fRmax[p];
    double rMax2= fRmax[p+1];
    
    double zMin = fDz[p];
    double zMax= fDz[p+1];
    
    double deltaPhi = fDPhi.getValue()/NPHI;
    double phi0     = fSPhi.getValue();
    double phi1     = phi0 + fDPhi.getValue();
    double cosPhi0  = cos(phi0);
    double sinPhi0  = sin(phi0);
    double cosPhi1  = cos(phi1);
    double sinPhi1  = sin(phi1);
    double cosDeltaPhi = cos(deltaPhi);
    double sinDeltaPhi = sin(deltaPhi);
    
    int    i;
    double sinPhi;
    double cosPhi;
    
    const bool noPhiCutout=fabs(fDPhi.getValue())==0.F || fabs(fabs(fDPhi.getValue())-2.0*M_PI)<0.01; // FIXME - better way to do this?
    
    //
    // The outer surface!
    //
    double dR =rMax2-rMax1;
    double dZ =zMax-zMin;
    double cosTheta = -dR/sqrt(dR*dR+dZ*dZ);
    double sinTheta =  dZ/sqrt(dR*dR+dZ*dZ);
    
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    for (i = 0; i<=NPHI; i++) {
      N_GEN_VERTEX(vls,pv,rMax2*cosPhi,rMax2*sinPhi,zMax,0.0,0.0,sinTheta*cosPhi,sinTheta*sinPhi,cosTheta);
      N_GEN_VERTEX(vls,pv,rMax1*cosPhi,rMax1*sinPhi,zMin,1.0,1.0,sinTheta*cosPhi,sinTheta*sinPhi,cosTheta);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }

    for(i = 0; i < ((int)vls.size() - 2); i += 2){
      fls.push_back(vls[i + 0]);
      fls.push_back(vls[i + 1]);
      fls.push_back(vls[i + 3]);
      fls.push_back(vls[i + 2]);
    }
    vls.clear();
    
    //
    // The inner surface!
    //
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    
     dR =rMin2-rMin1;
     dZ =zMax-zMin;
     cosTheta = -dR/sqrt(dR*dR+dZ*dZ);
     sinTheta =  dZ/sqrt(dR*dR+dZ*dZ);
    
    for (i = 0; i<=NPHI; i++) {
      N_GEN_VERTEX(vls,pv,rMin2*cosPhi,rMin2*sinPhi,zMax,0.0,0.0,-cosPhi*sinTheta,-sinPhi*sinTheta,-cosTheta);
      N_GEN_VERTEX(vls,pv,rMin1*cosPhi,rMin1*sinPhi,zMin,1.0,1.0,-cosPhi*sinTheta,-sinPhi*sinTheta,-cosTheta);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }
    for(i = 0; i < ((int)vls.size() - 2); i += 2){
      fls.push_back(vls[i + 1]);
      fls.push_back(vls[i + 0]);
      fls.push_back(vls[i + 2]);
      fls.push_back(vls[i + 3]);
    }
    vls.clear();
    
    
    if (!noPhiCutout) {
        //
        // The end
        //
        sinPhi=sinPhi0;
        cosPhi=cosPhi0;
        N_GEN_VERTEX(vls, pv,rMax2*cosPhi,rMax2*sinPhi,zMax,0.0,0.0,sinPhi,-cosPhi,0);
        N_GEN_VERTEX(vls, pv,rMax1*cosPhi,rMax1*sinPhi,zMin,1.0,1.0,sinPhi,-cosPhi,0);
        N_GEN_VERTEX(vls, pv,rMin2*cosPhi,rMin2*sinPhi,zMax,1.0,0.0,sinPhi,-cosPhi,0);
        N_GEN_VERTEX(vls, pv,rMin1*cosPhi,rMin1*sinPhi,zMin,0.0,1.0,sinPhi,-cosPhi,0);
        fls.push_back(vls[1]);
        fls.push_back(vls[0]);
        fls.push_back(vls[2]);
        fls.push_back(vls[3]);
	vls.clear();

        //
        // The other end
        //
        sinPhi=sinPhi1;
        cosPhi=cosPhi1;
        N_GEN_VERTEX(vls, pv,rMax2*cosPhi,rMax2*sinPhi, zMax,0.0,0.0,-sinPhi,+cosPhi,0);
        N_GEN_VERTEX(vls, pv,rMax1*cosPhi,rMax1*sinPhi, zMin,1.0,1.0,-sinPhi,+cosPhi,0);
        N_GEN_VERTEX(vls, pv,rMin2*cosPhi,rMin2*sinPhi, zMax,1.0,0.0,-sinPhi,+cosPhi,0);
        N_GEN_VERTEX(vls, pv,rMin1*cosPhi,rMin1*sinPhi, zMin,0.0,1.0,-sinPhi,+cosPhi,0);
        fls.push_back(vls[0]);
        fls.push_back(vls[1]);
        fls.push_back(vls[3]);
        fls.push_back(vls[2]);
        vls.clear();
    }
    if (p==(nSeg-1)) {
      //
      // The outer surface at z=+PDZ
      //
      sinPhi=sinPhi0;
      cosPhi=cosPhi0;
      for (i = 0; i<=NPHI; i++) {
        N_GEN_VERTEX(vls, pv,rMin2*cosPhi,rMin2*sinPhi,zMax,0.0,0.0,0,0,1);
        N_GEN_VERTEX(vls, pv,rMax2*cosPhi,rMax2*sinPhi,zMax,1.0,1.0,0,0,1);
        inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
      }
      for(i = 0; i < ((int)vls.size() - 2); i += 2){
        fls.push_back(vls[i + 0]);
        fls.push_back(vls[i + 1]);
        fls.push_back(vls[i + 3]);
        fls.push_back(vls[i + 2]);
      }
      vls.clear();
    }
    if (p==0) {
      //
      // The outer surface at z=-PDZ
      //
      sinPhi=sinPhi0;
      cosPhi=cosPhi0;
      for (i = 0; i<=NPHI; i++) {
        N_GEN_VERTEX(vls, pv,rMin1*cosPhi,rMin1*sinPhi,zMin,0.0,0.0,0,0,-1);
        N_GEN_VERTEX(vls, pv,rMax1*cosPhi,rMax1*sinPhi,zMin,1.0,1.0,0,0,-1);
        inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
      }
      for(i = 0; i < ((int)vls.size() - 2); i += 2){
        fls.push_back(vls[i + 1]);
        fls.push_back(vls[i + 0]);
        fls.push_back(vls[i + 2]);
        fls.push_back(vls[i + 3]);
      }
      vls.clear();
    }
  }
  /*
   *********************************************
   After finish colecting points of quadrangles, 
   must be builded one monolit geometry,
   with indexed triangles
   *********************************************
   */
  int numFaces = fls.size() / 4;  //Becouse used quadrangular primitives
  if(numFaces < 0) 
   numFaces = 0;

  SoVertexProperty *vertices = new SoVertexProperty();
  for(int i = 0; i < (int)fls.size(); i++){
    vertices->vertex.set1Value (i, fls[i].getPoint()[0], 
				   fls[i].getPoint()[1], 
				   fls[i].getPoint()[2]);
  }
  fls.clear();   //clearing collected vertices

  int* faces = new int[5 * numFaces];
  fprintf(stderr, "\nallocate  Faces: %i", numFaces);
  for(int i = 0; i < numFaces; i++){
    faces[5 * i]     = 4 * i;
    faces[5 * i + 1] = 4 * i + 1;
    faces[5 * i + 2] = 4 * i + 2;
    faces[5 * i + 3] = 4 * i + 3;
    faces[5 * i + 4] = -1;
  }

  SoIndexedFaceSet * faceset = new SoIndexedFaceSet;
  faceset->coordIndex.setValues(0, 5 * numFaces, faces);
  delete [] faces;
  faceset->vertexProperty = vertices;
  alternateRep.setValue(faceset);
  fprintf(stderr, "\n");
  /*
   *********************************************
   Finish
   *********************************************
   */
}

// clearAlternateRep
void SoPcons::clearAlternateRep() {
  alternateRep.setValue(NULL);
}
