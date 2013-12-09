/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AssignedHits.h, (c) ATLAS iPatRec
//
// book-keeping needed for ambiguity resolution.
// Maintain a record of which track(s) are assigned to each hit (cluster or straw)
//
///////////////////////////////////////////////////////////////////

#ifndef IPATTRACKFINDER_ASSIGNEDHITS_H
# define IPATTRACKFINDER_ASSIGNEDHITS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include <map>
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/Identifier.h"
#include "iPatInterfaces/IAssignedHits.h"
#include "iPatTrack/HitList.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class AssignedHits: public AthAlgTool,
		    virtual public IAssignedHits
{

public:
    AssignedHits	(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
    ~AssignedHits 	(void);

    StatusCode		initialize();
    StatusCode		finalize();

    bool		allHitsShared		(const Track& track) const;
    bool		assignedToPrimary	(const Identifier& clusterId) const;
    bool		assignedToPrimary	(const Point& point) const;
    void		clear			(void);
    void		includeTrack		(const Track* track);
    bool		isAssigned		(const Point& point) const;
    unsigned		numberOfClusters	(void) const;
    bool		onSameTrack		(const Point& point1, const Point& point2) const;
    void		print			(void) const;
    void		removeTrack		(const Track* track);
    void		setShared		(
	std::list<HitOnTrack*>::const_iterator hitListBegin,
	std::list<HitOnTrack*>::const_iterator hitListEnd) const;
    bool		sharesWithPrimary	(const Track& track) const;
    int			uniquenessScore		(const Track& track) const;
    const Track*	withSharedFinalHits	(const Track& track) const;

private:
    typedef	HitList::hit_citerator			hit_citerator;
    typedef	std::list<const Track*>			track_list;
    typedef	track_list::const_iterator 		track_citerator;
    typedef	track_list::iterator      		track_iterator;
    typedef     std::map<Identifier,track_list*>	track_map;
    typedef	track_map::const_iterator		map_citerator;
    
    bool		onSameTrack (map_citerator,map_citerator) const;
    
    track_map		m_clusterTrackMap;
};

//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                >>>>>>

inline bool
AssignedHits::onSameTrack (map_citerator m1, map_citerator m2) const 
{
    if (m2 == m_clusterTrackMap.end()) return false;
    // loop over track list associated to first hit
    // and look for a common track (associated to second hit) 
    for (track_citerator t = (*m1).second->begin();
	 t != (*m1).second->end();
	 ++t)
    {
	if (std::find((*m2).second->begin(),(*m2).second->end(),*t) != (*m2).second->end())
	    return true;
    }
    return false;
}

#endif // IPATTRACKFINDER_ASSIGNEDHITS_H
