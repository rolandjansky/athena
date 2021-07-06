/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtRdoToPrepDataTool.h"

#include "CxxUtils/checker_macros.h"

Muon::MdtRdoToPrepDataTool::MdtRdoToPrepDataTool(const std::string& t, const std::string& n, const IInterface* p) : base_class(t, n, p) {}

StatusCode Muon::MdtRdoToPrepDataTool::initialize() {
    ATH_MSG_VERBOSE("Starting init");
    ATH_CHECK(MdtRdoToPrepDataToolCore::initialize());
    ATH_MSG_DEBUG("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

void Muon::MdtRdoToPrepDataTool::printPrepData() const { printPrepDataImpl(m_mdtPrepDataContainer); }

Muon::MdtPrepDataContainer* Muon::MdtRdoToPrepDataTool::setupMdtPrepDataContainer(unsigned int sizeVectorRequested,
                                                                                  bool& fullEventDone) const {
    fullEventDone = false;
    if (!evtStore()->contains<Muon::MdtPrepDataContainer>(m_mdtPrepDataContainerKey.key())) {
        m_fullEventDone = false;

        SG::WriteHandle<Muon::MdtPrepDataContainer> handle(m_mdtPrepDataContainerKey);
        StatusCode status = handle.record(std::make_unique<Muon::MdtPrepDataContainer>(m_idHelperSvc->mdtIdHelper().module_hash_max()));

        if (status.isFailure() || !handle.isValid()) {
            ATH_MSG_FATAL("Could not record container of MDT PrepData Container at " << m_mdtPrepDataContainerKey.key());
            return nullptr;
        }
        m_mdtPrepDataContainer = handle.ptr();
    } else {
        const Muon::MdtPrepDataContainer* outputCollection_c = nullptr;
        if (evtStore()->retrieve(outputCollection_c, m_mdtPrepDataContainerKey.key()).isFailure()) { return nullptr; }
        m_mdtPrepDataContainer = const_cast<Muon::MdtPrepDataContainer*>(outputCollection_c);
    }
    fullEventDone = m_fullEventDone;

    // if requesting full event, set the full event done flag to true
    if (sizeVectorRequested == 0) m_fullEventDone = true;

    return m_mdtPrepDataContainer;
}
