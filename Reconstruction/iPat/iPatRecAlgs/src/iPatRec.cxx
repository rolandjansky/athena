/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 iPatRec Algorithm = an athena wrapper to iPatTrackFinder plus initialization
 of the class structure required for track finding, following and fitting.
 
 ATLAS Collaboration
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODEventInfo/EventInfo.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "iPatInterfaces/IDetectorSelection.h"
#include "iPatInterfaces/ITRT_DataSelector.h"
#include "iPatInterfaces/ITrackFinder.h"
#include "iPatInterfaces/ITrackFollower.h"
#include "iPatInterfaces/ITrackQuality.h"
#include "iPatRecAlgs/iPatRec.h"
#include "iPatRecEvent/iPatTrackContainer.h"
#include "iPatTrack/HitOnTrack.h"
#include "iPatTrack/Track.h"
#include "iPatTrackParameters/PerigeeParameters.h"
#include "iPatUtility/VertexRegion.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//------------------------------------------------------------------------

iPatRec::iPatRec(const std::string& name, ISvcLocator* pSvcLocator)
    :	AthAlgorithm			(name, pSvcLocator),
	m_detectorSelection		("DetectorSelection/DetectorSelection"),
	m_fitter			("Trk::iPatFitter/iPatFitter"),
	m_trackFinder			("TrackFinder/TrackFinder"),
	m_trackFollower			("TrackFollower/TrackFollower"),
	m_trackQuality			("TrackQuality/TrackQuality"),
	m_trtDataSelector		("TRT_DataSelector/TRT_DataSelector"),
	m_maxSecondaryImpact		(100.0*Gaudi::Units::mm),
	m_minPt				(0.4*Gaudi::Units::GeV),
	m_overlapSpacePointsLocation	("OverlapSpacePoints"),
	m_pixelClustersLocation		("PixelClusters"),
	m_pixelSpacePointsLocation	("PixelSpacePoints"),
	m_printLevel	       		(0),
	m_sctClustersLocation		("SCT_Clusters"),
	m_sctSpacePointsLocation	("SCT_SpacePoints"),
	m_slimOutputTracks		(false),
	m_tracksLocation		("ConvertedIPatTracks"),
	m_iPatTracksLocation		("iPatTracks"),
	m_vertexRegion			(0),
	m_trackList			(0),
	m_primaryCounter		(0),
	m_secondaryCounter		(0),
	m_truncatedCounter		(0)
{
    // defaults for vector properties:
    double vertex[3]		= {0.0*Gaudi::Units::mm, 0.0*Gaudi::Units::mm, 0.0*Gaudi::Units::mm};
    m_vertex			= std::vector<double>(&vertex[0],&vertex[3]);
    double vertexWidth[3]	= {1.0*Gaudi::Units::mm, 1.0*Gaudi::Units::mm, 250.0*Gaudi::Units::mm};
    m_vertexWidth		= std::vector<double>(&vertexWidth[0],&vertexWidth[3]);
    
    declareProperty("DetectorSelection",		m_detectorSelection);
    declareProperty("Fitter",				m_fitter);
    declareProperty("TrackFinder",			m_trackFinder);
    declareProperty("TrackFollower",			m_trackFollower);
    declareProperty("TrackQuality",			m_trackQuality);
    declareProperty("TRT_DataSelector",			m_trtDataSelector);
    declareProperty("maxSecondaryImpact",		m_maxSecondaryImpact);
    declareProperty("minPt",				m_minPt);
    declareProperty("overlapSpacePointsLocation",	m_overlapSpacePointsLocation);
    declareProperty("pixelClustersLocation",		m_pixelClustersLocation);
    declareProperty("pixelSpacePointsLocation",		m_pixelSpacePointsLocation);
    declareProperty("printLevel",			m_printLevel);
    declareProperty("sctClustersLocation",		m_sctClustersLocation);
    declareProperty("sctSpacePointsLocation",		m_sctSpacePointsLocation);
    declareProperty("slimOutputTracks",			m_slimOutputTracks);
    declareProperty("SpacePointsLocation",		m_spacePointsLocation);
    declareProperty("TracksLocation",			m_tracksLocation);
    declareProperty("iPatTracksLocation",		m_iPatTracksLocation);
    declareProperty("vertex",				m_vertex);
    declareProperty("vertexWidth",			m_vertexWidth);
}

