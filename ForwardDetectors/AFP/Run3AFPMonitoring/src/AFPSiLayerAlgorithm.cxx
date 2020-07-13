/* 
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*
*
*	AFPSiLayerAlgorithm
*
*
*/

#include "Run3AFPMonitoring/AFPSiLayerAlgorithm.h"
#include <Run3AFPMonitoring/AFPFastReco.h>
#include "StoreGate/ReadHandleKey.h"
#include "xAODForward/AFPStationID.h"


	unsigned int clusterCounter[1000][4][4];
	unsigned int clusterCounterFront[1000][4][4];
	unsigned int clusterCounterEnd[1000][4][4];
	unsigned int clusterCounterMiddle[1000][4][4];
	
	unsigned int clusterCounterStation[1000][4];
	unsigned int clusterCounterStationFront[1000][4];
	unsigned int clusterCounterStationEnd[1000][4];
	unsigned int clusterCounterStationMiddle[1000][4];

	int previouslb = -1;
	int previouslbFront = -1;
	int previouslbEnd = -1;
	int previouslbMiddle = -1;
	
	int previouslbStation = -1;
	int previouslbStationFront = -1;
	int previouslbStationEnd = -1;
	int previouslbStationMiddle = -1;

	unsigned int counterForEvents = 0;
	unsigned int counterForEventsFront = 0;
	unsigned int counterForEventsEnd = 0;
	unsigned int counterForEventsMiddle = 0;
	
	unsigned int counterForEventsStation = 0;
	unsigned int counterForEventsStationFront = 0;
	unsigned int counterForEventsStationEnd = 0;
	unsigned int counterForEventsStationMiddle = 0;
	
	int frontBCIDs[] = {63,75,87,99,114,126,138,150,165,177,189,201,240,252,264,276,291,303,315,327,342,354,366,378,-1};
	
	int middleBCIDs[] = {64,65,66,67,68,69,76,77,78,79,80,81,88,89,90,91,92,93,100,101,102,103,104,105,115,
	116,117,118,119,120,127,128,129,130,131,132,139,140,141,142,143,144,151,152,153,154,155,156,166,167,168,169,170,171,
	178,179,180,181,182,183,190,191,192,193,194,195,202,203,204,205,206,207,241,242,243,244,245,246,253,254,255,256,257,
	258,265,266,267,268,269,270,277,278,279,280,281,282,292,293,294,295,296,297,304,305,306,307,308,309,316,317,318,319,
	320,321,328,329,330,331,332,333,343,344,345,346,347,348,355,356,357,358,359,360,367,368,369,370,371,372,379,380,381,
	382,383,384,-1};
	
	int endBCIDs[] = {70,82,94,106,121,133,145,157,172,184,196,208,247,259,271,283,298,310,322,334,349,361,373,385,-1};
	
	bool isInList(int bcid, int* arr)
	{
		int i=0;
		while(arr[i] != -1)
		{
			if(bcid == arr[i])
			{
				return true;
			}
			i++;
		}
		return false;
	}


AFPSiLayerAlgorithm::AFPSiLayerAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
, m_afpHitContainerKey("AFPSiHitContainer")

{
	declareProperty( "AFPSiHitContainer", m_afpHitContainerKey );
}


AFPSiLayerAlgorithm::~AFPSiLayerAlgorithm() {}


StatusCode AFPSiLayerAlgorithm::initialize() {
	using namespace Monitored;

	m_HitmapGroups = buildToolMap<std::map<std::string,int>>(m_tools,"AFPSiLayerTool", m_stationnames, m_pixlayers);
	m_TrackGroup = buildToolMap<int>(m_tools, "AFPSiLayerTool", m_stationnames);

	// We must declare to the framework in initialize what SG objects we are going to use:
	SG::ReadHandleKey<xAOD::AFPSiHitContainer> afpHitContainerKey("AFPSiHits");
	ATH_CHECK(m_afpHitContainerKey.initialize());
	
	for(int a=0; a<1000; a++)
	{
		for(int b=0;b<4;b++)
		{
			for(int c=0; c<4; c++)
			{
				clusterCounter[a][b][c] = 0;
			}
		}
	}

	return AthMonitorAlgorithm::initialize();
}


