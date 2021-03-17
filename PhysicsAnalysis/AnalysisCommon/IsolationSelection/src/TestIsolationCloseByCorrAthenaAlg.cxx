/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

//// Developed by Johannes Josef Junggeburth (jojungge@cern.ch)
// Local include(s):
#include "TestIsolationCloseByCorrAthenaAlg.h"

// EDM include(s):
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include <IsolationSelection/IIsolationCloseByCorrectionTool.h>
#include <IsolationSelection/IIsolationSelectionTool.h>
#include <IsolationSelection/IsoVariableHelper.h>
#include <IsolationSelection/TestMacroHelpers.h>
#include <IsolationSelection/Defs.h>

namespace CP {
    static CharDecorator dec_PassQuality("isCloseByObject");
    static CharDecorator dec_Consider("considerInCorrection");
    static CharDecorator dec_PassIsol("defaultIso");

    TestIsolationCloseByCorrAthenaAlg::TestIsolationCloseByCorrAthenaAlg(const std::string& name, ISvcLocator* svcLoc) :
                AthAlgorithm(name, svcLoc),
                m_histSvc("THistSvc", name),
                m_isoCloseByCorrTool(""),
                m_isoSelectorTool(""),
                m_tree(nullptr),
                m_eventNumber(0),
                m_ele_helper(),
                m_muo_helper(),
                m_pho_helper(),
                m_consider_ele(true),
                m_consider_muo(true),
                m_consider_pho(true),
                m_mu_min_pt(5.e3),
                m_mu_max_eta(2.7),
                m_el_min_pt(7.e3),
                m_el_max_eta(2.47),
                m_ph_min_pt(25.e3),
                m_ph_max_eta(2.35) {
        declareProperty("IsoCloseByCorrTool", m_isoCloseByCorrTool);
        declareProperty("IsoSelectorTool", m_isoSelectorTool);
        declareProperty("considerElectrons", m_consider_ele);
        declareProperty("considerMuons", m_consider_muo);
        declareProperty("considerPhotons", m_consider_pho);
        
        declareProperty("MuonPt", m_mu_min_pt);
        declareProperty("MuonEta", m_mu_max_eta);
        declareProperty("ElectronPt", m_el_min_pt);
        declareProperty("ElectronEta", m_el_max_eta);
        declareProperty("PhotonPt", m_ph_min_pt);
        declareProperty("PhotonEta", m_ph_max_eta);
        
    }

    StatusCode TestIsolationCloseByCorrAthenaAlg::initialize() {
        ATH_CHECK(m_isoSelectorTool.retrieve());
        ATH_CHECK(m_isoCloseByCorrTool.retrieve());

        m_tree = new TTree("IsoCorrTest", "Test tree for the isolaiton correction tool");
        m_tree->Branch("eventNumber" , &m_eventNumber);
        m_ele_helper = std::make_unique < IsoCorrectionTestHelper > (m_tree, "Electrons", m_isoSelectorTool->getElectronWPs(), xAOD::Type::ObjectType::Electron);
        m_muo_helper = std::make_unique < IsoCorrectionTestHelper > (m_tree, "Muons", m_isoSelectorTool->getMuonWPs(), xAOD::Type::ObjectType::Muon);
        m_pho_helper = std::make_unique < IsoCorrectionTestHelper > (m_tree, "Photons", m_isoSelectorTool->getPhotonWPs(), xAOD::Type::ObjectType::Photon);

        ATH_CHECK(m_histSvc->regTree("/ISOCORRECTION/IsoCorrTest", m_tree));

        return StatusCode::SUCCESS;
    }

