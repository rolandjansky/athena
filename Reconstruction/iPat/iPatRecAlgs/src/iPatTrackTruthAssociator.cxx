/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 iPatTrackTruthAssociator - get truth (barcode) for each found track 
 
 ATLAS Collaboration
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iostream>
#include <iomanip>
#include <map>
#include "GaudiKernel/SystemOfUnits.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "GenInterfaces/ITruthSelector.h"
#include "iPatInterfaces/IDetectorSelection.h"
#include "iPatRecAlgs/iPatTrackTruthAssociator.h"
#include "iPatRecEvent/iPatTrackContainer.h"
#include "iPatTrack/HitList.h"
#include "iPatTrack/Track.h"
#include "iPatTrack/TruthAssociation.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

//------------------------------------------------------------------------

iPatTrackTruthAssociator::iPatTrackTruthAssociator(const std::string&	name, 
						   ISvcLocator*		pSvcLocator)
    :	AthAlgorithm		(name, pSvcLocator),
	m_detectorSelection	("DetectorSelection/DetectorSelection"),
	m_truthSelector		("TruthSelector/TruthSelector"),
	m_tracksLocation	(""),
	m_backgroundCount	(0),
	m_eventCount		(0),
	m_fakeCount		(0),
	m_haveTruth		(false),
	m_tracks		(0),
	m_wrongCount		(0)
{
    declareProperty("DetectorSelection",m_detectorSelection);
    declareProperty("TruthSelector",	m_truthSelector);
    declareProperty("TracksLocation", 	m_tracksLocation);
}

//--------------------------------------------------------------------------
iPatTrackTruthAssociator::~iPatTrackTruthAssociator(void)
{}

