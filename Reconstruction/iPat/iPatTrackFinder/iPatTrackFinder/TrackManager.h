/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackManager.h, (c) ATLAS iPatRec
//
// manage the tracks and track-candidates found during track finding
//
///////////////////////////////////////////////////////////////////

#ifndef IPATTRACKFINDER_TRACKMANAGER_H
# define IPATTRACKFINDER_TRACKMANAGER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/ITrackManager.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IAssignedHits;
class IDetectorSelection;
class ITrackBuilder;
class ITrackFollower;
class ITrackQuality;

class TrackManager: public AthAlgTool,
		    virtual public ITrackManager
{
    
public:
    TrackManager		(const std::string& type,
				 const std::string& name,
				 const IInterface*  parent);
    ~TrackManager 		(void);
    
    StatusCode			initialize();
    StatusCode			finalize();

    void			addCurrentSecondary	(Track*);
    void			addCurrentSegment	(Track*);
    void			addCurrentTrack		(Track*);
    void			clearTrackList		(void);
    void			finalizeTrackFitting	(void);
    void			newTrackSearch		(void);
    void			resetCurrentTracks	(void);
    void			saveCurrentTracks	(void);
    void			setProperties 		(int printLevel);
    TrackList::track_list*	trackList		(void);
    unsigned			trackListSize		(void) const;
    void			uniquenessCheck		(void);
    double			zFirstTrack		(void) const;
    double			zLastTrack		(void) const;

private:
    typedef     TrackList::track_list           track_list;
    typedef     TrackList::track_citerator      track_citerator;
    typedef     TrackList::track_iterator       track_iterator;
    typedef     TrackList::track_riterator      track_riterator;

    void			ambiguityCheck (void);
    void			insertInTrackList (Track*);
    bool			isFirstTrackBetter (const Track& first,
						    const Track& second) const;
    void			setSharedHits (void) const;

    // helpers, managers, tools
    ToolHandle<IAssignedHits>		m_assignedHits;
    ToolHandle<IDetectorSelection>	m_detectorSelection;
    ToolHandle<ITrackBuilder>		m_trackBuilder;
    ToolHandle<ITrackFollower>		m_trackFollower;
    ToolHandle<ITrackQuality>		m_trackQuality;

    // state
    Track*				m_primaryTrack;
    int					m_printLevel;
    Track*				m_secondaryTrack;
    track_list*				m_trackList;
    Track*				m_truncatedTrack;
};

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline void
TrackManager::resetCurrentTracks (void)
{
    m_primaryTrack	= 0;
    m_secondaryTrack	= 0;
    m_truncatedTrack	= 0;
}

inline TrackList::track_list*
TrackManager::trackList (void) 
{
    // NOTE: pass ownership of list to client
    TrackList::track_list* tracks = m_trackList;
    m_trackList = 0;
    return tracks;
}

inline unsigned
TrackManager::trackListSize (void) const
{
    if (m_trackList) return m_trackList->size();
    return 0;
}

inline double
TrackManager::zFirstTrack (void) const
{ return (**m_trackList->begin()).perigee_parameters().z(); }

inline double
TrackManager::zLastTrack (void) const
{ return (**m_trackList->rbegin()).perigee_parameters().z(); }

#endif // IPATTRACKFINDER_TRACKMANAGER_H
