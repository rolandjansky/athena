/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// MuonClusterHypoAlg
// (see header for history/etc..)
//
#include <cmath>
#include <algorithm>
#include <sstream>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "TrigLongLivedParticlesHypo/MuonClusterHypoAlg.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::viewString;
using TrigCompositeUtils::featureString;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::LinkInfo;
using TrigCompositeUtils::hypoAlgNodeName;
using TrigCompositeUtils::allFailed;

MuonClusterHypoAlg::MuonClusterHypoAlg(const std::string & name, ISvcLocator* pSvcLocator):
                          HypoBase(name, pSvcLocator){
}

MuonClusterHypoAlg::~MuonClusterHypoAlg(){
}

StatusCode MuonClusterHypoAlg::initialize()
{

    ATH_MSG_DEBUG("in initialize()");
    ATH_CHECK( m_outputCompositesKey.initialize() );

    if (!m_hypoTools.empty()) ATH_CHECK(m_hypoTools.retrieve());
    if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());


    ATH_MSG_INFO("initialize() success");
    return StatusCode::SUCCESS;
}

StatusCode MuonClusterHypoAlg::execute(const EventContext& ctx) const
{

    ATH_MSG_DEBUG("in execute()");

    auto CluNum = Monitored::Scalar("NumClu", 3);

    auto acceptAll     = Monitored::Scalar("AcceptAll", false);
    auto nRoIEndCap    = Monitored::Scalar("nRoIEndCap", 4);
    auto nRoIBarrel    = Monitored::Scalar("nRoIBarrel", 3);
    auto etaMax        = Monitored::Scalar("maxEta", 2.5);
    auto etaMid        = Monitored::Scalar("midEta", 1.0);
    auto isPass        = Monitored::Scalar("isPass", 0);

    auto t1            = Monitored::Timer("TIME_HypoAlg");
    auto t2            = Monitored::Timer("TIME_HypoAlg_DecisionLoop");

    auto mon = Monitored::Group(m_monTool, acceptAll, nRoIEndCap, nRoIBarrel, etaMax, etaMid, CluNum, isPass, t1, t2);

    if(acceptAll) {
        ATH_MSG_DEBUG("Accepting all the events with no cuts!");
    } else {
        ATH_MSG_DEBUG("Selecting barrel muon RoI clusters with |eta|< " << etaMid << " and >= " << nRoIBarrel << " RoIs");
        ATH_MSG_DEBUG("and endcap muon RoI clusters with |eta| > : " << etaMid << "and >= " <<nRoIEndCap << " RoIs");
    }

    // I think this imports previous trigger chain decisions, which we'll append to?
    auto previousDecisionsHandle = SG::makeHandle( decisionInput(), ctx );
    ATH_CHECK( previousDecisionsHandle.isValid() );
    ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

    // New trigger decisions
    SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), ctx); // Creating a decision output for our HypoTool
    auto decisions = outputHandle.ptr();

    // Getting previous Decision
    const Decision *previousDecision = previousDecisionsHandle->at(0);

    // Get the TrigCompositeContainer linked to the ReadHandleKey
    // it contains the Muon RoI Cluster information
    auto compContHdl = SG::makeHandle(m_outputCompositesKey, ctx);
    auto compCont = compContHdl.get();

    // Creating a DecisionIDContainer to hold all active chain IDs
    DecisionIDContainer prev;

    // Create new decisions and link to previous decision
    Decision* d = newDecisionIn(decisions, previousDecision, hypoAlgNodeName(), ctx);
    linkToPrevious(d, previousDecision, ctx);
    decisionIDs(d, prev);                   // This adds the active chain ID to prev
    d->setObjectLink( featureString(), ElementLink<xAOD::TrigCompositeContainer>(*compCont, 0, ctx) );

    // Creating the DecisionInfo struct to pass to the HypoTool
    MuonClusterHypoTool::DecisionInfo tool_info{d, compCont, prev};

    for ( auto& tool: m_hypoTools ){
        ATH_CHECK( tool->decide(tool_info) );
    }

    ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

    t2.start();
    // We only have 1 decision. If more decisions are needed in the future, wrap 'if' in a loop over the decision container.
    if (!allFailed(d)) {isPass = 1;}
    t2.stop();

    return StatusCode::SUCCESS;

}
