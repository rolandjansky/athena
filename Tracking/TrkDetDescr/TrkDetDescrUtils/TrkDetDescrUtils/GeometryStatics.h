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

alignas(16) static const Amg::Transform3D s_idTransform = Amg::Transform3D::Identity(); //!< idendity transformation
alignas(16) static const Amg::Rotation3D s_idRotation = Amg::Rotation3D::Identity();    //!< idendity rotation

// axis system

alignas(16) static const Amg::Vector3D s_xAxis(1, 0, 0); //!< global x Axis;
alignas(16) static const Amg::Vector3D s_yAxis(0, 1, 0); //!< global y Axis;
alignas(16) static const Amg::Vector3D s_zAxis(0, 0, 1); //!< global z Axis;

// origin

alignas(16) static const Amg::Vector3D s_origin(0, 0, 0); //!< origin position

static const double helper[9] = { 0., 1., 0., 1., 0., 0., 0., 0., -1. };

alignas(16) static const Amg::RotationMatrix3D s_idRotationZinverse(helper);

}

#endif // TRKDETDESCRUTILS_GEOMETRYSTATICS_H
