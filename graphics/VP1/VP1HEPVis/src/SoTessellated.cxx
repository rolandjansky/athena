/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1HEPVis/nodes/SoTessellated.h"

#include <Inventor/SbBox.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/C/glue/gl.h>
#include <Inventor/elements/SoGLCacheContextElement.h>

SO_NODE_SOURCE(SoTessellated)

//____________________________________________________________________
bool SoTessellated::s_didInit = false;
void SoTessellated::initClass()
{
  if(!s_didInit){
    SO_NODE_INIT_CLASS(SoTessellated, SoShape, "Shape");
    s_didInit = true;
  }
}

//____________________________________________________________________
SoTessellated::SoTessellated()
{
  SO_NODE_CONSTRUCTOR(SoTessellated);
  SO_NODE_ADD_FIELD(drawEdgeLines, (false));
  SO_NODE_ADD_FIELD(forceEdgeLinesInBaseColour, (true));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));
  setNodeType(EXTENSION);
}

//____________________________________________________________________
SoTessellated::~SoTessellated()
{
}

//____________________________________________________________________
void SoTessellated::addTriangularFacet(double x1, double y1, double z1,
				       double x2, double y2, double z2,
				       double x3, double y3, double z3)
{
  m_points.push_back(x1);
  m_points.push_back(y1);
  m_points.push_back(z1);
  m_points.push_back(x2);
  m_points.push_back(y2);
  m_points.push_back(z2);
  m_points.push_back(x3);
  m_points.push_back(y3);
  m_points.push_back(z3);

  // Normal (v2-v1)(x)(v3-v1)
  const double edge1X(x2-x1), edge1Y(y2-y1), edge1Z(z2-z1);
  const double edge2X(x3-x1), edge2Y(y3-y1), edge2Z(z3-z1);
  double nx(edge1Y*edge2Z-edge1Z*edge2Y);
  double ny(edge1Z*edge2X-edge1X*edge2Z);
  double nz(edge1X*edge2Y-edge1Y*edge2X);
  double nl(nx*nx+ny*ny+nz*nz);
  const double s(1.0/sqrt(nl));
  m_normals.push_back(nx*s);
  m_normals.push_back(ny*s);
  m_normals.push_back(nz*s);
}

//____________________________________________________________________
void SoTessellated::addQuadrangularFacet(double x1, double y1, double z1,
					 double x2, double y2, double z2,
					 double x3, double y3, double z3,
					 double x4, double y4, double z4)
{
  addTriangularFacet(x1,y1,z1,x2,y2,z2,x3,y3,z3);
  addTriangularFacet(x3,y3,z3,x4,y4,z4,x1,y1,z1);
}

//____________________________________________________________________
void SoTessellated::finalize()
{
  if(m_points.size()==0)
    setupDefaultPoints();

  //Bounding Box and Center
  float xmin(m_points[0]), ymin(m_points[1]), zmin(m_points[2]);
  float xmax(xmin), ymax(ymin), zmax(zmin);

  int nVertices = m_points.size()/3;
  for (int i=1;i<nVertices;i++) {
    xmin = std::min(xmin,m_points[i*3]);
    xmax = std::max(xmax,m_points[i*3]);
    ymin = std::min(ymin,m_points[i*3+1]);
    ymax = std::max(ymax,m_points[i*3+1]);
    zmin = std::min(zmin,m_points[i*3+2]);
    zmax = std::max(zmax,m_points[i*3+2]);
  }

  m_bbox.setBounds (xmin, ymin, zmin, xmax, ymax, zmax);
  m_center.setValue(0.5*(xmin+xmax),0.5*(ymin+ymax),0.5*(zmin+zmax));

  drawEdgeLines.touch();
}


