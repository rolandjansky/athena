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
	
	//void fillSynchHistograms(int &lbA, int &previouslbStationA, float &clustersPerStationA, std::vector<std::vector<unsigned int>> &clusterCounterStationA, unsigned int &counterForEventsStationA, float &muPerBCIDA, auto &cluster, std::map<std::string,int> &mapa);

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
	auto clustersPerPlane = Monitored::Scalar<float>("clustersPerPlane", 1.0);
	auto clustersPerPlane2 = Monitored::Scalar<float>("clustersPerPlane2", 0.0);
	auto clustersPerPlaneFront = Monitored::Scalar<float>("clustersPerPlaneFront", 0.0);
	auto clustersPerPlaneEnd = Monitored::Scalar<float>("clustersPerPlaneEnd", 0.0);
	auto clustersPerPlaneMiddle = Monitored::Scalar<float>("clustersPerPlaneMiddle", 0.0);
	
	//auto clustersPerStation = Monitored::Scalar<float>("clustersPerStation", 0.0);
	//auto clustersPerStationFront = Monitored::Scalar<float>("clustersPerStationFront", 0.0);
	auto clustersPerStationEnd = Monitored::Scalar<float>("clustersPerStationEnd", 0.0);
	auto clustersPerStationMiddle = Monitored::Scalar<float>("clustersPerStationMiddle", 0.0);

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

	// Clusters:

	for(const auto& cluster : fast.clusters()) 
	{
		clusterX = cluster.x;
		clusterY = cluster.y;
		fill(m_tools[m_HitmapGroups.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], clusterY, clusterX); 
		lb = GetEventInfo(ctx)->lumiBlock();
		
		// Time for fill - current and previous lb are different, and the previouslb is not -1 (it means - this is not the first lb)
		if(lb > previouslb && previouslb != 0)
		{
			for(int i=0; i<4; i++)
			{
				for(int j=0; j<4; j++)
				{
					clustersPerPlane = clusterCounter[previouslb][i][j]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerPlane = clustersPerPlane/(muPerBCID*counterForEvents);
					}
					else
					{
						clustersPerPlane = -0.1;
					}
					fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlane);
				}
			}
				
			previouslb = lb;
			++clusterCounter[lb][cluster.station][cluster.layer];
			counterForEvents = 1;
		}
			
			// First time in lumiblock (in plane)
		else if(clusterCounter[lb][cluster.station][cluster.layer] == 0) 
		{
			++clusterCounter[lb][cluster.station][cluster.layer];
			previouslb = lb;
		}
			
		// Lumiblock is same, so proceed
		else if(lb==previouslb)	// Same lumiblock
		{++clusterCounter[lb][cluster.station][cluster.layer];}
			
	}	// ..... end cluster loop .....
		
		
		
	// =============== Stations all BCIDs ===============
	for(const auto& cluster : fast.clusters()) 
	{
		
		lb = GetEventInfo(ctx)->lumiBlock();
		/*
		if(lb > previouslbStation && previouslbStation != 0)
		{
			for(int i=0; i<4; i++)
			{
				clustersPerStation = clusterCounterStation[previouslbStation][i]*1.0;
				if(muPerBCID != 0)
				{clustersPerStation = clustersPerStation/(muPerBCID*counterForEventsStation*4);}
				else{clustersPerStation = -0.1;}

				fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStation);
			}
			previouslbStation=lb;
			++clusterCounterStation[lb][cluster.station];
			counterForEventsStation=1;
		}
		else if (clusterCounterStation[lb][cluster.station] == 0)
		{
			++clusterCounterStation[lb][cluster.station];
			previouslbStation = lb;
		}
		else if (lb==previouslbStation)
		{++clusterCounterStation[lb][cluster.station];}
		*/
		fillSynchHistogramsStation(lb, previouslbStation, clusterCounterStation, counterForEventsStation, muPerBCID, cluster, 'S');
	
	}
	// ========== Front Station ==========
	if(isInListVector(GetEventInfo(ctx)->bcid(), frontBCIDsVector))
	{
		for(const auto& cluster : fast.clusters()) 
		{
			lb = GetEventInfo(ctx)->lumiBlock();
			/*
			if(lb > previouslbStationFront && previouslbStationFront != 0)
			{
				for(int i=0; i<4; i++)
				{
					clustersPerStationFront = clusterCounterStationFront[previouslbStationFront][i]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerStationFront = clustersPerStationFront/(muPerBCID*counterForEventsStationFront*4);
					}
					else{clustersPerStationFront = -0.1;}
					
					fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStationFront);
				}
				previouslbStationFront=lb;
				++clusterCounterStationFront[lb][cluster.station];
				counterForEventsStationFront=1;
			}
			else if (clusterCounterStationFront[lb][cluster.station] == 0)
			{
				++clusterCounterStationFront[lb][cluster.station];
				previouslbStationFront = lb;
			}
			else if (lb == previouslbStationFront)
			{++clusterCounterStationFront[lb][cluster.station];}
			*/
			fillSynchHistogramsStation(lb, previouslbStationFront, clusterCounterStationFront, counterForEventsStationFront, muPerBCID, cluster, 'F');
		}
	}
	// ..... end front station .....
			
	// ========== End Station ==========
	if(isInListVector(GetEventInfo(ctx)->bcid(), endBCIDsVector))
	{	
		for(const auto& cluster : fast.clusters())
		{
			lb = GetEventInfo(ctx)->lumiBlock();
			/*
			if(lb > previouslbStationEnd && previouslbStationEnd != 0)
			{
				for(int i=0; i<4; i++)
				{
					clustersPerStationEnd = clusterCounterStationEnd[previouslbStationEnd][i]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerStationEnd = clustersPerStationEnd/(muPerBCID*counterForEventsStationEnd*4);
					}
					else{clustersPerStationEnd = -0.1;}
				
					fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStationEnd);
				}
				previouslbStationEnd=lb;
				++clusterCounterStationEnd[lb][cluster.station];
				counterForEventsStationEnd=1;
			}
			else if (clusterCounterStationEnd[lb][cluster.station] == 0)
			{
				++clusterCounterStationEnd[lb][cluster.station];
				previouslbStationEnd = lb;
			}
			else if (lb == previouslbStationEnd)
			{++clusterCounterStationEnd[lb][cluster.station];}
			*/
			fillSynchHistogramsStation(lb, previouslbStationEnd, clusterCounterStationEnd, counterForEventsStationEnd, muPerBCID, cluster, 'E');
		}
	}
	// ..... end end station .....
	
	// ========== Middle Station ==========
	if(isInListVector(GetEventInfo(ctx)->bcid(), middleBCIDsVector))
	{
		for(const auto& cluster : fast.clusters())
		{
		/*
			if(lb > previouslbStationMiddle && previouslbStationMiddle != 0)
			{
				for(int i=0; i<4; i++)
				{
					clustersPerStationMiddle = clusterCounterStationMiddle[previouslbStationMiddle][i]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerStationMiddle = clustersPerStationMiddle/(muPerBCID*counterForEventsStationMiddle*4);
					}
					else{clustersPerStationMiddle = -0.1;}
					fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStationMiddle);
				}
				previouslbStationMiddle=lb;
				++clusterCounterStationMiddle[lb][cluster.station];
				counterForEventsStationMiddle=1;
			}
			else if (clusterCounterStationMiddle[lb][cluster.station] == 0)
			{
				++clusterCounterStationMiddle[lb][cluster.station];
				previouslbStationMiddle = lb;
			}
			else if (lb == previouslbStationMiddle)
			{++clusterCounterStationMiddle[lb][cluster.station];}
			// ..... end middle station .....
			*/
			fillSynchHistogramsStation(lb, previouslbStationMiddle, clusterCounterStationMiddle, counterForEventsStationMiddle, muPerBCID, cluster, 'M');
		}
	}
			// ..... end stations .....
		
		// ========== Front BCID ========== (planes)
	if(isInListVector(GetEventInfo(ctx)->bcid(), frontBCIDsVector))
	{
		for(const auto& cluster : fast.clusters()) 
			{
			lb = GetEventInfo(ctx)->lumiBlock();
			// Time for fill - current and previous lb are different, and the previouslbFront is not -1 (it means - this is not the first lb)
			if(lb > previouslbFront && previouslbFront != 0)
			{
				for(int i=0; i<4; i++)
				{
					for(int j=0; j<4; j++)
					{
						clustersPerPlaneFront = clusterCounterFront[previouslbFront][i][j]*1.0;
						if(muPerBCID != 0)
						{
							clustersPerPlaneFront = clustersPerPlaneFront/(muPerBCID*counterForEventsFront);
						}
						else
						{
							clustersPerPlaneFront = -0.1;
						}
						fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlaneFront);
					}
				}
				previouslbFront = lb;
				++clusterCounterFront[lb][cluster.station][cluster.layer];
				counterForEventsFront=1;
			}
		
		// First time in lumiblock (in plane)
			else if(clusterCounterFront[lb][cluster.station][cluster.layer] == 0) 
			{
				++clusterCounterFront[lb][cluster.station][cluster.layer];
				previouslbFront = lb;
			}
		
			// Lumiblock is same, so proceed
			else if(lb==previouslbFront)	// Same lumiblock
			{
				++clusterCounterFront[lb][cluster.station][cluster.layer];
			}
		}
	}
		
	// ========== End BCID ==========
	if(isInListVector(GetEventInfo(ctx)->bcid(), endBCIDsVector))
	{
		for(const auto& cluster : fast.clusters()) 
		{
			lb = GetEventInfo(ctx)->lumiBlock();
			// Time for fill - current and previous lb are different, and the previouslbEnd is not -1 (it means - this is not the first lb)
			if(lb > previouslbEnd && previouslbEnd != 0)
			{
				for(int i=0; i<4; i++)
				{
					for(int j=0; j<4; j++)
					{
						clustersPerPlaneEnd = clusterCounterEnd[previouslbEnd][i][j]*1.0;
						if(muPerBCID != 0)
						{
							clustersPerPlaneEnd = clustersPerPlaneEnd/(muPerBCID*counterForEventsEnd);
						}
						else
						{
							clustersPerPlaneEnd = -0.1;
						}
						fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlaneEnd);
					}
				}
					previouslbEnd = lb;
					++clusterCounterEnd[lb][cluster.station][cluster.layer];
					counterForEventsEnd=1;
			}
			
			// First time in lumiblock (in plane)
			else if(clusterCounterEnd[lb][cluster.station][cluster.layer] == 0) 
			{
				++clusterCounterEnd[lb][cluster.station][cluster.layer];
				previouslbEnd = lb;
			}
			
			// Lumiblock is same, so proceed
			else if(lb==previouslbEnd)	// Same lumiblock
			{
				++clusterCounterEnd[lb][cluster.station][cluster.layer];
			}
		}
	}
		
	// ========== Middle BCID ==========
	if(isInListVector(GetEventInfo(ctx)->bcid(), middleBCIDsVector))
	{
		for(const auto& cluster : fast.clusters()) 
		{
			lb = GetEventInfo(ctx)->lumiBlock();
			// Time for fill - current and previous lb are different, and the previouslbMiddle is not -1 (it means - this is not the first lb)
			if(lb > previouslbMiddle && previouslbMiddle != 0)
			{
				for(int i=0; i<4; i++)
				{
					for(int j=0; j<4; j++)
					{
						clustersPerPlaneMiddle = clusterCounterMiddle[previouslbMiddle][i][j]*1.0;
						if(muPerBCID != 0)
						{
							clustersPerPlaneMiddle = clustersPerPlaneMiddle/(muPerBCID*counterForEventsMiddle);
						}
						else
						{
							clustersPerPlaneMiddle = -0.1;
						}
						fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlaneMiddle);
					}
				}
				previouslbMiddle = lb;
				++clusterCounterMiddle[lb][cluster.station][cluster.layer];
				counterForEventsMiddle=1;
			}
			
			// First time in lumiblock (in plane)
			else if(clusterCounterMiddle[lb][cluster.station][cluster.layer] == 0) 
			{
				++clusterCounterMiddle[lb][cluster.station][cluster.layer];
				previouslbMiddle = lb;
			}
			
			// Lumiblock is same, so proceed
			else if(lb==previouslbMiddle)	// Same lumiblock
			{
				++clusterCounterMiddle[lb][cluster.station][cluster.layer];
			}
		}
	}
	

	return StatusCode::SUCCESS;
} // end of fillHistograms

void AFPSiLayerAlgorithm::fillSynchHistogramsStation(Monitored::Scalar<int> &lb, int &previouslbStationA, std::vector<std::vector<unsigned int>> &clusterCounterStationA, unsigned int &counterForEventsStationA, float &muPerBCIDA, const AFPMon::AFPCluster& cluster, char histogramType) const
{
	//using namespace Monitored;
	
	float clustersPerStationFloat = 0;
	if(lb > previouslbStationA && previouslbStationA != 0)
	{
		for(int i = 0; i < 4; i++)
		{
			clustersPerStationFloat = clusterCounterStationA[previouslbStationA][i]*1.0;
			if(muPerBCIDA != 0)
			{
				clustersPerStationFloat = clustersPerStationFloat/(muPerBCIDA*counterForEventsStationA*4);
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

/*
void AFPSiLayerAlgorithm::fillSynchHistogramsPlane(Monitored::Scalar<int> &lb, int &previouslbStationA, std::vector<std::vector<unsigned int>> &clusterCounterStationA, unsigned int &counterForEventsStationA, float &muPerBCIDA, const AFPMon::AFPCluster& cluster) const
{


}
*/

