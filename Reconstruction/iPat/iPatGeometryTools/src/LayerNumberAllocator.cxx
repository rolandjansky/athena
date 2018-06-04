/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool describing arrangement of silicon layers wrt vertex region. 
 Allocates layer number to a SpacePoint according to TrackFinder requirements.
 -----------------------------------------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "iPatGeometry/SiliconLayer.h"
#include "iPatGeometryTools/LayerNumberAllocator.h"
#include "iPatInterfaces/ILayerAllocator.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

LayerNumberAllocator::LayerNumberAllocator (const std::string&	type,
					    const std::string&	name, 
					    const IInterface*  	parent)
    :	AthAlgTool		(type, name, parent),
	m_layerAllocator	("LayerAllocator/LayerAllocator"),
	// pixel layout independent - always start SCT as if 3 preceding layers !
	m_pixel_layers		(3),
	m_pixel_sct_boundary	(0.),
	m_vertex_spread		(112.0*CLHEP::mm)	// TODO: make configurable
{
    declareInterface<ILayerNumberAllocator>(this);
    declareProperty("LayerAllocator",	m_layerAllocator);
}

LayerNumberAllocator::~LayerNumberAllocator(void)
{
    for (wheel_iterator w = m_pixel_r_boundaries.begin();
	 w != m_pixel_r_boundaries.end();
	 ++w)
	delete *w;
    for (wheel_iterator w = m_sct_r_boundaries.begin();
	 w != m_sct_r_boundaries.end();
	 ++w)
	delete *w;
}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
LayerNumberAllocator::initialize()
{
    // print where you are
    ATH_MSG_INFO( "LayerNumberAllocator::initialize()"
		  << "-package version " << PACKAGE_VERSION );

    // get the Tools
    if (m_layerAllocator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_layerAllocator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_layerAllocator );
    }

    // define the layer number boundaries
    pixel_definition(m_layerAllocator->pixelBarrel(),m_layerAllocator->pixelEndcap());
    sct_definition(m_layerAllocator->sctBarrel(),m_layerAllocator->sctEndcap());
    print();
    
    return StatusCode::SUCCESS;
}

StatusCode
LayerNumberAllocator::finalize()
{
    return StatusCode::SUCCESS;
}

int
LayerNumberAllocator::layerNumber(double r_point, double z_point) const
{
    // assume forwards/backwards symmetry
    z_point = fabs(z_point);

    // pixel allocation
    if (r_point < m_pixel_sct_boundary)
    {
	wheel_citerator   	w 	= m_pixel_r_boundaries.begin();
	boundary_citerator	z 	= m_pixel_z_boundary.begin();
	while (z_point > *z++) ++w;
	int 		       	layer	= 0;
	boundary_citerator	r 	= (*w)->begin();
	while (r_point > *r++) ++layer;
	return layer;
    }
    else	// sct allocation
    {
	wheel_citerator   	w 	= m_sct_r_boundaries.begin();
	boundary_citerator	z 	= m_sct_z_boundary.begin();
	while (z_point > *z++) ++w;
	int 		       	layer	= m_pixel_layers;
	boundary_citerator	r 	= (*w)->begin();
	while (r_point > *r++) ++layer;
	return layer;
    }
}

