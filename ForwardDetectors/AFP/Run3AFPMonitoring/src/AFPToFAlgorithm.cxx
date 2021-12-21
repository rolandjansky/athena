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

std::vector<int> frontBCIDsVectorToF;
std::vector<int> middleBCIDsVectorToF;
std::vector<int> endBCIDsVectorToF;

bool isInListVectorToF(const int bcid, const std::vector<int>&arr)
{
	return std::find_if(arr.begin(),arr.end(),[&bcid](const int& ele){return ele==bcid;})!= arr.end();
}

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
	m_TrainsToFGroup    = buildToolMap<int>(m_tools, "AFPToFTool", m_trainsToF);
	m_BarsInTrainsA     = buildToolMap<std::map<std::string,int>>(m_tools, "AFPToFTool", m_trainsToFA, m_barsToF);
	m_BarsInTrainsC     = buildToolMap<std::map<std::string,int>>(m_tools,"AFPToFTool", m_trainsToFC, m_barsToF);

	// We must declare to the framework in initialize what SG objects we are going to use
	SG::ReadHandleKey<xAOD::AFPToFHitContainer> afpToFHitContainerKey("AFPToFHits");
	ATH_CHECK(m_afpToFHitContainerKey.initialize());
	
	ATH_MSG_INFO( "BunchCrossingKey initialization (ToF)" );
	ATH_CHECK(m_bunchCrossingKeyToF.initialize());
	ATH_MSG_INFO( "initialization completed (ToF)" );

	return AthMonitorAlgorithm::initialize();
}


