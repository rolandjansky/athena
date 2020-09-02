/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Surface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iomanip>
#include <iostream>
#include <utility>

const double Trk::Surface::s_onSurfaceTolerance = 10e-5; // 0.1 * micron

#ifndef NDEBUG
std::atomic<unsigned int> Trk::Surface::s_numberOfInstantiations{ 0 };
std::atomic<unsigned int> Trk::Surface::s_numberOfFreeInstantiations{ 0 };
#endif

Trk::Surface::Surface()
  : m_transform(nullptr)
  , m_center(nullptr)
  , m_normal(nullptr)
  , m_associatedDetElement(nullptr)
  , m_associatedDetElementId()
  , m_associatedLayer(nullptr)
  , m_materialLayer(nullptr)
  , m_owner(Trk::noOwn)
{
#ifndef NDEBUG
  s_numberOfInstantiations++;     // EDM Monitor
  s_numberOfFreeInstantiations++; // EDM Monitor
#endif
}

Trk::Surface::Surface(Amg::Transform3D* tform)
  : m_transform(nullptr)
  , m_center(nullptr)
  , m_normal(nullptr)
  , m_associatedDetElement(nullptr)
  , m_associatedDetElementId()
  , m_associatedLayer(nullptr)
  , m_materialLayer(nullptr)
  , m_owner(Trk::noOwn)
{
  m_transform=std::unique_ptr<Amg::Transform3D>(tform);
#ifndef NDEBUG
  s_numberOfInstantiations++; // EDM Monitor - increment one instance
  s_numberOfFreeInstantiations++;
#endif
}

Trk::Surface::Surface(std::unique_ptr<Amg::Transform3D> tform)
  : Surface(tform.release())
{
  // No EDM monitor here since we delegate to the previous constructor.
}

Trk::Surface::Surface(const Trk::TrkDetElementBase& detelement)
  : m_transform(nullptr)
  , m_center(nullptr)
  , m_normal(nullptr)
  , m_associatedDetElement(&detelement)
  , m_associatedDetElementId()
  , m_associatedLayer(nullptr)
  , m_materialLayer(nullptr)
  , m_owner(Trk::DetElOwn)
{
#ifndef NDEBUG
  s_numberOfInstantiations++; // EDM Monitor - increment one instance
#endif
}

Trk::Surface::Surface(const Trk::TrkDetElementBase& detelement, const Identifier& id)
  : m_transform(nullptr)
  , m_center(nullptr)
  , m_normal(nullptr)
  , m_associatedDetElement(&detelement)
  , m_associatedDetElementId(id)
  , m_associatedLayer(nullptr)
  , m_materialLayer(nullptr)
  , m_owner(Trk::DetElOwn)
{
#ifndef NDEBUG
  s_numberOfInstantiations++; // EDM Monitor - increment one instance
#endif
}

// copy constructor - Attention! sets the associatedDetElement to 0 and the identifier to invalid
Trk::Surface::Surface(const Surface& sf)
  : m_transform(nullptr)
  , m_center(nullptr)
  , m_normal(nullptr)
  , m_associatedDetElement(nullptr)
  , m_associatedDetElementId()
  , m_associatedLayer(sf.m_associatedLayer)
  , m_materialLayer(sf.m_materialLayer)
  , m_owner(Trk::noOwn)
{

  m_transform = std::make_unique<Amg::Transform3D>(sf.transform());

#ifndef NDEBUG
  s_numberOfInstantiations++; // EDM Monitor - increment one instance
  // this is by definition a free surface since a copy is not allowed to point to the det element
  s_numberOfFreeInstantiations++;
#endif
}

// copy constructor with shift - Attention! sets the associatedDetElement to 0 and the identifieer to invalid
// also invalidates the material layer
Trk::Surface::Surface(const Surface& sf, const Amg::Transform3D& shift)
  : m_transform(sf.m_transform ? std::make_unique<Amg::Transform3D>(shift * (*(sf.m_transform)))
                               : std::make_unique<Amg::Transform3D>(shift))
  , m_center((sf.m_center) ? std::make_unique<const Amg::Vector3D>(shift * (*(sf.m_center))) : nullptr)
  , m_normal(nullptr)
  , m_associatedDetElement(nullptr)
  , m_associatedDetElementId()
  , m_associatedLayer(nullptr)
  , m_materialLayer(nullptr)
  , m_owner(Trk::noOwn)
{
#ifndef NDEBUG
  s_numberOfInstantiations++; // EDM Monitor - increment one instance
  // this is by definition a free surface since a copy is not allowed to point to the det element
  s_numberOfFreeInstantiations++;
#endif
}

