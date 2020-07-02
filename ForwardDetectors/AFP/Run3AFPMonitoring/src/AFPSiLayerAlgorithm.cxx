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

	return AthMonitorAlgorithm::initialize();
}


StatusCode AFPSiLayerAlgorithm::fillHistograms( const EventContext& ctx ) const {
	using namespace Monitored;

	// Temporary variables:
	//unsigned int numberOfHitsPerPlane[16]		=	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	//unsigned int numberOfHitsPerStation[4]	=	{0, 0, 0, 0};
	
	static unsigned int numberOfHitsStationPlane[4][4] =	{	{0, 0, 0, 0},
																													{0, 0, 0, 0},
																													{0, 0, 0, 0},
																													{0, 0, 0, 0} };
	
	//int i = 0;
	//float eff = 0.0;

	// Declare the quantities which should be monitored:
	auto lb = Monitored::Scalar<int>("lb", 0);
	auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID", 0.0);
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
	lumiPerBCID = lbAverageInteractionsPerCrossing(ctx);
	//run = GetEventInfo(ctx)->runNumber();

	fill("AFPSiLayerTool", lb, lumiPerBCID);

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
		pixelRowIDChip = hitsItr->pixelRowIDChip();
		pixelColIDChip = hitsItr->pixelColIDChip();
		timeOverThreshold = hitsItr->timeOverThreshold();
		++numberOfHitsStationPlane[hitsItr->stationID()][hitsItr->pixelLayerID()];
	
		if (hitsItr->stationID()<4 && hitsItr->stationID()>=0 && hitsItr->pixelLayerID()<4 && hitsItr->pixelLayerID()>=0) 
		{
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelRowIDChip, pixelColIDChip);
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelRowIDChip);
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelColIDChip);
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], timeOverThreshold);
		}
		else ATH_MSG_WARNING("Unrecognised station index: " << hitsItr->stationID());

		
		
				// Filling of cluster and track 2D histograms
		AFPMon::AFPFastReco fast(afpHitContainer.get());
		fast.reco();

		for (const auto& cluster : fast.clusters()) 
		{
			clusterX = cluster.x;
			clusterY = cluster.y;
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], clusterX, clusterY);
		}

		for (const auto& track : fast.tracks()) 
		{
			trackX = track.x;
			trackY = track.y;
			fill(m_tools[m_TrackGroup.at(m_stationnames.at(track.station))], trackX, trackY);
		}
	}

	// Filling of layerEff 2D histogram
	
	for(int i = 0; i < 4; i++)
	{
		unsigned long tempSum = numberOfHitsStationPlane[i][0] + numberOfHitsStationPlane[i][1] + numberOfHitsStationPlane[i][2] + numberOfHitsStationPlane[i][3];
		if(tempSum>0)
		{
			for(int j = 0; j < 4; j++)
			{
				layerNumber = i*4 + j;
				layerEfficiency = numberOfHitsStationPlane[i][j] / tempSum ;
				fill("AFPSiLayerTool", layerNumber, layerEfficiency);
				fill("AFPSiLayerTool", layerEfficiency);
			}
		}
	}
	

	return StatusCode::SUCCESS;
}


