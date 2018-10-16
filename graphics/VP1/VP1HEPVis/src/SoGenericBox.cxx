/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class SoGenericBox                      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: December 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1HEPVis/nodes/SoGenericBox.h"

#include <Inventor/SbBox.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
//#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/C/glue/gl.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <iostream>

SO_NODE_SOURCE(SoGenericBox)

static const int SOGENERICBOX_NPOINTS=8;
static const int SOGENERICBOX_NFACES=6;

static int sogenericbox_vindices[] =
{
  3, 2, 1, 0,
  4, 5, 6, 7,
  0, 1, 5, 4,
  1, 2, 6, 5,
  2, 3, 7, 6,
  3, 0, 4, 7
};

static int sogenericbox_vindices_foraltrepfaces[5*SOGENERICBOX_NFACES] =
{
  3, 2, 1, 0, SO_END_FACE_INDEX,
  4, 5, 6, 7, SO_END_FACE_INDEX,
  0, 1, 5, 4, SO_END_FACE_INDEX,
  1, 2, 6, 5, SO_END_FACE_INDEX,
  2, 3, 7, 6, SO_END_FACE_INDEX,
  3, 0, 4, 7, SO_END_FACE_INDEX
};

static int sogenericbox_vindices_foraltreplines[20] =
{
  0, 1, 2, 3, 0, 4, 5, 6, 7, 4, SO_END_LINE_INDEX,
  3, 7, SO_END_LINE_INDEX,
  6, 2, SO_END_LINE_INDEX,
  5, 1, SO_END_LINE_INDEX,
};

//____________________________________________________________________
bool SoGenericBox::s_didInit = false;
void SoGenericBox::initClass()
{
  if ( !s_didInit ) {
    SO_NODE_INIT_CLASS(SoGenericBox, SoShape, "Shape");
    s_didInit = true;
  }
}

//____________________________________________________________________
SoGenericBox::SoGenericBox()
  : m_points(0), m_normals(0)
{
  SO_NODE_CONSTRUCTOR(SoGenericBox);
  SO_NODE_ADD_FIELD(drawEdgeLines, (false));
  SO_NODE_ADD_FIELD(forceEdgeLinesInBaseColour, (true));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));
  setNodeType(EXTENSION);
}

//____________________________________________________________________
SoGenericBox::~SoGenericBox()
{
  delete [] m_normals;
  delete [] m_points;
}

