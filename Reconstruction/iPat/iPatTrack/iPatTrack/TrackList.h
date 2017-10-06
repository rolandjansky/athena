/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 typedef's for Track collections (only list at present)
 ------------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACK_TRACKLIST_H
# define IPATTRACK_TRACKLIST_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <list>
#include "iPatTrack/Track.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class TrackList
{
public:
    typedef	std::list<Track*>		track_list;
    typedef	track_list::const_iterator 	track_citerator;
    typedef	track_list::iterator		track_iterator;
    typedef	track_list::reverse_iterator 	track_riterator;
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>
//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // IPATTRACK_TRACKLIST_H
