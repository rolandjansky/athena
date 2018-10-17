/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class MuonChamberProjectionHelper         //
//                                                            //
//  Description: ...                                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2008                             //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef MUONCHAMBERPROJECTIONHELPER_H
#define MUONCHAMBERPROJECTIONHELPER_H

#include "VP1Base/VP1HelperClassBase.h"
#include "GeoModelKernel/GeoPVConstLink.h"

//#include "CLHEP/Geometry/Point3D.h"
//#include "CLHEP/Vector/ThreeVector.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"


class StoreGateSvc;
class IVP1System;

class MuonChamberProjectionHelper : public VP1HelperClassBase {
public:

  MuonChamberProjectionHelper( StoreGateSvc * detectorStore );//All messages to stdout
  MuonChamberProjectionHelper( IVP1System * sys );//Use this constructor to get messages in gui
  ~MuonChamberProjectionHelper();

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // To figure out if a given PV link is to an mdt chamber:
  //
  bool isKnownMDTChamber( const GeoPVConstLink& mdtChamber );


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // To figure out how long drift-circles one must construct in order to project them on the end of the chambers:
  // * If radius is not 0.0, then the returned distances will be
  //   increased, such as to be the maximum value that one would get if
  //   calculating the distance of all the points on the circumference
  //   of the drift-circle of the given radius, around "point".
  // * Returns false in case of problems.
  //
  bool getDistancesToMDTChamberWallsAlongLine( const GeoPVConstLink& mdtChamber,
					       const Amg::Vector3D & point, const Amg::Vector3D& lineDirection,
					       double& distanceToFirstEndPlane, double& distanceToSecondEndPlane,
					       const double& radius = 0.0 );
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // To project tracks/segments (line segment from pointA to pointB)
  // onto chamber ends.
  // * Sets outsidechamber to true in case projection falls entirely
  //   outside chamber (dont use returned points in that case).
  // * Returns false in case of problems.
  //
  bool projectAndConstrainLineSegmentToMDTChamberEndWalls( const GeoPVConstLink& mdtChamber,
							   const Amg::Vector3D & pointA, const Amg::Vector3D & pointB,
							   Amg::Vector3D & firstEndWall_pointA, Amg::Vector3D & firstEndWall_pointB,
							   Amg::Vector3D & secondEndWall_pointA, Amg::Vector3D & secondEndWall_pointB,
							   bool& outsidechamber );
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Clips the line segment from A to B to the volume of a given MDT chamber
  // * "extradist" effectively extends the chamber dimensions in order to
  //   let the resulting line segment extend slightly outside the
  //   chamber ( negative values are allowed ).
  // * Sets outsidechamber to true in case line segment falls entirely
  //   outside chamber (dont use returned points in that case).
  // * Returns false in case of problems.
  //
  bool clipLineSegmentToMDTChamber( const GeoPVConstLink& mdtChamber,
		            Amg::Vector3D & pointA, Amg::Vector3D & pointB, bool& outsidechamber,
				    const double & extradist = 0.0 );
  //FIXME: Right now we do not clip in the y dimensions!!!
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////



  static Amg::Vector3D& applyTransformToVector( const Amg::Transform3D& m, Amg::Vector3D& v);//Fixme: Better place for this?

private:

  //It is illegal to copy/assign a MuonChamberProjectionHelper:
  MuonChamberProjectionHelper(const MuonChamberProjectionHelper & );
  MuonChamberProjectionHelper & operator= (const MuonChamberProjectionHelper & );

  class Imp;
  Imp * m_d;

};

#endif
