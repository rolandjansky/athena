/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVertexOnTrack/VertexOnTrack.h"
#include "VxVertex/RecVertex.h"

// Gaudi & AthenaCommon
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <typeinfo>

namespace {
const double NaN(std::numeric_limits<double>::quiet_NaN());
alignas(16) const Amg::Vector3D INVALID_VECTOR3D(NaN, NaN, NaN);
}

// Constructor with parameters:
Trk::VertexOnTrack::VertexOnTrack(const LocalParameters& locpars,
                                  const Amg::MatrixX& locerr,
                                  const PerigeeSurface& assocSurf)
  : Trk::MeasurementBase(locpars, locerr)
  , PerigeeSurfacePtrHolder(assocSurf)
  , m_globalPosition(std::nullopt)
{}

Trk::VertexOnTrack::VertexOnTrack(
  const LocalParameters& locpars,
  const Amg::MatrixX& locerr,
  Trk::SurfaceUniquePtrT<const PerigeeSurface> assocSurf)
  : Trk::MeasurementBase(locpars, locerr)
  , PerigeeSurfacePtrHolder(assocSurf.release())
  , m_globalPosition(std::nullopt)
{}

// Constructor with parameters:
Trk::VertexOnTrack::VertexOnTrack(const Trk::RecVertex& rvertex,
                                  const Trk::Perigee& perigee)
  : Trk::MeasurementBase()
  , PerigeeSurfacePtrHolder(perigee.associatedSurface())
  , m_globalPosition(Amg::Vector3D(perigee.position()))
{
  if ((rvertex.position() - perigee.associatedSurface().center()).mag() > 10e-5)
    throw GaudiException(
      "Inconsistent input data, Perigee not expressed at vertex!",
      "Trk::VertexOnTrack",
      StatusCode::RECOVERABLE);

  // the local parameters are by definition at (0,0)
  Amg::Vector2D localPosition(0, 0);
  m_localParams = Trk::LocalParameters(localPosition);

  // transform Cartesian (x,y,z) to perigee (d0,z0)
  Amg::MatrixX jacobian(2, 3);
  jacobian.setZero();
  double ptInv = 1. / perigee.momentum().perp();
  jacobian(0, 0) = -ptInv * perigee.momentum().y();
  jacobian(0, 1) = ptInv * perigee.momentum().x();
  jacobian(1, 2) = 1.0;
  // the covariance on matrix of the vertex
  const Amg::MatrixX& vtxCov = rvertex.covariancePosition();
  // the local coordinate
  m_localCovariance = vtxCov.similarity(jacobian);
}

const Amg::Vector3D&
Trk::VertexOnTrack::globalPosition() const
{
  if (m_globalPosition) {
    return *m_globalPosition;
  }
  return INVALID_VECTOR3D;
}

MsgStream&
Trk::VertexOnTrack::dump(MsgStream& sl) const
{
  std::string name(typeid(*this).name());
  sl << "Concrete dump method not implemented - using base class" << std::endl;
  sl << name << "\t  local position = " << this->localParameters() << std::endl;
  sl << name << "\t  global position ( " << this->globalPosition().x() << " , "
     << this->globalPosition().y() << " , " << this->globalPosition().z()
     << " ) " << std::endl;
  sl << name << "\t  has Error Matrix: " << std::endl;
  sl << this->localCovariance() << std::endl;
  sl << name << "\t  has associated surface:" << std::endl;
  sl << this->associatedSurface() << std::endl;
  return sl;
}

std::ostream&
Trk::VertexOnTrack::dump(std::ostream& sl) const
{
  std::string name(typeid(*this).name());
  sl << "Concrete dump method not implemented - using base class" << std::endl;
  sl << name << "\t  local position = " << this->localParameters() << std::endl;
  sl << name << "\t  global position ( " << this->globalPosition().x() << " , "
     << this->globalPosition().y() << " , " << this->globalPosition().z()
     << " ) " << std::endl;
  sl << name << "\t  has Error Matrix: " << std::endl;
  sl << this->localCovariance() << std::endl;
  sl << name << "\t  has associated surface:" << std::endl;
  sl << this->associatedSurface() << std::endl;
  return sl;
}

