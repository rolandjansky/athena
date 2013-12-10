/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackQuality.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRACKQUALITY_H
# define IPATINTERFACES_ITRACKQUALITY_H

#include <list>
#include "GaudiKernel/IAlgTool.h"
#include "iPatTrack/TrackStatus.h"

// using iPatTrack family for now ...
class FitQuality;
class HitOnTrack;
class HitQuality;
class PerigeeParameters;
class Track;

/** Interface ID for ITrackQuality*/  
static const InterfaceID IID_ITrackQuality("ITrackQuality", 1, 0);
  
/**@class ITrackQuality

Abstract base class for TrackQuality AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class ITrackQuality: virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~ITrackQuality(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITrackQuality; }

    /**ITrackQuality interface:
       pT above cut-off */
    virtual bool	aboveMinPt	(const PerigeeParameters&) const = 0;

    /**ITrackQuality interface:
       track candidate with good fit and pT above cut-off */
    virtual bool	goodCandidate	(const FitQuality&, const PerigeeParameters&) const = 0;
    
    /**ITrackQuality interface:
       track with good fit, sufficient hits, not too many holes etc */
    virtual bool	goodTrack	(TrackStatus, const FitQuality&, const HitQuality&) const = 0;
    
    /**ITrackQuality interface:
       from track hit quality: how many hits is it reasonable to flag as outliers */
    virtual int		maxReject	(const HitQuality&) const = 0;
    
    /**ITrackQuality interface:
       set the minPt from parent algorithm */
    virtual void	minPt		(double minPt) = 0;
    
    /**ITrackQuality interface:
       debugging aids */
    virtual void	print		(const FitQuality&, const PerigeeParameters&) const = 0;
    virtual void	print		(const HitQuality&) const = 0;
    
    /**ITrackQuality interface:
       has the track got sufficient hits */
    virtual bool	sufficientHits	(TrackStatus				status,
					 HitQuality&				hitQuality,
					 std::list<HitOnTrack*>::const_iterator	begin,
					 std::list<HitOnTrack*>::const_iterator	end) const = 0;
};


#endif // IPATINTERFACES_ITRACKQUALITY_H

