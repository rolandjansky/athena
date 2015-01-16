/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 transient object formed during track-following through a layer giving
 predicted hit parameters in an intersected detector
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "InDetPrepRawData/SiCluster.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "iPatGeometry/SiliconLayer.h"
#include "iPatGeometry/SiliconRing.h"
#include "iPatInterfaces/ISiClusterProperties.h"
#include "iPatTrackFollower/LayerAssociation.h"
#include "iPatTrackFollower/LayerPrediction.h"
#include "iPatTrackFollower/SiClusterProperties.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

LayerPrediction::LayerPrediction(const SiliconDetector*		detector,
				 DetectorIntersect*    		intersect,
				 const Trk::TrackParameters*	parameters)
    :   m_association           (0),
	m_cluster		(0),
	m_detector              (detector),
	m_hit                   (0),
	m_intersect		(intersect),
	m_parameters		(parameters),
	m_radius                (intersect->radius())
{
    if (intersect->nearBoundary())
    {
	m_status = boundary;
    }
    else
    {
	m_status = hole;
    }
}

LayerPrediction::~LayerPrediction(void)
{
    delete m_intersect;
    delete m_parameters;
}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

HitOnTrack*
LayerPrediction::hit (const ToolHandle<ISiClusterProperties>& properties) const
{
    if (m_hit != 0)
    {
	return m_hit;
    }
    else if (m_association == 0)
    {
	return (new HitOnTrack(m_detector->ring().isBarrel(),
			       &m_detector->element(),
			       m_parameters->position(),
			       m_status)); 
    }
    else
    {
	const InDet::SiCluster* cluster	= &m_association->cluster();
	return properties->hitOnTrack(cluster,
				      m_detector,
				      m_parameters);
    }
}

HitOnTrack*
LayerPrediction::hit (double weight) const
{
    std::pair<double,double> material	= m_detector->ring().layer().materialProperties();
    double projection =
	weight*std::abs(m_parameters->momentum().mag() /
			m_detector->element().surface().normal().dot(m_parameters->momentum()));

     return new HitOnTrack(m_detector->ring().isBarrel(),
			   projection*material.first,
			   m_parameters->position(),
			   projection*material.second,
			   scatterer);
}

void
LayerPrediction::print(void) const
{
    if (m_association == 0)
    {
	std::cout << " LayerPrediction:  " << std::setiosflags(std::ios::fixed)
		  << m_status << "        r"
		  << std::setw(7) << std::setprecision(2) << m_radius
		  << "  z" << std::setw(9) << std::setprecision(2)
		  << m_intersect->position().z() << std::endl;
    }
    else
    {
	m_association->print();
    }
}


