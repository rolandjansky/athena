/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACK_LINKTOXAODTRACKPARTICLE_H
#define TRKTRACK_LINKTOXAODTRACKPARTICLE_H

#include "TrkTrackLink/ITrackLink.h"
#include "AthLinks/ElementLink.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"


namespace Trk
{
 /** @brief Element link to XAOD TrackParticle. 
  *  An EDM object to reduce dependencies among 
  * Tracking/InDet/XAOD domains. This is NOT an
  * analysis class. This link keeps the EDM of 
  * different domains together and makes it interchangeable.
  * WARNING: Do not use this class unless you understand 
  * how the ITrackLink functions.  
  *
  * A hack to the EDM which will be possibly removed at later stages
  * Kirill.Prokofiev@cern.ch
  *
  * February 2014
  */

 class LinkToXAODTrackParticle : public ITrackLink, public ElementLink<xAOD::TrackParticleContainer> 
 {
 
   public:
    /** default constructor (needed for persistency) */
    LinkToXAODTrackParticle();
    
    /** constructor with ElementLink */
    LinkToXAODTrackParticle( ElementLink<xAOD::TrackParticleContainer>& link );

    /** default destructor */
    virtual ~LinkToXAODTrackParticle() = default;
   
    /** return the track parameters of the track (to which the EL< TrackCollection > points) */
    const TrackParameters* parameters() const;

    /** return the neutral parameters of the NeutralParticle) */
    virtual const NeutralParameters* neutralParameters() const {return NULL;};
    
    /** method to clone the LinkToXAODTrackParticle object */
    LinkToXAODTrackParticle * clone() const ;
    
    // This is here to make sure that the TrackCollection typedef
    // will make it into the dictionary.
   // typedef TrackCollection TrackCollectionForDict;
 };//end of class definitions
}//end of namespace definitions

#endif //TRKTRACK_LINKTOXAODTRACKPARTICLE_H
