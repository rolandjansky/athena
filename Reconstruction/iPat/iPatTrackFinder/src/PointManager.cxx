/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PointManager.cxx, (c) ATLAS iPatRec
//
// allocate SpacePoint's into a point_vector per 'idealized' layer.
// These are further sub-divided into projective eta-slices.
// FIXME: currently hard wired to be 7 layers and 8 eta-slices
//
///////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <utility>
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "iPatGeometry/SiliconDetector.h"
#include "iPatInterfaces/IFinderConfiguration.h"
#include "iPatInterfaces/ILayerNumberAllocator.h"
#include "iPatInterfaces/ISiDetectorLocator.h"
#include "iPatTrackFinder/PointManager.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

PointManager::PointManager (const std::string&	type,
			    const std::string&	name,
			    const IInterface*	parent)
    :   AthAlgTool			(type, name, parent),
	m_detectorLocator		("SiDetectorLocator/SiDetectorLocator"),
	m_finderTolerances		("FinderTolerances/FinderTolerances"),
	m_layerNumberAllocator		("LayerNumberAllocator/LayerNumberAllocator"),
	m_extendedPoints		(0),
	m_numberOfCombinedLayers	(9),	// really 2 combined + 7 regular
	m_numberOfLayers		(7),
	m_numberOfSlices		(8)
{
    declareInterface<IPointManager>(this);
    declareProperty("FinderConfiguration",	m_finderTolerances);
    declareProperty("LayerNumberAllocator",	m_layerNumberAllocator);
    declareProperty("SiDetectorLocator",	m_detectorLocator);
}

PointManager::~PointManager (void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
PointManager::initialize()
{
    ATH_MSG_INFO( "initialize with " << m_numberOfLayers << " idealized layers and "
		  << m_numberOfSlices << " eta slices" );

    // get the Tools
    if (m_detectorLocator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_detectorLocator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_detectorLocator );
    }
    if (m_finderTolerances.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_finderTolerances );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_finderTolerances );
    }
    if (m_layerNumberAllocator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_layerNumberAllocator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_layerNumberAllocator );
    }	
   
    // reserve for a point_vector in each layer and in each layer in each slice
    m_layerContainer.reserve(m_numberOfCombinedLayers);
    //    std::cout << " LayerContainer:: capacity " << m_layerContainer.capacity() << std::endl;
    
    m_sliceContainer.reserve(m_numberOfSlices);
    for (int slice = 0; slice != m_numberOfSlices; ++slice)
    {
	layer_vector* layer = new layer_vector;
	layer->reserve(m_numberOfCombinedLayers);
	m_sliceContainer.push_back(layer);
    }
    
    return StatusCode::SUCCESS;
}

StatusCode
PointManager::finalize()
{
    ATH_MSG_DEBUG( "PointManager::finalize()" );

    // get rid of no longer wanted space_points and containers ...
    if (m_extendedPoints) m_extendedPoints->clearWithDelete();
    delete m_extendedPoints;

    for (layer_iterator l = m_layerContainer.begin();
	 l != m_layerContainer.end();
	 ++l)
	delete *l;

    for (slice_iterator s = m_sliceContainer.begin();
	 s != m_sliceContainer.end();
	 ++s)
    {
	for (layer_iterator l = (**s).begin();
	     l != (**s).end();
	     ++l)
	    delete *l;

	delete *s;
    }
    
    return StatusCode::SUCCESS; 
}

