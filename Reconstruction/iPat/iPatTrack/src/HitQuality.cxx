/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 assess type and distribution of hits in list<HitOnTrack*>
 ---------------------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: HitQuality.cxx,v 1.14 2003-06-11 14:40:25 pop Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "iPatTrack/HitQuality.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
HitQuality::set_quality (hit_citerator hit_list_begin,
			 hit_citerator hit_list_end)
{
    std::list<int>	missing_layers;
    bool       		missing_layer	= false;
    bool       		new_layer	= true;
    for (hit_citerator h = hit_list_begin;
	 h != hit_list_end;
	 ++h)
    {
	switch ((**h).status())
	{
	case scatterer:
	    new_layer		= true;
	    if (missing_layer) missing_layers.push_back(m_number_layers);
	    break;
	case pixel_cluster:
	case broad_pixel:
	case strip_cluster:    
	case broad_strip:
	    ++m_number_planar_clusters;
	    if (new_layer) ++m_number_layers;
	    new_layer		= false;
	    missing_layer	= false;
	    break;
	case drift_time:    
	case drift_wire:
	    ++m_number_drift_hits;
	    break;
	case hole:
	case pixel_reject:
	case strip_reject:
	    ++m_number_planar_holes;
	    if (new_layer) missing_layer = true;
	    break;
	case dead:
	    ++m_number_dead;
	    break;
	default:
	    break;
	};
    }

    // set continuity at each end of track (if there are missing layers)
    m_missing_layers = missing_layers.size();
    if (m_missing_layers > 1)
    {
	std::list<int>::iterator m = missing_layers.begin();
	if (*(++m) < 2) m_continuity_inwards = false;
	std::list<int>::reverse_iterator r = missing_layers.rbegin();
	if (*(++r) > m_number_layers - 2) m_continuity_outwards = false;
    }
}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

void
HitQuality::print (void) const
{
    std::cout << " HitQuality: "
	      << "   in/out continuity "       	<< m_continuity_inwards
	      << "/" 				<< m_continuity_outwards
	      << "   layers " 			<< m_number_layers
	      << " (missing " 			<< m_missing_layers
	      << ")   planar " 			<< m_number_planar_clusters
	      << " (holes " 			<< m_number_planar_holes
	      << ")   drift_hits " 		<< m_number_drift_hits
	      << std::endl;
}
