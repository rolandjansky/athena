/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingTRT_DriftCirclesOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


// Trk
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
// InDet
#include "InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack.h"
// std
#include <math.h>

// default constructor
InDet::CompetingTRT_DriftCirclesOnTrack::CompetingTRT_DriftCirclesOnTrack():
  Trk::CompetingRIOsOnTrack(),
  m_associatedSurface(0),
  m_globalPosition(0),
  m_containedChildRots(0),
  //        m_assumedTrackParameters(0),
  m_ROTsHaveCommonSurface(8) {}

// copy constructor
InDet::CompetingTRT_DriftCirclesOnTrack::CompetingTRT_DriftCirclesOnTrack(const InDet::CompetingTRT_DriftCirclesOnTrack& compROT) :
  Trk::CompetingRIOsOnTrack(compROT),
  //m_associatedSurface(compROT.m_associatedSurface ? compROT.m_associatedSurface->clone() : 0),
  m_globalPosition(compROT.m_globalPosition ? new Amg::Vector3D(*compROT.m_globalPosition) : 0),
  m_containedChildRots(0),
  //        m_assumedTrackParameters(compROT.m_assumedTrackParameters ? m_assumedTrackParameters->clone() : 0),
  m_ROTsHaveCommonSurface(compROT.m_ROTsHaveCommonSurface) {
  //        cout << "CompetingTRT_DriftCirclesOnTrack: in copy constructor" << endl;
  if (compROT.m_associatedSurface) {
    // copy only if surface is not one owned by a detector Element
    m_associatedSurface = (!compROT.m_associatedSurface->associatedDetectorElement()) ? compROT.m_associatedSurface->clone() : compROT.m_associatedSurface;
  } else {
    m_associatedSurface = 0;
  }
  m_containedChildRots = new std::vector< const InDet::TRT_DriftCircleOnTrack* >;
  std::vector< const InDet::TRT_DriftCircleOnTrack* >::const_iterator rotIter = compROT.m_containedChildRots->begin();
  for (; rotIter!=compROT.m_containedChildRots->end(); ++rotIter) {
    m_containedChildRots->push_back((*rotIter)->clone());
    //        cout << "pushed back cloned ROT" << endl;
  }
}

// explicit constructor
InDet::CompetingTRT_DriftCirclesOnTrack::CompetingTRT_DriftCirclesOnTrack(
									  const Trk::Surface* sf,
									  std::vector<const InDet::TRT_DriftCircleOnTrack*>* childrots,
									  std::vector<AssignmentProb>* assgnProb,
									  //        const Trk::TrackParameters*  assumedTrkPars,
									  const Trk::LocalParameters* effecLocalPars,
									  //const Trk::ErrorMatrix* effecLocalErrMat,
									  const Amg::MatrixX* effecLocalErrMat,
									  int ROTsHaveComSrfc // meaning of the values are described in the definition of ROTsHaveCommonSurface()
									  ):
  Trk::CompetingRIOsOnTrack(assgnProb),
  m_associatedSurface(sf),
  m_globalPosition(0),
  m_containedChildRots(childrots),
  //    m_assumedTrackParameters(assumedTrkPars),
  m_ROTsHaveCommonSurface(ROTsHaveComSrfc)
{
  
  m_localParams = *effecLocalPars;
  m_localCovariance = *effecLocalErrMat;
  delete effecLocalErrMat;
  delete effecLocalPars;
  //  if (childrots->size() != assgnProb->size()){
  //      // TODO: Throw something...
  //      // maybe delete given objects.
  //         // at least give an error message
  //  } else {
  //         // search for maximum assignment probability (surface of this ROT is used as common surface):
  //         Trk::AssignmentProb maxAssgnProb = m_assignProb->operator[](0);
  //         //unsigned int imax = 0;
  //         for (unsigned int i=1; i<m_rotsnum; i++) {
  //             if (m_assignProb->operator[](i) > maxAssgnProb) {imax=i};
  //         }
  //     }
}

InDet::CompetingTRT_DriftCirclesOnTrack& InDet::CompetingTRT_DriftCirclesOnTrack::operator=(const InDet::CompetingTRT_DriftCirclesOnTrack& compROT) {
  if (this!=&compROT) {
    // assingment operator of base class
    Trk::CompetingRIOsOnTrack::operator=(compROT);
    // clear rots
    clearChildRotVector();
    delete m_containedChildRots;
    delete m_globalPosition;
    //        delete m_assumedTrackParameters;
    // delete surface if not owned by detElement
    if (m_associatedSurface && !m_associatedSurface->associatedDetectorElement())
      delete m_associatedSurface;
    m_containedChildRots = new std::vector<const InDet::TRT_DriftCircleOnTrack*>;
    if (compROT.m_associatedSurface) {
      // copy only if surface is not one owned by a detector Element
      m_associatedSurface = (!compROT.m_associatedSurface->associatedDetectorElement()) ? compROT.m_associatedSurface->clone() : compROT.m_associatedSurface;
    } else {
      m_associatedSurface = 0;
    }
    m_globalPosition     = compROT.m_globalPosition ? new Amg::Vector3D(*compROT.m_globalPosition) : 0;
    //        m_assumedTrackParameters     = compROT.m_assumedTrackParameters ? compROT.m_assumedTrackParameters->clone() : 0;
    m_ROTsHaveCommonSurface     = compROT.m_ROTsHaveCommonSurface;
    std::vector<const InDet::TRT_DriftCircleOnTrack*>::const_iterator rotIter = compROT.m_containedChildRots->begin();
    for (; rotIter!=compROT.m_containedChildRots->end(); ++rotIter)
      m_containedChildRots->push_back((*rotIter)->clone());
  }
  return (*this);
}

