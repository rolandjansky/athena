/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
, m_afpToFHitContainerKey("AFPToFHitContainer")

{
	declareProperty( "AFPToFHitContainer", m_afpToFHitContainerKey );
}


AFPToFAlgorithm::~AFPToFAlgorithm() {}


StatusCode AFPToFAlgorithm::initialize() {
	using namespace Monitored;

	m_StationNamesGroup = buildToolMap<int>(m_tools,"AFPToFTool", m_stationNamesToF);
	m_TrainsToFGroup = buildToolMap<int>(m_tools, "AFPToFTool", m_trainsToF);

	// We must declare to the framework in initialize what SG objects we are going to use
	SG::ReadHandleKey<xAOD::AFPToFHitContainer> afpToFHitContainerKey("AFPToFHits");
	ATH_CHECK(m_afpToFHitContainerKey.initialize());

	return AthMonitorAlgorithm::initialize();
}


StatusCode AFPToFAlgorithm::fillHistograms( const EventContext& ctx ) const {
	using namespace Monitored;

	// Declare the quantities which should be monitored
	auto lb = Monitored::Scalar<int>("lb", 0);
	auto nTofHits = Monitored::Scalar<int>("nTofHits", 1);
	auto numberOfHit_S0 = Monitored::Scalar<int>("numberOfHit_S0", 0); 
	auto numberOfHit_S3 = Monitored::Scalar<int>("numberOfHit_S3", 0);
	auto trainID = Monitored::Scalar<int>("trainID", 0); 
	auto barInTrainID = Monitored::Scalar<int>("barInTrainID", 0); 
	auto barInTrainAll = Monitored::Scalar<int>("barInTrainAll", 0);
    
	lb = GetEventInfo(ctx)->lumiBlock();
 
	SG::ReadHandle<xAOD::AFPToFHitContainer> afpToFHitContainer(m_afpToFHitContainerKey, ctx);
	if(! afpToFHitContainer.isValid())
	{
		ATH_MSG_WARNING("evtStore() does not contain hits collection with name " << m_afpToFHitContainerKey);
		return StatusCode::SUCCESS;
	}

	ATH_CHECK( afpToFHitContainer.initialize() );

	nTofHits = afpToFHitContainer->size();
	fill("AFPToFTool", lb, nTofHits);

	for(const xAOD::AFPToFHit *hitsItr: *afpToFHitContainer)
	{
		trainID = hitsItr->trainID();
		barInTrainID = hitsItr->barInTrainID();

		if(hitsItr->isSideA())
		{
			numberOfHit_S0 = hitsItr->trainID();
			fill("AFPToFTool", numberOfHit_S0);
		}
		else if(hitsItr->isSideC())
		{
			numberOfHit_S3 = hitsItr->trainID();
			fill("AFPToFTool", numberOfHit_S3);
		}

		if (hitsItr->stationID() == 0 || hitsItr->stationID() == 3)
		{
			fill(m_tools[m_StationNamesGroup.at(m_stationNamesToF.at(hitsItr->stationID()))], barInTrainID, trainID);
			fill(m_tools[m_TrainsToFGroup.at(m_trainsToF.at(trainID))], barInTrainID);
			barInTrainAll = (trainID*4)+barInTrainID;
			fill("AFPToFTool", barInTrainAll);
		}
	}

	return StatusCode::SUCCESS;
}

