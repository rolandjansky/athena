/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackBuilder.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRACKBUILDER_H
# define IPATINTERFACES_ITRACKBUILDER_H

#include <list>
#include "GaudiKernel/IAlgTool.h"
#include "iPatTrack/TrackStatus.h"

// using iPatTrack family for now ...
class HitOnTrack;
class Track;

/** Interface ID for ITrackBuilder*/  
static const InterfaceID IID_ITrackBuilder("ITrackBuilder", 1, 0);
  
/**@class ITrackBuilder

Abstract base class for TrackBuilder AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class ITrackBuilder: virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~ITrackBuilder(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITrackBuilder; }

    /**ITrackBuilder interface:
        create a track corresponding to an existing track with an extended hit_list.
        The return track has TrackStatus and takes ownership of the hit_list  */
    virtual Track*		trackFromHits (TrackStatus,
					       std::list<HitOnTrack*>*,
					       const Track&) const = 0;

    /**ITrackBuilder interface:
       refit a track in case of hit modification (such as after shared hit or outlier removal) */
    virtual Track*		trackRefit (const Track&) const = 0;

};


#endif // IPATINTERFACES_ITRACKBUILDER_H

