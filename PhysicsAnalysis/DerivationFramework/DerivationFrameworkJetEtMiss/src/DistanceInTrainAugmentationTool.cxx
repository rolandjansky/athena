/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "xAODEventInfo/EventInfo.h"

DerivationFramework::DistanceInTrainAugmentationTool::DistanceInTrainAugmentationTool(const std::string& t,
                                                            const std::string& n,
                                                            const IInterface* p) :
  AthAlgTool(t, n, p),
  m_bcTool( "Trig::LHCBunchCrossingTool/BunchCrossingTool" )
{

  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty( "BCTool",        m_bcTool );
}

DerivationFramework::DistanceInTrainAugmentationTool::~DistanceInTrainAugmentationTool() {
}

StatusCode DerivationFramework::DistanceInTrainAugmentationTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ..");
  ATH_CHECK( m_bcTool.retrieve() );
  ATH_MSG_INFO("Retrieved tool: " << m_bcTool);

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::DistanceInTrainAugmentationTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::DistanceInTrainAugmentationTool::addBranches() const {

  const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo >();
  if(!eventInfo) {
    ATH_MSG_ERROR ("Couldn't retrieve xAOD::EventInfo");
    return StatusCode::FAILURE;
  }

  SG::AuxElement::Decorator< int >  decoratorBCIDDistanceFront("DFCommonJets_BCIDDistanceFromFront");
  SG::AuxElement::Decorator< int >  decoratorBCIDDistanceTail("DFCommonJets_BCIDDistanceFromTail");
  SG::AuxElement::Decorator< int >  decoratorBCIDGapBeforeTrain("DFCommonJets_BCIDGapBeforeTrain");
  SG::AuxElement::Decorator< int >  decoratorBCIDGapAfterTrain("DFCommonJets_BCIDGapAfterTrain");
  SG::AuxElement::Decorator< int >  decoratorBCIDType("DFCommonJets_BCIDType");
  SG::AuxElement::Decorator< int >  decoratorBCIDGapBeforeTrainMinus12("DFCommonJets_BCIDGapBeforeTrainMinus12");
  SG::AuxElement::Decorator< int >  decoratorBCIDGapAfterTrainMinus12("DFCommonJets_BCIDGapAfterTrainMinus12");
  SG::AuxElement::Decorator< int >  decoratorBCIDTypeMinus12("DFCommonJets_BCIDTypeMinus12");

  decoratorBCIDDistanceFront(*eventInfo) = m_bcTool->distanceFromFront(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);
  decoratorBCIDDistanceTail(*eventInfo)  = m_bcTool->distanceFromTail(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);
  decoratorBCIDGapBeforeTrain(*eventInfo) = m_bcTool->gapBeforeTrain(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);
  decoratorBCIDGapAfterTrain(*eventInfo) = m_bcTool->gapAfterTrain(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);
  decoratorBCIDType(*eventInfo) = m_bcTool->bcType(eventInfo->bcid());
  if (eventInfo->bcid()>=12){
    decoratorBCIDTypeMinus12(*eventInfo) = m_bcTool->bcType(eventInfo->bcid()-12);
    decoratorBCIDGapBeforeTrainMinus12(*eventInfo) = m_bcTool->gapBeforeTrain(eventInfo->bcid()-12, Trig::IBunchCrossingTool::BunchCrossings);
    decoratorBCIDGapAfterTrainMinus12(*eventInfo) = m_bcTool->gapAfterTrain(eventInfo->bcid()-12, Trig::IBunchCrossingTool::BunchCrossings);
  }else{
    decoratorBCIDTypeMinus12(*eventInfo) = -1;
    decoratorBCIDGapBeforeTrainMinus12(*eventInfo) = 0;
    decoratorBCIDGapAfterTrainMinus12(*eventInfo) = 0;
  }

  return StatusCode::SUCCESS;
}

