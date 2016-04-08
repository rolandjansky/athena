/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackManager.cxx, (c) ATLAS iPatRec
//
// manage the tracks and track-candidates found during track finding
//
///////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include "iPatInterfaces/IAssignedHits.h"
#include "iPatInterfaces/IDetectorSelection.h"
#include "iPatInterfaces/ITrackBuilder.h"
#include "iPatInterfaces/ITrackFollower.h"
#include "iPatInterfaces/ITrackQuality.h"
#include "iPatTrack/Track.h"
#include "iPatTrackFinder/TrackManager.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TrackManager::TrackManager (const std::string&	type,
			    const std::string&	name,
			    const IInterface*	parent)
    :   AthAlgTool		(type, name, parent),
	m_assignedHits		("AssignedHits/AssignedHits"),
	m_detectorSelection	("DetectorSelection/DetectorSelection"),
	m_trackBuilder		("TrackBuilder/TrackBuilder"),
	m_trackFollower		("TrackFollower/TrackFollower"),
	m_trackQuality		("TrackQuality/TrackQuality"),
	m_primaryTrack		(0),
	m_secondaryTrack	(0),
	m_trackList		(0),
	m_truncatedTrack	(0)
{
    declareInterface<ITrackManager>(this);
    declareProperty("AssignedHits",		m_assignedHits);
    declareProperty("DetectorSelection",	m_detectorSelection);
    declareProperty("TrackBuilder",		m_trackBuilder);
    declareProperty("TrackFollower",		m_trackFollower);
    declareProperty("TrackQuality",		m_trackQuality);
}

TrackManager::~TrackManager (void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TrackManager::initialize()
{
    ATH_MSG_INFO( "TrackManager::initialize()" );

    // get the Tools
    if (m_assignedHits.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_assignedHits );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_assignedHits );
    }
    if (m_detectorSelection.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_detectorSelection );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_detectorSelection );
    }
    if (m_trackBuilder.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackBuilder );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackBuilder );
    }
    if (m_trackFollower.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackFollower );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackFollower );
    }
    if (m_trackQuality.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackQuality );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackQuality );
    }

    return StatusCode::SUCCESS;
}

StatusCode
TrackManager::finalize()
{
    MsgStream log(msgSvc(), name());
    ATH_MSG_DEBUG( "TrackManager::finalize()" );

    clearTrackList();
    delete m_trackList;

    return StatusCode::SUCCESS; 
}

void
TrackManager::addCurrentSecondary (Track* track)
{
    // Firstly, discard any track with no unique hit association.
    // Secondly, attempt track extrapolation to the end of the silicon.
    if (!track || m_assignedHits->allHitsShared(*track))
    {
        delete track;
	return;
    }
  
    // extrapolate (including TRT with re-fit)
    Track* extrapolatedTrack	= m_trackFollower->extrapolate_outwards(*track);
    if (!extrapolatedTrack)
    {
        delete track;
	return;
    }
    
    if (m_detectorSelection->processTRT() && extrapolatedTrack->status() == secondary)
    {
	Track* trtTrack = m_trackFollower->associate_trt(*extrapolatedTrack);
	if (trtTrack)
	{
	    delete extrapolatedTrack;
	    if (trtTrack->status() == undefined)	// TRT veto
	    {
		extrapolatedTrack  = 0;
	    }
	    else
	    {
		extrapolatedTrack = m_trackBuilder->trackRefit(*trtTrack);
	    }
	    delete trtTrack;
	}
    }

    // select as secondaryTrack if "isBetterThan" existing secondary	    
    if (extrapolatedTrack)
    {
	if (m_secondaryTrack == 0)
	{
	    m_assignedHits->includeTrack(extrapolatedTrack);
	    m_secondaryTrack 	= extrapolatedTrack;
	}
	else if (isFirstTrackBetter(*extrapolatedTrack,*m_secondaryTrack))
	{
	    m_assignedHits->removeTrack(m_secondaryTrack);
	    delete m_secondaryTrack;
	    m_assignedHits->includeTrack(extrapolatedTrack);
	    m_secondaryTrack 	= extrapolatedTrack;
	}

	if (extrapolatedTrack != m_secondaryTrack) delete extrapolatedTrack;
    }
    delete track;
}

void
TrackManager::addCurrentSegment (Track* track)
{
    if (m_primaryTrack == 0)
    {
	m_assignedHits->includeTrack(track);
	m_primaryTrack = track;
    }
    else if (isFirstTrackBetter(*track,*m_primaryTrack))
    {
	m_assignedHits->removeTrack(m_primaryTrack);
	delete m_primaryTrack;
	m_assignedHits->includeTrack(track);
	m_primaryTrack = track;
    }
    else
    {
	delete track;
    }
}

