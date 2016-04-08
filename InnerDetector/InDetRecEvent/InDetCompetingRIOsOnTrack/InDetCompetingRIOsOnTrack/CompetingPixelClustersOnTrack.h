/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingPixelClustersOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGPIXELCLUSTERSONTRACK_H
#define COMPETINGPIXELCLUSTERSONTRACK_H

// Trk
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h" // cannot forward declare
#include <ostream>

//#include <iostream.h>
class MsgStream;

namespace Trk {
  class PrepRawData;
  class LocalParameters;
  class Surface;
}

namespace InDet {

/** @class CompetingPixelClustersOnTrack
    Class for competing PixelClusters, it
    extends the Trk::CompetingRIOsOnTrack base class.
    
    This class is used by the Deterministic Annealing Filter to
    handle several PixelClusterOnTrack in one detector element, which compete against
    each other in being assigned to a track. In contrast to the InDet::CompetingTRT_DriftCirclesOnTrack
    all competing measurements of the InDet::CompetingPixelClustersOnTrack have to be on the same
    detector element (i.e. have a common associated surface).
    localParameters() and localErrorMatrix() return the mean values
    according to the weights (assignment probabilities).
    
    @author Sebastian.Fleischmann@cern.ch
 
    */

class CompetingPixelClustersOnTrack : public Trk::CompetingRIOsOnTrack {

public:
    /** InDet::CompetingPixelClustersOnTrackTool is a friend to allow for updates of the
    assignment probabilities */
    friend class CompetingPixelClustersOnTrackTool;

    /** Default Constructor for POOL */
    CompetingPixelClustersOnTrack();
    /** Copy Constructor */
    CompetingPixelClustersOnTrack(const CompetingPixelClustersOnTrack& compROT);
    /** Assignment operator */
    CompetingPixelClustersOnTrack& operator=(const CompetingPixelClustersOnTrack& compROT);
    CompetingPixelClustersOnTrack& operator=(CompetingPixelClustersOnTrack&& compROT);

    /** Constructor with all parameters: PLEASE do not use directly,
    but call InDet::CompetingPixelClustersOnTrackTool, otherwise inconsistency of
    the data will be very probable. */
    CompetingPixelClustersOnTrack(
        //const Trk::Surface* sf,
        std::vector<const InDet::PixelClusterOnTrack*>* childrots,
        std::vector<AssignmentProb>* assgnProb
        //        const Trk::TrackParameters*  assumedTrkPars,
        //const Trk::LocalParameters* effecLocalPars,
        //const Trk::ErrorMatrix* effecLocalErrMat,
        //int ROTsHaveComSrfc // meaning of the values are described in the definition of ROTsHaveCommonSurface()
    );

    /** Destructor */
    virtual ~CompetingPixelClustersOnTrack();

    /** needed to avoid excessive RTTI*/
    CompetingPixelClustersOnTrack* clone() const;

    /** returns the surface for the local to global transformation .
        - interface from MeasurementBase */
    const Trk::Surface& associatedSurface() const;

    /**Interface method to get the global Position.
       - interface from MeasurementBase */
    const Amg::Vector3D& globalPosition() const;
    
    /** Number of RIO_OnTracks to be contained by this CompetingRIOsOnTrack. */
    unsigned int numberOfContainedROTs() const;

    /** returns the vector of PixelClusterOnTrack objects .
      - specific for this CompetingPixelClustersOnTrack: PixelClusterOnTrack */
    const std::vector<const InDet::PixelClusterOnTrack*>& containedROTs() const;

    /** returns the RIO_OnTrack (also known as ROT) objects depending on the integer*/
    const InDet::PixelClusterOnTrack& rioOnTrack(unsigned int) const;