StatusCode AFPSiLayerAlgorithm::fillHistograms( const EventContext& ctx ) const {
	using namespace Monitored;

	static unsigned int numberOfClusterStationPlane[4][4] = { {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} };
	//static unsigned int lumiBlocks[100];
	
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
	
	auto clustersPerStation = Monitored::Scalar<float>("clustersPerStation", 0.0);
	auto clustersPerStationFront = Monitored::Scalar<float>("clustersPerStationFront", 0.0);
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

	++counterForEvents;		// Counter for the all BCIDs
	++counterForEventsStation;

	int tempbcid = GetEventInfo(ctx)->bcid();
	if(isInList(tempbcid, frontBCIDs) == true)
	{
		++counterForEventsFront;
		++counterForEventsStationFront;
	} 
	else if (isInList(tempbcid, endBCIDs) == true)
	{
		++counterForEventsEnd;
		++counterForEventsStationEnd;
	}
	
	if(isInList(tempbcid, middleBCIDs) == true)
	{
		++counterForEventsMiddle;
		++counterForEventsStationMiddle;
	}
	//std::cout << "\t\t" << counterForEvents << " lb:" << lb << std::endl;
	

	SG::ReadHandle<xAOD::AFPSiHitContainer> afpHitContainer(m_afpHitContainerKey, ctx);
	if(! afpHitContainer.isValid())
	{
		ATH_MSG_WARNING("evtStore() does not contain hits collection with name " << m_afpHitContainerKey);
		return StatusCode::SUCCESS;
	}

	ATH_CHECK( afpHitContainer.initialize() );

	nSiHits = afpHitContainer->size();
	//hitsPerPlane = afpHitContainer->size();
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
// Davide's approach
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			clustersPerPlane2 = numberOfClusterStationPlane[i][j]*1.0;
			if(muPerBCID != 0)
			{
				clustersPerPlane2 /= (muPerBCID*counterForEvents);
			}
			else
			{
				clustersPerPlane2 = -99;
			}
			//std::cout <<"\t"<< clustersPerPlane2 << std::endl;
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlane2);
		}
	}
// Davide's approach - End

