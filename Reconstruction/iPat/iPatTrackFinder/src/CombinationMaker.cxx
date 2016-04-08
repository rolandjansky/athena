/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CombinationMaker.cxx, (c) ATLAS iPatRec
//
//  make SpacePoint combinatorials
//    (from vertex, inner, intermediate and outer layers)
//
///////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <utility>
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "iPatInterfaces/IAssignedHits.h"
#include "iPatInterfaces/IDetectorSelection.h"
#include "iPatInterfaces/IFinderConfiguration.h"
#include "iPatInterfaces/ILayerNumberAllocator.h"
#include "iPatInterfaces/IPointManager.h"
#include "iPatTrackFinder/CombinationMaker.h"
#include "iPatTrackFinder/Point.h"
#include "iPatTrackParameters/PerigeeParameters.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

CombinationMaker::CombinationMaker	(const std::string&	type,
					 const std::string&	name,
					 const IInterface*	parent)
    :   AthAlgTool			(type, name, parent),
	m_assignedHits			("AssignedHits/AssignedHits"),
	m_detectorSelection		("DetectorSelection/DetectorSelection"),
	m_finderTolerances		("FinderTolerances/FinderTolerances"),
	m_layerNumberAllocator		("LayerNumberAllocator/LayerNumberAllocator"),
	m_pointManager			("PointManager/PointManager"),
	m_outerCombinations		(0),
	m_innerCombinations		(0),
	m_intermediateCombinations	(0),
	m_vertexCombinations		(0)
{
    declareInterface<ICombinationMaker>(this);
    declareProperty("AssignedHits",		m_assignedHits);
    declareProperty("DetectorSelection",	m_detectorSelection);
    declareProperty("FinderConfiguration",	m_finderTolerances);
    declareProperty("LayerNumberAllocator",	m_layerNumberAllocator);
    
    // FIXME: this should (will) go into FinderTolerances
    m_vPhiTolerance	= 0.1/4.0;	// i.e. max impact wrt b-layer
}

CombinationMaker::~CombinationMaker (void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
CombinationMaker::initialize()
{
    ATH_MSG_INFO( "CombinationMaker::initialize()" );

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
    if (m_pointManager.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_pointManager );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_pointManager );
    }

    // set search patterns according to layout/luminosity
    if (StatusCode::SUCCESS != setSearchPatterns()) return StatusCode::FAILURE;
    
//     // debug
//     for (search_order::const_iterator s = m_searchOrder.begin();
// 	 s != m_searchOrder.end();
// 	 ++s)
//     {
// 	const search_pattern& searchPattern = *s;
	
// 	std::cout << " search " << (s - m_searchOrder.begin())
// 		  << "   pattern.size " << searchPattern.size();
// 	for (search_pattern::const_iterator p = searchPattern.begin();
// 	     p != searchPattern.end();
// 	     ++p)
// 	{
// 	    std::cout << "  layer " << *p;
// 	}
// 	std::cout << std::endl;
//     }

    return StatusCode::SUCCESS;
}

StatusCode
CombinationMaker::finalize()
{
    ATH_MSG_INFO( "CombinationMaker: outerCombinations "	<< m_outerCombinations
		  << "   innerCombinations " 			<< m_innerCombinations
		  << "   intermediateCombinations " 		<< m_intermediateCombinations
		  << "   vertexCombinations " 			<< m_vertexCombinations	);

    return StatusCode::SUCCESS; 
}

void
CombinationMaker::clear (void)
{ m_pointManager->clear(); }
   
bool
CombinationMaker::doublePixelPrimarySearch (void)
{
    m_allowPrimaryShare	= true;
    m_maxImpact		= m_finderTolerances->maxPrimaryImpact();
    m_search		= m_doublePixelPrimarySearchBegin;
    m_searchEnd		= m_doublePixelPrimarySearchEnd;
    return newSearch();
}

