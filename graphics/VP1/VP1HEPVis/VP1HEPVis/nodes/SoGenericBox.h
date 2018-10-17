/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SoGenericBox                        //
//                                                            //
//  Description: Generic way to draw boxes with arbitrary     //
//               corners and possibly lines at the edges.     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: December 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SOGENERICBOX_H
#define SOGENERICBOX_H

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/nodes/SoShape.h>

class SoGenericBox : public SoShape {

  SO_NODE_HEADER(SoGenericBox);

public:

  //Fields:
  SoSFBool drawEdgeLines;//Default: False
  SoSFBool forceEdgeLinesInBaseColour;//Default: True
  SoSFNode alternateRep;

  //NB: For performance reason we deviate from the standard Inventor
  //way here. We keep the points in an array, and the user must use
  //one of the following setXXX(..) methods to ensure that they get
  //sensible values (the fDrawEdgeLines field will be touched inside
  //these methods, to ensure a proper cache-invalidation).

  // -> Axis-aligned box
  void setParametersForBox( float dx, float dy, float dz,
			    float xcenter = 0.0, float ycenter = 0.0,  float zcenter = 0.0 );//NB: These are half-lengths.

  // -> Barrel cell (i.e. fix inner face at cylinder around Z-axis, with r=cellDistance):
  void setParametersForBarrelEtaPhiCell( double etaMin, double etaMax,
					 double phiMin, double phiMax,
					 double cellDepth, double cellDistance,
					 double etasqueezefact = 1.0,
					 double phisqueezefact = 1.0 );

  // -> EndCap cell (i.e. fix inner face at plane at  |Z|=|cellDistance|):
  void setParametersForEndCapEtaPhiCell( double etaMin, double etaMax,
					 double phiMin, double phiMax,
					 double cellDepth, double cellDistance,
					 double etasqueezefact = 1.0,
					 double phisqueezefact = 1.0 );

  // -> Trd
  void setParametersForTrd( float dx1, float dx2,
			    float dy1, float dy2,
			    float dz );

  // -> Trapezoid
  void setParametersForTrapezoid( float dz, float theta, float phi, float dy1,
				  float dx1, float dx2, float dy2, float dx3,
				  float dx4, float alp1, float alp2 );

  // -> Generic [NB: The 4 points around each face should lie in a common plane!!]
  void setGenericParameters( float x0, float y0, float z0,
			     float x1, float y1, float z1,
			     float x2, float y2, float z2,
			     float x3, float y3, float z3,
			     float x4, float y4, float z4,
			     float x5, float y5, float z5,
			     float x6, float y6, float z6,
			     float x7, float y7, float z7 );

  //Todo: We could even add a transformation for the TRD (or anything
  //else... perhaps an "applyTransform" method to call after the first
  //setParameters...?)
  //(i.e. specify in absolute coordinates, and thus avoid a
  //transformation in the scenegraph!)

  //Required stuff:
  SoGenericBox();
  static void initClass();//You can call this as many times as you like
  virtual void generateAlternateRep();
  virtual void clearAlternateRep();

protected:

  virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center );
  virtual void generatePrimitives(SoAction *action);
  virtual ~SoGenericBox();

private:

  void internalSetParametersForEtaPhiCell( bool barrel, double etaMin, double etaMax,
					   double phiMin, double phiMax,
					   double cellDepth, double cellDistance,
					   double etasqueezefact, double phisqueezefact );

  void ensurePointsAllocated();
  void pointsUpdated();
  void setupDefaultPoints();

  float * m_points;
  float * m_normals;
  SbBox3f m_bbox;
  SbVec3f m_center;
  static bool s_didInit;
};

#endif