// Kris's approach:
		for(const auto& cluster : fast.clusters()) 
		{
			clusterX = cluster.x;
			clusterY = cluster.y;
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], clusterY, clusterX); // Swap after suggestion
			
			lb = GetEventInfo(ctx)->lumiBlock();
			++numberOfClusterStationPlane[cluster.station][cluster.layer];
			
			// Time for fill - current and previous lb are different, and the previouslb is not -1 (it means - this is not the first lb)
			if(lb != previouslb && previouslb != -1)
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
							clustersPerPlane = -99;
						}
						fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlane);
					}
				}
				
				previouslb = lb;
				++clusterCounter[lb][cluster.station][cluster.layer];
				counterForEvents=1;
				
				for(int i=0; i<4; i++)
				{
					for(int j=0; j<4; j++)
					{
						numberOfClusterStationPlane[i][j] = 0;
					}
				}
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
			
			// =============== Stations ===============
			
			if(lb != previouslbStation && previouslbStation != -1)
			{
				for(int i=0; i<4; i++)
				{
					clustersPerStation = clusterCounterStation[previouslbStation][i]*1.0;
					
					if(muPerBCID != 0)
					{clustersPerStation = clustersPerStation/(muPerBCID*counterForEventsStation*4);}
					else{clustersPerStation = -99;}

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
			
			// ========== Front Station ==========
			if(lb != previouslbStationFront && previouslbStationFront != -1)
			{
				for(int i=0; i<4; i++)
				{
					clustersPerStationFront = clusterCounterStationFront[previouslbStationFront][i]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerStationFront = clustersPerStationFront/(muPerBCID*counterForEventsStationFront*4);
					}
					else{clustersPerStationFront = -99;}
					
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
			// end front station
			
			// ========== End Station ==========
			if(lb != previouslbStationEnd && previouslbStationEnd != -1)
			{
				for(int i=0; i<4; i++)
				{
					clustersPerStationEnd = clusterCounterStationEnd[previouslbStationEnd][i]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerStationEnd = clustersPerStationEnd/(muPerBCID*counterForEventsStationEnd*4);
					}
					else{clustersPerStationEnd = -99;}
					
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
			// end end station
			
			// ========== Middle Station ==========
			if(lb != previouslbStationMiddle && previouslbStationMiddle != -1)
			{
				for(int i=0; i<4; i++)
				{
					clustersPerStationMiddle = clusterCounterStationMiddle[previouslbStationMiddle][i]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerStationMiddle = clustersPerStationMiddle/(muPerBCID*counterForEventsStationMiddle*4);
					}
					else{clustersPerStationMiddle = -99;}
					
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
			// end middle station
			
			// end stations
			
		}	// end cluster loop
		
		// ========== Front BCID ==========
		if(isInList(GetEventInfo(ctx)->bcid(), frontBCIDs))
		{
			for(const auto& cluster : fast.clusters()) 
			{
				lb = GetEventInfo(ctx)->lumiBlock();
				// Time for fill - current and previous lb are different, and the previouslbFront is not -1 (it means - this is not the first lb)
				if(lb != previouslbFront && previouslbFront != -1)
				{
					//std::cout << "\tlb!=previouslbFront" << std::endl;
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
								clustersPerPlaneFront = -99;
							}
							//std::cout << "\tFILL FRONT" << std::endl;
							//std::cout << "clustersPerPlaneFront (it was zero on cernbox): " << clustersPerPlaneFront << std::endl;
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
				//std::cout << "\tFirst time" << std::endl;
				//std::cout << "\tLuminosity block: " << lb << std::endl;
				}
			
				// Lumiblock is same, so proceed
				else if(lb==previouslbFront)	// Same lumiblock
				{
					++clusterCounterFront[lb][cluster.station][cluster.layer];
				}
			}
		}
		
		// ========== End BCID ==========
		if(isInList(GetEventInfo(ctx)->bcid(), endBCIDs))
		{
			for(const auto& cluster : fast.clusters()) 
			{
				lb = GetEventInfo(ctx)->lumiBlock();
				// Time for fill - current and previous lb are different, and the previouslbEnd is not -1 (it means - this is not the first lb)
				if(lb != previouslbEnd && previouslbEnd != -1)
				{
					//std::cout << "\tlb!=previouslbEnd" << std::endl;
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
								clustersPerPlaneEnd = -99;
							}
							//std::cout << "\tFILL END" << std::endl;
							//std::cout << "clustersPerPlaneEnd (it was zero on cernbox): " << clustersPerPlaneEnd << std::endl;
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
				//std::cout << "\tFirst time" << std::endl;
				//std::cout << "\tLuminosity block: " << lb << std::endl;
				}
			
				// Lumiblock is same, so proceed
				else if(lb==previouslbEnd)	// Same lumiblock
				{
					++clusterCounterEnd[lb][cluster.station][cluster.layer];
				}
			}
		}
		
		// ========== Middle BCID ==========
		if(isInList(GetEventInfo(ctx)->bcid(), middleBCIDs))
		{
			for(const auto& cluster : fast.clusters()) 
			{
				lb = GetEventInfo(ctx)->lumiBlock();
				// Time for fill - current and previous lb are different, and the previouslbMiddle is not -1 (it means - this is not the first lb)
				if(lb != previouslbMiddle && previouslbMiddle != -1)
				{
					//std::cout << "\tlb!=previouslbMiddle" << std::endl;
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
								clustersPerPlaneMiddle = -99;
							}
							//std::cout << "\tFILL Middle" << std::endl;
							//std::cout << "clustersPerPlaneMiddle (it was zero on cernbox): " << clustersPerPlaneMiddle << std::endl;
							fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlaneMiddle);
						}
					}
					previouslbMiddle = lb;
					++clusterCounterMiddle[lb][cluster.station][cluster.layer];
					counterForEventsMiddle=1;
				
				//(int i=0; i<4; i++)
				//{
					//for(int j=0; j<4; j++)
					//{
					//	numberOfClusterStationPlane[i][j] = 0;
					//}
				//}
				}
			
			// First time in lumiblock (in plane)
				else if(clusterCounterMiddle[lb][cluster.station][cluster.layer] == 0) 
				{
					++clusterCounterMiddle[lb][cluster.station][cluster.layer];				
					previouslbMiddle = lb;
				//std::cout << "\tFirst time" << std::endl;
				//std::cout << "\tLuminosity block: " << lb << std::endl;
				}
			
				// Lumiblock is same, so proceed
				else if(lb==previouslbMiddle)	// Same lumiblock
				{
					++clusterCounterMiddle[lb][cluster.station][cluster.layer];
				}
			}
		}
	


	// Filling of layerEff 2D histogram
/*	
	for(int i = 0; i < 4; i++)
	{
		unsigned long tempSum = numberOfHitsStationPlane[i][0] + numberOfHitsStationPlane[i][1] + numberOfHitsStationPlane[i][2] + numberOfHitsStationPlane[i][3];
		if(tempSum>0)
		{
			for(int j = 0; j < 4; j++)
			{
				layerNumber = i*4 + j;
				layerEfficiency = numberOfHitsStationPlane[i][j] / tempSum;
				layerEfficiency += i*4+j;
				//fill("AFPSiLayerTool", layerNumber, layerEfficiency);
				fill("AFPSiLayerTool", layerEfficiency);
			}
		}
	}
*/	

	return StatusCode::SUCCESS;
} // end of fillHistograms