void
PointManager::clear (void)
{
//     //////////////////////////////
//     // debug
//     std::cout << " before clear:: points size " << m_extendedPoints->size() << std::endl;
    

//     std::cout << " layer ";
//     for (layer_iterator l = m_layerContainer.begin();
// 	 l != m_layerContainer.end();
// 	 ++l)
//     {
// 	int size = -1;
// 	if (*l) size = (**l).size();
// 	std::cout << size << "  ";
//     }
//     for (slice_iterator s = m_sliceContainer.begin();
// 	 s != m_sliceContainer.end();
// 	 ++s)
//     {
// 	std::cout << std::endl << " slice ";
// 	for (layer_iterator l = (**s).begin();
// 	     l != (**s).end();
// 	     ++l)
// 	    std::cout << (**l).size() << "  "
// 		      << (**l).radius() << "  ";
//     }
//     std::cout << std::endl;

    // erase any existing points
    m_extendedPoints->clearWithDelete();
    delete m_extendedPoints;
    m_extendedPoints = 0;

    // clear layerContainer
    for (layer_riterator l = m_layerContainer.rbegin();
	 l != m_layerContainer.rend();
	 ++l)
	delete *l;
    m_layerContainer.clear();
    
    // clear sliceContainer
    for (slice_iterator s = m_sliceContainer.begin();
	 s != m_sliceContainer.end();
	 ++s)
    {
	for (layer_iterator l = (**s).begin();
	     l != (**s).end();
	     ++l)
	{
	    delete *l;
	}
	(**s).erase((**s).begin(),(**s).end());
    }

//     //////////////////////////////
//     // debug
// //     std::cout << " after  clear:: points size " << m_extendedPoints->size() << std::endl;
//     std::cout << " after  clear:: " << std::endl;

//     std::cout << " layer ";
//     for (layer_iterator l = m_layerContainer.begin();
// 	 l != m_layerContainer.end();
// 	 ++l)
//     {
// 	int size = -1;
// 	if (*l) size = (**l).size();
// 	std::cout << size << "  ";
//     }

//     for (slice_iterator s = m_sliceContainer.begin();
// 	 s != m_sliceContainer.end();
// 	 ++s)
//     {
// 	std::cout << std::endl << " slice ";
// 	for (layer_iterator l = (**s).begin();
// 	     l != (**s).end();
// 	     ++l)
// 	{
// 	    std::cout << (**l).size() << "  ";
// 	}
//     }
//     std::cout << std::endl;
}

