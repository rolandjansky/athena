///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomToTrackParticleAssocs.h 
// Header file for class INav4MomToTrackParticleAssocs.h
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENT_INAV4MOMTOTRACKPARTICLEASSOCS_H 
#define PARTICLEEVENT_INAV4MOMTOTRACKPARTICLEASSOCS_H 

// STL includes
 
#include "AthLinks/AssociationMap.h"
#include "AthLinks/DataLink.h"

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"

// TrackParticleIncludes
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
// Forward declaration



class INav4MomToTrackParticleAssocs : public AssociationMap< INavigable4MomentumCollection, 
                                                             Rec::TrackParticleContainer >
{ 
  
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 
  
  /** Default constructor: */ 
  INav4MomToTrackParticleAssocs();

  /** Copy constructor:    */
  INav4MomToTrackParticleAssocs( const INav4MomToTrackParticleAssocs& rhs );

  /** Assignment operator:     */
  INav4MomToTrackParticleAssocs& operator=( const INav4MomToTrackParticleAssocs& rhs ); 
  
  /** Constructor with parameters:     */
  /** Destructor: */
  ~INav4MomToTrackParticleAssocs(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  
  /** Retrieve all the associated-to objects which have been associated
    *  to the given \param object
    */
  bool assocs( const INavigable4Momentum* object,
               Rec::TrackParticleContainer& assocs ) const;
  
  /** Retrieve all the associated-to ElementLinks_to_objects which have
   *  been associated to the given \param objectLink
   */
  bool getAssociations( const INav4MomLink& objectLink,
                        std::vector< AssociationMap< INavigable4MomentumCollection,
                                                     Rec::TrackParticleContainer >::asso_link >& associatedElementLinks ) const;

  /** Retrieve all the associated-to ElementLinks_to_objects which have
   *  been associated to the given \param object
   */
  bool getAssociations( const INavigable4Momentum* object,
                        std::vector< AssociationMap< INavigable4MomentumCollection,
                                                     Rec::TrackParticleContainer >::asso_link >& associatedElementLinks ) const;
  
  /** Reimplement the method from the base class to not shaddow it */
  bool getAssociations(const AssociationMap< INavigable4MomentumCollection,
                                             Rec::TrackParticleContainer >::object_type* objPointer,                   
                       AssociationMap< INavigable4MomentumCollection,
                                       Rec::TrackParticleContainer >::asso_list& assocs) const
  {
    return AssociationMap< INavigable4MomentumCollection,
                           Rec::TrackParticleContainer >::getAssociations( objPointer, assocs );
  }
  
  /** Reimplement the method from the base class to not shaddow it */
  bool getAssociations(const AssociationMap< INavigable4MomentumCollection,
                                             Rec::TrackParticleContainer >::object_iterator& objIter,
                       AssociationMap< INavigable4MomentumCollection,
                                       Rec::TrackParticleContainer >::asso_list& assocs) const
  {
    return AssociationMap< INavigable4MomentumCollection, 
                           Rec::TrackParticleContainer >::getAssociations( objIter, assocs );
  }  
  
  void addAssocStore( const DataLink<INav4MomToTrackParticleAssocs>& assocStore );

  /// Look up an associated store.  For testing.
  DataLink<INav4MomToTrackParticleAssocs>
  assocStore (const std::string& name) const;

  /// Number of associated stores.
  size_t nAssocStores() const;

  
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
protected: 
  typedef DataLink<INav4MomToTrackParticleAssocs> INav4MomToTrackParticleAssocsLink_t;
  std::map<std::string, INav4MomToTrackParticleAssocsLink_t> m_assocStores;
}; 

 

// Destructor
///////////////
inline INav4MomToTrackParticleAssocs::~INav4MomToTrackParticleAssocs() 
{}

CLASS_DEF( INav4MomToTrackParticleAssocs , 54913024 , 1 )
 
#endif //> PARTICLEEVENT_INAV4MOMTOTRACKPARTICLEASSOCS_H
 
