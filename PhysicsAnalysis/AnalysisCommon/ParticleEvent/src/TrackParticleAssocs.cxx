/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TrackParticleAssocs.cxx 
// Implementation file for class TrackParticleAssocs
// Author: C.Anastopoulos
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <algorithm>

#include "ParticleEvent/TrackParticleAssocs.h"
#include "EventKernel/IParticle.h"
#include "AthContainers/DataVector.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
TrackParticleAssocs::TrackParticleAssocs() :
  AssociationMap<Rec::TrackParticleContainer, Rec::TrackParticleContainer>(), 
  m_assocStores()
{}

TrackParticleAssocs::TrackParticleAssocs( const TrackParticleAssocs& rhs) :
  AssociationMap<Rec::TrackParticleContainer,Rec::TrackParticleContainer>(rhs),
  m_assocStores( rhs.m_assocStores )
{}
 
// Assignment operator: 
TrackParticleAssocs& TrackParticleAssocs::operator=(const TrackParticleAssocs& rhs)
{
  if ( this != &rhs ) {
    AssociationMap< Rec::TrackParticleContainer,Rec::TrackParticleContainer >::operator=(rhs);
    m_assocStores = rhs.m_assocStores;
  }
  return *this;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
 
 bool  TrackParticleAssocs::assocs( const Rec::TrackParticle* object,
				    Rec::TrackParticleContainer& associatedElems ) const
{
  if ( 0 == object ) {
    return false;
  }

  // do the underlying stores contain this object ?
  bool contained = false;
  for ( std::map<std::string, DataLink<TrackParticleAssocs> >::const_iterator itr = m_assocStores.begin();
	itr != m_assocStores.end();
	++itr ) {
    if ( itr->second->assocs(object, associatedElems) ) {
      contained = true;
    }
  }
  
  if ( !contained && !containsObject( object ) ) {
    return false;
  }
  
  associatedElems.reserve( associatedElems.size() + getNumberOfAssociations(object) );
  TrackParticleAssocs::asso_iterator endAssocs = endAssociation( object );
  for ( TrackParticleAssocs::asso_iterator itr = beginAssociation( object );
	itr != endAssocs;
	++itr ) {
    associatedElems.push_back( const_cast<Rec::TrackParticle*>(*itr) );
  }
  
  return true;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
 /////////////////////////////////////////////////////////////////// 

void TrackParticleAssocs::addAssocStore( const DataLink<TrackParticleAssocs>& assocStore )
  {
     const std::string& id = assocStore.dataID();
     const std::map<std::string, DataLink<TrackParticleAssocs> >::const_iterator itr = m_assocStores.find(id);
     if ( itr == m_assocStores.end() ) {
       m_assocStores[id] = assocStore;
     }
  }


DataLink<TrackParticleAssocs>
TrackParticleAssocs::assocStore (const std::string& name) const
{
  std::map<std::string, TrackParticleAssocsLink_t>::const_iterator i =
    m_assocStores.find (name);
  if (i != m_assocStores.end())
    return i->second;
  return DataLink<TrackParticleAssocs>();
}


size_t TrackParticleAssocs::nAssocStores() const
{
  return m_assocStores.size();
}