void
TrackManager::addCurrentTrack (Track* track)
{
    // Firstly, discard any track with no unique hit association.
    // Secondly, attempt track extrapolation to the end of the silicon.
    //   If successful, the extrapolation may become m_primary and/or m_truncated.
    //   If both: m_truncated will be a copy, if neither: it is deleted or kept as a secondary candidate.
    // Thirdly, the original track may still be the preferred choice for m_truncated,
    //   otherwise it will be deleted.

    if (track == 0 || m_assignedHits->allHitsShared(*track))
    {
        delete track;
	return;
    }

    Track* extrapolatedTrack	= m_trackFollower->extrapolate_outwards(*track);
    Track* truncatedTrack	= 0;
    
    // select extrapolatedTrack as primaryTrack if "isBetterThan" existing primary
    if (extrapolatedTrack != 0)
    {
	if (m_primaryTrack == 0)
	{
	    m_assignedHits->includeTrack(extrapolatedTrack);
	    m_primaryTrack 	= extrapolatedTrack;
	}
	else if (isFirstTrackBetter(*extrapolatedTrack,*m_primaryTrack))
	{
	    m_assignedHits->removeTrack(m_primaryTrack);
	    delete m_primaryTrack;
	    m_assignedHits->includeTrack(extrapolatedTrack);
	    m_primaryTrack 	= extrapolatedTrack;
	}

	// candidate truncatedTrack according to priority/quality
	// a minimum status of long_segment is required (== hit in first or second layer)
	if (extrapolatedTrack->status() == secondary)
	{
	    if (extrapolatedTrack != m_primaryTrack) delete extrapolatedTrack;
	    delete track;
	    return;
	}
	else if (m_trackQuality->goodTrack(truncated,
					   extrapolatedTrack->fitQuality(),
					   extrapolatedTrack->hitQuality()))
	{
	    truncatedTrack = extrapolatedTrack;
	    delete track;
	}
	else if (track->status() > segment
		 && m_trackQuality->goodTrack(truncated,
					      track->fitQuality(),
					      track->hitQuality()))
	{
	    truncatedTrack = track;
	    if (extrapolatedTrack != m_primaryTrack) delete extrapolatedTrack;
	}
	else
	{
	    if (extrapolatedTrack != m_primaryTrack) delete extrapolatedTrack;
	    delete track;
	    return;
	}
    }
    else if (track->status() > segment
	     && m_trackQuality->goodTrack(truncated,
					  track->fitQuality(),
					  track->hitQuality()))
    {
	truncatedTrack = track;
    }
    else
    {
	delete track;
	return;
    }

    // select truncatedTrack if "isBetterThan" existing truncated
    if (m_truncatedTrack == 0)
    {
	if (truncatedTrack != m_primaryTrack)
	{
	    truncatedTrack->status(truncated);
	    m_truncatedTrack = truncatedTrack;
	    m_assignedHits->includeTrack(m_truncatedTrack);
	}
    }
    else if (isFirstTrackBetter(*truncatedTrack,*m_truncatedTrack))
    {
	m_assignedHits->removeTrack(m_truncatedTrack);
	delete m_truncatedTrack;
	if (truncatedTrack == m_primaryTrack)
	{
	    m_truncatedTrack = 0;
	}
	else
	{
	    truncatedTrack->status(truncated);
	    m_truncatedTrack = truncatedTrack;
	    m_assignedHits->includeTrack(m_truncatedTrack);
	}
    }
    else
    {
	if (truncatedTrack != m_primaryTrack) delete truncatedTrack;
    }
}

void
TrackManager::clearTrackList (void)
{
    if (! m_trackList) return;

    while (m_trackList->size())
    {
	delete *m_trackList->rbegin();
	m_trackList->pop_back();
    }
    m_assignedHits->clear();
}

