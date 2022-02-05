/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_RawDataProviderTool.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "CxxUtils/checker_macros.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

// using namespace OFFLINE_FRAGMENTS_NAMESPACE;
Muon::MDT_RawDataProviderTool::MDT_RawDataProviderTool(const std::string& t, const std::string& n, const IInterface* p) :
    base_class(t, n, p) {
    declareInterface<Muon::IMuonRawDataProviderTool>(this);
}

Muon::MDT_RawDataProviderTool::~MDT_RawDataProviderTool() {}

StatusCode Muon::MDT_RawDataProviderTool::initialize() {
    ATH_MSG_VERBOSE("Starting init");

    ATH_CHECK(MDT_RawDataProviderToolCore::initialize());

    ATH_MSG_DEBUG("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

StatusCode Muon::MDT_RawDataProviderTool::finalize() { return StatusCode::SUCCESS; }

// the new one
StatusCode Muon::MDT_RawDataProviderTool::convert() const  // call decoding function using list of all detector ROBId's
{
    return this->convert(Gaudi::Hive::currentContext());
}

// EventContext
StatusCode Muon::MDT_RawDataProviderTool::convert(const EventContext& ctx) const {
    SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey, ctx};
    const MuonMDT_CablingMap* readCdo{*readHandle};
    if (!readCdo) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }
    return convert(readCdo->getAllROBId(), ctx);
}

StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<IdentifierHash>& HashVec) const {
    return this->convert(HashVec, Gaudi::Hive::currentContext());
}

// EventContext
StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<IdentifierHash>& HashVec, const EventContext& ctx) const {
    SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey, ctx};
    const MuonMDT_CablingMap* readCdo{*readHandle};
    if (!readCdo) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }
    return convert(readCdo->getROBId(HashVec, msgStream()), ctx);
}

StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<uint32_t>& robIds) const {
    return this->convert(robIds, Gaudi::Hive::currentContext());  // using the old one
}

StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<uint32_t>& robIds, const EventContext& ctx) const {
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    m_robDataProvider->getROBData(robIds, vecOfRobf);
    return convert(vecOfRobf, ctx);  // using the old one
}

StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                                  const std::vector<IdentifierHash>&) const {
    return this->convert(vecRobs, Gaudi::Hive::currentContext());
}

StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                                  const std::vector<IdentifierHash>& /*collections*/, const EventContext& ctx) const {
    return convert(vecRobs, ctx);
}

/// This decode function is for single-thread running only
StatusCode Muon::MDT_RawDataProviderTool::convert ATLAS_NOT_THREAD_SAFE(
    const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs) const {
    return this->convert(vecRobs, Gaudi::Hive::currentContext());
}

StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                                  const EventContext& ctx) const {
    ATH_MSG_VERBOSE("convert(): " << vecRobs.size() << " ROBFragments.");

    SG::WriteHandle<MdtCsmContainer> rdoContainerHandle(m_rdoContainerKey, ctx);

    MdtCsmContainer* rdoContainer = nullptr;

    // here we have to check if the container is already present and if it is we retrieve from SG
    if (rdoContainerHandle.isPresent()) {
        const MdtCsmContainer* rdoContainer_c = nullptr;
        ATH_CHECK(evtStore()->retrieve(rdoContainer_c, m_rdoContainerKey.key()));
        rdoContainer = const_cast<MdtCsmContainer*>(rdoContainer_c);

    } else {
        ATH_CHECK(rdoContainerHandle.record(std::make_unique<MdtCsmContainer>(m_maxhashtoUse)));
        ATH_MSG_DEBUG("Created container");
        rdoContainer = rdoContainerHandle.ptr();
    }

    // this should never happen, but since we dereference the pointer, we should check
    if (!rdoContainer) {
        ATH_MSG_ERROR("MdtCsmContainer is null, cannot decode MDT data");
        return StatusCode::FAILURE;
    }

    // use the convert function in the MDT_RawDataProviderToolCore class
    ATH_CHECK(convertIntoContainer(vecRobs, *rdoContainer));

    return StatusCode::SUCCESS;
}