bool
CombinationMaker::doublePixelTruncatedSearch (void)
{
    m_allowPrimaryShare	= true;
    m_search   		= m_doublePixelTruncatedSearchBegin;
    m_searchEnd		= m_doublePixelTruncatedSearchEnd;
    return newSearch();
}

bool
CombinationMaker::secondarySearch (void)
{
    m_allowPrimaryShare	= false;
    m_maxImpact		= m_finderTolerances->maxSecondaryImpact();
    m_search		= m_secondarySearchBegin;
    m_searchEnd		= m_secondarySearchEnd;
    return newSearch();
}

bool
CombinationMaker::singlePixelSearch (void)
{
    m_allowPrimaryShare	= true;
    m_maxImpact		= m_finderTolerances->maxPrimaryImpact();
    m_search		= m_singlePixelSearchBegin;
    m_searchEnd		= m_singlePixelSearchEnd;
    return newSearch();
}

bool
CombinationMaker::vertexSearch (double minPt)
{
    m_allowPrimaryShare	= true;
    m_maxCurvature	= m_finderTolerances->maxCurvature(minPt);
    m_maxImpact		= m_finderTolerances->maxPrimaryImpact();
    m_search		= m_vertexSearchBegin;
    m_searchEnd		= m_vertexSearchEnd;
    return newSearch();
}

bool
CombinationMaker::next_inner_point (void)
{
    while (++m_i != m_iEnd && m_assignedHits->onSameTrack(**m_o,**m_i)) ;

    if (m_i == m_iEnd || (**m_i).phi() > m_iPhiMax)
    {
	return false;
    }
    else
    {
	++m_innerCombinations;
	return true;
    }    
}

bool
CombinationMaker::next_intermediate_point (void)
{
    do
    {	
	if (++m_m == m_mEnd || (**m_m).phi() > m_mPhiMax) return false;
    } while (!m_allowPrimaryShare && m_assignedHits->assignedToPrimary(**m_m));

    ++m_intermediateCombinations;
    return true;
}

