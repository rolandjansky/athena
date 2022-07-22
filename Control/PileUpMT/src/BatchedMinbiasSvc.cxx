/*
  Copyright (C) 2022 CERN for the benefit of the ATLAS collaboration
*/

#include "BatchedMinbiasSvc.h"

#include <algorithm>
#include <chrono>
#include <thread>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <boost/core/demangle.hpp>

#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IProxyProviderSvc.h"
#include "SGTools/CurrentEventStore.h"
// #include "AthenaKernel/IStoreGateLocked.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfo.h"

inline std::string CLIDToString(const CLID& clid) {
    return boost::core::demangle(CLIDRegistry::CLIDToTypeinfo(clid)->name());
}

BatchedMinbiasSvc::BatchedMinbiasSvc(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc),
      m_bkg_evt_sel_ctx(nullptr),
      m_idx_list(),
      m_cache(),
      m_cache_used(),
      m_cache_mtxs(),
      m_reading_batch_mtx(),
      m_empty_caches(),
      m_empty_caches_mtx(),
      m_batch_use_count(),
      m_last_loaded_batch(),
      m_last_unloaded_batch() {}

BatchedMinbiasSvc::~BatchedMinbiasSvc() {}

int BatchedMinbiasSvc::event_to_batch(std::uint64_t hs_id) {
    return int(hs_id / m_HSBatchSize.value());
}