void
PointManager::makeEtaSlices (double vertex, double halfWidth)
{
    // erase any existing point_vector's (and reset mean radius)
    for (slice_iterator s = m_sliceContainer.begin();
	 s != m_sliceContainer.end();
	 ++s)
    {
	for (layer_iterator l = (**s).begin();
	     l != (**s).end();
	     ++l)
	{
	    (**l).clear();
	}
    }

    // fill a layer_vector for each eta-slice
    for (layer_iterator l = m_layerContainer.begin();
	 l != m_layerContainer.end();
	 ++l)
    {
	int layer = l - m_layerContainer.begin();

	// now fill 8 eta-slices:
	// (projective from vertex width to a point at outer_radius)
	double	maxVertex	= +halfWidth;
	double	minVertex	= -halfWidth;
	double	outerRadius	= 540.;
	double	outerZ		= 0.;
	double	minSlope0	= (outerZ - minVertex)/outerRadius;
	outerZ			= 0.64*outerRadius;	// eta = 0.6
	double  maxSlope1	= (outerZ - maxVertex)/outerRadius;
	double  minSlope1	= (outerZ - minVertex)/outerRadius;
	outerZ			= 1.51*outerRadius;	// eta = 1.2
	double  maxSlope2	= (outerZ - maxVertex)/outerRadius;
	double  minSlope2	= (outerZ - minVertex)/outerRadius;
	outerZ			= 2.94*outerRadius;	// eta = 1.8
	double  maxSlope3	= (outerZ - maxVertex)/outerRadius;
	double  minSlope3	= (outerZ - minVertex)/outerRadius;

	int	firstSlice;
	int	lastSlice;
	int     numSlices	= 0;
	int	order[]		= {7, 5, 3, 1, 0, 2, 4, 6};
	
	for (point_citerator p = (**l).begin(); p != (**l).end(); ++p)
	{
	    double	absZ	= std::abs((**p).z() - vertex);
	    if (absZ > maxVertex - minSlope0*(**p).r())
	    {
		if (absZ > maxVertex + maxSlope2*(**p).r())
		{
		    if (absZ > maxVertex + maxSlope3*(**p).r())
		    {
			firstSlice     = 7;
		    }
		    else
		    {
			firstSlice     = 6;
		    }
		}
		else
		{
		    if (absZ > maxVertex + maxSlope1*(**p).r())
		    {
			firstSlice     = 5;
		    }
		    else
		    {
			firstSlice     = 4;
		    }
		}

		if (absZ > std::abs(minVertex + minSlope2*(**p).r()))
		{
		    if (absZ > std::abs(minVertex + minSlope3*(**p).r()))
		    {
			lastSlice     = 8;
		    }
		    else
		    {
			lastSlice     = 7;
		    }
		}
		else
		{
		    if (absZ > std::abs(minVertex + minSlope1*(**p).r()))
		    {
			lastSlice     = 6;
		    }
		    else
		    {
			lastSlice     = 5;
		    }
		}
		if ((**p).z() < vertex)
		{
		    int temp	= 8 - firstSlice;
		    firstSlice	= 8 - lastSlice;
		    lastSlice	= temp;
		}
	    }
	    else
	    {
		double	maxSlope	= ((**p).z() - minVertex - vertex)/(**p).r();
		if (maxSlope > minSlope2)
		{
		    if (maxSlope > minSlope3)
		    {
			lastSlice	= 8;
		    }
		    else
		    {
			lastSlice	= 7;
			
		    }
		}
		else
		{
		    if (maxSlope > minSlope1)
		    {
			lastSlice	= 6;
		    }
		    else
		    {
			lastSlice	= 5;
		    }
		}
		
		double  minSlope       = (maxVertex + vertex - (**p).z())/(**p).r();
		if (minSlope > minSlope2)
		{
		    if (minSlope > minSlope3)
		    {
			firstSlice	= 0;
		    }
		    else
		    {
			firstSlice	= 1;
		    }
		}
		else
		{
		    if (minSlope > minSlope1)
		    {
			firstSlice	= 2;
		    }
		    else
		    {
			firstSlice	= 3;
		    }
		}
	    }

	    numSlices	+= lastSlice - firstSlice;
	    for (int slice = firstSlice; slice != lastSlice; ++slice)
	    {
		layer_iterator li = (**(m_sliceContainer.begin()+order[slice])).begin() + layer;
		(**li).addPoint(*p);
	    }
	}
	
	// debug print
	if (m_finderTolerances->printLevel() > 4)
	{
	    if (layer == 0)	std::cout << "    PointManager::make_eta_slices   "
					  << "duplication factor per layer:  ";
	    double factor = 0;
	    if ((**l).size() > 0)
		factor	= static_cast<double>(numSlices)/static_cast<double>((**l).size());
	    if (layer < 7)	std::cout << factor << "  ";
	    if (layer == 6)	std::cout << std::endl;
	}
    }

    // set mean radius
    for (slice_iterator s = m_sliceContainer.begin();
	 s != m_sliceContainer.end();
	 ++s)
    {
	for (layer_iterator l = (**s).begin();
	     l != (**s).end();
	     ++l)
	    (**l).prepare();
    }
    
    // reset iterator
    newSearch();
}
  
bool
PointManager::nextSlice (void)
{
    if (++m_sliceIterator == m_sliceContainer.end())
    {
    	m_sliceIterator = m_sliceContainer.begin();
	return false;
    }
    else
    {
	return true;
    }
}
    
std::vector<Point*>*
PointManager::points (int layer) const
{ return (**((**m_sliceIterator).begin() + layer)).points(); }
    
double
PointManager::radius (int layer) const
{ return (**((**m_sliceIterator).begin() + layer)).radius(); }

