/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

                                        
#include "JetTagEvent/TrackAssociation.h"
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
namespace Analysis
{

  TrackAssociation::TrackAssociation()
  { }  
  
  TrackAssociation::TrackAssociation(const NameType& name)
  {  this->setName(name); }  
  
  JetAssociationBase* TrackAssociation::clone() const
  {
    return new TrackAssociation( *this );
  }
  
  void TrackAssociation::set_track(const Rec::TrackParticleContainer* theContainer, 
				   const Rec::TrackParticle* theTrack, double weight)
  {
    double newWeight = weight;
    // track already in collection
    if ( this->contains(theTrack) )
      {
	// update weight
	newWeight *= this->getTrackWeight(theTrack);
	// remove the previous track 
	this->remove(theTrack);
      }
    
    // from Navigable base
    this->putElement(theContainer,theTrack,newWeight);
  }
  
  void TrackAssociation::set_track(const Rec::TrackParticleContainer* theContainer, 
				   const index_type& theIndex, double weight)
  {
    double newWeight = weight;
    // track already in collection
    if ( this->contains(theContainer,theIndex) )
      {
	// update weight
	newWeight *= this->getTrackWeight(theContainer,theIndex);
	// remove the previous track
	this->remove(theContainer,theIndex);
      }
    
    // from Navigable base
    this->putElement(theContainer,theIndex,newWeight);
  }
  
  void TrackAssociation::set_association(const Rec::TrackParticleContainer* theContainer, 
					 const Rec::TrackParticle* theTrack, double weight)
  {
    this->set_track(theContainer, theTrack, weight);
  }
  
  void TrackAssociation::set_association(const Rec::TrackParticleContainer* theContainer, 
					 const index_type& theIndex, double weight)
  {
    this->set_track(theContainer, theIndex, weight);
  }
  
  double TrackAssociation::getTrackWeight(const Rec::TrackParticle* theTrack) const
  {
    // from Navigable
    return (this->contains(theTrack) )
      ? this->getParameter(theTrack)
      : 0.;
  }
  
  double TrackAssociation::getTrackWeight(const Rec::TrackParticleContainer* theContainer,
					  const index_type& theIndex) const
  {
    // from Navigable
    return (this->contains(theContainer,theIndex))
      ? this->getParameter(theContainer,theIndex)
      : 0.;
  }
  
  std::vector<const Rec::TrackParticle*>* TrackAssociation::tracks() const
  {
    std::vector<const Rec::TrackParticle*>* tracks  = 
      new std::vector<const Rec::TrackParticle*>();
    
    for(iterator it = begin(); it!= end(); ++it)
      tracks->push_back(*it);
    
    return tracks;
  }
}