void
TrackManager::finalizeTrackFitting (void)
{
    // remove any tracks below pt threshold
    int			remove = 0;
    track_list		pendingList;
    std::vector<bool>	refit;
    for (track_iterator t = m_trackList->begin();
         t != m_trackList->end();
         ++t)
    {
	pendingList.push_back(*t);
	refit.push_back(false);
	if (m_trackQuality->aboveMinPt((**t).perigee_parameters())) continue;
	++remove;
	delete *t;
	*t = 0;
    }
    
    // process pending TRT associations
    std::vector<bool>::iterator r = refit.begin();
    if (m_detectorSelection->processTRT())
    {
	for (track_iterator t = m_trackList->begin();
	     t != m_trackList->end();
	     ++t, ++r)
	{
  	    if (*t == 0) continue;
	    if ((**t).status() == pendingTRT)
	    {
		Track* trtTrack = m_trackFollower->associate_trt(**t);
		if (!trtTrack)					// no TRT
		{
		    if ((**t).status() == pendingTRT) (**t).status(primary);
		}
		else if (trtTrack->status() == undefined)	// veto
		{
		    delete trtTrack;
		    if ((**t).status() == pendingTRT) (**t).status(truncated);
		}
		else						// confirmation
		{
		    m_assignedHits->removeTrack(*t);
		    *t = trtTrack;
		    m_assignedHits->includeTrack(*t);
		    *r = true;
		}
	    }
	}
    }
    
    // set shared hits and refit according to status
    track_iterator p = pendingList.begin();
    r = refit.begin();
    for (track_iterator t = m_trackList->begin();
         t != m_trackList->end();
         ++t, ++p, ++r)
    {
	if (*t == 0) continue;
	m_assignedHits->setShared((**t).hit_list_begin(),(**t).hit_list_end());
	HitList::hit_citerator h = (**t).hit_list_begin();
	while (!*r && h != (**t).hit_list_end())
	{
	    if ((**h).isShared()) *r = true;
	    ++h;
	}
	if (*r)
	{
	    Track* trackRefit = m_trackBuilder->trackRefit(**t);
	    if (trackRefit != 0)
	    {
		if (*p != *t) delete *p;
		delete *t;
		*t = trackRefit;
	    }
	    else 
	    {
// 		std::cout << " refit fails: #straws "
// 			  << (**t).hitQuality().number_drift_hits()
// 			  << (**t).status() << std::endl;
		if (*p != *t) delete *t;
		*t = *p;
		if ((**t).status() == pendingTRT) (**t).status(truncated);
	    }
	}
    }
    
    // remove deleted tracks
    while (remove-- > 0)
    {
	Track* null = 0;
	track_iterator t = std::find(m_trackList->begin(),m_trackList->end(),null);
	m_trackList->erase(t);
    }
}

void
TrackManager::newTrackSearch (void)
{
    clearTrackList();
    delete m_trackList;
    m_trackList = new track_list;
}

void
TrackManager::saveCurrentTracks (void)
{
    // take current best primary:
    //   1) demand at least one unique hit
    //   2) try to associate to trt if it has a truncated alternative
    //   3) if OK, remove any truncated and secondary subset track copies
    //   4) then save - subject to ambiguity status
    if (m_truncatedTrack) m_assignedHits->removeTrack(m_truncatedTrack);
    if (m_primaryTrack)
    {
	m_assignedHits->removeTrack(m_primaryTrack);
	if (m_primaryTrack->status() == pendingTRT)
	{
	    if (m_detectorSelection->processTRT())
	    {
		HitQuality hitQuality;
	        if (m_truncatedTrack != 0
		    || !m_trackQuality->sufficientHits (truncated,
							hitQuality,
							m_primaryTrack->hit_list_begin(),
							m_primaryTrack->hit_list_end()))
		{
		    Track* trtTrack = m_trackFollower->associate_trt(*m_primaryTrack);
		    if (!trtTrack)					// no TRT
		    {
		        m_primaryTrack->status(primary);
		    }
		    else if (trtTrack->status() == undefined)		// veto
		    {
		        delete trtTrack;
			trtTrack	= 0;
			delete m_primaryTrack;
			m_primaryTrack	= 0;
		    }
		    else						// confirmation
		    {
			Track* trackRefit = m_trackBuilder->trackRefit(*trtTrack);
			if (trackRefit != 0)
			{
			    delete trtTrack;
			    trtTrack	= trackRefit;
			}
			else
			{
			    // 		    std::cout << " TrackManager   trt QC fail " 
			    // 			      << trtTrack->status() << std::endl;
			    delete trtTrack;
			    trtTrack	= 0;
			}
			delete m_primaryTrack;
			m_primaryTrack	= trtTrack; 
		    }
		}
	    }
	    else
	    {
		m_primaryTrack->status(primary);
	    }
	}
	else if (m_primaryTrack->status() == secondary && m_detectorSelection->processTRT())
	{
	    Track* trtTrack = m_trackFollower->associate_trt(*m_primaryTrack);
	    if (trtTrack)
	    {
		Track* trackRefit = 0;
		if (trtTrack->status() == secondary)
		    trackRefit = m_trackBuilder->trackRefit(*trtTrack);
		delete trtTrack;
		delete m_primaryTrack;
		m_primaryTrack  = trackRefit;
	    }
	}
    }
	
    if (m_primaryTrack)
    {
	delete m_secondaryTrack;
	m_secondaryTrack	= 0;
	delete m_truncatedTrack;
	m_truncatedTrack	= 0;
	ambiguityCheck();   
    }
 
    // no primary: then save truncated - subject to ambiguity status
    if (m_truncatedTrack)
    {
	delete m_primaryTrack;
	m_primaryTrack		= m_truncatedTrack;
	m_truncatedTrack	= 0;
	ambiguityCheck();
    }

    // finally if no primary/truncated, save secondary (if it exists)
    if (m_secondaryTrack)
    {
	m_assignedHits->removeTrack(m_secondaryTrack);
	if (m_assignedHits->sharesWithPrimary(*m_secondaryTrack))
	{
	    delete m_secondaryTrack;
	    m_secondaryTrack = 0;
	}
	// back-track secondaries towards beam-axis
	// discard in case of cross-over (sharing with existing primary track)
	else
	{
	    Track* extrapolatedTrack = m_trackFollower->extrapolate_inwards(*m_secondaryTrack);
	    if (extrapolatedTrack
		&& ! m_assignedHits->sharesWithPrimary(*extrapolatedTrack))
	    {
		delete m_secondaryTrack;
		m_secondaryTrack = extrapolatedTrack;
	    }
	    else
	    {
		delete extrapolatedTrack;   
	    }
	}
	m_primaryTrack		= m_secondaryTrack;
	m_secondaryTrack	= 0;
	ambiguityCheck();
    }
}

