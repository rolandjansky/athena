/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SoPolyhedron                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//                                                            //
//  Initial version: November 2008                            //
//                                                            //
//  Update: Giorgi Gvaberidze (ggvaberi@cern.ch)              // 
//          November 2010                                     //
//                                                            //
//          Riccardo-Maria BIANCHI (rbianchi@cern.ch)         //
//          13.12.2012                                        //
//                                                            //
////////////////////////////////////////////////////////////////

#include <VP1HEPVis/nodes/SoPolyhedron.h>

#include <Inventor/SbBox.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoVertexProperty.h>

#include <Inventor/C/glue/gl.h>

#include "SbMath.h"
#include <VP1HEPVis/SbPolyhedron.h>

//// debug messages
//#include "VP1Base/VP1Msg.h"
//#include <QString>


SO_NODE_SOURCE(SoPolyhedron)

//____________________________________________________________________
bool SoPolyhedron::s_didInit = false;
void SoPolyhedron::initClass()
{
  if ( !s_didInit ) {
    SO_NODE_INIT_CLASS(SoPolyhedron,SoShape,"Shape");
    s_didInit = true;
  }
}

//////////////////////////////////////////////////////////////////////////////
SoPolyhedron::SoPolyhedron(
)
:m_polyhedron(0), m_vertices(0), m_indices(0), m_vcount(0), m_icount(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(SoPolyhedron);
  //  SO_NODE_ADD_FIELD(solid,(TRUE));
  //  SO_NODE_ADD_FIELD(reducedWireFrame,(TRUE));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));
  setNodeType(EXTENSION);
}
//////////////////////////////////////////////////////////////////////////////
SoPolyhedron::SoPolyhedron(
 const SbPolyhedron& aPolyhedron
)
:m_polyhedron(0), m_vertices(0), m_indices(0), m_vcount(0), m_icount(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(SoPolyhedron);
  //  SO_NODE_ADD_FIELD(solid,(TRUE));
  //  SO_NODE_ADD_FIELD(reducedWireFrame,(TRUE));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  m_polyhedron = new SbPolyhedron(aPolyhedron);
  setNodeType(EXTENSION);
  makeShape(m_polyhedron);
}
//////////////////////////////////////////////////////////////////////////////
SoPolyhedron::SoPolyhedron(
 const SbPolyhedron* aPolyhedron
)
:m_polyhedron(0), m_vertices(0), m_indices(0), m_vcount(0), m_icount(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(SoPolyhedron);
  //  SO_NODE_ADD_FIELD(solid,(TRUE));
  //  SO_NODE_ADD_FIELD(reducedWireFrame,(TRUE));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  m_polyhedron = new SbPolyhedron(*aPolyhedron);
  setNodeType(EXTENSION);
  makeShape(m_polyhedron);
}
//////////////////////////////////////////////////////////////////////////////
SoPolyhedron::~SoPolyhedron(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  delete m_polyhedron;
  if(m_vertices)
   delete [] m_vertices;
  if(m_indices)
   delete [] m_indices;
}
//////////////////////////////////////////////////////////////////////////////
void SoPolyhedron::generatePrimitives(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!m_polyhedron) return;
  if(m_polyhedron->GetNoFacets()<=0) return; // Abnormal polyhedron.

  SoState *state = aAction->getState();
  if (!state)
    return;
  SbBool useTexFunction =
    (SoTextureCoordinateElement::getType(state) ==
     SoTextureCoordinateElement::FUNCTION);
  const SoTextureCoordinateElement *tce = NULL;
  SbVec4f texCoord;
  if (useTexFunction) {
    tce = SoTextureCoordinateElement::getInstance(state);
  } else {
    texCoord[2] = 0.0;
    texCoord[3] = 1.0;
  }

  //  if(solid.getValue()==TRUE) {
  if(true) {
    SoPrimitiveVertex pv;
    SbVec3f point, normal;
    //////////////////////////////////////////
    //----------------------------------------
#define GEN_VERTEX(pv,x,y,z,s,t,nx,ny,nz)  \
  point.setValue(x,y,z);                   \
  normal.setValue(nx,ny,nz);               \
  if (useTexFunction) {                    \
    texCoord=tce->get(point,normal);       \
  } else {                                 \
    texCoord[0]=s;                         \
    texCoord[1]=t;                         \
  }                                        \
  pv.setPoint(point);                      \
  pv.setNormal(normal);                    \
  pv.setTextureCoords(texCoord);           \
  shapeVertex(&pv);
  //----------------------------------------
  //////////////////////////////////////////
   /////////////////////////////////////////////////////////////
   // For pick action use old method, for for see selected lines
   /////////////////////////////////////////////////////////////
   if ( aAction->getTypeId().isDerivedFrom(SoPickAction::getClassTypeId()) ){  //comment this line to return on old method
    // Assume all facets are convex quadrilaterals :
    bool notLastFace;
    do {
      HVNormal3D unitNormal;
      notLastFace = m_polyhedron->GetNextUnitNormal(unitNormal);

      beginShape(aAction,POLYGON);
      bool notLastEdge;
      int edgeFlag = 1;
      do {
        HVPoint3D vertex;
        notLastEdge = m_polyhedron->GetNextVertex(vertex,edgeFlag);
        GEN_VERTEX(pv,
                   vertex[0],
                   vertex[1],
                   vertex[2],
                   0.0,0.0,
                   unitNormal[0],
                   unitNormal[1],
                   unitNormal[2]);
      } while (notLastEdge);
      endShape();
    } while (notLastFace);  ///*  Activate this comment if u gona return to old method
   }
   else
   {
    if(m_vertices){
     glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &m_vertices[0].pos);
     glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT,    sizeof(Vertex), &m_vertices[0].nor);
     /////////////////////////////////////////
     //For I-Method. Comment it if u use II-Method
     /////////////////////////////////////////
     //glDrawElements(GL_TRIANGLES, m_icount, GL_UNSIGNED_INT, m_indices);  //comment it if u use II-Method
     /////////////////////////////////////////
     //For II-Method. Comment it if u use I-Method
     /////////////////////////////////////////
     glDrawArrays(GL_TRIANGLES, 0, m_vcount);  //comment it if u use I-Method
     glDisableClientState(GL_VERTEX_ARRAY);
     glDisableClientState(GL_NORMAL_ARRAY);
    }
   }  //End of comment for return to old method */
  } else {//lala
//     SoPrimitiveVertex pvb,pve;
//     pve.setTextureCoords(texCoord);
//     pvb.setTextureCoords(texCoord);

// #ifdef __COIN__ // To bypass a bug in invokeLineSegment when picking.
//     beginShape(aAction,POLYGON);
//     endShape();
// #endif

//     SbVec3f point;
//     bool notLastFace;
//     do {
//       HVNormal3D unitNormal;
//       notLastFace = m_polyhedron->GetNextUnitNormal(unitNormal);

//       SbVec3f normal;
//       if( false ) {
//         normal.setValue(0,0,1);
//       } else {
//         normal.setValue(unitNormal[0],unitNormal[1],unitNormal[2]);
//       }

//       // Treat edges :
//       int edgeFlag = 1;
//       int prevEdgeFlag = edgeFlag;
//       bool notLastEdge;
//       SbBool firstEdge = TRUE;
//       do {
//         HVPoint3D vertex;
//         notLastEdge = m_polyhedron->GetNextVertex(vertex,edgeFlag);
//         if(reducedWireFrame.getValue()==FALSE) edgeFlag = 1;
//         if(firstEdge) {
//           if(edgeFlag) {
//             pvb.setNormal(normal);
//             point.setValue(vertex[0],vertex[1],vertex[2]);
//             //DONOTHING projector.project(1,&point);
//             pvb.setPoint(point);
//           } else {
//           }
//           firstEdge = FALSE;
//           prevEdgeFlag = edgeFlag;
//         } else {
//           if(edgeFlag!=prevEdgeFlag) {
//             if(edgeFlag) { // Pass to a visible edge :
//               pvb.setNormal(normal);
//               point.setValue(vertex[0],vertex[1],vertex[2]);
//               //DONOTHING projector.project(1,&point);
//               pvb.setPoint(point);
//             } else { // Pass to an invisible edge :
//               pve.setNormal(normal);
//               point.setValue(vertex[0],vertex[1],vertex[2]);
//               //DONOTHING projector.project(1,&point);
//               pve.setPoint(point);
//               invokeLineSegmentCallbacks(aAction,&pvb,&pve);
//             }
//             prevEdgeFlag = edgeFlag;
//           } else {
//             if(edgeFlag) {
//               pve.setNormal(normal);
//               point.setValue(vertex[0],vertex[1],vertex[2]);
//               //DONOTHING projector.project(1,&point);
//               pve.setPoint(point);
//               invokeLineSegmentCallbacks(aAction,&pvb,&pve);
//               pvb = pve;
//             } else {
//             }
//           }
//         }
//       } while (notLastEdge);
//       } while (notLastFace);
  }

  if (state&&state->isElementEnabled(SoGLCacheContextElement::getClassStackIndex())) {
    //Encourage auto caching
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DO_AUTO_CACHE);
#if ((COIN_MAJOR_VERSION>=3)||((COIN_MAJOR_VERSION==2)&&(COIN_MINOR_VERSION>=5)))
    SoGLCacheContextElement::incNumShapes(state);
