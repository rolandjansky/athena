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

	std::vector<std::vector<std::vector<unsigned int>>> clusterCounter (1000, std::vector<std::vector<unsigned int>> (4, std::vector <unsigned int> (4)));
	std::vector<std::vector<std::vector<unsigned int>>> clusterCounterFront (1000, std::vector<std::vector<unsigned int>> (4, std::vector <unsigned int> (4)));
	std::vector<std::vector<std::vector<unsigned int>>> clusterCounterEnd (1000, std::vector<std::vector<unsigned int>> (4, std::vector <unsigned int> (4)));
	std::vector<std::vector<std::vector<unsigned int>>> clusterCounterMiddle (1000, std::vector<std::vector<unsigned int>> (4, std::vector <unsigned int> (4)));

	std::vector<std::vector<unsigned int>> clusterCounterStation(1000, std::vector<unsigned int>(4));
	std::vector<std::vector<unsigned int>> clusterCounterStationFront(1000, std::vector<unsigned int>(4));
	std::vector<std::vector<unsigned int>> clusterCounterStationEnd(1000, std::vector<unsigned int>(4));
	std::vector<std::vector<unsigned int>> clusterCounterStationMiddle(1000, std::vector<unsigned int>(4));

	int previouslb = 0;
	int previouslbFront = 0;
	int previouslbEnd = 0;
	int previouslbMiddle = 0;
	
	int previouslbStation = 0;
	int previouslbStationFront = 0;
	int previouslbStationEnd = 0;
	int previouslbStationMiddle = 0;

	unsigned int counterForEvents = 0;
	unsigned int counterForEventsFront = 0;
	unsigned int counterForEventsEnd = 0;
	unsigned int counterForEventsMiddle = 0;
	
	unsigned int counterForEventsStation = 0;
	unsigned int counterForEventsStationFront = 0;
	unsigned int counterForEventsStationEnd = 0;
	unsigned int counterForEventsStationMiddle = 0;
	
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

	m_HitmapGroups = buildToolMap<std::map<std::string,int>>(m_tools,"AFPSiLayerTool", m_stationnames, m_pixlayers);
	m_TrackGroup = buildToolMap<int>(m_tools, "AFPSiLayerTool", m_stationnames);


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
		if(!bcData->isFilled(temp-1))
		{
			frontBCIDsVector.push_back(temp);
			++counterForEventsFront;
			++counterForEventsStationFront;
		}
		else
		{
			if(bcData->isFilled(temp+1))
			{
				middleBCIDsVector.push_back(temp);
				++counterForEventsMiddle;
				++counterForEventsStationMiddle;
			}
			else
			{
				endBCIDsVector.push_back(temp);
				++counterForEventsEnd;
				++counterForEventsStationEnd;
			}
		}
	}
	
	// Declare the quantities which should be monitored:
	auto lb = Monitored::Scalar<int>("lb", 0);
	auto muPerBCID = Monitored::Scalar<float>("muPerBCID", 0.0);
	auto run = Monitored::Scalar<int>("run",0);
	auto weight = Monitored::Scalar<float>("weight", 1.0);

	auto nSiHits = Monitored::Scalar<int>("nSiHits", 1);

	auto pixelRowIDChip = Monitored::Scalar<int>("pixelRowIDChip", 0); 
	auto pixelColIDChip = Monitored::Scalar<int>("pixelColIDChip", 0); 

	auto timeOverThreshold = Monitored::Scalar<float>("timeOverThreshold", 0.0);

	auto clusterX = Monitored::Scalar<float>("clusterX", 0.0);
	auto clusterY = Monitored::Scalar<float>("clusterY", 0.0); 

	auto trackX = Monitored::Scalar<float>("trackX", 0.0);
	auto trackY = Monitored::Scalar<float>("trackY", 0.0);

	auto layerEfficiency = Monitored::Scalar<float>("layerEfficiency", 0.0);
	auto layerNumber = Monitored::Scalar<int>("layerNumber", 0);
	
	
	lb = GetEventInfo(ctx)->lumiBlock();
	muPerBCID = lbAverageInteractionsPerCrossing(ctx);
	//run = GetEventInfo(ctx)->runNumber();
	fill("AFPSiLayerTool", lb, muPerBCID);
	

	++counterForEvents;		
	++counterForEventsStation;
	
	SG::ReadHandle<xAOD::AFPSiHitContainer> afpHitContainer(m_afpHitContainerKey, ctx);
	if(! afpHitContainer.isValid())
	{
		ATH_MSG_WARNING("evtStore() does not contain hits collection with name " << m_afpHitContainerKey);
		return StatusCode::SUCCESS;
	}

	ATH_CHECK( afpHitContainer.initialize() );

	nSiHits = afpHitContainer->size();
	fill("AFPSiLayerTool", lb, nSiHits);


	for(const xAOD::AFPSiHit *hitsItr: *afpHitContainer)
	{
		lb = GetEventInfo(ctx)->lumiBlock();
		pixelRowIDChip = hitsItr->pixelRowIDChip();
		pixelColIDChip = hitsItr->pixelColIDChip();
		timeOverThreshold = hitsItr->timeOverThreshold();
		
		if (hitsItr->stationID()<4 && hitsItr->stationID()>=0 && hitsItr->pixelLayerID()<4 && hitsItr->pixelLayerID()>=0) 
		{
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelRowIDChip, pixelColIDChip);
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelRowIDChip);
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelColIDChip);
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], timeOverThreshold);
			
		}
		else ATH_MSG_WARNING("Unrecognised station index: " << hitsItr->stationID());
	}
	
	// Filling of cluster and track 2D histograms
	AFPMon::AFPFastReco fast(afpHitContainer.get());
	fast.reco();

	for (const auto& track : fast.tracks()) 
	{
		trackX = track.x;
		trackY = track.y;
		fill(m_tools[m_TrackGroup.at(m_stationnames.at(track.station))], trackY, trackX);
	}

	// Clusters and synch histograms:

	for(const auto& cluster : fast.clusters()) 
	{
		clusterX = cluster.x;
		clusterY = cluster.y;
		fill(m_tools[m_HitmapGroups.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], clusterY, clusterX); 
		lb = GetEventInfo(ctx)->lumiBlock();
		
		
		fillSynchHistogramsPlane(lb, previouslb, clusterCounter, counterForEvents, muPerBCID, cluster, 'P');
		fillSynchHistogramsStation(lb, previouslbStation, clusterCounterStation, counterForEventsStation, muPerBCID, cluster, 'S');
		
		if (isInListVector(GetEventInfo(ctx)->bcid(), frontBCIDsVector))
		{
			fillSynchHistogramsStation(lb, previouslbStationFront, clusterCounterStationFront, counterForEventsStationFront, muPerBCID, cluster, 'F');
			fillSynchHistogramsPlane(lb, previouslbFront, clusterCounterFront, counterForEventsFront, muPerBCID, cluster, 'F');
		}
		else if (isInListVector(GetEventInfo(ctx)->bcid(), middleBCIDsVector))
		{
			fillSynchHistogramsStation(lb, previouslbStationMiddle, clusterCounterStationMiddle, counterForEventsStationMiddle, muPerBCID, cluster, 'M');
			fillSynchHistogramsPlane(lb, previouslbMiddle, clusterCounterMiddle, counterForEventsMiddle, muPerBCID, cluster, 'M');
		}
		else if (isInListVector(GetEventInfo(ctx)->bcid(), endBCIDsVector))
		{
			fillSynchHistogramsStation(lb, previouslbStationEnd, clusterCounterStationEnd, counterForEventsStationEnd, muPerBCID, cluster, 'E');
			fillSynchHistogramsPlane(lb, previouslbEnd, clusterCounterEnd, counterForEventsEnd, muPerBCID, cluster, 'E');
		}
	}

	return StatusCode::SUCCESS;
} // end of fillHistograms