//____________________________________________________________________
void SoGenericBox::generatePrimitives(SoAction *action) {

  if (!m_points)
    setupDefaultPoints();

  // Access the state from the action
  SoState *state = action->getState();

  if ( action->getTypeId().isDerivedFrom(SoPickAction::getClassTypeId()) ) {
    //For picking:

    SoPrimitiveVertex vertex;
    beginShape(action,QUADS);
    int *iptr = sogenericbox_vindices;
    for (int i=0;i<SOGENERICBOX_NFACES;++i) {
      for (int j = 0; j < 4; ++j) {
        const int ivert = *iptr++;
        vertex.setPoint(SbVec3f(m_points[ivert*3+0], m_points[ivert*3+1], m_points[ivert*3+2]));
        shapeVertex(&vertex);
      }
    }
    endShape();
  } else {

    glBegin(GL_QUADS);
    int *iptr = sogenericbox_vindices;
    for (int i=0;i<SOGENERICBOX_NFACES;i++) {
      glNormal3fv((const GLfloat*)&m_normals[i*3]);
      for (int j = 0; j < 4; j++)
        glVertex3fv((const GLfloat*)&m_points[(*iptr++)*3]);
    }
    glEnd();


    if (drawEdgeLines.getValue()) {

      const bool disableLighting(forceEdgeLinesInBaseColour.getValue()&&glIsEnabled(GL_LIGHTING));
      const bool transparencyOn = glIsEnabled(GL_BLEND);

      if (disableLighting) glDisable(GL_LIGHTING);
      if (transparencyOn) glDisable(GL_BLEND);

#define trdV(i) glVertex3fv((const GLfloat*)&m_points[i*3]);
      glBegin(GL_LINE_STRIP);
      trdV(0)trdV(1)trdV(2)trdV(3)trdV(0)trdV(4)trdV(5)trdV(6)trdV(7)trdV(4)
      glEnd();

      glBegin(GL_LINES);
      trdV(3)trdV(7)
      trdV(6)trdV(2)
      trdV(5)trdV(1)
      glEnd();

      if (disableLighting) glEnable(GL_LIGHTING);
      if (transparencyOn) glEnable(GL_BLEND);
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

//____________________________________________________________________
void SoGenericBox::ensurePointsAllocated()
{
  if (!m_points)
    m_points = new float[SOGENERICBOX_NPOINTS*3];
}

//____________________________________________________________________
void SoGenericBox::setupDefaultPoints()
{
  setParametersForBox( 0.5, 0.5, 0.5 );
}

//____________________________________________________________________
void SoGenericBox::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  if (!m_points)
    setupDefaultPoints();
  box = m_bbox;
  center = m_center;
}

//____________________________________________________________________
void SoGenericBox::setParametersForBox( float dx, float dy, float dz,
					float xcenter, float ycenter, float zcenter ) {
  ensurePointsAllocated();
  m_points[0*3+0] = xcenter+dx; m_points[0*3+1] = ycenter+dy; m_points[0*3+2] = zcenter-dz;
  m_points[1*3+0] = xcenter-dx; m_points[1*3+1] = ycenter+dy; m_points[1*3+2] = zcenter-dz;
  m_points[2*3+0] = xcenter-dx; m_points[2*3+1] = ycenter-dy; m_points[2*3+2] = zcenter-dz;
  m_points[3*3+0] = xcenter+dx; m_points[3*3+1] = ycenter-dy; m_points[3*3+2] = zcenter-dz;
  m_points[4*3+0] = xcenter+dx; m_points[4*3+1] = ycenter+dy; m_points[4*3+2] = zcenter+dz;
  m_points[5*3+0] = xcenter-dx; m_points[5*3+1] = ycenter+dy; m_points[5*3+2] = zcenter+dz;
  m_points[6*3+0] = xcenter-dx; m_points[6*3+1] = ycenter-dy; m_points[6*3+2] = zcenter+dz;
  m_points[7*3+0] = xcenter+dx; m_points[7*3+1] = ycenter-dy; m_points[7*3+2] = zcenter+dz;
  pointsUpdated();
}

//____________________________________________________________________
void SoGenericBox::internalSetParametersForEtaPhiCell( bool barrel, double etaMin, double etaMax,
						       double phiMin, double phiMax,
						       double cellDepth, double cellDistance,
						       double etasqueezefact, double phisqueezefact )
{
  ensurePointsAllocated();
  if (phiMax<phiMin) std::swap(phiMin,phiMax);
  if (etaMax<etaMin) std::swap(etaMin,etaMax);

  if (etasqueezefact!=1.0) {
    double etashift(0.5*(1.0-etasqueezefact)*fabs(etaMax-etaMin));
    etaMax -= etashift;
    etaMin += etashift;
  }
  if (phisqueezefact!=1.0) {
    double phishift(0.5*(1.0-phisqueezefact)*fabs(phiMax-phiMin));
    phiMax -= phishift;
    phiMin += phishift;
  }

  if (cellDistance<0) cellDistance = -cellDistance;
  if (cellDepth<0) cellDistance = std::max(cellDistance-cellDepth,0.0);
  double tantheta2 = tan(2*atan(exp(-etaMin)));
  double tantheta1 = tan(2*atan(exp(-etaMax)));
  const double cpMin(cos(phiMin));
  const double cpMax(cos(phiMax));
  const double spMin(sin(phiMin));
  const double spMax(sin(phiMax));
  if (barrel) {
    const double rf = cellDistance;
    const double rb = cellDistance+cellDepth;
    const double rfdivtantheta1(rf/tantheta1);
    const double rbdivtantheta1(rb/tantheta1);
    const double rfdivtantheta2(rf/tantheta2);
    const double rbdivtantheta2(rb/tantheta2);
    m_points[0*3+0] = rf*cpMin; m_points[0*3+1] = rf*spMin; m_points[0*3+2] = rfdivtantheta1;
    m_points[1*3+0] = rf*cpMin; m_points[1*3+1] = rf*spMin; m_points[1*3+2] = rfdivtantheta2;
    m_points[2*3+0] = rf*cpMax; m_points[2*3+1] = rf*spMax; m_points[2*3+2] = rfdivtantheta2;
    m_points[3*3+0] = rf*cpMax; m_points[3*3+1] = rf*spMax; m_points[3*3+2] = rfdivtantheta1;
    m_points[4*3+0] = rb*cpMin; m_points[4*3+1] = rb*spMin; m_points[4*3+2] = rbdivtantheta1;
    m_points[5*3+0] = rb*cpMin; m_points[5*3+1] = rb*spMin; m_points[5*3+2] = rbdivtantheta2;
    m_points[6*3+0] = rb*cpMax; m_points[6*3+1] = rb*spMax; m_points[6*3+2] = rbdivtantheta2;
    m_points[7*3+0] = rb*cpMax; m_points[7*3+1] = rb*spMax; m_points[7*3+2] = rbdivtantheta1;
  } else {
    const double eta(0.5*(etaMin+etaMax));
    const double zf(eta>0?cellDistance:-cellDistance);
    double zb = (zf > 0 ?  zf+cellDepth: zf-cellDepth);
    m_points[0*3+0] = zf*tantheta1*cpMin; m_points[0*3+1] = zf*tantheta1*spMin; m_points[0*3+2] = zf;
    m_points[1*3+0] = zf*tantheta2*cpMin; m_points[1*3+1] = zf*tantheta2*spMin; m_points[1*3+2] = zf;
    m_points[2*3+0] = zf*tantheta2*cpMax; m_points[2*3+1] = zf*tantheta2*spMax; m_points[2*3+2] = zf;
    m_points[3*3+0] = zf*tantheta1*cpMax; m_points[3*3+1] = zf*tantheta1*spMax; m_points[3*3+2] = zf;
    m_points[4*3+0] = zb*tantheta1*cpMin; m_points[4*3+1] = zb*tantheta1*spMin; m_points[4*3+2] = zb;
    m_points[5*3+0] = zb*tantheta2*cpMin; m_points[5*3+1] = zb*tantheta2*spMin; m_points[5*3+2] = zb;
    m_points[6*3+0] = zb*tantheta2*cpMax; m_points[6*3+1] = zb*tantheta2*spMax; m_points[6*3+2] = zb;
    m_points[7*3+0] = zb*tantheta1*cpMax; m_points[7*3+1] = zb*tantheta1*spMax; m_points[7*3+2] = zb;
  }

  pointsUpdated();
}


//____________________________________________________________________
void SoGenericBox::setParametersForBarrelEtaPhiCell( double etaMin, double etaMax,
						     double phiMin, double phiMax,
						     double cellDepth, double cellDistance,
						     double etasqueezefact, double phisqueezefact )
{
  internalSetParametersForEtaPhiCell( true, etaMin, etaMax, phiMin, phiMax,
				      cellDepth, cellDistance, etasqueezefact, phisqueezefact );
}

//____________________________________________________________________
void SoGenericBox::setParametersForEndCapEtaPhiCell( double etaMin, double etaMax,
						     double phiMin, double phiMax,
						     double cellDepth, double cellDistance,
						     double etasqueezefact, double phisqueezefact )
{
  internalSetParametersForEtaPhiCell( false, etaMin, etaMax, phiMin, phiMax,
				      cellDepth, cellDistance, etasqueezefact, phisqueezefact );
}

//____________________________________________________________________
void SoGenericBox::setParametersForTrd( float dx1, float dx2,
					float dy1, float dy2,
					float dz )
{
  ensurePointsAllocated();
  m_points[0*3+0] =  dx1; m_points[0*3+1] =  dy1; m_points[0*3+2] = -dz;
  m_points[1*3+0] = -dx1; m_points[1*3+1] =  dy1; m_points[1*3+2] = -dz;
  m_points[2*3+0] = -dx1; m_points[2*3+1] = -dy1; m_points[2*3+2] = -dz;
  m_points[3*3+0] =  dx1; m_points[3*3+1] = -dy1; m_points[3*3+2] = -dz;
  m_points[4*3+0] =  dx2; m_points[4*3+1] =  dy2; m_points[4*3+2] =  dz;
  m_points[5*3+0] = -dx2; m_points[5*3+1] =  dy2; m_points[5*3+2] =  dz;
  m_points[6*3+0] = -dx2; m_points[6*3+1] = -dy2; m_points[6*3+2] =  dz;
  m_points[7*3+0] =  dx2; m_points[7*3+1] = -dy2; m_points[7*3+2] =  dz;
  pointsUpdated();
}

//____________________________________________________________________
void SoGenericBox::setParametersForTrapezoid( float dz, float theta, float phi, float dy1,
					      float dx1, float dx2, float dy2, float dx3,
					      float dx4, float alp1, float alp2 )

{
  ensurePointsAllocated();
  const float tanTheta(tan(theta));
  const float TthetaCphi = tanTheta*cos(phi);
  const float TthetaSphi = tanTheta*sin(phi);
  const float Talp1 = tan(alp1);
  const float Talp2 = tan(alp2);

  m_points[0*3+0] =  dx2+dy1*Talp1;
  m_points[0*3+1] =  dy1;
  m_points[0*3+2] = -dz;

  m_points[1*3+0] = -dx2+dy1*Talp1;
  m_points[1*3+1] =  dy1;
  m_points[1*3+2] = -dz;

  m_points[2*3+0] = -dx1-dy1*Talp1;
  m_points[2*3+1] = -dy1;
  m_points[2*3+2] = -dz;

  m_points[3*3+0] =  dx1-dy1*Talp1;
  m_points[3*3+1] = -dy1;
  m_points[3*3+2] = -dz;

  m_points[4*3+0] =  dx4+dy2*Talp2;
  m_points[4*3+1] =  dy2;
  m_points[4*3+2] =  dz;

  m_points[5*3+0] = -dx4+dy2*Talp2;
  m_points[5*3+1] =  dy2;
  m_points[5*3+2] =  dz;

  m_points[6*3+0] = -dx3-dy2*Talp2;
  m_points[6*3+1] = -dy2;
  m_points[6*3+2] =  dz;

  m_points[7*3+0] =  dx3-dy2*Talp2;
  m_points[7*3+1] = -dy2;
  m_points[7*3+2] =  dz;

  const float dzTthetaCphi(dz*TthetaCphi);
  const float dzTthetaSphi(dz*TthetaSphi);
  int i;
  for (i=0;i<4;i++) {
    m_points[i*3+0] -= dzTthetaCphi;
    m_points[i*3+1] -= dzTthetaSphi;
  }
  for (i=4;i<8;i++) {
    m_points[i*3+0] += dzTthetaCphi;
    m_points[i*3+1] += dzTthetaSphi;
  }
  pointsUpdated();
}


//____________________________________________________________________
void SoGenericBox::setGenericParameters( float x0, float y0, float z0,
					 float x1, float y1, float z1,
					 float x2, float y2, float z2,
					 float x3, float y3, float z3,
					 float x4, float y4, float z4,
					 float x5, float y5, float z5,
					 float x6, float y6, float z6,
					 float x7, float y7, float z7 )
{
  ensurePointsAllocated();
  m_points[0*3+0] = x0;  m_points[0*3+1] = y0;  m_points[0*3+2] = z0;
  m_points[1*3+0] = x1;  m_points[1*3+1] = y1;  m_points[1*3+2] = z1;
  m_points[2*3+0] = x2;  m_points[2*3+1] = y2;  m_points[2*3+2] = z2;
  m_points[3*3+0] = x3;  m_points[3*3+1] = y3;  m_points[3*3+2] = z3;
  m_points[4*3+0] = x4;  m_points[4*3+1] = y4;  m_points[4*3+2] = z4;
  m_points[5*3+0] = x5;  m_points[5*3+1] = y5;  m_points[5*3+2] = z5;
  m_points[6*3+0] = x6;  m_points[6*3+1] = y6;  m_points[6*3+2] = z6;
  m_points[7*3+0] = x7;  m_points[7*3+1] = y7;  m_points[7*3+2] = z7;
  pointsUpdated();
}

//____________________________________________________________________
void SoGenericBox::pointsUpdated()
{
  //Normals:
  if (!m_normals)
    m_normals = new float[SOGENERICBOX_NFACES*3];
  for (int iface = 0; iface < SOGENERICBOX_NFACES; ++iface) {
    const int index_v1 = sogenericbox_vindices[iface*4+0];
    const int index_v2 = sogenericbox_vindices[iface*4+1];
    const int index_v3 = sogenericbox_vindices[iface*4+2];

    //Figure out normal: (v2-v1)(x)(v3-v1)
    const double v1X(m_points[index_v1*3+0]), v1Y(m_points[index_v1*3+1]), v1Z(m_points[index_v1*3+2]);
    const double edge1X(m_points[index_v2*3+0]-v1X), edge1Y(m_points[index_v2*3+1]-v1Y), edge1Z(m_points[index_v2*3+2]-v1Z);
    const double edge2X(m_points[index_v3*3+0]-v1X), edge2Y(m_points[index_v3*3+1]-v1Y), edge2Z(m_points[index_v3*3+2]-v1Z);

    double nx(edge1Y*edge2Z-edge1Z*edge2Y);
    double ny(edge1Z*edge2X-edge1X*edge2Z);
    double nz(edge1X*edge2Y-edge1Y*edge2X);
    double nl(nx*nx+ny*ny+nz*nz);
    if (nl<=0) {
      std::cout<<"SoGenericBox::Error Could not calculate normal due to degenerate face edge"<<std::endl;
      nx=1.0;ny=0.0;nz=0.0;nl=1;
      //Fixme: We should try to use index_v4 instead of one vertex,
      //to get the normal (this would allow faces where one edge was
      //collapsed).
      //const int index_v4 = sogenericbox_vindices[iface*4+3];
    }
    const double s(1.0/sqrt(nl));
    m_normals[iface*3+0] = nx*s;
    m_normals[iface*3+1] = ny*s;
    m_normals[iface*3+2] = nz*s;
  }

  //BBox:
  float xmin(1.0e99), ymin(1.0e99), zmin(1.0e99);//fixme: -> inf
  float xmax(-1.0e99), ymax(-1.0e99), zmax(-1.0e99);
  for (int i=0;i<SOGENERICBOX_NPOINTS;++i) {
    const float x(m_points[i*3+0]), y(m_points[i*3+1]), z(m_points[i*3+2]);
    xmin = std::min(xmin,x); xmax = std::max(xmax,x);
    ymin = std::min(ymin,y); ymax = std::max(ymax,y);
    zmin = std::min(zmin,z); zmax = std::max(zmax,z);
  }
  m_bbox.setBounds (xmin, ymin, zmin, xmax, ymax, zmax);
  m_center.setValue(0.5*(xmin+xmax),0.5*(ymin+ymax),0.5*(zmin+zmax));

  drawEdgeLines.touch();
}


///////////////////////////////
//  Stuff for alternate rep  //
///////////////////////////////

//____________________________________________________________________
void SoGenericBox::generateAlternateRep()
{
  if (!m_points)
    setupDefaultPoints();
  if (alternateRep.getValue())
    clearAlternateRep();
  SoSeparator * sep = new SoSeparator;

  //Vertice coordinates:
  SoVertexProperty *vertices = new SoVertexProperty();
  for (int i=0;i<SOGENERICBOX_NPOINTS;++i)
    vertices->vertex.set1Value ( i, m_points[i*3+0],m_points[i*3+1],m_points[i*3+2] );
  SoIndexedFaceSet * faceset = new SoIndexedFaceSet;
  faceset->coordIndex.setValues(0,5*6, sogenericbox_vindices_foraltrepfaces);
  faceset->vertexProperty = vertices;
  sep->addChild(faceset);

  if (drawEdgeLines.getValue()) {
//Disable the lightmodel now since we in any case gets baselightning with the lineset defined this way...
//     if (forceEdgeLinesInBaseColour.getValue()) {
//       SoLightModel * lm = new SoLightModel;//Fixme: share
//       lm->model = SoLightModel::BASE_COLOR;
//       sep->addChild(lm);
//     }
    SoIndexedLineSet * lineset = new SoIndexedLineSet;
    lineset->coordIndex.setValues(0,20,sogenericbox_vindices_foraltreplines);
    lineset->vertexProperty = vertices;
    sep->addChild(lineset);
  }

  alternateRep.setValue(sep);

}

//____________________________________________________________________
void SoGenericBox::clearAlternateRep()
{
  alternateRep.setValue(NULL);
  //Hmm... no ref/unref??
}
