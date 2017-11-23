/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Algorithm iPatStatistics: produce SpacePoint and track statistics.
 Summary printed at finalize step.
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "GenInterfaces/ITruthSelector.h"
#include "iPatInterfaces/IDetectorSelection.h"
#include "iPatInterfaces/ILayerNumberAllocator.h"
#include "iPatInterfaces/ITruthParameters.h"
#include "iPatRecAlgs/iPatStatistics.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

//------------------------------------------------------------------------

iPatStatistics::iPatStatistics(const std::string& name, ISvcLocator* pSvcLocator)
    :	AthAlgorithm			(name, pSvcLocator),
	m_detectorSelection		("DetectorSelection/DetectorSelection"),
	m_layerNumberAllocator		("LayerNumberAllocator/LayerNumberAllocator"),
	m_truthParameters		("TruthParameters/TruthParameters"),
	m_truthSelector			("TruthSelector/TruthSelector"),
	m_haveTruth			(false),
	m_minPt				(0.5*Gaudi::Units::GeV),
	m_overlapSpacePointsLocation	("OverlapSpacePoints"),
	m_pixelSpacePointsLocation	("PixelSpacePoints"),
	m_sctSpacePointsLocation	("SCT_SpacePoints"),
	m_tracksLocation		("iPatTracks"),
	m_tracks			(0),
	m_countPixelBroad		(0),
	m_countPixelHits		(0),
	m_countPixelXXX			(0),
	m_countPixelXXO			(0),
	m_countPixelXOX			(0),
	m_countPixelOXX			(0),
	m_countPixelOOX			(0),
	m_countPrimaryXXX		(0),
	m_countPrimaryXXO		(0),
	m_countPrimaryXOX		(0),
	m_countPrimaryOXX		(0),
	m_countPrimaryOOX		(0),
	m_countSctBroad			(0),
	m_countSctHits			(0),
	m_countTrtAssociated		(0),
	m_countTrtBroad			(0),
	m_countTrtHits			(0),
	m_countTrtMissed		(0),
	m_countTrtTruthAssociated	(0),
	m_countTrtTruthMissed		(0),
	m_eventCount			(0),
	m_selectSpacePoints		(true),
	m_selectTracks			(true),
	m_spacePointCount       	(0)
{
    declareProperty("DetectorSelection",		m_detectorSelection);
    declareProperty("LayerNumberAllocator",		m_layerNumberAllocator);
    declareProperty("TruthParameters",			m_truthParameters);
    declareProperty("TruthSelector",			m_truthSelector);
    declareProperty("haveTruth",			m_haveTruth);
    declareProperty("minPt",				m_minPt);
    declareProperty("overlapSpacePointsLocation",	m_overlapSpacePointsLocation);
    declareProperty("pixelSpacePointsLocation",		m_pixelSpacePointsLocation);
    declareProperty("sctSpacePointsLocation",		m_sctSpacePointsLocation);
    declareProperty("TracksLocation",			m_tracksLocation);
    
    // initialise counters for 7 layers of SpacePoint's and 10 types of Track statistics
    for (int i = 0; i < 11; ++i)
    {
	m_spacePointsByLayer.push_back(0);
	m_primaryCount.push_back(0);
	m_secondaryCount.push_back(0);
	m_trackClusters.push_back(0);
	m_trackCount.push_back(0);
	m_trackFakes.push_back(0);
	m_trackHoles.push_back(0);
	m_trackMissedPrimary.push_back(0);
	m_trackMissedSecondary.push_back(0);
	m_trackPrimary.push_back(0);
	m_trackSecondary.push_back(0);
	m_trackStraws.push_back(0);
    }
}

//--------------------------------------------------------------------------
iPatStatistics::~iPatStatistics(void)
{}

