///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackParticleAssocs.h 
// Header file for class TrackParticleAssocs.h
// Author: C.Anastopoulos
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENT_TRACKPARTICLEASSOCS_H 
#define PARTICLEEVENT_TRACKPARTICLEASSOCS_H 

// STL includes
 
#include "AthLinks/AssociationMap.h"
#include "AthLinks/DataLink.h"
  
// TrackParticleIncludes
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
// Forward declaration

class TrackParticleAssocs : public AssociationMap<  Rec::TrackParticleContainer, 
						    Rec::TrackParticleContainer >

{ 
  
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 
  
  /** Default constructor:*/ 

  TrackParticleAssocs();

  /** Copy constructor:    */
  TrackParticleAssocs( const TrackParticleAssocs& rhs );

  /** Assignment operator:     */
  TrackParticleAssocs& operator=( const TrackParticleAssocs& rhs ); 
  
  /** Constructor with parameters:     */
  /** Destructor: */
  ~TrackParticleAssocs(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  
  /** Retrieve all the associated-to objects which have been associated
    *  to the given \param object
    */
  bool assocs( const Rec::TrackParticle* object,
	       Rec::TrackParticleContainer& assocs ) const;
 
  /** Reimplement the method from the base class to not shaddow it */
  bool getAssociations(const AssociationMap< Rec::TrackParticleContainer,
		       Rec::TrackParticleContainer >::object_type* objPointer,                   
		       AssociationMap< Rec::TrackParticleContainer,
		       Rec::TrackParticleContainer >::asso_list& assocs) const
  {
    return AssociationMap< Rec::TrackParticleContainer, 
      Rec::TrackParticleContainer >::getAssociations( objPointer, assocs );
  }
  
  /** Reimplement the method from the base class to not shaddow it */
  bool getAssociations(const AssociationMap< Rec::TrackParticleContainer,
		       Rec::TrackParticleContainer >::object_iterator& objIter,
		       AssociationMap< Rec::TrackParticleContainer,
		       Rec::TrackParticleContainer >::asso_list& assocs) const
  {
    return AssociationMap< Rec::TrackParticleContainer, 
      Rec::TrackParticleContainer >::getAssociations( objIter, assocs );
  }  

  void addAssocStore( const DataLink<TrackParticleAssocs>& assocStore );
  

  /// Look up an associated store.  For testing.
  DataLink<TrackParticleAssocs>
  assocStore (const std::string& name) const;

  /// Number of associated stores.
  size_t nAssocStores() const;

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
protected: 
  typedef DataLink<TrackParticleAssocs> TrackParticleAssocsLink_t;
  std::map<std::string, TrackParticleAssocsLink_t> m_assocStores;
}; 

 

// Destructor
///////////////
inline TrackParticleAssocs::~TrackParticleAssocs() 
{}

CLASS_DEF(TrackParticleAssocs, 94197921, 1 )
 
 #endif //> PARTICLEEVENT_TRACKPARTICLEASSOCS_H
 
