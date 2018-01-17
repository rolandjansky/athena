/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_INDETCANDIDATE_H
#define MUONCOMBINEDEVENT_INDETCANDIDATE_H

#include "MuonCombinedEvent/TagBase.h"
#include <vector>
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthLinks/ElementLink.h"
#include "MuonLayerEvent/MuonSystemExtension.h"

namespace MuonCombined {

  class TagBase;
  
  class InDetCandidate {
    
  public:
    /** constructor taking a xAOD::TrackParicle&
	Users should ensure that the lifetime of the idTrack object is longer than the InDetCandidate
	as it internally caches a pointer to it. 
     */
    InDetCandidate(const xAOD::TrackParticle& idTrack);

    /** constructor taking an ElementLink to a xAOD::TrackParicle&
	Users should ensure that the element link is valid and the lifetime of the idTrack object is longer than the InDetCandidate
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
    
    //access MuonSystemExtension
    const Muon::MuonSystemExtension* getExtension() const;

    //set MuonSystemExtension, taking ownership
    void setExtension(const Muon::MuonSystemExtension* extension);

    /** Returns true if this candidate was formed from a special far forward InDet track.*/
    bool isSiliconAssociated() const;
    
    /** Pass true if this candiate was created from a special far forward InDet track.*/
    void setSiliconAssociated(bool);

    /** print candidate to string */
    std::string toString() const;
    
  private:
    /** ElementLink to the InDet TrackParticle */
    ElementLink<xAOD::TrackParticleContainer> m_idTrackParticleLink;

    /** cached pointer to the InDet TrackParticle */
    const xAOD::TrackParticle*   m_idTrackParticle;

    /** vector of tags */
    std::vector<const TagBase*> m_tags;
    
    /** Was this created using a special far forward indet track*/
    bool m_siAssociated;

    const Muon::MuonSystemExtension* m_extension;
    
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

  inline bool InDetCandidate::isSiliconAssociated() const { return m_siAssociated; }
  
  inline void InDetCandidate::setSiliconAssociated( bool value ) { m_siAssociated=value; }

  inline void InDetCandidate::setExtension(const Muon::MuonSystemExtension* extension) { m_extension=extension;}

  inline const Muon::MuonSystemExtension* InDetCandidate::getExtension() const { return m_extension;}
}


#endif
