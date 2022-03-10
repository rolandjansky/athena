/* 
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*
*
*	AFPSiLayerAlgorithm
*
*
*/

#include "Run3AFPMonitoring/AFPSiLayerAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODForward/AFPStationID.h"
#include "EventInfo/EventID.h"


namespace {
int reorganizePlanes(const int station, const int layer)
{
	if(station == 0 || station == 1)
	{
		return (station*4)+3-(layer);
	}
	return 0;
}
}


AFPSiLayerAlgorithm::AFPSiLayerAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
, m_afpHitContainerKey("AFPSiHitContainer")
{
	declareProperty("AFPSiHitContainer", m_afpHitContainerKey);
}


AFPSiLayerAlgorithm::~AFPSiLayerAlgorithm() {}


StatusCode AFPSiLayerAlgorithm::initialize() {

	using namespace Monitored;

	m_StationPlaneGroup = buildToolMap<std::map<std::string,int>>(m_tools,"AFPSiLayerTool", m_stationnames, m_pixlayers);
	m_StationGroup = buildToolMap<int>(m_tools, "AFPSiLayerTool", m_stationnames);


	// We must declare to the framework in initialize what SG objects we are going to use:
	SG::ReadHandleKey<xAOD::AFPSiHitContainer> afpHitContainerKey("AFPSiHits");
	ATH_CHECK(m_afpHitContainerKey.initialize());
	
	ATH_MSG_INFO( "BunchCrossingKey initialization (SiT)" );
	ATH_CHECK(m_bunchCrossingKey.initialize());
	ATH_MSG_INFO( "initialization completed (SiT)" );
	return AthMonitorAlgorithm::initialize();
}

