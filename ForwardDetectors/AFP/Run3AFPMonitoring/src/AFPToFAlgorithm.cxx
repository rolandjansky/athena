/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*
*
*	AFPToFAlgorithm
*
*
*/

#include "Run3AFPMonitoring/AFPToFAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODForward/AFPStationID.h"


AFPToFAlgorithm::AFPToFAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
//, m_histsDirectoryName ("AFP/")
//, m_cNearStation (s_cNearStationIndex)
//, m_cFarStation (s_cFarStationIndex)
, m_afpToFContainerKey("AFPToFContainer")

{
    declareProperty( "AFPToFContainer", m_afpToFContainerKey );
}


AFPToFAlgorithm::~AFPToFAlgorithm() {}


StatusCode AFPToFAlgorithm::initialize() {
    using namespace Monitored;
 
    m_HitmapGroups = buildToolMap<std::map<std::string,int>>(m_tools,"AFPToFTool",m_stationnames,m_pixlayers);

    // std::map<std::string,std::map<std::string,int>> <std::map<std::string,int>> 
    // We must declare to the framework in initialize what SG objects we are going to use
    SG::ReadHandleKey<xAOD::AFPToFContainer> afpToFContainerKey("AFPToFHits");
        ATH_CHECK(m_afpToFContainerKey.initialize());
    // ...
    return AthMonitorAlgorithm::initialize();
}


StatusCode AFPToFAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    // My histograms:
    // auto h_hitMultiplicity = Monitored::Scalar<int>("h_hitMultiplicity", 0);
    // auto h_timeOverThreshold = Monitored::Scalar<int>("h_timeOverThreshold", 0);
    // auto h_hitMap = Monitored::Scalar<int>("h_hitMap", 0);
    // End

    auto lb = Monitored::Scalar<int>("lb", 0); // Nikola
    auto nhits = Monitored::Scalar<int>("nhits", 1);
    
    lb = GetEventInfo(ctx)->lumiBlock();	// Nikola

 
    SG::ReadHandle<xAOD::AFPToFContainer> afpToFContainer(m_afpToFContainerKey, ctx);
    if(! afpToFContainer.isValid())
    {
	ATH_MSG_ERROR("evtStore() does not contain hits collection with name " << m_afpToFContainerKey);
	return StatusCode::FAILURE;
    }

    /******************************************
     *
     *	const int xAOD::AFPStationID::farA = 0;
     *	const int xAOD::AFPStationID::nearA = 1;
     *	const int xAOD::AFPStationID::nearC = 2;
     *	const int xAOD::AFPStationID::farC = 3;
     *
     ******************************************/

    ATH_CHECK( afpToFContainer.initialize() );

    nhits = afpToFContainer->size();
    fill("AFPSiLayerTool", lb, nhits);

    auto pixelRowIDChip = Monitored::Scalar<int>("pixelRowIDChip", 0); // Nikola
    auto pixelColIDChip = Monitored::Scalar<int>("pixelColIDChip", 0); // Nikola
    auto h_hitMap = Monitored::Scalar<int>("h_hitMap", 0); // Nikola

    for(const xAOD::AFPSiHit *hitsItr: *afpToFContainer)
    {
       pixelRowIDChip=hitsItr->pixelRowIDChip();
       pixelColIDChip = hitsItr->pixelColIDChip();
       std::cout << "Hits iteration -> stationID = " << hitsItr->stationID() << std::endl;
       std::cout << "Hits iteration -> pixelLayerID = " << hitsItr->pixelLayerID() << std::endl;
	
       if (hitsItr->stationID()<4 && hitsItr->stationID()>=0 && hitsItr->pixelLayerID()<4 && hitsItr->pixelLayerID()>=0) 
       {
          std::cout << "I AM RIGHT BEFORE FILL METHOD\n" << std::endl;
          fill(m_tools[m_HitmapGroups.at( m_stationnames.at(hitsItr->stationID())).at( m_pixlayers.at(hitsItr->pixelLayerID()))] , pixelRowIDChip, pixelColIDChip);
       }
       else 
	ATH_MSG_WARNING("Unrecognised station index: " << hitsItr->stationID());

     }


    return StatusCode::SUCCESS;
}

