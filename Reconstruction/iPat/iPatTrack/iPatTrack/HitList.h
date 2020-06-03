/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 typedef's for HitOnTrack list
 -----------------------------
 ***************************************************************************/

#ifndef IPATTRACK_HITLIST_H
# define IPATTRACK_HITLIST_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <list>
#include "iPatTrack/HitOnTrack.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class HitList
{
public:
    typedef	std::list<HitOnTrack*>			hit_list;
    typedef	hit_list::const_iterator		hit_citerator;
    typedef	hit_list::iterator			hit_iterator;
    typedef	hit_list::const_reverse_iterator 	hit_riterator;
};

#endif // IPATTRACK_HITLIST_H
