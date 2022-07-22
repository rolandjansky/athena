/*
  Copyright (C) 2022 CERN for the benefit of the ATLAS collaboration
*/

// PileUpMT includes
#include "PileUpMTAlg.h"

#include "AthenaKernel/RNGWrapper.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"
#include "PileUpTools/PileUpHashHelper.h"
#include "PileUpTools/PileUpMisc.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoAuxContainer.h"
#include "xAODEventInfo/EventInfoContainer.h"

#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandomEngine.h"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <boost/core/demangle.hpp>
#include <range/v3/all.hpp>

#include <array>
#include <chrono>
#include <tuple>

using SubEvent = xAOD::EventInfo::SubEvent;
namespace rv = ranges::views;
// namespace ra = ranges::actions;

inline std::string CLIDToString(const CLID& clid) {
    return boost::core::demangle(CLIDRegistry::CLIDToTypeinfo(clid)->name());
}

namespace {
class HepRandomEngine_URBG {
  public:
    /// Adapts a HepRandomEngine to satisfy UniformRandomBitGenerator
    using result_type = unsigned int;
    static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    HepRandomEngine_URBG(CLHEP::HepRandomEngine* rng) : m_rng(rng) {}
    result_type operator()() const { return static_cast<result_type>(*m_rng); }

  private:
    CLHEP::HepRandomEngine* m_rng;
};
} // anonymous namespace

PileUpMTAlg::PileUpMTAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator) {}

PileUpMTAlg::~PileUpMTAlg() {}

StatusCode PileUpMTAlg::get_ei(StoreGateSvc& sg, std::unique_ptr<const xAOD::EventInfo>& ei_,
                               bool pileup) const {
    const xAOD::EventInfo* ei = nullptr;
    ei = sg.tryConstRetrieve<xAOD::EventInfo>();
    if (ei != nullptr) {
        ei_.reset(ei);
        return StatusCode::SUCCESS;
    }
    const ::EventInfo* ei2 = nullptr;
    ei2 = sg.tryConstRetrieve<::EventInfo>();
    if (ei2 == nullptr) {
        // Just in case
        ATH_MSG_INFO("Got null ::EventInfo from " << sg.name());
        return StatusCode::FAILURE;
    }
    xAOD::EventInfo* newEi = new xAOD::EventInfo();
    xAOD::EventAuxInfo* eiAux = new xAOD::EventAuxInfo();
    newEi->setStore(eiAux);
    if (pileup) {
        ATH_CHECK(m_xAODEICnvTool->convert(ei2, newEi, true));
    }
    else {
        ATH_MSG_INFO("Converting");
        ATH_CHECK(m_xAODEICnvTool->convert(ei2, newEi, false));
    }
    newEi->setEvtStore(&sg);
    ei_.reset(newEi);
    return StatusCode::SUCCESS;
}

