/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PseudoMeasurementOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
//#include "TrkEventPrimitives/LocalParameters.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkSurfaces/Surface.h"
#include <string>
#include <typeinfo>

namespace {
// helper to move pointer and set source to nullptr
template<class T>
T
move_ptr(T& src)
{
  T tmp = src;
  src = nullptr;
  return tmp;
}

const double NaN(std::numeric_limits<double>::quiet_NaN());
alignas(16) const Amg::Vector3D INVALID_VECTOR3D(NaN, NaN, NaN);
}

Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack(
  const LocalParameters& locpars,
  const Amg::MatrixX& locerr,
  const Surface& assocSurf)
  : Trk::MeasurementBase(locpars, locerr)
  , SurfacePtrHolder(assocSurf)
  , m_globalPosition()
{
  m_globalPosition = m_associatedSurface->center();
}

Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack(
  const LocalParameters& locpars,
  const Amg::MatrixX& locerr,
  Trk::ConstSurfaceUniquePtr assocSurf)
  : Trk::MeasurementBase(locpars, locerr)
  , SurfacePtrHolder(assocSurf.release())
  , m_globalPosition()
{
  m_globalPosition = m_associatedSurface->center();
}

// default constructor:
Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack()
  : Trk::MeasurementBase()
  , SurfacePtrHolder(nullptr)
  , m_globalPosition(INVALID_VECTOR3D)
{}

MsgStream&
Trk::PseudoMeasurementOnTrack::dump(MsgStream& sl) const
{
  std::string name(typeid(*this).name());
  sl << "Concrete dump method not implemented - using base class" << std::endl;
  sl << name << "\t  local position = " << this->localParameters() << std::endl;
  sl << name << "\t  global position ( " << this->globalPosition().x() << " , "
     << this->globalPosition().y() << " , " << this->globalPosition().z()
     << " ) " << std::endl;
  sl << name << "\t  has error matrix: " << std::endl;
  sl << this->localCovariance() << std::endl;
  sl << name << "\t  has associated surface:" << std::endl;
  sl << this->associatedSurface() << std::endl;
  return sl;
}

std::ostream&
Trk::PseudoMeasurementOnTrack::dump(std::ostream& sl) const
{
  std::string name(typeid(*this).name());
  sl << "Concrete dump method not implemented - using base class" << std::endl;
  sl << name << "\t  local position = " << this->localParameters() << std::endl;
  sl << name << "\t  global position ( " << this->globalPosition().x() << " , "
     << this->globalPosition().y() << " , " << this->globalPosition().z()
     << " ) " << std::endl;
  sl << name << "\t  has error matrix: " << std::endl;
  sl << this->localCovariance() << std::endl;
  sl << name << "\t  has associated surface:" << std::endl;
  sl << this->associatedSurface() << std::endl;
  return sl;
}