//-----------------------------------------------------------------------
StatusCode
iPatStatistics::initialize()
{
    // print where you are
    ATH_MSG_INFO( "iPatStatistics::initialize()" );

    // get the Tools
    if (m_detectorSelection.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_detectorSelection );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_detectorSelection );
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
    if (m_truthParameters.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_truthParameters );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_truthParameters );
    }
    if (m_truthSelector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_truthSelector );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_truthSelector );
    }

    // check configuration setting
    if (m_selectSpacePoints)
    {
	// check that space points have names
	if (m_pixelSpacePointsLocation		== ""
	    && m_sctSpacePointsLocation		== ""
	    && m_overlapSpacePointsLocation	== "")
	{
	    ATH_MSG_FATAL( "No location given for space points" );
	    return StatusCode::FAILURE;
	}
    }
    
    if (m_selectTracks && m_tracksLocation == "")
    {
	ATH_MSG_FATAL( "No location given for iPat Tracks" );
	return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
    
StatusCode
iPatStatistics::execute()
{
    ATH_MSG_DEBUG( "entered execution with eventCount " << m_eventCount );

    // on first event find out if McTruth really exists
    if (!m_eventCount++ && m_haveTruth)
    {
        const xAOD::EventInfo* eventInfo;
	if (StatusCode::SUCCESS != evtStore()->retrieve(eventInfo))
	{
	    ATH_MSG_WARNING( "Could not retrieve event info" );
	    m_haveTruth = false;
	}
	else if (!eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION))
	{
	    m_haveTruth = false;
	}
	if (m_haveTruth)  m_haveTruth = m_truthSelector->mcEventCollection();
    }

    // pixel SpacePoints
    if (m_selectSpacePoints && m_detectorSelection->processPixels())
    {
	const SpacePointContainer* spacePointContainer;
	if (StatusCode::SUCCESS != evtStore()->retrieve(spacePointContainer,m_pixelSpacePointsLocation))
	{
	    ATH_MSG_WARNING( "Unable to retrieve PixelSpacePoint container" );
	}
	else
	{
	    for (SpacePointContainer::const_iterator
		     colNext = spacePointContainer->begin();
		 colNext != spacePointContainer->end();
		 ++colNext)
	    {
		for (SpacePointCollection::const_iterator sp = (**colNext).begin();
		     sp != (**colNext).end();
		     ++sp)
		{
		    std::vector<int>::iterator lay =
			m_spacePointsByLayer.begin() +
			m_layerNumberAllocator->layerNumber((**sp).globalPosition().perp(),
							    (**sp).globalPosition().z());
		    ++(*lay);
		    ++m_spacePointCount;
		}
	    }
	}
    }

    // SCT SpacePoints
    if (m_selectSpacePoints && m_detectorSelection->processSCT())
    {
	const SpacePointContainer* spacePointContainer;
	if (StatusCode::SUCCESS !=
	    evtStore()->retrieve(spacePointContainer,m_sctSpacePointsLocation))
	{
	    ATH_MSG_WARNING( "Unable to retrieve SCT_SpacePoint container" );
	}
	else
	{
	    for (SpacePointContainer::const_iterator
		     colNext = spacePointContainer->begin();
		 colNext != spacePointContainer->end();
		 ++colNext)
	    {
		for (SpacePointCollection::const_iterator sp = (**colNext).begin();
		     sp != (**colNext).end();
		     ++sp)
		{
		    std::vector<int>::iterator lay =
			m_spacePointsByLayer.begin() +
			m_layerNumberAllocator->layerNumber((**sp).globalPosition().perp(),
							    (**sp).globalPosition().z());
		    ++(*lay);
		    ++m_spacePointCount;
		}
	    }
	}
	
	// module overlaps have their own collection
	const SpacePointOverlapCollection* overlapCollection;
	if (StatusCode::SUCCESS !=
	    evtStore()->retrieve(overlapCollection,m_overlapSpacePointsLocation))
	{
	    ATH_MSG_DEBUG( "Unable to retrieve SpacePoint overlap collection"	);
	}
	else
	{
	    for (SpacePointOverlapCollection::const_iterator sp = overlapCollection->begin();
		 sp != overlapCollection->end();
		 ++sp)
	    {
		std::vector<int>::iterator lay =
		    m_spacePointsByLayer.begin() +
		    m_layerNumberAllocator->layerNumber((**sp).globalPosition().perp(),
							(**sp).globalPosition().z());
		++(*lay);
		++m_spacePointCount;
	    }   
	}
    }

    // Tracks - first get the MC Truth for reconstructable primaries and secondaries
    if (!m_selectTracks)	return StatusCode::SUCCESS;
    std::vector<int>	barrel_primary_truth;
    std::vector<int>	endcap_primary_truth;
    std::vector<int>	mixed_primary_truth;
    std::vector<int>	primary_truth;
    std::vector<int>	barrel_secondary_truth;
    std::vector<int>	endcap_secondary_truth;
    std::vector<int>	mixed_secondary_truth;
    std::vector<int>	secondary_truth;
    if (m_haveTruth)
    {
	// first get primaries
	const std::vector<int>& primaries = m_truthSelector->reconstructablePrimaries(m_minPt);
	for (std::vector<int>::const_iterator p = primaries.begin();
	     p != primaries.end();
	     ++p)
	{
	    double absEta = std::abs(m_truthParameters->trackParameters(*p)->eta());
	    if (absEta < 0.8)
	    {
		++(*(m_primaryCount.begin()+1));
		++(*(m_primaryCount.begin()+2));
		++(*(m_primaryCount.begin()+3));
		++(*(m_primaryCount.begin()+4));
		barrel_primary_truth.push_back(*p);
		primary_truth.push_back(*p);
	    }
	    else if (absEta < 1.6)
	    {
		++(*(m_primaryCount.begin()+1));
		++(*(m_primaryCount.begin()+5));
		++(*(m_primaryCount.begin()+6));
		++(*(m_primaryCount.begin()+7));
		mixed_primary_truth.push_back(*p);
		primary_truth.push_back(*p);
	    }
	    else if (absEta < 2.5)
	    {
		++(*(m_primaryCount.begin()+1));
		++(*(m_primaryCount.begin()+8));
		++(*(m_primaryCount.begin()+9));
		++(*(m_primaryCount.begin()+10));
		endcap_primary_truth.push_back(*p);
		primary_truth.push_back(*p);
	    }
	}
    
	// then same thing for secondaries
	const std::vector<int>& secondaries = m_truthSelector->reconstructableSecondaries(m_minPt);
	for (std::vector<int>::const_iterator s = secondaries.begin();
	     s != secondaries.end();
	     ++s)
	{
	    double absEta = std::abs(m_truthParameters->trackParameters(*s)->eta());
	    if (absEta < 0.8)
	    {
		++(*(m_secondaryCount.begin()+1));
		++(*(m_secondaryCount.begin()+2));
		++(*(m_secondaryCount.begin()+3));
		++(*(m_secondaryCount.begin()+4));
		barrel_secondary_truth.push_back(*s);
		secondary_truth.push_back(*s);
	    }
	    else if (absEta < 1.6)
	    {
		++(*(m_secondaryCount.begin()+1));
		++(*(m_secondaryCount.begin()+5));
		++(*(m_secondaryCount.begin()+6));
		++(*(m_secondaryCount.begin()+7));
		mixed_secondary_truth.push_back(*s);
		secondary_truth.push_back(*s);
	    }
	    else if (absEta < 2.5)
	    {
		++(*(m_secondaryCount.begin()+1));
		++(*(m_secondaryCount.begin()+8));
		++(*(m_secondaryCount.begin()+9));
		++(*(m_secondaryCount.begin()+10));
		endcap_secondary_truth.push_back(*s);
		secondary_truth.push_back(*s);
	    }
	}
    }
    
    // Get hold of Tracks from TDS and compare with Truth
    std::vector<int>	barrel_primary_found;
    std::vector<int>	barrel_secondary_found;
    std::vector<int>	endcap_primary_found;
    std::vector<int>	endcap_secondary_found;
    std::vector<int>	mixed_primary_found;
    std::vector<int>	mixed_secondary_found;
    std::vector<int>	primary_found;
    std::vector<int>	secondary_found;
    if (StatusCode::SUCCESS != evtStore()->retrieve(m_tracks, m_tracksLocation))
    {
	ATH_MSG_WARNING( "Unable to retrieve the TrackContainer" );
	return StatusCode::SUCCESS;
    }
    
    for (std::vector<Track*>::const_iterator t = m_tracks->begin();
	 t != m_tracks->end();
	 ++t)
    {
 	// physics selection cuts
 	int b_layer	= 0;
	int endcapPixel	= 0;
	int middlePixel	= 0;
 	int outerPixel	= 0;
   
 	for (hit_const_iterator h = (**t).hit_list_begin(); h!= (**t).hit_list_end(); ++h)
 	{
	    if ((**h).isCluster())
	    {
		if ((**h).status() == pixel_cluster || (**h).status() == broad_pixel)
		{
		    ++m_countPixelHits;
		    if ((**h).status() == broad_pixel) ++m_countPixelBroad;
		    if ((**h).position().perp() < 60.)
		    {
			++b_layer;
		    }
		    else if ((**h).isBarrel() && (**h).position().perp() < 100.)
		    {
			++middlePixel;
		    }
		    else  if ((**h).isBarrel()) 
		    {
			++outerPixel;
		    }
		    else
		    {
			++endcapPixel;
		    }
		}
		else
		{
		    ++m_countSctHits;
		    if ((**h).status() == broad_strip) ++m_countSctBroad;
		}
	    }
	    else if ((**h).isDrift())
	    {
		++m_countTrtHits;
		if ((**h).status() == drift_wire) ++m_countTrtBroad;
	    }
	    
 	}
	// 	// egamma cut   TODO: fix barrel
	// 	if ((**t).hitQuality().number_planar_clusters() >= 9
	// 	    && outerPixel > 0 
	// 	    && b_layer > 0) continue;
 	//

	int	row;
	// int	type;
	if ((**t).status() == primary)
	{
	    row	= -1;
	    // type = 1;
	}
	else if ((**t).status() == truncated)
	{
	    row	=  0;
	    // type = 2;
	}
	else
	{
	    row	= 1;
	    // type = 3;
	}

	int	barcode		= 0;
	if ((**t).hasTruthAssociation()) barcode = (**t).truthAssociation().barcode();
	bool	is_fake		= false;
	bool	is_primary     	= false;
	bool	is_secondary	= false;
	++(*(m_trackCount.begin()));
	if (! m_haveTruth)
	{
	    if ((**t).perigee_parameters().abs_pt() < m_minPt) continue;
	    is_primary	= true;
	    ++(*(m_trackCount.begin()+1));
	    ++(*(m_trackPrimary.begin()+1));
	    double absEta = std::abs((**t).perigee_parameters().eta());
	    if (absEta < 0.8)
	    {
		row	+= 3;
	    }
	    else if (absEta < 1.6)
	    {
		row	+= 6;
	    }
	    else if (absEta < 2.5)
	    {
		row	+= 9;
	    }
	}
	else
	{
	    
	    if (barcode == 0)
	    {
		is_fake	= true;
		++(*(m_trackFakes.begin()));
	    }

	    // track is a true primary
	    if (std::find(primary_truth.begin(),
			  primary_truth.end(),
			  barcode) != primary_truth.end())
	    {
		is_primary	= true;
		++(*(m_trackCount.begin()+1));
		++(*(m_trackPrimary.begin()+1));
		if (std::find(primary_found.begin(),
			      primary_found.end(),
			      barcode) == primary_found.end())
		{
		    primary_found.push_back(barcode);
		}
		if (std::find(barrel_primary_truth.begin(),
			      barrel_primary_truth.end(),
			      barcode) != barrel_primary_truth.end())
		{
		    row	+= 3;
		    if (std::find(barrel_primary_found.begin(),
				  barrel_primary_found.end(),
				  barcode) == barrel_primary_found.end())
		    {
			barrel_primary_found.push_back(barcode);
		    }
		}
		else if (std::find(endcap_primary_truth.begin(),
				   endcap_primary_truth.end(),
				   barcode) != endcap_primary_truth.end())
		{
		    row	+= 9;
		    if (std::find(endcap_primary_found.begin(),
				  endcap_primary_found.end(),
				  barcode) == endcap_primary_found.end())
		    {
			endcap_primary_found.push_back(barcode);
		    }
		}
		else if (std::find(mixed_primary_truth.begin(),
				   mixed_primary_truth.end(),
				   barcode) != mixed_primary_truth.end())
		{
		    row	+= 6;
		    if (std::find(mixed_primary_found.begin(),
				  mixed_primary_found.end(),
				  barcode) == mixed_primary_found.end())
		    {
			mixed_primary_found.push_back(barcode);
		    }
		}
	    }
	    // track is a true secondary
	    else if (std::find(secondary_truth.begin(),
			       secondary_truth.end(),
			       barcode) != secondary_truth.end())
	    {
		is_secondary	= true;
		++(*(m_trackCount.begin()+1));
		++(*(m_trackSecondary.begin()+1));
		if (std::find(secondary_found.begin(),
			      secondary_found.end(),
			      barcode) == secondary_found.end())
		{
		    secondary_found.push_back(barcode);
		}
		if (std::find(barrel_secondary_truth.begin(),
			      barrel_secondary_truth.end(),
			      barcode) != barrel_secondary_truth.end())
		{
		    row	+= 3;
		    if (std::find(barrel_secondary_found.begin(),
				  barrel_secondary_found.end(),
				  barcode) == barrel_secondary_found.end())
		    {
			barrel_secondary_found.push_back(barcode);
		    }
		}
		else if (std::find(endcap_secondary_truth.begin(),
				   endcap_secondary_truth.end(),
				   barcode) != endcap_secondary_truth.end())
		{
		    row	+= 9;
		    if (std::find(endcap_secondary_found.begin(),
				  endcap_secondary_found.end(),
				  barcode) == endcap_secondary_found.end())
		    {
			endcap_secondary_found.push_back(barcode);
		    }
		}
		else if (std::find(mixed_secondary_truth.begin(),
				   mixed_secondary_truth.end(),
				   barcode) != mixed_secondary_truth.end())
		{
		    row	+= 6;
		    if (std::find(mixed_secondary_found.begin(),
				  mixed_secondary_found.end(),
				  barcode) == mixed_secondary_found.end())
		    {
			mixed_secondary_found.push_back(barcode);
		    }
		}
	    }
	    // track is a fake above minPt
	    else if (is_fake && (**t).perigee_parameters().abs_pt() > m_minPt)
	    {
		++(*(m_trackFakes.begin()+1));
		if (std::abs((**t).perigee_parameters().eta()) < 0.8)
		{
		    row	+= 3;
		}
		else if (std::abs((**t).perigee_parameters().eta()) < 1.6)
		{
		    row	+= 6;
		}
		else if (std::abs((**t).perigee_parameters().eta()) < 2.5)
		{
		    row	+= 9;
		}
	    }
	}
	
	*(m_trackClusters.begin())		+= (**t).hitQuality().number_planar_clusters();
	*(m_trackHoles.begin())			+= (**t).hitQuality().number_planar_holes();
	*(m_trackStraws.begin())		+= (**t).hitQuality().number_drift_hits();

	// hit pattern counters for pixel barrel
	if (! endcapPixel)
	{
	    if (b_layer)
	    {
		if (middlePixel && outerPixel)	++m_countPixelXXX;
		else if (middlePixel)		++m_countPixelXXO;
		else if (outerPixel)		++m_countPixelXOX;
	    }
	    else if (middlePixel && outerPixel)	++m_countPixelOXX;
	    else if (outerPixel)		++m_countPixelOOX;
	    if (is_primary)
	    {
		if (b_layer)
		{
		    if (middlePixel && outerPixel)	++m_countPrimaryXXX;
		    else if (middlePixel)		++m_countPrimaryXXO;
		    else if (outerPixel)		++m_countPrimaryXOX;
		}
		else if (middlePixel && outerPixel)	++m_countPrimaryOXX;
		else if (outerPixel)		++m_countPrimaryOOX;
	    }
	}

	// and TRT association counters
	if (std::abs((**t).perigee_parameters().eta()) < 2.0)
	{
	    if ((**t).hitQuality().number_drift_hits())
	    {
		if (m_haveTruth && (is_primary || is_secondary)) ++m_countTrtTruthAssociated;
		++m_countTrtAssociated;
	    }
	    else if ((**t).status() == truncated)
	    {
		if (is_primary || is_secondary) ++m_countTrtTruthMissed;
		++m_countTrtMissed;
	    }
	}
	
	if (row > 1)
	{
	    if (!is_fake)
	    {
		*(m_trackClusters.begin() + 1)	+= (**t).hitQuality().number_planar_clusters();
		*(m_trackHoles.begin() + 1)    	+= (**t).hitQuality().number_planar_holes();
		*(m_trackStraws.begin() + 1)   	+= (**t).hitQuality().number_drift_hits();
	    }
	    
	    *(m_trackClusters.begin() + row)	+= (**t).hitQuality().number_planar_clusters();
	    *(m_trackHoles.begin() + row)   	+= (**t).hitQuality().number_planar_holes();
	    *(m_trackStraws.begin() + row)  	+= (**t).hitQuality().number_drift_hits();
	    ++(*(m_trackCount.begin() + row));
	    if (is_primary)
	    {
		++(*(m_trackPrimary.begin() + row));
	    }
	    else if (is_secondary)
	    {
		++(*(m_trackSecondary.begin() + row));
	    }
	    else if (is_fake)
	    {
		++(*(m_trackFakes.begin() + row));
	    }
	}
    }
    *(m_trackMissedPrimary.begin() + 1)		+= primary_truth.size() -
						   primary_found.size();
    *(m_trackMissedPrimary.begin() + 2)       	+= barrel_primary_truth.size() -
						   barrel_primary_found.size();
    *(m_trackMissedPrimary.begin() + 5)       	+= mixed_primary_truth.size() -
						   mixed_primary_found.size();
    *(m_trackMissedPrimary.begin() + 8)       	+= endcap_primary_truth.size() -
						   endcap_primary_found.size();
    *(m_trackMissedSecondary.begin()+1)		+= secondary_truth.size() -
						   secondary_found.size();
    *(m_trackMissedSecondary.begin() + 4)      	+= barrel_secondary_truth.size() -
						   barrel_secondary_found.size();
    *(m_trackMissedSecondary.begin() + 7)      	+= mixed_secondary_truth.size() -
						   mixed_secondary_found.size();
    *(m_trackMissedSecondary.begin() + 10)	+= endcap_secondary_truth.size() -
						   endcap_secondary_found.size();

//     if (secondary_truth.size() != secondary_found.size())
// 	std::cout << " MISSED secondary " << std::endl;
    
    ATH_MSG_DEBUG( "exit execution" );
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode
iPatStatistics::finalize()
{
    ATH_MSG_INFO( m_eventCount << " events processed" );
    int prec	= std::cout.precision();
    if (m_eventCount > 0)
    {
	double	norm	= 1./static_cast<double> (m_eventCount);
	double	count;
	if (m_selectSpacePoints)
	{
	    count	= norm*static_cast<double> (m_spacePointCount);
	    int	layer	= 0;
	    std::cout << std::setiosflags(std::ios::fixed | std::ios::right) << std::endl
		      << "  SpacePoints/event :     total        .... pixel layers ....  "
		      << "       ......... SCT layers ........." << std::endl
		      << "                     "
		      << std::setw(10) << std::setprecision(1) << count << "   ";
	    for (std::vector<int>::const_iterator i = m_spacePointsByLayer.begin();
		 i != m_spacePointsByLayer.end();
		 ++i, ++layer)
	    {
		if (layer > 6) break;
		count = norm*static_cast<double> (*i);
		std::cout << std::setw(9) << std::setprecision(1) << count;
		if (layer == 2) std::cout << "   ";
	    }
	    std::cout << std::resetiosflags(std::ios::fixed) << std::endl;
	}

	if (m_selectTracks)
	{
	    std::cout << std::setiosflags(std::ios::fixed | std::ios::right) << std::endl
		      << "  Track statistics :               "
		      << "    ....... rates wrt truth .......      bkgd"
		      << "       ......... hits/track ........." << std::endl
		      << "                   tracks/event      "
		      << "  primary missed secondary missed      rate"
		      << "       SiClusters   SiHoles    Straws" << std::endl;

	    for (int i = 0; i != 11; ++i)
	    {
		switch (i)
		{
		case 0:
		    std::cout << "          Total    ";
		    break;
		case 1:
		    std::cout << "          Signal   ";
		    break;    
		case 2:
		    std::cout << "  signal by type in barrel region (eta<0.8) :"
			      << std::endl << "          Primary  ";
		    break;
		case 3:
		    std::cout << "          Truncated";
		    break;
		case 4:
		    std::cout << "          Secondary";
		    break;
		case 5:
		    std::cout << "  signal by type in transition region (0.8<eta<1.6) :"
			      << std::endl << "          Primary  ";
		    break;
		case 6:
		    std::cout << "          Truncated";
		    break;
		case 7:
		    std::cout << "          Secondary";
		    break;
		case 8:
		    std::cout << "  signal by type in endcap region (1.6<eta<2.5) :"
			      << std::endl << "          Primary  ";
		    break;
		case 9:
		    std::cout << "          Truncated";
		    break;
		case 10:
		    std::cout << "          Secondary";
		    break;    
		};
		count	= norm*static_cast<double> (*(m_trackCount.begin()+i));
		std::cout << std::setiosflags(std::ios::fixed | std::ios::right)
			  << std::setw(12) << std::setprecision(1) << count;
		double tknorm	= *(m_primaryCount.begin()+i);
		if (tknorm > 0.)
		{
		    tknorm		= 1./tknorm;
		    double missedP	= tknorm *
					  static_cast<double> (*(m_trackMissedPrimary.begin()+i));
		    double primary	= tknorm *
					  static_cast<double> (*(m_trackPrimary.begin()+i));
		    std::cout << std::setiosflags(std::ios::fixed)
			      << std::setw(15) << std::setprecision(4) << primary;
		    switch (i)
		    {
		    case 1:
		    case 2:
		    case 5:
		    case 8:			
			std::cout << std::setw(7) << std::setprecision(4) << missedP;
			break;
		    default:
			std::cout << "       ";
			break;
		    };
		}
		else
		{
		    std::cout << "                      ";
		}
		
		tknorm	= *(m_secondaryCount.begin()+i);
		if (tknorm > 0.)
		{
		    tknorm		= 1./tknorm;
		    double missedS	= tknorm *
					  static_cast<double> (*(m_trackMissedSecondary.begin()+i));
		    double secondary	= tknorm *
					  static_cast<double> (*(m_trackSecondary.begin()+i));
		    std::cout << std::setiosflags(std::ios::fixed)
			      << std::setw(10) << std::setprecision(4) << secondary;
		    switch (i)
		    {
		    case 1:
		    case 4:
		    case 7:
		    case 10:			
			std::cout << std::setw(7) << std::setprecision(4) << missedS;
			break;
		    default:
			std::cout << "       ";
			break;
		    };
		}
		else
		{
		    std::cout << "                 ";
		}
		
		tknorm	= *(m_trackCount.begin()+i);
		if (tknorm > 0.)
		{
		    tknorm		= 1./tknorm;
		    double fakes	= tknorm*static_cast<double> (*(m_trackFakes.begin()+i));
		    double clusters	= tknorm*static_cast<double> (*(m_trackClusters.begin()+i));
		    double holes	= tknorm*static_cast<double> (*(m_trackHoles.begin()+i));
		    double straws	= tknorm*static_cast<double> (*(m_trackStraws.begin()+i));
		    std::cout << std::setiosflags(std::ios::fixed)
			      << std::setw(10) << std::setprecision(4) << fakes
			      << std::setw(17) << std::setprecision(2) << clusters
			      << std::setw(10) << std::setprecision(2) << holes
			      << std::setw(10) << std::setprecision(2) << straws;
		}
		std::cout << std::endl;
	    }

	    // spoilt/outlier hit frequencies
	    std::cout << std::endl << std::setiosflags(std::ios::fixed)
		      << " broad error frequencies: ";
	    if (m_countPixelHits)
		std::cout << std::setw(8) << std::setprecision(4)
			  << static_cast<double>(m_countPixelBroad) /
		    static_cast<double>(m_countPixelHits) << " pixels  ";
	    if (m_countSctHits)
		std::cout << std::setw(8) << std::setprecision(4)
			  << static_cast<double>(m_countSctBroad) /
		    static_cast<double>(m_countSctHits) << " SCT  ";
	    if (m_countTrtHits)
		std::cout << std::setw(8) << std::setprecision(4)
			  << static_cast<double>(m_countTrtBroad) /
		    static_cast<double>(m_countTrtHits) << " TRT";
	    std::cout << std::endl;

	    // pixel/sct/TRT hit patterns
	    if (m_countPixelXXX)
	    {
		double normPix = 1./static_cast<double>
				 (m_countPixelXXX + m_countPixelXXO + m_countPixelXOX +
				  m_countPixelOXX + m_countPixelOOX);
		std::cout << std::endl << std::setiosflags(std::ios::fixed)
			  << " Barrel-pixel hit-pattern frequencies:"
			  << std::setw(8) << std::setprecision(4)
			  << normPix*static_cast<double>(m_countPixelXXX) << " XXX"
			  << std::setw(10) << std::setprecision(4)
			  << normPix*static_cast<double>(m_countPixelXXO) << " XXO"
			  << std::setw(10) << std::setprecision(4)
			  << normPix*static_cast<double>(m_countPixelXOX) << " XOX"
			  << std::setw(10) << std::setprecision(4)
			  << normPix*static_cast<double>(m_countPixelOXX) << " OXX"
			  << std::setw(10) << std::setprecision(4)
			  << normPix*static_cast<double>(m_countPixelOOX) << " OOX"
			  << std::endl;
		if (m_countPrimaryXXX)
		{
		    
		    normPix = 1./static_cast<double>
			      (m_countPrimaryXXX + m_countPrimaryXXO + m_countPrimaryXOX +
			       m_countPrimaryOXX + m_countPrimaryOOX);
		    std::cout << std::setiosflags(std::ios::fixed)
			      << "            for primary truth-matches:"
			      << std::setw(8) << std::setprecision(4)
			      << normPix*static_cast<double>(m_countPrimaryXXX) << " XXX"
			      << std::setw(10) << std::setprecision(4)
			      << normPix*static_cast<double>(m_countPrimaryXXO) << " XXO"
			      << std::setw(10) << std::setprecision(4)
			      << normPix*static_cast<double>(m_countPrimaryXOX) << " XOX"
			      << std::setw(10) << std::setprecision(4)
			      << normPix*static_cast<double>(m_countPrimaryOXX) << " OXX"
			      << std::setw(10) << std::setprecision(4)
			      << normPix*static_cast<double>(m_countPrimaryOOX) << " OOX"
			      << std::endl;
		}
	    }

	    if (m_countTrtAssociated)
	    {
		double trtRate = static_cast<double>(m_countTrtAssociated) /
				 static_cast<double>(m_countTrtAssociated+m_countTrtMissed);
		std::cout << " TRT association rate (for eta < 2.0):"
			  << std::setw(8) << std::setprecision(4) << trtRate;
		if (m_countTrtTruthAssociated)
		{
		    trtRate = static_cast<double>(m_countTrtTruthAssociated) /
			      static_cast<double>(m_countTrtTruthAssociated+m_countTrtTruthMissed);
		    std::cout << "  ("
			      << std::setw(6) << std::setprecision(4) << trtRate
			      << " for truth-matches)";
		}
		std::cout << std::endl;
	    }
	}
    }

    std::cout << std::endl;
    std::cout.precision(prec);
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------


