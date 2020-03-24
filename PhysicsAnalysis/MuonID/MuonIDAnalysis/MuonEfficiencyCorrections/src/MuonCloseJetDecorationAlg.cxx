/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
// Local include(s):
#include "MuonCloseJetDecorationAlg.h"
#include "PATInterfaces/SystematicsTool.h"
#include <cmath>

#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "FourMomUtils/xAODP4Helpers.h"
namespace CP {
    MuonCloseJetDecorationAlg::MuonCloseJetDecorationAlg(const std::string& name, ISvcLocator* svcLoc) :
                AthAlgorithm(name, svcLoc),
                m_muon_container("Muons"),                   
                m_jet_container("AntiKt4EMTopoJets"),
                m_jet_dr_decoration("dRJet"),
                m_dummy_value(-1.),
                m_jet_pt_cut(20.e3),
                m_decorator(nullptr) {
        declareProperty("MuonContainer", m_muon_container);
        declareProperty("JetContainer", m_jet_container);
        declareProperty("dRDecoration", m_jet_dr_decoration);
        declareProperty("DefaultValue", m_dummy_value);  
        declareProperty("MinJetPt", m_jet_pt_cut);
    }
    StatusCode MuonCloseJetDecorationAlg::initialize() {
        if (m_muon_container.empty()){
            ATH_MSG_FATAL("Please provide a suitable muon container");
            return StatusCode::FAILURE;
        }
        if (m_jet_container.empty()){
            ATH_MSG_FATAL("Please provide a suitable jet container");
            return StatusCode::FAILURE;
        }
        if (m_jet_dr_decoration.empty()){
            ATH_MSG_FATAL("What's the decoration to apply to the muon");
            return StatusCode::FAILURE;
        }
        m_decorator = std::make_unique<SG::AuxElement::Decorator<float>>(m_jet_dr_decoration);
        ATH_MSG_INFO("Decorator the jet dR between "<<m_muon_container<<" and "<<m_jet_container<<" to "
                    <<m_jet_dr_decoration<<". If no jet is found the following dummy value is assigned: "<<m_dummy_value);
        return StatusCode::SUCCESS;
    }

    StatusCode MuonCloseJetDecorationAlg::execute() {

        const xAOD::MuonContainer* muons = nullptr;
        ATH_CHECK(evtStore()->retrieve(muons, m_muon_container));
        
        const xAOD::JetContainer* jets = nullptr;
        ATH_CHECK(evtStore()->retrieve(jets, m_jet_container));
        for ( const xAOD::Muon* mu : *muons){
            const xAOD::Jet* closest_jet = nullptr;
            for (const xAOD::Jet* jet: *jets){
                if (jet->pt() < m_jet_pt_cut) continue;
                if (closest_jet == nullptr ||  xAOD::P4Helpers::deltaR2(jet,mu) < xAOD::P4Helpers::deltaR2(closest_jet,mu)) closest_jet = jet;
            }           
            (*m_decorator)(*mu) = closest_jet? xAOD::P4Helpers::deltaR(closest_jet,mu) : m_dummy_value;
        }
        return StatusCode::SUCCESS;
    }

} // namespace CP
