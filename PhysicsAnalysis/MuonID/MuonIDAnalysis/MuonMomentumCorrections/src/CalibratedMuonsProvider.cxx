/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// Local include(s):
#include "CalibratedMuonsProvider.h"

#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowCopy.h"

namespace CP {
    static SG::AuxElement::ConstAccessor<unsigned int> acc_rnd("RandomRunNumber");

    CalibratedMuonsProvider::CalibratedMuonsProvider(const std::string& name, ISvcLocator* svcLoc) : AthAlgorithm(name, svcLoc) {}

    StatusCode CalibratedMuonsProvider::initialize() {
        ATH_CHECK(m_eventInfo.initialize());
        ATH_CHECK(m_inputKey.initialize());
        ATH_CHECK(m_outputKey.initialize());
        ATH_CHECK(m_tool.retrieve());
        if (!m_prwTool.empty()) {
            m_useRndNumber = true;
            ATH_MSG_DEBUG("prwTool is given assume that the selection of the periods is based on the random run number");
            ATH_CHECK(m_prwTool.retrieve());
        }
        return StatusCode::SUCCESS;
    }

    StatusCode CalibratedMuonsProvider::execute() {
        const EventContext& ctx = Gaudi::Hive::currentContext();
        SG::ReadHandle<xAOD::MuonContainer> readHandle{m_inputKey, ctx};
        if (!readHandle.isValid()) {
            ATH_MSG_FATAL("No muon container found");
            return StatusCode::FAILURE;
        }
        const xAOD::MuonContainer* muons{readHandle.cptr()};

        std::pair<std::unique_ptr<xAOD::MuonContainer>, std::unique_ptr<xAOD::ShallowAuxContainer>> output =
            xAOD::shallowCopyContainer(*muons, ctx);

        if (!output.first || !output.second) {
            ATH_MSG_FATAL("Creation of shallow copy failed");
            return StatusCode::FAILURE;
        }

        if (!setOriginalObjectLink(*muons, *output.first)) {
            ATH_MSG_ERROR("Failed to add original object links to shallow copy of " << m_inputKey);
            return StatusCode::FAILURE;
        }

        if (m_useRndNumber) {
            SG::ReadHandle<xAOD::EventInfo> evInfo{m_eventInfo, ctx};
            if (!evInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
                m_useRndNumber = false;
                ATH_MSG_DEBUG("On data no random run number is needed.");
            } else if (!acc_rnd.isAvailable(*evInfo)) {
                ATH_MSG_DEBUG("Apply the prw tool");
                ATH_CHECK(m_prwTool->apply(*evInfo));
            }
        }
        for (xAOD::Muon* iParticle : *(output.first)) {
            ATH_MSG_DEBUG(" Old pt=" << iParticle->pt());
            if (m_tool->applyCorrection(*iParticle).code() == CorrectionCode::Error) return StatusCode::FAILURE;
            ATH_MSG_DEBUG(" New pt=" << iParticle->pt());
        }
        SG::WriteHandle<xAOD::MuonContainer> writeHandle{m_outputKey, ctx};
        ATH_CHECK(writeHandle.record(std::move(output.first), std::move(output.second)));
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

}  // namespace CP
