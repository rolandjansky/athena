/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Volume.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/Volume.h"
#include "TrkDetDescrUtils/ObjectAccessor.h"
#include "TrkVolumes/VolumeBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iostream>

// Default constructor
Trk::Volume::Volume()
  : m_transform(nullptr)
  , m_center(nullptr)
  , m_volumeBounds()
{}

// constructor with HepGeom::Transform3D
Trk::Volume::Volume(Amg::Transform3D* htrans, Trk::VolumeBounds* volbounds)
  : m_transform(htrans)
  , m_center(nullptr)
  , m_volumeBounds(volbounds)
{}

// copy constructor - will up to now not copy the sub structure!
Trk::Volume::Volume(const Trk::Volume& vol)
  : m_transform(
      vol.m_transform ? std::make_unique<Amg::Transform3D>(*(vol.m_transform))
                      : nullptr)
  , m_center(
      (vol.m_center) ? std::make_unique<Amg::Vector3D>(*(vol.m_center))
                     : nullptr)
  , m_volumeBounds(vol.m_volumeBounds)
{}

// copy constructor with shift
Trk::Volume::Volume(const Trk::Volume& vol, const Amg::Transform3D& shift)
  : m_transform(
      vol.m_transform
        ? std::make_unique<Amg::Transform3D>(shift * (*(vol.m_transform)))
        : std::make_unique<Amg::Transform3D>(shift))
  , m_center(
      (vol.m_center)
        ? std::make_unique<Amg::Vector3D>(shift * (*(vol.m_center)))
        : nullptr)
  , m_volumeBounds(vol.m_volumeBounds)
{}

// destructor
Trk::Volume::~Volume() {}

// assignment operator
Trk::Volume&
Trk::Volume::operator=(const Trk::Volume& vol)
{
  if (this != &vol) {
    m_transform = vol.m_transform
                    ? std::make_unique<Amg::Transform3D>(*vol.m_transform)
                    : nullptr;
    m_center = vol.m_center
                 ? std::make_unique<const Amg::Vector3D>(*(vol.m_center))
                 : nullptr;
    m_volumeBounds = vol.m_volumeBounds;
  }
  return *this;
}

Trk::Volume*
Trk::Volume::clone() const
{
  return new Trk::Volume(*this);
}

#if defined(FLATTEN) && defined(__GNUC__)
// We compile this package with optimization, even in debug builds; otherwise,
// the heavy use of Eigen makes it too slow.  However, from here we may call
// to out-of-line Eigen code that is linked from other DSOs; in that case,
// it would not be optimized.  Avoid this by forcing all Eigen code
// to be inlined here if possible.
__attribute__((flatten))
#endif
bool
Trk::Volume::inside(const Amg::Vector3D& gp, double tol) const
{
  if (!m_transform) {
    return (volumeBounds()).inside(gp, tol);
  }
  Amg::Vector3D posInVolFrame((transform().inverse()) * gp);
  return (volumeBounds()).inside(posInVolFrame, tol);
}

Trk::ObjectAccessor
Trk::Volume::boundarySurfaceAccessor(
  const Amg::Vector3D& gp,
  const Amg::Vector3D& dir,
  bool forceInside) const
{
  if (!m_transform) {
    return Trk::ObjectAccessor(
      volumeBounds().boundarySurfaceAccessor(gp, dir, forceInside));
  }
  return Trk::ObjectAccessor(volumeBounds().boundarySurfaceAccessor(
    transform().inverse() * gp, dir, forceInside));
}

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream&
Trk::operator<<(MsgStream& sl, const Trk::Volume& vol)
{
  sl << "Trk::Volume with VolumeBounds :" << vol.volumeBounds() << endmsg;
  return sl;
}

std::ostream&
Trk::operator<<(std::ostream& sl, const Trk::Volume& vol)
{
  sl << "Trk::Volume with VolumeBounds :" << vol.volumeBounds() << std::endl;
  return sl;
}