//--------------------------------------------------------------------------
iPatRec::~iPatRec(void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

//-----------------------------------------------------------------------
StatusCode
iPatRec::initialize()
{
    // print where you are
    ATH_MSG_INFO( "iPatRec::initialize() - package version " << PACKAGE_VERSION );

    // get the Tools
    if (m_detectorSelection.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_detectorSelection );
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << m_detectorSelection );

    if (m_fitter.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_fitter );
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << m_fitter );

    if (m_trackFinder.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackFinder );
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << m_trackFinder );
    
    if (m_trackFollower.retrieve().isFailure())
    {
    	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackFollower );
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << m_trackFollower );

    if (m_trackQuality.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackQuality );
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << m_trackQuality );

    if (! m_trtDataSelector.empty())
    {
	if (m_trtDataSelector.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_trtDataSelector );
	    return StatusCode::FAILURE;
	}
	ATH_MSG_INFO( "Retrieved tool " << m_trtDataSelector );
    }
    
    // check that some space point containers are selected
    if (m_pixelSpacePointsLocation == ""
	&& m_sctSpacePointsLocation == ""
	&& m_overlapSpacePointsLocation == "")
    {
        ATH_MSG_FATAL( "No location given set for space points" );
        return StatusCode::FAILURE;
    }
    if (m_iPatTracksLocation == "")
    {
	ATH_MSG_FATAL( "No location given for iPat Tracks" );
	return StatusCode::FAILURE;
    }
    if (! (m_detectorSelection->processPixels() || m_detectorSelection->processSCT()))
    {
        ATH_MSG_FATAL( "No input specified for iPatRec" );
        return StatusCode::FAILURE;
    }

    // configure minPt in TrackQuality tool
    m_trackQuality->minPt(m_minPt);
  
    // set some configuration to the ipat trackFinder
    // specify the vertex region from which primary tracks originate
    m_vertexRegion	= new VertexRegion(Amg::Vector3D(m_vertex[0],
							 m_vertex[1],
							 m_vertex[2]),
					   Amg::Vector3D(m_vertexWidth[0],
							 m_vertexWidth[1],
							 m_vertexWidth[2]));
    m_trackFinder->setProperties(m_maxSecondaryImpact,
				 m_minPt,
				 m_printLevel,
				 *m_vertexRegion);
    
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
    
