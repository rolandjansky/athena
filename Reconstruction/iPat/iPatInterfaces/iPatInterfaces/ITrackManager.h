/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackManager.h, (c) ATLAS iPatRec
//
// manage the tracks and track-candidates found during track finding
//
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRACKMANAGER_H
# define IPATINTERFACES_ITRACKMANAGER_H

#include "GaudiKernel/IAlgTool.h"
#include "iPatTrack/TrackList.h"

/** Interface ID for ITrackManager*/  
static const InterfaceID IID_ITrackManager("ITrackManager", 1, 0);
  
/**@class ITrackManager

Abstract base class for TrackManager AlgTool
      
@author Alan.Poppleton@cern.ch
*/

class ITrackManager: virtual public IAlgTool
{
public:
    /**Virtual destructor*/
    virtual ~ITrackManager()	{}

    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITrackManager; }

    /**ITrackManager interface: include current secondary track	*/
    virtual void			addCurrentSecondary	(Track*) = 0;
    
    /**ITrackManager interface: include current secondary track	*/
    virtual void			addCurrentSegment	(Track*) = 0;
    
    /**ITrackManager interface: include current track		*/
    virtual void			addCurrentTrack		(Track*) = 0;

    /**ITrackManager interface: clear				*/
    virtual void			clearTrackList		(void) = 0;

    /**ITrackManager interface: make sure all tracks have up-to-date track fit	*/
    virtual void			finalizeTrackFitting	(void) = 0;

    /**ITrackManager interface: initialize for new track search	*/
    virtual void			newTrackSearch		(void) = 0;

    /**ITrackManager interface: reset the current tracks	*/
    virtual void			resetCurrentTracks	(void) = 0;

    /**ITrackManager interface: save the current tracks		*/
    virtual void			saveCurrentTracks	(void) = 0;

    /**ITrackManager interface: set printLevel property		*/
    virtual void			setProperties 		(int printLevel) = 0;

    /**ITrackManager interface: return the track_list (accepted tracks)	*/
    virtual TrackList::track_list*	trackList		(void) = 0;

    /**ITrackManager interface: number of accepted tracks	*/
    virtual unsigned			trackListSize		(void) const = 0;

    /**ITrackManager interface: check for ambiguous tracks	*/
    virtual void			uniquenessCheck		(void) = 0;

    /**ITrackManager interface: zFirstTrack			*/
    virtual double			zFirstTrack		(void) const = 0;

    /**ITrackManager interface: zLastTrack			*/
    virtual double			zLastTrack		(void) const = 0;
};

#endif // IPATINTERFACES_ITRACKMANAGER_H