//____________________________________________________________________
void SoTessellated::generatePrimitives(SoAction *action) 
{
  if(m_points.size()==0)
    setupDefaultPoints();

  // Access the state from the action
  SoState *state = action->getState();

  if(action->getTypeId().isDerivedFrom(SoPickAction::getClassTypeId())) {

    //For picking:
    SoPrimitiveVertex vertex;
    beginShape(action,TRIANGLES);
    for(size_t i=0;i<m_points.size();i+=3) {
      vertex.setPoint(SbVec3f(m_points[i],m_points[i+1],m_points[i+2]));
      shapeVertex(&vertex);
    }
    endShape();

  }else{

    // Draw the shape itself
    glBegin(GL_TRIANGLES);
    int nFaces = m_normals.size()/3;
    for (int i=0;i<nFaces;i++) {
      glNormal3fv((const GLfloat*)&m_normals[i*3]);
      for (int j = 0; j < 3; j++)
	glVertex3fv((const GLfloat*)&m_points[i*9+j*3]);
    }
    glEnd();

    // Draw edges if required
    // 
    // -- ToDo: this can be optimizid. We are currently drawing edges twice 
    // -- and we could also eliminate 'diagonal' lines coming from
    // -- quadrangular shapes
    //
    if(drawEdgeLines.getValue()){
      const bool disableLighting(forceEdgeLinesInBaseColour.getValue()&&glIsEnabled(GL_LIGHTING));
      const bool transparencyOn = glIsEnabled(GL_BLEND);

      if (disableLighting) glDisable(GL_LIGHTING);
      if (transparencyOn) glDisable(GL_BLEND);

      for(size_t i=0;i<m_points.size()-9;i+=9) {
	glBegin(GL_LINE_STRIP);
	glVertex3fv((const GLfloat*)&m_points[i]);
	glVertex3fv((const GLfloat*)&m_points[i+3]);
	glVertex3fv((const GLfloat*)&m_points[i+6]);
	glVertex3fv((const GLfloat*)&m_points[i]);
	glEnd();
      }

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
void SoTessellated::setupDefaultPoints()
{
  double targetSize = 100.;
  addTriangularFacet(-targetSize,-targetSize,0.,targetSize,-targetSize,0.,0.,0.,targetSize);
  addTriangularFacet(targetSize,-targetSize,0.,targetSize,targetSize,0.,0.,0.,targetSize);
  addTriangularFacet(targetSize,targetSize,0.,-targetSize,targetSize,0.,0.,0.,targetSize);
  addTriangularFacet(-targetSize,targetSize,0.,-targetSize,-targetSize,0.,0.,0.,targetSize);
  addQuadrangularFacet(-targetSize,-targetSize,0.,-targetSize,targetSize,0.,targetSize,targetSize,0.,targetSize,-targetSize,0.);
}

//____________________________________________________________________
void SoTessellated::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  if(m_points.size()==0)
    setupDefaultPoints();
  box = m_bbox;
  center = m_center;
}

//____________________________________________________________________
void SoTessellated::generateAlternateRep()
{
  if(m_points.size()==0)
    setupDefaultPoints();
  if (alternateRep.getValue())
    clearAlternateRep();

  SoSeparator * sep = new SoSeparator;

  // Faceset representing the shape
  SoVertexProperty* vertices = new SoVertexProperty();
  for(size_t i=0;i<m_normals.size();++i)
    vertices->vertex.set1Value(i,m_points[i*3+0],m_points[i*3+1],m_points[i*3+2]);
  SoIndexedFaceSet* faceset = new SoIndexedFaceSet;
  int j=0;
  for(size_t i=0;i<m_normals.size();++i) {
    faceset->coordIndex.set1Value(j++,i);
    if((i>1) && ((i-2)%3==0)) {
      faceset->coordIndex.set1Value(j++,SO_END_FACE_INDEX);
    }
  }

  faceset->vertexProperty = vertices;
  sep->addChild(faceset);

  // Lineset representing the outline
  //
  // -- ToDo: this can be optimizid. We are currently drawing edges twice 
  // -- and we could also eliminate 'diagonal' lines coming from
  // -- quadrangular shapes
  //
  if (drawEdgeLines.getValue()) {
    SoVertexProperty* verticesL = new SoVertexProperty();
    SoIndexedLineSet* lineset = new SoIndexedLineSet;
    int kk=0, jj=0;
    for(size_t i=0;i<m_points.size()-9;i+=9) {
      verticesL->vertex.set1Value(kk,m_points[i],m_points[i+1],m_points[i+2]);
      lineset->coordIndex.set1Value(jj++,kk++);
      verticesL->vertex.set1Value(kk,m_points[i+3],m_points[i+4],m_points[i+5]);
      lineset->coordIndex.set1Value(jj++,kk++);
      verticesL->vertex.set1Value(kk,m_points[i+6],m_points[i+7],m_points[i+8]);
      lineset->coordIndex.set1Value(jj++,kk++);
      verticesL->vertex.set1Value(kk,m_points[i],m_points[i+1],m_points[i+2]);
      lineset->coordIndex.set1Value(jj++,kk++);
      lineset->coordIndex.set1Value(jj++,SO_END_LINE_INDEX);
    }
    lineset->vertexProperty = verticesL;
    sep->addChild(lineset);
  }

  alternateRep.setValue(sep);
}

//____________________________________________________________________
void SoTessellated::clearAlternateRep()
{
  alternateRep.setValue(NULL);
}

