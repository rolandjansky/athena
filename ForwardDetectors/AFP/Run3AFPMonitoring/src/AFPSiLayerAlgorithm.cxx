/* 
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


#include <vector>

std::vector<int> frontBCIDsVector;
std::vector<int> middleBCIDsVector;
std::vector<int> endBCIDsVector;


bool isInListVector(const int bcid, const std::vector<int>&arr)
{
	return std::find_if(arr.begin(),arr.end(),[&bcid](const int& ele){return ele==bcid;})!= arr.end();
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
	
	ATH_MSG_INFO( "BunchCrossingKey initialization" );
	ATH_CHECK(m_bunchCrossingKey.initialize());
	ATH_MSG_INFO( "initialization completed" );
	return AthMonitorAlgorithm::initialize();
}

StatusCode AFPSiLayerAlgorithm::fillHistograms( const EventContext& ctx ) const {
	using namespace Monitored;
	
	auto bcidAll = Monitored::Scalar<int>("bcidAll", 0);
	auto bcidFront = Monitored::Scalar<int>("bcidFront", 0);
	auto bcidMiddle = Monitored::Scalar<int>("bcidMiddle", 0);
	auto bcidEnd = Monitored::Scalar<int>("bcidEnd", 0);
	
	auto numberOfEventsPerLumiblockFront = Monitored::Scalar<int>("numberOfEventsPerLumiblockFront", 0);
	auto numberOfEventsPerLumiblockMiddle = Monitored::Scalar<int>("numberOfEventsPerLumiblockMiddle", 0);
	auto numberOfEventsPerLumiblockEnd = Monitored::Scalar<int>("numberOfEventsPerLumiblockEnd", 0);
	
	numberOfEventsPerLumiblockFront = GetEventInfo(ctx)->lumiBlock();
	numberOfEventsPerLumiblockMiddle = GetEventInfo(ctx)->lumiBlock();
	numberOfEventsPerLumiblockEnd = GetEventInfo(ctx)->lumiBlock();

	// BCX handler
	unsigned int temp = GetEventInfo(ctx)->bcid();
	SG::ReadCondHandle<BunchCrossingCondData> bcidHdl(m_bunchCrossingKey,ctx);
	if (!bcidHdl.isValid()) {
		ATH_MSG_ERROR( "Unable to retrieve BunchCrossing conditions object" );
	}
	const BunchCrossingCondData* bcData{*bcidHdl};

	// Classifying bunches by position in train (Front, Middle, End)
	if(bcData->isFilled(temp))
	{
		bcidAll = temp;
		fill("AFPSiLayerTool", bcidAll);
		if(!bcData->isFilled(temp-1))
		{
			if(!isInListVector(temp, frontBCIDsVector))
			{
				frontBCIDsVector.push_back(temp);
			}
			bcidFront = temp;
			fill("AFPSiLayerTool", bcidFront);
			fill("AFPSiLayerTool", numberOfEventsPerLumiblockFront);
		}
		else
		{
			if(bcData->isFilled(temp+1))
			{
				if(!isInListVector(temp, middleBCIDsVector))
				{
					middleBCIDsVector.push_back(temp);
				}
				bcidMiddle = temp;
				fill("AFPSiLayerTool", bcidMiddle);
				fill("AFPSiLayerTool", numberOfEventsPerLumiblockMiddle);
			}
			else
			{
				if(!isInListVector(temp, endBCIDsVector))
				{
					endBCIDsVector.push_back(temp);
				}
				bcidEnd = temp;
				fill("AFPSiLayerTool", bcidEnd);
				fill("AFPSiLayerTool", numberOfEventsPerLumiblockEnd);
			}
		}
	}
	
		
	// Declare the quantities which should be monitored:
	auto lb = Monitored::Scalar<int>("lb", 0);
	auto muPerBX = Monitored::Scalar<float>("muPerBX", 0.0);
	//auto run = Monitored::Scalar<int>("run",0);

	auto nSiHits = Monitored::Scalar<int>("nSiHits", 1);

	auto pixelRowIDChip = Monitored::Scalar<int>("pixelRowIDChip", 0); 
	auto pixelColIDChip = Monitored::Scalar<int>("pixelColIDChip", 0); 

	auto timeOverThreshold = Monitored::Scalar<float>("timeOverThreshold", 0.0);

	auto clusterX = Monitored::Scalar<float>("clusterX", 0.0);
	auto clusterY = Monitored::Scalar<float>("clusterY", 0.0); 
	auto clustersInPlanes = Monitored::Scalar<int>("clustersInPlanes", 0);

	auto trackX = Monitored::Scalar<float>("trackX", 0.0);
	auto trackY = Monitored::Scalar<float>("trackY", 0.0);
	
	auto planeHits = Monitored::Scalar<int>("planeHits", 0);
	auto planeHitsAll = Monitored::Scalar<int>("planeHitsAll", 0);
	auto planeHitsAllMU = Monitored::Scalar<int>("planeHitsAllMU", 0);
	auto weightAllPlanes = Monitored::Scalar<float>("weightAllPlanes", 1.0);
	
	auto numberOfHitsPerStation = Monitored::Scalar<int>("numberOfHitsPerStation", 0);
	
	auto lbEvents = Monitored::Scalar<int>("lbEvents", 0);
	auto lbHits = Monitored::Scalar<int>("lbHits", 0);
	auto lbEventsStations = Monitored::Scalar<int>("lbEventsStations", 0);
	auto lbEventsStationsAll = Monitored::Scalar<int>("lbEventsStationsAll", 0);
	
	
	auto lbClustersPerPlanes = Monitored::Scalar<int>("lbClustersPerPlanes", 0);
	auto weightClustersByMU = Monitored::Scalar<float>("weightClustersByMU", 1.0);
	
	auto lbClustersPerPlanes_full = Monitored::Scalar<int>("lbClustersPerPlanes_full", 0);
	
	auto clustersPerPlaneFrontPP = Monitored::Scalar<int>("clustersPerPlaneFrontPP", 0);
	auto clustersPerPlaneMiddlePP = Monitored::Scalar<int>("clustersPerPlaneMiddlePP", 0);
	auto clustersPerPlaneEndPP = Monitored::Scalar<int>("clustersPerPlaneEndPP", 0);
	auto weightClustersPerPlaneFrontPP = Monitored::Scalar<float>("weightClustersPerPlaneFrontPP", 1.0);
	auto weightClustersPerPlaneMiddlePP = Monitored::Scalar<float>("weightClustersPerPlaneMiddlePP", 1.0);
	auto weightClustersPerPlaneEndPP = Monitored::Scalar<float>("weightClustersPerPlaneEndPP", 1.0);

	auto clustersPerPlaneFrontPP_full = Monitored::Scalar<int>("clustersPerPlaneFrontPP_full", 0);
	auto clustersPerPlaneMiddlePP_full = Monitored::Scalar<int>("clustersPerPlaneMiddlePP_full", 0);
	auto clustersPerPlaneEndPP_full = Monitored::Scalar<int>("clustersPerPlaneEndPP_full", 0);
	
	auto lbHitsPerPlanes = Monitored::Scalar<int>("lbHitsPerPlanes", 0);
	auto lbHitsPerPlanes_full = Monitored::Scalar<float>("lbHitsPerPlanes_full", 0.0);
	auto weightHitsByMU = Monitored::Scalar<float>("weightHitsByMU", 1.0);
	
	auto hitsCounterPlanesTProfile = Monitored::Scalar<int>("hitsCounterPlanesTProfile", 0.0);
	auto hitsCounterStationsTProfile = Monitored::Scalar<int>("hitsCounterStationsTProfile", 0.0);
	
	auto planes = Monitored::Scalar<int>("planes", 0);
	
	auto eventsPerStation = Monitored::Scalar<int>("eventsPerStation", 0);
	
	lb = GetEventInfo(ctx)->lumiBlock();
	lbEvents = GetEventInfo(ctx)->lumiBlock();
	muPerBX = lbAverageInteractionsPerCrossing(ctx);
	//run = GetEventInfo(ctx)->runNumber();
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

	std::vector<int>stationValues;
	std::vector<int>hitsPerPlanes(16);
	std::vector<std::vector<int>> numberOfHitsPerPlane(4, std::vector<int>(4));

	for(const xAOD::AFPSiHit *hitsItr: *afpHitContainer)
	{
		lb = GetEventInfo(ctx)->lumiBlock();
		lbHits = GetEventInfo(ctx)->lumiBlock();
		lbEventsStations = GetEventInfo(ctx)->lumiBlock();
		lbEventsStationsAll = GetEventInfo(ctx)->lumiBlock();
		pixelRowIDChip = hitsItr->pixelRowIDChip();
		pixelColIDChip = hitsItr->pixelColIDChip();
		timeOverThreshold = hitsItr->timeOverThreshold();
		
		stationValues.push_back(hitsItr->stationID());
		
		if (hitsItr->stationID()<4 && hitsItr->stationID()>=0 && hitsItr->pixelLayerID()<4 && hitsItr->pixelLayerID()>=0) 
		{
			
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelRowIDChip, pixelColIDChip);
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelRowIDChip);
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelColIDChip);
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], timeOverThreshold);
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], lb, hitsCounterPlanesTProfile);
			
			planeHits = hitsItr->pixelLayerID();
			fill(m_tools[m_StationGroup.at(m_stationnames.at(hitsItr->stationID()))], planeHits);
			
			++numberOfHitsPerPlane[hitsItr->stationID()][hitsItr->pixelLayerID()];
			planeHitsAll = (hitsItr->stationID())*4+hitsItr->pixelLayerID();
			planeHitsAllMU = (hitsItr->stationID())*4+hitsItr->pixelLayerID();
			weightAllPlanes = 1 / muPerBX;
			fill("AFPSiLayerTool", planeHitsAll);
			fill("AFPSiLayerTool", planeHitsAllMU, weightAllPlanes);
			weightAllPlanes = 1.0;
			
			++hitsPerPlanes[(hitsItr->stationID())*4+hitsItr->pixelLayerID()];
			
			numberOfHitsPerStation = hitsItr->stationID();
			fill("AFPSiLayerTool", numberOfHitsPerStation);
			
			fill("AFPSiLayerTool", lbHits);
			
			lbHitsPerPlanes = GetEventInfo(ctx)->lumiBlock();
			lbHitsPerPlanes_full = GetEventInfo(ctx)->lumiBlock();
			weightHitsByMU = 1 / muPerBX;
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], lbHitsPerPlanes, weightHitsByMU);
			fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], lbHitsPerPlanes_full);
			weightHitsByMU = 1.0;
		}
		else ATH_MSG_WARNING("Unrecognised station index: " << hitsItr->stationID());
	}
	
		std::vector<int> indicatorForEventsInStations(4);
		if(!stationValues.empty())
		{
			fill("AFPSiLayerTool", lbEventsStationsAll);
		}
		
		if(isInListVector(0, stationValues))
		{
			eventsPerStation = 0;
			++indicatorForEventsInStations[eventsPerStation];
			fill("AFPSiLayerTool", eventsPerStation);

			eventsPerStation = 1;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 2;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 3;
			fill("AFPSiLayerTool", eventsPerStation);
		}
		if(isInListVector(1, stationValues))
		{
			eventsPerStation = 1;
			++indicatorForEventsInStations[eventsPerStation];
			
			eventsPerStation = 4;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 5;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 6;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 7;
			fill("AFPSiLayerTool", eventsPerStation);
		}
		if(isInListVector(2, stationValues))
		{
			eventsPerStation = 2;
			++indicatorForEventsInStations[eventsPerStation];
			
			eventsPerStation = 8;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 9;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 10;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 11;
			fill("AFPSiLayerTool", eventsPerStation);
		}
		if(isInListVector(3, stationValues))
		{
			eventsPerStation = 3;
			++indicatorForEventsInStations[eventsPerStation];

			eventsPerStation = 12;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 13;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 14;
			fill("AFPSiLayerTool", eventsPerStation);
			eventsPerStation = 15;
			fill("AFPSiLayerTool", eventsPerStation);
		}
		
		for(int i=0; i<4; i++)
		{
			if(indicatorForEventsInStations[i]>0)
				fill(m_tools[m_StationGroup.at(m_stationnames.at(i))], lbEventsStations);
		}
	indicatorForEventsInStations.clear();
	stationValues.clear();
	
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
			
			numberOfHitsPerPlane[i][j] = 0;
		}
	}
	
	// Filling of cluster and track 2D histograms
	AFPMon::AFPFastReco fast(afpHitContainer.get());
	fast.reco();

	// Track histograms:
	std::vector<unsigned int> totalTracksAll(4);
	std::vector<unsigned int> totalTracksFront(4);
	std::vector<unsigned int> totalTracksMiddle(4);
	std::vector<unsigned int> totalTracksEnd(4);
	
	for (const auto& track : fast.tracks()) 
	{
		trackX = track.x * 1.0;
		trackY = track.y * 1.0;
		fill(m_tools[m_StationGroup.at(m_stationnames.at(track.station))], trackY, trackX);
		
		if (isInListVector(GetEventInfo(ctx)->bcid(), frontBCIDsVector))
		{
			++totalTracksFront[track.station];
			++totalTracksAll[track.station];
		}
		else if (isInListVector(GetEventInfo(ctx)->bcid(), middleBCIDsVector))
		{
			++totalTracksMiddle[track.station];
			++totalTracksAll[track.station];
		}
		else if (isInListVector(GetEventInfo(ctx)->bcid(), endBCIDsVector))
		{
			++totalTracksEnd[track.station];
			++totalTracksAll[track.station];
		}
	}
	
	auto weightTracksAll = Monitored::Scalar<float>("weightTracksAll", 0.0);
	auto weightTracksFront = Monitored::Scalar<float>("weightTracksFront", 0.0);
	auto weightTracksMiddle = Monitored::Scalar<float>("weightTracksMiddle", 0.0);
	auto weightTracksEnd = Monitored::Scalar<float>("weightTracksEnd", 0.0);
	
	auto lbTracksAll = Monitored::Scalar<int>("lbTracksAll", 0);
	auto lbTracksFront = Monitored::Scalar<int>("lbTracksFront", 0);
	auto lbTracksMiddle = Monitored::Scalar<int>("lbTracksMiddle", 0);
	auto lbTracksEnd = Monitored::Scalar<int>("lbTracksEnd", 0);
	
	auto weightTracksAll_full = Monitored::Scalar<int>("weightTracksAll_full", 0);
	auto weightTracksFront_full = Monitored::Scalar<int>("weightTracksFront_full", 0);
	auto weightTracksMiddle_full = Monitored::Scalar<int>("weightTracksMiddle_full", 0);
	auto weightTracksEnd_full = Monitored::Scalar<int>("weightTracksEnd_full", 0);
	
	auto lbTracksAll_full = Monitored::Scalar<int>("lbTracksAll_full", 0);
	auto lbTracksFront_full = Monitored::Scalar<int>("lbTracksFront_full", 0);
	auto lbTracksMiddle_full = Monitored::Scalar<int>("lbTracksMiddle_full", 0);
	auto lbTracksEnd_full = Monitored::Scalar<int>("lbTracksEnd_full", 0);
	
	lbTracksAll = GetEventInfo(ctx)->lumiBlock();
	lbTracksFront = GetEventInfo(ctx)->lumiBlock();
	lbTracksMiddle = GetEventInfo(ctx)->lumiBlock();
	lbTracksEnd = GetEventInfo(ctx)->lumiBlock();
	
	lbTracksAll_full = GetEventInfo(ctx)->lumiBlock();
	lbTracksFront_full = GetEventInfo(ctx)->lumiBlock();
	lbTracksMiddle_full = GetEventInfo(ctx)->lumiBlock();
	lbTracksEnd_full = GetEventInfo(ctx)->lumiBlock();
	
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
		clustersInPlanes = (cluster.station*4)+cluster.layer;
		fill("AFPSiLayerTool", clustersInPlanes);
		
		lbClustersPerPlanes = GetEventInfo(ctx)->lumiBlock();
		lbClustersPerPlanes_full = GetEventInfo(ctx)->lumiBlock();
		weightClustersByMU = 1/muPerBX;
		fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], lbClustersPerPlanes, weightClustersByMU);
		fill(m_tools[m_StationPlaneGroup.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], lbClustersPerPlanes_full);
		weightClustersByMU = 1.0;
	}
	return StatusCode::SUCCESS;
} // end of fillHistograms