StatusCode BatchedMinbiasSvc::initialize() {
    m_idx_list.resize(m_MBBatchSize.value(), 0);
    std::iota(m_idx_list.begin(), m_idx_list.end(), 0);

    m_cache.clear();
    m_cache_used.clear();
    m_empty_caches.clear();
    m_batch_use_count.clear();
    m_batch_use_count.reserve(m_actualNHSEventsPerBatch.value().size());
    for (std::size_t i = 0; i < m_actualNHSEventsPerBatch.value().size(); ++i) {
        m_batch_use_count.emplace_back(std::make_unique<std::atomic_int>(0));
    }
    ATH_CHECK(m_bkgEventSelector.retrieve());
    ATH_CHECK(m_activeStoreSvc.retrieve());
    // Setup context
    if (!m_bkgEventSelector->createContext(m_bkg_evt_sel_ctx).isSuccess()) {
        ATH_MSG_ERROR("Failed to create background event selector context");
        return StatusCode::FAILURE;
    }

    // Setup proxy provider
    IProxyProviderSvc* proxyProviderSvc = nullptr;
    ATH_CHECK(serviceLocator()->service(fmt::format("ProxyProviderSvc/BkgPPSvc_{}", name()),
                                        proxyProviderSvc, true));
    // Setup Address Providers
    IAddressProvider* addressProvider = dynamic_cast<IAddressProvider*>(m_bkgEventSelector.get());
    if (addressProvider == nullptr) {
        ATH_MSG_WARNING("Could not cast background event selector to IAddressProvider");
    }
    else {
        proxyProviderSvc->addProvider(addressProvider);
    }
    // AthenaPoolAddressProviderSvc
    IAddressProvider* athPoolAP = nullptr;
    IService* athPoolSvc = nullptr;
    ATH_CHECK(serviceLocator()->service(
          fmt::format("AthenaPoolAddressProviderSvc/BkgAPAPSvc_{}", name()), athPoolSvc));
    if ((athPoolAP = dynamic_cast<IAddressProvider*>(athPoolSvc)) == nullptr) {
        ATH_MSG_WARNING("Could not cast AthenaPoolAddressProviderSvc to IAddressProvider");
    }
    else {
        proxyProviderSvc->addProvider(athPoolAP);
    }
    // AddressRemappingSvc
    IAddressProvider* addRemapAP = nullptr;
    IService* addRemapSvc = nullptr;
    ATH_CHECK(serviceLocator()->service(fmt::format("AddressRemappingSvc/BkgARSvc_{}", name()),
                                        addRemapSvc));
    if ((addRemapAP = dynamic_cast<IAddressProvider*>(addRemapSvc)) == nullptr) {
        ATH_MSG_WARNING("Could not cast AddressRemappingSvc to IAddressProvider");
    }
    else {
        proxyProviderSvc->addProvider(addRemapAP);
    }

    int mbBatchSize = m_MBBatchSize.value();
    // setup NSimultaneousBatches vectors of MBBatchSize StoreGates in m_empty_caches
    for (int i = 0; i < m_NSimultaneousBatches.value(); ++i) {
        auto& sgs = m_empty_caches.emplace_back(std::make_unique<SGHandleArray>());
        sgs->reserve(mbBatchSize);
        for (int j = 0; j < mbBatchSize; ++j) {
            // creates / retrieves a different StoreGateSvc for each slot
            auto& sg = sgs->emplace_back(
                  fmt::format("StoreGateSvc/StoreGate_{}_{}_{}", name(), i, j), name());
            ATH_CHECK(sg.retrieve());
            sg->setStoreID(StoreID::PILEUP_STORE);
            sg->setProxyProviderSvc(proxyProviderSvc);
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode BatchedMinbiasSvc::beginHardScatter(std::uint64_t hs_id) {
    using namespace std::chrono_literals;
    int batch = event_to_batch(hs_id);
    while (true) {
        if (m_cache.count(batch)) {
            // batch already loaded
            // mutex prevents returning when batch is partially loaded
            m_cache_mtxs[batch].lock();
            m_cache_mtxs[batch].unlock();
            return StatusCode::SUCCESS;
        }
        // prevent batches loading out-of-order
        while (m_last_loaded_batch < (batch - 1)) {
            std::this_thread::sleep_for(50ms);
        }
        // See if there are any free caches
        // Using try_lock here to avoid reading same batch twice
        if (m_empty_caches_mtx.try_lock()) {
            if (!m_empty_caches.empty()) {
                std::scoped_lock reading{m_cache_mtxs[batch], m_reading_batch_mtx};
                if (m_HSBatchSize != 1) {
                    ATH_MSG_INFO("Reading next batch in event "
                                 << Gaudi::Hive::currentContext().evt() << ", slot "
                                 << Gaudi::Hive::currentContext().slot());
                }
                auto start_time = std::chrono::system_clock::now();
                m_cache[batch] = std::move(m_empty_caches.front());
                m_cache_used[batch].reserve(m_MBBatchSize.value());
                for (int i = 0; i < m_MBBatchSize.value(); ++i) {
                    m_cache_used[batch].emplace_back(std::make_unique<std::atomic_bool>(false));
                }
                m_empty_caches.pop_front();
                // Remember old store to reset later
                auto* old_store = m_activeStoreSvc->activeStore();
                for (auto&& sg : *m_cache[batch]) {
                    if (sg->proxies().size() != 0) {
                        // Not cleared therefore not used -- don't reload
                        continue;
                    }
                    // Change active store
                    m_activeStoreSvc->setStore(sg.get());
                    SG::CurrentEventStore::Push reader_sg_ces(sg.get());
                    // Read next event
                    ATH_CHECK(sg->clearStore(true));
                    if (!(m_bkgEventSelector->next(*m_bkg_evt_sel_ctx)).isSuccess()) {
                        ATH_MSG_FATAL("Ran out of minbias events");
                        return StatusCode::FAILURE;
                    }
                    IOpaqueAddress* addr = nullptr;
                    if (!m_bkgEventSelector->createAddress(*m_bkg_evt_sel_ctx, addr).isSuccess()) {
                        ATH_MSG_WARNING("Failed to create address. No more events?");
                        return StatusCode::FAILURE;
                    }
                    if (addr == nullptr) {
                        ATH_MSG_WARNING("createAddress returned nullptr. No more events?");
                        return StatusCode::FAILURE;
                    }
                    ATH_CHECK(sg->recordAddress(addr));
                    ATH_CHECK(sg->loadEventProxies());
                    // Read data now if desired
                    for (auto* proxy_ptr : sg->proxies()) {
                        if (!proxy_ptr->isValid()) {
                            // Get this warning on every event
                            // ATH_MSG_WARNING("Invalid proxy");
                            continue;
                        }

                        if (!m_onDemandMB) {
                            // Sort of a const_cast, then ->accessData()
                            sg->proxy_exact(proxy_ptr->sgkey())->accessData();
                        }
                    }
                }
                // Reset active store
                m_activeStoreSvc->setStore(old_store);
                if (m_HSBatchSize != 1) {
                    ATH_MSG_INFO(fmt::format("Reading {} events took {:%OMm %OSs}",
                                             m_cache[batch]->size(),
                                             std::chrono::system_clock::now() - start_time));
                }
                m_empty_caches_mtx.unlock();
                m_last_loaded_batch.exchange(batch);
                return StatusCode::SUCCESS;
            }
            else {
                // Unlock mutex if we got the lock but all caches were empty
                m_empty_caches_mtx.unlock();
            }
        }
        // Wait  100ms then try again
        std::this_thread::sleep_for(100ms);
    }
    return StatusCode::SUCCESS;
}

StoreGateSvc* BatchedMinbiasSvc::getMinbias(std::uint64_t hs_id, std::uint64_t mb_id) {
    int batch = event_to_batch(hs_id);
    m_cache_used[batch].at(mb_id)->store(true);
    return m_cache[batch]->at(mb_id).get();
}

StatusCode BatchedMinbiasSvc::endHardScatter(std::uint64_t hs_id) {
    using namespace std::chrono_literals;
    int batch = event_to_batch(hs_id);
    int uses = m_batch_use_count[batch]->fetch_add(1) + 1;

    // If we're done with every event in the batch, clear the stores and return them
    if (uses == m_actualNHSEventsPerBatch.value().at(batch)) {
        std::unique_ptr temp = std::move(m_cache[batch]);
        std::vector used = std::move(m_cache_used[batch]);
        m_cache.erase(batch);
        m_cache_used.erase(batch);
        std::size_t i = 0;
        for (auto&& sg : *temp) {
            if (used.at(i)) {
                ATH_CHECK(sg->clearStore(true));
            }
        }
        while (m_last_unloaded_batch < (batch - 1)) {
            std::this_thread::sleep_for(50ms);
        }
        std::lock_guard lg{m_empty_caches_mtx};
        m_empty_caches.emplace_back(std::move(temp));
        m_last_unloaded_batch.store(batch);
    }
    return StatusCode::SUCCESS;
}
