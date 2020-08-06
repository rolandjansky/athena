/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompetingRpcClustersOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGRPCCLUSTERSONTRACK_H
#define COMPETINGRPCCLUSTERSONTRACK_H

// Trk
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
// Identifier
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
// Muon
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include "CxxUtils/CachedUniquePtr.h"

#include <ostream>

class MsgStream;

namespace Trk {
  class PrepRawData;
  class TrkDetElementBase;
}

namespace Muon {


  /** @class CompetingMuonClustersOnTrack
    
  Class for competing MuonClusters, it
  extends the Trk::CompetingRIOsOnTrack base class.
    
  This class is used for two use cases:
  - by the Deterministic Annealing Filter to handle several MuonClusterOnTrack in one detector element, which compete against
    each other in being assigned to a track. 
  - to handle reclustering on ROT level

  @author Niels van Eldik
 
  */

  class CompetingMuonClustersOnTrack : public Trk::CompetingRIOsOnTrack {

  public:
    /** Muon::CompetingMuonClustersOnTrackTool is a friend to allow for updates of the assignment probabilities */
    friend class CompetingMuonClustersOnTrackTool;

    /** Default Constructor for POOL */
    CompetingMuonClustersOnTrack();
    /** Copy Constructor */
    CompetingMuonClustersOnTrack(const CompetingMuonClustersOnTrack& compROT);
    /** Assignment operator */
    CompetingMuonClustersOnTrack& operator=(const CompetingMuonClustersOnTrack& compROT);
    CompetingMuonClustersOnTrack& operator=(CompetingMuonClustersOnTrack&& compROT);

    /** Constructor with all parameters: PLEASE do not use directly,
    but call Muon::CompetingMuonClustersOnTrackTool, otherwise inconsistency of the data will be very probable. 
    - The constructor takes following parameters:
    @param childrots  a vector of MuonClusterOnTrack objects that form the CompetingMuonClustersOnTrack
    @param assgnProb  a vector with the assignment probabilities for each of the child ROTs
    */
    CompetingMuonClustersOnTrack( std::vector<const MuonClusterOnTrack*>* childrots,
                  std::vector<AssignmentProb>* assgnProb );

    /** constructor taking the local parameters + error matrix + associated surface directly, 
    instead of using the base-class to recalculate them 
    @param locPars   the Trk::LocalParameters of the new CompetingMuonClustersOnTrack
    @param error     the Amg::MatrixX of the new CompetingMuonClustersOnTrack
    @param assSurf   the surface at which the local parameters and erro are expressed
    @param childrots  a vector of MuonClusterOnTrack objects that form the CompetingMuonClustersOnTrack
    @param assgnProb  a vector with the assignment probabilities for each of the child ROTs
    */
    CompetingMuonClustersOnTrack( const Trk::LocalParameters* locPars,
                  const Amg::MatrixX*     error,
                  const Trk::Surface*         assSurf,
                  std::vector<const MuonClusterOnTrack*>* childrots,
                  std::vector<AssignmentProb>* assgnProb );

    /** constructor taking the local parameters + error matrix + associated surface directly, 
    instead of using the base-class to recalculate them 
    @param locPars   the Trk::LocalParameters of the new CompetingMuonClustersOnTrack
    @param error     the Amg::MatrixX of the new CompetingMuonClustersOnTrack
    @param assSurf   the surface at which the local parameters and erro are expressed
    @param childrots  a vector of MuonClusterOnTrack objects that form the CompetingMuonClustersOnTrack
    @param assgnProb  a vector with the assignment probabilities for each of the child ROTs
    */
    CompetingMuonClustersOnTrack( const Trk::LocalParameters& locPars,
                  const Amg::MatrixX&     error,
                  const Trk::Surface*         assSurf,
                  std::vector<const MuonClusterOnTrack*>* childrots,
                  std::vector<AssignmentProb>* assgnProb );

    /** Destructor */
    virtual ~CompetingMuonClustersOnTrack();

    /** needed to avoid excessive RTTI*/
    CompetingMuonClustersOnTrack* clone() const;

    /** returns the surface for the local to global transformation .
        - interface from MeasurementBase */
    const Trk::Surface& associatedSurface() const;
    const Trk::Surface* associatedSurfaceRaw() const;

    /**Interface method to get the global Position.
       - interface from MeasurementBase */
    const Amg::Vector3D& globalPosition() const;

    /** Number of RIO_OnTracks to be contained by this CompetingRIOsOnTrack. */
    unsigned int numberOfContainedROTs() const;

    /** returns the vector of SCT_ClusterOnTrack objects .
    - specific for this CompetingMuonClustersOnTrack: MuonClusterOnTrack */
    const std::vector<const MuonClusterOnTrack*>& containedROTs() const;

    /** returns the RIO_OnTrack (also known as ROT) objects depending on the integer*/
    const MuonClusterOnTrack& rioOnTrack(unsigned int) const;

    /**returns some information about this MeasurementBase/CompetingMuonClustersOnTrack. */
    MsgStream&              dump( MsgStream& out ) const;
    /**returns some information about this MeasurementBase/CompetingMuonClustersOnTrack. */
    std::ostream&           dump( std::ostream& out ) const;

  private:

    /** private method to clear the Trk::RIO_OnTrack vector */
    void                    clearChildRotVector();

    /** The global Position */
    CxxUtils::CachedUniquePtr<const Amg::Vector3D> m_globalPosition;

    /** The vector of contained Muon::MuonClusterOnTrack objects */
    std::vector<const MuonClusterOnTrack*>*   m_containedChildRots;

    /** Pointer the the associated surface, if zero the surface of the first MuonClusterOnTrack will be used */
    const Trk::Surface*                       m_associatedSurface;

    /** Have all the contained ROTs a common associated surface?
    If withNonVanishingAssignProb==true just the ROTs with non-vanishing assignment probabilities
    are checked.
    - interface  from CompetingRIOsOnTrack
    */
    bool ROTsHaveCommonSurface(const bool withNonVanishingAssignProb=true) const;
  };

  inline CompetingMuonClustersOnTrack* CompetingMuonClustersOnTrack::clone() const {
    return new CompetingMuonClustersOnTrack(*this);
  }

  inline const Trk::Surface& CompetingMuonClustersOnTrack::associatedSurface() const {
    if( m_associatedSurface ) return *m_associatedSurface;
    return ((*(m_containedChildRots->begin()))->associatedSurface());
  }
  
  inline const Trk::Surface* CompetingMuonClustersOnTrack::associatedSurfaceRaw() const
  {
    return m_associatedSurface;
  }
  
  inline unsigned int CompetingMuonClustersOnTrack::numberOfContainedROTs() const {
      return m_containedChildRots->size();
  }
  
  inline const std::vector<const MuonClusterOnTrack*>& CompetingMuonClustersOnTrack::containedROTs() const {
    return (*m_containedChildRots);
  }

  inline const MuonClusterOnTrack& CompetingMuonClustersOnTrack::rioOnTrack(unsigned int indx) const {
    return *m_containedChildRots->operator[](indx);
  }

  inline const Amg::Vector3D& CompetingMuonClustersOnTrack::globalPosition() const {
    if (not m_globalPosition) m_globalPosition.set(std::unique_ptr<const Amg::Vector3D>(associatedSurface().localToGlobal(localParameters())));
    return (*m_globalPosition);
  }
}

#endif // COMPETINGRPCCLUSTERSONTRACK_H