StatusCode
iPatRec::execute()
{
    bool	process = true;  // gets set false when obligatory input data is missing
    if (msgLvl(MSG::DEBUG))
    {
	const xAOD::EventInfo* eventInfo;
	if (StatusCode::SUCCESS != evtStore()->retrieve(eventInfo))
	{
	    ATH_MSG_WARNING( "Could not retrieve event info" );
	}
	else
	{	
	    ATH_MSG_DEBUG( "entered execution for run "
			   << eventInfo->runNumber() << "   event "
			   << eventInfo->eventNumber() );
	}
    }
	
    // retrieve pixel container
    const InDet::SiClusterContainer* pixelContainer = 0;
    if (m_detectorSelection->processPixels()
	&& StatusCode::SUCCESS != evtStore()->retrieve(pixelContainer, m_pixelClustersLocation))
	ATH_MSG_WARNING( "Unable to retrieve the PixelCluster container" );

    // retrieve SCT container
    const InDet::SiClusterContainer* sctContainer = 0;
    if (m_detectorSelection->processSCT()
	&& StatusCode::SUCCESS != evtStore()->retrieve(sctContainer, m_sctClustersLocation))
	ATH_MSG_WARNING( "Unable to retrieve the SCT_Cluster container" );

    m_trackFollower->setDataContainers(pixelContainer,sctContainer);
    
    // get new SpacePoints
    std::vector<Trk::SpacePoint*>	spacePoints;
    const SpacePointContainer*		pixelSPContainer	= 0;
    const SpacePointContainer*		sctSPContainer		= 0;
    const SpacePointOverlapCollection*	overlapCollection	= 0;
    if (m_detectorSelection->processPixels()
	&& StatusCode::SUCCESS != evtStore()->retrieve(pixelSPContainer,m_pixelSpacePointsLocation))
    {
	ATH_MSG_DEBUG( "Unable to retrieve PixelSpacePoint container" );
    }
    if (m_detectorSelection->processSCT()
	&& StatusCode::SUCCESS != evtStore()->retrieve(sctSPContainer,m_sctSpacePointsLocation))
    {
	ATH_MSG_DEBUG( "Unable to retrieve SCT_SpacePoint container" );
    }
    if (m_detectorSelection->processSCT()
	&& StatusCode::SUCCESS != evtStore()->retrieve(overlapCollection,m_overlapSpacePointsLocation))
    {
	ATH_MSG_DEBUG( "Unable to retrieve Overlap SpacePoint container" );
    }
    
    if (! pixelSPContainer && ! sctSPContainer && ! overlapCollection)
    {
	ATH_MSG_WARNING( "Unable to retrieve any SpacePoint containers" );
	process = false;
    }

    if (m_detectorSelection->processTRT())
    {
	// retrieve TRT_DriftCircle container
	const InDet::TRT_DriftCircleContainer* driftCircleContainer = 0; 
	if (m_detectorSelection->processTRT()
	    && StatusCode::SUCCESS != evtStore()->retrieve(driftCircleContainer, "TRT_DriftCircles"))
	{
	    ATH_MSG_WARNING( "could not retrieve TRT_DriftCircles" );
	}
	m_trtDataSelector->setDataContainer(driftCircleContainer); 
    }
    
    if (process)
    {
	if (pixelSPContainer)
	{
	    for (SpacePointContainer::const_iterator
		     colNext = pixelSPContainer->begin();
		 colNext != pixelSPContainer->end();
		 ++colNext)
	    {
		for (SpacePointCollection::const_iterator sp = (**colNext).begin();
		     sp != (**colNext).end();
		     ++sp)
		    spacePoints.push_back(*sp);
	    }
	}
	if (sctSPContainer)
	{
	    for (SpacePointContainer::const_iterator
		     colNext = sctSPContainer->begin();
		 colNext != sctSPContainer->end();
		 ++colNext)
	    {
		for (SpacePointCollection::const_iterator sp = (**colNext).begin();
		     sp != (**colNext).end();
		     ++sp)
		    spacePoints.push_back(*sp);
	    }
	}
	if (overlapCollection)
	{
	    for (SpacePointCollection::const_iterator sp = overlapCollection->begin();
		 sp != overlapCollection->end();
		 ++sp)
		spacePoints.push_back(*sp);
	}
    }

    ATH_MSG_DEBUG( spacePoints.size() << " SpacePoints found" );

    // finally find tracks (produce at least an empty container)
    iPatTrackContainer* tracks = new iPatTrackContainer;
    TrackCollection* trkTracks = new TrackCollection;
    unsigned primaryCounter	= 0;
    unsigned truncatedCounter	= 0;
    unsigned secondaryCounter	= 0;
    
    if (process)
    {
	m_trackList       	=  m_trackFinder->trackSearch(spacePoints);
	for (track_list::iterator t = m_trackList->begin();
	     t != m_trackList->end();
	     ++t)
	{
	    tracks->push_back(*t);
	    Trk::Track* convertedTrack = convertTrack(**t);
	    if (convertedTrack) trkTracks->push_back(convertedTrack);
	    
	    switch ((**t).status())    
	    {
	    case primary:
		++primaryCounter;
		break;
	    case truncated:
		++truncatedCounter;
		break;
	    default:
		++secondaryCounter;
		break;
	    };
	}
	
	delete m_trackList;
	if (m_printLevel > 1) tracks->print();
    }
    
    if (StatusCode::SUCCESS != evtStore()->record(trkTracks, m_tracksLocation, false))
    {
	ATH_MSG_FATAL( "Could not record Trk::Tracks" );
	return StatusCode::FAILURE;
    }
    
    if (StatusCode::SUCCESS != evtStore()->record(tracks, m_iPatTracksLocation, false))
    {
	ATH_MSG_FATAL( "Could not record iPatTracks" );
	return StatusCode::FAILURE;
    }

    // keep some statistics
    m_primaryCounter	+= primaryCounter;
    m_truncatedCounter	+= truncatedCounter;
    m_secondaryCounter	+= secondaryCounter;
    ATH_MSG_DEBUG( "exit iPatRec with "
		   << (primaryCounter+truncatedCounter+secondaryCounter)
		   << " tracks found, of which " 
		   << primaryCounter << " primaries, "
		   << truncatedCounter << " truncated, and "
		   << secondaryCounter << " secondaries " );
    
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode
iPatRec::finalize()
{
    delete m_vertexRegion;

    ATH_MSG_INFO( "finalised with "
		  << (m_primaryCounter+m_truncatedCounter+m_secondaryCounter)
		  << " tracks found, of which " 
		  << m_primaryCounter << " primaries, "
		  << m_truncatedCounter << " truncated, and "
		  << m_secondaryCounter << " secondaries " );
  
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

Trk::Track*
iPatRec::convertTrack (Track& track)
{
    // create Trk::Track from HitList
    //   reserve a TSOS for each hot + 1 for the perigee
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces =
 	new DataVector<const Trk::TrackStateOnSurface>;
    trackStateOnSurfaces->reserve(static_cast<unsigned>(1 + track.hits_for_edit().size()));

    // start with perigee parameters
    const PerigeeParameters&    perigeeParameters = track.perigee_parameters();
    const Trk::Perigee* perigee = new Trk::Perigee(perigeeParameters.transverseImpact(),
						   perigeeParameters.z(),
						   perigeeParameters.phi(),
						   perigeeParameters.theta(),
						   perigeeParameters.qOverP(),
						   perigeeParameters.vertex());
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    trackStateOnSurfaces->push_back(new const Trk::TrackStateOnSurface(0,
								       perigee,
								       0,
								       0,
								       typePattern));
    
    for (hit_const_iterator h = track.hit_list_begin(); h != track.hit_list_end(); ++h)
    {
	const Trk::FitQualityOnSurface*	fitQoS			= 0;
	const Trk::MaterialEffectsOnTrack* materialEffects	= 0;
	const Trk::TrackParameters* trackParameters		= 0;
	std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
	if ((**h).rot())
	{
	    typePattern.set(Trk::TrackStateOnSurface::Measurement);
	    if ((**h).isClusterOutlier()) typePattern.set(Trk::TrackStateOnSurface::Outlier);
	    trackStateOnSurfaces->push_back(new const Trk::TrackStateOnSurface((**h).rot()->clone(),
									       trackParameters,
									       fitQoS,
									       materialEffects,
									       typePattern));
	} 
    }

    // set patternRecoInfo
    Trk::TrackInfo info(Trk::TrackInfo::Unknown,Trk::pion);
    info.setPatternRecognitionInfo(Trk::TrackInfo::iPatRec);
    Trk::Track unfittedTrack(info,trackStateOnSurfaces,0);

    // perform fit
    Trk::Track* convertedTrack	= m_fitter->fit(unfittedTrack, false, Trk::pion);
    if (! convertedTrack) return 0;


    Trk::Track* returnTrack = convertedTrack;
    if (m_slimOutputTracks)
    {	
	// slim
	DataVector<const Trk::TrackStateOnSurface>* slimTSOS =
	    new DataVector<const Trk::TrackStateOnSurface>;
	for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
		 s = unfittedTrack.trackStateOnSurfaces()->begin();
	     s != unfittedTrack.trackStateOnSurfaces()->end();
	     ++s)
	{
	    if (s == unfittedTrack.trackStateOnSurfaces()->begin())
	    {
		slimTSOS->push_back(convertedTrack->trackStateOnSurfaces()->front()->clone());
	    }
	    else
	    {
		slimTSOS->push_back((**s).clone());
	    }
	}
	returnTrack = new Trk::Track(info,slimTSOS,convertedTrack->fitQuality()->clone());
	delete convertedTrack;
    }

    return returnTrack;
}

