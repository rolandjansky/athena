/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_VOLUMEBOUNDS_H
#define TRKVOLUMES_VOLUMEBOUNDS_H

// STD
#include <iomanip>
#include <iostream>
// GaudiKernel
#include "GaudiKernel/MsgStream.h"
// Trk
#include "CxxUtils/checker_macros.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkDetDescrUtils/ObjectAccessor.h"

namespace Trk {

class Surface;
class Volume;

/**
 @class VolumeBounds

 Pure Absract Base Class for Volume bounds.

 Trk::VolumeBounds are a set of up to six confining Surfaces that are stored in
 a std::vector. Each type of Trk::VolumeBounds has to implement a
 decomposeToSurfaces() and a inside() method.

 The orientation of the Surfaces are in a way that the normal vector points to
 the oustide world.

 The Volume, retrieving a set of Surfaces from the VolumeBounds,
 can turn the Surfaces into BoundarySurfaces.

 @author Andreas.Salzburger@cern.ch
 */

class VolumeBounds {
 public:
  /**Default Constructor*/
  VolumeBounds() {}

  /**Destructor*/
  virtual ~VolumeBounds() {}

  /** clone() method to make deep copy in Volume copy constructor and for
     assigment operator of the Surface class.*/
  virtual VolumeBounds* clone() const = 0;

  /** Checking if position given in volume frame is inside*/
  virtual bool inside(const Amg::Vector3D& pos, double tol = 0.) const = 0;

  /** Method to decompose the Bounds into Surfaces, the Volume can turn them
   * into BoundarySurfaces */
  virtual const std::vector<const Trk::Surface*>* decomposeToSurfaces(
    const Amg::Transform3D& transform) = 0;

  /** Provide accessor for BoundarySurfaces */
  virtual ObjectAccessor boundarySurfaceAccessor(
      const Amg::Vector3D& gp, const Amg::Vector3D& dir,
      bool forceInside = false) const = 0;

  /** Output Method for MsgStream, to be overloaded by child classes */
  virtual MsgStream& dump(MsgStream& sl) const = 0;

  /** Output Method for std::ostream, to be overloaded by child classes */
  virtual std::ostream& dump(std::ostream& sl) const = 0;
};

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream& operator<<(MsgStream& sl, const VolumeBounds& vb);
std::ostream& operator<<(std::ostream& sl, const VolumeBounds& vb);

}  // namespace Trk

#endif  // TRKVOLUMES_VOLUMEBOUNDS_H