void
TrackManager::setProperties (int printLevel)
{
    m_printLevel	= printLevel;
}

void
TrackManager::uniquenessCheck (void)
{
    // remove any tracks with a score of zero or worse.
    // Score being the number of uniquely assigned hits minus shared hits.
    // A 2 hit penalty is added for "short" tracks (Truncated/Secondary)
    // to preferentially remove "short" rather than Primary tracks
    int	worstScore;
    do
    {
	worstScore = 1;
	track_iterator worstTrack = m_trackList->end();
	for (track_iterator t = m_trackList->begin();
	     t != m_trackList->end();
	     ++t)
	{
	    int score = m_assignedHits->uniquenessScore(**t);
	    // add penalty
	    if ((**t).status() <= truncated) score -= 2;
	    if (score < worstScore)
	    {
		worstScore = score;
		worstTrack = t;
	    }
	}

	// kill worst track
	if (worstTrack != m_trackList->end())
	{
	    m_assignedHits->removeTrack(*worstTrack);
	    delete *worstTrack;
	    *worstTrack = 0;
	    m_trackList->erase(worstTrack);
	}
    } while (worstScore < 1);
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
TrackManager::ambiguityCheck (void)
{
    if (m_primaryTrack == 0) return;

    // look for ambiguous track.
    // If found, make sure both tracks have same association status wrt trt
    const Track* ambiguousTrack = m_assignedHits->withSharedFinalHits(*m_primaryTrack);
    if (ambiguousTrack && ambiguousTrack->status() != m_primaryTrack->status())
    {
	// associate trt and refit ambiguous track if pending
	if (ambiguousTrack->status() == pendingTRT)
	{
	    m_assignedHits->removeTrack(ambiguousTrack);
	    m_trackList->erase(std::find(m_trackList->begin(),
					 m_trackList->end(),
					 ambiguousTrack));
	    Track* trtTrack	= m_trackFollower->associate_trt(*ambiguousTrack);
	    Track* trackRefit	= 0;
	    if (!trtTrack)					// no TRT
	    {
		trackRefit = new Track(*ambiguousTrack);
		trackRefit->status(primary);
	    }
	    else if (trtTrack->status() != undefined)		// confirmation
	    {
		trackRefit = m_trackBuilder->trackRefit(*trtTrack);
	    }
	    if (m_printLevel > 4)
	    {
		if (trtTrack && trtTrack->status() == undefined)
		{
		    std::cout << " TrackManager::ambiguityCheck: ambiguousTrack has trt veto"
			      << std::endl;
		}
		else if (!trackRefit)
		{
		    std::cout << " TrackManager::ambiguityCheck: ambiguousTrack fails trt refit"
			      << std::endl;
		}
	    }
	    if (trackRefit)
	    {
		m_assignedHits->includeTrack(trackRefit);
		insertInTrackList(trackRefit);
	    }
	    delete ambiguousTrack;
	    delete trtTrack;
	    ambiguousTrack = trackRefit;
	}

	// associate trt and refit current track if pending
	if (m_primaryTrack->status() == pendingTRT)
	{
	    Track* trtTrack = m_trackFollower->associate_trt(*m_primaryTrack);
	    if (!trtTrack)					// no TRT
	    {
		m_primaryTrack->status(primary);
	    }
	    else if (trtTrack->status() == undefined)		// veto
	    {
		if (m_printLevel > 4)
		    std::cout << " TrackManager::ambiguityCheck: current track has trt veto "
			      << std::endl;
		delete trtTrack;
		delete m_primaryTrack;
		m_primaryTrack	= 0;
		return;
	    }
	    else						// confirmation
	    {
		delete m_primaryTrack;
		m_primaryTrack	= 0;
		Track* trackRefit = m_trackBuilder->trackRefit(*trtTrack);
		delete trtTrack;
		if (m_printLevel > 4 && !trackRefit)
		    std::cout << " TrackManager::ambiguityCheck: current track fails trt refit"
			      << std::endl;
		if (!trackRefit) return;
		m_primaryTrack	= trackRefit;
	    }
	}
    }
	
    // print debug before resolving ambiguity
    if (m_printLevel > 4)
    {
	if (ambiguousTrack)
	{
	    bool keepCurrent = true;
	    std::cout << " TrackManager::ambiguityCheck ";
	    if (isFirstTrackBetter(*m_primaryTrack,*ambiguousTrack))
	    {
		std::cout << "   delete ambiguous track " << std::endl << "  keep   ";
	    }
	    else
	    {
		keepCurrent = false;
		std::cout << "   delete current track " << std::endl << "  delete ";
	    }
	    m_primaryTrack->print_parameters();
	    if (m_printLevel > 5)
	    {
		m_primaryTrack->print_hits();
		std::cout << std::endl;
	    }
	    if (keepCurrent)
	    {
		std::cout << "  delete ";
	    }
	    else
	    {
		std::cout << "  keep   ";
	    }
	    ambiguousTrack->print_parameters();
	    if (m_printLevel > 5)
	    {
		ambiguousTrack->print_hits();
		std::cout << std::endl;
	    }
	}
    }

    // resolve any ambiguity
    if (!ambiguousTrack)
    {
	m_assignedHits->includeTrack(m_primaryTrack);
	insertInTrackList(m_primaryTrack);
    }
    else if (isFirstTrackBetter(*m_primaryTrack,*ambiguousTrack))
    {
	m_assignedHits->includeTrack(m_primaryTrack);
	insertInTrackList(m_primaryTrack);
	m_assignedHits->removeTrack(ambiguousTrack);
	m_trackList->erase(std::find(m_trackList->begin(),
				     m_trackList->end(),
				     ambiguousTrack));
	delete ambiguousTrack;
    }
    else
    {
	delete m_primaryTrack;
	m_primaryTrack	= 0;
    }
}

void
TrackManager::insertInTrackList (Track* track)
{
    // order list by abs_pt apart from fast_segments which go at the end
    if (track->status() == fast_segment || m_trackList->empty())
    {
	m_trackList->push_back(track);
	return;
    }

    track_iterator  t	= m_trackList->begin();
    while (t != m_trackList->end()
	   && (**t).status() != fast_segment
	   && (**t).perigee_parameters().abs_pt() > track->perigee_parameters().abs_pt()) ++t;

    m_trackList->insert(t,track);
}

bool
TrackManager::isFirstTrackBetter (const Track& first, const Track& second) const
{
    // track is better if it has a more highly ranked status,
    // else a higher fit probability (subject to a penalty from the number of holes)
    if (first.status() > second.status()
	&& first.fitQuality().fit_probability()  > 0.)  return true;
    if (first.status() < second.status()
	&& second.fitQuality().fit_probability() > 0.)  return false;

    double penalty		= pow(10.,first.hitQuality().number_planar_holes() -
				      second.hitQuality().number_planar_holes());
    double firstEstimator	= first.fitQuality().fit_probability();
    double secondEstimator	= second.fitQuality().fit_probability()*penalty;
    if (firstEstimator >= secondEstimator)
    {
	return true;
    }
    else
    {
	return false;
    }
}

void
TrackManager::setSharedHits (void) const
{
    for (track_citerator t = m_trackList->begin();
	 t != m_trackList->end();
	 ++t)
    {
      	m_assignedHits->setShared((**t).hit_list_begin(),(**t).hit_list_end());
    }
}
