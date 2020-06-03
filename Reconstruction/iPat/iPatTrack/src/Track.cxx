/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 base Track data class
 ------------------------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include "TrkTrack/Track.h"
#include "iPatTrack/Track.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

Track::Track (void)
    :	m_status		(undefined),
	m_perigeeParameters   	(0),
	m_fitQuality	       	(0),
	m_hitQuality	       	(0),
	m_trkTrack		(0),
	m_truthAssociation	(0)
{}

Track::Track (TrackStatus		status,
	      hit_list* 		hits,
	      parameter_vector*	       	parameters,	
	      PerigeeParameters* 	perigee,
	      FitQuality*		fitQuality,
	      HitQuality* 		hitQuality)
    :	m_status		(status),
	m_hits			(*hits),
	m_parameter_vector	(*parameters),
	m_perigeeParameters   	(perigee),
	m_fitQuality	       	(fitQuality),
	m_hitQuality	       	(hitQuality),
	m_trkTrack		(0),
	m_truthAssociation	(0)
{
    // this is necessary to transfer ownership without creating a mem leak
    delete hits;
    delete parameters;
}

Track::Track (hit_list* 		hits,
	      parameter_vector*	       	parameters,	
	      PerigeeParameters* 	perigee,
	      FitQuality*		fitQuality,
	      HitQuality* 		hitQuality)
    :	m_status		(undefined),
	m_hits			(*hits),
	m_parameter_vector	(*parameters),
	m_perigeeParameters   	(perigee),
	m_fitQuality	       	(fitQuality),
	m_hitQuality	       	(hitQuality),
	m_trkTrack		(0),
	m_truthAssociation	(0)
{}

Track::Track (const Track& track)
     :	m_status			(track.m_status),
	m_perigeeParameters		(0),
	m_fitQuality			(0),
	m_hitQuality			(0),
	m_trkTrack			(track.m_trkTrack),
	m_truthAssociation		(0)
{
    // copy constructor
    
    // cout << " Track copy constructor " << endl;
    // print_parameters();

    for (hit_const_iterator h = track.hit_list_begin(); h != track.hit_list_end(); ++h)
    {
	m_hits.push_back(new HitOnTrack(**h));
    }

    if (track.hasFitQuality())
	m_fitQuality 		= new FitQuality(*track.m_fitQuality);
    if (track.hasHitQuality())
	m_hitQuality 		= new HitQuality(*track.m_hitQuality);
    if (track.has_perigee_parameters())
	m_perigeeParameters 	= new PerigeeParameters(*track.m_perigeeParameters);
    if (track.hasTruthAssociation())
	m_truthAssociation 	= new TruthAssociation(*track.m_truthAssociation);
    for (parameter_const_iterator p = track.m_parameter_vector.begin();
	 p != track.m_parameter_vector.end();
	 ++p)
	m_parameter_vector.push_back(new ScattererParameters(**p));
    
    //cout << " end copy constructor " << endl;
}

Track::~Track (void)
{
    // to avoid unnecessary copying, member data is stored using pointers
    // => needs explicit delete to avoid memory leaks
    for (hit_iterator h = m_hits.begin();
	 h != m_hits.end();
	 ++h)
	delete *h;

    m_hits.clear();    
    delete m_fitQuality;
    delete m_hitQuality;
    delete m_perigeeParameters;
    delete m_truthAssociation;
    
    for (parameter_iterator p = m_parameter_vector.begin();
	 p != m_parameter_vector.end();
	 ++p)
	delete *p;
}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

void
Track::print (void) const
{
    std::cout << m_status << " hits " 	<< m_hits.size();
    if (m_hitQuality != 0)
    {
	std::cout << "  drift_hits " 	<< m_hitQuality->number_drift_hits()
		  << "  layers "       	<< m_hitQuality->number_layers()
		  << "  (missing  " 	<< m_hitQuality->missing_layers()
		  << ")  planar "      	<< m_hitQuality->number_planar_clusters()
		  << "  holes "        	<< m_hitQuality->number_planar_holes();
    }
    if (m_fitQuality != 0)
    {
	std::cout << std::setiosflags(std::ios::fixed)
		  << "  chisq/ndf" 	<< std::setw(6) << std::setprecision(2)
		  << m_fitQuality->chi_squared()
		  << "/" 		<< std::setw(2)
		  << m_fitQuality->degrees_of_freedom()
		  << "  prob" 		<< std::setw(5) << std::setprecision(2)
		  << m_fitQuality->fit_probability();
    }
    std::cout << std::endl;
}

void
Track::print_hits (void) const
{
    for (hit_const_iterator h = hit_list_begin();
	 h != hit_list_end();
	 ++h)
	(*h)->print();

    std::cout << std::endl;
}

void
Track::print_parameters (void) const
{
    std::cout << std::setiosflags(std::ios::fixed) << m_status;
    if (m_truthAssociation == 0)
    {
	std::cout << "       ";
    }
    else
    {
	std::cout << std::setw(7) << m_truthAssociation->barcode();
    }
    std::cout << std::resetiosflags(std::ios::fixed);
    if (m_perigeeParameters != 0) m_perigeeParameters->print();
    std::cout  << std::setiosflags(std::ios::fixed);
    if (m_fitQuality == 0)
    {
	std::cout << "       ";
    }
    else
    {
	int prec	= std::cout.precision();
	std::cout << "  prob" << std::setw(6) << std::setprecision(3)
		  << m_fitQuality->fit_probability();
	std::cout.precision(prec);
    }
    if (m_hitQuality != 0)
    {
	std::cout	<< "  parts " 	<< m_hitQuality->number_effective_layers()
			<< "/" 		<< m_hitQuality->number_layers() 
			<< "  planar " 	<< std::setw(2) << m_hitQuality->number_planar_clusters()
			<< " (" 		<< m_hitQuality->number_planar_holes() << ")" 
			<< "  drift " 	<< std::setw(2) << m_hitQuality->number_drift_hits();
    }
    std::cout << std::resetiosflags(std::ios::fixed) << std::endl;
}

void
Track::scattererParameters (parameter_vector* parameters) 
{
    // clear any existing parameter_vector
    for (parameter_iterator p = m_parameter_vector.begin();
	 p != m_parameter_vector.end();
	 ++p)
	delete *p;
    m_parameter_vector.clear();

    // transfer ownership of the new one: assign and delete
    m_parameter_vector = *parameters;
    delete parameters;
}

Trk::Track*
Track::trkTrack (void) const
{ return m_trkTrack; }

void
Track::trkTrack (Trk::Track* track)
{
    delete m_trkTrack;
    m_trkTrack = track;
}
