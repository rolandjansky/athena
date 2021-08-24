/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSegment.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSegment/TrackSegment.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/MsgStream.h"

namespace {
const double NaN(std::numeric_limits<double>::quiet_NaN());
alignas(16) const Amg::Vector3D INVALID_VECTOR3D(NaN, NaN, NaN);
}

// default constructor
Trk::TrackSegment::TrackSegment()
  : Trk::Segment()
  , SurfacePtrHolderDetEl()
  , m_globalPosition(INVALID_VECTOR3D)
{}

// explicit constructor
Trk::TrackSegment::TrackSegment(const Trk::LocalParameters& locpars,
                                const Amg::MatrixX& locerr,
                                const Trk::Surface* sf,
                                DataVector<const MeasurementBase>* crots,
                                FitQuality* fqual,
                                Segment::Author author)
  : Trk::Segment(locpars, locerr, crots, fqual, author)
  , SurfacePtrHolderDetEl(sf)
  , m_globalPosition(INVALID_VECTOR3D)
{
  if (m_associatedSurface) {
    m_globalPosition =
      Amg::Vector3D(m_associatedSurface->localToGlobal(localParameters()));
  }
}

const Amg::Vector3D&
Trk::TrackSegment::globalPosition() const
{
  return m_globalPosition;
}

MsgStream&
Trk::TrackSegment::dump(MsgStream& out) const
{
  out << "Trk::TrackSegment (generic track segment) " << std::endl;
  out << "  - it contains   : " << numberOfMeasurementBases()
      << " RIO_OnTrack object" << std::endl;
  out << "  - parmaters     : " << std::endl;
  out << "  - parameter key : " << std::endl;
  // TODO - out proper output (see MuonSegment) EJWM
  return out;
}

std::ostream&
Trk::TrackSegment::dump(std::ostream& out) const
{
  out << "Trk::TrackSegment (generic track segment) " << std::endl;
  out << "  - it contains   : " << numberOfMeasurementBases()
      << " RIO_OnTrack object" << std::endl;
  out << "  - parmaters     : " << std::endl;
  out << "  - parameter key : " << std::endl;
  // TODO - out proper output (see MuonSegment) EJWM
  return out;
}

