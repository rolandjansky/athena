/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 predict ordered list<LayerPrediction*> for intersected SiliconDetector's
 within a given SiliconLayer
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cassert>
#include <iomanip>
#include <iostream>
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/Surface.h"
#include "iPatGeometry/SiliconDetector.h"
#include "iPatGeometry/SiliconRing.h"
#include "iPatInterfaces/ISiClusterProperties.h"
#include "iPatTrackFollower/LayerPrediction.h"
#include "iPatTrackFollower/LayerPredictor.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>

const double   	LayerPredictor::s_boundaryTolerance = 5.0; // 5mm boundary tolerance

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

LayerPredictor::LayerPredictor(hit_list*		hits,
			       const SiliconLayer&	layer,
			       const Amg::Vector3D&	position,
			       const Amg::Vector3D&	transverseDirection,
			       double			rPhiWidth,
			       double			rZWidth)
    :   m_finalHit		(0),
	m_finalPrediction	(0),
	m_hits                  (hits),
	m_initialHit		(0),
	m_initialPrediction	(0),
	m_isActive		(layer.isActive()),
	m_isBarrel		(layer.isBarrel()),
	m_numberOfHits		(0),
	m_numberOfHoles		(0),
	m_position              (position),
	m_rPhiWidth		(rPhiWidth),
	m_rZWidth		(rZWidth),
	m_transverseDirection	(transverseDirection)
{
    // make ordered list of LayerPrediction's in each ring
    if (m_isActive)
    {
	SiliconLayer::ring_vector*  rings = layer.rings(m_transverseDirection.z(),
							m_position.perp(),
							m_position.z(),
							m_rZWidth);
	predict(rings->begin(),rings->end());
	delete rings;
    }
    else
    {
	// temporary hack to get inactive material
	m_materialProperties = layer.materialProperties(m_position,
							  m_transverseDirection);
    }

    // set boundary status for predictions in layers which are not necessarily intersected
    if ((m_isBarrel
	 && (m_position.z()    	> layer.zMax() - s_boundaryTolerance
	     || m_position.z()	< layer.zMin() + s_boundaryTolerance))
	|| (!m_isBarrel
	    && (m_position.perp()   	> layer.rMax() - s_boundaryTolerance
		|| m_position.perp()	< layer.rMin() + s_boundaryTolerance)))
    {
	for (prediction_iterator p = m_predictions.begin();
	     p != m_predictions.end();
	     ++p)
	    if ((**p).status() == hole) (**p).status(boundary);
    }	
}

LayerPredictor::~LayerPredictor(void)
{
    for (prediction_iterator p = m_predictions.begin();
	 p != m_predictions.end();
	 ++p)
	delete *p;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
LayerPredictor::predict (SiliconLayer::ring_const_iterator    begin,
			 SiliconLayer::ring_const_iterator    end)
{
    // make list of predictions - ordered by increasing radius (outgoing order)
    for (SiliconLayer::ring_vector::const_iterator ring = begin;
	 ring != end;
	 ++ring)
    {
	const SiliconDetector*  detector        = (**ring).detector(m_position.phi());
	if (detector == 0)			continue;

	DetectorIntersect* intersect = detector->intersect(m_position,
							   m_transverseDirection,
							   m_rPhiWidth,
							   m_rZWidth);
	if (intersect == 0)			continue;
	// intersect->print();
	prediction_iterator	p       = m_predictions.begin();
	while (p != m_predictions.end() && (**p).radius() < intersect->radius()) ++p;

	// create TrackParameters at the detector surface to store on the prediction
	const Trk::AtaPlane* trkParameters =
	    new Trk::AtaPlane(intersect->position(),
			      m_transverseDirection,
			      1.,
			      dynamic_cast<const Trk::PlaneSurface&>(detector->element().surface()));
	
	// insert the new prediction
	LayerPrediction* prediction = new LayerPrediction(detector,intersect,trkParameters);
	p = m_predictions.insert(p,prediction);
    }
}
	
//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

int
LayerPredictor::insertHits (const ToolHandle<ISiClusterProperties>& properties)
{
    // return 1 (success) for inactive material layers
    if (! m_isActive) return 1;
    
    // insert hits in hit_list (skip if boundary or outside)
    m_initialHit = 0;
    for (prediction_iterator p = m_predictions.begin();
	 p != m_predictions.end();
	 ++p)
    {
	if ((**p).status() != boundary && (**p).status() != outside)
	{
	    if ((**p).status() == hole)	++m_numberOfHoles;
	    ++m_numberOfHits;
	    m_hits->push_back((**p).hit(properties));
	    m_finalHit = *m_hits->rbegin();
	    if (! m_initialHit && (**p).status() != hole)
	    {
		m_initialHit		= m_finalHit;
		m_initialPrediction	= *p;
	    }
	    m_finalPrediction = *p;
	}
    }
    return m_numberOfHits;
}

void
LayerPredictor::insertScatterer (void)
{
    // test for inactive layer
    if (! m_isActive)
    {
	m_hits->push_back(new HitOnTrack(m_isBarrel,
					 m_materialProperties.first,
					 m_position,
					 m_materialProperties.second,
					 inactive));
	return;
    }
    
    // insert scatterer as last hit in layer
    if (m_finalPrediction == 0) return;
    
    // pixel: total thickness scaled by number of intersected modules
    //        overlap has initial and final scatterers
    double weight = 1.;
    if (m_finalPrediction->detector().ring().layer().isPixel())
    {
	if (m_numberOfHits) weight = static_cast<double> (m_numberOfHits);
	if (m_initialHit
	    && m_initialHit != m_finalHit
	    && m_finalHit->status() != hole)
	{
	    weight = 0.5;
	    for (hit_iterator h = m_hits->begin(); h != m_hits->end(); ++h)
	    {
		if (*h == m_initialHit)
		{
		    m_hits->insert(++h,m_initialPrediction->hit(weight));
		    break;
		}
	    }
	}
    }
    m_hits->push_back(m_finalPrediction->hit(weight));
    m_finalPrediction = 0;
}

void
LayerPredictor::print (void) const
{
    for (prediction_const_iterator p = m_predictions.begin();
	 p != m_predictions.end();
	 ++p)
	(**p).print();

    for (hit_iterator h = m_hits->begin(); h != m_hits->end(); ++h)
    {
	if ((**h).isCluster())
	{
	    std::cout << "   ";
	}
	else if ((**h).isMaterial())
	{
	    if ((**h).status() == scatterer)
	    {
		std::cout << "  S";
	    }
	    else
	    {
		std::cout << "  M";
	    }
	}
	else
	{
	    std::cout << "  H";
	}
	if ((**h).isBarrel())
	{
	    std::cout << (**h).position().perp() << " ";
	}
	else
	{
	    std::cout << (**h).position().z() << "Z";
	}
    }
    std::cout << std::endl;
    
    std::cout << std::endl;
}