void
PointManager::setPoints (const std::vector<Trk::SpacePoint*>&	space_points,
			 const Amg::Vector3D&			vertexRegionCentre)
{
    // existing points: erase them and clear pointers from containers
    if (m_extendedPoints) clear();

    int size		=  (3*space_points.size())/2;	// *1.5 to allow for phi wraps
    m_extendedPoints	= new PointGroup(size);
    if (size > 500) size /= m_numberOfLayers;		// size per layer
    for (int layer = 0; layer != m_numberOfLayers; ++layer)
	m_layerContainer.push_back(new PointGroup(size));

    // first store a pointer to each point into the point_vector corresponding to its layer
    // (i.e. into m_layerContainer)
    for (std::vector<Trk::SpacePoint*>::const_iterator s = space_points.begin();
	 s != space_points.end();
	 ++s)
    {
	Point*		p	= 0;
	int		layer	= m_layerNumberAllocator->layerNumber((**s).globalPosition().perp(),
								      (**s).globalPosition().z());
	layer_iterator	l	= m_layerContainer.begin() + layer;
	const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>&
	    clusterList		= (**s).clusterList();
	const std::pair<IdentifierHash, IdentifierHash>
	    elementIdList	= (**s).elementIdList();

	// ignore garbage
	const InDet::SiCluster* cluster1 = dynamic_cast<const InDet::SiCluster*>(clusterList.first);
	if (! cluster1)		continue;

	// test for pixel i.e. only one cluster
	const InDet::SiCluster* cluster2 = dynamic_cast<const InDet::SiCluster*>(clusterList.second);
	if (! cluster2)
	{
	    bool isBarrel	= m_detectorLocator->
				  pixelDetector(elementIdList.first).element().isBarrel();
	    p			= new Point(*cluster1,
					    elementIdList.first,
					    (**s).globalPosition(),
					    isBarrel,
					    vertexRegionCentre);
	}
	else
	{
	    bool isBarrel	= m_detectorLocator->
				  SCT_Detector(elementIdList.first).element().isBarrel();
	    p			= new Point(*cluster1,
					    *cluster2,
					    elementIdList.first,
					    elementIdList.second,
					    (**s).globalPosition(),
					    isBarrel,
					    vertexRegionCentre);
	}
	m_extendedPoints->addPoint(p);
	(**l).addPoint(p);
    }
    
    // then sort each point_vector by phi increasing
    // and make a phi-wrapped copy for those near the +PI/-PI boundary
    double maxCurvature = m_finderTolerances->maxCurvature();
    for (layer_iterator l = m_layerContainer.begin();
	 l != m_layerContainer.end();
	 ++l)
    {
	///////////// skip if no points
	if ((**l).empty()) continue;

	// prepare midRadius etc for group
	(**l).prepare();
	
	// sort group by ascending phi
	(**l).sort();
    
	// copy points in phi wrap-round region
	// TODO:: precompute phiMax from PointGroup
	point_citerator	p      	= (**l).begin();
	int		size	= m_extendedPoints->size();
	while (p != (**l).end()
	       && (**p).phi() < -M_PI + maxCurvature*(**p).r() + 3.0/(**p).r())
	{
	    Point* point = (**(p++)).copyAndWrapPhi();
	    m_extendedPoints->addPoint(point);
	}
	for (p = m_extendedPoints->begin() + size;
	     p != m_extendedPoints->end();
	     ++p)
	    (**l).addPoint(*p);
    }

    // make combined (double) SCT intermediate layers (3+4=7, 4+5=8)
    m_layerContainer.push_back(new PointGroup(**(m_layerContainer.begin()+3),
					      **(m_layerContainer.begin()+4)));
    m_layerContainer.push_back(new PointGroup(**(m_layerContainer.begin()+4),
					      **(m_layerContainer.begin()+5)));

    // can now reserve appropriate vectors for each layer in each slice
    for (slice_iterator s = m_sliceContainer.begin();
	 s != m_sliceContainer.end();
	 ++s)
    {
	for (layer_iterator l = m_layerContainer.begin();
	     l != m_layerContainer.end();
	     ++l)
	    (**s).push_back(new PointGroup((**l).size()));
    }
    
    // debug print
    if (m_finderTolerances->printLevel() > 2)
    {
	for (layer_iterator l = m_layerContainer.begin();
	     l != m_layerContainer.end();
	     ++l)
	{
	    int layer = l - m_layerContainer.begin();
	    if (m_finderTolerances->printLevel() < 7 && layer > 6) continue;

	    if (layer < 3)
	    {
		std::cout << " pixel" << " layer " << layer;
	    }
	    else
	    {
		std::cout << "   SCT" << " layer " << (layer-3);
	    }
	    std::cout << " has " << (**l).size() << " space points " << std::endl;
	    if (m_finderTolerances->printLevel() > 4)
	    {
		for (point_citerator p = (**l).begin(); p != (**l).end(); ++p)
		{
		    std::cout << "     ";
		    (**p).print();
		}
		std::cout << std::endl;
	    }
	}
    }
}
