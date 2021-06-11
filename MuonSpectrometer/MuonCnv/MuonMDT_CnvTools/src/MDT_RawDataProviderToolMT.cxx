/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_RawDataProviderToolMT.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

Muon::MDT_RawDataProviderToolMT::MDT_RawDataProviderToolMT(const std::string& t, const std::string& n, const IInterface* p) :
    base_class(t, n, p), m_rdoContainerCacheKey("") {
    declareInterface<Muon::IMuonRawDataProviderTool>(this);

    declareProperty("CsmContainerCacheKey", m_rdoContainerCacheKey, "Optional external cache for the CSM container");
}

StatusCode Muon::MDT_RawDataProviderToolMT::initialize() {
    ATH_MSG_VERBOSE("Starting init");

    ATH_CHECK(MDT_RawDataProviderToolCore::initialize());

    ATH_CHECK(m_rdoContainerCacheKey.initialize(!m_rdoContainerCacheKey.key().empty()));

    ATH_MSG_DEBUG("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

StatusCode Muon::MDT_RawDataProviderToolMT::finalize() { return StatusCode::SUCCESS; }

// the new one
StatusCode Muon::MDT_RawDataProviderToolMT::convert() const  // call decoding function using list of all detector ROBId's
{
    return this->convert(Gaudi::Hive::currentContext());
}

StatusCode Muon::MDT_RawDataProviderToolMT::convert(
    const EventContext& ctx) const  // call decoding function using list of all detector ROBId's
{
    SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey, ctx};
    const MuonMDT_CablingMap* readCdo{*readHandle};
    if (readCdo == nullptr) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }
    return convert(readCdo->getAllROBId(), ctx);
}

StatusCode Muon::MDT_RawDataProviderToolMT::convert(const std::vector<IdentifierHash>& HashVec) const {
    return this->convert(HashVec, Gaudi::Hive::currentContext());
}

StatusCode Muon::MDT_RawDataProviderToolMT::convert(const std::vector<IdentifierHash>& HashVec, const EventContext& ctx) const {
    SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey, ctx};
    const MuonMDT_CablingMap* readCdo{*readHandle};
    if (readCdo == nullptr) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }
    return convert(readCdo->getROBId(HashVec), ctx);
}

StatusCode Muon::MDT_RawDataProviderToolMT::convert(const std::vector<uint32_t>& robIds) const {
    return this->convert(robIds, Gaudi::Hive::currentContext());
}

StatusCode Muon::MDT_RawDataProviderToolMT::convert(const std::vector<uint32_t>& robIds, const EventContext& ctx) const {
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    m_robDataProvider->getROBData(robIds, vecOfRobf);
    return convert(vecOfRobf, ctx);  // using the old one
}

StatusCode Muon::MDT_RawDataProviderToolMT::convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                                    const std::vector<IdentifierHash>&) const {
    return this->convert(vecRobs, Gaudi::Hive::currentContext());
}

StatusCode Muon::MDT_RawDataProviderToolMT::convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                                    const std::vector<IdentifierHash>& /*collection*/, const EventContext& ctx) const {
    return convert(vecRobs, ctx);
}

StatusCode Muon::MDT_RawDataProviderToolMT::convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs) const {
    return this->convert(vecRobs, Gaudi::Hive::currentContext());
}

StatusCode Muon::MDT_RawDataProviderToolMT::convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                                    const EventContext& ctx) const {
    ATH_MSG_VERBOSE("convert(): " << vecRobs.size() << " ROBFragments.");

    SG::WriteHandle<MdtCsmContainer> rdoContainerHandle(m_rdoContainerKey, ctx);

    MdtCsmContainer* rdoContainer = nullptr;

    // here we have two paths. The first one we do not use an external cache, so just create
    // the MDT CSM container and record it. For the second path, we create the container
    // by passing in the container cache key so that the container is linked with the event
    // wide cache.
    const bool externalCacheRDO = !m_rdoContainerCacheKey.key().empty();
    if (!externalCacheRDO) {
        // without the cache we just record the container
        ATH_CHECK(rdoContainerHandle.record(std::make_unique<MdtCsmContainer>(m_maxhashtoUse)));
        ATH_MSG_DEBUG("Created container");
        rdoContainer = rdoContainerHandle.ptr();
    } else {
        // use the cache to get the container
        SG::UpdateHandle<MdtCsm_Cache> update(m_rdoContainerCacheKey, ctx);
        ATH_CHECK(update.isValid());
        ATH_CHECK(rdoContainerHandle.record(std::make_unique<MdtCsmContainer>(update.ptr())));
        ATH_MSG_DEBUG("Created container using cache for " << m_rdoContainerCacheKey.key());
        rdoContainer = rdoContainerHandle.ptr();
    }

    // this should never happen, but since we dereference the pointer we should check it first
    if (!rdoContainer) {
        ATH_MSG_ERROR("MdtCsmContainer is null, cannot convert MDT raw data");
        return StatusCode::FAILURE;
    }

    // use the convert function in the MDT_RawDataProviderToolCore class
    ATH_CHECK(convertIntoContainer(vecRobs, *rdoContainer));

    return StatusCode::SUCCESS;
}
