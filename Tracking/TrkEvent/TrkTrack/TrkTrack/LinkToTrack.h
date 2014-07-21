/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACK_LINKTOTRACK_H
#define TRKTRACK_LINKTOTRACK_H

#include "TrkTrackLink/ITrackLink.h"
#include "DataModel/ElementLink.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h" 


namespace Trk
{
 /** @brief AUTO - An Undocumented Tracking Object */

 class LinkToTrack : public ITrackLink, public ElementLink<TrackCollection> 
 {
 
   public:
    /** default constructor (needed for persistency) */
    LinkToTrack();
    
    /** constructor with ElementLink */
    LinkToTrack( ElementLink<TrackCollection>& link );
    
    /** default destructor */
    virtual ~LinkToTrack();
   
    /** return the track parameters of the track (to which the EL< TrackCollection > points) */
    const TrackParameters* parameters() const;

    /** return the neutral parameters of the NeutralParticle) */
    const NeutralParameters* neutralParameters() const {return NULL;};
    
    /** method to clone the LinkToTrack object */
    LinkToTrack * clone() const ;
    
    /** assignment operator */
    Trk::LinkToTrack& operator= ( const Trk::LinkToTrack& rhs );
    // This is here to make sure that the TrackCollection typedef
    // will make it into the dictionary.
    typedef TrackCollection TrackCollectionForDict;
 };//end of class definitions
}//end of namespace definitions

#endif //TRKTRACK_LINKTOTRACK_H
