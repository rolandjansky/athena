/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Run3AFPMonitoring/AFPSiLayerAlgorithm.h"
//#include <xAODForward/AFPSiHit.h>
//#include <xAODForward/AFPSiHitContainer.h>
#include "StoreGate/ReadHandleKey.h"

/* Old constructor:
AFPHitsMonitorAlgorithm::AFPHitsMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_doRandom(true)
{}
*/

const int s_cNearStationIndex = 2;
const int s_cFarStationIndex = 3;


AFPSiLayerAlgorithm::AFPSiLayerAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
//, m_histsDirectoryName ("AFP/")
//, m_cNearStation (s_cNearStationIndex)
//, m_cFarStation (s_cFarStationIndex)
, m_afpHitContainerKey("AFPSiHitContainer")

{
    declareProperty( "AFPSiHitContainer", m_afpHitContainerKey );
}


AFPSiLayerAlgorithm::~AFPSiLayerAlgorithm() {}


StatusCode AFPSiLayerAlgorithm::initialize() {
    using namespace Monitored;
 
    std::vector<std::string> layers = { "P0","P1", "P2", "P3"};
    std::vector<std::string> stations = { "FarStation","NearStation"};
    std::vector<std::string> sides = { "Aside","Cside"};


    m_HitmapGroups = buildToolMap<std::map<std::string,std::map<std::string,int>>>(m_tools,"AFPSiLayerTool",sides,stations,layers);

//  std::map<std::string,std::map<std::string,int>> <std::map<std::string,int>> 
     // We must declare to the framework in initialize what SG objects we are going to use
    SG::ReadHandleKey<xAOD::AFPSiHitContainer> afpHitContainerKey("AFPSiHits");
        ATH_CHECK(m_afpHitContainerKey.initialize());
    // ...
    return AthMonitorAlgorithm::initialize();
}


StatusCode AFPSiLayerAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    // My histograms:
 //   auto h_hitMultiplicity = Monitored::Scalar<int>("h_hitMultiplicity", 0);
 //   auto h_timeOverThreshold = Monitored::Scalar<int>("h_timeOverThreshold", 0);
    //auto h_hitMap = Monitored::Scalar<int>("h_hitMap", 0);
    // End
/*	
    auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
    auto lb = Monitored::Scalar<int>("lb",0);
    auto run = Monitored::Scalar<int>("run",0);
    auto random = Monitored::Scalar<float>("random",0.0);
    auto testweight = Monitored::Scalar<float>("testweight",1.0);
*/
    auto totHits = Monitored::Scalar<int>("totHits", 0); // Nikola

/*    // Two variables (value and passed) needed for TEfficiency
    auto pT = Monitored::Scalar<float>("pT",0.0);
    auto pT_passed = Monitored::Scalar<bool>("pT_passed",false);
*/
    // Set the values of the monitored variables for the event
  //  h_timeOverThreshold = lbAverageInteractionsPerCrossing(ctx);
  //  h_hitMultiplicity = lbAverageInteractionsPerCrossing(ctx);
    //    h_hitMap = lbAverageInteractionsPerCrossing(ctx);
    totHits = GetEventInfo(ctx)->lumiBlock();	// Nikola
/*
    lumiPerBCID = lbAverageInteractionsPerCrossing(ctx);
    lb = GetEventInfo(ctx)->lumiBlock();
    run = GetEventInfo(ctx)->runNumber();
    testweight = 2.0;
    
    TRandom3 r(ctx.eventID().event_number());
    // Example of using flags
    if (m_doRandom) {
        random = r.Rndm();
    }

    // Fake efficiency calculator
    pT = r.Landau(15);
    pT_passed = pT>r.Poisson(15);
*/
    // Fill. First argument is the tool name, all others are the variables to be saved.
    //fill("AFPSiLayer", h_timeOverThreshold, h_hitMultiplicity,lumiPerBCID,lb,random,pT,pT_passed,testweight);
    //fill("AFPSiLayerAlgorithm",h_hitMultiplicity,lb,random,pT,pT_passed,testweight);
    //////////fill("AFPSiLayer", h_timeOverThreshold, h_hitMultiplicity);
    //fill("AFPSiLayerTool", totHits);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    //auto tool = getGroup("AFPSiLayer");
    //fill(tool,run);

    // Fill with a vector; useful in some circumstances.
/*
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> varVec = {lumiPerBCID,pT};
    fill("AFPSiLayer",varVec);
    fill(tool,varVec);
*/
/*
    // Filling using a pre-defined array of groups.
    auto a = Scalar<float>("a",0.0);
    auto b = Scalar<float>("b",1.0);
    auto c = Scalar<float>("c",2.0);
    for ( auto iEta : {0,1} ) {
        // 1) Valid but inefficient fill
        fill("AFPSiLayer_"+std::to_string(iEta),a,b,c);
        // 2) Faster way to fill a vector of histograms
        fill(m_tools[m_abGroups1[iEta]],a,b,c);
        for ( auto iPhi : {0,1} ) {
            // Same efficient method for 2D array
            fill(m_tools[m_abGroups2[iEta][iPhi]],a,b);
        }
    }

    // Filling using a pre-defined map of groups.
    for ( auto& layer : std::vector<std::string>({"layer1","layer2"}) ) {
        fill(m_tools[m_cGroups1.at(layer)],c);
        for ( auto& cluster : std::vector<std::string>({"clusterX","clusterB"}) ) {
            // Same efficient method for 2D map
            fill(m_tools[m_cGroups2.at(layer).at(cluster)],c);
        }
    }
*/    
    SG::ReadHandle<xAOD::AFPSiHitContainer> afpHitContainer(m_afpHitContainerKey, ctx);
    if(! afpHitContainer.isValid())
    {
	ATH_MSG_ERROR("evtStore() does not contain hits collection with name " << m_afpHitContainerKey);
	return StatusCode::FAILURE;
    }


    ATH_CHECK( afpHitContainer.initialize() );
 //for (const auto& muonItr : *muons) {
    for(const xAOD::AFPSiHit *hitsItr: *afpHitContainer)
    {
       std::cout << hitsItr->stationID() << std::endl;

	switch(hitsItr->stationID())
	{
	    case 2:
	        std::cout << hitsItr->pixelColIDChip() << std::endl;
		break;
	    //case s_cFarStationIndex:
		//m_cFarStation.fillHistograms(*hitsItr);
		//break;
	    default:
		ATH_MSG_WARNING("Unrecognised station index: " << hitsItr->stationID());
	}

    }

	//m_cNearStation.eventEnd();
	//m_cFarStation.eventEnd();

    return StatusCode::SUCCESS;
}

