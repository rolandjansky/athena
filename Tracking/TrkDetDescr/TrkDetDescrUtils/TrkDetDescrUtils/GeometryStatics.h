/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeometryStatics.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_GEOMETRYSTATICS_H
#define TRKDETDESCRUTILS_GEOMETRYSTATICS_H

#include "GeoPrimitives/GeoPrimitives.h"

/** Define statics for Geometry in Tracking
 */
namespace Trk {

// transformations

  static Amg::Transform3D s_idTransform = Amg::Transform3D::Identity();          //!< idendity transformation
  static Amg::Rotation3D s_idRotation = Amg::Rotation3D::Identity();            //!< idendity rotation
  
  // axis system
  
  static Amg::Vector3D s_xAxis(1,0,0);        //!< global x Axis;
  static Amg::Vector3D s_yAxis(0,1,0);        //!< global y Axis;
  static Amg::Vector3D s_zAxis(0,0,1);        //!< global z Axis;
  
  // origin
  
  static Amg::Vector3D s_origin(0,0,0);       //!< origin position
  
  static double helper[9] = {0.,1.,0.,1.,0.,0.,0.,0.,-1.};
  
  static Amg::RotationMatrix3D s_idRotationZinverse(helper);

}

#endif // TRKDETDESCRUTILS_GEOMETRYSTATICS_H
