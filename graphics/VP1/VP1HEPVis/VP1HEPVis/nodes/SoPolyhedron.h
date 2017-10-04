/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SoPolyhedron                      //
//                                                            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2008                            //
//  Update: Giorgi Gvaberidze (ggvaberi@cern.ch)              // 
//          November 2010                                     //
////////////////////////////////////////////////////////////////

#ifndef HEPVis_SoPolyhedron_h
#define HEPVis_SoPolyhedron_h

// Inheritance :
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoSFBool.h>

#include <Inventor/fields/SoSFNode.h>
#include <Inventor/actions/SoGetPrimitiveCountAction.h>

class SbPolyhedron;


/**
 *  SoPolyhedron is an Inventor encapsulation of the HepPolyedron
 * class written by E.Chernyaev. In particular SoPolyhedron permits
 * to represent boolean operations over solids.
 *  To avoid clashes with other libraries (Geant4) where the HepPolyhedron
 * classes may be found, the HepPolyhedron (through usage of CPP macros)
 * had been renamed SbPolyhedron in the HEPVis lib.
 *  The solids are modeled with SbPolyedron objects. The SbPolyhedron
 * permits to produce a new SbPolyhedron according to the boolean
 * operation done on them. The resulting SbPolyhedron  is then given
 * to an SoPolyhedron for rendering.
 *  Note that a boolean operation could be rendered in wire frame
 * by drawing the contour of the resulting solid (not by drawing
 * the wire frame of a triangulation).
 *  See the applications/Polyhedron example.
 */

class SoPolyhedron : public SoShape {

  SO_NODE_HEADER(SoPolyhedron);

public:
  //SoSFBool solid;
  //SoSFBool reducedWireFrame;
  SoSFNode      alternateRep;
public:
  SoPolyhedron();
  SoPolyhedron(const SbPolyhedron&);
  SoPolyhedron(const SbPolyhedron*);
public:
  static void initClass();
protected:
  virtual void computeBBox(SoAction*,SbBox3f&,SbVec3f&);
  virtual void generatePrimitives(SoAction*);

  //virtual void getPrimitiveCount(SoGetPrimitiveCountAction * action);
protected:
  virtual ~SoPolyhedron();
private:
  static bool   s_didInit;
  SbPolyhedron* m_polyhedron;

  //---------------//
public:
  typedef struct{
   float pos[3];
   float nor[3];
  } Vertex;

  Vertex*  m_vertices;
  long*    m_indices;
  long     m_vcount;
  long     m_icount;
  long     hasVertex(Vertex* vertices, long len, Vertex& v);
  void     makeShape(SbPolyhedron*);
public:
 virtual void generateAlternateRep();
 virtual void clearAlternateRep();
 //---------------------//
};

#endif
