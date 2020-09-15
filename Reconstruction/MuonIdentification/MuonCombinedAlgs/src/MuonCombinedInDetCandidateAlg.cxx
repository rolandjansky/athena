/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedInDetCandidateAlg.h"

#include "MuonLayerEvent/MuonSystemExtension.h"
#include "xAODTruth/TruthParticleContainer.h"

using namespace MuonCombined;

MuonCombinedInDetCandidateAlg::MuonCombinedInDetCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator), m_doSiliconForwardMuons(false)
{
    declareProperty("TrackParticleLocation", m_indetTrackParticleLocation = {"InDetTrackParticles"});
    declareProperty("ForwardParticleLocation", m_indetForwardTrackParticleLocation = "InDetForwardTrackParticles");
    declareProperty("InDetCandidateLocation", m_candidateCollectionName = "InDetCandidates");
    declareProperty("DoSiliconAssocForwardMuons", m_doSiliconForwardMuons = false);
    declareProperty("ExtensionPtThreshold", m_extThreshold = 2500);
}

StatusCode
MuonCombinedInDetCandidateAlg::initialize()
{
    ATH_CHECK(m_trackSelector.retrieve());
    ATH_CHECK(m_muonSystemExtensionTool.retrieve());
    ATH_CHECK(m_indetTrackParticleLocation.initialize());
    ATH_CHECK(m_indetForwardTrackParticleLocation.initialize());
    ATH_CHECK(m_candidateCollectionName.initialize());

    ATH_CHECK(m_forwardTrackSelector.retrieve(DisableTool{!m_doSiliconForwardMuons}));

    return StatusCode::SUCCESS;
}

StatusCode
MuonCombinedInDetCandidateAlg::execute(const EventContext& ctx) const
{
    auto collection = std::make_unique<InDetCandidateCollection>(SG::OWN_ELEMENTS);


    for (auto location : m_indetTrackParticleLocation) {
        if (create(ctx, m_trackSelector, location, collection).isFailure()) {
            ATH_MSG_FATAL("Could not create InDetCandidateCollection");
            return StatusCode::FAILURE;
        }
    }
    if (m_doSiliconForwardMuons) {
        if (create(ctx, m_forwardTrackSelector, m_indetForwardTrackParticleLocation, collection, true).isFailure()) {
            ATH_MSG_FATAL("Could not create InDetForwardCandidateCollection");
            return StatusCode::FAILURE;
        }
    }
    SG::WriteHandle<InDetCandidateCollection> indetCandidateCollection(m_candidateCollectionName, ctx);
    ATH_CHECK(indetCandidateCollection.record(std::move(collection)));

    return StatusCode::SUCCESS;
}

StatusCode
MuonCombinedInDetCandidateAlg::create(const EventContext&                                    ctx,
                                      const ToolHandle<Trk::ITrackSelectorTool>&             currentTrackSelector,
                                      const SG::ReadHandleKey<xAOD::TrackParticleContainer>& location,
                                      std::unique_ptr<InDetCandidateCollection>&             collection,
                                      bool flagCandidateAsSiAssociate) const
{
    SG::ReadHandle<xAOD::TrackParticleContainer> indetTrackParticles(location, ctx);
    if (!indetTrackParticles.isValid()) {
        ATH_MSG_ERROR("Could not read " << location);
        return StatusCode::FAILURE;
    }
    InDetCandidateCollection* tempCandidates = new InDetCandidateCollection(SG::VIEW_ELEMENTS);
    create(currentTrackSelector, *indetTrackParticles, *tempCandidates, flagCandidateAsSiAssociate);
    collection->insert(collection->end(), tempCandidates->begin(), tempCandidates->end());
    delete tempCandidates;
    return StatusCode::SUCCESS;
}

void
MuonCombinedInDetCandidateAlg::create(const ToolHandle<Trk::ITrackSelectorTool>& currentTrackSelector,
                                      const xAOD::TrackParticleContainer&        indetTrackParticles,
                                      InDetCandidateCollection& outputContainer, bool flagCandidateAsSiAssociated) const
{

    ATH_MSG_DEBUG("Producing InDetCandidates for " << indetTrackParticles.size());
    unsigned int ntracks    = 0;
    int          trackIndex = -1;

    for (auto* tp : indetTrackParticles) {
        ++trackIndex;
        if (!isValidTrackParticle(currentTrackSelector, tp)) continue;

        ElementLink<xAOD::TrackParticleContainer> link(indetTrackParticles, trackIndex);
        if (!link.isValid()) {
            ATH_MSG_WARNING("Bad element link ");
        } else {
            if (*link != tp) {
                ATH_MSG_WARNING("Dereferenced Link not equal to TrackParticle, skipping track ");
                continue;
            }
        }
        link.toPersistent();
        printTrackParticleInfo(tp, "Creating");

        if (msgLvl(MSG::VERBOSE) && tp->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
            ElementLink<xAOD::TruthParticleContainer> truthLink =
                tp->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
            if (truthLink.isValid()) {
                ATH_MSG_VERBOSE("  Truth particle: pdgId "
                                << (*truthLink)->pdgId() << " type " << tp->auxdata<int>("truthType") << " origin "
                                << tp->auxdata<int>("truthOrigin") << " pt " << (*truthLink)->pt() << " eta "
                                << (*truthLink)->eta() << " phi " << (*truthLink)->phi());
            }
        }
        InDetCandidate* candidate = new InDetCandidate(link);
        if (flagCandidateAsSiAssociated)
            candidate->setSiliconAssociated(true);  // Si-associated candidates don't need these
        else if (tp->pt() > m_extThreshold) {       // MuGirl only operates on ID tracks with pt at least this high
            const Muon::MuonSystemExtension* muonSystemExtension = 0;
            m_muonSystemExtensionTool->muonSystemExtension(candidate->indetTrackParticle(), muonSystemExtension);
            candidate->setExtension(muonSystemExtension);
        }
        ++ntracks;
        outputContainer.push_back(candidate);
    }
    ATH_MSG_DEBUG("InDetCandidates selected " << ntracks);
}

bool
MuonCombinedInDetCandidateAlg::isValidTrackParticle(const ToolHandle<Trk::ITrackSelectorTool>& currentTrackSelector,
                                                    const xAOD::TrackParticle* const           tp) const
{
    if (!tp->perigeeParameters().covariance()) {
        ATH_MSG_WARNING("InDet TrackParticle without perigee! ");
        return false;
    }

    if (!currentTrackSelector->decision(*tp)) {
        if (msgLvl(MSG::VERBOSE) && tp->pt() > 5000.) printTrackParticleInfo(tp, "Discarding");
        return false;
    }

    return true;
}

void
MuonCombinedInDetCandidateAlg::printTrackParticleInfo(const xAOD::TrackParticle* const tp,
                                                      const std::string&               what) const
{
    ATH_MSG_DEBUG(
        what << " InDet TrackParticle: pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi() << " Pixel "
             << getCount(*tp, xAOD::numberOfInnermostPixelLayerHits) + getCount(*tp, xAOD::numberOfPixelHits) << " SCT "
             << getCount(*tp, xAOD::numberOfSCTHits) << " TRT " << getCount(*tp, xAOD::numberOfTRTHits));
}

int
MuonCombinedInDetCandidateAlg::getCount(const xAOD::TrackParticle& tp, xAOD::SummaryType type) const
{
    uint8_t val;
    if (!tp.summaryValue(val, type)) return 0;
    return static_cast<int>(val);
}
