/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackFollower.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRACKFOLLOWER_H
# define IPATINTERFACES_ITRACKFOLLOWER_H

#include "GaudiKernel/IAlgTool.h"

// this is a typedef
#include "InDetPrepRawData/SiClusterContainer.h"

// using iPatTrack family for now ...
class Track;
  
/** Interface ID for ITrackFollower*/  
static const InterfaceID IID_ITrackFollower("ITrackFollower", 1, 0);
  
/**@class ITrackFollower

Abstract base class for TrackFollower AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class ITrackFollower: virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~ITrackFollower(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITrackFollower; }

    /**ITrackFollower interface:
        create a track with association (by extrapolation) to TRT */
    virtual Track*		associate_trt (const Track&) = 0;

    /**ITrackFollower interface: create a track after extrapolation (inwards or outwards) */
    virtual Track*		extrapolate_inwards (const Track&) const = 0;
    virtual Track*		extrapolate_outwards (const Track&) const = 0;
    
    /**ITrackFollower interface: create a track after interpolation (various possibilities) */
    virtual Track*		fast_interpolate_outwards (const Track&) const = 0;
    virtual Track*		interpolate (const Track&) const = 0;
    virtual Track*		interpolate_from_beam_spot (const Track&) const = 0;

    /**ITrackFollower interface: set input data containers (silicon clusters) */
    virtual void		setDataContainers (const InDet::SiClusterContainer* pixelContainer,
						   const InDet::SiClusterContainer* sctContainer) = 0;

};


#endif // IPATINTERFACES_ITRACKFOLLOWER_H

