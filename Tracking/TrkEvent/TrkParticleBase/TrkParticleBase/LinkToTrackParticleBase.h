/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKPARTICLEBASE_LINKTOTRACKPARTICLEBASE_H
#define TRKPARTICLEBASE_LINKTOTRACKPARTICLEBASE_H

#include "TrkTrackLink/ITrackLink.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "DataModel/ElementLink.h"

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
    virtual ~LinkToTrackParticleBase();
   
    /** return the track parameters of the track (to which the EL< TrackParticleBaseCollection > points) */
    const TrackParameters * parameters() const;

    /** return the neutral parameters of the NeutralParticle) */
    const NeutralParameters* neutralParameters() const {return NULL;};
    
    /** method to clone the LinkToTrack object */
    LinkToTrackParticleBase * clone() const ;
    
    /** assignment operator */
    LinkToTrackParticleBase& operator= ( const LinkToTrackParticleBase& rhs );
 
 };//end of class definitions
}//end of namespace definitions

#endif //LINKTOTRACKPARTICLEBASE_H
