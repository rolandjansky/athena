/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventShapeEvent/EventFeatureStore.h"

//EventFeature EventFeatureStore::m_invalidConstReference = EventFeature();
//EventFeature EventFeatureStore::m_invalidReference = EventFeature();


EventFeatureStore::EventFeatureStore(SG::OwnershipPolicy own)
  : DataVector<EventFeature>(own)
{ }

EventFeatureStore::~EventFeatureStore()
{ }

bool EventFeatureStore::retrieve(EventFeature::FeatureType type,
				 const std::string&        tag,
				 const EventFeature*      &pFeature) const
{
  const_iterator fFeat(this->find(type,tag));
  if ( fFeat != this->end() )
    {
      pFeature = *fFeat;
      return true;
    }
  return false;
}

bool EventFeatureStore::retrieve(EventFeature::FeatureType type,
				 const std::string&        tag,
				 EventFeature*            &pFeature)
{
  iterator fFeat(this->find(type,tag));
  if ( fFeat != this->end() )
    {
      pFeature = *fFeat;
      return true;
    }
  return false;
}

EventFeatureStore::iterator EventFeatureStore::find(EventFeature::FeatureType
						    type,
						    const std::string& tag)
{
  iterator fFeat(this->begin());
  iterator lFeat(this->end());
  while ( fFeat != lFeat && ( type != (*fFeat)->type() || 
			      tag != (*fFeat)->tag() ) )
    { ++fFeat; }
  return fFeat;
}

EventFeatureStore::const_iterator 
EventFeatureStore::find(EventFeature::FeatureType type,
			const std::string& tag) const
{
  const_iterator fFeat(this->begin());
  const_iterator lFeat(this->end());
  while ( fFeat != lFeat && ( type != (*fFeat)->type() || 
			      tag  != (*fFeat)->tag() ) )
    { ++fFeat; }
  return fFeat;
}

EventFeatureStore::iterator EventFeatureStore::find(EventFeature* pFeature) 
{
  iterator fFeat(this->begin());
  iterator lFeat(this->end());
  while ( fFeat != lFeat && (*fFeat) != pFeature ) { ++fFeat; }
  return fFeat;  
}

EventFeatureStore::const_iterator 
EventFeatureStore::find(const EventFeature* pFeature) const
{
  const_iterator fFeat(this->begin());
  const_iterator lFeat(this->end());
  while ( fFeat != lFeat && (*fFeat) != pFeature ) { ++fFeat; }
  return fFeat;  
}