    /**returns some information about this MeasurementBase/CompetingPixelClustersOnTrack. */
    MsgStream&    dump( MsgStream& out ) const;
    /**returns some information about this MeasurementBase/CompetingPixelClustersOnTrack. */
    std::ostream& dump( std::ostream& out ) const;

//    /** returns the effective LocalParameters @f$ \hat{m} @f$ according to the weights @f$ p_i @f$ (assignment probabilities).
//     - interface from MeasurementBase
//     - CompetingPixelClustersOnTrack overwrites implementation in CompetingRIOsOnTrack, because TRT
//        needs special attention in calculating the effective parameters */
//    const Trk::LocalParameters& localParameters() const;

//    /** returns the effective local ErrorMatrix (@f$ \hat{G} @f$ = effective WeightMatrix) according to the weights @f$ p_i @f$ (assignment probabilities) .
//    - interface from MeasurementBase
//    - CompetingPixelClustersOnTrack overwrites implementation in CompetingRIOsOnTrack, because TRT
//       needs special attention in calculating the effective ErrorMatrix */
//    const Trk::ErrorMatrix& localErrorMatrix() const;

    //    /** returns the TrackParameters used for calculation of the effective measurement.
    //    - specific for CompetingPixelClustersOnTrack */
    //    const Trk::TrackParameters* assumedTrackParameters() const;


private:

    //    /** returns the RIO_OnTrack vector (of base class type) i.e. cast upwards to provide RIO_OnTrack
    //    for base class CompetingRIOsOnTrack*/
    //    const std::vector<const Trk::RIO_OnTrack*>* getBaseRioOnTrack() const;

    /** private method to clear the Trk::RIO_OnTrack vector */
    void                               clearChildRotVector();
//    /** The surface to which the competingROTs parameters are expressed to */
//    mutable const Trk::Surface*             m_associatedSurface;

    /** The global Position */
    mutable const Amg::Vector3D*        m_globalPosition;

    /** The vector of contained InDet::PixelClusterOnTrack objects */
    std::vector<const InDet::PixelClusterOnTrack*>*   m_containedChildRots;

    /** Have all the contained ROTs a common associated surface?
      If withNonVanishingAssignProb==true just the ROTs with non-vanishing assignment probabilities
      are checked.
      - interface  from CompetingRIOsOnTrack
      - CompetingPixelClustersOnTrack assume that all PixelClusters belong to the same detector element,
        so this function will always return true */
    bool ROTsHaveCommonSurface(const bool withNonVanishingAssignProb=true) const;
};

inline CompetingPixelClustersOnTrack* CompetingPixelClustersOnTrack::clone() const {
    //     cout << "in clone()" << endl;
    return new InDet::CompetingPixelClustersOnTrack(*this);
}

inline const Trk::Surface& CompetingPixelClustersOnTrack::associatedSurface() const {
    //if (m_containedChildRots)
    return ((*(m_containedChildRots->begin()))->associatedSurface());
}


inline const std::vector<const InDet::PixelClusterOnTrack*>& CompetingPixelClustersOnTrack::containedROTs() const {
    return (*m_containedChildRots);
}

inline const InDet::PixelClusterOnTrack& CompetingPixelClustersOnTrack::rioOnTrack(unsigned int indx) const {
  //if (m_containedChildRots && indx<m_containedChildRots->size())
        return * m_containedChildRots->operator[](indx);
	//return 0;
}

 inline const Amg::Vector3D& CompetingPixelClustersOnTrack::globalPosition() const {
    if (m_globalPosition)
        return (*m_globalPosition);
    m_globalPosition = associatedSurface().localToGlobal(localParameters());
    return (*m_globalPosition);
}

inline unsigned int CompetingPixelClustersOnTrack::numberOfContainedROTs() const {
    return m_containedChildRots->size();
}

// inline const Trk::LocalParameters& CompetingPixelClustersOnTrack::localParameters() const {
//     return (*m_localParameters);
// }
// 
// inline const Trk::ErrorMatrix& CompetingPixelClustersOnTrack::localErrorMatrix() const {
//     return (*m_localErrorMatrix);
// }

}

#endif // COMPETINGPIXELCLUSTERSONTRACK_H

