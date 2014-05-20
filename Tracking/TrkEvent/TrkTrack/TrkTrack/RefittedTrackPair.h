/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ====================================================================================
			RefittedTrackPair.h  -  description
			-----------------------------------
begin                : Friday 30th March 2006
author               : Tom Atkinson
email                : t.atkinson@physics.unimelb.edu.au
decription           : Class holding an element link to a track and pointer to the
                       refitted
======================================================================================= */

#ifndef TrkRefittedTrackPair_H
#define TrkRefittedTrackPair_H

#include "DataModel/ElementLink.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

namespace Trk{

/** 

@brief pair associating a refitted track to a track element link

  The RefittedTrack class is a pair. 
    - The first element is an element link to the original track
    - The second element is a pointer to the refitted track

@author t.atkinson@physics.unimelb.edu.au

*/


class RefittedTrackPair : public std::pair< ElementLink< DataVector< Track > >, Track* >{
 public:

  //!< Default constructor
  RefittedTrackPair();

  //!< Constructor using the element link to the original track and the pointer to the refitted track
  RefittedTrackPair( ElementLink< DataVector< Track > >, Track* );

  //!< Virtual destructor
  virtual ~RefittedTrackPair();

};

} // end Trk namespace

#endif
