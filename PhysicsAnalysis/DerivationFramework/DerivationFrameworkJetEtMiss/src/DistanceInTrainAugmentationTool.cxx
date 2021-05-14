/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistanceInTrainAugmentationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Chris Young (christopher.young@cern.ch)
///////////////////////////////////////////////////////////////////
// This code is designed to augment EventInfo with a variety
// of variables to describe where in the bunch train structure 
// a given event lies. Care is taken to ensure that new trains
// are not defined at the abort gap or 4 non-colliding crossings
// in the 8b4e scheme.
///////////////////////////////////////////////////////////////////
#include "DistanceInTrainAugmentationTool.h"

#include "StoreGate/WriteDecorHandle.h"


DerivationFramework::DistanceInTrainAugmentationTool::DistanceInTrainAugmentationTool(const std::string& t,
                                                            const std::string& n,
                                                            const IInterface* p) :
  AthAlgTool(t, n, p)
{

  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty( "BCTool",        m_bunchCrossingKey );
}

DerivationFramework::DistanceInTrainAugmentationTool::~DistanceInTrainAugmentationTool() {
}

StatusCode DerivationFramework::DistanceInTrainAugmentationTool::initialize()
{

  ATH_CHECK(m_eventInfo_key.initialize());
  ATH_CHECK(m_BCIDDistanceFrontKey.initialize());
  ATH_CHECK(m_BCIDDistanceTailKey.initialize());
  ATH_CHECK(m_BCIDGapBeforeTrainKey.initialize());
  ATH_CHECK(m_BCIDGapAfterTrainKey.initialize());
  ATH_CHECK(m_BCIDTypeKey.initialize());
  ATH_CHECK(m_BCIDGapBeforeTrainMinus12Key.initialize());
  ATH_CHECK(m_BCIDGapAfterTrainMinus12Key.initialize());
  ATH_CHECK(m_BCIDTypeMinus12Key.initialize());

  ATH_MSG_VERBOSE("initialize() ..");
  ATH_CHECK( m_bunchCrossingKey.initialize() );
  ATH_MSG_INFO("The bunch crossing conditions key being used: " << m_bunchCrossingKey);

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::DistanceInTrainAugmentationTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::DistanceInTrainAugmentationTool::addBranches() const {

  auto eventInfo = SG::makeHandle (m_eventInfo_key);
  if (!eventInfo.isValid()){
    ATH_MSG_WARNING("Invalid  xAOD::EventInfo datahandle"
		    << m_eventInfo_key.key());
    return StatusCode::FAILURE;
  }
  auto ei = eventInfo.cptr();

  const EventContext& context = Gaudi::Hive::currentContext();
  SG::ReadCondHandle<BunchCrossingCondData> bunchCrossingTool (m_bunchCrossingKey, context);
  ATH_CHECK( bunchCrossingTool.isValid() );

  SG::WriteDecorHandle<xAOD::EventInfo,int> dec_BCIDDistanceFront(m_BCIDDistanceFrontKey);
  SG::WriteDecorHandle<xAOD::EventInfo,int> dec_BCIDDistanceTail(m_BCIDDistanceTailKey);
  SG::WriteDecorHandle<xAOD::EventInfo,int> dec_BCIDGapBeforeTrain(m_BCIDGapBeforeTrainKey);
  SG::WriteDecorHandle<xAOD::EventInfo,int> dec_BCIDGapAfterTrain(m_BCIDGapAfterTrainKey);
  SG::WriteDecorHandle<xAOD::EventInfo,int> dec_BCIDType(m_BCIDTypeKey);
  SG::WriteDecorHandle<xAOD::EventInfo,int> dec_BCIDGapBeforeTrainMinus12(m_BCIDGapBeforeTrainMinus12Key);
  SG::WriteDecorHandle<xAOD::EventInfo,int> dec_BCIDGapAfterTrainMinus12(m_BCIDGapAfterTrainMinus12Key);
  SG::WriteDecorHandle<xAOD::EventInfo,int> dec_BCIDTypeMinus12(m_BCIDTypeMinus12Key);

  dec_BCIDDistanceFront(*ei)  = bunchCrossingTool->distanceFromFront(ei->bcid(), BunchCrossingCondData::BunchDistanceType::BunchCrossings);
  dec_BCIDDistanceTail(*ei)   = bunchCrossingTool->distanceFromTail(ei->bcid(), BunchCrossingCondData::BunchDistanceType::BunchCrossings);
  dec_BCIDGapBeforeTrain(*ei) = bunchCrossingTool->gapBeforeTrain(ei->bcid(), BunchCrossingCondData::BunchDistanceType::BunchCrossings);
  dec_BCIDGapAfterTrain(*ei)  = bunchCrossingTool->gapAfterTrain(ei->bcid(), BunchCrossingCondData::BunchDistanceType::BunchCrossings);
  dec_BCIDType(*ei)           = bunchCrossingTool->bcType(ei->bcid());
  if (ei->bcid()>=12){
    dec_BCIDTypeMinus12(*ei)           = bunchCrossingTool->bcType(ei->bcid()-12);
    dec_BCIDGapBeforeTrainMinus12(*ei) = bunchCrossingTool->gapBeforeTrain(ei->bcid()-12, BunchCrossingCondData::BunchDistanceType::BunchCrossings);
    dec_BCIDGapAfterTrainMinus12(*ei)  = bunchCrossingTool->gapAfterTrain(ei->bcid()-12, BunchCrossingCondData::BunchDistanceType::BunchCrossings);
  }else{
    dec_BCIDTypeMinus12(*ei) = -1;
    dec_BCIDGapBeforeTrainMinus12(*ei) = 0;
    dec_BCIDGapAfterTrainMinus12(*ei) = 0;
  }

  return StatusCode::SUCCESS;
}