StatusCode PileUpMTAlg::add_subevt(const std::vector<std::uint32_t>& bcid,
                                   SG::WriteHandle<xAOD::EventInfo>& overlaidEvt,
                                   SG::WriteHandle<xAOD::EventInfoContainer>& puCont,
                                   ServiceHandle<IMinbiasSvc>& mbSvc,
                                   xAOD::EventInfo::PileUpType puType, int bc, long long hs_id,
                                   unsigned long subevt_id) {
    // Keep the code to add and process a subevent in one place
    const unsigned int bc_idx = bc - m_earliestDeltaBC;
    StoreGateSvc* sg = mbSvc->getMinbias(hs_id, subevt_id);
    std::unique_ptr<const xAOD::EventInfo> ei;
    ATH_CHECK(get_ei(*sg, ei, true));
    xAOD::EventInfo mb_to_modify(*ei);
    mb_to_modify.setBCID(bcid[bc_idx]);
    try {
        addSubEvent(overlaidEvt.ptr(), &mb_to_modify, bc * m_BCSpacing, puType, puCont.ptr(),
                    m_evtInfoContKey.key());
    }
    catch (const std::exception& e) {
        ATH_MSG_ERROR("Caught exception adding subevent: " << e.what());
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

StatusCode PileUpMTAlg::initialize() {
    ATH_MSG_DEBUG("Initializing " << name() << "...");
    if (m_fracLowPt != 0) {
        ATH_CHECK(m_lowptMBSvc.retrieve());
    }
    if (m_fracHighPt != 0) {
        ATH_CHECK(m_highptMBSvc.retrieve());
    }
    if (m_numCavern != 0) {
        ATH_CHECK(m_cavernMBSvc.retrieve());
    }
    if (m_numBeamGas != 0) {
        ATH_CHECK(m_beamgasMBSvc.retrieve());
    }
    if (m_numBeamHalo != 0) {
        ATH_CHECK(m_beamhaloMBSvc.retrieve());
    }
    ATH_CHECK(m_beamInt.retrieve());
    ATH_CHECK(m_beamLumi.retrieve());
    ATH_CHECK(m_rngSvc.retrieve());
    ATH_CHECK(m_xAODEICnvTool.retrieve());
    ATH_CHECK(m_puTools.retrieve());

    for (const ToolHandle<IPileUpTool>& tool : m_puTools) {
        const IProperty* tool_p = dynamic_cast<const IProperty*>(tool.get());
        std::vector<std::string>
              prop_names = !tool_p
                                 ? std::vector{std::string("NOT IProperty")}
                                 : tool_p->getProperties()
                                         | rv::transform([](const Gaudi::Details::PropertyBase* p) {
                                               return fmt::format("{} = {}", p->name(),
                                                                  p->toString());
                                           })
                                         | ranges::to<std::vector>;
        ATH_MSG_INFO("TOOL INFO for "
                     << tool.typeAndName()
                     << fmt::format(" PROPERTIES [{}]", fmt::join(prop_names, ", ")));
    }

    // Figure out actual earliest and latest delta bunch crossing
    bool found_min = false;
    bool found_max = false;
    for (int bc = m_earliestDeltaBC; bc <= m_latestDeltaBC; ++bc) {
        int bc_idx = bc * m_BCSpacing;
        if (!found_min) {
            // First BC any tool will process is the first BC we need to consider
            if (ranges::accumulate(
                      m_puTools | rv::transform([bc_idx](const ToolHandle<IPileUpTool>& tool) {
                          return tool->toProcess(bc_idx);
                      }),
                      false, std::logical_or{})) {
                found_min = true;
                m_earliestDeltaBC.set(bc);
            }
        }
        else if (!found_max) {
            // First BC (after minimum is found) no tool will process is the one after the
            // last we need to consider
            if (ranges::accumulate(
                      m_puTools | rv::transform([bc_idx](const ToolHandle<IPileUpTool>& tool) {
                          return !(tool->toProcess(bc_idx));
                      }),
                      true, std::logical_and{})) {
                found_max = true;
                m_latestDeltaBC.set(bc - 1);
            }
        }
    }
    ATH_MSG_INFO("Considering BCs from " << m_earliestDeltaBC << " to " << m_latestDeltaBC);
    m_evtInfoContKey = fmt::format("{}Container", m_evtInfoKey.key());
    ATH_CHECK(m_evtInfoKey.initialize());
    ATH_CHECK(m_evtInfoContKey.initialize());
    return StatusCode::SUCCESS;
}

StatusCode PileUpMTAlg::finalize() {
    ATH_MSG_DEBUG("Finalizing " << name() << "...");
    //
    // Things that happen once at the end of the event loop go here
    //

    return StatusCode::SUCCESS;
}

StatusCode PileUpMTAlg::execute() {
    using PUType = xAOD::EventInfo::PileUpType;
    ATH_MSG_DEBUG("Executing " << name() << "...");
    // Gaudi::Hive::setCurrentContext(ctx);
    const EventContext& ctx = Gaudi::Hive::currentContext();
    const long long hs_id = ctx.evt();
    ATH_CHECK(evtStore().retrieve());
    if (m_fracLowPt != 0) {
        ATH_CHECK(m_lowptMBSvc->beginHardScatter(hs_id));
    }
    if (m_fracHighPt != 0) {
        ATH_CHECK(m_highptMBSvc->beginHardScatter(hs_id));
    }
    if (m_numCavern != 0) {
        ATH_CHECK(m_cavernMBSvc->beginHardScatter(hs_id));
    }
    if (m_numBeamHalo != 0) {
        ATH_CHECK(m_beamhaloMBSvc->beginHardScatter(hs_id));
    }
    if (m_numBeamGas != 0) {
        ATH_CHECK(m_beamgasMBSvc->beginHardScatter(hs_id));
    }
    setFilterPassed(false); // optional: start with algorithm not passed

    // Code based on PileUpEventLoopMgr and PileUpToolsAlg (trying to extract the core
    // merging code)
    // Read hard scatter
    std::unique_ptr<const xAOD::EventInfo> hsEvt = nullptr;
    ATH_CHECK(get_ei(*evtStore(), hsEvt));
    ATH_MSG_INFO(hs_id << " READ EVT " << hsEvt->eventNumber());

    // Setup overlaid event
    SG::WriteHandle<xAOD::EventInfo> overlaidEvt(m_evtInfoKey, ctx);
    ATH_CHECK(overlaidEvt.record(std::make_unique<xAOD::EventInfo>(),
                                 std::make_unique<xAOD::EventAuxInfo>()));
    *overlaidEvt = *hsEvt; // copy in hard scatter
    overlaidEvt->clearSubEvents();

    // Pileup container
    SG::WriteHandle<xAOD::EventInfoContainer> puCont(m_evtInfoContKey, ctx);
    ATH_CHECK(puCont.record(std::make_unique<xAOD::EventInfoContainer>(),
                            std::make_unique<xAOD::EventInfoAuxContainer>()));

    // Get crossing number
    m_beamInt->selectT0();
    overlaidEvt->setBCID(m_beamInt->getCurrentT0BunchCrossing());

    // Set simulation bit
    overlaidEvt->setEventTypeBitmask(hsEvt->eventTypeBitmask() | xAOD::EventInfo::IS_SIMULATION);

    // Set properties
    bool sf_updated = false;
    float lumi_sf = m_beamLumi->scaleFactor(hsEvt->runNumber(), hsEvt->lumiBlock(), sf_updated);
    float cur_avg_mu = lumi_sf * m_avgMu;
    overlaidEvt->setAverageInteractionsPerCrossing(cur_avg_mu);
    overlaidEvt->setActualInteractionsPerCrossing(m_beamInt->normFactor(0) * cur_avg_mu);
    ATH_MSG_INFO("m_avgMu = " << m_avgMu << ", sf = " << lumi_sf << " ∴ mu = " << cur_avg_mu);

    // Copy subevents
    if (!hsEvt->subEvents().empty()) {
        for (const SubEvent& se : hsEvt->subEvents()) {
            addSubEvent(overlaidEvt.ptr(), se, puCont.ptr(), m_evtInfoContKey.key());
        }
    }
    else {
        // if no subevents, add original event
        addSubEvent(overlaidEvt.ptr(), hsEvt.get(), 0, xAOD::EventInfo::Signal, puCont.ptr(),
                    m_evtInfoContKey.key());
    }

    // Get PRNG
    ATHRNG::RNGWrapper* rngWrapper = m_rngSvc->getEngine(this);
    rngWrapper->setSeed(name(), ctx);
    CLHEP::HepRandomEngine* prng = *rngWrapper;

    // Add pileup subevents
    // First determine which events to use
    std::uint32_t central_bcid = overlaidEvt->bcid();
    std::vector<std::uint32_t> bcid{};
    std::vector<float> mu{};
    bcid.reserve(m_latestDeltaBC - m_earliestDeltaBC + 1);
    mu.reserve(m_latestDeltaBC - m_earliestDeltaBC + 1);

    // Doesn't really matter if these get setup but not used (if a certain type of background isn't
    // present)
    // Low pT
    std::vector<int> n_low_pt{};
    std::vector<std::uint64_t> low_pt_mbs{};
    n_low_pt.reserve(m_latestDeltaBC - m_earliestDeltaBC + 1);

    // High pT
    std::vector<int> n_high_pt{};
    std::vector<std::uint64_t> high_pt_mbs{};
    n_high_pt.reserve(m_latestDeltaBC - m_earliestDeltaBC + 1);

    // Cavern
    std::vector<std::uint64_t> cavern_mbs{};

    // Beam Halo
    std::vector<int> n_beam_halo{};
    std::vector<std::uint64_t> beam_halo_mbs{};
    n_beam_halo.reserve(m_latestDeltaBC - m_earliestDeltaBC + 1);

    // Beam Gas
    std::vector<int> n_beam_gas{};
    std::vector<std::uint64_t> beam_gas_mbs{};
    n_beam_gas.reserve(m_latestDeltaBC - m_earliestDeltaBC + 1);

    for (int bc = m_earliestDeltaBC; bc <= m_latestDeltaBC; ++bc) {
        unsigned int bc_idx = bc - m_earliestDeltaBC;
        bcid.push_back(get_BCID(bc, central_bcid));
        float this_beamInt_norm = m_beamInt->normFactor(bc);
        mu.push_back(this_beamInt_norm * cur_avg_mu);
        // The fmaxes here prevent the poisson distribution getting λ = 0
        // Low pT
        n_low_pt.push_back(CLHEP::RandPoisson::shoot(
              prng, std::fmax(1e-10, m_fracLowPt * std::fabs(mu[bc_idx]))));
        // High pT
        n_high_pt.push_back(CLHEP::RandPoisson::shoot(
              prng, std::fmax(1e-10, m_fracHighPt * std::fabs(mu[bc_idx]))));
        // Cavern -- No Poisson distribution here
        // Beam halo -- use m_beamInt but not m_beamLumi
        n_beam_halo.push_back(CLHEP::RandPoisson::shoot(
              prng, std::fmax(1e-10, std::fabs(this_beamInt_norm * m_numBeamHalo))));
        // Beam gas -- use m_beamInt but not m_beamLumi
        n_beam_gas.push_back(CLHEP::RandPoisson::shoot(
              prng, std::fmax(1e-10, std::fabs(this_beamInt_norm * m_numBeamGas))));
    }

    if (m_fracLowPt != 0) {
        const std::size_t total_n_low_pt = ranges::accumulate(n_low_pt, 0);
        if (total_n_low_pt > m_lowptMBSvc->indexList().size()) {
            ATH_MSG_ERROR("Need " << total_n_low_pt << " low pt but we only loaded "
                                  << m_lowptMBSvc->indexList().size() << " per batch");
        }
        low_pt_mbs.reserve(total_n_low_pt);
        ranges::sample(m_lowptMBSvc->indexList(), ranges::back_inserter(low_pt_mbs), total_n_low_pt,
                       HepRandomEngine_URBG(prng));
    }

    if (m_fracHighPt != 0) {
        const std::size_t total_n_high_pt = ranges::accumulate(n_high_pt, 0);
        if (total_n_high_pt > m_highptMBSvc->indexList().size()) {
            ATH_MSG_ERROR("Need " << total_n_high_pt << " high pt but we only loaded "
                                  << m_highptMBSvc->indexList().size() << " per batch");
        }
        high_pt_mbs.reserve(total_n_high_pt);
        ranges::sample(m_highptMBSvc->indexList(), ranges::back_inserter(high_pt_mbs),
                       total_n_high_pt, HepRandomEngine_URBG(prng));
    }

    if (m_numCavern != 0) {
        const std::size_t total_n_cavern = (m_latestDeltaBC - m_earliestDeltaBC + 1) * m_numCavern;
        if (total_n_cavern > m_cavernMBSvc->indexList().size()) {
            ATH_MSG_ERROR("Need " << total_n_cavern << " cavern but we only loaded "
                                  << m_cavernMBSvc->indexList().size() << " per batch");
        }
        cavern_mbs.reserve(total_n_cavern);
        ranges::sample(m_cavernMBSvc->indexList(), ranges::back_inserter(cavern_mbs),
                       total_n_cavern, HepRandomEngine_URBG(prng));
    }

    if (m_numBeamHalo != 0) {
        const std::size_t total_n_beam_halo = ranges::accumulate(n_beam_halo, 0);
        if (total_n_beam_halo > m_beamhaloMBSvc->indexList().size()) {
            ATH_MSG_ERROR("Need " << total_n_beam_halo << " beam halo but we only loaded "
                                  << m_beamhaloMBSvc->indexList().size() << " per batch");
        }
        beam_halo_mbs.reserve(total_n_beam_halo);
        ranges::sample(m_beamhaloMBSvc->indexList(), ranges::back_inserter(beam_halo_mbs),
                       total_n_beam_halo, HepRandomEngine_URBG(prng));
    }

    if (m_numBeamGas != 0) {
        const std::size_t total_n_beam_gas = ranges::accumulate(n_beam_gas, 0);
        if (total_n_beam_gas > m_beamgasMBSvc->indexList().size()) {
            ATH_MSG_ERROR("Need " << total_n_beam_gas << " beam gas but we only loaded "
                                  << m_beamgasMBSvc->indexList().size() << " per batch");
        }
        beam_gas_mbs.reserve(total_n_beam_gas);
        ranges::sample(m_beamgasMBSvc->indexList(), ranges::back_inserter(beam_gas_mbs),
                       total_n_beam_gas, HepRandomEngine_URBG(prng));
    }

    // Setup tools
    // Account for existing subevents
    std::map<int, int> existing_subevt_bc_counts{};
    for (const auto& se : overlaidEvt->subEvents()) {
        existing_subevt_bc_counts[se.time() / m_BCSpacing]++;
    }
    for (auto&& tool : m_puTools) {
        // First figure out how many events we need to run on
        unsigned int n_evts = 0;
        for (int bc = m_earliestDeltaBC; bc <= m_latestDeltaBC; ++bc) {
            if (tool->toProcess(bc * m_BCSpacing)) {
                std::size_t bc_idx = bc - m_earliestDeltaBC;
                n_evts += existing_subevt_bc_counts[bc];
                n_evts += n_low_pt[bc_idx];
                n_evts += n_high_pt[bc_idx];
                n_evts += m_numCavern;
                n_evts += n_beam_halo[bc_idx];
                n_evts += n_beam_gas[bc_idx];
            }
        }
        ATH_MSG_DEBUG(tool.name() << " will handle " << n_evts << " events");
        // Setup the tool
        tool->resetFilter();
        ATH_CHECK(tool->prepareEvent(ctx, n_evts));
    }

    // Now add the events
    // Background types that aren't present are "automatically" skipped in the inner loops
    auto low_pt_iter = low_pt_mbs.begin();
    auto high_pt_iter = high_pt_mbs.begin();
    auto cavern_iter = cavern_mbs.begin();
    auto beam_halo_iter = beam_halo_mbs.begin();
    auto beam_gas_iter = beam_gas_mbs.begin();
    auto now = std::chrono::high_resolution_clock::now();
    for (int bc = m_earliestDeltaBC; bc <= m_latestDeltaBC; ++bc) {
        for (int i = 0; i < n_low_pt[bc - m_earliestDeltaBC]; ++i) {
            if (low_pt_iter >= low_pt_mbs.end()) {
                ATH_MSG_WARNING("Need more than " << low_pt_mbs.size() << " low pT events");
                break;
            }
            ATH_CHECK_RECOVERABLE(add_subevt(bcid, overlaidEvt, puCont, m_lowptMBSvc,
                                             PUType::MinimumBias, bc, hs_id, *low_pt_iter));
            ++low_pt_iter;
        }
        for (int i = 0; i < n_high_pt[bc - m_earliestDeltaBC]; ++i) {
            if (high_pt_iter >= high_pt_mbs.end()) {
                ATH_MSG_WARNING("Need more than " << high_pt_mbs.size() << " high pT events");
                break;
            }
            ATH_CHECK(add_subevt(bcid, overlaidEvt, puCont, m_highptMBSvc,
                                 PUType::HighPtMinimumBias, bc, hs_id, *high_pt_iter));
            ++high_pt_iter;
        }
        for (int i = 0; i < m_numCavern; ++i) {
            if (cavern_iter >= cavern_mbs.end()) {
                ATH_MSG_WARNING("Need more than " << cavern_mbs.size() << " cavern events");
                break;
            }
            ATH_CHECK(add_subevt(bcid, overlaidEvt, puCont, m_cavernMBSvc, PUType::Cavern, bc,
                                 hs_id, *cavern_iter));
            ++cavern_iter;
        }
        for (int i = 0; i < n_beam_halo[bc - m_earliestDeltaBC]; ++i) {
            if (beam_halo_iter >= beam_halo_mbs.end()) {
                ATH_MSG_WARNING("Need more than " << beam_halo_mbs.size() << " beam halo events");
                break;
            }
            ATH_CHECK(add_subevt(bcid, overlaidEvt, puCont, m_beamhaloMBSvc, PUType::HaloGas, bc,
                                 hs_id, *beam_halo_iter));
            ++beam_halo_iter;
        }
        for (int i = 0; i < n_beam_gas[bc - m_earliestDeltaBC]; ++i) {
            if (beam_gas_iter >= beam_gas_mbs.end()) {
                ATH_MSG_WARNING("Need more than " << beam_gas_mbs.size() << " beam gas events");
                break;
            }
            ATH_CHECK(add_subevt(bcid, overlaidEvt, puCont, m_beamgasMBSvc, PUType::HaloGas, bc,
                                 hs_id, *beam_gas_iter));
            ++beam_gas_iter;
        }
    }

    // Sort so we can go 1 BC at a time
    ranges::stable_sort(const_cast<std::vector<SubEvent>&>(overlaidEvt->subEvents()),
                        ranges::less{}, &SubEvent::time);
    auto subevt_bc_intervals = rv::group_by(overlaidEvt->subEvents(),
                                            [](const SubEvent& lhs, const SubEvent& rhs) {
                                                return lhs.time() == rhs.time();
                                            })
                               | ranges::to<std::vector>;
    for (auto&& tool : m_puTools) {
        for (const auto& bc_interval : subevt_bc_intervals) {
            const SubEventIterator start_evt = bc_interval.begin();
            const SubEventIterator end_evt = bc_interval.end();
            if (!tool->toProcess(start_evt->time())) {
                continue;
            }
            try {
                ATH_CHECK(tool->processBunchXing(start_evt->time(), start_evt, end_evt));
            }
            catch (const std::exception& e) {
                ATH_MSG_ERROR("Caught exception processing subevent: " << e.what() << ", TOOL: "
                                                                       << tool.name());
                return StatusCode::FAILURE;
            }
        }
        try {
            // Merge event
            ATH_CHECK(tool->mergeEvent(ctx));
        }
        catch (const std::exception& e) {
            ATH_MSG_ERROR("Caught exception merging event: " << e.what()
                                                             << ", TOOL: " << tool.name());
        }
        // Propagate filter result
        if (!tool->filterPassed()) {
            setFilterPassed(false);
        }
    }
    ATH_MSG_INFO(fmt::format("***** Took {:%OMm %OSs} to process all subevents",
                             std::chrono::high_resolution_clock::now() - now));
    //
    // Save hash (direct copy from PileUpEventLoopMgr)
    PileUpHashHelper pileUpHashHelper;
    pileUpHashHelper.addToHashSource(overlaidEvt.cptr());
    ATH_MSG_VERBOSE("Pile-up hash source:" << pileUpHashHelper.hashSource());

    // Calculate and set hash
    uuid_t pileUpHash;
    pileUpHashHelper.calculateHash(pileUpHash);
    overlaidEvt->setPileUpMixtureID(PileUpHashHelper::uuidToPileUpMixtureId(pileUpHash));
    ATH_MSG_DEBUG("PileUpMixtureID = " << overlaidEvt->pileUpMixtureID());

    setFilterPassed(true); // if got here, assume that means algorithm passed
    if (m_fracLowPt != 0) {
        ATH_CHECK(m_lowptMBSvc->endHardScatter(hs_id));
    }
    if (m_fracHighPt != 0) {
        ATH_CHECK(m_highptMBSvc->endHardScatter(hs_id));
    }
    if (m_numCavern != 0) {
        ATH_CHECK(m_cavernMBSvc->endHardScatter(hs_id));
    }
    if (m_numBeamHalo != 0) {
        ATH_CHECK(m_beamhaloMBSvc->endHardScatter(hs_id));
    }
    if (m_numBeamGas != 0) {
        ATH_CHECK(m_beamgasMBSvc->endHardScatter(hs_id));
    }
    return StatusCode::SUCCESS;
}
