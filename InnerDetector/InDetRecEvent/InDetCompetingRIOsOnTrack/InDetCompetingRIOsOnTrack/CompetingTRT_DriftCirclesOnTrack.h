/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingTRT_DriftCirclesOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGTRTDRIFTCIRCLESONTRACK_H
#define COMPETINGTRTDRIFTCIRCLESONTRACK_H

// Trk
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "CxxUtils/CachedUniquePtr.h"
#include <atomic>
#include <iosfwd>

class MsgStream;

namespace Trk {
  class LocalParameters;
  class Surface;
}

namespace InDet {

  /** @class CompetingTRT_DriftCirclesOnTrack
    
      @brief Class for competing TRT_DriftCircles, it
      extends the Trk::CompetingRIOsOnTrack base class.
    
      This class is used by the Deterministic Annealing Filter to
      handle several TRT_DriftCircleOnTrack in one "effective detector layer"
      (might be more than one DetectorElement), which compete against
      each other in being assigned to a track.
      localParameters() and localErrorMatrix() return the mean values
      according to the weights (assignment probabilities).
    
      @author Sebastian.Fleischmann -at- cern.ch
 
  */

  class CompetingTRT_DriftCirclesOnTrack : public Trk::CompetingRIOsOnTrack {

  public:
    /** @brief InDet::CompetingTRT_DriftCirclesOnTrackTool is a friend to allow for updates
        of the assignment probabilities */
    friend class CompetingTRT_DriftCirclesOnTrackTool;

    //! Default Constructor for POOL
    CompetingTRT_DriftCirclesOnTrack();
    //! Copy Constructor
    CompetingTRT_DriftCirclesOnTrack(const CompetingTRT_DriftCirclesOnTrack& compROT);
    //! Assignment operator
    CompetingTRT_DriftCirclesOnTrack& operator=(const CompetingTRT_DriftCirclesOnTrack& compROT);
    /** Default move assignment operator */
    CompetingTRT_DriftCirclesOnTrack& operator=(CompetingTRT_DriftCirclesOnTrack&& compROT) = default;
    

    /** @brief Constructor with all parameters: PLEASE do not use directly,
	but call InDet::CompetingTRT_DriftCirclesOnTrackTool, otherwise inconsistency of
	the data will be very probable. */
    CompetingTRT_DriftCirclesOnTrack(
				     const Trk::Surface* sf,
				     std::vector<const InDet::TRT_DriftCircleOnTrack*>* childrots,
				     std::vector<AssignmentProb>* assgnProb,
				     //        const Trk::TrackParameters*  assumedTrkPars,
				     const Trk::LocalParameters* effecLocalPars,
				     const Amg::MatrixX* effecLocalErrMat,
				     int ROTsHaveComSrfc // meaning of the values are described in the definition of ROTsHaveCommonSurface()
				     );

    //! Destructor
    virtual ~CompetingTRT_DriftCirclesOnTrack();

    //! virtual constructor - needed to avoid excessive RTTI
    CompetingTRT_DriftCirclesOnTrack* clone() const;

    /** @brief returns the surface for the local to global transformation .
        - interface from MeasurementBase */
    const Trk::Surface& associatedSurface() const;

    /** @brief Interface method to get the global Position.
	- interface from MeasurementBase */
    const Amg::Vector3D& globalPosition() const;
    
    //! Number of RIO_OnTracks to be contained by this CompetingRIOsOnTrack. 
    unsigned int numberOfContainedROTs() const;

    /** @brief returns the vector of TRT_DriftCircleOnTrack objects .
	- specific for this CompetingTRT_DriftCirclesOnTrack: TRT_DriftCircleOnTrack */
    const std::vector<const InDet::TRT_DriftCircleOnTrack*>& containedROTs() const;

    //! returns the RIO_OnTrack (also known as ROT) objects depending on the integer
    const InDet::TRT_DriftCircleOnTrack& rioOnTrack(unsigned int) const;

    //! returns some information about this MeasurementBase/CompetingTRT_DriftCirclesOnTrack.
    MsgStream&    dump( MsgStream& out ) const;
    //! returns some information about this MeasurementBase/CompetingTRT_DriftCirclesOnTrack.
    std::ostream& dump( std::ostream& out ) const;

    virtual void setLocalParametersAndErrorMatrix();
  private:


    //! private method to clear the Trk::RIO_OnTrack vector
    void                               clearChildRotVector();
    //! The surface to which the competingROTs parameters are expressed to
    const Trk::Surface* m_associatedSurface;

    //! The global Position
    CxxUtils::CachedUniquePtr<const Amg::Vector3D> m_globalPosition;

    //! The vector of contained InDet::TRT_DriftCircleOnTrack objects
    std::vector<const InDet::TRT_DriftCircleOnTrack*>*   m_containedChildRots;



    /** Have all the contained ROTs a common associated surface?
	If withNonVanishingAssignProb==true just the ROTs with non-vanishing assignment probabilities
	are checked.
	- interface  from CompetingRIOsOnTrack */
    bool ROTsHaveCommonSurface(const bool withNonVanishingAssignProb=true) const;
    mutable std::atomic_int m_ROTsHaveCommonSurface;


  };


  inline CompetingTRT_DriftCirclesOnTrack* CompetingTRT_DriftCirclesOnTrack::clone() const {
    return new InDet::CompetingTRT_DriftCirclesOnTrack(*this);
  }
  
  inline const Trk::Surface& CompetingTRT_DriftCirclesOnTrack::associatedSurface() const {
    return (*m_associatedSurface);
  }
  
  
  inline const std::vector<const InDet::TRT_DriftCircleOnTrack*>& CompetingTRT_DriftCirclesOnTrack::containedROTs() const {
    return (*m_containedChildRots);
  }
  
  inline const InDet::TRT_DriftCircleOnTrack& CompetingTRT_DriftCirclesOnTrack::rioOnTrack(unsigned int indx) const {
      return * m_containedChildRots->operator[](indx);
  }
  
  inline unsigned int CompetingTRT_DriftCirclesOnTrack::numberOfContainedROTs() const {
    return m_containedChildRots->size();
  }
}


#endif // COMPETINGTRTDRIFTCIRCLESONTRACK_H

