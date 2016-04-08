/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruthBase/TruthStrategy.h"
#include "MCTruthBase/TruthStrategyManager.h"
#include "SimHelpers/StepHelper.h"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

// For depth checking
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4LogicalVolume.hh"
#include <stdexcept>

TruthStrategy::TruthStrategy(const std::string sN):
		  theTruthManager(TruthStrategyManager::GetStrategyManager()),
          m_log(0), 
          strategyName(sN),
          m_init(false)
{	
	theTruthManager->RegisterStrategy(this);
	activated=false;
}	
bool TruthStrategy::Activate(const std::string vN, int iL)
{
    MCActiveArea.insert(valType(std::string(vN),iL));
    if(MCActiveArea.size()>0) activated=true;

    std::map<std::string,int>::iterator it;
    for(it=MCActiveArea.begin();it!=MCActiveArea.end();++it){
    log()<<MSG::INFO<<"MCTruth::TruthStrategy: "<<strategyName<<
           " activated at "<<(*it).first<<
           " and level "<<(*it).second<<std::endl;
    }
    log()<<MSG::INFO<<"MCTruth::TruthStrategy: "<<strategyName<<
           "active in "<<MCActiveArea.size()<<" main volume"<<endreq;
    return activated;
}
bool TruthStrategy::IsApplicable(const G4Step* aStep)
{
    static StepHelper step(aStep);
    bool applicable=false;
    std::map<std::string,int>::iterator it;
    for(it=MCActiveArea.begin();it!=MCActiveArea.end();++it){
        //std::cout<<"MCTruth::TruthStrategy: "<<strategyName<<
        //           " " <<(*it).first<<" "<<(*it).second<<std::endl;
      if(step.PostStepBranchDepth()>=(*it).second && step.GetPostStepLogicalVolumeName((*it).second)==(*it).first)
        applicable=true;
    }
    if (!m_init){ // Validation of configuration
        checkVolumeDepth( G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume() );
        m_init=true;
    }
    return applicable;
}

MsgStream TruthStrategy::log()
{
    if (m_log) return *m_log;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* p_msgSvc = 0;
    if (svcLocator->service("MessageSvc", p_msgSvc).isFailure() || !p_msgSvc)
        std::cout << "FadsSensitiveDetector: Trouble getting the message service.  Should never happen.  Will crash now." << std::endl;
    m_log = new MsgStream(p_msgSvc,strategyName);
    return *m_log;
}

void TruthStrategy::checkVolumeDepth( G4LogicalVolume * lv , int d ){
  if (lv==0) return;
  std::map<std::string,int>::iterator it;
  for(it=MCActiveArea.begin();it!=MCActiveArea.end();++it){
    if (d!=(*it).second && lv->GetName().compareTo(it->first)==0){
      log()<<MSG::ERROR<<"Volume " << lv->GetName() << " at depth " << d << " instead of depth " << (*it).second << endreq;
      throw "WrongDepth";
    } else if (d==(*it).second && lv->GetName().compareTo(it->first)==0){
      log()<<MSG::DEBUG<<"Volume " << lv->GetName() << " is correctly registered at depth " << d << endreq;
    }
  }
  for (int i=0; i<lv->GetNoDaughters(); ++i){
    checkVolumeDepth( lv->GetDaughter(i)->GetLogicalVolume() , d+1 );
  }
}

