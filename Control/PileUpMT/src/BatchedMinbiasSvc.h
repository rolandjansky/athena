/* -*- C++ -*- */
/*
  Copyright (C) 2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTCODE_LOWPTMINBIASSVC
#define TESTCODE_LOWPTMINBIASSVC

#include "PileUpMT/IMinbiasSvc.h"

#include "AthenaBaseComps/AthService.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include <atomic>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class BatchedMinbiasSvc : public extends<AthService, IMinbiasSvc> {
  public:
    /// Constructor
    BatchedMinbiasSvc(const std::string& name, ISvcLocator* svc);
    /// Destructor
    ~BatchedMinbiasSvc();

    /// AthService initialize
    StatusCode initialize() override final;

    StatusCode beginHardScatter(std::uint64_t hs_id) override;
    StoreGateSvc* getMinbias(std::uint64_t hs_id, std::uint64_t mb_id) override;
    StatusCode endHardScatter(std::uint64_t hs_id) override;
    inline const std::vector<std::uint64_t>& indexList() override { return m_idx_list; }

  private:
    using SGHandle = ServiceHandle<StoreGateSvc>;
    using SGHandleArray = std::vector<SGHandle>;
    Gaudi::Property<bool> m_onDemandMB{this, "OnDemandMB", false,
                                        "Should minbias event contents be read on demand"};
    Gaudi::Property<int> m_MBBatchSize{this, "MBBatchSize", 10000,
                                         "Number of low pT minbias events to load per batch"};
    Gaudi::Property<int> m_NSimultaneousBatches{this, "NSimultaneousBatches", 1,
                                                  "Max number of batches to load simultaneously"};
    Gaudi::Property<int> m_HSBatchSize{
          this, "HSBatchSize", 1, "Number of HS events per batch (aka max reuse factor)"};
    Gaudi::Property<std::vector<int>> m_actualNHSEventsPerBatch{
          this,
          "actualNHSEventsPerBatch",
          {},
          "Dynamic map of actual number of HS events for each batch, in this run."};
    ServiceHandle<IEvtSelector> m_bkgEventSelector{
          this, "BkgEventSelector", {}, "Event selector for minbias events"};
    ServiceHandle<ActiveStoreSvc> m_activeStoreSvc{this, "ActiveStoreSvc", "ActiveStoreSvc",
                                                   "ActiveStoreSvc"};

    IEvtSelector::Context* m_bkg_evt_sel_ctx;

    std::vector<std::uint64_t> m_idx_list;
    std::map<int, std::unique_ptr<SGHandleArray>> m_cache;
    std::map<int, std::vector<std::unique_ptr<std::atomic_bool>>> m_cache_used;
    std::map<int, std::mutex> m_cache_mtxs; // protects m_cache entries
    // prevents attempting to read multiple batches at once
    std::mutex m_reading_batch_mtx;
    std::deque<std::unique_ptr<SGHandleArray>> m_empty_caches;
    std::mutex m_empty_caches_mtx; // protects m_empty_caches
    std::vector<std::unique_ptr<std::atomic_int>> m_batch_use_count;
    std::atomic_int m_last_loaded_batch;
    std::atomic_int m_last_unloaded_batch;
    int event_to_batch(std::uint64_t hs_id);
};

#endif // TESTCODE_LOWPTMINBIASSVC
