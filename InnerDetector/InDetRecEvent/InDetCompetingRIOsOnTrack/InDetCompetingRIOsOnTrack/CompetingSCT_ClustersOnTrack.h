/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingSCT_ClustersOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGSCTCLUSTERSONTRACK_H
#define COMPETINGSCTCLUSTERSONTRACK_H

// Trk
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
// #include "TrkParameters/TrackParameters.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h" // cannot fwd-declare because of covariant method
#include <ostream>

//#include <iostream.h>
class MsgStream;

namespace Trk {class Surface;}

namespace InDet {

/** @class CompetingSCT_ClustersOnTrack
    
    Class for competing SCT_Clusters, it
    extends the Trk::CompetingRIOsOnTrack base class.
    
    This class is used by the Deterministic Annealing Filter to
    handle several SCT_ClusterOnTrack in one detector element, which compete against
    each other in being assigned to a track. In contrast to the InDet::CompetingTRT_DriftCirclesOnTrack
    all competing measurements of the InDet::CompetingSCT_ClustersOnTrack have to be on the same
    detector element (i.e. have a common associated surface).
    localParameters() and localErrorMatrix() return the mean values
    according to the weights (assignment probabilities).
    
    @author Sebastian.Fleischmann@cern.ch
 
    */

class CompetingSCT_ClustersOnTrack : public Trk::CompetingRIOsOnTrack {

public:
    /** InDet::CompetingSCT_ClustersOnTrackTool is a friend to allow for updates of the
    assignment probabilities */
    friend class CompetingSCT_ClustersOnTrackTool;

    /** Default Constructor for POOL */
    CompetingSCT_ClustersOnTrack();
    /** Copy Constructor */
    CompetingSCT_ClustersOnTrack(const CompetingSCT_ClustersOnTrack& compROT);
    /** Assignment operator */
    CompetingSCT_ClustersOnTrack& operator=(const CompetingSCT_ClustersOnTrack& compROT);

    /** Constructor with all parameters: PLEASE do not use directly,
    but call InDet::CompetingSCT_ClustersOnTrackTool, otherwise inconsistency of
    the data will be very probable. */
    CompetingSCT_ClustersOnTrack(
        //const Trk::Surface* sf,
        std::vector<const InDet::SCT_ClusterOnTrack*>* childrots,
        std::vector<AssignmentProb>* assgnProb
        //const Trk::TrackParameters*  assumedTrkPars,
        //const Trk::LocalParameters* effecLocalPars,
        //const Trk::ErrorMatrix* effecLocalErrMat,
        //int ROTsHaveComSrfc // meaning of the values are described in the definition of ROTsHaveCommonSurface()
    );

    /** Destructor */
    virtual ~CompetingSCT_ClustersOnTrack();

    /** needed to avoid excessive RTTI*/
    CompetingSCT_ClustersOnTrack* clone() const;

    /** returns the surface for the local to global transformation .
        - interface from MeasurementBase */
    const Trk::Surface& associatedSurface() const;

    /**Interface method to get the global Position.
       - interface from MeasurementBase */
    const Amg::Vector3D& globalPosition() const;
    
    /** Number of RIO_OnTracks to be contained by this CompetingRIOsOnTrack. */
    unsigned int numberOfContainedROTs() const;

    /** returns the vector of SCT_ClusterOnTrack objects .
      - specific for this CompetingSCT_ClustersOnTrack: SCT_ClusterOnTrack */
    const std::vector<const InDet::SCT_ClusterOnTrack*>& containedROTs() const;

    /** returns the RIO_OnTrack (also known as ROT) objects depending on the integer*/
    const InDet::SCT_ClusterOnTrack& rioOnTrack(unsigned int) const;

    /**returns some information about this MeasurementBase/CompetingSCT_ClustersOnTrack. */
    MsgStream&    dump( MsgStream& out ) const;
    /**returns some information about this MeasurementBase/CompetingSCT_ClustersOnTrack. */
    std::ostream& dump( std::ostream& out ) const;

    /** @brief recalculate the LocalParameters and ErrorMatrix
     - CompetingSCT_ClustersOnTrack overwrites implementation in CompetingRIOsOnTrack, because SCT end cap
       needs special attention in calculating the effective parameters */
    virtual void setLocalParametersAndErrorMatrix();
//    /** returns the effective LocalParameters @f$ \hat{m} @f$ according to the weights @f$ p_i @f$ (assignment probabilities).
//     - interface from MeasurementBase
//     - CompetingSCT_ClustersOnTrack overwrites implementation in CompetingRIOsOnTrack, because TRT
//        needs special attention in calculating the effective parameters */
//    const Trk::LocalParameters& localParameters() const;

//    /** returns the effective local ErrorMatrix (@f$ \hat{G} @f$ = effective WeightMatrix) according to the weights @f$ p_i @f$ (assignment probabilities) .
//    - interface from MeasurementBase
//    - CompetingSCT_ClustersOnTrack overwrites implementation in CompetingRIOsOnTrack, because TRT
//       needs special attention in calculating the effective ErrorMatrix */
//    const Trk::ErrorMatrix& localErrorMatrix() const;

    //    /** returns the TrackParameters used for calculation of the effective measurement.
    //    - specific for CompetingSCT_ClustersOnTrack */
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

    /** The vector of contained InDet::SCT_ClusterOnTrack objects */
    std::vector<const InDet::SCT_ClusterOnTrack*>*   m_containedChildRots;

    /** Have all the contained ROTs a common associated surface?
      If withNonVanishingAssignProb==true just the ROTs with non-vanishing assignment probabilities
      are checked.
      - interface  from CompetingRIOsOnTrack
      - CompetingSCT_ClustersOnTrack assume that all PixelClusters belong to the same detector element,
        so this function will always return true */
    bool ROTsHaveCommonSurface(const bool withNonVanishingAssignProb=true) const;
};

inline CompetingSCT_ClustersOnTrack* CompetingSCT_ClustersOnTrack::clone() const {
    //     cout << "in clone()" << endl;
    return new InDet::CompetingSCT_ClustersOnTrack(*this);
}

inline const Trk::Surface& CompetingSCT_ClustersOnTrack::associatedSurface() const {
    //if (m_containedChildRots)
    return ((*(m_containedChildRots->begin()))->associatedSurface());
}


inline const std::vector<const InDet::SCT_ClusterOnTrack*>& CompetingSCT_ClustersOnTrack::containedROTs() const {
    return (*m_containedChildRots);
}

inline const InDet::SCT_ClusterOnTrack& CompetingSCT_ClustersOnTrack::rioOnTrack(unsigned int indx) const {
  //if (m_containedChildRots && indx<m_containedChildRots->size())
        return * m_containedChildRots->operator[](indx);
	//return 0;
}

 inline const Amg::Vector3D& CompetingSCT_ClustersOnTrack::globalPosition() const {
    if (m_globalPosition)
        return (*m_globalPosition);
    m_globalPosition = associatedSurface().localToGlobal(localParameters());
    return (*m_globalPosition);
}

inline unsigned int CompetingSCT_ClustersOnTrack::numberOfContainedROTs() const {
    return m_containedChildRots->size();
}

}

#endif // COMPETINGSCTCLUSTERSONTRACK_H

