/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_INDETCANDIDATE_H
#define MUONCOMBINEDEVENT_INDETCANDIDATE_H

#include <vector>
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthLinks/ElementLink.h"
#include "MuonLayerEvent/MuonSystemExtension.h"

namespace MuonCombined {

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

    /** Was this created using a special far forward indet track*/
    bool m_siAssociated;

    const Muon::MuonSystemExtension* m_extension;
    
  };

  inline const xAOD::TrackParticle& InDetCandidate::indetTrackParticle() const { 
    if( m_idTrackParticleLink.isValid() ) return **m_idTrackParticleLink;
    return *m_idTrackParticle; 
  }

  inline const ElementLink<xAOD::TrackParticleContainer>& InDetCandidate::indetTrackParticleLink() const { return m_idTrackParticleLink; }

  inline bool InDetCandidate::isSiliconAssociated() const { return m_siAssociated; }
  
  inline void InDetCandidate::setSiliconAssociated( bool value ) { m_siAssociated=value; }

  inline void InDetCandidate::setExtension(const Muon::MuonSystemExtension* extension) { m_extension=extension;}

  inline const Muon::MuonSystemExtension* InDetCandidate::getExtension() const { return m_extension;}
}


#endif