#endif
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoPolyhedron::computeBBox(
 SoAction*
,SbBox3f& aBox
,SbVec3f& aCenter
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!m_polyhedron) return;
  if(m_polyhedron->GetNoFacets()<=0) { // Abnormal polyhedron.
    SbVec3f vmin(-1,-1,-1);
    SbVec3f vmax( 1, 1, 1);
    aBox.setBounds(vmin,vmax);
    aCenter.setValue(0,0,0);
  } else {
    //    SbBool wireFrame = (solid.getValue()==TRUE ? FALSE : TRUE);
    //DONOTHING HEPVis::SbProjector projector(wireFrame?fProjection:SbProjectionNone);
    SbBool first = TRUE;
    float xmn = 0,ymn = 0,zmn = 0;
    float xmx = 0,ymx = 0,zmx = 0;
    float xct = 0,yct = 0,zct = 0;
    SbVec3f point;
    int count = 0;
    // Assume all facets are convex quadrilaterals :
    bool notLastFace;
    do {
      HVNormal3D unitNormal;
      notLastFace = m_polyhedron->GetNextUnitNormal(unitNormal);
      bool notLastEdge;
      do {
        HVPoint3D vertex;
        int edgeFlag = 1;
        notLastEdge = m_polyhedron->GetNextVertex(vertex,edgeFlag);
        point.setValue(vertex[0],vertex[1],vertex[2]);
        //DONOTHING projector.project(1,&point);
        if(first==TRUE) {
          xct = xmx = xmn = point[0];
          yct = ymx = ymn = point[1];
          zct = zmx = zmn = point[2];
          count++;
          first = FALSE;
        } else {
          xmn = SbMinimum(xmn,point[0]);
          ymn = SbMinimum(ymn,point[1]);
          zmn = SbMinimum(zmn,point[2]);
          //
          xmx = SbMaximum(xmx,point[0]);
          ymx = SbMaximum(ymx,point[1]);
          zmx = SbMaximum(zmx,point[2]);
          //
          xct += point[0];
          yct += point[1];
          zct += point[2];
          count++;
        }
        //
      } while (notLastEdge);
    } while (notLastFace);
    SbVec3f vmin(xmn,ymn,zmn);
    SbVec3f vmax(xmx,ymx,zmx);
    aBox.setBounds(vmin,vmax);

    if(count==0) {
    	aCenter.setValue(0,0,0);
    }
    else {
    	aCenter.setValue(xct/count,yct/count,zct/count);
    }
    //DEBUG
//    std::cout << "vmin: " << xmn << " " << ymn << " " << zmn << std::endl;
//    std::cout << "vmax: " << xmx << " " << ymx << " " << zmx << std::endl;
  }


}

