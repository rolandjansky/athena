/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BoundarySurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_BOUNDARYSURFACE_H
#define TRKVOLUMES_BOUNDARYSURFACE_H

// Trk
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkVolumes/Volume.h"
// Gaudi
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

// class TrackParameters;
class Surface;

/**
 @class BoundarySurface

 Description of a BoundarySurface inside the tracking realm,
 it extends the Surface description to make a surface being a boundary of a
 Trk::Volume, Trk::TrackingVolume or a Trk::MagneticFieldVolume.

 To avoid dynamic_cast operations the BoundarySurface class is realized as a
 templated class, with the Volume type as the template argument.

 A Trk::BoundarySurface can have an inside Volume and an outside Volume, resp.
 a Trk::BinnedArray for inside or outside direction.

 The GeometryBuilder as defined in the TrkDetDescrTools Package is declared
 to be friend, so that it can glue Volumes together by sharing the same
 Boundary Surface.

 @author Andreas.Salzburger@cern.ch
*/

template <class Tvol>
class BoundarySurface {
  /** typedef the BinnedArray */
  typedef BinnedArray<const Tvol> VolumeArray;

 public:
  /** Default Constructor - needed for pool and inherited classes */
  BoundarySurface()
      : m_insideVolume(0),
        m_outsideVolume(0),
        m_insideVolumeArray(),
        m_outsideVolumeArray() {}

  /** Constructor for a Boundary with exact two Volumes attached to it*/
  BoundarySurface(const Tvol* inside, const Tvol* outside)
      : m_insideVolume(inside),
        m_outsideVolume(outside),
        m_insideVolumeArray(),
        m_outsideVolumeArray() {}

  /** Constructor for a Boundary with exact two Volumes attached to it*/
  BoundarySurface(SharedObject<VolumeArray> insideArray,
                  SharedObject<VolumeArray> outsideArray)
      : m_insideVolume(),
        m_outsideVolume(),
        m_insideVolumeArray(insideArray),
        m_outsideVolumeArray(outsideArray) {}

  /** Get the next Volume depending on the TrackParameters and the requested
   * direction */
  virtual const Tvol* attachedVolume(const TrackParameters& parms,
                                     PropDirection dir) const = 0;

  /** Get the next Volume depending on
   GlobalPosition, GlobalMomentum, dir
   on the TrackParameters and the requested direction */
  virtual const Tvol* attachedVolume(const Amg::Vector3D& pos,
                                     const Amg::Vector3D& mom,
                                     PropDirection dir) const = 0;

  /** templated onBoundary method */
  template <class T>
  bool onBoundary(const T& pars) const {
    return surfaceRepresentation().onSurface(pars);
  }

  /** The Surface Representation of this */
  virtual const Surface& surfaceRepresentation() const = 0;

  /**Virtual Destructor*/
  virtual ~BoundarySurface() {}

  /** output debug information */
  void debugInfo(MsgStream& msg) const;

  /** getters/setters for inside/outside Volume*/
  Tvol const* insideVolume() const;
  void setInsideVolume(const Tvol*);

  Tvol const* outsideVolume() const;
  void setOutsideVolume(const Tvol*);

  /** getters/setters for inside/outside Volume arrays */
  const SharedObject<VolumeArray>& insideVolumeArray() const;
  void setInsideVolumeArray(const SharedObject<VolumeArray>&);
  const SharedObject<VolumeArray>& outsideVolumeArray() const;
  void setOutsideVolumeArray(const SharedObject<VolumeArray>&);

 protected:
  const Tvol* m_insideVolume;
  const Tvol* m_outsideVolume;
  SharedObject<VolumeArray> m_insideVolumeArray;
  SharedObject<VolumeArray> m_outsideVolumeArray;
};

template <class Tvol>
Tvol const* BoundarySurface<Tvol>::insideVolume() const {
  return m_insideVolume;
}
template <class Tvol>
void BoundarySurface<Tvol>::setInsideVolume(const Tvol* vol) {
  m_insideVolume = vol;
}

template <class Tvol>
Tvol const* BoundarySurface<Tvol>::outsideVolume() const {
  return m_outsideVolume;
}
template <class Tvol>
void BoundarySurface<Tvol>::setOutsideVolume(const Tvol* vol) {
  m_outsideVolume = vol;
}

template <class Tvol>
const SharedObject<BinnedArray<const Tvol>>&
BoundarySurface<Tvol>::insideVolumeArray() const {
  return m_insideVolumeArray;
}
template <class Tvol>
void BoundarySurface<Tvol>::setInsideVolumeArray(
    const SharedObject<BinnedArray<const Tvol>>& volArray) {
  m_insideVolumeArray = volArray;
}

template <class Tvol>
const SharedObject<BinnedArray<const Tvol>>&
BoundarySurface<Tvol>::outsideVolumeArray() const {
  return m_outsideVolumeArray;
}
template <class Tvol>
void BoundarySurface<Tvol>::setOutsideVolumeArray(
    const SharedObject<BinnedArray<const Tvol>>& volArray) {
  m_outsideVolumeArray = volArray;
}

template <class Tvol>
inline void BoundarySurface<Tvol>::debugInfo(MsgStream& msg) const {
  msg << "BoundarySurface debug information: " << std::endl;
  msg << "     -> pointer to insideVolume         = " << m_insideVolume
      << std::endl;
  msg << "     -> pointer to insideVolumeArray    = "
      << m_insideVolumeArray.get() << std::endl;
  msg << "     -> pointer to outsideVolume        = " << m_outsideVolume
      << std::endl;
  msg << "     -> pointer to outsideVolumeArray   = "
      << m_outsideVolumeArray.get() << endmsg;
}

}  // end of namespace Trk

#endif  // TRKVOLUMES_BOUNDARYSURFACE_H

