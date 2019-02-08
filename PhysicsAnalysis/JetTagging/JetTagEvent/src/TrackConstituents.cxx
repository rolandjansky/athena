/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           TrackConstituent.h  -  Description
                             -------------------
    begin   : January, 2005
    authors : Kyle Cranmer, Andreas Wildauer
    email   : cranmer@cern.ch, andreas.wildauer@cern.ch

    changes : 

***************************************************************************/
                                        
#include "JetTagEvent/TrackConstituents.h"
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
namespace Analysis
{
  

  TrackConstituents::TrackConstituents()
  {  }  

  TrackConstituents::TrackConstituents(NameType& name)
  { m_name = name; }  
  
  void TrackConstituents::setName(NameType& name) {m_name = name;}      
  
  NameType TrackConstituents::name() const { return m_name;}      

  IConstituent* TrackConstituents::clone() const
  { return new TrackConstituents( *this );}	
  

  void TrackConstituents::set_track(const Rec::TrackParticleContainer* theContainer, 
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
  
  void TrackConstituents::set_track(const Rec::TrackParticleContainer* theContainer, 
				    index_type& theIndex, double weight)
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

  void TrackConstituents::set_constituent(const Rec::TrackParticleContainer* theContainer, 
				    const Rec::TrackParticle* theTrack, double weight)
  { this->set_track(theContainer, theTrack, weight);  }
  
  void TrackConstituents::set_constituent(const Rec::TrackParticleContainer* theContainer, 
				    index_type& theIndex, double weight)
  { this->set_track(theContainer, theIndex, weight);  }

/*  void TrackConstituents::set_track(const INavigable4MomentumCollection* theContainer, 
				    const INavigable4Momentum* theTrack, double weight)
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
    

  }*/
  
  double TrackConstituents::getTrackWeight(const Rec::TrackParticle* theTrack) const
  {
    // from Navigable
    return (this->contains(theTrack) )
      ? this->getParameter(theTrack)
      : 0.;
  }
  
  double TrackConstituents::getTrackWeight(const Rec::TrackParticleContainer* theContainer,
				      index_type& theIndex) const
  {
    // from Navigable
    return (this->contains(theContainer,theIndex))
      ? this->getParameter(theContainer,theIndex)
      : 0.;
  }
  
  std::vector<const Rec::TrackParticle*>* TrackConstituents::tracks() const
  {
    std::vector<const Rec::TrackParticle*>* tracks  = 
      new std::vector<const Rec::TrackParticle*>();
  
    for(iterator it = begin(); it!= end(); ++it)
      tracks->push_back(*it);

    return tracks;
  }	

}