StatusCode AFPSiLayerAlgorithm::fillHistograms( const EventContext& ctx ) const {
	using namespace Monitored;
	
	auto bcidAll    = Monitored::Scalar<int>("bcidAll", 0);
	auto bcidFront  = Monitored::Scalar<int>("bcidFront", 0);
	auto bcidMiddle = Monitored::Scalar<int>("bcidMiddle", 0);
	auto bcidEnd    = Monitored::Scalar<int>("bcidEnd", 0);
	
	auto numberOfEventsPerLumiblockFront  = Monitored::Scalar<int>("numberOfEventsPerLumiblockFront", 0);
	auto numberOfEventsPerLumiblockMiddle = Monitored::Scalar<int>("numberOfEventsPerLumiblockMiddle", 0);
	auto numberOfEventsPerLumiblockEnd    = Monitored::Scalar<int>("numberOfEventsPerLumiblockEnd", 0);

	SG::ReadHandle<xAOD::EventInfo> eventInfo = GetEventInfo(ctx);
	numberOfEventsPerLumiblockFront   = eventInfo->lumiBlock();
	numberOfEventsPerLumiblockMiddle  = eventInfo->lumiBlock();
	numberOfEventsPerLumiblockEnd     = eventInfo->lumiBlock();

	// BCX handler
	const unsigned int bcid = eventInfo->bcid();
	SG::ReadCondHandle<BunchCrossingCondData> bcidHdl(m_bunchCrossingKey,ctx);
	if (!bcidHdl.isValid()) {
		ATH_MSG_ERROR( "Unable to retrieve BunchCrossing conditions object (SiT)" );
	}
	const BunchCrossingCondData* bcData{*bcidHdl};

	// Classifying bunches by position in train (Front, Middle, End)
        enum { FRONT, MIDDLE, END, NPOS } position = NPOS;
	if(bcData->isFilled(bcid))
	{
		bcidAll = bcid;
		fill("AFPSiLayerTool", bcidAll);
		if(!bcData->isFilled(bcid-1))
		{
			position = FRONT;
			bcidFront = bcid;
			fill("AFPSiLayerTool", bcidFront);
			fill("AFPSiLayerTool", numberOfEventsPerLumiblockFront);
		}
		else
		{
			if(bcData->isFilled(bcid+1))
			{
				position = MIDDLE;
				bcidMiddle = bcid;
				fill("AFPSiLayerTool", bcidMiddle);
				fill("AFPSiLayerTool", numberOfEventsPerLumiblockMiddle);
			}
			else
			{
				position = END;
				bcidEnd = bcid;
				fill("AFPSiLayerTool", bcidEnd);
				fill("AFPSiLayerTool", numberOfEventsPerLumiblockEnd);
			}
		}
	}
	
		
	// Declare the quantities which should be monitored:
	auto lb       = Monitored::Scalar<int>("lb", 0);
	auto muPerBX  = Monitored::Scalar<float>("muPerBX", 0.0);
	//auto run = Monitored::Scalar<int>("run",0);

	auto nSiHits = Monitored::Scalar<int>("nSiHits", 1);

	auto pixelRowIDChip = Monitored::Scalar<int>("pixelRowIDChip", 0); 
	auto pixelColIDChip = Monitored::Scalar<int>("pixelColIDChip", 0); 

	auto timeOverThreshold = Monitored::Scalar<int>("timeOverThreshold", 0);

	auto clusterX = Monitored::Scalar<float>("clusterX", 0.0);
	auto clusterY = Monitored::Scalar<float>("clusterY", 0.0); 
	auto clustersInPlanes = Monitored::Scalar<int>("clustersInPlanes", 0);

	auto trackX = Monitored::Scalar<float>("trackX", 0.0);
	auto trackY = Monitored::Scalar<float>("trackY", 0.0);
	
	auto planeHits        = Monitored::Scalar<int>("planeHits", 0);
	auto planeHitsAll     = Monitored::Scalar<int>("planeHitsAll", 0);
	auto planeHitsAllMU   = Monitored::Scalar<int>("planeHitsAllMU", 0);
	auto weightAllPlanes  = Monitored::Scalar<float>("weightAllPlanes", 1.0);
	
	auto numberOfHitsPerStation = Monitored::Scalar<int>("numberOfHitsPerStation", 0);
	
	auto lbEvents             = Monitored::Scalar<int>("lbEvents", 0);
	auto lbHits               = Monitored::Scalar<int>("lbHits", 0);
	auto lbEventsStations     = Monitored::Scalar<int>("lbEventsStations", 0);
	auto lbEventsStationsAll  = Monitored::Scalar<int>("lbEventsStationsAll", 0);
	
	auto lbClustersPerPlanes  = Monitored::Scalar<int>("lbClustersPerPlanes", 0);
	auto weightClustersByMU   = Monitored::Scalar<float>("weightClustersByMU", 1.0);
	
	auto lbClustersPerPlanes_full = Monitored::Scalar<int>("lbClustersPerPlanes_full", 0);
	
	auto clustersPerPlaneFrontPP        = Monitored::Scalar<int>("clustersPerPlaneFrontPP", 0);
	auto clustersPerPlaneMiddlePP       = Monitored::Scalar<int>("clustersPerPlaneMiddlePP", 0);
	auto clustersPerPlaneEndPP          = Monitored::Scalar<int>("clustersPerPlaneEndPP", 0);
	auto weightClustersPerPlaneFrontPP  = Monitored::Scalar<float>("weightClustersPerPlaneFrontPP", 1.0);
	auto weightClustersPerPlaneMiddlePP = Monitored::Scalar<float>("weightClustersPerPlaneMiddlePP", 1.0);
	auto weightClustersPerPlaneEndPP    = Monitored::Scalar<float>("weightClustersPerPlaneEndPP", 1.0);

	auto clustersPerPlaneFrontPP_full   = Monitored::Scalar<int>("clustersPerPlaneFrontPP_full", 0);
	auto clustersPerPlaneMiddlePP_full  = Monitored::Scalar<int>("clustersPerPlaneMiddlePP_full", 0);
	auto clustersPerPlaneEndPP_full     = Monitored::Scalar<int>("clustersPerPlaneEndPP_full", 0);
	
	auto lbHitsPerPlanes      = Monitored::Scalar<int>("lbHitsPerPlanes", 0);
	auto lbHitsPerPlanes_full = Monitored::Scalar<float>("lbHitsPerPlanes_full", 0.0);
	auto weightHitsByMU       = Monitored::Scalar<float>("weightHitsByMU", 1.0);
	
	auto hitsCounterPlanesTProfile    = Monitored::Scalar<int>("hitsCounterPlanesTProfile", 0.0);
	auto hitsCounterStationsTProfile  = Monitored::Scalar<int>("hitsCounterStationsTProfile", 0.0);
	
	auto planes = Monitored::Scalar<int>("planes", 0);
	
	auto eventsPerStation = Monitored::Scalar<int>("eventsPerStation", 0);
	
	auto clusterToT = Monitored::Scalar<int>("clusterToT", 0);
	
	lb        = eventInfo->lumiBlock();
	lbEvents  = eventInfo->lumiBlock();
	muPerBX   = lbAverageInteractionsPerCrossing(ctx);
	fill("AFPSiLayerTool", lb, muPerBX);
	fill("AFPSiLayerTool", lbEvents);
	
	
	SG::ReadHandle<xAOD::AFPSiHitContainer> afpHitContainer(m_afpHitContainerKey, ctx);
	if(! afpHitContainer.isValid())
	{
		ATH_MSG_WARNING("evtStore() does not contain hits collection with name " << m_afpHitContainerKey);
		return StatusCode::SUCCESS;
	}

	ATH_CHECK( afpHitContainer.initialize() );

	nSiHits = afpHitContainer->size();
	fill("AFPSiLayerTool", lb, nSiHits);

	int eventsInStations[4] = {};
	int numberOfHitsPerPlane[4][4] = {};

	for(const xAOD::AFPSiHit *hitsItr: *afpHitContainer)
	{
		lb                  = eventInfo->lumiBlock();
		lbHits              = eventInfo->lumiBlock();
		lbEventsStations    = eventInfo->lumiBlock();
		lbEventsStationsAll = eventInfo->lumiBlock();
		pixelRowIDChip      = hitsItr->pixelRowIDChip();
		pixelColIDChip      = hitsItr->pixelColIDChip();
		timeOverThreshold   = hitsItr->timeOverThreshold();
		
		
		if (hitsItr->stationID()<4 && hitsItr->stationID()>=0 && hitsItr->pixelLayerID()<4 && hitsItr->pixelLayerID()>=0) 
		{
			++eventsInStations[hitsItr->stationID()];

			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelRowIDChip, pixelColIDChip);
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelRowIDChip);
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelColIDChip);
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], timeOverThreshold);
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], lb, hitsCounterPlanesTProfile);
			
			if(hitsItr->stationID() == 0 || hitsItr->stationID() == 1)
			{
				planeHits = reorganizePlanes(0, hitsItr->pixelLayerID());
			}
			else
			{
				planeHits = hitsItr->pixelLayerID();
			}
			fill(m_tools[m_StationGroup.at(m_stationnames.at(hitsItr->stationID()))], planeHits);
			
			++numberOfHitsPerPlane[hitsItr->stationID()][hitsItr->pixelLayerID()];
			if(hitsItr->stationID() == 0 || hitsItr->stationID() == 1)
			{
				planeHitsAll = reorganizePlanes(hitsItr->stationID(), hitsItr->pixelLayerID());
				planeHitsAllMU = reorganizePlanes(hitsItr->stationID(), hitsItr->pixelLayerID());
			}
			else
			{
				planeHitsAll = (hitsItr->stationID())*4+hitsItr->pixelLayerID();
				planeHitsAllMU = (hitsItr->stationID())*4+hitsItr->pixelLayerID();
			}
			weightAllPlanes = 1 / muPerBX;
			fill("AFPSiLayerTool", planeHitsAll);
			fill("AFPSiLayerTool", planeHitsAllMU, weightAllPlanes);
			weightAllPlanes = 1.0;
			
			numberOfHitsPerStation = hitsItr->stationID();
			fill("AFPSiLayerTool", numberOfHitsPerStation);
			
			fill("AFPSiLayerTool", lbHits);
			
			lbHitsPerPlanes       = eventInfo->lumiBlock();
			lbHitsPerPlanes_full  = eventInfo->lumiBlock();
			weightHitsByMU = 1 / muPerBX;
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], lbHitsPerPlanes, weightHitsByMU);
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], lbHitsPerPlanes_full);
			weightHitsByMU = 1.0;
		}
		else ATH_MSG_WARNING("Unrecognised station index: " << hitsItr->stationID());
	}
			
	bool noEventsInStations = true;
	for(int i=0; i<4; i++)
	{
		if(eventsInStations[i]>0) {
			fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lbEventsStations);
			
			eventsPerStation = i * 4;
			fill("AFPSiLayerTool", eventsPerStation);
			++eventsPerStation;
			fill("AFPSiLayerTool", eventsPerStation);
			++eventsPerStation;
			fill("AFPSiLayerTool", eventsPerStation);
			++eventsPerStation;
			fill("AFPSiLayerTool", eventsPerStation);

			noEventsInStations = false;
		}
	}
	if(!noEventsInStations)
	{
		fill("AFPSiLayerTool", lbEventsStationsAll);
	}
	
	for(int i=0; i<4; i++)
	{
		hitsCounterStationsTProfile = numberOfHitsPerPlane[i][0] + numberOfHitsPerPlane[i][1] + numberOfHitsPerPlane[i][2] + numberOfHitsPerPlane[i][3];
		fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lb, hitsCounterStationsTProfile);
		for(int j=0; j<4; j++)
		{
			if(numberOfHitsPerPlane[i][j]>0)
			{
				hitsCounterPlanesTProfile = numberOfHitsPerPlane[i][j];
				fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, hitsCounterPlanesTProfile);
			}
		}
	}
	
	// Filling of cluster and track 2D histograms
	AFPMon::AFPFastReco fast(afpHitContainer.get());
	fast.reco();

	// Track histograms:
	unsigned int totalTracksAll[4] = {};
	unsigned int totalTracksFront[4] = {};
	unsigned int totalTracksMiddle[4] = {};
	unsigned int totalTracksEnd[4] = {};
	
	for (const auto& track : fast.tracks()) 
	{
		trackX = track.x * 1.0;
		trackY = track.y * 1.0;
		fill(m_tools[m_StationGroup.at(m_stationnames.at(track.station))], trackY, trackX);
		
		if (position == FRONT)
		{
			++totalTracksFront[track.station];
			++totalTracksAll[track.station];
		}
		else if (position == MIDDLE)
		{
			++totalTracksMiddle[track.station];
			++totalTracksAll[track.station];
		}
		else if (position == END)
		{
			++totalTracksEnd[track.station];
			++totalTracksAll[track.station];
		}
	}
	
	auto weightTracksAll    = Monitored::Scalar<float>("weightTracksAll", 0.0);
	auto weightTracksFront  = Monitored::Scalar<float>("weightTracksFront", 0.0);
	auto weightTracksMiddle = Monitored::Scalar<float>("weightTracksMiddle", 0.0);
	auto weightTracksEnd    = Monitored::Scalar<float>("weightTracksEnd", 0.0);
	
	auto lbTracksAll        = Monitored::Scalar<int>("lbTracksAll", 0);
	auto lbTracksFront      = Monitored::Scalar<int>("lbTracksFront", 0);
	auto lbTracksMiddle     = Monitored::Scalar<int>("lbTracksMiddle", 0);
	auto lbTracksEnd        = Monitored::Scalar<int>("lbTracksEnd", 0);
	
	auto weightTracksAll_full     = Monitored::Scalar<int>("weightTracksAll_full", 0);
	auto weightTracksFront_full   = Monitored::Scalar<int>("weightTracksFront_full", 0);
	auto weightTracksMiddle_full  = Monitored::Scalar<int>("weightTracksMiddle_full", 0);
	auto weightTracksEnd_full     = Monitored::Scalar<int>("weightTracksEnd_full", 0);
	
	auto lbTracksAll_full     = Monitored::Scalar<int>("lbTracksAll_full", 0);
	auto lbTracksFront_full   = Monitored::Scalar<int>("lbTracksFront_full", 0);
	auto lbTracksMiddle_full  = Monitored::Scalar<int>("lbTracksMiddle_full", 0);
	auto lbTracksEnd_full     = Monitored::Scalar<int>("lbTracksEnd_full", 0);
	
	lbTracksAll     = eventInfo->lumiBlock();
	lbTracksFront   = eventInfo->lumiBlock();
	lbTracksMiddle  = eventInfo->lumiBlock();
	lbTracksEnd     = eventInfo->lumiBlock();
	
	lbTracksAll_full      = eventInfo->lumiBlock();
	lbTracksFront_full    = eventInfo->lumiBlock();
	lbTracksMiddle_full   = eventInfo->lumiBlock();
	lbTracksEnd_full      = eventInfo->lumiBlock();
	
	lbTracksAll_full    = eventInfo->lumiBlock();
	lbTracksFront_full  = eventInfo->lumiBlock();
	lbTracksMiddle_full = eventInfo->lumiBlock();
	lbTracksEnd_full    = eventInfo->lumiBlock();
	
	for(int i = 0; i < 4; i++)
	{
		weightTracksAll = totalTracksAll[i] / muPerBX;
		weightTracksAll_full = totalTracksAll[i];
		fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lbTracksAll, weightTracksAll);
		fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lbTracksAll_full, weightTracksAll_full);
		totalTracksAll[i] = 0;
		
		weightTracksFront = totalTracksFront[i] / muPerBX;
		weightTracksFront_full = totalTracksFront[i];
		fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lbTracksFront, weightTracksFront);
		fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lbTracksFront_full, weightTracksFront_full);
		totalTracksFront[i] = 0;
		
		weightTracksMiddle = totalTracksMiddle[i] / muPerBX;
		weightTracksMiddle_full = totalTracksMiddle[i];
		fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lbTracksMiddle, weightTracksMiddle);
		fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lbTracksMiddle_full, weightTracksMiddle_full);
		totalTracksMiddle[i] = 0;
		
		weightTracksEnd = totalTracksEnd[i] / muPerBX;
		weightTracksEnd_full = totalTracksEnd[i];
		fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lbTracksEnd, weightTracksEnd);
		fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lbTracksEnd_full, weightTracksEnd_full);
		totalTracksEnd[i] = 0;
	}
	
	// Cluster histograms 
	for(const auto& cluster : fast.clusters()) 
	{
		clusterX = cluster.x * 1.0;
		clusterY = cluster.y * 1.0;
		fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], clusterY, clusterX);
		if (cluster.station == 0 || cluster.station == 1)
		{
			clustersInPlanes = reorganizePlanes(cluster.station, cluster.layer);
		}
		else
		{
			clustersInPlanes = (cluster.station*4)+cluster.layer;
		}
		fill("AFPSiLayerTool", clustersInPlanes);
		
		clusterToT = cluster.sumToT;
		fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], clusterToT);
		
		lbClustersPerPlanes = eventInfo->lumiBlock();
		lbClustersPerPlanes_full = eventInfo->lumiBlock();
		weightClustersByMU = 1/muPerBX;
		fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], lbClustersPerPlanes, weightClustersByMU);
		fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], lbClustersPerPlanes_full);
		weightClustersByMU = 1.0;
	}
	return StatusCode::SUCCESS;
} // end of fillHistograms