InDet::CompetingTRT_DriftCirclesOnTrack::~CompetingTRT_DriftCirclesOnTrack() {
  // delete surface if not owned by detElement
  if (m_associatedSurface && !m_associatedSurface->associatedDetectorElement())
    delete m_associatedSurface;
  delete m_globalPosition;
  clearChildRotVector();
  delete m_containedChildRots;
  //    delete m_assumedTrackParameters;
}

void InDet::CompetingTRT_DriftCirclesOnTrack::clearChildRotVector() {
  std::vector<const InDet::TRT_DriftCircleOnTrack*>::const_iterator rotIter = m_containedChildRots->begin();
  for (; rotIter!=m_containedChildRots->end(); ++rotIter)
    delete (*rotIter);
}

MsgStream& InDet::CompetingTRT_DriftCirclesOnTrack::dump( MsgStream& out ) const {
  using std::ios;
  out << "Trk::CompetingTRT_DriftCirclesOnTrack with [" << numberOfContainedROTs() 
      << "] competing RIO_OnTrack objects" << std::endl;
  out << "  - " << (this->ROTsHaveCommonSurface(true)?"on common surface":
                    "over different surfaces") << "  (given prob>cut)" << std::endl;
  Trk::CompetingRIOsOnTrack::dump(out);
  out << "  - GlobalPosition        : ";
  if (m_globalPosition==NULL) out << "null pointer"<<endreq;
  else out << *m_globalPosition<<endreq;
  return out;
}

std::ostream& InDet::CompetingTRT_DriftCirclesOnTrack::dump( std::ostream& out ) const {
  using std::ios;
  out << "Trk::CompetingTRT_DriftCirclesOnTrack with [" << numberOfContainedROTs() 
      << "] competing RIO_OnTrack objects" << std::endl;
  out << "  - " << (this->ROTsHaveCommonSurface(true)?"on common surface":
                    "over different surfaces") << "  (given prob>cut)" << std::endl;
  Trk::CompetingRIOsOnTrack::dump(out);
  out << "  - GlobalPosition        : ";
  if (m_globalPosition==NULL) out << "null pointer"<<std::endl;
  else out << *m_globalPosition<<std::endl;
  return out;
}

// Have all the contained ROTs a common associated surface?
bool InDet::CompetingTRT_DriftCirclesOnTrack::ROTsHaveCommonSurface(const bool withNonVanishingAssignProb) const {
  //if (m_ROTsHaveCommonSurface==0) {
  // values of int m_ROTsHaveCommonSurface:
  //  no  yes  NA    (ROTs on a common surface)  NA means not checked yet
  //  0    1   2
  //     for all    | just ROTs with non-vanishing assgnProb
  //      * 1       |   * 3
  if (withNonVanishingAssignProb) {
    //if (!(m_ROTsHaveCommonSurface / 3)==2){
    return (m_ROTsHaveCommonSurface / 3);
    //}else{
    //have to calc: should not happen here!!!
    //}
  } else {
    //if (!(m_ROTsHaveCommonSurface % 3)==2){
    return (m_ROTsHaveCommonSurface % 3);
    //}else{
    //have to calc: should not happen here!!!
    //}
  }

}



const Amg::Vector3D& InDet::CompetingTRT_DriftCirclesOnTrack::globalPosition() const {
  if (m_globalPosition) return (*m_globalPosition);
  // cannot use the localToGlobal transformation, because the local z-coordinate along
  // the wire is not known here. The contained TRT_DriftCircleOnTrack use the full
  // transformation => use the weighted mean of their GlobalPositions
    
  // FIXME: introduce a special function in base class, which returns the sum of assignment probabilities
  double assgnProbSum = 0.;
  std::vector<AssignmentProb>::const_iterator assgnProbIter = m_assignProb->begin();
  for (; assgnProbIter != m_assignProb->end(); ++assgnProbIter) {
    assgnProbSum += (*assgnProbIter);
  }

  Amg::Vector3D globalPos(0.,0.,0.);
  if (assgnProbSum > 0.) {
    std::vector< const InDet::TRT_DriftCircleOnTrack* >::const_iterator rotIter = m_containedChildRots->begin();
    assgnProbIter = m_assignProb->begin();
    for (; rotIter != m_containedChildRots->end(); ++rotIter, ++assgnProbIter) {
      globalPos += ( ((*assgnProbIter)/assgnProbSum) * ((*rotIter)->globalPosition()) );
    }
    //     for (unsigned int index = 0; index < m_rotsnum; index++) {
    //         globalPos += (m_assignProb->operator[](index)) * (m_containedChildRots->operator[](index)->globalPosition());
    //     }
  } else {
    globalPos = (*m_containedChildRots->begin())->globalPosition();
  }
    
  m_globalPosition = new Amg::Vector3D(globalPos);
  return *m_globalPosition;
}

void InDet::CompetingTRT_DriftCirclesOnTrack::setLocalParametersAndErrorMatrix() {
    std::cout << "InDet::CompetingTRT_DriftCirclesOnTrack: can not handle ROTs in different surfaces without detector specific knowledge " << std::endl;
}
