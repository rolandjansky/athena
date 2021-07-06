/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// Trk
#include "TrkCaloCluster_OnTrack/CaloCluster_OnTrack.h"

// Gaudi & AthenaCommon
#include "GaudiKernel/MsgStream.h"
#include <limits>
#include <ostream>
#include <string>

namespace {
const double NaN(std::numeric_limits<double>::quiet_NaN());
alignas(16) const Amg::Vector3D INVALID_VECTOR3D(NaN, NaN, NaN);
}

Trk::CaloCluster_OnTrack::CaloCluster_OnTrack(
  const Trk::LocalParameters& locpars,
  const Amg::MatrixX& locerr,
  const Trk::Surface& surface,
  const Trk::EnergyLoss* eloss)
  : MeasurementBase(locpars, locerr)
  , SurfacePtrHolder(surface)
  , m_globalpos(surface.localToGlobal(locpars))
  , m_eloss(eloss)
{}

// default constructor:
Trk::CaloCluster_OnTrack::CaloCluster_OnTrack()
  : Trk::MeasurementBase()
  , SurfacePtrHolder()
  , m_globalpos{ INVALID_VECTOR3D }
  , m_eloss{ nullptr }
{}

// copy constructor:
Trk::CaloCluster_OnTrack::CaloCluster_OnTrack(
  const Trk::CaloCluster_OnTrack& cot)
  : Trk::MeasurementBase(cot)
  , SurfacePtrHolder(cot)
  , m_globalpos(cot.m_globalpos)
{
  m_eloss.reset(cot.m_eloss ? new Trk::EnergyLoss(*cot.m_eloss) : nullptr);
}

// assignment operator:
Trk::CaloCluster_OnTrack&
Trk::CaloCluster_OnTrack::operator=(const Trk::CaloCluster_OnTrack& cot)
{
  if (&cot != this) {
    Trk::MeasurementBase::operator=(cot);
    SurfacePtrHolder::operator=(cot);
    m_globalpos = cot.m_globalpos;
    m_eloss.reset(cot.m_eloss ? new Trk::EnergyLoss(*cot.m_eloss) : nullptr);
  }
  return *this;
}

Trk::CaloCluster_OnTrack*
Trk::CaloCluster_OnTrack::clone() const
{
  return new Trk::CaloCluster_OnTrack(*this);
}

const Amg::Vector3D&
Trk::CaloCluster_OnTrack::globalPosition() const
{
  return m_globalpos;
}

MsgStream&
Trk::CaloCluster_OnTrack::dump(MsgStream& sl) const
{
  sl << "Trk::CaloCluster_OnTrack { "
     << "\n";
  sl << "\t  surface = " << associatedSurface() << "\n";
  sl << "\t  position = (" << localParameters() << endmsg;
  sl << "\t  has Error Matrix: "
     << "\n";
  sl << localCovariance() << "}"
     << "\n";

  return sl;
}

std::ostream&
Trk::CaloCluster_OnTrack::dump(std::ostream& sl) const
{
  sl << "\t  surface = " << associatedSurface() << "\n";
  sl << "\t  position = (" << localParameters() << "\n";
  sl << "\t  has Error Matrix: "
     << "\n";
  sl << localCovariance() << "}"
     << "\n";
  return sl;
}