//---
//____________________________________________________________________

/*
void SoPolyhedron::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
 if(action)
  action->addNumTriangles(10);
}
//*/

///*
void SoPolyhedron::generateAlternateRep()
{
 if (alternateRep.getValue())
  clearAlternateRep();
 if( m_polyhedron && ( m_polyhedron->GetNoFacets() > 0 ) ){
  //SoSeparator *sep = new SoSeparator;
  SoVertexProperty *vertices = new SoVertexProperty();

  //Retreive geometry from polyhedron
  int vno = m_polyhedron->GetNoVertices();
  for(int i = 0; i < vno; i++){
   HVPoint3D vertex;
   vertex = m_polyhedron->GetVertex(i + 1);
   vertices->vertex.set1Value (i, vertex[0], vertex[1], vertex[2]);
  }

  int  fno = m_polyhedron->GetNoFacets();
  int  fcr = 0;
  int* aface = new int[8 * fno];
  for(int i = 0; i < fno; i++){
	  int n, inodes[4];
	  //SbVec3d nr = m_polyhedron->GetNormal(i + 1); // not used, gives warning, commenting out:
	  //---
	  m_polyhedron->GetFacet(i + 1, n, inodes);
	  aface[fcr] = (inodes[0] <= vno) ? (inodes[0] - 1) : (0); fcr++;
	  aface[fcr] = (inodes[1] <= vno) ? (inodes[1] - 1) : (0); fcr++;
	  aface[fcr] = (inodes[2] <= vno) ? (inodes[2] - 1) : (0); fcr++;
	  if(n == 4) { aface[fcr] = (inodes[3] <= vno) ? (inodes[3] - 1) : (0); fcr++; }
	  aface[fcr] = -1; fcr++;
  }
  SoIndexedFaceSet * faceset = new SoIndexedFaceSet;
  faceset->coordIndex.setValues(0, fcr, aface);
  delete [] aface;
  faceset->vertexProperty = vertices;
  //sep->addChild(faceset);
  alternateRep.setValue(faceset);
 }
}
//*/

