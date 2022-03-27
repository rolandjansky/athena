/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigMuonTLAHypoAlg.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"

using namespace TrigCompositeUtils;
using xAOD::MuonContainer;

TrigMuonTLAHypoAlg::TrigMuonTLAHypoAlg(const std::string &name, ISvcLocator *pSvcLocator) : ::HypoBase(name, pSvcLocator) {}

StatusCode TrigMuonTLAHypoAlg::initialize()
{
    // not used now
   // ATH_CHECK(m_hypoTools.retrieve());
    ATH_CHECK(m_TLAMuonsKey.initialize());  

    if ( not m_monTool.name().empty() ) {
      ATH_CHECK( m_monTool.retrieve() );
      ATH_MSG_DEBUG("MonTool name: " << m_monTool);
    }      

    ATH_MSG_DEBUG("Initializing TrigMuonTLAHypoAlg");
    return StatusCode::SUCCESS;
}

StatusCode TrigMuonTLAHypoAlg::execute(const EventContext &ctx) const
{
    ATH_MSG_DEBUG("Executing " << name() << "...");

    // create handles for TLA muons
    SG::WriteHandle<MuonContainer> h_TLAMuons = SG::makeHandle(m_TLAMuonsKey, ctx);
    //make the output muon container
    ATH_CHECK(h_TLAMuons.record(std::make_unique<xAOD::MuonContainer>(),
                                std::make_unique<xAOD::MuonAuxContainer>()));

    auto previousDecisionHandle = SG::makeHandle(decisionInput(), ctx);
    ATH_CHECK(previousDecisionHandle.isValid());
    ATH_MSG_DEBUG("Running with " << previousDecisionHandle->size() << " previous decisions");

    // prepare output decisions
    SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), ctx);
    DecisionContainer *outputDecisions = outputHandle.ptr();
    

    // the HypoTool needs a pair of <object*, decision*> as input
    std::vector<std::pair<Decision *, const Decision *> > HypoInputs;

    // loops over previous decisions
    int nDecision = 0;
    for (const auto previousDecision : *previousDecisionHandle)
    {
        /* differnt behaviours are possibile
            - 1) copy all muons linked by the decisions (those passing the chains)
            - 2) copy only one muon per decision
            Now follow 1), and at the end, to reduce the output colleciton, one can resolves overlaps between Views 
        */

        // get muons from the decision
        const xAOD::Muon *muonPrev = nullptr;
        auto prevMuons = TrigCompositeUtils::findLinks<xAOD::MuonContainer>(previousDecision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);
        ATH_MSG_DEBUG("This decision has " << prevMuons.size() << " decisions");

        // verify that only one object is found per decision
        if (prevMuons.size() != 1) {
            ATH_MSG_DEBUG("Did not locate exactly one muon for this Decision Object, found " << prevMuons.size());
            return StatusCode::FAILURE;
        }
     
        auto prevMuLink = prevMuons.at(0).link;
        ATH_CHECK(prevMuLink.isValid());
        muonPrev = *prevMuLink;

        xAOD::Muon *copiedMuon = new xAOD::Muon();            
        h_TLAMuons->push_back(copiedMuon);
        *copiedMuon = *muonPrev;

        // now go on with the normal Hypo, linking new decision with previous one
        auto newDecision = newDecisionIn( outputDecisions, hypoAlgNodeName() );
        TrigCompositeUtils::linkToPrevious( newDecision, previousDecision, ctx );
        newDecision->setObjectLink(featureString(), ElementLink<xAOD::MuonContainer>(*h_TLAMuons, h_TLAMuons->size() - 1, ctx));

        ATH_MSG_DEBUG("Copied muon with pT: " << copiedMuon->pt() << " from decision " << nDecision);

        HypoInputs.push_back( std::make_pair(newDecision, previousDecision) );
        nDecision++;
    }

    // add monitor variables    
    auto Nmuons = Monitored::Scalar("Nmuon", -9999.);
    Nmuons = h_TLAMuons->size();
    auto muonPtMon  = Monitored::Collection("Pt", *h_TLAMuons, []( const auto& t ) { return t->pt() * t->charge() / Gaudi::Units::GeV; });
    auto muonEtaMon = Monitored::Collection("Eta", *h_TLAMuons, &xAOD::Muon::eta);
    auto muonPhiMon = Monitored::Collection("Phi", *h_TLAMuons, &xAOD::Muon::phi);
    auto muonAuthor	= Monitored::Collection("Author", *h_TLAMuons, &xAOD::Muon::author);
    auto monitorIt  = Monitored::Group(m_monTool, Nmuons, muonPtMon, muonAuthor, muonEtaMon, muonPhiMon);


    // this is bypassing any selectioon, remove if you want to apply HypoTools
    for (auto& hypoPair: HypoInputs ){
        TrigCompositeUtils::insertDecisionIDs(hypoPair.second, hypoPair.first);
    }
    
    ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

    return StatusCode::SUCCESS;
}
