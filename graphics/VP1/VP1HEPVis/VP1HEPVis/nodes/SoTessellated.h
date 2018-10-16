/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SOTESSELLATED_H
#define SOTESSELLATED_H

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/nodes/SoShape.h>
#include <vector>

//**************************************************************//
//                                                              //
// Class SoTessellated                                          //
//                                                              //
// Visualization of Tessellated Solid - generic solid defined   //
// by a number of either triangular or quadrangular facets.     //
//                                                              //
//**************************************************************//

class SoTessellated : public SoShape 
{

  SO_NODE_HEADER(SoTessellated);

 public:

  //Fields:
  SoSFBool drawEdgeLines;              //Default: False
  SoSFBool forceEdgeLinesInBaseColour; //Default: True
  SoSFNode alternateRep;

  // Following two methods add facets to the solid
  //
  // NB: must provide ABSOLUTE coordinates!
  // 
  // (relative coordinates should be handled in the
  // SoVisualizeAction beforehand)
  //
  void addTriangularFacet(double x1, double y1, double z1,
			  double x2, double y2, double z2,
			  double x3, double y3, double z3);
  
  void addQuadrangularFacet(double x1, double y1, double z1,
			    double x2, double y2, double z2,
			    double x3, double y3, double z3,
			    double x4, double y4, double z4);

  // Finalize:
  //  1. Compute bounding box and center
  //  2. Touch drawEdgeLines
  void finalize();

  //_____ Required stuff _____ 
  SoTessellated();
  static void initClass();
  virtual void generateAlternateRep();
  virtual void clearAlternateRep();

 protected:

  virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center );
  virtual void generatePrimitives(SoAction *action);
  virtual ~SoTessellated();

 private:

  void setupDefaultPoints();

  std::vector<float> m_points;
  std::vector<float> m_normals;
  SbBox3f m_bbox; 
  SbVec3f m_center; 
  static bool s_didInit;
};

#endif