void AFPSiLayerAlgorithm::fillSynchHistogramsStation(Monitored::Scalar<int> &lb, int &previouslbStationA, std::vector<std::vector<unsigned int>> &clusterCounterStationA, unsigned int &counterForEventsStationA, float &muPerBCID, const AFPMon::AFPCluster& cluster, char histogramType) const
{
	//using namespace Monitored;
	
	float clustersPerStationFloat = 0;
	if(lb > previouslbStationA && previouslbStationA != 0)
	{
		for(int i = 0; i < 4; i++)
		{
			clustersPerStationFloat = clusterCounterStationA[previouslbStationA][i]*1.0;
			if(muPerBCID != 0)
			{
				clustersPerStationFloat = clustersPerStationFloat/(muPerBCID*counterForEventsStationA*4);
			}
			else{clustersPerStationFloat = -0.1;}

			if(histogramType == 'S')
			{
				auto clustersPerStation = Monitored::Scalar<float>("clustersPerStation", 0.0);
				clustersPerStation = clustersPerStationFloat;
				fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStation);
			}
			else if (histogramType == 'F')
			{
				auto clustersPerStationFront = Monitored::Scalar<float>("clustersPerStationFront", 0.0);
				clustersPerStationFront = clustersPerStationFloat;
				fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStationFront);
			}
			else if (histogramType == 'M')
			{
				auto clustersPerStationMiddle = Monitored::Scalar<float>("clustersPerStationMiddle", 0.0);
				clustersPerStationMiddle = clustersPerStationFloat;
				fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStationMiddle);
			}
			else if (histogramType == 'E')
			{
				auto clustersPerStationEnd = Monitored::Scalar<float>("clustersPerStationEnd", 0.0);
				clustersPerStationEnd = clustersPerStationFloat;
				fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStationEnd);
			}
		}
		previouslbStationA=lb;
		++clusterCounterStationA[lb][cluster.station];
		counterForEventsStationA=1;
	}
	else if (clusterCounterStationA[lb][cluster.station] == 0)
	{
		++clusterCounterStationA[lb][cluster.station];
		previouslbStationA = lb;
	}
	else if (lb==previouslbStationA)
	{++clusterCounterStationA[lb][cluster.station];}
}


