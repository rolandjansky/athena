/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKPARTICLEBASE_LINKTOTRACKPARTICLEBASE_H
#define TRKPARTICLEBASE_LINKTOTRACKPARTICLEBASE_H

#include "TrkTrackLink/ITrackLink.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "AthLinks/ElementLink.h"

namespace Trk
{

 
 class LinkToTrackParticleBase : public ITrackLink, public ElementLink<TrackParticleBaseCollection>
 {
 
   public:
    /** default constructor (needed for persistency) */
    LinkToTrackParticleBase( );
    
    /** constructor with ElementLink */
    LinkToTrackParticleBase( ElementLink<TrackParticleBaseCollection>& link);
    
    /** default destructor */
    virtual ~LinkToTrackParticleBase() = default;
   
    /** return the track parameters of the track (to which the EL< TrackParticleBaseCollection > points) */
    const TrackParameters * parameters() const;

    /** return the neutral parameters of the NeutralParticle) */
    const NeutralParameters* neutralParameters() const {return nullptr;};
    
    /** method to clone the LinkToTrack object */
    LinkToTrackParticleBase * clone() const ;
    
 };//end of class definitions
}//end of namespace definitions

#endif //LINKTOTRACKPARTICLEBASE_H
