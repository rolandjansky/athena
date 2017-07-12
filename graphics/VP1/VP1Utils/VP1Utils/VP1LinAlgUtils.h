/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1LinAlgUtils                      //
//                                                            //
//  Description: Class holding CLHEP/Inventor algebra         //
//               methods.                                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1LINALGUTILS_H
#define VP1LINALGUTILS_H

class SoTransform;
class SbMatrix;

#include "CLHEP/Geometry/Transform3D.h"

// Eigen Migration
// see atlasoff/DetectorDescription/GeoPrimitives/trunk/GeoPrimitives/CLHEPtoEigenConverter.h
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"


#include <QString>


class VP1LinAlgUtils {
public:

  static void transformToMatrix(SoTransform * xf, SbMatrix& result);
  static void transformToMatrix(const HepGeom::Transform3D &, SbMatrix& result);

  //Will modify (and return) passed SoTransform pointer, unless it is zero. In that case it creates and returns a new SoTransform.
  static SoTransform * toSoTransform(const HepGeom::Transform3D &, SoTransform * t = 0);
  static SoTransform * toSoTransform(const Amg::Transform3D &, SoTransform * t = 0);
  static SoTransform * toSoTransform(const SbMatrix&, SoTransform * t = 0);

  static void decodeTransformation( const SbMatrix&, //NB: Ignores the parts of the matrix related to scaling!
				    float& translation_x, float& translation_y, float& translation_z,
				    float& rotaxis_x, float& rotaxis_y, float& rotaxis_z, float& rotangle_radians );

  //The following tests for sane transformations and will, if
  //contextStr!="", print ERRORs to messageDebug.
  //If called for a large number of transformations, it might be a
  //good idea to only call when in verbose mode. contextStr's are used
  //in the output to ease debugging (i.e. use values such as "input
  //volume transformation").:

  static bool isSane( const HepGeom::Transform3D &, const QString& contextStr = "" );
  static bool isSane( const Amg::Transform3D &, const QString& contextStr = "" );
  static bool isSane( const SoTransform *, const QString& contextStr = "" );
  static bool isSane( const SbMatrix&, const QString& contextStr = "" );

  //Misc:
  static double phiFromXY(const double& x, const double&y );

  //Misc. point/line distances:

  static void distPointLineParam(const Amg::Vector3D& point, const Amg::Vector3D& point0,
             const Amg::Vector3D& point1, double& s);
  //Modifies 's' to contain the
  //parameter describing the point 'P' on the line giving the shortest
  //distance as P=point0 + s*(point1-point0)
  static double distPointLine2(const Amg::Vector3D& point, const Amg::Vector3D& point0,
			       const Amg::Vector3D& point1, double& s);
  //Returns the shortest distance squared between the point 'point'
  //and the line passing 'point0' and 'point1'.

  static void distLineLineParam(const Amg::Vector3D& point0, const Amg::Vector3D& point1,
			    const Amg::Vector3D& point2, const Amg::Vector3D& point3,
			    double& s, double& t);
  //returns void, but modifies 's' and 't' to describe the points 'P'
  //and 'Q' giving the shortest distance between the line passing
  //point0 and point1 and the line passing point2 and point3. I.e: P =
  //point0 + s*(point1 - point0). Q = point2 + t*(point3-point2)

  //FIXME: this should have a corresponding non-void function
  //returning the actual distance (squared), e.g. the distance |PQ|,
  //but this is omitted as it adds unneeded calculations ( for the
  //current usage ). Also rename method.

private:
  VP1LinAlgUtils(){}
  ~VP1LinAlgUtils(){}
  class Imp;
};

#endif