void AFPSiLayerAlgorithm::fillSynchHistogramsPlane(Monitored::Scalar<int> &lb, int &previouslbPlane, std::vector<std::vector<std::vector<unsigned int>>> &clusterCounterPlane, unsigned int &counterForEventsPlane, float &muPerBCID, const AFPMon::AFPCluster& cluster, char histogramType) const
{
	float clustersPerPlaneFloat = 0;
	if(lb > previouslbPlane && previouslbPlane != 0)
	{
		for(int i=0; i<4; i++)
		{
			for(int j=0; j<4; j++)
			{
				clustersPerPlaneFloat = clusterCounterPlane[previouslbPlane][i][j]*1.0;
				if(muPerBCID != 0)
				{
					clustersPerPlaneFloat = clustersPerPlaneFloat/(muPerBCID*counterForEventsPlane);
				}
				else
				{
					clustersPerPlaneFloat = -0.1;
				}
				
				if(histogramType == 'P')
				{
					auto clustersPerPlane = Monitored::Scalar<float>("clustersPerPlane", 0.0);
					clustersPerPlane = clustersPerPlaneFloat;
					fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlane);
				}
				else if(histogramType == 'F')
				{
					auto clustersPerPlaneFront = Monitored::Scalar<float>("clustersPerPlaneFront", 0.0);
					clustersPerPlaneFront = clustersPerPlaneFloat;
					fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlaneFront);
				}
				else if(histogramType == 'M')
				{
					auto clustersPerPlaneMiddle = Monitored::Scalar<float>("clustersPerPlaneMiddle", 0.0);
					clustersPerPlaneMiddle = clustersPerPlaneFloat;
					fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlaneMiddle);
				}
				else if(histogramType == 'E')
				{
					auto clustersPerPlaneEnd = Monitored::Scalar<float>("clustersPerPlaneEnd", 0.0);
					clustersPerPlaneEnd = clustersPerPlaneFloat;
					fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlaneEnd);
				}
					
			}
		}
		previouslbPlane = lb;
		++clusterCounterPlane[lb][cluster.station][cluster.layer];
		counterForEventsPlane=1;
	}
	// First time in lumiblock (in plane)
	else if(clusterCounterPlane[lb][cluster.station][cluster.layer] == 0) 
	{
		++clusterCounterPlane[lb][cluster.station][cluster.layer];
		previouslbPlane = lb;
	}
	
	// Lumiblock is same, so proceed
	else if(lb==previouslbPlane)	// Same lumiblock
	{
		++clusterCounterPlane[lb][cluster.station][cluster.layer];
	}
	
}