// destructor
Trk::Surface::~Surface()
{
#ifndef NDEBUG
  s_numberOfInstantiations--; // EDM Monitor - decrement one instance
  if (isFree())
    s_numberOfFreeInstantiations--;
#endif
}

// assignment operator
// the assigned surfaces loses its link to the detector element
Trk::Surface&
Trk::Surface::operator=(const Trk::Surface& sf)
{
  if (this != &sf) {
    m_transform.release();
    m_center.release();
    m_normal.release();
    m_transform = std::make_unique<Amg::Transform3D>(sf.transform());
    m_associatedDetElement = nullptr;
    m_associatedDetElementId = Identifier();
    m_associatedLayer = sf.m_associatedLayer;
    m_materialLayer = sf.m_materialLayer;
    m_owner = Trk::noOwn;
  }
  return *this;
}

// returns the LocalPosition on a surface of a GlobalPosition
Amg::Vector2D*
Trk::Surface::positionOnSurface(const Amg::Vector3D& glopo,
                                const BoundaryCheck& bchk,
                                double tol1,
                                double tol2) const
{
  Amg::Vector2D* posOnSurface = globalToLocal(glopo, tol1);
  if (!bchk){
    return posOnSurface;
  }
  if (posOnSurface && insideBounds(*posOnSurface, tol1, tol2)){
    return posOnSurface;
  }
  delete posOnSurface;
  return nullptr;
}

// checks if GlobalPosition is on Surface and inside bounds
bool
Trk::Surface::isOnSurface(const Amg::Vector3D& glopo,
                          BoundaryCheck bchk,
                          double tol1,
                          double tol2) const
{
  const Amg::Vector2D* posOnSurface = positionOnSurface(glopo, bchk, tol1, tol2);
  if (posOnSurface) {
    delete posOnSurface;
    return true;
  } 
    return false;
}

// return the measurement frame
Amg::RotationMatrix3D
Trk::Surface::measurementFrame(const Amg::Vector3D&, const Amg::Vector3D&) const
{
  return transform().rotation();
}

// for the EDM monitor
unsigned int
Trk::Surface::numberOfInstantiations()
{
#ifndef NDEBUG
  return s_numberOfInstantiations;
#else
  return 0;
#endif
}

// for the EDM monitor
unsigned int
Trk::Surface::numberOfFreeInstantiations()
{
#ifndef NDEBUG
  return s_numberOfFreeInstantiations;
#else
  return 0;
#endif
}

// overload dump for MsgStream operator
MsgStream&
Trk::Surface::dump(MsgStream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(4);
  sl << name() << std::endl;
  sl << "     Center position  (x, y, z) = (" << center().x() << ", " << center().y() << ", " << center().z() << ")"
     << std::endl;
  Amg::RotationMatrix3D rot(transform().rotation());
  Amg::Vector3D rotX(rot.col(0));
  Amg::Vector3D rotY(rot.col(1));
  Amg::Vector3D rotZ(rot.col(2));
  sl << std::setprecision(6);
  sl << "     Rotation:             colX = (" << rotX(0) << ", " << rotX(1) << ", " << rotX(2) << ")" << std::endl;
  sl << "                           colY = (" << rotY(0) << ", " << rotY(1) << ", " << rotY(2) << ")" << std::endl;
  sl << "                           colZ = (" << rotZ(0) << ", " << rotZ(1) << ", " << rotZ(2) << ")" << std::endl;
  sl << "     Bounds  : " << bounds();
  sl << std::setprecision(-1);
  return sl;
}

// overload dump for MsgStream operator
std::ostream&
Trk::Surface::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(4);
  sl << name() << std::endl;
  sl << "     Center position  (x, y, z) = (" << center().x() << ", " << center().y() << ", " << center().z() << ")"
     << std::endl;
  Amg::RotationMatrix3D rot(transform().rotation());
  Amg::Vector3D rotX(rot.col(0));
  Amg::Vector3D rotY(rot.col(1));
  Amg::Vector3D rotZ(rot.col(2));
  sl << std::setprecision(6);
  sl << "     Rotation:             colX = (" << rotX(0) << ", " << rotX(1) << ", " << rotX(2) << ")" << std::endl;
  sl << "                           colY = (" << rotY(0) << ", " << rotY(1) << ", " << rotY(2) << ")" << std::endl;
  sl << "                           colZ = (" << rotZ(0) << ", " << rotZ(1) << ", " << rotZ(2) << ")" << std::endl;
  sl << "     Bounds  : " << bounds();
  sl << std::setprecision(-1);
  return sl;
}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/
MsgStream&
Trk::operator<<(MsgStream& sl, const Trk::Surface& sf)
{
  return sf.dump(sl);
}

std::ostream&
Trk::operator<<(std::ostream& sl, const Trk::Surface& sf)
{
  return sf.dump(sl);
}
