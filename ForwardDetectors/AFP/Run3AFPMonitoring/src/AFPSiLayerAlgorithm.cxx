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
	int previouslb = -1;
	unsigned int counterForEvents = 0;
	unsigned int counterForEventsFront = 0;
	unsigned int counterForEventsEnd = 0;
	unsigned int counterForEventsMiddle = 0;

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
	++counterForEvents;
	//std::cout << "\tBCID: " << GetEventInfo(ctx)->bcid() << std::endl;
	//if(GetEventInfo(ctx)->bcid() == 63)
	//{
	//	++counterForEventsFront;
	//}
	//std::cout << "\t\t" << counterForEvents << " lb:" << lb << std::endl;
	fill("AFPSiLayerTool", lb, muPerBCID);

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
				std::cout << "\tlb!=previouslb" << std::endl;
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
						std::cout << "\tFILL" << std::endl;
						std::cout << "ClustersPerPlane (it was zero on cernbox): " << clustersPerPlane << std::endl;
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
				//std::cout << "\tFirst time" << std::endl;
				//std::cout << "\tLuminosity block: " << lb << std::endl;
			}
			
			// Lumiblock is same, so proceed
			else if(lb==previouslb)	// Same lumiblock
			{
				++clusterCounter[lb][cluster.station][cluster.layer];
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
}



