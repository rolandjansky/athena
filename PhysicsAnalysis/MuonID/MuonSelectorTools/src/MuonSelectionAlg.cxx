/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSelectionAlg.h"

#include "AthContainers/ConstDataVector.h"
#include "xAODMuon/MuonContainer.h"
namespace CP {

    MuonSelectionAlg::MuonSelectionAlg(const std::string& name, ISvcLocator* svcloc) :
        AthAlgorithm(name, svcloc), m_tool("CP::MuonSelectionTool") /*public tool*/ {
        declareProperty("Input", m_inputMuons = "");
        declareProperty("Output", m_outputMuons = "");
        declareProperty("Tool", m_tool);
    }

    StatusCode MuonSelectionAlg::initialize() {
        if (m_inputMuons.empty()) {
            ATH_MSG_ERROR("You must specify an INPUT muon collection");
            return StatusCode::FAILURE;
        }
        if (m_outputMuons.empty()) {
            ATH_MSG_ERROR("You must specify an OUTPUT muon collection");
            return StatusCode::FAILURE;
        }
        CHECK(m_tool.retrieve());

        return StatusCode::SUCCESS;
    }

    StatusCode MuonSelectionAlg::execute() {
        // fetch input collection
        const xAOD::MuonContainer* muons = nullptr;
        CHECK(evtStore()->retrieve(muons, m_inputMuons));

        // create a view container for the selected muons
        // basically acts like pointers to the selected muons in the muon container
        ConstDataVector<xAOD::MuonContainer>* out = new ConstDataVector<xAOD::MuonContainer>(SG::VIEW_ELEMENTS);

        for (const xAOD::Muon* iMuon : *muons) {
            if (m_tool->accept(*iMuon)) out->push_back(iMuon);
        }
        ATH_MSG_DEBUG("Selected " << out->size() << " muons");

        CHECK(evtStore()->record(out, m_outputMuons));
        // if(evtStore()->proxy(muons)->isConst()) CHECK( evtStore()->setConst(out) ); - may allow in the future
        CHECK(evtStore()->setConst(out));  // for now we just always lock everything we put into storegate

        return StatusCode::SUCCESS;
    }

}  // namespace CP