//____________________________________________________________________
///*
void SoPolyhedron::clearAlternateRep()
{
  alternateRep.setValue(NULL);
  //Hmm... no ref/unref??
}
//*/

//____________________________________________________________________
///*
inline long SoPolyhedron::hasVertex(Vertex* vertices, long len, Vertex& v){
 for(long i = 0; i < len; i++){
  if((v.pos[0] == vertices[i].pos[0]) && (v.pos[1] == vertices[i].pos[1]) && (v.pos[2] == vertices[i].pos[2]) &&
	 (v.nor[0] == vertices[i].nor[0]) && (v.nor[1] == vertices[i].nor[1]) && (v.nor[2] == vertices[i].nor[2]))
   return i;
 }
 return -1;
}
//*/

//////////////////////////////////////////////////
// I - Method, cooment it if u use II - Method
//////////////////////////////////////////////////
/*
void SoPolyhedron::makeShape(SbPolyhedron* sp)
{
 if(!sp || (m_polyhedron->GetNoFacets() < 1))
  return;

 if(m_vertices){
  delete [] m_vertices;
  m_vertices = 0;
  m_vcount = 0;
 }

 if(m_indices){
  delete [] m_indices;
  m_indices = 0;
  m_icount = 0;
 }

 int   fno = m_polyhedron->GetNoFacets();
 long  faces = 0;	//this is primitive face(triangle)
 for(int i = 0; i < fno; i++){
  int n, inodes[4];
  m_polyhedron->GetFacet(i + 1, n, inodes);
  if(n == 3)
   faces += 1;
  else if(n == 4)
   faces += 2;
 }

 if(faces < 1)
  return;

 long fcr = 0;
 m_icount = 3 * faces;
 m_indices = new long[m_icount];

 Vertex* varr = new Vertex[m_icount];
 long    vidx = 0;
 for(int i = 0; i < fno; i++){
  int n, inodes[4];
  SbVec3f nor = m_polyhedron->GetNormal(i + 1);
  m_polyhedron->GetFacet(i + 1, n, inodes);

  for(int j = 0; j < n; j++){
   Vertex v;
   SbVec3f pos = m_polyhedron->GetVertex(inodes[j]);
   v.pos[0] = pos[0], v.pos[1] = pos[1], v.pos[2] = pos[2];
   v.nor[0] = nor[0], v.nor[1] = nor[1], v.nor[2] = nor[2];
   long iarr = hasVertex(varr, vidx, v);
   if(iarr < 0){
	varr[vidx] = v;
	inodes[j] = vidx;
	vidx++;
   }else{
	inodes[j] = iarr;
   }
  }

  if(n >= 3){
   m_indices[fcr] = (inodes[0] <= m_icount) ? (inodes[0]) : (0); fcr++;
   m_indices[fcr] = (inodes[1] <= m_icount) ? (inodes[1]) : (0); fcr++;
   m_indices[fcr] = (inodes[2] <= m_icount) ? (inodes[2]) : (0); fcr++;
   if(n == 4){
    m_indices[fcr] = (inodes[0] <= m_icount) ? (inodes[0]) : (0); fcr++;
    m_indices[fcr] = (inodes[1] <= m_icount) ? (inodes[2]) : (0); fcr++;
    m_indices[fcr] = (inodes[2] <= m_icount) ? (inodes[3]) : (0); fcr++;
   }
  }
 }
 m_vcount = vidx;
 m_vertices = new Vertex[m_vcount];
 memcpy(m_vertices, varr, m_vcount * sizeof(Vertex));
 delete [] varr;
}
//*/

