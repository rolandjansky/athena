/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AssignedHits.cxx, (c) ATLAS iPatRec
//
// book-keeping needed for ambiguity resolution.
// Maintain a record of which track(s) are assigned to each hit (cluster or straw)
//
///////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include "iPatTrack/HitStatus.h"
#include "iPatTrack/Track.h"
#include "iPatTrackFinder/AssignedHits.h"
#include "iPatTrackFinder/Point.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

AssignedHits::AssignedHits  (const std::string&	type,
			     const std::string&	name,
			     const IInterface*	parent)
    :   AthAlgTool	(type, name, parent)
{
    declareInterface<IAssignedHits>(this);
}

AssignedHits::~AssignedHits (void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
AssignedHits::initialize()
{
    ATH_MSG_DEBUG( "AssignedHits::initialize()" );

    return StatusCode::SUCCESS;
}

StatusCode
AssignedHits::finalize()
{
    ATH_MSG_DEBUG( "AssignedHits::finalize()" );

    for (map_citerator m = m_clusterTrackMap.begin();
	 m != m_clusterTrackMap.end();
	 ++m)
	delete (*m).second;
    
    return StatusCode::SUCCESS; 
}

bool
AssignedHits::allHitsShared (const Track& track) const
{
    for (hit_citerator h = track.hit_list_begin();
	 h != track.hit_list_end();
	 ++h)
    {
        if ((**h).isCluster())
	{
	    map_citerator m = m_clusterTrackMap.find((**h).identifier());
	    if (m == m_clusterTrackMap.end() || (*m).second->size() == 0)
	        return false;
	}
    }

    return true;
}

bool
AssignedHits::assignedToPrimary (const Identifier& clusterId) const
{
    // true if cluster belongs a track already associated to a vertex
    map_citerator m = m_clusterTrackMap.find(clusterId);
    if (m != m_clusterTrackMap.end())
    {
	for (track_citerator t = (*m).second->begin();
	     t != (*m).second->end();
	     ++t)
	    if ((**t).status() >= truncated)	return true;
    }
    
    return false;    
}

bool
AssignedHits::assignedToPrimary (const Point& point) const
{
    // true if either cluster shared with a track already associated to a vertex
    // check first cluster
    if (assignedToPrimary(point.clusterId1()))	return true;
    if (point.isPixel())			return false;

    // similar check for second cluster
    return assignedToPrimary(point.clusterId2());
}
void
AssignedHits::clear (void)
{
    for (map_citerator m = m_clusterTrackMap.begin();
	 m != m_clusterTrackMap.end();
	 ++m)
	delete (*m).second;
    
    m_clusterTrackMap.clear();
}

void
AssignedHits::includeTrack (const Track* track)
{
    for (hit_citerator h = track->hit_list_begin();
	 h != track->hit_list_end();
	 ++h)
    {	
	if ((**h).isPositionMeasurement())
	{
	    Identifier index = (**h).identifier();
	    if (m_clusterTrackMap[index] == 0) m_clusterTrackMap[index] = new track_list;
	    m_clusterTrackMap[index]->push_front(track);
	}
    }
}

bool
AssignedHits::isAssigned (const Point& point) const
{
    map_citerator	m1 = m_clusterTrackMap.find(point.clusterId1());
    if (m1 == m_clusterTrackMap.end())	return false;
    if (point.isPixel())		return true;
    map_citerator	m2 = m_clusterTrackMap.find(point.clusterId2());
    return onSameTrack(m1,m2);
}

unsigned
AssignedHits::numberOfClusters (void) const
{ return m_clusterTrackMap.size(); }

bool
AssignedHits::onSameTrack (const Point& point1, const Point& point2) const
{
    map_citerator       m1 =  m_clusterTrackMap.find(point1.clusterId1());
    if (m1 == m_clusterTrackMap.end())					return false;
    if (!onSameTrack(m1, m_clusterTrackMap.find(point2.clusterId1())))	return false;
    if (!point2.isPixel()
	&& !onSameTrack(m1,m_clusterTrackMap.find(point2.clusterId2())))	return false;
    if (!point1.isPixel()
	&& !onSameTrack(m1,m_clusterTrackMap.find(point1.clusterId2())))	return false;

    return true;    
}

void
AssignedHits::print (void) const
{ 
    std::cout << " m_clusterTrackMap.size() " << m_clusterTrackMap.size() << std::endl;
    for (map_citerator m = m_clusterTrackMap.begin();
	 m != m_clusterTrackMap.end();
	 ++m)
    {
        std::cout << " identifier " << (*m).first
 		  << "   #tracks " << (*m).second->size() << std::endl;
    }
}

void
AssignedHits::removeTrack (const Track* track)
{
    if (track == 0) return;

    for (hit_citerator h = track->hit_list_begin();
	 h != track->hit_list_end();
	 ++h)
    {
	if ((**h).isPositionMeasurement())
	{
	    Identifier index = (**h).identifier();
	    track_iterator t = std::find(m_clusterTrackMap[index]->begin(),
					 m_clusterTrackMap[index]->end(),
					 track);
	    m_clusterTrackMap[index]->erase(t);
	}
    }
}
		
void
AssignedHits::setShared (hit_citerator hitListBegin,
			 hit_citerator hitListEnd) const
{
    for (hit_citerator h = hitListBegin;
	 h != hitListEnd;
	 ++h)
    {
	if ((**h).isPositionMeasurement())
	{
	    map_citerator  m =  m_clusterTrackMap.find((**h).identifier());
	    // assert(m != m_clusterTrackMap.end());
	    track_citerator t = (*m).second->begin();
	    // shared hit: set broad errors and (temporarily) remove associated ROT
	    if (++t != (*m).second->end())
	    {
		(**h).setShared();
		if ((**h).status() == pixel_cluster)
		{
		    (**h).status(broad_pixel);
		}
		else if ((**h).status() == strip_cluster)
		{
		    (**h).status(broad_strip);
		}
		else if ((**h).status() == drift_time)
		{
		    (**h).status(drift_wire);
		}
	    }
	}
    }
}

bool
AssignedHits::sharesWithPrimary (const Track& track) const
{
    for (hit_citerator h = track.hit_list_begin();
	 h != track.hit_list_end();
	 ++h)
    {
	if ((**h).isCluster()
	    && assignedToPrimary((**h).identifier())) return true;
    }
    return false;
}

int
AssignedHits::uniquenessScore (const Track& track) const
{
    int score = 0;
    for (hit_citerator	h = track.hit_list_begin();
	 h != track.hit_list_end();
	 ++h)
    {
	if (! (**h).isCluster()) continue;
	map_citerator m = m_clusterTrackMap.find((**h).identifier());
	if (m != m_clusterTrackMap.end() && (*m).second->size() > 1)
	{
	    --score;
	}
	else
	{
	    ++score;
	}
    }
    return score;
}

const Track*
AssignedHits::withSharedFinalHits (const Track& track) const
{
    int		       	maxSharedHits	= 4;
    int		       	minUniqueHits	= 7;
    int		       	sharedHits	= 0;
    int		       	uniqueHits	= 0;
    track_list		ambiguousTracks;
    hit_citerator	h		= track.hit_list_end();
    do
    {
	--h;
	if ((**h).isCluster())
	{
	    bool		shared_hit	= false;
	    map_citerator	m		= m_clusterTrackMap.find((**h).identifier());
	    if (m != m_clusterTrackMap.end())
	    {
		for (track_citerator t = (*m).second->begin();
		     t != (*m).second->end();
		     ++t)
		{
		    shared_hit = true;
		    ambiguousTracks.push_back(*t);
		}
	    }
	    
	    if (shared_hit)
	    {
		++sharedHits;
	    }
	    else
	    {
		++uniqueHits;
	    }
	}
    } while (   uniqueHits <  minUniqueHits
	     && sharedHits <= maxSharedHits
	     && h != track.hit_list_begin());

    const Track* ambiguousTrack = 0;
    if (uniqueHits < minUniqueHits || sharedHits > maxSharedHits) 
    {
	int	maxShared = 2;	 
	for (track_iterator t = ambiguousTracks.begin();
	     t != ambiguousTracks.end();
	     ++t)
	{
	    if (*t != ambiguousTrack)
	    {
		int shared = std::count(t,ambiguousTracks.end(),*t);
		if (shared > maxShared)
		{
		    maxShared		= shared;
		    ambiguousTrack	= *t;
		}   
	    }
	}
    }
    return ambiguousTrack;
}