bool
CombinationMaker::next_outer_point (void)
{
    // find next outer point ...
    bool loop;
    do
    {
	loop = true;
	++m_o;

	// as loops get exhausted - move onto next slice, then next search pattern
	while (m_o == m_oEnd)
	{
	    if (!m_pointManager->nextSlice())
	    {
		++m_search;
		if (m_search == m_searchEnd) return false;
		// check for interchanges to o/m/i/v order
		m_middleInterchange	= false;
		m_pixelInterchange	= false;
		search_pattern::const_reverse_iterator r = m_search->rbegin();
		if (m_search->size() == 4
		    && *r++ > *r) m_pixelInterchange = true;
		if (*r++ > *r) m_middleInterchange = true;
		if (*r > 2)	// combined layers and SCT endcap have spread
		{
		    m_radialSpread = 40.;
		}
		else
		{
		    m_radialSpread = 10.;
		}
		if (m_finderTolerances->printLevel() > 3) printSearch();
	    }
	    search_pattern::const_iterator s = m_search->begin();
	    double outerR	= m_pointManager->radius(*s);
	    point_vector* p	= m_pointManager->points(*s++);
	    m_o			= p->begin();
	    m_oEnd		= p->end();

	    // set intermediate (middle) iterators
	    if (m_o != m_oEnd)
	    {
		m_middleR	= m_pointManager->radius(*s);
		p		= m_pointManager->points(*s++);
		m_mFirst	= p->begin();
		m_mEnd		= p->end();

		// inner iterators
		if (m_mFirst == m_mEnd)
		{
		    m_o = m_oEnd;
		}
		else
		{
		    double innerR	= m_pointManager->radius(*s);
		    p			= m_pointManager->points(*s++);
		    m_iFirst		= p->begin();
		    m_iEnd		= p->end();
		    if (m_iFirst == m_iEnd)
		    {
			m_o = m_oEnd;
		    }
		    else
		    {
			// ptMin and maxImpact define phi range - but allow a safety margin
			// to account for radialSpread and threshold effects (esp in endcap)
			double invI		= 1./innerR;
			double invO     	= 1./outerR;
			m_deltaPhiInner		= m_maxImpact*(invI-invO) + 1.2*m_maxCurvature *
						  (outerR-innerR);
			double invM		= 1./m_middleR;
			m_deltaPhiMiddle	= m_maxImpact*(invM-invO) + 1.2*m_maxCurvature *
						  (outerR-m_middleR);
			m_middleTolerance	= m_radialSpread*m_maxCurvature;
			if (invI-invM < invM-invO)
			{
			    m_middleTolerance += m_maxImpact*std::abs(invI - invM);
			}
			else
			{
			    m_middleTolerance += m_maxImpact*(invM - invO);
			}
// 			std::cout << " InnerR,MiddleR,OuterR "
// 				  << innerR << "  " << m_middleR << "  " << outerR
// 				  << std::endl
// 				  << "   m_maxCurvature*(outerR-innerR) " 
// 				  << 1000.*m_maxCurvature*(outerR-innerR)
// 				  << "  m_maxCurvature*(outerR-middleR) "
// 				  << 1000.*m_maxCurvature*(outerR-m_middleR)
// 				  << "  m_deltaPhiMiddle " << 1000.*m_deltaPhiMiddle
// 				  << "  m_middleTolerance " << 1000.*m_middleTolerance << std::endl;
			
		    }
		}
	    }
	}

	if (! m_assignedHits->isAssigned(**m_o)
	    && (m_allowPrimaryShare || ! m_assignedHits->assignedToPrimary(**m_o)))
	{
	    // compute valid phi window and position iterators at start of window ...
	    //   intermediate (middle) iterator
	    double phiMin = (**m_o).phi() - m_deltaPhiMiddle;
	    while (m_mFirst != m_mEnd && (**m_mFirst).phi() < phiMin) ++m_mFirst;
	    //   inner iterator
	    if (m_mFirst != m_mEnd && (**m_mFirst).phi() < (**m_o).phi() + m_deltaPhiMiddle)
	    {
		phiMin		= (**m_o).phi() - m_deltaPhiInner;
		m_iPhiMax	= (**m_o).phi() + m_deltaPhiInner;
		while (m_iFirst != m_iEnd && (**m_iFirst).phi() < phiMin) ++m_iFirst;
		if (m_iFirst != m_iEnd && (**m_iFirst).phi() < m_iPhiMax) loop = false;
	    }
	}
    } while (loop);	

    m_i	= m_iFirst - 1;
    ++m_outerCombinations;
//     int sizeO = m_oEnd - m_o;
//     int size_i = m_iEnd - m_iFirst;
//     int size_m = m_mEnd - m_mFirst;
//     std::cout << " sizes " << sizeO << " " << size_i << " " << size_m << std::endl;
    
    return true;
}
    
void
CombinationMaker::intermediatePhiRange (void)
{
    double phiSlope	= ((**m_o).phi() - (**m_i).phi()) / ((**m_o).r() - (**m_i).r());
    double phi 		= (**m_i).phi() +  phiSlope*(m_middleR - (**m_i).r());
    m_mPhiMax		= phi + m_middleTolerance;
    double phiMin	= phi - m_middleTolerance;
    m_m			= m_mFirst;
    while (m_m != m_mEnd && (**m_m).phi() < phiMin) ++m_m;
    --m_m;
}

bool
CombinationMaker::newSearch (void)
{
    if (m_search == m_searchEnd) return false;
    m_oEnd	= m_pointManager->points(*m_search->begin())->end();
    --m_search;
    m_o	       	= m_oEnd - 1;
    m_pointManager->newSearch();
    return true;
}
 
bool
CombinationMaker::next_vertex_point (void)
{
    do
    {
	++m_v;
	if (m_v == m_vertexPoints.end()) return false;
    } while (m_assignedHits->onSameTrack(**m_o,**m_v));
    
    ++m_vertexCombinations;
    return true;
}

