/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 tentative association of a SiCluster to a predicted hit (see class 
 LayerPrediction).
 This is a transient object formed during track-following in a layer
 ***************************************************************************/

//<doc><file>	$Id: LayerAssociation.cxx,v 1.10 2004-03-02 13:34:15 pop Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "iPatTrackFollower/LayerAssociation.h"

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

double
LayerAssociation::closest_approach (LayerAssociation* reference_hit) const
{
    // evaluate closest_approach to cluster perimeter
    double closest_approach;
    if (reference_hit == 0)
    {
	closest_approach = std::abs(m_phi_offset) - m_tolerance - 0.5*m_cluster->width().phiR();
	if (m_is_pixel)
	{
	    double eta_approach	= m_eta_scale*(std::abs(m_eta_offset) - m_tolerance -
					       0.5*m_cluster->width().z());
	    // add components when same side of perimeter, otherwise take outside component
	    if (closest_approach*eta_approach > 0.)
	    {
		closest_approach += eta_approach;
	    }
	    else if (eta_approach > closest_approach)
	    {
		closest_approach = eta_approach;
	    }
	}
    }
    else
    {
	// update from eta separation
	double offset = m_phi_offset - reference_hit->m_phi_offset;
	if (!m_is_pixel && reference_hit->m_eta_offset != 0.)
	    offset += reference_hit->m_phi_offset *
		      std::abs(m_eta_offset - reference_hit->m_eta_offset);

	// demand cluster overlap within some tolerance
	closest_approach = std::abs(offset) - m_tolerance - 0.5*m_cluster->width().phiR() -
			   0.5*reference_hit->m_cluster->width().phiR();
	if (closest_approach > 0.) closest_approach = 99999.;
	if (m_is_pixel)
	{
	    double eta_approach = m_eta_scale *
				  (std::abs(m_eta_offset - reference_hit->m_eta_offset) -
				   m_tolerance - 0.5*m_cluster->width().z() -
				   0.5*reference_hit->m_cluster->width().z());
	    if (eta_approach > 0.)
	    {
		closest_approach = 99999.;
	    }
	    else
	    {
		closest_approach += eta_approach;
	    }
	}
    }
    return closest_approach;
}

void
LayerAssociation::print(void) const
{
    LayerAssociation* null_approach = 0;
    std::cout << " LayerAssociation: " << m_status << std::setiosflags(std::ios::fixed)
	      << std::setw(6) << m_cluster->identify()
	      << "  r"
	      << std::setw(7) << std::setprecision(2) << m_cluster->globalPosition().perp()
	      << "  z"
	      << std::setw(9) << std::setprecision(2) << m_cluster->globalPosition().z()
	      << "  eta,phi_offset"
	      << std::setw(7) << std::setprecision(2) << m_eta_offset
	      << std::setw(7) << std::setprecision(2) << m_phi_offset
	      << "  closest_approach"
	      << std::setw(8) << std::setprecision(2) << closest_approach(null_approach)
	      << std::endl;
}


