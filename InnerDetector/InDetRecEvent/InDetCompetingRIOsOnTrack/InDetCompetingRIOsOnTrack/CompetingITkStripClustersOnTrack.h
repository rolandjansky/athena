/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingITkStripClustersOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGITKSTRIPCLUSTERSONTRACK_H
#define COMPETINGITKSTRIPCLUSTERSONTRACK_H

// Trk
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
// #include "TrkParameters/TrackParameters.h"
#include "InDetRIO_OnTrack/ITk_Strip_ClusterOnTrack.h" // cannot fwd-declare because of covariant method
#include <ostream>

//#include <iostream.h>
class MsgStream;

namespace Trk {class Surface;}

namespace InDet {

/** @class CompetingITkStripClustersOnTrack
    
    Class for competing ITk_Strip_Clusters, it
    extends the Trk::CompetingRIOsOnTrack base class.
    
    This class is used by the Deterministic Annealing Filter to
    handle several ITk_Strip_ClusterOnTrack in one detector element, which compete against
    each other in being assigned to a track. In contrast to the InDet::CompetingTRT_DriftCirclesOnTrack
    all competing measurements of the InDet::CompetingITkStripClustersOnTrack have to be on the same
    detector element (i.e. have a common associated surface).
    localParameters() and localErrorMatrix() return the mean values
    according to the weights (assignment probabilities).
    
    @author Sebastian.Fleischmann@cern.ch
 
    */

class CompetingITkStripClustersOnTrack : public Trk::CompetingRIOsOnTrack {

public:
    /** InDet::CompetingITkStripClustersOnTrackTool is a friend to allow for updates of the
    assignment probabilities */
    friend class CompetingITkStripClustersOnTrackTool;

    /** Default Constructor for POOL */
    CompetingITkStripClustersOnTrack();
    /** Copy Constructor */
    CompetingITkStripClustersOnTrack(const CompetingITkStripClustersOnTrack& compROT);
    /** Assignment operator */
    CompetingITkStripClustersOnTrack& operator=(const CompetingITkStripClustersOnTrack& compROT);

    /** Constructor with all parameters: PLEASE do not use directly,
    but call InDet::CompetingITkStripClustersOnTrackTool, otherwise inconsistency of
    the data will be very probable. */
    CompetingITkStripClustersOnTrack(
        //const Trk::Surface* sf,
        std::vector<const InDet::ITk_Strip_ClusterOnTrack*>* childrots,
        std::vector<AssignmentProb>* assgnProb
        //const Trk::TrackParameters*  assumedTrkPars,
        //const Trk::LocalParameters* effecLocalPars,
        //const Trk::ErrorMatrix* effecLocalErrMat,
        //int ROTsHaveComSrfc // meaning of the values are described in the definition of ROTsHaveCommonSurface()
    );

    /** Destructor */
    virtual ~CompetingITkStripClustersOnTrack();

    /** needed to avoid excessive RTTI*/
    CompetingITkStripClustersOnTrack* clone() const;

    /** returns the surface for the local to global transformation .
        - interface from MeasurementBase */
    const Trk::Surface& associatedSurface() const;

    /**Interface method to get the global Position.
       - interface from MeasurementBase */
    const Amg::Vector3D& globalPosition() const;
    
    /** Number of RIO_OnTracks to be contained by this CompetingRIOsOnTrack. */
    unsigned int numberOfContainedROTs() const;

    /** returns the vector of ITk_Strip_ClusterOnTrack objects .
      - specific for this CompetingITkStripClustersOnTrack: ITk_Strip_ClusterOnTrack */
    const std::vector<const InDet::ITk_Strip_ClusterOnTrack*>& containedROTs() const;

    /** returns the RIO_OnTrack (also known as ROT) objects depending on the integer*/
    const InDet::ITk_Strip_ClusterOnTrack& rioOnTrack(unsigned int) const;

    /**returns some information about this MeasurementBase/CompetingITkStripClustersOnTrack. */
    MsgStream&    dump( MsgStream& out ) const;
    /**returns some information about this MeasurementBase/CompetingITkStripClustersOnTrack. */
    std::ostream& dump( std::ostream& out ) const;

    /** @brief recalculate the LocalParameters and ErrorMatrix
     - CompetingITkStripClustersOnTrack overwrites implementation in CompetingRIOsOnTrack, because Strip end cap
       needs special attention in calculating the effective parameters */
    virtual void setLocalParametersAndErrorMatrix();
//    /** returns the effective LocalParameters @f$ \hat{m} @f$ according to the weights @f$ p_i @f$ (assignment probabilities).
//     - interface from MeasurementBase
//     - CompetingITkStripClustersOnTrack overwrites implementation in CompetingRIOsOnTrack, because TRT
//        needs special attention in calculating the effective parameters */
//    const Trk::LocalParameters& localParameters() const;

//    /** returns the effective local ErrorMatrix (@f$ \hat{G} @f$ = effective WeightMatrix) according to the weights @f$ p_i @f$ (assignment probabilities) .
//    - interface from MeasurementBase
//    - CompetingITkStripClustersOnTrack overwrites implementation in CompetingRIOsOnTrack, because TRT
//       needs special attention in calculating the effective ErrorMatrix */
//    const Trk::ErrorMatrix& localErrorMatrix() const;

    //    /** returns the TrackParameters used for calculation of the effective measurement.
    //    - specific for CompetingITkStripClustersOnTrack */
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

    /** The vector of contained InDet::ITk_Strip_ClusterOnTrack objects */
    std::vector<const InDet::ITk_Strip_ClusterOnTrack*>*   m_containedChildRots;

    /** Have all the contained ROTs a common associated surface?
      If withNonVanishingAssignProb==true just the ROTs with non-vanishing assignment probabilities
      are checked.
      - interface  from CompetingRIOsOnTrack
      - CompetingITkStripClustersOnTrack assume that all PixelClusters belong to the same detector element,
        so this function will always return true */
    bool ROTsHaveCommonSurface(const bool withNonVanishingAssignProb=true) const;
};

inline CompetingITkStripClustersOnTrack* CompetingITkStripClustersOnTrack::clone() const {
    //     cout << "in clone()" << endl;
    return new InDet::CompetingITkStripClustersOnTrack(*this);
}

inline const Trk::Surface& CompetingITkStripClustersOnTrack::associatedSurface() const {
    //if (m_containedChildRots)
    return ((*(m_containedChildRots->begin()))->associatedSurface());
}


inline const std::vector<const InDet::ITk_Strip_ClusterOnTrack*>& CompetingITkStripClustersOnTrack::containedROTs() const {
    return (*m_containedChildRots);
}

inline const InDet::ITk_Strip_ClusterOnTrack& CompetingITkStripClustersOnTrack::rioOnTrack(unsigned int indx) const {
  //if (m_containedChildRots && indx<m_containedChildRots->size())
        return * m_containedChildRots->operator[](indx);
	//return 0;
}

 inline const Amg::Vector3D& CompetingITkStripClustersOnTrack::globalPosition() const {
    if (m_globalPosition)
        return (*m_globalPosition);
    m_globalPosition = associatedSurface().localToGlobal(localParameters());
    return (*m_globalPosition);
}

inline unsigned int CompetingITkStripClustersOnTrack::numberOfContainedROTs() const {
    return m_containedChildRots->size();
}

}

#endif // COMPETINGITKSTRIPCLUSTERSONTRACK_H

