/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingRIOsOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGRIOSONTRACK_H
#define COMPETINGRIOSONTRACK_H

// Amg 
#include "EventPrimitives/EventPrimitives.h" 
#include "GeoPrimitives/GeoPrimitives.h" 

// Trk
#include "TrkEventPrimitives/LocalParameters.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include <atomic>
#include <ostream>
#include <cassert>

class MsgStream;
class CompetingRIOsOnTrackCnv_p1;

namespace Trk {

class PrepRawData;
class RIO_OnTrack;
class TrkDetElementBase;

/** @class CompetingRIOsOnTrack
    @brief Base class for all CompetingRIOsOnTack implementations,
    extends the common MeasurementBase.
    
    This class is used e.g. by the Deterministic Annealing Filter to
    handle several RIO_OnTracks in one "effective detector layer"
    (might be more than one DetectorElement), which compete against
    each other in being assigned to a track.
    localParameters() and localErrorMatrix() should return the mean values
    according to the weights (assignment probabilities).
    
    effective LocalParameters, an ErrorMatrix and a number of competing 
    ROTs with their assignment probabilities are the commonalities of all
    CompetingRIOsOnTrack, the derived classes
    can overwrite the base class definitions of Trk::RIO_OnTrack
    and Trk::Surface with derived classes for internal use and to avoid extensive RTTI.
 
    The Surface is chosen not to be a private member of
    the base class, such that dedicated Segments can save specific Surface types and overwrite
    the return type by a child class to avoid extensive RTTI.
 
    Access to any information of the contained ROTs, such as the underlying Trk::PrepRawData,
    the pointer to the Trk::DetectorElementBase such as identifiers have to be retrieved from
    the ROT itself.   
    
    @author Sebastian.Fleischmann@cern.ch
    */

class CompetingRIOsOnTrack : public MeasurementBase {

public:


  //! Type def of Assignment probability: probability for a certain RIO_OnTrack to be assigned to the track
    typedef double AssignmentProb;

    //! Default Constructor for POOL
    CompetingRIOsOnTrack();
    //! Copy Constructor
    CompetingRIOsOnTrack(const CompetingRIOsOnTrack& compROT);
    //! Assignment operator
    CompetingRIOsOnTrack& operator=(const CompetingRIOsOnTrack& compROT);
    CompetingRIOsOnTrack& operator=( CompetingRIOsOnTrack&& compROT);
    //! Constructor with parameters 
    CompetingRIOsOnTrack( std::vector<AssignmentProb>* assgnProb);

    //! Destructor
    virtual ~CompetingRIOsOnTrack();

    //! Pseudo-constructor:  needed to avoid excessive RTTI
    virtual CompetingRIOsOnTrack* clone() const = 0;

    /** @brief Number of RIO_OnTracks to be contained by this CompetingRIOsOnTrack.
     - extends MeasurementBase */
    virtual unsigned int numberOfContainedROTs() const =0;

    /** @brief Index of the ROT with the highest assignment probability.

    - extends MeasurementBase */
    unsigned int indexOfMaxAssignProb() const;

    /** @brief  returns the RIO_OnTrack (also known as ROT) objects depending on the integer.

    - extends MeasurementBase */
    virtual const RIO_OnTrack& rioOnTrack(unsigned int) const = 0;


    /** @brief returns the AssignmentProbability depending on the integer.

    - extends MeasurementBase */
    AssignmentProb assignmentProbability(unsigned int indx) const;

    /** @brief recalculate the LocalParameters and ErrorMatrix */
    virtual void setLocalParametersAndErrorMatrix();

protected:
    friend class ::CompetingRIOsOnTrackCnv_p1;

    //! index of hightest assignment probability
    mutable std::atomic_uint m_indexMaxAssignProb;

    //! assignment probabilities of the ROTs
    const std::vector<AssignmentProb>* m_assignProb;

    //! used to flag that the m_indexMaxAssignProb hasn't been calculated yet
    bool                            m_maxProbCalculated;

    /** @brief query if all the contained ROTs have a common associated surface.

        If withNonVanishingAssignProb==true just the ROTs with non-vanishing
        assignment probabilities are checked.*/
    virtual bool ROTsHaveCommonSurface(const bool withNonVanishingAssignProb=true) const = 0;

    //! returns the some information about the base class members (avoid code duplication)
    virtual MsgStream&    dump( MsgStream& out ) const;  
    //! returns the some information about the base class members (avoid code duplication)
    virtual std::ostream& dump( std::ostream& out ) const;

};

inline CompetingRIOsOnTrack::AssignmentProb CompetingRIOsOnTrack::assignmentProbability(unsigned int indx) const {
    assert ( indx < numberOfContainedROTs() );
    if (indx < numberOfContainedROTs() )
        return m_assignProb->operator[](indx);
    return 0; // could consider throwing an exception here - EJWM
}

}

#endif // COMPETINGRIOSONTRACK_H