//-----------------------------------------------------------------------
StatusCode
iPatTrackTruthAssociator::initialize()
{
    // print where you are
    ATH_MSG_INFO( "iPatTrackTruthAssociator::initialize()" );

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
    if (m_truthSelector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_truthSelector );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_truthSelector );
    }

    if (m_tracksLocation == "")
    {
	ATH_MSG_FATAL( "No location set for iPat Tracks" );
	return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG( "Initialized successfully" );
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
    
StatusCode
iPatTrackTruthAssociator::execute()
{
    ATH_MSG_DEBUG( "entered execution" );
    
    // on first event find out if McTruth exists (exit if real data)
    if (!m_eventCount++)
    {
        const xAOD::EventInfo* eventInfo;
	if (StatusCode::SUCCESS != evtStore()->retrieve(eventInfo))
	{
	    ATH_MSG_WARNING( "Could not retrieve event info" );
	    return StatusCode::SUCCESS;
	}
	else if (eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)
		 && m_truthSelector->mcEventCollection())
	{
	    m_haveTruth = true;
	}	
    }
    if (! m_haveTruth) return StatusCode::SUCCESS;

    // retrieve the pixel and sct Cluster truth maps for this event
    const PRD_MultiTruthCollection* simPixClusterMap = 0;
    if (m_detectorSelection->processPixels()
	&& StatusCode::SUCCESS != evtStore()->retrieve(simPixClusterMap,
						       "PRD_MultiTruthPixel"))
    {
	ATH_MSG_WARNING( "Unable to retrieve the Pixel PRD_MultiTruthCollection" );
	return StatusCode::SUCCESS;
    }
    const PRD_MultiTruthCollection* simSctClusterMap = 0;
    if (m_detectorSelection->processSCT()
	&& StatusCode::SUCCESS != evtStore()->retrieve(simSctClusterMap,
						       "PRD_MultiTruthSCT"))
    {
	ATH_MSG_WARNING( "Unable to retrieve the SCT PRD_MultiTruthCollection" );
	return StatusCode::SUCCESS;
    }
    
    // retrieve map of all charged indet track kine/barcode associations
    // (this is only needed for G3 data)
    //const std::map<int,int>& indetKineMap = m_truthSelector->indetKineMap();
    
    // get hold of Tracks from TDS or return if nothing found/expected
    if (m_tracksLocation == ""
	|| ! evtStore()->contains<iPatTrackContainer>(m_tracksLocation))
    {
	ATH_MSG_DEBUG( "no TrackContainer at " << m_tracksLocation);
	return StatusCode::SUCCESS;
    }
    if (StatusCode::SUCCESS != evtStore()->retrieve(m_tracks, m_tracksLocation))
    {
	ATH_MSG_WARNING( "Unable to retrieve the TrackContainer" );
	return StatusCode::SUCCESS;
    }

    // loop over all tracks in TrackContainer
    for (std::vector<Track*>::const_iterator t = m_tracks->begin();
	 t != m_tracks->end();
	 ++t)
    {
	// barcodeMap - ensure trackBarcode = 0 unless at least 3 hits with same barcode
	std::map<int,int>	trackBarcodeMap;
	int			trackBarcode = 0;
	trackBarcodeMap[trackBarcode] = 1;

	// map the barcode association for the hits
	for (HitList::hit_citerator h = (**t).hit_list_begin();
	     h != (**t).hit_list_end();
	     ++h)
	{
	    if ((**h).isCluster())
	    {
		int barcode = 0;
		std::pair <PRD_MultiTruthCollection::const_iterator,
		           PRD_MultiTruthCollection::const_iterator> range;
		if (simPixClusterMap && (**h).isPixel())
		{
		    range = simPixClusterMap->equal_range((**h).identifier());
		}
		else if (simSctClusterMap)
		{
		    range = simSctClusterMap->equal_range((**h).identifier());
		}
		if (range.first == range.second)
		{
		    ++trackBarcodeMap[barcode];
		}
		else
		{
		    for (PRD_MultiTruthCollection::const_iterator i = range.first;
			 i != range.second;
			 ++i)
		    {
			barcode = i->second.barcode();
			if (trackBarcodeMap.find(barcode) == trackBarcodeMap.end())
			{
		
			    trackBarcodeMap[barcode] = 0;
			}
			else if (barcode)
			{
			    ++trackBarcodeMap[barcode];
			}
		    }
		}
	    }
	}

	// set trackBarcode to maximum #hits in map:
	int	maximum	= 0;
	for (std::map<int,int,std::less<int> >::iterator k = trackBarcodeMap.begin();
	     k != trackBarcodeMap.end();
	     ++k)
	{
	    if ((*k).second > maximum)
	    {
		trackBarcode	= (*k).first;
		maximum		= (*k).second;
	    }
	}

	// set the truth association ...
// 	//  for G3 data, RIO's return kine from the barcode() function, so we need to convert
// 	//  NB trackKine is the true barcode in this case whereas trackBarcode is the kine.
// 	//     This ridiculous convention follows the RTF definition for RIO and fixes the
// 	//     unique/spoilt/wrong logic below for G3/4 compatibility
// 	TruthAssociation* assoc = 0;
// 	if (m_usingG3)
// 	{
// 	    std::map<int,int>::const_iterator b = indetKineMap.find(trackBarcode);
// 	    int trackKine = 0;
// 	    if (b != indetKineMap.end()) trackKine = (*b).second;
// 	    assoc = new TruthAssociation(trackKine,trackBarcode);
// 	}
// 	else
// 	{
	TruthAssociation* assoc = new TruthAssociation(trackBarcode,trackBarcode);
// 	}
	(**t).truthAssociation(assoc);
	
	// finally count spoilt/unique/wrong/shared statistics 
	// according to the barcode association of each hit
	unsigned sharedSilicon	= 0;
	unsigned uniqueSilicon	= 0;
	unsigned wrongSilicon	= 0;
	
	for (HitList::hit_citerator h = (**t).hit_list_begin();
	     h != (**t).hit_list_end();
	     ++h)
	{
	    if ((**h).isPositionMeasurement())
	    {
		int barcode = 0;
		std::pair <PRD_MultiTruthCollection::const_iterator,
		           PRD_MultiTruthCollection::const_iterator> range;
		if (simPixClusterMap && (**h).isPixel())
		{
		    range = simPixClusterMap->equal_range((**h).identifier());
		}
		else if (simSctClusterMap)
		{
		    range = simSctClusterMap->equal_range((**h).identifier());
		}
		int num = 0;
		for (PRD_MultiTruthCollection::const_iterator i = range.first;
		     i != range.second;
		     ++i, ++num)
		{
		    if (barcode != trackBarcode) barcode = i->second.barcode();
		}

		// classify good association with multiple contributions as spoilt
		if (num != 1 && barcode == trackBarcode) barcode = 0;
		
		if (barcode == 0)   		// spoilt hit
		{
		    switch ((**h).status())    
		    {
		    case pixel_cluster:
		    case broad_pixel:
			if ((**h).isShared()) ++sharedSilicon;
			if (inB_Layer(**h)) 
			{
			    assoc->spoiltB_LayerCluster();
			}
			else
			{
			    assoc->spoiltOuterPixelCluster();
			}
			break;
		    case strip_cluster:    
		    case broad_strip:
			if ((**h).isShared()) ++sharedSilicon;
			assoc->spoiltStripCluster();
			break;
		    case drift_time:
			assoc->spoiltDriftHit();
			break;
		    case drift_wire:
			assoc->spoiltDriftHit();
			break;
		    default:
			break;
		    };
		}
		else if (barcode == trackBarcode)	// unique hit
		{
		    switch ((**h).status())    
		    {
		    case pixel_cluster:
		    case broad_pixel:
			++uniqueSilicon;
			if ((**h).isShared()) ++sharedSilicon;
			if (inB_Layer(**h)) 
			{
			    assoc->uniqueB_LayerCluster();
			}
			else
			{
			    assoc->uniqueOuterPixelCluster();
			}
			break;
		    case strip_cluster:    
		    case broad_strip:
			++uniqueSilicon;
			if ((**h).isShared()) ++sharedSilicon;
			assoc->uniqueStripCluster();
			break;
		    case drift_time:
			assoc->uniqueDriftHit();
			break;
		    case drift_wire:
			assoc->uniqueDriftHit();
			break;
		    default:
			break;
		    };
		}
		else   				// wrong hit
		{
		    switch ((**h).status())    
		    {
		    case pixel_cluster:
		    case broad_pixel:
			++wrongSilicon;
			if ((**h).isShared()) ++sharedSilicon;
			if (inB_Layer(**h)) 
			{
			    assoc->wrongB_LayerCluster();
			}
			else
			{
			    assoc->wrongOuterPixelCluster();
			}
			break;
		    case strip_cluster:    
		    case broad_strip:
			++wrongSilicon;
			if ((**h).isShared()) ++sharedSilicon;
			assoc->wrongStripCluster();
			break;
		    case drift_time:
		    case drift_wire:	
			assoc->wrongDriftHit();
			break;
		    default:
			break;
		    };
		}
	    }
	}

	// report possible fakes
	if (! uniqueSilicon
	    || wrongSilicon
	    || sharedSilicon)
	{
	    if (! uniqueSilicon 
		&& ! wrongSilicon
		&& sharedSilicon < 3)
	    {
		// from pile-up or smthg missing from cluster truth map
		++m_backgroundCount;
	    }
	    else if (! uniqueSilicon 
		     && wrongSilicon < 2
		     && sharedSilicon < 2)
	    {
		// as above but with a wrong hit
		++m_wrongCount;
	    }
	    else if (uniqueSilicon < sharedSilicon
		     || uniqueSilicon < wrongSilicon)
	    {
		// these really are fakes
		++m_fakeCount;
	    }
	    else if (wrongSilicon)
	    {
		++m_wrongCount;
	    }
	    
	    if (msgLvl(MSG::DEBUG))
	    {
		ATH_MSG_DEBUG( endreq << " iPatTrackTruthAssociator: possible fake-track with "
			       << uniqueSilicon << " unique, "
			       << wrongSilicon  << " wrong and "
			       << sharedSilicon << " shared SiliconClusters" );
		assoc->print();
		std::cout << " ";
		(**t).print_parameters();
	    }
	}
    }
    
    ATH_MSG_DEBUG( "exit execution" );
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode
iPatTrackTruthAssociator::finalize()
{
    ATH_MSG_INFO( "finalized with "
		  << m_fakeCount << " probable fake tracks, "
		  << m_wrongCount << " tracks with a wrong silicon cluster, and "
		  << m_backgroundCount << " background tracks" );

    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------

bool
iPatTrackTruthAssociator::inB_Layer(const HitOnTrack& hit)
{
    // hardwire max radius for any B_Layer so far ...
    if (hit.isBarrel()
	&& hit.isPixel()
	&& hit.position().perp() < 70.0*Gaudi::Units::mm) return true;

    return false;
}

//-------------------------------------------------------------------------------
