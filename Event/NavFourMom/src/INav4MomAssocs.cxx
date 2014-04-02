/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// INav4MomAssocs.cxx 
// Implementation file for class INav4MomAssocs
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <algorithm>

// NavFourMom includes
#include "NavFourMom/INav4MomAssocs.h"
#include "EventKernel/IParticle.h"
#include "DataModel/DataVector.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
INav4MomAssocs::INav4MomAssocs() :
  AssociationMap<INavigable4MomentumCollection, 
                 INavigable4MomentumCollection>(),
  m_assocStores()
{}

INav4MomAssocs::INav4MomAssocs( const INav4MomAssocs& rhs) :
  AssociationMap<INavigable4MomentumCollection, 
                 INavigable4MomentumCollection>(rhs),
  m_assocStores( rhs.m_assocStores )
{}

// Assignment operator: 
INav4MomAssocs& INav4MomAssocs::operator=(const INav4MomAssocs& rhs)
{
  if ( this != &rhs ) {
    AssociationMap< INavigable4MomentumCollection,
                    INavigable4MomentumCollection >::operator=(rhs);
    m_assocStores = rhs.m_assocStores;
  }
  return *this;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

bool 
INav4MomAssocs::assocs( const INavigable4Momentum* object,
			INavigable4MomentumCollection& associatedElems ) const
{
  if ( 0 == object ) {
    return false;
  }

  // do the underlying stores contain this object ?
  bool contained = false;
  for ( std::map<std::string, DataLink<INav4MomAssocs> >::const_iterator itr = m_assocStores.begin();
	itr != m_assocStores.end();
	++itr ) {
    if ( itr->second->assocs(object, associatedElems) ) {
      contained = true;
    }
  }

  if ( !contained && !containsObject( object ) ) {
    return false;
  }

  associatedElems.reserve( associatedElems.size() +
			   getNumberOfAssociations(object) );
  INav4MomAssocs::asso_iterator endAssocs = endAssociation( object );
  for ( INav4MomAssocs::asso_iterator itr = beginAssociation( object );
        itr != endAssocs;
        ++itr ) {
    associatedElems.push_back( const_cast<INavigable4Momentum*>(*itr) );
  }

  return true;
}



bool 
INav4MomAssocs::getAssociations( const INav4MomLink& objectLink,
                                 INav4MomLinkContainer& associatedElementLinks ) const
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
INav4MomAssocs::getAssociations( const INavigable4Momentum* object,
                                 INav4MomLinkContainer& associatedElementLinks ) const
{
  if ( 0 == object )
    {
      return false;
    }

  // do the underlying stores contain this object ?
  bool contained = false;
  for ( std::map<std::string, DataLink<INav4MomAssocs> >::const_iterator itr = m_assocStores.begin();
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
  ElementLinkVector<INavigable4MomentumCollection>::const_iterator assoItr    = (foundIter->second).begin();
  ElementLinkVector<INavigable4MomentumCollection>::const_iterator assoItrEnd = (foundIter->second).end();
  for ( ; assoItr != assoItrEnd; ++assoItr )
    {
      const INav4MomLink assoLink = (*assoItr);
      associatedElementLinks.push_back( assoLink );
    }

  return true;
}



/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void INav4MomAssocs::addAssocStore( const DataLink<INav4MomAssocs>& assocStore )
{
  const std::string& id = assocStore.dataID();
  const std::map<std::string, DataLink<INav4MomAssocs> >::const_iterator itr = m_assocStores.find(id);
  if ( itr != m_assocStores.end() ) {
    m_assocStores[id] = assocStore;
  }
}

#ifdef __APPLE__
/////////////////////////////////////////////////////////////////// 
// Static template members: 
/////////////////////////////////////////////////////////////////// 
// This is only necessary for MacOSX where there's a bug in the linker that causes duplicate symbols if
// the static templates for the same concrete type are instantiated in two different .cxx files
template <class T> DataVector_detail::RegisterDVLEltBaseInit<T> DataVector_detail::DVLEltBaseInit<T>::s_regbase;
template class DataVector_detail::RegisterDVLEltBaseInit<IParticle> DataVector_detail::DVLEltBaseInit<IParticle>::s_regbase;
#endif

