/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 detailed statistics of hits associated to track
 (including comparison with truth - kine indices)
 -----------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cassert>
#include <iomanip>
#include <iostream>
#include "iPatTrack/HitList.h"
#include "iPatTrack/HitStatistics.h"
#include "iPatTrack/HitStatus.h"
#include "iPatTrack/Track.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

HitStatistics::HitStatistics (const Track &track)
    :	m_number_boundary		(0),
	m_number_dead			(0),
	m_number_drift_rejects		(0),
	m_number_drift_wires		(0),
	m_number_inactive	       	(0),
	m_number_pending		(0),
	m_number_planar_holes		(0),
	m_number_tr_hits		(0),
	m_pattern			(0),
	m_shared_b_layer_clusters	(0),
	m_shared_drift_hits		(0),
	m_shared_pixel_clusters		(0),
	m_shared_strip_clusters		(0),
	m_spoilt_b_layer_clusters	(0), 
	m_spoilt_drift_hits		(0),
	m_spoilt_pixel_clusters		(0),
	m_spoilt_strip_clusters		(0),
	m_unique_b_layer_clusters	(0),
	m_unique_drift_hits		(0),
	m_unique_pixel_clusters		(0),
	m_unique_strip_clusters		(0),
	m_wrong_b_layer_clusters	(0),
	m_wrong_drift_hits		(0),
	m_wrong_pixel_clusters		(0),
	m_wrong_strip_clusters		(0)
{
    // increment appropriate counters for each hit
//     int track_kine_index	= 0;
    for (HitList::hit_citerator h = track.hit_list_begin();
	 h != track.hit_list_end();
	 ++h)
    {
	if ((**h).isPositionMeasurement())
	{
// 	    if ((**h).isCluster())  // pattern word
// 	    {
// 		int row = (**h).geometry_row();
// 		int bit = 0001;
// 		bit <<= (igeom.G_Super[row]-1);
// 		m_pattern |= bit;
// 	    }
	    if ((**h).aboveTrThreshold()) ++m_number_tr_hits;
	    if ((**h).isShared())					// shared hit
	    {
		switch ((**h).status())    
		{
		case pixel_cluster:
		case broad_pixel:   
		    if (in_b_layer(**h)) ++m_shared_b_layer_clusters;
		    ++m_shared_pixel_clusters;
		    break;
		case strip_cluster:    
		case broad_strip:
		    ++m_shared_strip_clusters; 
		    break;
		case drift_time:
		case drift_wire:
		    ++m_shared_drift_hits;
		    break;
		case calorimeter:
		    break;
		default:
		    std::cout << " HitStatistics: (1) unexpected HitStatus "
			      << (**h).status() << std::endl;
		    assert(false);
		    break;
		};
	    }
// 	    cout << " track_kine_index " << track_kine_index
// 		 << "   kine_index " << (**h).kine_index() << endl;

// 	    if ((**h).kine_index() == 0)   // spoilt hit
// 	    {
// 		switch ((**h).status())    
// 		{
// 		case pixel_cluster:
// 		case broad_pixel:		    
// 		    if (in_b_layer(**h)) ++m_spoilt_b_layer_clusters;
// 		    ++m_spoilt_pixel_clusters;
// 		    break;
// 		case strip_cluster:    
// 		case broad_strip:		    
// 		    ++m_spoilt_strip_clusters;
// 		    break;
// 		case drift_time:
// 		    ++m_spoilt_drift_hits;
// 		    break;
// 		case drift_wire:
// 		    ++m_number_drift_wires;
// 		    ++m_spoilt_drift_hits;
// 		    break;
// 		case calorimeter:
// 		    break;    
// 		default:
// 		    std::cout << " HitStatistics: (2) unexpected HitStatus "
// 			      << (**h).status() << std::endl;
// 		    assert(false);
// 		    break;
// 		};
// 	    }
// 	    else if ((**h).kine_index() == track_kine_index)		// unique hit
// 	    {
// 		switch ((**h).status())    
// 		{
// 		case pixel_cluster:
// 		case broad_pixel:		    
// 		    if (in_b_layer(**h)) ++m_unique_b_layer_clusters;
// 		    ++m_unique_pixel_clusters;
// 		    break;
// 		case strip_cluster:    
// 		case broad_strip:		    
// 		    ++m_unique_strip_clusters;
// 		    break;
// 		case drift_time:
// 		    ++m_unique_drift_hits;
// 		    break;
// 		case drift_wire:
// 		    ++m_number_drift_wires;
// 		    ++m_unique_drift_hits;
// 		    break;
// 		case calorimeter:
// 		    break;    
// 		default:
// 		    std::cout << " HitStatistics: (3) unexpected HitStatus "
// 			      << (**h).status() << std::endl;
// 		    assert(false);
// 		    break;
// 		};
// 	    }
// 	    else   // wrong hit
// 	    {
// 		switch ((**h).status())    
// 		{
// 		case pixel_cluster:
// 		case broad_pixel:		    
// 		    if (in_b_layer(**h)) ++m_wrong_b_layer_clusters;
// 		    ++m_wrong_pixel_clusters;
// 		    break;
// 		case strip_cluster:    
// 		case broad_strip:		    
// 		    ++m_wrong_strip_clusters;
// 		    break;
// 		case drift_time:
// 		    ++m_wrong_drift_hits;
// 		    break;
// 		case drift_wire:
// 		    ++m_number_drift_wires;
// 		    ++m_wrong_drift_hits;
// 		    break;
// 		case calorimeter:
// 		    break;    
// 		default:
// 		    std::cout << " HitStatistics: (4) unexpected HitStatus "
// 			      << (**h).status() << std::endl;
// 		    assert(false);
// 		    break;
// 		};
// 	    }
	}
	else
	{
	    switch ((**h).status())    
	    {
	    case pending:
		++m_number_pending;
		break;
	    case outside:
	    case brem:	
	    case scatterer:	
		break;
	    case hole:
	    case pixel_reject:
	    case strip_reject:			
		++m_number_planar_holes;
		break;
	    case drift_reject:
	    case rejected:
		++m_number_drift_rejects;
		break;
	    case calorimeter:
	    case muon_parameters:    
	    case inactive:
		++m_number_inactive;
		break;
	    case dead:
		++m_number_dead;
		break;
	    case boundary:
		++m_number_boundary;
		break;
	    default:
		std::cout << " HitStatistics: (5) unexpected HitStatus "
			  << (**h).status() << std::endl;
		assert(false);
		break;
	    };
	}
    }
//     cout << " HitStatistics created " << endl;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

bool
HitStatistics::in_b_layer (const HitOnTrack& hit) const
{ 
    // hardwire max radius for any b-layer so far ...
    if (hit.isBarrel()
	&& hit.isPixel()
	&& hit.position().perp() < 7.0)
    {
	return true;
    }
    else
    {
	return false;
    }
 } 

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

void    
HitStatistics::print (void) const
{
    std::cout << std::setiosflags(std::ios::fixed);
    std::cout << "  HitStatistics : total :   unique   spoilt   wrong   shared"
	      << std::setw(9)
	      << number_planar_holes()	<< " planar_holes    "
	      << number_drift_rejects()	<< " drift_rejects    "
	      << number_dead() 		<< " dead     "
	      << std::endl;
    std::cout << "        b_layer : "
	      << std::setw(5) << number_b_layer_clusters() << " :"
	      << std::setw(9) << m_unique_b_layer_clusters
	      << std::setw(9) << m_spoilt_b_layer_clusters
	      << std::setw(8) << m_wrong_b_layer_clusters
	      << std::setw(9) << m_shared_b_layer_clusters << std::endl;
    std::cout << "   outer_pixels : "
	      << std::setw(5) << number_outer_pixel_clusters() << " :"
	      << std::setw(9) << unique_outer_pixel_clusters()
	      << std::setw(9) << spoilt_outer_pixel_clusters()
	      << std::setw(8) << wrong_outer_pixel_clusters()
	      << std::setw(9) << shared_outer_pixel_clusters() << std::endl;
    std::cout << "         strips : "
	      << std::setw(5) << number_strip_clusters() << " :"
	      << std::setw(9) << m_unique_strip_clusters
	      << std::setw(9) << m_spoilt_strip_clusters
	      << std::setw(8) << m_wrong_strip_clusters
	      << std::setw(9) << m_shared_strip_clusters << std::endl;
    std::cout << "          drift : "
	      << std::setw(5) << number_drift_hits() << " :"
	      << std::setw(9) << m_unique_drift_hits
	      << std::setw(9) << m_spoilt_drift_hits
	      << std::setw(8) << m_wrong_drift_hits
	      << std::setw(9) << m_shared_drift_hits
	      << std::setw(9) << m_number_drift_wires << " wires "
	      << std::setw(3) << m_number_tr_hits << " tr_hits   ";
    int maxbit	= CHAR_BIT * sizeof (m_pattern);
    // int pattern	= m_pattern;
    for (int j = 0; j < maxbit; ++j)
    {
	std::cout << bits ((const char *) &m_pattern, maxbit, j, 1);
    }
    std::cout << " pattern " << std::resetiosflags(std::ios::fixed) << std::endl;
}


