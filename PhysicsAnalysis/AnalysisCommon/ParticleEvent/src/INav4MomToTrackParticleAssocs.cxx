/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// INav4MomToTrackParticleAssocs.cxx 
// Implementation file for class INav4MomToTrackParticleAssocs
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <algorithm>

#include "ParticleEvent/INav4MomToTrackParticleAssocs.h"
#include "AthContainers/DataVector.h"
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
INav4MomToTrackParticleAssocs::INav4MomToTrackParticleAssocs() :
  AssociationMap<INavigable4MomentumCollection, Rec::TrackParticleContainer>(), 
  m_assocStores()
{}

INav4MomToTrackParticleAssocs::INav4MomToTrackParticleAssocs( const INav4MomToTrackParticleAssocs& rhs) :
  AssociationMap<INavigable4MomentumCollection, Rec::TrackParticleContainer>(rhs),
  m_assocStores( rhs.m_assocStores )
{}
 
// Assignment operator: 
INav4MomToTrackParticleAssocs& INav4MomToTrackParticleAssocs::operator=(const INav4MomToTrackParticleAssocs& rhs)
{
  if ( this != &rhs ) {
    AssociationMap< INavigable4MomentumCollection, Rec::TrackParticleContainer >::operator=(rhs);
    m_assocStores = rhs.m_assocStores;
  }
  return *this;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
 
bool INav4MomToTrackParticleAssocs::assocs( const INavigable4Momentum* object,
                                            Rec::TrackParticleContainer& associatedElems ) const
{
  // Make a sanity check against a null pointer
  if ( 0 == object ) { return false; }

  // do the underlying stores contain this object ?
  bool contained = false;
  for ( std::map<std::string, DataLink<INav4MomToTrackParticleAssocs> >::const_iterator itr(m_assocStores.begin()), itrEnd(m_assocStores.end());
        itr != itrEnd;
        ++itr )
    {
      if ( itr->second->assocs(object, associatedElems) )
        {
          contained = true;
        }
    }
  
  if ( !contained && !containsObject( object ) )
    {
      return false;
    }
  
  associatedElems.reserve( associatedElems.size() + getNumberOfAssociations(object) );
  INav4MomToTrackParticleAssocs::asso_iterator endAssocs = endAssociation( object );
  for ( INav4MomToTrackParticleAssocs::asso_iterator itr = beginAssociation( object );
        itr != endAssocs;
        ++itr )
    {
      associatedElems.push_back( const_cast<Rec::TrackParticle*>(*itr) );
    }
  
  return true;
}



bool 
INav4MomToTrackParticleAssocs::getAssociations( const INav4MomLink& objectLink,
                                                std::vector< AssociationMap< INavigable4MomentumCollection,
                                                                             Rec::TrackParticleContainer >::asso_link >& associatedElementLinks ) const
{
  if ( !objectLink.isValid() )
    {
      return false;
    }
  
  // Get the object
  const INavigable4Momentum* object(NULL);
  object = (*objectLink);
  if ( object == NULL )
    {
      return false;
    }
  
  return getAssociations( object, associatedElementLinks );
}



bool
INav4MomToTrackParticleAssocs::getAssociations( const INavigable4Momentum* object,
                                                std::vector< AssociationMap< INavigable4MomentumCollection,
                                                                             Rec::TrackParticleContainer >::asso_link >& associatedElementLinks ) const
{
  if ( 0 == object )
    {
      return false;
    }
 
  // do the underlying stores contain this object ?
  bool contained = false;
  for ( std::map<std::string, DataLink<INav4MomToTrackParticleAssocs> >::const_iterator itr = m_assocStores.begin();
        itr != m_assocStores.end();
        ++itr )
    {
      if ( itr->second->getAssociations( object, associatedElementLinks ) )
        {
          contained = true;
        }
    }
  
  if ( !contained && !containsObject( object ) )
    {
      return false;
    }
 
  // check key
  store_type::const_iterator mapEnd    = m_associationMap.end();
  store_type::const_iterator foundIter = mapEnd;
  for ( store_type::const_iterator iMap = m_associationMap.begin();
        iMap != mapEnd;
        ++iMap )
    {
      // look for the address of the pointed-at object 
      // must dereference the ElementLink pointer
      if ( &*(*(iMap->first)) == &*(object) )
        {
          foundIter = iMap;
          break;
        }
    }
  if ( foundIter == m_associationMap.end() )
    {
      return false;
    }
  
  // Now, loop over all associations and fill the vector of associations
  associatedElementLinks.reserve( associatedElementLinks.size() +
                                  getNumberOfAssociations(object) );
  ElementLinkVector<Rec::TrackParticleContainer>::const_iterator assoItr    = (foundIter->second).begin();
  ElementLinkVector<Rec::TrackParticleContainer>::const_iterator assoItrEnd = (foundIter->second).end();
  for ( ; assoItr != assoItrEnd; ++assoItr )
    {
      const AssociationMap< INavigable4MomentumCollection, Rec::TrackParticleContainer >::asso_link assoLink = (*assoItr);
      associatedElementLinks.push_back( assoLink );
    }
 
  return true;
}



/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
 /////////////////////////////////////////////////////////////////// 

void INav4MomToTrackParticleAssocs::addAssocStore( const DataLink<INav4MomToTrackParticleAssocs>& assocStore )
{
  const std::string& id = assocStore.dataID();
  const std::map<std::string, DataLink<INav4MomToTrackParticleAssocs> >::const_iterator itr = m_assocStores.find(id);
  if ( itr == m_assocStores.end() )
    {
      m_assocStores[id] = assocStore;
    }
}

DataLink<INav4MomToTrackParticleAssocs>
INav4MomToTrackParticleAssocs::assocStore (const std::string& name) const
{
  std::map<std::string, INav4MomToTrackParticleAssocsLink_t>::const_iterator i =
    m_assocStores.find (name);
  if (i != m_assocStores.end())
    return i->second;
  return DataLink<INav4MomToTrackParticleAssocs>();
}


size_t INav4MomToTrackParticleAssocs::nAssocStores() const
{
  return m_assocStores.size();
}

