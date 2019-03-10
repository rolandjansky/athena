/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingPixelClustersOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
#include "InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack.h"
#include <ostream>
#include <cmath>

// default constructor
InDet::CompetingPixelClustersOnTrack::CompetingPixelClustersOnTrack():
        Trk::CompetingRIOsOnTrack(),
        m_globalPosition(0),
        m_containedChildRots(0)
        {}

// copy constructor
InDet::CompetingPixelClustersOnTrack::CompetingPixelClustersOnTrack(const InDet::CompetingPixelClustersOnTrack& compROT) :
        Trk::CompetingRIOsOnTrack(compROT),
        m_globalPosition(compROT.m_globalPosition ? new Amg::Vector3D(*compROT.m_globalPosition) : 0),
        m_containedChildRots(0) {
    m_containedChildRots = new std::vector< const InDet::PixelClusterOnTrack* >;
    std::vector< const InDet::PixelClusterOnTrack* >::const_iterator rotIter = compROT.m_containedChildRots->begin();
    for (; rotIter!=compROT.m_containedChildRots->end(); ++rotIter) {
        m_containedChildRots->push_back((*rotIter)->clone());
    }
}

// explicit constructor
InDet::CompetingPixelClustersOnTrack::CompetingPixelClustersOnTrack(
    std::vector<const InDet::PixelClusterOnTrack*>* childrots,
    std::vector<AssignmentProb>* assgnProb
    )
    :
    Trk::CompetingRIOsOnTrack( assgnProb),
    m_globalPosition(0),
    m_containedChildRots(childrots)
{
  // initialize local position and error matrix
  setLocalParametersAndErrorMatrix();
}

InDet::CompetingPixelClustersOnTrack& InDet::CompetingPixelClustersOnTrack::operator=(const InDet::CompetingPixelClustersOnTrack& compROT) {
    if (this!=&compROT) {
        // assignment operator of base class
        Trk::CompetingRIOsOnTrack::operator=(compROT);
        // clear rots
        clearChildRotVector();
        delete m_containedChildRots;
        delete m_globalPosition;
        m_containedChildRots = new std::vector<const InDet::PixelClusterOnTrack*>;
        m_globalPosition     = compROT.m_globalPosition ? new Amg::Vector3D(*compROT.m_globalPosition) : 0;
        std::vector<const InDet::PixelClusterOnTrack*>::const_iterator rotIter = compROT.m_containedChildRots->begin();
        for (; rotIter!=compROT.m_containedChildRots->end(); ++rotIter)
            m_containedChildRots->push_back((*rotIter)->clone());
    }
    return (*this);
}

InDet::CompetingPixelClustersOnTrack& InDet::CompetingPixelClustersOnTrack::operator=(InDet::CompetingPixelClustersOnTrack&& compROT) {
    if (this!=&compROT) {
        // move operator of base class
        Trk::CompetingRIOsOnTrack::operator=(std::move(compROT));
        // clear rots
        clearChildRotVector();
        delete m_containedChildRots;
        delete m_globalPosition;
        m_containedChildRots = compROT.m_containedChildRots;
        compROT.m_containedChildRots = nullptr;
        m_globalPosition = compROT.m_globalPosition.exchange(nullptr, std::memory_order_relaxed);
    }
    return (*this);
}

InDet::CompetingPixelClustersOnTrack::~CompetingPixelClustersOnTrack() {
    delete m_globalPosition;
    clearChildRotVector();
    delete m_containedChildRots;
}

void InDet::CompetingPixelClustersOnTrack::clearChildRotVector() {
    std::vector<const InDet::PixelClusterOnTrack*>::const_iterator rotIter = m_containedChildRots->begin();
    for (; rotIter!=m_containedChildRots->end(); ++rotIter)
        delete (*rotIter);
}

MsgStream& InDet::CompetingPixelClustersOnTrack::dump( MsgStream& out ) const {
  using std::ios;
  out << "Trk::CompetingPixelClustersOnTrack with [" << numberOfContainedROTs() 
        << "] competing Pixel RIO_OnTrack objects" << std::endl;
  Trk::CompetingRIOsOnTrack::dump(out);
  out << "  - GlobalPosition        : ";
  if (m_globalPosition==NULL) out << "null pointer"<<std::endl;
  else out << *m_globalPosition<<endmsg;
  return out;
}

std::ostream& InDet::CompetingPixelClustersOnTrack::dump( std::ostream& out ) const {
  using std::ios;
  out << "Trk::CompetingPixelClustersOnTrack with [" << numberOfContainedROTs() 
        << "] competing Pixel RIO_OnTrack objects" << std::endl;
  Trk::CompetingRIOsOnTrack::dump(out);
  out << "  - GlobalPosition        : ";
  if (m_globalPosition==NULL) out << "null pointer"<<std::endl;
  else out << *m_globalPosition<<std::endl;
  return out;
}

// Have all the contained ROTs a common associated surface?
bool InDet::CompetingPixelClustersOnTrack::ROTsHaveCommonSurface(const bool) const {
    return true;
}

const Amg::Vector3D& InDet::CompetingPixelClustersOnTrack::globalPosition() const {
    const Amg::Vector3D* ptr = m_globalPosition.load();
    if(ptr) return *ptr;
    const Amg::Vector3D* newptr = associatedSurface().localToGlobal(localParameters());
    if(m_globalPosition.compare_exchange_strong(ptr, newptr)){
       return *newptr; //new object is now stored in m_globalPosition
    }
    delete newptr; //Object was created on another thread, while this was running, so this is now unneeded.
    return *ptr; //ptr was replaced with other object by compare_exchange_strong, this is now returned.
}