//////////////////////////////////////////////////
// II - Method, comment it if u use I - Method
//////////////////////////////////////////////////
///*
void SoPolyhedron::makeShape(SbPolyhedron* sp)
{
 if(!sp || (m_polyhedron->GetNoFacets() < 1))
  return;

 if(m_vertices){
  delete [] m_vertices;
  m_vertices = 0;
  m_vcount = 0;
 }

 if(m_indices){
  delete [] m_indices;
  m_indices = 0;
  m_icount = 0;
 }

 int   fno = m_polyhedron->GetNoFacets();
 long  faces = 0;	//this is primitive face(triangle)
 for(int i = 0; i < fno; i++){
  int n, inodes[4];
  m_polyhedron->GetFacet(i + 1, n, inodes);
  if(n == 3)
   faces += 1;
  else if(n == 4)
   faces += 2;
 }

 if(faces < 1)
  return;

 long fcr = 0;
 long vno = m_polyhedron->GetNoVertices();
 m_vcount = 3 * faces;
 m_vertices = new Vertex[m_vcount];
 for(int i = 0; i < fno; i++){
  int n, inodes[4];
  HVPoint3D  pos[4];

  // rbianchi
  //  SbVec3f nor = m_polyhedron->GetNormal(i + 1);
  HVNormal3D nor = m_polyhedron->GetNormal(i + 1);
  //---
  m_polyhedron->GetFacet(i + 1, n, inodes);
  pos[0] = (inodes[0] <= vno) ? (m_polyhedron->GetVertex(inodes[0])) : (m_polyhedron->GetVertex(1));
  pos[1] = (inodes[1] <= vno) ? (m_polyhedron->GetVertex(inodes[1])) : (m_polyhedron->GetVertex(1));
  pos[2] = (inodes[2] <= vno) ? (m_polyhedron->GetVertex(inodes[2])) : (m_polyhedron->GetVertex(1));
  if(n == 4)
   pos[3] = (inodes[3] <= vno) ? (m_polyhedron->GetVertex(inodes[3])) : (m_polyhedron->GetVertex(1));

  if(n >= 3){
   m_vertices[fcr].pos[0] = pos[0][0]; m_vertices[fcr].nor[0] = nor[0];
   m_vertices[fcr].pos[1] = pos[0][1]; m_vertices[fcr].nor[1] = nor[1];
   m_vertices[fcr].pos[2] = pos[0][2]; m_vertices[fcr].nor[2] = nor[2];
   fcr++;
   m_vertices[fcr].pos[0] = pos[1][0]; m_vertices[fcr].nor[0] = nor[0];
   m_vertices[fcr].pos[1] = pos[1][1]; m_vertices[fcr].nor[1] = nor[1];
   m_vertices[fcr].pos[2] = pos[1][2]; m_vertices[fcr].nor[2] = nor[2];
   fcr++;
   m_vertices[fcr].pos[0] = pos[2][0]; m_vertices[fcr].nor[0] = nor[0];
   m_vertices[fcr].pos[1] = pos[2][1]; m_vertices[fcr].nor[1] = nor[1];
   m_vertices[fcr].pos[2] = pos[2][2]; m_vertices[fcr].nor[2] = nor[2];
   fcr++;
   if(n == 4){
    m_vertices[fcr].pos[0] = pos[0][0]; m_vertices[fcr].nor[0] = nor[0];
	m_vertices[fcr].pos[1] = pos[0][1]; m_vertices[fcr].nor[1] = nor[1];
	m_vertices[fcr].pos[2] = pos[0][2]; m_vertices[fcr].nor[2] = nor[2];
	fcr++;
	m_vertices[fcr].pos[0] = pos[2][0]; m_vertices[fcr].nor[0] = nor[0];
	m_vertices[fcr].pos[1] = pos[2][1]; m_vertices[fcr].nor[1] = nor[1];
	m_vertices[fcr].pos[2] = pos[2][2]; m_vertices[fcr].nor[2] = nor[2];
	fcr++;
	m_vertices[fcr].pos[0] = pos[3][0]; m_vertices[fcr].nor[0] = nor[0];
	m_vertices[fcr].pos[1] = pos[3][1]; m_vertices[fcr].nor[1] = nor[1];
	m_vertices[fcr].pos[2] = pos[3][2]; m_vertices[fcr].nor[2] = nor[2];
	fcr++;
   }
  }
 }
}
//*/
