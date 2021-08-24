/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingMuonClustersOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "GaudiKernel/MsgStream.h"
// Muon
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
// std
#include <cmath>

namespace Muon {
// default constructor
CompetingMuonClustersOnTrack::CompetingMuonClustersOnTrack()
  : Trk::CompetingRIOsOnTrack()
  , Trk::SurfacePtrHolderDetEl()
  , m_globalPosition()
  , m_containedChildRots(nullptr)
{}

// copy constructor
CompetingMuonClustersOnTrack::CompetingMuonClustersOnTrack(
  const CompetingMuonClustersOnTrack& compROT)
  : Trk::CompetingRIOsOnTrack(compROT)
  , Trk::SurfacePtrHolderDetEl(compROT)
  , m_globalPosition()
  , m_containedChildRots(nullptr)
{
  if (compROT.m_globalPosition) {
    m_globalPosition.store(
      std::make_unique<const Amg::Vector3D>(*compROT.m_globalPosition));
  }

  m_containedChildRots = new std::vector<const MuonClusterOnTrack*>;

  std::vector<const MuonClusterOnTrack*>::const_iterator rotIter =
    compROT.m_containedChildRots->begin();

  for (; rotIter != compROT.m_containedChildRots->end(); ++rotIter) {
    m_containedChildRots->push_back((*rotIter)->clone());
  }
}

// explicit constructor
CompetingMuonClustersOnTrack::CompetingMuonClustersOnTrack(
  std::vector<const MuonClusterOnTrack*>* childrots,
  std::vector<AssignmentProb>* assgnProb)
  : Trk::CompetingRIOsOnTrack(assgnProb)
  , Trk::SurfacePtrHolderDetEl()
  , m_globalPosition()
  , m_containedChildRots(childrots)
{
  setLocalParametersAndErrorMatrix();
}

CompetingMuonClustersOnTrack::CompetingMuonClustersOnTrack(
  const Trk::LocalParameters* locPars,
  const Amg::MatrixX* error,
  const Trk::Surface* assSurf,
  std::vector<const MuonClusterOnTrack*>* childrots,
  std::vector<AssignmentProb>* assgnProb)
  : Trk::CompetingRIOsOnTrack(assgnProb)
  , Trk::SurfacePtrHolderDetEl(assSurf)
  , m_globalPosition()
  , m_containedChildRots(childrots)
{
  Trk::MeasurementBase::m_localParams = *locPars;
  Trk::MeasurementBase::m_localCovariance = *error;
  delete locPars;
  delete error;
}

CompetingMuonClustersOnTrack::CompetingMuonClustersOnTrack(
  const Trk::LocalParameters& locPars,
  const Amg::MatrixX& error,
  const Trk::Surface* assSurf,
  std::vector<const MuonClusterOnTrack*>* childrots,
  std::vector<AssignmentProb>* assgnProb)
  : Trk::CompetingRIOsOnTrack(assgnProb)
  , Trk::SurfacePtrHolderDetEl(assSurf)
  , m_globalPosition()
  , m_containedChildRots(childrots)
{
  Trk::MeasurementBase::m_localParams = locPars;
  Trk::MeasurementBase::m_localCovariance = error;
}

CompetingMuonClustersOnTrack&
CompetingMuonClustersOnTrack::operator=(
  const CompetingMuonClustersOnTrack& compROT)
{
  if (this != &compROT) {
    // assingment operator of base class
    Trk::CompetingRIOsOnTrack::operator=(compROT);
    Trk::SurfacePtrHolderDetEl::operator=(compROT);
    // clear rots
    clearChildRotVector();
    delete m_containedChildRots;

    m_containedChildRots = new std::vector<const MuonClusterOnTrack*>;
    std::vector<const MuonClusterOnTrack*>::const_iterator rotIter =
      compROT.m_containedChildRots->begin();

    for (; rotIter != compROT.m_containedChildRots->end(); ++rotIter) {
      Trk::RIO_OnTrack* rot = (*rotIter)->clone();
      MuonClusterOnTrack* mrot = dynamic_cast<MuonClusterOnTrack*>(rot);
      if (mrot) {
        m_containedChildRots->push_back(mrot);
      }
    }
    if (compROT.m_globalPosition) {
      m_globalPosition.store(
        std::make_unique<const Amg::Vector3D>(*compROT.m_globalPosition));
    } else if (m_globalPosition) {
      m_globalPosition.release().reset();
    }
  }
  return (*this);
}

CompetingMuonClustersOnTrack&
CompetingMuonClustersOnTrack::operator=(CompetingMuonClustersOnTrack&& compROT)
 noexcept {
  if (this != &compROT) {
    Trk::CompetingRIOsOnTrack::operator=(std::move(compROT));
    Trk::SurfacePtrHolderDetEl::operator=(std::move(compROT));
    clearChildRotVector();
    delete m_containedChildRots;
    m_containedChildRots = compROT.m_containedChildRots;
    compROT.m_containedChildRots = nullptr;
    m_globalPosition = std::move(compROT.m_globalPosition);
  }
  return (*this);
}

CompetingMuonClustersOnTrack::~CompetingMuonClustersOnTrack()
{
  clearChildRotVector();
  delete m_containedChildRots;
}

void
CompetingMuonClustersOnTrack::clearChildRotVector()
{
  if (m_containedChildRots) {
    for (const MuonClusterOnTrack* cl : *m_containedChildRots)
      delete cl;
  }
}

MsgStream&
CompetingMuonClustersOnTrack::dump(MsgStream& out) const
{
  out << "Muon::CompetingMuonClustersOnTrack (Muon competingROTs) "
      << std::endl;
  out << "  - it contains   : " << m_containedChildRots->size()
      << " RIO_OnTrack objects" << std::endl;
  out << "  - parameters     : " << std::endl;
  out << "  - parameter key : " << std::endl;
  return out;
}

std::ostream&
CompetingMuonClustersOnTrack::dump(std::ostream& out) const
{
  out << "Muon::CompetingMuonClustersOnTrack (Muon competingROTs) "
      << std::endl;
  out << "  - it contains   : " << m_containedChildRots->size()
      << " RIO_OnTrack objects" << std::endl;
  out << "  - it contains   : " << numberOfContainedROTs()
      << " RIO_OnTrack objects" << std::endl;
  out << "  - parameters     : " << std::endl;
  out << "  - parameter key : " << std::endl;
  return out;
}

// Have all the contained ROTs a common associated surface?
bool
CompetingMuonClustersOnTrack::ROTsHaveCommonSurface(const bool) const
{
  return true;
}

}