void
CombinationMaker::setProperties (const std::vector<Trk::SpacePoint*>&	spacePoints,
				 const Amg::Vector3D&			vertexRegionCentre)
{
    m_pointManager->setPoints(spacePoints,vertexRegionCentre);
}
  
void
CombinationMaker::set_tolerances (double minPt)
{
    m_maxCurvature	= m_finderTolerances->maxCurvature(minPt);
}

void
CombinationMaker::set_vertex_points (const PerigeeParameters&	parameters)
{
    // remove any existing vertex points
    m_vertexPoints.erase(m_vertexPoints.begin(),m_vertexPoints.end());
    
    // crude phi range uses max impact parameter and deltaPhi between inner point and vertex 
    double deltaPhi	= std::abs((**m_i).phi() - (**m_o).phi()) * (**m_i).r()/(**m_o).r() +
			  m_vPhiTolerance;
    double phiMax	= (**m_i).phi() + deltaPhi;
    double phiMin	= (**m_i).phi() - deltaPhi;
    point_vector* p	= m_pointManager->points(*m_search->rbegin());
    m_v		       	= p->begin();
    m_vEnd	       	= p->end();
    while (m_v != m_vEnd && (**m_v).phi() < phiMin) ++m_v;

    // select point if z and phi compatible with prediction from perigee
    // skip phi wrapped points
    double zTolerance		= 9.0*parameters.covariance()(1,1);
    double rPhiTolerance	= 9.0*parameters.covariance()(0,0);
    while (m_v != m_vEnd && (**m_v).phi() < phiMax)
    {
	double deltaZ = (**m_v).z() - parameters.z() - parameters.cotTheta()*(**m_v).r();
	if (deltaZ*deltaZ < zTolerance)
	{
// 	    std::cout << " deltaPhi,deltaZ " << (**m_v).phi() - (**m_o).phi()
// 		  << "  " << deltaZ
// 		  << "  tolerance " << std::sqrt(zTolerance) << std::endl;
	    // prepare for new perigee phi range ...
	    double phiPredict	= parameters.phi();
	    if (phiPredict >  M_PI) phiPredict -= 2.*M_PI;
	    if (phiPredict < -M_PI) phiPredict += 2.*M_PI;
	    phiPredict	+= m_finderTolerances->halfField() *
			   parameters.qOverPt()*(**m_v).r();
	    double deltaPhi	= phiPredict - (**m_v).phi();
// 	    if (deltaPhi < -M_PI) deltaPhi += 2.*M_PI;
// 	    if (deltaPhi >  M_PI) deltaPhi -= 2.*M_PI;
	    double deltaRPhi	= parameters.transverseImpact() + (**m_v).r()*deltaPhi;
	    if (deltaRPhi*deltaRPhi < rPhiTolerance) m_vertexPoints.push_back(*m_v);
	    
// 	    std::cout << " deltaPhi " << deltaPhi << "  term1,2 "
// 		      << parameters.transverse_impact()
// 		      << "  " << (**m_v).r()*deltaPhi
// 		      << "   deltaRPhi " << deltaRPhi << std::endl;
	}
	++m_v;
    }
//     std::cout << "  deltaPhiMax,Min " << phiMax << "  " << phiMin
// 	      << "   point_vector.size " 
// 	      << p->size() << "  vertexPoints.size() "
// 	      << m_vertexPoints.size() << std::endl;
    m_v = m_vertexPoints.begin() - 1;
}

void
CombinationMaker::vertex_region (double vertex, double halfWidth)
{ m_pointManager->makeEtaSlices(vertex,halfWidth); }
    
