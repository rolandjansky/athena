/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TruthStrategy.h"
#include "MCTruth/TruthStrategyManager.h"
#include "SimHelpers/StepHelper.h"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

TruthStrategy::TruthStrategy(const std::string sN):
		  theTruthManager(TruthStrategyManager::GetStrategyManager()),
          m_log(0), 
          strategyName(sN)
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
	       "active in "<<MCActiveArea.size()<<" main volume"<<std::endl;
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

