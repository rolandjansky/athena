/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruthBase/TruthStrategy.h"
#include "MCTruthBase/TruthStrategyManager.h"
#include "SimHelpers/StepHelper.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/GaudiException.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// For depth checking
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4LogicalVolume.hh"

TruthStrategy::TruthStrategy(const std::string& name)
  : theTruthManager(TruthStrategyManager::GetStrategyManager()),
    m_msg(name),
    strategyName(name),
    m_init(false)
{
  // Register myself with the manager.
  theTruthManager->RegisterStrategy(this);
  activated = false;
}

bool TruthStrategy::Activate(const std::string vN, int iL)
{
  MCActiveArea.emplace(vN, iL);
  if(MCActiveArea.size()>0) activated=true;

  for(const auto& volPair : MCActiveArea) {
    ATH_MSG_INFO("MCTruth::TruthStrategy: " << strategyName <<
                 " activated at " << volPair.first <<
                 " and level " << volPair.second);
  }
  ATH_MSG_INFO("MCTruth::TruthStrategy: " << strategyName <<
               "active in " << MCActiveArea.size() << " main volume");
  return activated;
}

bool TruthStrategy::IsApplicable(const G4Step* aStep)
{
  StepHelper step(aStep);
  bool applicable = false;
  for(const auto& volPair : MCActiveArea) {
    //std::cout<<"MCTruth::TruthStrategy: "<<strategyName<<
    //           " " <<volPair.first<<" "<<volPair.second<<std::endl;
    if(step.PostStepBranchDepth() >= volPair.second &&
       step.GetPostStepLogicalVolumeName(volPair.second) == volPair.first)
      applicable=true;
  }
  if (!m_init){ // Validation of configuration
    checkVolumeDepth( G4TransportationManager::GetTransportationManager()->
                      GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume() );
    m_init=true;
  }
  return applicable;
}

void TruthStrategy::checkVolumeDepth( G4LogicalVolume * lv , int d ){
  if (lv==0) return;
  for(const auto& volPair : MCActiveArea) {
    if (d!=volPair.second && lv->GetName().compareTo(volPair.first)==0){
      ATH_MSG_ERROR("Volume " << lv->GetName() << " at depth " << d <<
                    " instead of depth " << volPair.second);
      throw GaudiException("Volume at wrong depth", "WrongDepth", StatusCode::FAILURE);
    }
    else if (d == volPair.second && lv->GetName().compareTo(volPair.first)==0){
      ATH_MSG_DEBUG("Volume " << lv->GetName() <<
                    " is correctly registered at depth " << d);
    }
  }
  for (int i=0; i<lv->GetNoDaughters(); ++i){
    checkVolumeDepth( lv->GetDaughter(i)->GetLogicalVolume() , d+1 );
  }
}