//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
CombinationMaker::addSearchPattern (int outerLayer,
				    int middleLayer,
				    int innerLayer,
				    int vertexLayer = -1) 
{
    search_pattern searchPattern;
    searchPattern.push_back(outerLayer);
    searchPattern.push_back(middleLayer);
    searchPattern.push_back(innerLayer);
    if (vertexLayer != -1) searchPattern.push_back(vertexLayer);
    m_searchOrder.push_back(searchPattern);
    ++m_numberOfSearches;
}

void
CombinationMaker::printSearch (void) const
{
    search_pattern::const_iterator s = m_search->begin();
    int out = *s++;
    int mid = *s++;
    int inn = *s++;
		    
    std::cout << "    search #"	<< (m_search - m_searchOrder.begin()) << "  outer_layer: ";
    if (out == 8)
    {
	std::cout << "  SCT 1+2";
    }
    else if (out == 7)
    {
	std::cout << "  SCT 0+1";
    }
    else if (out > 2)
    {
	std::cout << "  SCT " << out-3;
    }
    else
    {
	std::cout << "pixel " << out;
    }
		    
    std::cout << "  inner_layer: ";
    if (inn == 8)
    {
	std::cout << "  SCT 1+2";
    }
    else if (inn == 7)
    {
	std::cout << "  SCT 0+1";
    }
    else if (inn > 2)
    {
	std::cout << "  SCT " << inn-3;
    }
    else
    {
	std::cout << "pixel " << inn;
    }	   
    std::cout << "  intermediate_layer: ";
    if (mid == 8)
    {
	std::cout << "  SCT 1+2";
    }
    else if (mid == 7)
    {
	std::cout << "  SCT 0+1";
    }
    else if (mid > 2)
    {
	std::cout << "  SCT " << mid-3;
    }
    else
    {
	std::cout << "pixel " << mid;
    }
    if (m_middleInterchange) std::cout << "   middleInterchange";
    if (m_pixelInterchange)  std::cout << "   pixelInterchange";
    std::cout << std::endl;
}