StatusCode AFPToFAlgorithm::fillHistograms( const EventContext& ctx ) const {
	using namespace Monitored;
	
	auto bcidAllToF     = Monitored::Scalar<int>("bcidAllToF", 0);
	auto bcidFrontToF   = Monitored::Scalar<int>("bcidFrontToF", 0);
	auto bcidMiddleToF  = Monitored::Scalar<int>("bcidMiddleToF", 0);
	auto bcidEndToF     = Monitored::Scalar<int>("bcidEndToF", 0);

	// Declare the quantities which should be monitored
	auto lb             = Monitored::Scalar<int>("lb", 0);
	auto nTofHits       = Monitored::Scalar<int>("nTofHits", 1);
	auto numberOfHit_S0 = Monitored::Scalar<int>("numberOfHit_S0", 0); 
	auto numberOfHit_S3 = Monitored::Scalar<int>("numberOfHit_S3", 0);
	auto trainID        = Monitored::Scalar<int>("trainID", 0); 
	auto barInTrainID   = Monitored::Scalar<int>("barInTrainID", 0); 
	auto barInTrainAllA = Monitored::Scalar<int>("barInTrainAllA", 0);
	auto barInTrainIDA  = Monitored::Scalar<int>("barInTrainIDA", 0); 
	auto barInTrainAllC = Monitored::Scalar<int>("barInTrainAllC", 0);
	auto barInTrainIDC  = Monitored::Scalar<int>("barInTrainIDC", 0);
	
	auto ToFHits_sideA      = Monitored::Scalar<int>("ToFHits_sideA", 0);
	auto ToFHits_sideC      = Monitored::Scalar<int>("ToFHits_sideC", 0);
	auto ToFHits_MU_Weight  = Monitored::Scalar<float>("ToFHits_MU_Weight", 0.0);
	auto muPerBXToF         = Monitored::Scalar<float>("muPerBXToF", 0.0);
	
	auto lbAToF           = Monitored::Scalar<int>("lbAToF", 0);
	auto lbCToF           = Monitored::Scalar<int>("lbCToF", 0);
	auto lbAToF_Weight    = Monitored::Scalar<float>("lbAToF_Weight", 0.0);
	auto lbCToF_Weight    = Monitored::Scalar<float>("lbCToF_Weight", 0.0);
	
	std::vector<int>stationValues;
	auto lbAToFEvents     = Monitored::Scalar<int>("lbAToFEvents", 0);
	auto lbCToFEvents     = Monitored::Scalar<int>("lbCToFEvents", 0);
	auto lbAandCToFEvents = Monitored::Scalar<int>("lbAandCToFEvents", 0);
	
	// FME histograms quantites (side A)
	
	auto lbAToF_T0        = Monitored::Scalar<int>("lbAToF_T0", 0);
	auto lbAToF_T1        = Monitored::Scalar<int>("lbAToF_T1", 0);
	auto lbAToF_T2        = Monitored::Scalar<int>("lbAToF_T2", 0);
	auto lbAToF_T3        = Monitored::Scalar<int>("lbAToF_T3", 0);
	
	auto lbAToF_TAll_Weight = Monitored::Scalar<float>("lbAToF_TAll_Weight", 1);
	
	auto lbAToF_T0_Front = Monitored::Scalar<int>("lbAToF_T0_Front", 0);
	auto lbAToF_T1_Front = Monitored::Scalar<int>("lbAToF_T1_Front", 0);
	auto lbAToF_T2_Front = Monitored::Scalar<int>("lbAToF_T2_Front", 0);
	auto lbAToF_T3_Front = Monitored::Scalar<int>("lbAToF_T3_Front", 0);

	auto lbAToF_TFront_Weight = Monitored::Scalar<float>("lbAToF_TFront_Weight", 1);
	
	auto lbAToF_T0_Middle = Monitored::Scalar<int>("lbAToF_T0_Middle", 0);
	auto lbAToF_T1_Middle = Monitored::Scalar<int>("lbAToF_T1_Middle", 0);
	auto lbAToF_T2_Middle = Monitored::Scalar<int>("lbAToF_T2_Middle", 0);
	auto lbAToF_T3_Middle = Monitored::Scalar<int>("lbAToF_T3_Middle", 0);
	
	auto lbAToF_TMiddle_Weight = Monitored::Scalar<float>("lbAToF_TMiddle_Weight", 1);
	
	auto lbAToF_T0_End = Monitored::Scalar<int>("lbAToF_T0_End", 0);
	auto lbAToF_T1_End = Monitored::Scalar<int>("lbAToF_T1_End", 0);
	auto lbAToF_T2_End = Monitored::Scalar<int>("lbAToF_T2_End", 0);
	auto lbAToF_T3_End = Monitored::Scalar<int>("lbAToF_T3_End", 0);
	
	auto lbAToF_TEnd_Weight = Monitored::Scalar<float>("lbAToF_TEnd_Weight", 1);
	
		
	// FME histograms quantites (side C)
	
	auto lbCToF_T0 = Monitored::Scalar<int>("lbCToF_T0", 0);
	auto lbCToF_T1 = Monitored::Scalar<int>("lbCToF_T1", 0);
	auto lbCToF_T2 = Monitored::Scalar<int>("lbCToF_T2", 0);
	auto lbCToF_T3 = Monitored::Scalar<int>("lbCToF_T3", 0);
	
	auto lbCToF_TAll_Weight = Monitored::Scalar<float>("lbCToF_TAll_Weight", 1);
	
	auto lbCToF_T0_Front = Monitored::Scalar<int>("lbCToF_T0_Front", 0);
	auto lbCToF_T1_Front = Monitored::Scalar<int>("lbCToF_T1_Front", 0);
	auto lbCToF_T2_Front = Monitored::Scalar<int>("lbCToF_T2_Front", 0);
	auto lbCToF_T3_Front = Monitored::Scalar<int>("lbCToF_T3_Front", 0);
	
	auto lbCToF_TFront_Weight = Monitored::Scalar<float>("lbCToF_TFront_Weight", 1);
	
	auto lbCToF_T0_Middle = Monitored::Scalar<int>("lbCToF_T0_Middle", 0);
	auto lbCToF_T1_Middle = Monitored::Scalar<int>("lbCToF_T1_Middle", 0);
	auto lbCToF_T2_Middle = Monitored::Scalar<int>("lbCToF_T2_Middle", 0);
	auto lbCToF_T3_Middle = Monitored::Scalar<int>("lbCToF_T3_Middle", 0);
	
	auto lbCToF_TMiddle_Weight = Monitored::Scalar<float>("lbCToF_TMiddle_Weight", 1);
	
	auto lbCToF_T0_End = Monitored::Scalar<int>("lbCToF_T0_End", 0);
	auto lbCToF_T1_End = Monitored::Scalar<int>("lbCToF_T1_End", 0);
	auto lbCToF_T2_End = Monitored::Scalar<int>("lbCToF_T2_End", 0);
	auto lbCToF_T3_End = Monitored::Scalar<int>("lbCToF_T3_End", 0);
	
	auto lbCToF_TEnd_Weight = Monitored::Scalar<float>("lbCToF_TEnd_Weight", 1);
	SG::ReadHandle<xAOD::EventInfo> eventInfo = GetEventInfo(ctx);
	lb                = eventInfo->lumiBlock();
	lbAToF            = eventInfo->lumiBlock();
	lbCToF            = eventInfo->lumiBlock();
	lbAToFEvents      = eventInfo->lumiBlock();
	lbCToFEvents      = eventInfo->lumiBlock();
	lbAandCToFEvents  = eventInfo->lumiBlock();
	muPerBXToF        = lbAverageInteractionsPerCrossing(ctx);
	
	ToFHits_MU_Weight       = 1/muPerBXToF;
	lbAToF_Weight           = 1/muPerBXToF;
	lbCToF_Weight           = 1/muPerBXToF;
	lbAToF_TAll_Weight      = 1/muPerBXToF;
	lbCToF_TAll_Weight      = 1/muPerBXToF;
	lbAToF_TFront_Weight    = 1/muPerBXToF;
	lbAToF_TMiddle_Weight   = 1/muPerBXToF;
	lbAToF_TEnd_Weight      = 1/muPerBXToF;
	lbCToF_TFront_Weight    = 1/muPerBXToF;
	lbCToF_TMiddle_Weight   = 1/muPerBXToF;
	lbCToF_TEnd_Weight      = 1/muPerBXToF;
	
	fill("AFPToFTool", lb, muPerBXToF);

	// BCX handler
	unsigned int tempBCID = eventInfo->bcid();
	SG::ReadCondHandle<BunchCrossingCondData> bcidHdlToF(m_bunchCrossingKeyToF,ctx);
	if (!bcidHdlToF.isValid()) {
		ATH_MSG_ERROR( "Unable to retrieve BunchCrossing conditions object (ToF)" );
	}
	const BunchCrossingCondData* bcDataToF{*bcidHdlToF};
	
	// Classifying bunches by position in train (Front, Middle, End)
	if(bcDataToF->isFilled(tempBCID))
	{
		bcidAllToF = tempBCID;
		fill("AFPToFTool", bcidAllToF);
		if(!bcDataToF->isFilled(tempBCID-1))
		{
			if(!isInListVectorToF(tempBCID, frontBCIDsVectorToF))
			{
				frontBCIDsVectorToF.push_back(tempBCID);
			}
			bcidFrontToF = tempBCID;
			fill("AFPToFTool", bcidFrontToF);
		}
		else
		{
			if(bcDataToF->isFilled(tempBCID+1))
			{
				if(!isInListVectorToF(tempBCID, middleBCIDsVectorToF))
				{
					middleBCIDsVectorToF.push_back(tempBCID);
				}
				bcidMiddleToF = tempBCID;
				fill("AFPToFTool", bcidMiddleToF);
			}
			else
			{
				if(!isInListVectorToF(tempBCID, endBCIDsVectorToF))
				{
					endBCIDsVectorToF.push_back(tempBCID);
				}
				bcidEndToF = tempBCID;
				fill("AFPToFTool", bcidEndToF);
			}
		}
	}


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
		stationValues.push_back(hitsItr->stationID());

		if(hitsItr->isSideA())
		{
			numberOfHit_S0 = hitsItr->trainID();
			fill("AFPToFTool", numberOfHit_S0);
			
			barInTrainIDA = hitsItr->barInTrainID();
			fill(m_tools[m_TrainsToFGroup.at(m_trainsToF.at(hitsItr->trainID()))], barInTrainIDA);
			barInTrainAllA = (hitsItr->trainID()*4)+barInTrainIDA;
			fill("AFPToFTool", barInTrainAllA);
			
			ToFHits_sideA = eventInfo->lumiBlock();
			fill("AFPToFTool", ToFHits_sideA, ToFHits_MU_Weight);
			
			
			if(hitsItr->trainID() == 0)
			{
				lbAToF_T0 = eventInfo->lumiBlock();
				fill("AFPToFTool", lbAToF_T0, lbAToF_TAll_Weight);
				
				if(isInListVectorToF(tempBCID, frontBCIDsVectorToF))
				{
					lbAToF_T0_Front = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T0_Front, lbAToF_TFront_Weight);
				}
				else if(isInListVectorToF(tempBCID, middleBCIDsVectorToF))
				{
					lbAToF_T0_Middle = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T0_Middle, lbAToF_TMiddle_Weight);
				}
				else if(isInListVectorToF(tempBCID, endBCIDsVectorToF))
				{
					lbAToF_T0_End = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T0_End, lbAToF_TEnd_Weight);
				}
				
			}
			else if(hitsItr->trainID() == 1)
			{
				lbAToF_T1 = eventInfo->lumiBlock();
				fill("AFPToFTool", lbAToF_T1, lbAToF_TAll_Weight);
				
				if(isInListVectorToF(tempBCID, frontBCIDsVectorToF))
				{
					lbAToF_T1_Front = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T1_Front, lbAToF_TFront_Weight);
				}
				else if(isInListVectorToF(tempBCID, middleBCIDsVectorToF))
				{
					lbAToF_T1_Middle = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T1_Middle, lbAToF_TMiddle_Weight);
				}
				else if(isInListVectorToF(tempBCID, endBCIDsVectorToF))
				{
					lbAToF_T1_End = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T1_End, lbAToF_TEnd_Weight);
				}
			}
			else if(hitsItr->trainID() == 2)
			{
				lbAToF_T2 = eventInfo->lumiBlock();
				fill("AFPToFTool", lbAToF_T2, lbAToF_TAll_Weight);
				
				if(isInListVectorToF(tempBCID, frontBCIDsVectorToF))
				{
					lbAToF_T2_Front = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T2_Front, lbAToF_TFront_Weight);
				}
				else if(isInListVectorToF(tempBCID, middleBCIDsVectorToF))
				{
					lbAToF_T2_Middle = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T2_Middle, lbAToF_TMiddle_Weight);
				}
				else if(isInListVectorToF(tempBCID, endBCIDsVectorToF))
				{
					lbAToF_T2_End = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T2_End, lbAToF_TEnd_Weight);
				}
			}
			else if(hitsItr->trainID() == 3)
			{
				lbAToF_T3 = eventInfo->lumiBlock();
				fill("AFPToFTool", lbAToF_T3, lbAToF_TAll_Weight);
				
				if(isInListVectorToF(tempBCID, frontBCIDsVectorToF))
				{
					lbAToF_T3_Front = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T3_Front, lbAToF_TFront_Weight);
				}
				else if(isInListVectorToF(tempBCID, middleBCIDsVectorToF))
				{
					lbAToF_T3_Middle = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T3_Middle, lbAToF_TMiddle_Weight);
				}
				else if(isInListVectorToF(tempBCID, endBCIDsVectorToF))
				{
					lbAToF_T3_End = eventInfo->lumiBlock();
					fill("AFPToFTool", lbAToF_T3_End, lbAToF_TEnd_Weight);
				}
			}
		}
		else if(hitsItr->isSideC())
		{
			numberOfHit_S3 = hitsItr->trainID();
			fill("AFPToFTool", numberOfHit_S3);
			
			barInTrainIDC = hitsItr->barInTrainID();
			fill(m_tools[m_TrainsToFGroup.at(m_trainsToF.at(hitsItr->trainID()))], barInTrainIDC);
			barInTrainAllC = (hitsItr->trainID()*4)+barInTrainIDC;
			fill("AFPToFTool", barInTrainAllC);
			
			ToFHits_sideC = eventInfo->lumiBlock();
			fill("AFPToFTool", ToFHits_sideC, ToFHits_MU_Weight);
			
			if(hitsItr->trainID() == 0)
			{
				lbCToF_T0 = eventInfo->lumiBlock();
				fill("AFPToFTool", lbCToF_T0, lbCToF_TAll_Weight);
				
				if(isInListVectorToF(tempBCID, frontBCIDsVectorToF))
				{
					lbCToF_T0_Front = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T0_Front, lbCToF_TFront_Weight);
				}
				else if(isInListVectorToF(tempBCID, middleBCIDsVectorToF))
				{
					lbCToF_T0_Middle = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T0_Middle, lbCToF_TMiddle_Weight);
				}
				else if(isInListVectorToF(tempBCID, endBCIDsVectorToF))
				{
					lbCToF_T0_End = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T0_End, lbCToF_TEnd_Weight);
				}
			}
			else if(hitsItr->trainID() == 1)
			{
				lbCToF_T1 = eventInfo->lumiBlock();
				fill("AFPToFTool", lbCToF_T1, lbCToF_TAll_Weight);
				
				if(isInListVectorToF(tempBCID, frontBCIDsVectorToF))
				{
					lbCToF_T1_Front = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T1_Front, lbCToF_TFront_Weight);
				}
				else if(isInListVectorToF(tempBCID, middleBCIDsVectorToF))
				{
					lbCToF_T1_Middle = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T1_Middle, lbCToF_TMiddle_Weight);
				}
				else if(isInListVectorToF(tempBCID, endBCIDsVectorToF))
				{
					lbCToF_T1_End = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T1_End, lbCToF_TEnd_Weight);
				}
			}
			else if(hitsItr->trainID() == 2)
			{
				lbCToF_T2 = eventInfo->lumiBlock();
				fill("AFPToFTool", lbCToF_T2, lbCToF_TAll_Weight);
				
				if(isInListVectorToF(tempBCID, frontBCIDsVectorToF))
				{
					lbCToF_T2_Front = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T2_Front, lbCToF_TFront_Weight);
				}
				else if(isInListVectorToF(tempBCID, middleBCIDsVectorToF))
				{
					lbCToF_T2_Middle = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T2_Middle, lbCToF_TMiddle_Weight);
				}
				else if(isInListVectorToF(tempBCID, endBCIDsVectorToF))
				{
					lbCToF_T2_End = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T2_End, lbCToF_TEnd_Weight);
				}
			}
			else if(hitsItr->trainID() == 3)
			{
				lbCToF_T3 = eventInfo->lumiBlock();
				fill("AFPToFTool", lbCToF_T3, lbCToF_TAll_Weight);
				
				if(isInListVectorToF(tempBCID, frontBCIDsVectorToF))
				{
					lbCToF_T3_Front = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T3_Front, lbCToF_TFront_Weight);
				}
				else if(isInListVectorToF(tempBCID, middleBCIDsVectorToF))
				{
					lbCToF_T3_Middle = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T3_Middle, lbCToF_TMiddle_Weight);
				}
				else if(isInListVectorToF(tempBCID, endBCIDsVectorToF))
				{
					lbCToF_T3_End = eventInfo->lumiBlock();
					fill("AFPToFTool", lbCToF_T3_End, lbCToF_TEnd_Weight);
			}
		}
		}
		if (hitsItr->stationID() == 0 || hitsItr->stationID() == 3)
		{
			fill(m_tools[m_StationNamesGroup.at(m_stationNamesToF.at(hitsItr->stationID()))], barInTrainID, trainID);
			
			if(hitsItr->stationID() == 0)	// farAside
			{
				fill(m_tools[m_BarsInTrainsA.at(m_trainsToFA.at(hitsItr->trainID())).at(m_barsToF.at(hitsItr->barInTrainID()))], lbAToF, lbAToF_Weight);
			}
			else	// farCside
			{
				fill(m_tools[m_BarsInTrainsC.at(m_trainsToFC.at(hitsItr->trainID())).at(m_barsToF.at(hitsItr->barInTrainID()))], lbCToF, lbCToF_Weight);
			}
		}
	}
	ToFHits_MU_Weight 	= 1;
	lbAToF_Weight 			= 1;
	lbCToF_Weight 			= 1;
	
	// Events histograms
	if(!stationValues.empty())
	{
		fill("AFPToFTool", lbAandCToFEvents);
		
		if(isInListVectorToF (0, stationValues))
		{
			fill("AFPToFTool", lbAToFEvents);
		}
		if(isInListVectorToF (3, stationValues))
		{
			fill("AFPToFTool", lbCToFEvents);
		}
	}
	stationValues.clear();

	return StatusCode::SUCCESS;
}

