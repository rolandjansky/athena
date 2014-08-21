/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_INDETCANDIDATE_H
#define MUONCOMBINEDEVENT_INDETCANDIDATE_H

#include "MuonCombinedEvent/TagBase.h"
#include <vector>
#include "Particle/ParticleExtrapolationMap.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthLinks/ElementLink.h"

namespace MuonCombined {

  class TagBase;
  
  class InDetCandidate {
    
  public:
    /** constructor taking a xAOD::TrackParicle&
	Users should ensure that the lifetime of the idTrack object is longer that the InDetCandidate
	as it internally caches a pointer to it. 
     */
    InDetCandidate(const xAOD::TrackParticle& idTrack);

    /** constructor taking an ElementLink to a xAOD::TrackParicle&
	Users should ensure that the element link is valid and the lifetime of the idTrack object is longer that the InDetCandidate
	as it internally caches a pointer to it. 
     */
    InDetCandidate( const ElementLink<xAOD::TrackParticleContainer>& idTrackLink );

    /** destructor */
    ~InDetCandidate();
    
    /** access TrackParticle */
    const xAOD::TrackParticle& indetTrackParticle() const;

    /** access TrackParticleLink */
    const ElementLink<xAOD::TrackParticleContainer>& indetTrackParticleLink() const;
    
    /** extend InDetCandidate, takes ownership of tag object */
    void addTag(const TagBase& tag);
    
    /** access tags */
    const std::vector<const TagBase*>& combinedDataTags() const;

    /** access last tag */
    const TagBase*                     lastCombinedDataTag() const;

    /** access of a given type tag */
    const TagBase*                     lastCombinedDataTag( TagBase::Type type ) const;

    /** set cached extrapolations */
    void particleExtrapolationVector( const Rec::ParticleExtrapolationVector* pars ); 

    /** access cached extrapolations */
    const Rec::ParticleExtrapolationVector* particleExtrapolationVector() const; 

    /** print candidate to string */
    std::string toString() const;
    
  private:
    /** ElementLink to the InDet TrackParticle */
    ElementLink<xAOD::TrackParticleContainer> m_idTrackParticleLink;

    /** cached pointer to the InDet TrackParticle */
    const xAOD::TrackParticle*   m_idTrackParticle;

    /** vector of tags */
    std::vector<const TagBase*> m_tags;
    
    /** cached pointer to Inner detector track particle expressed several reference surfaces in ATLAS */
    const Rec::ParticleExtrapolationVector* m_parameters;

  };

  inline const xAOD::TrackParticle& InDetCandidate::indetTrackParticle() const { 
    if( m_idTrackParticleLink.isValid() ) return **m_idTrackParticleLink;
    return *m_idTrackParticle; 
  }

  inline const ElementLink<xAOD::TrackParticleContainer>& InDetCandidate::indetTrackParticleLink() const { return m_idTrackParticleLink; }


  inline void InDetCandidate::addTag(const TagBase& tag) {
    m_tags.push_back(&tag);
  }

  inline  const std::vector<const TagBase*>& InDetCandidate::combinedDataTags() const {
    return m_tags;
  }

  inline const TagBase* InDetCandidate::lastCombinedDataTag() const {
    if( m_tags.empty() ) return 0;
    return m_tags.back();
  }

  inline const TagBase* InDetCandidate::lastCombinedDataTag( TagBase::Type type) const {
    for( auto x : m_tags ) {
      if( x->type() == type ) return x;
    }
    return 0;
  }

  inline void InDetCandidate::particleExtrapolationVector( const Rec::ParticleExtrapolationVector* pars ) {
    if( m_parameters ) delete m_parameters;
    m_parameters = pars;
  }

  inline const Rec::ParticleExtrapolationVector* InDetCandidate::particleExtrapolationVector() const {
    return m_parameters;
  } 

}


#endif