StatusCode
CombinationMaker::setSearchPatterns (void)
{
    // then get layout from GeoModel pixel or SCT DetectorManager
    const InDetDD::SiDetectorManager* manager;
    std::string layout = "";
    if (StatusCode::SUCCESS == detStore()->retrieve(manager,"Pixel"))
    {
        layout = manager->getLayout();
	ATH_MSG_INFO( " layout " << layout );
    }
    else if (StatusCode::SUCCESS == detStore()->retrieve(manager,"SCT"))
    {
	layout = manager->getLayout();
    }
    else
    {
        ATH_MSG_FATAL( "Could not find Pixel or SCT DetectorManager: " );
        return StatusCode::FAILURE;
    }

    // set patterns according to layout (CTB, Initial or Final),
    // except case with only one sub-detector (pixel or SCT) selected
    // when appropriate secondary pattern chosen.
    //
    // vertex search produces 'fast_segments' to pre-define potential vertices
    // primary/truncated searches have 1 pixel from layers 0 or 1
    // secondary searches have layers 2 or 3 as inner and an outer sct point from layers 5 or 6
    //
    // note the searches will be performed in order: double pixel before single pixel,
    // vertex, then primary, then truncated, finally secondary.

    m_numberOfSearches			= 0;
    unsigned singlePixelSearch		= 0;
    unsigned doublePixelTruncatedSearch	= 0;
    unsigned secondarySearch		= 0;
    unsigned vertexSearch		= 0;

    if (! m_detectorSelection->processPixels())
    {
	secondarySearch		= m_numberOfSearches;
	addSearchPattern(6, 8, 3);
	addSearchPattern(5, 4, 3);
	addSearchPattern(6, 5, 4);
	singlePixelSearch	= m_numberOfSearches;
	vertexSearch		= m_numberOfSearches;
    }
    else if (! m_detectorSelection->processSCT())
    {
	secondarySearch		= m_numberOfSearches;
	addSearchPattern(2, 1, 0);
	singlePixelSearch	= m_numberOfSearches;
	vertexSearch		= m_numberOfSearches;
    }
    else if (layout == "TestBeam")
    {
	// CTB - only make secondary searches
	secondarySearch		= m_numberOfSearches;
	addSearchPattern(6, 1, 0);
	addSearchPattern(6, 2, 0);
	addSearchPattern(5, 1, 0);
	addSearchPattern(5, 2, 0);
	addSearchPattern(4, 1, 0);
	addSearchPattern(6, 2, 1);
	addSearchPattern(5, 2, 1);
	addSearchPattern(6, 8, 3);
	addSearchPattern(5, 4, 3);
	addSearchPattern(6, 5, 4);
	singlePixelSearch	= m_numberOfSearches;
	vertexSearch		= m_numberOfSearches;
    }
    else if (layout == "Initial")
    {
	// initial layout - start with doublePixelPrimary
	addSearchPattern(6, 0, 1, 0);
	addSearchPattern(5, 0, 1, 0);
	doublePixelTruncatedSearch	= m_numberOfSearches;
	addSearchPattern(4, 0, 1, 0);
	addSearchPattern(3, 0, 1, 0);
	secondarySearch			= m_numberOfSearches;
	addSearchPattern(6, 8, 3);
	addSearchPattern(5, 4, 3);
	addSearchPattern(6, 5, 4);
	singlePixelSearch		= m_numberOfSearches;
	addSearchPattern(6, 7, 1);
	addSearchPattern(6, 7, 0);
	addSearchPattern(4, 3, 1);
	addSearchPattern(5, 7, 0);
	vertexSearch			= m_numberOfSearches;
	addSearchPattern(6, 7, 1);
	addSearchPattern(5, 7, 1);
	addSearchPattern(4, 3, 0);
    }
    else	// this had better be Final !
    {
	// final layout - start with doublePixelPrimary
	addSearchPattern(6, 2, 1, 2);
	addSearchPattern(6, 0, 1, 0);
	addSearchPattern(5, 2, 1, 2);
	addSearchPattern(5, 0, 1, 0);
	doublePixelTruncatedSearch	= m_numberOfSearches;
	addSearchPattern(4, 0, 1, 0);
	addSearchPattern(3, 2, 1, 2);
	secondarySearch			= m_numberOfSearches;
	addSearchPattern(6, 8, 3);
	addSearchPattern(5, 4, 3);
	addSearchPattern(6, 5, 4);
	singlePixelSearch		= m_numberOfSearches;
	addSearchPattern(6, 7, 1);
	addSearchPattern(4, 3, 0);
	addSearchPattern(6, 8, 2);
	addSearchPattern(5, 7, 2);
// 	addSearchPattern(6, 8, 2, 0);
// 	addSearchPattern(5, 7, 1);
// 	addSearchPattern(4, 3, 0);    // skip with final layout?
	vertexSearch			= m_numberOfSearches;
	addSearchPattern(6, 7, 1);
	addSearchPattern(5, 7, 1);
	addSearchPattern(4, 3, 0);
    }	   

    // set begin/end iterators to each search pattern
    m_doublePixelPrimarySearchBegin	= m_searchOrder.begin();
    m_doublePixelPrimarySearchEnd	= m_searchOrder.begin() + doublePixelTruncatedSearch;
    m_doublePixelTruncatedSearchBegin	= m_searchOrder.begin() + doublePixelTruncatedSearch;
    m_doublePixelTruncatedSearchEnd	= m_searchOrder.begin() + secondarySearch;
    m_secondarySearchBegin		= m_searchOrder.begin() + secondarySearch;
    m_secondarySearchEnd		= m_searchOrder.begin() + singlePixelSearch;
    m_singlePixelSearchBegin		= m_searchOrder.begin() + singlePixelSearch;
    m_singlePixelSearchEnd		= m_searchOrder.begin() + vertexSearch;
    m_vertexSearchBegin			= m_searchOrder.begin() + vertexSearch;
    m_vertexSearchEnd			= m_searchOrder.end();

    return StatusCode::SUCCESS;
}

