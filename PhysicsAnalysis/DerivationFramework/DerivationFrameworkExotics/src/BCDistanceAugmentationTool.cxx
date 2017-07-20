/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkExotics/BCDistanceAugmentationTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"


DerivationFramework::BCDistanceAugmentationTool::BCDistanceAugmentationTool(const std::string& t,
                                                            const std::string& n,
                                                            const IInterface* p) :
  AthAlgTool(t, n, p),
  m_bcTool( "Trig::LHCBunchCrossingTool/BunchCrossingTool" )
{

  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty( "BCTool", m_bcTool );
  //declareProperty( "RunNumber", m_runNumber = 0 );
}

DerivationFramework::BCDistanceAugmentationTool::~BCDistanceAugmentationTool() {
}

StatusCode DerivationFramework::BCDistanceAugmentationTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ..");
  ATH_CHECK( m_bcTool.retrieve() );
  ATH_MSG_INFO("Retrieved tool: " << m_bcTool);
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::BCDistanceAugmentationTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::BCDistanceAugmentationTool::addBranches() const {

  const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo >();
  if(!eventInfo) {
    ATH_MSG_ERROR ("Couldn't retrieve xAOD::EventInfo");
    return StatusCode::FAILURE;
  }

  SG::AuxElement::Decorator< int >  decoratorBCIDDistance("BCIDDistanceFromFront");

  decoratorBCIDDistance(*eventInfo) = m_bcTool->distanceFromFront(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);
  return StatusCode::SUCCESS;
}