    StatusCode TestIsolationCloseByCorrAthenaAlg::execute() {
        xAOD::ElectronContainer* Electrons = nullptr;
        xAOD::MuonContainer* Muons = nullptr;
        xAOD::PhotonContainer* Photons = nullptr;
        const xAOD::EventInfo* info = nullptr;
        ATH_CHECK(evtStore()->retrieve(info,"EventInfo"));
        ATH_MSG_DEBUG("Start to run over event "<<info->eventNumber()<<".");
        m_eventNumber = info->eventNumber();
        //Create the links to the shallow copy objects
        ATH_CHECK(CreateContainerLinks("Electrons", Electrons));
        for (const auto ielec : *Electrons) {
            //Store if the electron passes the isolation
            dec_PassIsol(*ielec) = m_isoSelectorTool->accept(*ielec);
            //Quality criteria only baseline kinematic selection
            dec_PassQuality(*ielec) =  m_consider_ele && ielec->pt() > m_el_min_pt &&( m_el_max_eta < 0 || fabs(ielec->eta()) < m_el_max_eta);
            dec_Consider(*ielec) = ielec->pt() > m_el_min_pt &&( m_el_max_eta < 0 || fabs(ielec->eta()) < m_el_max_eta);
        }
        ATH_CHECK(CreateContainerLinks("Photons", Photons));
        for (const auto iphot : *Photons) {
            //Store if the photon passes the isolation (only needed for later comparisons)
            dec_PassIsol(*iphot) = m_isoSelectorTool->accept(*iphot);
            //Quality criteria only baseline kinematic selection
            dec_PassQuality(*iphot) = m_consider_pho && iphot->pt() > m_ph_min_pt && (m_ph_max_eta < 0 || fabs(iphot->eta()) < m_ph_max_eta);
            dec_Consider(*iphot) =  iphot->pt() > m_ph_min_pt && (m_ph_max_eta < 0 || fabs(iphot->eta()) < m_ph_max_eta);
        }

        ATH_CHECK(CreateContainerLinks("Muons", Muons));
        for (const auto imuon : *Muons) {
            //Store if the muon passes the isolation
            dec_PassIsol(*imuon) = m_isoSelectorTool->accept(*imuon);
            //Quality criteria only baseline kinematic selection
            dec_PassQuality(*imuon) = m_consider_muo && imuon->pt() > m_mu_min_pt && ( m_mu_max_eta < 0 || fabs(imuon->eta()) < m_mu_max_eta);
            dec_Consider(*imuon) = imuon->pt() > m_mu_min_pt && ( m_mu_max_eta < 0 || fabs(imuon->eta()) < m_mu_max_eta);
        }
        //Okay everything is defined for the preselection of the algorithm. lets  pass the things  towards the IsoCorrectionTool

        if (m_isoCloseByCorrTool->getCloseByIsoCorrection(Electrons, Muons, Photons).code() == CorrectionCode::Error) {
            ATH_MSG_ERROR("Something weird happened with the tool");
            return StatusCode::FAILURE;
        }
        // The isoCorrectionTool has now corrected everything using close-by objects satisfiyng the dec_PassQuality criteria
        // The name of the decorator is set via the 'SelectionDecorator' property of the tool
        // Optionally one can also define that the tool shall only objects surviving the overlap removal without  changing the initial decorator
        // Use therefore the 'PassOverlapDecorator' property to define the decorators name
        // If you define  the 'BackupPrefix' property then the original values are stored before correction <Prefix>_<IsolationCone>
        // The final result  whether the object  passes the isolation criteria now can be stored in the 'IsolationSelectionDecorator' e.g. 'CorrectedIso'

        // parse the associated muon clusters to the tool
        ClusterCollection muon_clusters;
        correction_tool()->getClusterCandidates(Muons, muon_clusters);       
        m_muo_helper->SetClusters(muon_clusters);
        
        //Store everything in the final ntuples
        ATH_CHECK(m_ele_helper->Fill(Electrons));
        ATH_CHECK(m_muo_helper->Fill(Muons));
        ATH_CHECK(m_pho_helper->Fill(Photons));
        m_tree->Fill();
        return StatusCode::SUCCESS;
    }
    const CP::IsolationCloseByCorrectionTool* TestIsolationCloseByCorrAthenaAlg::correction_tool() const {
        return dynamic_cast<const  CP::IsolationCloseByCorrectionTool*> (m_isoCloseByCorrTool.get());
    }
}