void
LayerNumberAllocator::print(void) const
{
    // print z boundary and vector of r boundaries for pixel and sct systems.
    // For each wheel where barrel counts as wheel 0
    std::cout << " LayerNumberAllocator: pixel wheel   z-boundary       r-boundaries "
	      << std::endl;

    int 		prec	= std::cout.precision();
    int 	       	wheel	= 0;
    boundary_citerator	z	= m_pixel_z_boundary.begin();
    for (wheel_citerator w = m_pixel_r_boundaries.begin();
	 w != m_pixel_r_boundaries.end();
	 ++w, ++wheel, ++z)
    {
	std::cout << std::setiosflags(std::ios::fixed)
		  << "                               " << std::setw(3) << wheel
		  << std::setw(13) << std::setprecision(1) 
		  << *z << "    ";
	for (boundary_citerator i = (*w)->begin();
	     i != (*w)->end();
	     ++i)
	{
	    std::cout << std::setw(7) << std::setprecision(1) << *i << "  ";
	}
	std::cout << std::endl;
    }

    // repeat for sct
    std::cout << " LayerNumberAllocator:   sct wheel   z-boundary       r-boundaries"
	      << std::endl;
    wheel	= 0;
    z		= m_sct_z_boundary.begin();
    for (wheel_citerator w = m_sct_r_boundaries.begin();
	 w != m_sct_r_boundaries.end();
	 ++w, ++wheel, ++z)
    {
	std::cout << std::setiosflags(std::ios::fixed)
		  << "                               " << std::setw(3) << wheel
		  << std::setw(13) << std::setprecision(1) 
		  << *z << "    ";
	for (boundary_citerator i = (*w)->begin();
	     i != (*w)->end();
	     ++i)
	{
	    std::cout << std::setw(7) << std::setprecision(1) << *i << "  ";
	}
	std::cout << std::resetiosflags(std::ios::fixed) << std::endl;
    }
    std::cout.precision(prec);
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
LayerNumberAllocator::pixel_definition (const layer_vector& barrel,
					const layer_vector& endcap)
{
    // check there are some active layers
    int activeLayers = 0;
    for (layer_citerator b = barrel.begin();
	 b != barrel.end();
	 ++b)
	if ((**b).isActive()) ++activeLayers;
    
    for (layer_citerator e = endcap.begin();
	 e != endcap.end();
	 ++e)
	if ((**e).isActive()) ++activeLayers;
    
    if (!activeLayers) return;
    
    // allocate boundaries in pixels
    int			layer	= 0;
    boundary_vector*	pixel_r	= new boundary_vector;
    m_pixel_z_boundary.push_back(0.);
    m_pixel_r_boundaries.push_back(pixel_r);
    for (layer_citerator e = endcap.begin();
	 e != endcap.end();
	 ++e)
    {
	if ((**e).isInactive()) continue;
	m_pixel_r_boundaries.push_back(new boundary_vector);
	m_pixel_z_boundary.push_back((*e)->zMin());
    }

    for (layer_citerator b = barrel.begin();
	 b != barrel.end();
	 ++b)
    {
	if ((**b).isInactive()) continue;
	++layer;
	if (*(m_pixel_z_boundary.begin()) < (*b)->zMax())
	    *(m_pixel_z_boundary.begin()) = (*b)->zMax();
	if (*b != *(barrel.rbegin()))
	{
	    pixel_r->push_back(0.5*((*b)->rMax() + (*(b+1))->rMin()));
	    double		cot_theta	= ((*b)->zMax() - m_vertex_spread) /
						  (*b)->rMax();
	    layer_citerator	e		= endcap.begin();
	    for (wheel_iterator w = m_pixel_r_boundaries.begin()+1;
		 w != m_pixel_r_boundaries.end();
		 ++e, ++w)
	    {
		while ((**e).isInactive()) ++e;
		double boundary = ((*e)->zMin() - m_vertex_spread) / cot_theta;
		if (boundary < (*e)->rMin()
		    || w+1 == m_pixel_r_boundaries.end())
		    boundary = layer;
		
		if (boundary < (*e)->rMax())
		{
		    double r_lower = (*e)->rMin();
		    if ((*w)->size() > 0 && *((*w)->rbegin()) > r_lower)
			r_lower = *((*w)->rbegin());

		    if (boundary > (*e)->rMin())
			cot_theta = ((*e)->zMin() - m_vertex_spread) / r_lower;
		    (*w)->push_back(boundary);   
		}
	    }
	}
	else		// put something reasonable for last r boundary
	{
	    wheel_iterator w = m_pixel_r_boundaries.begin();
	    double thickness = (*b)->rMax() - (*b)->rMin();
	    m_pixel_sct_boundary = (*b)->rMax() + thickness;
	    (*w)->push_back(m_pixel_sct_boundary);
	    layer_citerator e = endcap.begin();
	    for (w = m_pixel_r_boundaries.begin()+1;
		 w != m_pixel_r_boundaries.end();
		 ++e, ++w)
	    {
		while ((**e).isInactive()) ++e;
		double thickness = (*e)->zMax() - (*e)->zMin();
		if ((*e)->rMax() + thickness > m_pixel_sct_boundary)
		    m_pixel_sct_boundary = (*e)->rMax() + thickness;
		(*w)->push_back(m_pixel_sct_boundary);
	    }
	}
    }

    // set z boundaries between wheels with huge terminator
    for (boundary_iterator z = m_pixel_z_boundary.begin();
	 (z != m_pixel_z_boundary.end() && z+1 != m_pixel_z_boundary.end());
	 ++z)
	*z = 0.5*(*z + *(z+1));
    *(m_pixel_z_boundary.rbegin())	*= 2.;
}

void
LayerNumberAllocator::sct_definition (const layer_vector& barrel,
				      const layer_vector& endcap)
{
    // check there are some active layers
    int activeLayers = 0;
    for (layer_citerator b = barrel.begin();
	 b != barrel.end();
	 ++b)
	if ((**b).isActive()) ++activeLayers;
    
    for (layer_citerator e = endcap.begin();
	 e != endcap.end();
	 ++e)
	if ((**e).isActive()) ++activeLayers;
    
    if (!activeLayers) return;
    
    // allocate boundaries in sct
    int			layer	= 0;
    boundary_vector*	sct_r 	= new boundary_vector;
    m_sct_r_boundaries.push_back(sct_r);
    m_sct_z_boundary.push_back(0.);
    for (layer_citerator e = endcap.begin();
	 e != endcap.end();
	 ++e)
    {
	if ((**e).isInactive()) continue;
	m_sct_r_boundaries.push_back(new boundary_vector);
	m_sct_z_boundary.push_back((*e)->zMin());
    }

    for (layer_citerator b = barrel.begin();
	 b != barrel.end();
	 ++b)
    {
	if ((**b).isInactive()) continue;
	++layer;
	if (*(m_sct_z_boundary.begin()) < (*b)->zMax())
	    *(m_sct_z_boundary.begin()) = (*b)->zMax();
	if (*b != *(barrel.rbegin()))
	{
	    sct_r->push_back(0.5*((*b)->rMax() + (*(b+1))->rMin()));
	    double 		cot_theta	= ((*b)->zMax() - m_vertex_spread) /
						  (*b)->rMax();
	    layer_citerator 	e		= endcap.begin();
	    for (wheel_iterator w = m_sct_r_boundaries.begin()+1;
		 w != m_sct_r_boundaries.end();
		 ++e, ++w)
	    {
		while ((**e).isInactive()) ++e;
		double boundary = ((*e)->zMin() - m_vertex_spread) / cot_theta;
		if (boundary < (*e)->rMin()
		    || (layer < m_pixel_layers+2 && e+3 == endcap.end())
		    || (layer < m_pixel_layers+3 && e+2 == endcap.end())
		    || (layer < m_pixel_layers+4 && e+1 == endcap.end()))
		    boundary = layer;
		if (boundary < (*e)->rMax())
		{
		    double r_lower = (*e)->rMin();
		    if ((*w)->size() > 0 && *((*w)->rbegin()) > r_lower)
			r_lower = *((*w)->rbegin());

		    if (boundary > (*e)->rMin())
			cot_theta = ((*e)->zMin() - m_vertex_spread) / r_lower;
		    (*w)->push_back(boundary);   
		}
	    }
	}
	else		// put something huge for last r boundary
	{
	    wheel_iterator w = m_sct_r_boundaries.begin();
	    (*w)->push_back(2.*(*(barrel.rbegin()))->rMax());
	    layer_citerator e = endcap.begin();
	    for (w = m_sct_r_boundaries.begin()+1;
		 w != m_sct_r_boundaries.end();
		 ++e, ++w)
	    {
		while ((**e).isInactive()) ++e;
		(*w)->push_back(2.*(*e)->rMax());
	    }
	}
    }

    // backwards pass for sct only
    if (endcap.size() > 0 && m_sct_r_boundaries.size() > 2)
    {
	layer_criterator 	e      	= endcap.rbegin();
	double 		cot_theta 	= ((*e)->zMin() - m_vertex_spread) /
					  (*e)->rMax();
	++e;
	for (wheel_riterator w = m_sct_r_boundaries.rbegin()+1;
	     w != m_sct_r_boundaries.rend()-2;
	     ++e, ++w)
	{
	    while ((**e).isInactive()) ++e;
	    double boundary = ((*e)->zMin() - m_vertex_spread) / cot_theta;
	    if (boundary > *((*w)->rbegin()+1))
	    {
		*((*w)->rbegin()+1) = boundary;
		boundary = ((*(e+1))->zMin() - m_vertex_spread) / cot_theta;
		if (boundary > *((*(w+1))->rbegin()+2)) *((*(w+1))->rbegin()+2) = boundary;
	    }
	
	    cot_theta = ((*e)->zMin() - m_vertex_spread) / (*e)->rMax();
	}
    }

    // set z boundaries between wheels with huge terminator
    for (boundary_iterator z = m_sct_z_boundary.begin();
	 (z != m_sct_z_boundary.end() && z+1 != m_sct_z_boundary.end());
	 ++z)
	*z = 0.5*(*z + *(z+1));
    
    *(m_sct_z_boundary.rbegin()) *= 2.;
}
