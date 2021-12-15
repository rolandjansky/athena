/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
// MuonCreatorTool
//  Creates xAOD::Muon objects from muon candidates
//
//////////////////////////////////////////////////////////////////////////////

#include "MuonCreatorTool.h"

#include "AthContainers/ConstDataVector.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "FourMomUtils/P4Helpers.h"
#include "MuidEvent/FieldIntegral.h"
#include "MuonCombinedEvent/CaloTag.h"
#include "MuonCombinedEvent/CombinedFitTag.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuGirlLowBetaTag.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedEvent/SegmentTag.h"
#include "MuonCombinedEvent/StacoTag.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonSegment/MuonSegment.h"
#include "SortInDetCandidates.h"
#include "StoreGate/ReadCondHandle.h"
#include "TrackToCalo/CaloCellCollector.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "muonEvent/CaloEnergy.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"

namespace {
    static const SG::AuxElement::Accessor<int> acc_nUnspoiledCscHits("nUnspoiledCscHits");
    static const SG::AuxElement::Accessor<float> acc_MuonSpectrometerPt("MuonSpectrometerPt");
    static const SG::AuxElement::Accessor<float> acc_InnerDetectorPt("InnerDetectorPt");
    static const SG::AuxElement::Accessor<unsigned int> acc_numEnergyLossPerTrack("numEnergyLossPerTrack");

    static const SG::AuxElement::Accessor<float> acc_ET_Core("ET_Core");
    static const SG::AuxElement::Accessor<float> acc_ET_EMCore("ET_EMCore");
    static const SG::AuxElement::Accessor<float> acc_ET_TileCore("ET_TileCore");
    static const SG::AuxElement::Accessor<float> acc_ET_HECCore("ET_HECCore");

    static const SG::AuxElement::Accessor<float> acc_deltaphi_1("deltaphi_1");
    static const SG::AuxElement::Accessor<float> acc_deltatheta_1("deltatheta_1");
    static const SG::AuxElement::Accessor<float> acc_sigmadeltaphi_1("sigmadeltaphi_1");
    static const SG::AuxElement::Accessor<float> acc_sigmadeltatheta_1("sigmadeltatheta_1");

    static const SG::AuxElement::Accessor<float> acc_deltaphi_0("deltaphi_0");
    static const SG::AuxElement::Accessor<float> acc_deltatheta_0("deltatheta_0");
    static const SG::AuxElement::Accessor<float> acc_sigmadeltaphi_0("sigmadeltaphi_0");
    static const SG::AuxElement::Accessor<float> acc_sigmadeltatheta_0("sigmadeltatheta_0");

}  // namespace
namespace MuonCombined {

    MuonCreatorTool::MuonCreatorTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonCreatorTool>(this);
    }

    StatusCode MuonCreatorTool::initialize() {
        if (m_buildStauContainer) ATH_MSG_DEBUG(" building Stau container ");

        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_muonPrinter.retrieve());
        ATH_CHECK(m_caloExtTool.retrieve());
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_CHECK(m_particleCreator.retrieve());
        ATH_CHECK(m_ambiguityProcessor.retrieve());
        ATH_CHECK(m_muonDressingTool.retrieve());
        ATH_CHECK(m_muonSegmentConverterTool.retrieve());
        ATH_CHECK(m_caloMgrKey.initialize());

        if (!m_trackSegmentAssociationTool.empty())
            ATH_CHECK(m_trackSegmentAssociationTool.retrieve());
        else
            m_trackSegmentAssociationTool.disable();
        ATH_CHECK(m_trackQuery.retrieve());
        if (!m_momentumBalanceTool.empty())
            ATH_CHECK(m_momentumBalanceTool.retrieve());
        else
            m_momentumBalanceTool.disable();
        if (!m_scatteringAngleTool.empty())
            ATH_CHECK(m_scatteringAngleTool.retrieve());
        else
            m_scatteringAngleTool.disable();
        if (!m_selectorTool.empty())
            ATH_CHECK(m_selectorTool.retrieve());
        else
            m_selectorTool.disable();
        if (!m_meanMDTdADCTool.empty())
            ATH_CHECK(m_meanMDTdADCTool.retrieve());
        else
            m_meanMDTdADCTool.disable();

        ATH_CHECK(m_caloNoiseKey.initialize(SG::AllowEmpty));

        ATH_MSG_INFO("ET_Core calculation: doNoiseCut, sigma - " << !m_caloNoiseKey.empty() << " " << m_sigmaCaloNoiseCut);

        if (!m_doSA) {
            ATH_CHECK(m_caloMaterialProvider.retrieve());
            ATH_CHECK(m_propagator.retrieve());
        } else {
            m_caloMaterialProvider.disable();
            m_propagator.disable();
        }
        if (m_buildStauContainer) m_runComissioningChain = false;
        ATH_CHECK(m_cellContainerName.initialize(m_useCaloCells));
        ATH_CHECK(m_trackSummaryTool.retrieve());

        return StatusCode::SUCCESS;
    }
    void MuonCreatorTool::create(const EventContext& ctx, const MuonCandidateCollection* muonCandidates,
                                 const std::vector<const InDetCandidateToTagMap*>& tagMaps,
                                 OutputData& outputData) const {
        create(ctx, muonCandidates, tagMaps, outputData, false);

        if (m_runComissioningChain) { create(ctx, muonCandidates, tagMaps, outputData, true); }
    }
    void MuonCreatorTool::create(const EventContext& ctx, const MuonCandidateCollection* muonCandidates,
                                 const std::vector<const InDetCandidateToTagMap*>& tagMaps,
                                 OutputData& outputData, bool select_comissioning) const {
       
        // Create containers for resolved candidates (always of type VIEW_ELEMENTS)
        InDetCandidateTagsMap resolvedInDetCandidates;
        // std::vector<const MuonCombined::InDetCandidate*> resolvedInDetCandidates;
        std::vector<const MuonCombined::MuonCandidate*> resolvedMuonCandidates;

        // Resolve Overlap
        if (!m_buildStauContainer)
            resolveOverlaps(ctx, muonCandidates, tagMaps, resolvedInDetCandidates, resolvedMuonCandidates,
                            select_comissioning);
        else
            selectStaus(resolvedInDetCandidates, tagMaps);

        unsigned int numIdCan = resolvedInDetCandidates.size();
        unsigned int numMuCan = muonCandidates ? muonCandidates->size() : 0;
        ATH_MSG_DEBUG("Creating xAOD::Muons from: " << numIdCan << " indet candidates and " << numMuCan << " muon candidates ");

        if (!m_buildStauContainer && muonCandidates)
            ATH_MSG_DEBUG("MuonCandidates  : overlap removal " << muonCandidates->size() << " in, " << resolvedMuonCandidates.size()
                                                               << " out");

        // Create a container for resolved candidates (always of type VIEW_ELEMENTS)
        for (InDetCandidateTags& can : resolvedInDetCandidates) {
            ATH_MSG_DEBUG("New InDetCandidate");
            xAOD::Muon* muon = create(ctx, can, outputData);
            if (!muon) {
                ATH_MSG_DEBUG("no muon found");
            } else {
                ATH_MSG_DEBUG("muon found");
                if (select_comissioning) { muon->addAllAuthor(xAOD::Muon::Author::Commissioning); }
                if (!muon->primaryTrackParticleLink().isValid()) {
                    ATH_MSG_ERROR("This muon has no valid primaryTrackParticleLink! Author=" << muon->author());
                }
            }
            ATH_MSG_DEBUG("Creation of Muon from InDetCandidates done");
        }
        if (!m_requireIDTracks) {  // only build SA muons if ID tracks are not required
            for (const MuonCombined::MuonCandidate* can : resolvedMuonCandidates) {
                ATH_MSG_DEBUG("New MuonCandidate");
                xAOD::Muon* muon = create(ctx, *can, outputData);
                if (muon && select_comissioning) { muon->addAllAuthor(xAOD::Muon::Author::Commissioning); }
                ATH_MSG_DEBUG("Creation of Muon from MuonCandidates done");
            }
        }

        // Add alignment effects on tracks to ME and CB tracks
        if (m_fillAlignmentEffectsOnTrack) {
            if (outputData.combinedTrackParticleContainer) addAlignmentEffectsOnTrack(outputData.combinedTrackParticleContainer);
            if (outputData.extrapolatedTrackParticleContainer) addAlignmentEffectsOnTrack(outputData.extrapolatedTrackParticleContainer);
            if (outputData.msOnlyExtrapolatedTrackParticleContainer)
                addAlignmentEffectsOnTrack(outputData.msOnlyExtrapolatedTrackParticleContainer);
            for (xAOD::Muon* mu : *outputData.muonContainer) {
                if (mu->primaryTrackParticle() == mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)) continue;  // no CT or ST muons
                if (mu->primaryTrackParticle() == mu->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle))
                    continue;  // no SA muons w/o ME tracks
                const xAOD::TrackParticle* ptp = mu->primaryTrackParticle();
                uint8_t prec = 0;  // all precision layers
                mu->summaryValue(prec, xAOD::numberOfPrecisionLayers);
                int nTotPrec = (int)prec, nGoodPrec = 0, nBadPrec = 0, nBadBar = 0, nBadEnd = 0, nBadSmall = 0, nBadLarge = 0, nGoodBar = 0,
                    nGoodEnd = 0, nGoodLarge = 0, nGoodSmall = 0;

                static const SG::AuxElement::Accessor<std::vector<std::vector<unsigned int>>> acc_alignEffectChId("alignEffectChId");
                static const SG::AuxElement::Accessor<std::vector<float>> acc_alligSigmaDeltaTrans("alignEffectSigmaDeltaTrans");

                const std::vector<std::vector<unsigned int>>& chIds = acc_alignEffectChId(*ptp);
                const std::vector<float>& alignEffSDT = acc_alligSigmaDeltaTrans(*ptp);
                std::map<Muon::MuonStationIndex::ChIndex, int> chamberQual;  // 1=good, 2=bad; for choosing large/small
                for (unsigned int i = 0; i < chIds.size(); i++) {
                    for (unsigned int j = 0; j < chIds[i].size(); j++) {
                        Muon::MuonStationIndex::ChIndex currInd = (Muon::MuonStationIndex::ChIndex)chIds[i][j];
                        if (alignEffSDT[i] >= 0.5) {
                            if ((chamberQual.count(currInd) && chIds[i].size() > 1) || !chamberQual.count(currInd)) {
                                // either we haven't seen this chamber before, or we have but now
                                // we're in a sub-vector that's not just this chamber
                                chamberQual[currInd] = 2;
                            }
                        } else {
                            if ((chamberQual.count(currInd) && chIds[i].size() > 1) || !chamberQual.count(currInd)) {
                                // either we haven't seen this chamber before, or we have but now
                                // we're in a sub-vector that's not just this chamber
                                chamberQual[currInd] = 1;
                            }
                        }
                    }
                }
                for (std::map<Muon::MuonStationIndex::ChIndex, int>::iterator it = chamberQual.begin(); it != chamberQual.end(); ++it) {
                    int chnum = (unsigned int)((*it).first);
                    if ((*it).second == 2) {
                        nBadPrec++;
                        if (chnum < 7) {
                            nBadBar++;
                            if (chnum % 2 == 0)
                                nBadSmall++;
                            else
                                nBadLarge++;
                        } else {
                            nBadEnd++;
                            if (chnum % 2 == 0)
                                nBadLarge++;
                            else
                                nBadSmall++;
                        }
                    } else {
                        if (chnum < 7) {
                            nGoodBar++;
                            if (chnum % 2 == 0)
                                nGoodSmall++;
                            else
                                nGoodLarge++;
                        } else {
                            nGoodEnd++;
                            if (chnum % 2 == 0)
                                nGoodLarge++;
                            else
                                nGoodSmall++;
                        }
                    }
                }
                int isSmall = 0, isEnd = 0;
                bool countHits = false;
                if ((nTotPrec - nBadPrec) > nBadPrec) {
                    nGoodPrec = nTotPrec - nBadPrec;
                    if (nGoodEnd > nGoodBar)
                        isEnd = 1;  // arbitrarily setting it to barrel if they're equal for
                                    // now
                    if (nGoodSmall > nGoodLarge) isSmall = 1;
                    if (nGoodSmall == nGoodLarge) countHits = true;
                } else {
                    nGoodPrec = nBadPrec;
                    if ((nTotPrec - nBadPrec) == nBadPrec) {  // if equal, set to whichever
                                                              // has the smaller error, right?
                        if (nGoodEnd > nGoodBar) isEnd = 1;
                        if (nGoodSmall > nGoodLarge) isSmall = 1;
                        if (nGoodSmall == nGoodLarge) countHits = true;
                    } else {
                        if (nBadEnd > nBadBar) isEnd = 1;
                        if (nBadSmall > nBadLarge) isSmall = 1;
                        if (nBadSmall == nBadLarge) countHits = true;
                    }
                }
                // as we arbitrarily declare this to be a barrel track if there are equal
                // numbers of good barrel and endcap chambers, we need not worry about that
                // situation
                if (countHits) {  // decide large-small by counting hits
                    uint8_t sumval = 0;
                    int nSmallHits = 0, nLargeHits = 0;
                    mu->summaryValue(sumval, xAOD::innerSmallHits);
                    nSmallHits += (int)sumval;
                    sumval = 0;
                    mu->summaryValue(sumval, xAOD::middleSmallHits);
                    nSmallHits += (int)sumval;
                    sumval = 0;
                    mu->summaryValue(sumval, xAOD::outerSmallHits);
                    nSmallHits += (int)sumval;
                    sumval = 0;
                    mu->summaryValue(sumval, xAOD::extendedSmallHits);
                    nSmallHits += (int)sumval;
                    sumval = 0;
                    mu->summaryValue(sumval, xAOD::innerLargeHits);
                    nLargeHits += (int)sumval;
                    sumval = 0;
                    mu->summaryValue(sumval, xAOD::middleLargeHits);
                    nLargeHits += (int)sumval;
                    sumval = 0;
                    mu->summaryValue(sumval, xAOD::outerLargeHits);
                    nLargeHits += (int)sumval;
                    sumval = 0;
                    mu->summaryValue(sumval, xAOD::extendedLargeHits);
                    nLargeHits += (int)sumval;
                    if (nSmallHits > nLargeHits) isSmall = 1;
                }
                mu->setSummaryValue(nGoodPrec, xAOD::numberOfGoodPrecisionLayers);
                mu->setSummaryValue(isSmall, xAOD::isSmallGoodSectors);
                mu->setSummaryValue(isEnd, xAOD::isEndcapGoodLayers);
            }
        }

        if (msgLvl(MSG::DEBUG) || m_printSummary) {
            ATH_MSG_INFO("Printing muon container:");
            ATH_MSG_INFO(m_muonPrinter->print(*outputData.muonContainer));
            ATH_MSG_INFO("Done");
        }
        if (msgLvl(MSG::VERBOSE) && outputData.clusterContainer) {
            ATH_MSG_VERBOSE("Associated clusters : " << outputData.clusterContainer->size());
        }
    }

    xAOD::Muon* MuonCreatorTool::create(const EventContext& ctx, const MuonCandidate& candidate, OutputData& outputData) const {
        // skip all muons without extrapolated track
        if (!candidate.extrapolatedTrack()) {
            ATH_MSG_DEBUG(
                "MuonCreatorTool::create(...) No extrapolated track - "
                "aborting. Will not create Muon.");
            return nullptr;  // Do we really want to do this?
        }

        // Create the xAOD object:
        xAOD::Muon* muon = new xAOD::Muon();
        outputData.muonContainer->push_back(muon);
        decorateDummyValues(ctx, *muon, outputData);

        muon->setAuthor(xAOD::Muon::MuidSA);
        muon->setMuonType(xAOD::Muon::MuonStandAlone);
        muon->addAllAuthor(xAOD::Muon::MuidSA);

        // create candidate from SA muon only
        addMuonCandidate(ctx, candidate, *muon, outputData);

        if (!muon->extrapolatedMuonSpectrometerTrackParticleLink().isValid()) {
            ATH_MSG_DEBUG("Creation of track particle for SA muon failed, removing it");
            outputData.muonContainer->pop_back();
            return nullptr;
        }

        if (!dressMuon(*muon, outputData.xaodSegmentContainer)) {
            ATH_MSG_WARNING("Failed to dress muon");
            outputData.muonContainer->pop_back();
            return nullptr;
        }

        // make sure we can extrapolate the track back through the calo, otherwise
        // it's not a muon shouldn't above requirement that an extrapolated track exist
        // do this, though? difference between different ways of extrapolating probably
        // needs to be investigated the muons that are rejected by this all seem to be
        // useless low-pT SA muons with 2 precision layers that would never make it
        // into analyses, though
        std::unique_ptr<Trk::CaloExtension> caloExtension =
            m_caloExtTool->caloExtension(ctx, *muon->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle));
        if (!caloExtension) {
            ATH_MSG_DEBUG("failed to get a calo extension for this SA muon, discard it");
            outputData.muonContainer->pop_back();
            return nullptr;
        }
        if (caloExtension->caloLayerIntersections().empty()) {
            ATH_MSG_DEBUG("failed to retrieve any calo layers for this SA muon, discard it");
            outputData.muonContainer->pop_back();
            return nullptr;
        }

        // check if there is a cluster container, if yes collect the cells around the
        // muon and fill Etcore variables for muon
        if (m_useCaloCells) collectCells(ctx, *muon, outputData.clusterContainer, caloExtension.get());

        return muon;
    }

    void MuonCreatorTool::decorateDummyValues(const EventContext& ctx, xAOD::Muon& muon, OutputData& outputData) const {
        // Set variables to zero by calling the functions with null pointers.
        addCaloTag(muon, nullptr);
        addCombinedFit(ctx, muon, nullptr, outputData);
        addStatisticalCombination(ctx, muon, nullptr, nullptr, outputData);
        addMuGirl(muon, nullptr, outputData);
        addSegmentTag(muon, nullptr, outputData);

        /// Unspoiled CSC hits
        acc_nUnspoiledCscHits(muon) = 0;
        acc_MuonSpectrometerPt(muon) = -1;
        acc_InnerDetectorPt(muon) = -1;

        acc_ET_Core(muon) = 0;
        acc_ET_EMCore(muon) = 0;
        acc_ET_TileCore(muon) = 0;
        acc_ET_HECCore(muon) = 0;

        acc_deltaphi_1(muon) = -999;
        acc_deltatheta_1(muon) = -999;
        acc_sigmadeltaphi_1(muon) = -999;
        acc_sigmadeltatheta_1(muon) = -999;

        acc_deltaphi_0(muon) = -999;
        acc_deltatheta_0(muon) = -999;
        acc_sigmadeltaphi_0(muon) = -999;
        acc_sigmadeltatheta_0(muon) = -999;

        fillEnergyLossFromTrack(muon, nullptr);
    }
    xAOD::Muon* MuonCreatorTool::create(const EventContext& ctx, InDetCandidateTags& candidate, OutputData& outputData) const {
        // no tags, no muon
        if (candidate.second.empty()) {
            ATH_MSG_DEBUG("MuonCreatorTool::create(...) - InDetCandidate with empty combinedDataTags. Aborting. Will not create Muon.");
            return nullptr;
        }       
        std::vector<const TagBase*>& tags = candidate.second;
        if (tags.size() == 1 && !m_buildStauContainer) {
            const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tags[0]);
            if (muGirlLowBetaTag) {
                ATH_MSG_DEBUG("Track has only a MuGirlLowBetaTag but Staus are not being built, so will not create muon");
                return nullptr;
            }
        }

        // Create the xAOD object:
        xAOD::Muon* muon = new xAOD::Muon();
        outputData.muonContainer->push_back(muon);
        std::vector<ElementLink<xAOD::MuonSegmentContainer>> segments;
        muon->setMuonSegmentLinks(segments);

        // now we need to sort the tags to get the best muon

        // set the link to the ID track particle
        muon->setTrackParticleLink(xAOD::Muon::InnerDetectorTrackParticle, candidate.first->indetTrackParticleLink());
        ATH_MSG_DEBUG("Adding InDet Track: pt " << candidate.first->indetTrackParticle().pt() << " eta "
                                                << candidate.first->indetTrackParticle().eta() << " phi "
                                                << candidate.first->indetTrackParticle().phi());

        ATH_MSG_DEBUG("creating Muon with " << tags.size() << " tags ");
        // loop over the tags

        decorateDummyValues(ctx, *muon, outputData);
        bool first = true;
        for (const MuonCombined::TagBase* tag : tags) {
            ATH_MSG_DEBUG("Handling tag: type " << tag->type());

            // staus
            if (m_buildStauContainer) {
                const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tag);

                if (muGirlLowBetaTag) {
                    ATH_MSG_DEBUG("MuonCreatorTool MuGirlLowBetaTag ");

                    muon->setAuthor(tag->author());
                    muon->setMuonType(tag->type());

                    if (tag->type() == xAOD::Muon::Combined) {
                        ATH_MSG_DEBUG("MuonCreatorTool MuGirlLowBetaTag combined");

                        // Create the xAOD object:
                        if (outputData.slowMuonContainer) {
                            xAOD::SlowMuon* slowMuon = new xAOD::SlowMuon();
                            outputData.slowMuonContainer->push_back(slowMuon);

                            addMuGirlLowBeta(*muon, muGirlLowBetaTag, slowMuon,
                                             outputData);  // CHECK to see what variables are created here.

                            ATH_MSG_DEBUG("slowMuon muonContainer size " << outputData.muonContainer->size());
                            ElementLink<xAOD::MuonContainer> muonLink(*outputData.muonContainer, outputData.muonContainer->size() - 1);
                            if (slowMuon && muonLink.isValid()) {
                                ATH_MSG_DEBUG("slowMuon muonLink valid");
                                slowMuon->setMuonLink(muonLink);
                            }
                        }
                    }
                }
            } else {
                // Don't want staus in muon branch
                const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tag);
                if (muGirlLowBetaTag) continue;

                // set author info
                if (first) {
                    ATH_MSG_DEBUG("MuonCreatorTool first muon tag: author=" << tag->author() << "  type=" << tag->type());
                    muon->setAuthor(tag->author());
                    muon->setMuonType(tag->type());
                    // Overrride type if InDet track is SiAssociated.
                    if (candidate.first->isSiliconAssociated()) { muon->setMuonType(xAOD::Muon::SiliconAssociatedForwardMuon); }
                    first = false;
                }

                muon->addAllAuthor(tag->author());

                // this is not too elegant, maybe rethink implementation
                xAOD::Muon::MuonType type = tag->type();
                if (type == xAOD::Muon::Combined) {
                    // work out type of tag
                    const CombinedFitTag* cbFitTag = dynamic_cast<const CombinedFitTag*>(tag);
                    const StacoTag* stacoTag = dynamic_cast<const StacoTag*>(tag);
                    const MuGirlTag* muGirlTag = dynamic_cast<const MuGirlTag*>(tag);

                    addCombinedFit(ctx, *muon, cbFitTag, outputData);
                    addMuGirl(*muon, muGirlTag, outputData);
                    addStatisticalCombination(ctx, *muon, candidate.first, stacoTag, outputData);
                    if (!(cbFitTag || stacoTag || muGirlTag)) { ATH_MSG_WARNING("Unknown combined tag "); }

                } else if (type == xAOD::Muon::SegmentTagged) {
                    const SegmentTag* segTag = dynamic_cast<const SegmentTag*>(tag);
                    const MuGirlTag* muGirlTag = dynamic_cast<const MuGirlTag*>(tag);

                    addSegmentTag(*muon, segTag, outputData);
                    addMuGirl(*muon, muGirlTag, outputData);

                    if (!(segTag || muGirlTag)) { ATH_MSG_WARNING("Unknown segment-tagged tag "); }
                } else if (type == xAOD::Muon::CaloTagged) {
                    const CaloTag* caloTag = dynamic_cast<const CaloTag*>(tag);
                    addCaloTag(*muon, caloTag);
                    if (!caloTag) { ATH_MSG_WARNING("Unknown calo tag type "); }
                } else {
                    ATH_MSG_WARNING("Unknown tag type. Type= " + std::to_string(type));
                }
            }
        }  // m_buildStauContainer

        if (!dressMuon(*muon, outputData.xaodSegmentContainer)) {
            ATH_MSG_WARNING("Failed to dress muon");
            outputData.muonContainer->pop_back();
            // if we are dealing with staus, also need to remove the slowMuon
            if (m_buildStauContainer) outputData.slowMuonContainer->pop_back();
            return nullptr;
        }

        // If eLoss is not already available then build it
        float eLoss = -1;
        bool haveEloss = muon->parameter(eLoss, xAOD::Muon::EnergyLoss);
        if (!haveEloss || eLoss == 0) {
            ATH_MSG_DEBUG("Adding Energy Loss to muon" << std::endl << m_muonPrinter->print(*muon));
            addEnergyLossToMuon(*muon);
        }

        // check if there is a cluster container, if yes collect the cells around the
        // muon and fill Etcore variables for muon
        if (m_useCaloCells) collectCells(ctx, *muon, outputData.clusterContainer, candidate.first->getCaloExtension());

        ATH_MSG_DEBUG("Done creating muon with " << acc_nUnspoiledCscHits(*muon) << " unspoiled csc hits");

        return muon;
    }

    void MuonCreatorTool::addStatisticalCombination(const EventContext& ctx, xAOD::Muon& muon, const InDetCandidate* candidate,
                                                    const StacoTag* tag, OutputData& outputData) const {
        static const SG::AuxElement::Accessor<float> acc_d0("d0_staco");
        static const SG::AuxElement::Accessor<float> acc_z0("z0_staco");
        static const SG::AuxElement::Accessor<float> acc_phi0("phi0_staco");
        static const SG::AuxElement::Accessor<float> acc_theta("theta_staco");
        static const SG::AuxElement::Accessor<float> acc_qOverP("qOverP_staco");
        static const SG::AuxElement::Accessor<float> acc_qOverPerr("qOverPErr_staco");

        if (!tag) {
            // init variables if necessary.
            acc_d0(muon) = -999;
            acc_z0(muon) = -999;
            acc_phi0(muon) = -999;
            acc_theta(muon) = -999;
            acc_qOverP(muon) = -999;
            acc_qOverPerr(muon) = -999.;
            return;
        }

        ATH_MSG_DEBUG("Adding Staco Muon  " << tag->author() << " type " << tag->type());

        if (!muon.combinedTrackParticleLink().isValid()) {
            // create primary track particle
            // get summary
            const Trk::Track* idTrack = candidate->indetTrackParticle().track();
            const Trk::Track* msTrack = tag->muonCandidate().extrapolatedTrack() ? tag->muonCandidate().extrapolatedTrack()
                                                                                 : &tag->muonCandidate().muonSpectrometerTrack();

            const Trk::TrackSummary* idSummary = idTrack ? idTrack->trackSummary() : nullptr;
            const Trk::TrackSummary* msSummary = msTrack->trackSummary();

            Trk::TrackSummary summary;
            if (idSummary) summary += *idSummary;
            if (msSummary) summary += *msSummary;

            Trk::FitQuality fq(tag->matchChi2(), 5);
            Trk::TrackInfo info(msTrack->info());
            // todo update patrec bit set adding ID values

            if (outputData.combinedTrackParticleContainer) {
                xAOD::TrackParticle* tp = m_particleCreator->createParticle(ctx, &tag->combinedParameters(), &fq, &info, &summary, {}, {},
                                                                            xAOD::muon, outputData.combinedTrackParticleContainer);
                if (!tp) {
                    ATH_MSG_WARNING("Failed to create track particle");
                } else {
                    /// Also add this decoration to maintain data-format consistency to the combined track particles
                    /// (cf. ATLASRECTS-6454)
                    std::vector<float> dummy_cov(15, 0.);
                    tp->setTrackParameterCovarianceMatrix(0, dummy_cov);

                    ElementLink<xAOD::TrackParticleContainer> link(*outputData.combinedTrackParticleContainer,
                                                                   outputData.combinedTrackParticleContainer->size() - 1);
                    if (link.isValid()) {
                        // link.toPersistent();
                        ATH_MSG_DEBUG("Adding statistical combination: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi "
                                                                            << (*link)->phi());
                        muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
                    }
                    // for the purpose of the truth matching, set the track link to point to
                    // the ID track
                    // tp->setTrackLink(candidate.indetTrackParticle().trackLink());
                    tp->setTrackLink(ElementLink<TrackCollection>());
                    std::bitset<xAOD::NumberOfTrackRecoInfo> pattern;
                    pattern.set(xAOD::STACO);
                    tp->setPatternRecognitionInfo(pattern);
                }
            }  // endif outputData.combinedTrackParticleContainer
        }
        // add muon candidate
        addMuonCandidate(ctx, tag->muonCandidate(), muon, outputData);

        // Add inner match chi^2
        muon.setParameter(5, xAOD::Muon::msInnerMatchDOF);
        muon.setParameter(static_cast<float>(tag->matchChi2()), xAOD::Muon::msInnerMatchChi2);

        // STACO parameters added as auxdata
        acc_d0(muon) = tag->combinedParameters().parameters()[Trk::d0];
        acc_z0(muon) = tag->combinedParameters().parameters()[Trk::z0];
        acc_phi0(muon) = tag->combinedParameters().parameters()[Trk::phi0];
        acc_theta(muon) = tag->combinedParameters().parameters()[Trk::theta];
        acc_qOverP(muon) = tag->combinedParameters().parameters()[Trk::qOverP];
        acc_qOverPerr(muon) = Amg::error(*tag->combinedParameters().covariance(), Trk::qOverP);

        ATH_MSG_DEBUG("Done adding Staco Muon  " << tag->author() << " type " << tag->type());
    }

    void MuonCreatorTool::addCombinedFit(const EventContext& ctx, xAOD::Muon& muon, const CombinedFitTag* tag,
                                         OutputData& outputData) const {
        if (!tag) {
            // init variables if necessary.
            return;
        }

        ATH_MSG_DEBUG("Adding Combined fit Muon  " << tag->author() << " type " << tag->type());
        if (!muon.combinedTrackParticleLink().isValid()) {
            // if the combined track particle is part of a container set the link
            if (outputData.combinedTrackParticleContainer) {
                // create element link from the track
                ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink(
                    tag->combinedTrackLink(), *outputData.combinedTrackParticleContainer, outputData.combinedTrackCollection);

                if (link.isValid()) {
                    // link.toPersistent();
                    ATH_MSG_DEBUG("Adding combined fit: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi());
                    muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
                } else
                    ATH_MSG_WARNING("Creating of Combined TrackParticle Link failed");
            }
        }

        // add muon candidate
        addMuonCandidate(ctx, tag->muonCandidate(), muon, outputData, tag->updatedExtrapolatedTrackLink());

        // Add inner match chi^2
        const float inner_chi2 = tag->matchChi2();
        muon.setParameter(tag->matchDoF(), xAOD::Muon::msInnerMatchDOF);
        muon.setParameter(inner_chi2, xAOD::Muon::msInnerMatchChi2);

        ATH_MSG_DEBUG("Done adding Combined Fit Muon  " << tag->author() << " type " << tag->type());
    }

    void MuonCreatorTool::addMuGirlLowBeta(xAOD::Muon& muon, const MuGirlLowBetaTag* tag, xAOD::SlowMuon* slowMuon,
                                           OutputData& outputData) const {
        if (!tag) {
            // init variables if necessary.
            return;
        }

        ATH_MSG_DEBUG("Adding MuGirlLowBeta Muon  " << tag->author() << " type " << tag->type());

        // get stauExtras and write to slowMuon
        const MuGirlNS::StauExtras* stauExtras = tag->getStauExtras();
        if (slowMuon && stauExtras) {
            ATH_MSG_DEBUG("StauSummary beta " << stauExtras->betaAll);
            slowMuon->setBeta(stauExtras->betaAll);
            slowMuon->setBetaT(stauExtras->betaAllt);
            slowMuon->setAnn(stauExtras->ann);
            slowMuon->setNRpcHits(stauExtras->numRpcHitsInSeg);
            slowMuon->setNTileCells(stauExtras->numCaloCells);
            slowMuon->setRpcInfo(stauExtras->rpcBetaAvg, stauExtras->rpcBetaRms, stauExtras->rpcBetaChi2, stauExtras->rpcBetaDof);
            slowMuon->setMdtInfo(stauExtras->mdtBetaAvg, stauExtras->mdtBetaRms, stauExtras->mdtBetaChi2, stauExtras->mdtBetaDof);
            slowMuon->setCaloInfo(stauExtras->caloBetaAvg, stauExtras->caloBetaRms, stauExtras->caloBetaChi2, stauExtras->caloBetaDof);
            std::vector<uint8_t> eTechVec;
            std::vector<unsigned int> idVec;
            std::vector<float> mToFVec;
            std::vector<float> xVec;
            std::vector<float> yVec;
            std::vector<float> zVec;
            std::vector<float> eVec;
            std::vector<float> errorVec;
            std::vector<float> shiftVec;
            std::vector<float> propagationTimeVec;

            for (const auto& hit : stauExtras->hits) {
                eTechVec.push_back(hit.eTech);
                idVec.push_back(hit.id.get_identifier32().get_compact());
                mToFVec.push_back(hit.mToF);
                xVec.push_back(hit.x);
                yVec.push_back(hit.y);
                zVec.push_back(hit.z);
                eVec.push_back(hit.e);
                errorVec.push_back(hit.error);
                shiftVec.push_back(hit.shift);
                propagationTimeVec.push_back(hit.propagationTime);
            }
            slowMuon->auxdata<std::vector<uint8_t>>("hitTechnology") = eTechVec;
            slowMuon->auxdata<std::vector<unsigned int>>("hitIdentifier") = idVec;
            slowMuon->auxdata<std::vector<float>>("hitTOF") = mToFVec;
            slowMuon->auxdata<std::vector<float>>("hitPositionX") = xVec;
            slowMuon->auxdata<std::vector<float>>("hitPositionY") = yVec;
            slowMuon->auxdata<std::vector<float>>("hitPositionZ") = zVec;
            slowMuon->auxdata<std::vector<float>>("hitEnergy") = eVec;
            slowMuon->auxdata<std::vector<float>>("hitError") = errorVec;
            slowMuon->auxdata<std::vector<float>>("hitShift") = shiftVec;
            slowMuon->auxdata<std::vector<float>>("hitPropagationTime") = propagationTimeVec;
        }

        if (!muon.combinedTrackParticleLink().isValid() && tag->combinedTrack()) {
            // if the combined track particle is part of a container set the link
            if (outputData.combinedTrackParticleContainer) {
                // create element link
                ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink(
                    tag->combinedTrackLink(), *outputData.combinedTrackParticleContainer, outputData.combinedTrackCollection);

                if (link.isValid()) {
                    // link.toPersistent();
                    ATH_MSG_DEBUG("Adding MuGirlLowBeta: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi());
                    muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
                } else
                    ATH_MSG_WARNING("Creating of MuGirlLowBeta TrackParticle Link failed");
            }
        }

        if (outputData.xaodSegmentContainer) {
            ATH_MSG_DEBUG("Adding MuGirlLowBeta muonSegmentColection");

            std::vector<ElementLink<xAOD::MuonSegmentContainer>> segments;
            for (const auto& seglink : tag->segments()) {
                ElementLink<xAOD::MuonSegmentContainer> link =
                    createMuonSegmentElementLink(seglink, *outputData.xaodSegmentContainer, outputData.muonSegmentCollection);
                if (link.isValid()) {
                    // link.toPersistent();
                    segments.push_back(link);
                    ATH_MSG_DEBUG("Adding MuGirlLowBeta: xaod::MuonSegment px " << (*link)->px() << " py " << (*link)->py() << " pz "
                                                                                << (*link)->pz());
                } else
                    ATH_MSG_WARNING("Creating of MuGirlLowBeta segment Link failed");
            }
            muon.setMuonSegmentLinks(segments);
        }
    }

    void MuonCreatorTool::addMuGirl(xAOD::Muon& muon, const MuGirlTag* tag, OutputData& outputData) const {
        if (!tag) {
            // init variables if necessary.
            return;
        }

        ATH_MSG_DEBUG("Adding MuGirl Muon  " << tag->author() << " type " << tag->type());

        if (!muon.combinedTrackParticleLink().isValid() && tag->combinedTrack()) {
            // if the combined track particle is part of a container set the link
            if (outputData.combinedTrackParticleContainer) {
                // create element link
                ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink(
                    tag->combinedTrackLink(), *outputData.combinedTrackParticleContainer, outputData.combinedTrackCollection);

                if (link.isValid()) {
                    // link.toPersistent();
                    ATH_MSG_DEBUG("Adding MuGirl: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi());
                    muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
                } else
                    ATH_MSG_WARNING("Creating of MuGirl TrackParticle Link failed");
            }

            if (outputData.extrapolatedTrackParticleContainer && tag->updatedExtrapolatedTrack()) {
                // create element link
                ElementLink<xAOD::TrackParticleContainer> link =
                    createTrackParticleElementLink(tag->updatedExtrapolatedTrackLink(), *outputData.extrapolatedTrackParticleContainer,
                                                   outputData.extrapolatedTrackCollection);

                if (link.isValid()) {
                    ATH_MSG_DEBUG("Adding MuGirl: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi());
                    muon.setTrackParticleLink(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle, link);
                } else
                    ATH_MSG_WARNING("Creating of MuGirl TrackParticle Link failed");
            }

            if (outputData.xaodSegmentContainer) {
                ATH_MSG_DEBUG("Adding MuGirl muonSegmentCollection");

                std::vector<ElementLink<xAOD::MuonSegmentContainer>> segments;
                for (const auto& segLink : tag->segments()) {
                    ElementLink<xAOD::MuonSegmentContainer> link =
                        createMuonSegmentElementLink(segLink, *outputData.xaodSegmentContainer, outputData.muonSegmentCollection);
                    if (link.isValid()) {
                        segments.push_back(link);
                        ATH_MSG_DEBUG("Adding MuGirl: xaod::MuonSegment px " << (*link)->px() << " py " << (*link)->py() << " pz "
                                                                             << (*link)->pz());
                    } else
                        ATH_MSG_WARNING("Creating of MuGirl segment Link failed");
                }
                muon.setMuonSegmentLinks(segments);
            }
        }
        ATH_MSG_DEBUG("Done Adding MuGirl Muon  " << tag->author() << " type " << tag->type());
    }

    void MuonCreatorTool::addSegmentTag(xAOD::Muon& muon, const SegmentTag* tag, OutputData& outputData) const {
        if (!tag) {
            // init variables if necessary.
            muon.setParameter(-1.f, xAOD::Muon::segmentDeltaEta);
            muon.setParameter(-1.f, xAOD::Muon::segmentDeltaPhi);
            muon.setParameter(-1.f, xAOD::Muon::segmentChi2OverDoF);
            return;
        }

        ATH_MSG_DEBUG("Adding Segment Tag Muon  " << tag->author() << " type " << tag->type());

        std::vector<ElementLink<xAOD::MuonSegmentContainer>> segments;
        bool foundseg = false;
        for (const auto& info : tag->segmentsInfo()) {
            if (info.link.isValid()) {
                // this is a bit tricky, as we have here a link to an xAOD segment in the old container
                // but the new container should have the segments in the same order, plus the MuGirl ones tacked on the end
                // so we should be able to just make a new link here
                // note that this only applies to segment-tagged muons, others get their associated segments elsewhere
                if (muon.author() == xAOD::Muon::MuTagIMO) {
                    ElementLink<xAOD::MuonSegmentContainer> seglink(*outputData.xaodSegmentContainer, info.link.index());
                    segments.push_back(seglink);
                }
                if (!foundseg) {  // add parameters for the first segment
                    muon.setParameter(static_cast<float>(info.dtheta), xAOD::Muon::segmentDeltaEta);
                    muon.setParameter(static_cast<float>(info.dphi), xAOD::Muon::segmentDeltaPhi);
                    muon.setParameter(
                        static_cast<float>(info.segment->fitQuality()->chiSquared() / info.segment->fitQuality()->numberDoF()),
                        xAOD::Muon::segmentChi2OverDoF);
                    foundseg = true;
                } else if (muon.author() != xAOD::Muon::MuTagIMO)
                    break;  // for non-segment-tagged muons, we only need to set the above
                            // parameters
            }
        }
        if (muon.author() == xAOD::Muon::MuTagIMO) muon.setMuonSegmentLinks(segments);  // set the associated segments
    }

    void MuonCreatorTool::addCaloTag(xAOD::Muon& mu, const CaloTag* tag) const {
        static const SG::AuxElement::Accessor<float> acc_ElType("CT_EL_Type");  // FIXME - should be uint
        static const SG::AuxElement::Accessor<float> acc_ElLikelihood("CT_ET_LRLikelihood");
        static const SG::AuxElement::Accessor<float> acc_ElFSREnergy("CT_ET_FSRCandidateEnergy");

        if (!tag) {
            // init variables if necessary.

            mu.setParameter(0.f, xAOD::Muon::CaloLRLikelihood);
            mu.setParameter(0.f, xAOD::Muon::CaloMuonScore);
            mu.setParameter(static_cast<int>(0xFF), xAOD::Muon::CaloMuonIDTag);
            if (m_fillExtraELossInfo) {
                // Here we can make sure that we store the extra calotag information -
                // just always add it since this is then unambigious for debugging
                acc_ET_Core(mu) = 0.0;
                acc_ElType(mu) = -999.0;
                acc_ElLikelihood(mu) = -999.0;
                acc_ElFSREnergy(mu) = -999.0;
            }
            return;
        }

        ATH_MSG_DEBUG("Adding Calo Muon with author " << tag->author() << ", type " << tag->type() << ", LHR " << tag->caloLRLikelihood()
                                                      << ", CaloMuonScore " << tag->caloMuonScore());
        mu.setParameter(static_cast<float>(tag->caloLRLikelihood()), xAOD::Muon::CaloLRLikelihood);
        mu.setParameter(static_cast<float>(tag->caloMuonScore()), xAOD::Muon::CaloMuonScore);
        mu.setParameter(static_cast<int>(tag->caloMuonIdTag()), xAOD::Muon::CaloMuonIDTag);

        if (m_fillExtraELossInfo) {
            // Here we can make sure that we store the extra calotag information - just
            // always add it since this is then unambigious for debugging
            acc_ET_Core(mu) = tag->etCore();
            acc_ElType(mu) = tag->energyLossType();
            acc_ElLikelihood(mu) = tag->caloLRLikelihood();
            acc_ElFSREnergy(mu) = tag->fsrCandidateEnergy();
        }
        // FIXME - calo deposits
    }

    ElementLink<xAOD::TrackParticleContainer> MuonCreatorTool::createTrackParticleElementLink(
        const ElementLink<TrackCollection>& trackLink, xAOD::TrackParticleContainer& trackParticleContainer,
        TrackCollection* trackCollection) const {
        ATH_MSG_DEBUG("createTrackParticleElementLink");
        const xAOD::TrackParticle* tp = nullptr;
        if (trackCollection) {
            trackCollection->push_back(new Trk::Track(**trackLink));
            // want to link the track particle to this track
            ElementLink<TrackCollection> link(*trackCollection, trackCollection->size() - 1);
            if (link.isValid())
                tp = m_particleCreator->createParticle(link, &trackParticleContainer, nullptr, xAOD::muon);
            else
                ATH_MSG_WARNING("new Track Collection link invalid");
        }
        if (!tp) {
            // create track particle without a link to the track
            tp = m_particleCreator->createParticle(**trackLink, &trackParticleContainer, nullptr, xAOD::muon);
        }

        if (tp) {
            ElementLink<xAOD::TrackParticleContainer> link(trackParticleContainer, trackParticleContainer.size() - 1);
            return link;
        }
        return ElementLink<xAOD::TrackParticleContainer>();
    }

    ElementLink<xAOD::MuonSegmentContainer> MuonCreatorTool::createMuonSegmentElementLink(
        const ElementLink<Trk::SegmentCollection>& segLink, xAOD::MuonSegmentContainer& xaodSegmentContainer,
        Trk::SegmentCollection* muonSegmentCollection) const {
        const xAOD::MuonSegment* ms = nullptr;
        if (muonSegmentCollection) {
            // if a muon segment collection is provided, duplicate the segment and
            // create a link to that
            const Muon::MuonSegment* oldSeg = dynamic_cast<const Muon::MuonSegment*>(*segLink);
            muonSegmentCollection->push_back(new Muon::MuonSegment(*oldSeg));

            ElementLink<Trk::SegmentCollection> link(*muonSegmentCollection, muonSegmentCollection->size() - 1);
            if (link.isValid()) {
                link.toPersistent();
                ms = m_muonSegmentConverterTool->convert(link, &xaodSegmentContainer);
            } else
                ATH_MSG_WARNING("new Segment Collection link invalid");
        }
        if (!ms) {
            // create muon segments without a link to the original
            const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*>(*segLink);
            ms = m_muonSegmentConverterTool->convert(*seg, &xaodSegmentContainer);
        }
        if (ms) {
            ElementLink<xAOD::MuonSegmentContainer> link(xaodSegmentContainer, xaodSegmentContainer.size() - 1);
            link.toPersistent();
            return link;
        }
        return ElementLink<xAOD::MuonSegmentContainer>();
    }

    void MuonCreatorTool::addMuonCandidate(const EventContext& ctx, const MuonCandidate& candidate, xAOD::Muon& muon,
                                           OutputData& outputData, const ElementLink<TrackCollection>& meLink) const {
        // only set once
        if (muon.muonSpectrometerTrackParticleLink().isValid()) { return; }
        // case where we have a MuGirl muon that is also reconstructed by STACO: don't
        // want to add this track as it is misleading however, we will still keep the
        // MS-only extrapolated track (see below) for debugging purposes
        if (muon.author() != xAOD::Muon::MuGirl)
            muon.setTrackParticleLink(xAOD::Muon::MuonSpectrometerTrackParticle, candidate.muonSpectrometerTrackLink());

        // we need both the container and the extrapolated muon track to add the link
        if (!outputData.extrapolatedTrackParticleContainer || (!candidate.extrapolatedTrack() && !meLink.isValid())) { return; }

        const Trk::Track* extrapolatedTrack = candidate.extrapolatedTrack();

        if (!extrapolatedTrack || !extrapolatedTrack->perigeeParameters()) {
            ATH_MSG_DEBUG("There is no extrapolated track associated to the MuonCandidate.");
            if (muon.author() == xAOD::Muon::MuidCo) {  // this can happen for MuidCo muons, though it's
                                                        // quite rare: in this case just add the ME track
                if (meLink.isValid()) {
                    ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink(
                        meLink, *outputData.extrapolatedTrackParticleContainer, outputData.extrapolatedTrackCollection);
                    if (link.isValid()) {
                        ATH_MSG_DEBUG("Adding standalone fit (refitted): pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi "
                                                                              << (*link)->phi());
                        muon.setTrackParticleLink(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle, link);
                        float fieldInt = m_trackQuery->fieldIntegral(**meLink, ctx).betweenSpectrometerMeasurements();
                        muon.setParameter(fieldInt, xAOD::Muon::spectrometerFieldIntegral);
                        int nunspoiled = (*link)->track()->trackSummary()->get(Trk::numberOfCscUnspoiltEtaHits);
                        acc_nUnspoiledCscHits(muon) = nunspoiled;
                    }
                }
            } else {  // I don't think we should ever get here, but just in case
                if (!extrapolatedTrack) ATH_MSG_WARNING("Track doesn't have extrapolated track. Skipping");
                if (extrapolatedTrack && !extrapolatedTrack->perigeeParameters())
                    ATH_MSG_WARNING(
                        "Track doesn't have perigee parameters on extrapolated "
                        "track. Skipping");
            }
        } else {
            // Now we just add the original extrapolated track itself
            // but not for SA muons, for consistency they will still have
            // extrapolatedTrackParticle
            if (muon.muonType() != xAOD::Muon::MuonStandAlone) {
                if (meLink.isValid()) {                                         // add ME track and MS-only extrapolated track
                    if (outputData.msOnlyExtrapolatedTrackParticleContainer) {  // add un-refitted
                                                                                // extrapolated track
                                                                                // as MS-only
                                                                                // extrapolated track
                        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink(
                            candidate.extrapolatedTrackLink(), *outputData.msOnlyExtrapolatedTrackParticleContainer,
                            outputData.msOnlyExtrapolatedTrackCollection);

                        if (link.isValid()) {
                            ATH_MSG_DEBUG("Adding MS-only extrapolated track: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi "
                                                                                   << (*link)->phi());
                            // link.toPersistent();
                            muon.setTrackParticleLink(xAOD::Muon::MSOnlyExtrapolatedMuonSpectrometerTrackParticle, link);
                        } else
                            ATH_MSG_WARNING("failed to create MS-only extrapolated track particle");
                    }
                    // now add refitted track as ME track
                    ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink(
                        meLink, *outputData.extrapolatedTrackParticleContainer, outputData.extrapolatedTrackCollection);
                    if (link.isValid()) {
                        ATH_MSG_DEBUG("Adding standalone fit (refitted): pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi "
                                                                              << (*link)->phi());
                        muon.setTrackParticleLink(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle, link);
                        float fieldInt = m_trackQuery->fieldIntegral(**meLink, ctx).betweenSpectrometerMeasurements();
                        muon.setParameter(fieldInt, xAOD::Muon::spectrometerFieldIntegral);
                        int nunspoiled = (*link)->track()->trackSummary()->get(Trk::numberOfCscUnspoiltEtaHits);
                        acc_nUnspoiledCscHits(muon) = nunspoiled;
                    }
                } else {  // no refitted track, so add original un-refitted extrapolated
                          // track as ME track
                    if (muon.author() == xAOD::Muon::MuGirl && muon.extrapolatedMuonSpectrometerTrackParticleLink().isValid()) {
                        // MuGirl case: ME track is already set, but now we have the
                        // extrapolated track from the STACO tag add this as the MS-only
                        // extrapolated track instead
                        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink(
                            candidate.extrapolatedTrackLink(), *outputData.msOnlyExtrapolatedTrackParticleContainer,
                            outputData.msOnlyExtrapolatedTrackCollection);

                        if (link.isValid()) {
                            ATH_MSG_DEBUG("Adding MS-only extrapolated track to MuGirl muon: pt "
                                          << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi());
                            // link.toPersistent();
                            muon.setTrackParticleLink(xAOD::Muon::MSOnlyExtrapolatedMuonSpectrometerTrackParticle, link);
                            float fieldInt =
                                m_trackQuery->fieldIntegral(**candidate.extrapolatedTrackLink(), ctx).betweenSpectrometerMeasurements();
                            muon.setParameter(fieldInt, xAOD::Muon::spectrometerFieldIntegral);
                        }
                    } else {
                        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink(
                            candidate.extrapolatedTrackLink(), *outputData.extrapolatedTrackParticleContainer,
                            outputData.extrapolatedTrackCollection);

                        if (link.isValid()) {
                            ATH_MSG_DEBUG("Adding standalone fit (un-refitted): pt " << (*link)->pt() << " eta " << (*link)->eta()
                                                                                     << " phi " << (*link)->phi());
                            // link.toPersistent();
                            muon.setTrackParticleLink(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle, link);
                            float fieldInt =
                                m_trackQuery->fieldIntegral(**candidate.extrapolatedTrackLink(), ctx).betweenSpectrometerMeasurements();
                            muon.setParameter(fieldInt, xAOD::Muon::spectrometerFieldIntegral);
                        }
                    }
                }
            } else {  // SA tracks only get un-refitted track as ME track
                // create element link from the track
                ElementLink<xAOD::TrackParticleContainer> link =
                    createTrackParticleElementLink(candidate.extrapolatedTrackLink(), *outputData.extrapolatedTrackParticleContainer,
                                                   outputData.extrapolatedTrackCollection);

                if (link.isValid()) {
                    ATH_MSG_DEBUG("Adding standalone fit: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi());
                    // link.toPersistent();
                    muon.setTrackParticleLink(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle, link);
                    float fieldInt =
                        m_trackQuery->fieldIntegral(**candidate.extrapolatedTrackLink(), ctx).betweenSpectrometerMeasurements();
                    muon.setParameter(fieldInt, xAOD::Muon::spectrometerFieldIntegral);
                    int nunspoiled = extrapolatedTrack->trackSummary()->get(Trk::numberOfCscUnspoiltEtaHits);
                    acc_nUnspoiledCscHits(muon) = nunspoiled;
                } else {
                    ATH_MSG_WARNING("failed to create ME track particle for SA muon");
                }
            }
        }
    }

    void MuonCreatorTool::selectStaus(InDetCandidateTagsMap& resolvedInDetCandidates,
                                      const std::vector<const InDetCandidateToTagMap*>& tagMaps) const {
        resolvedInDetCandidates.clear();
        for (const InDetCandidateToTagMap* tag_map: tagMaps){
            if (!tag_map) continue;
            for (const auto& combined_tag : *tag_map){
                const TagBase* tag = combined_tag.second.get();
                const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tag);
                if (muGirlLowBetaTag) {
                    resolvedInDetCandidates.emplace_back(combined_tag.first,  std::vector<const TagBase*> {tag});                    
                }
            }
            break;
        }
        // print-out
        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("ID candidates:  " << tagMaps.size() << "  after stau selection " << resolvedInDetCandidates.size());
            for (const InDetCandidateTags& candidate : resolvedInDetCandidates) {
                msg(MSG::DEBUG) << "ID candidate staus:  " << candidate.first->toString() << endmsg;
            }
        }
    }

    void MuonCreatorTool::resolveOverlaps(const EventContext& ctx ,
                                          const MuonCandidateCollection* muonCandidates,
                                          const std::vector<const InDetCandidateToTagMap*>& tagMaps,
                                          InDetCandidateTagsMap& resolvedInDetCandidates,
                                          std::vector<const MuonCombined::MuonCandidate*>& resolvedMuonCandidates,
                                          bool select_comissioning) const {
        
        resolvedMuonCandidates.clear();
        resolvedInDetCandidates.clear();
        
        std::unique_ptr<const TrackCollection> resolvedTracks;
        std::vector<std::unique_ptr<Trk::Track>> garbage_collection;
             
       
        /// Find all InDetCandidate -- Combined tag combinations
        /// Be aware that different InDetCandidateMaps could have different
        /// InDetCandidate objects but the underlying tracks are always the same
        InDetCandidateTagsMap inDetCandidateMap;
        for (const InDetCandidateToTagMap* tag_map : tagMaps) {
            if (!tag_map) continue;
            for (const auto& comb_tag : *tag_map) {
                const TagBase* tag = comb_tag.second.get();
                /// Check whether the author arises from the comissioning chain
                /// The maps are filled in dedicated algorithim. So all tags will 
                /// fail / satisfy this condition
                if (tag->isComissioning() != select_comissioning) break;
                InDetCandidateTagsMap::iterator itr = std::find_if(inDetCandidateMap.begin(),inDetCandidateMap.end(), 
                                        [&comb_tag](const InDetCandidateTags& to_test) {
                                            return (*to_test.first) == (*comb_tag.first);
                                        });
                if (itr != inDetCandidateMap.end()) itr->second.emplace_back(tag);
                else inDetCandidateMap.emplace_back(std::make_pair(comb_tag.first, std::vector<const TagBase*>{tag}));
            }            
        }        
        
        // Each InDetCandidate corresponds to a different ID track.
        // Resolve overlap among InDetCandidates for cases where different
        // ID tracks are tagged by the same MS info (track or segment)
        if (!inDetCandidateMap.empty()) {
            // the muons only found by the calo tagger should always be kept so we can
            // filter them out from the start
            InDetCandidateTagsMap caloMuons;

            // first loop over ID candidates and select all candidates that have a tag
            resolvedInDetCandidates.reserve(inDetCandidateMap.size());
            caloMuons.reserve(inDetCandidateMap.size());            
            for (InDetCandidateTags& comb_tag : inDetCandidateMap) {
                std::stable_sort(comb_tag.second.begin(), comb_tag.second.end(), SortTagBasePtr());
                if (comb_tag.second.size() == 1 && comb_tag.second.front()->type() == xAOD::Muon::CaloTagged) {
                    caloMuons.emplace_back(std::move(comb_tag));
                } else resolvedInDetCandidates.emplace_back(std::move(comb_tag));

            }
            inDetCandidateMap.clear();
            // now sort the selected ID candidates
            std::stable_sort(resolvedInDetCandidates.begin(), resolvedInDetCandidates.end(), SortInDetCandidates());
            if (msgLvl(MSG::DEBUG)) {
                ATH_MSG_DEBUG("Found " << resolvedInDetCandidates.size()<<" inner detector tags in event "<<ctx.eventID().event_number());
                for (const InDetCandidateTags& candidate : resolvedInDetCandidates) {
                    std::stringstream tags;
                    for (const TagBase* tag : candidate.second) tags<<"  "<<tag->toString();
                    ATH_MSG_DEBUG("ID candidate:  " << candidate.first->toString()<<" "<<tags.str());
                }
            }
            
            ConstDataVector<TrackCollection> to_resolve{SG::VIEW_ELEMENTS};

            to_resolve.reserve(resolvedInDetCandidates.size());
            garbage_collection.reserve(resolvedInDetCandidates.size());

            // a dummy track for segment tagged candidates to be used in the overlap
            // check
            std::map<const Trk::Track*, InDetCandidateTags> trackInDetCandLinks;
           
            for (InDetCandidateTags& candidate : resolvedInDetCandidates) {
                // retrieve the primary tag
                const TagBase* primaryTag = candidate.second[0];
             
                // check if a track is available
                if (primaryTag->primaryTrack()) {                    
                    /// Add the track for the ambiguity reprocessing
                    to_resolve.push_back(primaryTag->primaryTrack());
                    // create a track summary for this track
                    trackInDetCandLinks[to_resolve.back()] = std::move(candidate);
                }
                // if not, make a dummy track out of segments, muonTracks takes ownership
                // of the memory
                else {
                    std::vector<const Muon::MuonSegment*> segments = primaryTag->associatedSegments();
                    if (!segments.empty()) {
                        /// Create firs the dummy track
                        garbage_collection.emplace_back(
                            createDummyTrack(ctx, primaryTag->associatedSegments(), *(candidate.first->indetTrackParticle().track())));                        
                        /// Add it to the list piped to ambiguity solving
                        to_resolve.push_back(garbage_collection.back().get());
                        /// Move the candidate into the map to find the resolved tracks
                        trackInDetCandLinks[garbage_collection.back().get()] = std::move(candidate);
                    }
                }
            }
            resolvedInDetCandidates.clear();

            // Resolve ambiguity between muon tracks
           resolvedTracks.reset(m_ambiguityProcessor->process(to_resolve.asDataVector()));
          
            // link back to InDet candidates and fill the resolved container
            for (const Trk::Track* track : *resolvedTracks) {
                std::map<const Trk::Track*, InDetCandidateTags>::iterator trackCandLink = trackInDetCandLinks.find(track);
                if (trackCandLink == trackInDetCandLinks.end()) {
                    ATH_MSG_WARNING("Unable to find internal link between MS track and ID candidate!");
                    continue;
                }
                resolvedInDetCandidates.push_back(std::move(trackCandLink->second));
            }
                 
            // print-out
            if (msgLvl(MSG::VERBOSE)) {
                ATH_MSG_DEBUG("ID candidates after ambiguity solving " << resolvedInDetCandidates.size()
                        <<" trackCandLinks: "<<trackInDetCandLinks.size()
                        <<" to_resolve: "<<to_resolve.size()
                        <<" resolvedTracks: "<<resolvedTracks->size());
                for (const InDetCandidateTags& candidate : resolvedInDetCandidates) {
                    ATH_MSG_DEBUG("ID candidate:  " << candidate.first->toString()<<" "<<candidate.second[0]->toString());             
                }
                ATH_MSG_DEBUG("Calo muons after ambiguity solving: ");
                for (const InDetCandidateTags& candidate : caloMuons) {
                    ATH_MSG_DEBUG("ID candidate:  " << candidate.first->toString()<<" "<<candidate.second[0]->toString());             
                }                
            }
            // add muons only found by calo tagger
            resolvedInDetCandidates.insert(resolvedInDetCandidates.end(), 
                                           caloMuons.begin(), 
                                           caloMuons.end()); 
        }

        

        // MuonCandidateCollection contains all muon tracks (SA extrapolated or not)
        // Resolve overlap with InDetCandidate collection
        if (!muonCandidates) { return; }

        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("Muon candidates:  " << muonCandidates->size());
            for (const MuonCandidate* candidate : *muonCandidates) {
                ATH_MSG_DEBUG("Muon candidate:  " << candidate->toString());
            }
        }

        ConstDataVector<TrackCollection> resolvedTracks2{SG::VIEW_ELEMENTS};
        if (resolvedTracks) { resolvedTracks2.assign(resolvedTracks->begin(), resolvedTracks->end()); }
        // Keep track of the MuonCandidates used by MuidCo
        std::set<const MuonCandidate*> used_candidates;
        for (const InDetCandidateTags& indet_cand : resolvedInDetCandidates) {
            for (const TagBase* tag : indet_cand.second) {
                /// In principle we can include here STACO as well but that is lower ranked as MuidSA 
                if (tag->author() == xAOD::Muon::MuidCo) {
                    const CombinedFitTag* cmb_tag  = dynamic_cast<const CombinedFitTag*>(tag);
                    used_candidates.insert(&cmb_tag->muonCandidate());
                } else if (indet_cand.second.size() == 1 && tag->author() == xAOD::Muon::STACO) {
                    const StacoTag* staco_tag = dynamic_cast<const StacoTag*>(tag);
                    used_candidates.insert(&staco_tag->muonCandidate());
                }
            }
        }

        // add MS tracks to resolvedTrack collection and store a link between tracks
        // and muon candidates
        std::map<const Trk::Track*, const MuonCandidate*> trackMuonCandLinks;
        for (const MuonCandidate* candidate : *muonCandidates) {
            if (candidate->isComissioning() != select_comissioning) continue;
            const Trk::Track* track = candidate->primaryTrack();
            if (used_candidates.count(candidate)) {
                ATH_MSG_DEBUG("Duplicate MS track " << m_printer->print(*track));
                continue;
            }
            used_candidates.insert(candidate);
            resolvedTracks2.push_back(track);  // VIEW_ELEMENTS, pointer only
            trackMuonCandLinks[track] = candidate;
        }

        // solve ambiguity
        resolvedTracks.reset(m_ambiguityProcessor->process(resolvedTracks2.asDataVector()));

        // loop over resolved tracks and fill resolved muon candidates
        for (const Trk::Track* track : *resolvedTracks) {
            auto trackCandLink = trackMuonCandLinks.find(track);
            if (trackCandLink != trackMuonCandLinks.end()) resolvedMuonCandidates.push_back(trackCandLink->second);
        }

        // print-out
        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("Muon candidates:  " << muonCandidates->size() << "  after ambiguity solving "
                                                << resolvedMuonCandidates.size());
            for (const MuonCandidate* candidate : resolvedMuonCandidates) {
                msg(MSG::DEBUG) << "Muon candidate:  " << candidate->toString() << endmsg;
            }
        }        
    }

    std::unique_ptr<Trk::Track> MuonCreatorTool::createDummyTrack(const EventContext& ctx,
                                                  const std::vector<const Muon::MuonSegment*>& segments,
                                                  const Trk::Track& indetTrack) const {
        ATH_MSG_VERBOSE("Creating dummy tracks from segments...");

        Trk::TrackStates trackStateOnSurfaces{};

        for (const Muon::MuonSegment* seg : segments) {
            // create pars for muon and loop over hits
            double momentum{1e8}, charge{0.};
            std::unique_ptr<const Trk::TrackParameters> pars{m_edmHelperSvc->createTrackParameters(*seg, momentum, charge)};
            for (const Trk::MeasurementBase* meas : seg->containedMeasurements()) {
                std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
                typePattern.set(Trk::TrackStateOnSurface::Measurement);
                // TSoS takes ownership
                std::unique_ptr<Trk::TrackParameters> exPars{m_propagator->propagateParameters(
                    ctx, *pars, meas->associatedSurface(), Trk::anyDirection, false, Trk::MagneticFieldProperties(Trk::NoField))};
                if (!exPars) { ATH_MSG_VERBOSE("Could not propagate Track to segment surface"); }
                Trk::TrackStateOnSurface* trackState =
                    new Trk::TrackStateOnSurface(meas->uniqueClone(), std::move(exPars), nullptr, nullptr, typePattern);
                trackStateOnSurfaces.push_back(trackState);
            }  // end segment loop
        }

        Trk::TrackInfo info(Trk::TrackInfo::Unknown, Trk::muon);
        Trk::TrackInfo::TrackPatternRecoInfo author = Trk::TrackInfo::MuTag;
        info.setPatternRecognitionInfo(author);
        std::unique_ptr<Trk::Track> newtrack =
            std::make_unique<Trk::Track>(info, std::move(trackStateOnSurfaces), (indetTrack.fitQuality())->clone());

        // create a track summary for this track
        if (m_trackSummaryTool.isEnabled()) { m_trackSummaryTool->computeAndReplaceTrackSummary(*newtrack, nullptr, false); }

        return newtrack;
    }

    bool MuonCreatorTool::dressMuon(xAOD::Muon& muon, const xAOD::MuonSegmentContainer* segments) const {
        if (muon.primaryTrackParticleLink().isValid()) {
            const xAOD::TrackParticle* primary = muon.primaryTrackParticle();

            // update parameters with primary track particle
            setP4(muon, *primary);
            float qOverP = primary->qOverP();
            if (qOverP != 0.0) {
                muon.setCharge(qOverP / std::abs(qOverP));
                // try/catch didn't work...
            } else {
                ATH_MSG_WARNING(
                    "MuonCreatorTool::dressMuon - trying to set qOverP, but value from "
                    "muon.primaryTrackParticle ["
                    << muon.primaryTrackParticleLink().dataID()
                    << "] is zero. Setting charge=0.0. The eta/phi of the muon is: " << muon.eta() << "/" << muon.phi());
                muon.setCharge(0.0);
            }
        } else {
            ATH_MSG_DEBUG("No primary track particle set, deleting muon");
            return false;
        }

        // add hit summary
        m_muonDressingTool->addMuonHitSummary(muon);

        // calculate scattering significance and momentum balance significance
        if (!m_scatteringAngleTool.empty()) {
            Rec::ScatteringAngleSignificance scatSign = m_scatteringAngleTool->scatteringAngleSignificance(muon);
            float curvatureSignificance = scatSign.curvatureSignificance();
            muon.setParameter(curvatureSignificance, xAOD::Muon::scatteringCurvatureSignificance);
            float neighbourSignificance = scatSign.neighbourSignificance();
            muon.setParameter(neighbourSignificance, xAOD::Muon::scatteringNeighbourSignificance);
            ATH_MSG_VERBOSE("Got curvatureSignificance " << curvatureSignificance << "  and neighbourSignificance "
                                                         << neighbourSignificance);
        }

        if (!m_momentumBalanceTool.empty()) {
            float momentumBalanceSignificance = m_momentumBalanceTool->momentumBalanceSignificance(muon);
            muon.setParameter(momentumBalanceSignificance, xAOD::Muon::momentumBalanceSignificance);
            ATH_MSG_VERBOSE("Got momentumBalanceSignificance " << momentumBalanceSignificance);
        }

        if (!m_meanMDTdADCTool.empty()) {
            float meanDeltaADC = float(m_meanMDTdADCTool->meanMDTdADCFiller(muon));
            muon.setParameter(meanDeltaADC, xAOD::Muon::meanDeltaADCCountsMDT);
            ATH_MSG_VERBOSE("Got meanDeltaADCCountsMDT " << meanDeltaADC);
        }

        if (!m_selectorTool.empty()) {
            acc_MuonSpectrometerPt(muon) = muon.pt();
            acc_InnerDetectorPt(muon) = muon.pt();
            // set id cuts
            m_selectorTool->setPassesIDCuts(muon);
            ATH_MSG_VERBOSE("Setting passesIDCuts " << muon.passesIDCuts());

            // set quality
            m_selectorTool->setQuality(muon);
            ATH_MSG_VERBOSE("Setting Quality " << muon.quality());
        }

        if (m_fillEnergyLossFromTrack) {
            const Trk::Track* trk = nullptr;
            if (muon.combinedTrackParticleLink().isValid()) { trk = (*(muon.combinedTrackParticleLink()))->track(); }
            if (!trk && muon.extrapolatedMuonSpectrometerTrackParticleLink().isValid()) {
                trk = (*(muon.extrapolatedMuonSpectrometerTrackParticleLink()))->track();
            }
            if (trk) {
                fillEnergyLossFromTrack(muon, &(trk->trackStateOnSurfaces()->stdcont()));
            } else {
                fillEnergyLossFromTrack(muon, nullptr);  // Just fill empty variables.
                ATH_MSG_VERBOSE("Couldn't find matching track which might have energy loss.");
            }
        }

        if (m_fillTimingInformationOnMuon) addRpcTiming(muon);

        if (!m_trackSegmentAssociationTool.empty() && muon.author() != xAOD::Muon::MuTagIMO && muon.author() != xAOD::Muon::MuGirl &&
            (muon.author() != xAOD::Muon::MuGirlLowBeta || m_segLowBeta))
            addSegmentsOnTrack(muon, segments);

        addMSIDScatteringAngles(muon);
        addMSIDScatteringAngles(muon.trackParticle(xAOD::Muon::CombinedTrackParticle));
        addMSIDScatteringAngles(muon.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle));
        addMSIDScatteringAngles(muon.trackParticle(xAOD::Muon::MSOnlyExtrapolatedMuonSpectrometerTrackParticle));
        return true;
    }

    void MuonCreatorTool::getRpcTiming(const xAOD::TrackParticle& tp, std::vector<unsigned int>& rpcHitIdentifier,
                                       std::vector<float>& rpcHitPositionX, std::vector<float>& rpcHitPositionY,
                                       std::vector<float>& rpcHitPositionZ, std::vector<float>& rpcHitTime) const {
        // loop over measurements
        const Trk::TrackStates& tsos = *(tp.track()->trackStateOnSurfaces());
        auto end = tsos.end();
        for (auto it = tsos.begin(); it != end; ++it) {
            // require measurement and track parameters
            const Trk::MeasurementBase* meas = (*it)->measurementOnTrack();
            const Trk::TrackParameters* pars = (*it)->trackParameters();
            if (!meas || !pars) continue;

            // only consider RPC hits
            Identifier mid = m_edmHelperSvc->getIdentifier(*meas);
            if (!m_idHelperSvc->isMuon(mid) || !m_idHelperSvc->isRpc(mid)) continue;

            // lambda to add a hit
            auto addHit = [&](const Trk::MeasurementBase& meas) {
                const Muon::RpcClusterOnTrack* rot = dynamic_cast<const Muon::RpcClusterOnTrack*>(&meas);
                if (!rot) return;

                rpcHitIdentifier.push_back(rot->identify().get_identifier32().get_compact());
                rpcHitPositionX.push_back(pars->position().x());
                rpcHitPositionY.push_back(pars->position().y());
                rpcHitPositionZ.push_back(pars->position().z());
                rpcHitTime.push_back(rot->time());
            };

            // in case competing rots loop over the contained rots
            const Muon::CompetingMuonClustersOnTrack* crot = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(meas);
            if (crot) {
                for (unsigned int i = 0; i < crot->numberOfContainedROTs(); ++i) { addHit(crot->rioOnTrack(i)); }
            } else {
                addHit(*meas);
            }
        }
    }

    void MuonCreatorTool::addRpcTiming(xAOD::Muon& muon) const {
        // vectors to be filled
        static const SG::AuxElement::Accessor<std::vector<unsigned int>> acc_HitIdentifier("rpcHitIdentifier");
        static const SG::AuxElement::Accessor<std::vector<float>> acc_positionX("rpcHitPositionX");
        static const SG::AuxElement::Accessor<std::vector<float>> acc_positionY("rpcHitPositionY");
        static const SG::AuxElement::Accessor<std::vector<float>> acc_positionZ("rpcHitPositionZ");
        static const SG::AuxElement::Accessor<std::vector<float>> acc_time("rpcHitTime");

        std::vector<unsigned int>& rpcHitIdentifier = acc_HitIdentifier(muon);
        std::vector<float>& rpcHitPositionX = acc_positionX(muon);
        std::vector<float>& rpcHitPositionY = acc_positionY(muon);
        std::vector<float>& rpcHitPositionZ = acc_positionZ(muon);
        std::vector<float>& rpcHitTime = acc_time(muon);

        // only fill if the primary track particle is not equal to the ID track
        // particle and it has an associated track with track states
        const xAOD::TrackParticle* tp = muon.primaryTrackParticle();
        if (tp && tp->track() && tp->track()->trackStateOnSurfaces() && tp != muon.trackParticle(xAOD::Muon::InnerDetectorTrackParticle)) {
            getRpcTiming(*tp, rpcHitIdentifier, rpcHitPositionX, rpcHitPositionY, rpcHitPositionZ, rpcHitTime);
        }
    }

    void MuonCreatorTool::addSegmentsOnTrack(xAOD::Muon& muon, const xAOD::MuonSegmentContainer* segments) const {
        std::vector<ElementLink<xAOD::MuonSegmentContainer>> associatedSegments;
        if (!m_trackSegmentAssociationTool->associatedSegments(muon, segments, associatedSegments)) {
            ATH_MSG_DEBUG("Failed to find associated segments ");
        }
        muon.setMuonSegmentLinks(associatedSegments);
    }

    void MuonCreatorTool::addEnergyLossToMuon(xAOD::Muon& muon) const {
        if (!muon.inDetTrackParticleLink().isValid()) {
            ATH_MSG_WARNING("Missing ID track particle link in addEnergyLossToMuon!");
            return;
        }

        // get ID track particle
        const Trk::Track* trk = (*(muon.inDetTrackParticleLink()))->track();
        if (!trk) {
            ATH_MSG_WARNING("Missing ID trk::track in addEnergyLossToMuon!");
            return;
        }

        // find last ID TSOS with track parameters
        const Trk::TrackStateOnSurface* lastID = nullptr;
        Trk::TrackStates::const_reverse_iterator it = trk->trackStateOnSurfaces()->rbegin();
        Trk::TrackStates::const_reverse_iterator itEnd = trk->trackStateOnSurfaces()->rend();
        for (; it != itEnd; ++it) {
            if ((*it)->trackParameters()) {
                lastID = *it;
                break;
            }
        }
        if (!lastID) {
            ATH_MSG_WARNING("Missing ID TSOS with track parameters in addEnergyLossToMuon!");
            return;
        }

        // get calorimeter TSOS
        std::vector<const Trk::TrackStateOnSurface*>* caloTSOS = m_caloMaterialProvider->getCaloTSOS(*((*it)->trackParameters()), *trk);

        if (!caloTSOS) {
            ATH_MSG_WARNING("Unable to find calo TSOS in addEnergyLossToMuon!");
            return;
        }

        // fill muon parameters for eloss
        fillEnergyLossFromTrack(muon, caloTSOS);

        // delete caloTSOS
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator it2 = caloTSOS->begin();
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator itEnd2 = caloTSOS->end();
        for (; it2 != itEnd2; ++it2) delete *it2;
        delete caloTSOS;
    }

    void MuonCreatorTool::fillEnergyLossFromTrack(xAOD::Muon& muon, const std::vector<const Trk::TrackStateOnSurface*>* tsosVector) const {
        // Ensure these are set for every muon
        if (!tsosVector) {
            muon.setParameter(0.f, xAOD::Muon::EnergyLoss);
            muon.setParameter(0.f, xAOD::Muon::ParamEnergyLoss);
            muon.setParameter(0.f, xAOD::Muon::MeasEnergyLoss);
            muon.setParameter(0.f, xAOD::Muon::EnergyLossSigma);
            muon.setParameter(0.f, xAOD::Muon::MeasEnergyLossSigma);
            muon.setParameter(0.f, xAOD::Muon::ParamEnergyLossSigmaPlus);
            muon.setParameter(0.f, xAOD::Muon::ParamEnergyLossSigmaMinus);

            muon.setEnergyLossType(xAOD::Muon::Parametrized);  // Not so nice! Add 'unknown' type?
            muon.setParameter(0.f, xAOD::Muon::FSR_CandidateEnergy);
            if (m_fillExtraELossInfo) acc_numEnergyLossPerTrack(muon) = 0;

            return;
        }

        unsigned int numEnergyLossPerTrack = 0;
        bool problem = false;
        for (const auto* tsos : *tsosVector) {
            const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());
            if (!meot) continue;
            const Trk::EnergyLoss* el = meot->energyLoss();
            const CaloEnergy* caloEnergy = dynamic_cast<const CaloEnergy*>(el);
            if (!caloEnergy) continue;
            ++numEnergyLossPerTrack;

            muon.setParameter(static_cast<float>(caloEnergy->deltaE()), xAOD::Muon::EnergyLoss);
            muon.setParameter(static_cast<float>(caloEnergy->deltaEParam()), xAOD::Muon::ParamEnergyLoss);
            muon.setParameter(static_cast<float>(caloEnergy->deltaEMeas()), xAOD::Muon::MeasEnergyLoss);
            muon.setParameter(static_cast<float>(caloEnergy->sigmaDeltaE()), xAOD::Muon::EnergyLossSigma);
            muon.setParameter(static_cast<float>(caloEnergy->sigmaDeltaEMeas()), xAOD::Muon::MeasEnergyLossSigma);
            muon.setParameter(static_cast<float>(caloEnergy->sigmaPlusDeltaEParam()), xAOD::Muon::ParamEnergyLossSigmaPlus);
            muon.setParameter(static_cast<float>(caloEnergy->sigmaMinusDeltaEParam()), xAOD::Muon::ParamEnergyLossSigmaMinus);

            muon.setEnergyLossType(static_cast<xAOD::Muon::EnergyLossType>(caloEnergy->energyLossType()));
            muon.setParameter(static_cast<float>(caloEnergy->fsrCandidateEnergy()), xAOD::Muon::FSR_CandidateEnergy);
        }
        if (numEnergyLossPerTrack > 1) {
            ATH_MSG_VERBOSE("More than one e loss per track... ");
            problem = true;
        }
        if (m_fillExtraELossInfo) acc_numEnergyLossPerTrack(muon) = numEnergyLossPerTrack;
        if (problem) ATH_MSG_VERBOSE("Dumping problematic muon: " << m_muonPrinter->print(muon));
    }

    void MuonCreatorTool::collectCells(const EventContext& ctx,
                                       xAOD::Muon& muon, xAOD::CaloClusterContainer* clusterContainer,
                                       const Trk::CaloExtension* inputCaloExt) const {
    
        const xAOD::TrackParticle* tp = muon.primaryTrackParticle();
        if (!tp || !clusterContainer) {
            if (!tp) ATH_MSG_WARNING("Can not get primary track.");
            return;
        }

        // get ParticleCellAssociation
        ATH_MSG_DEBUG(" Selected track: pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi());

        xAOD::CaloCluster* cluster = nullptr;
        SG::ReadHandle<CaloCellContainer> container(m_cellContainerName, ctx);

        SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey, ctx};
        const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;

        if (!inputCaloExt) {  // need to make one
            // for some reason, ID tracks need to be extrapolated from the ID exit, and
            // combined from the perigee
            std::unique_ptr<Trk::CaloExtension> caloExtension = m_caloExtTool->caloExtension(ctx, *tp);
            if (!caloExtension) {
                ATH_MSG_WARNING("Cannot get caloExtension.");
                return;
            }
            if (caloExtension->caloLayerIntersections().empty())
                ATH_MSG_DEBUG("Received a caloExtension object without track extrapolation");

            cluster = m_cellCollector.collectCells(*caloExtension, caloDDMgr, *container, *clusterContainer);
        } else
            cluster = m_cellCollector.collectCells(*inputCaloExt, caloDDMgr, *container, *clusterContainer);

        if (!cluster) {
            ATH_MSG_WARNING("Failed to create cluster from ParticleCellAssociation");
            return;
        } else {
            ATH_MSG_DEBUG(" New cluster: eta " << cluster->eta() << " phi " << cluster->phi() << " cells " << cluster->size());
        }

        // create element links
        ElementLink<xAOD::CaloClusterContainer> clusterLink(*clusterContainer, cluster->index(), ctx);
        muon.setClusterLink(clusterLink);
        const CaloNoise* caloNoise = nullptr;
        if (!m_caloNoiseKey.empty()) {
            SG::ReadCondHandle<CaloNoise> noiseH(m_caloNoiseKey, ctx);
            caloNoise = noiseH.cptr();
        }
        // collect the core energy
        std::vector<float> etcore(4, 0);
        m_cellCollector.collectEtCore(*cluster, etcore, caloNoise, m_sigmaCaloNoiseCut);

        acc_ET_Core(muon) = etcore[Rec::CaloCellCollector::ET_Core];
        acc_ET_EMCore(muon) = etcore[Rec::CaloCellCollector::ET_EMCore];
        acc_ET_TileCore(muon) = etcore[Rec::CaloCellCollector::ET_TileCore];
        acc_ET_HECCore(muon) = etcore[Rec::CaloCellCollector::ET_HECCore];
        if (m_caloNoiseKey.empty())
            ATH_MSG_DEBUG("NO Tool for calo noise,sigma: " << m_sigmaCaloNoiseCut);
        else
            ATH_MSG_DEBUG("sigma: " << m_sigmaCaloNoiseCut);

        ATH_MSG_DEBUG("Etcore: tot/em/tile/hec "
                      << etcore[Rec::CaloCellCollector::ET_Core] << "/" << etcore[Rec::CaloCellCollector::ET_EMCore] << "/"
                      << etcore[Rec::CaloCellCollector::ET_TileCore] << "/" << etcore[Rec::CaloCellCollector::ET_HECCore]);
    }

    void MuonCreatorTool::addAlignmentEffectsOnTrack(xAOD::TrackParticleContainer* trkCont) const {
        static const SG::AuxElement::Accessor<std::vector<std::vector<unsigned int>>> acc_ChId("alignEffectChId");
        static const SG::AuxElement::Accessor<std::vector<float>> acc_DeltaTrans("alignEffectDeltaTrans");
        static const SG::AuxElement::Accessor<std::vector<float>> acc_SigmaDeltaTrans("alignEffectSigmaDeltaTrans");
        static const SG::AuxElement::Accessor<std::vector<float>> acc_deltaAngle("alignEffectDeltaAngle");
        static const SG::AuxElement::Accessor<std::vector<float>> acc_sigmaDeltaAngle("alignEffectSigmaDeltaAngle");

        for (xAOD::TrackParticle* tp : *trkCont) {
            std::vector<std::vector<unsigned int>>& chId = acc_ChId(*tp);
            std::vector<float>& deltaTrans = acc_DeltaTrans(*tp);
            std::vector<float>& sigmaDeltaTrans = acc_SigmaDeltaTrans(*tp);
            std::vector<float>& deltaAngle = acc_deltaAngle(*tp);
            std::vector<float>& sigmaDeltaAngle = acc_sigmaDeltaAngle(*tp);

            const Trk::Track* trk = tp->track();
            if (trk && trk->trackStateOnSurfaces()) {
                int nAEOT = 0;
                for (const auto* tsos : *(trk->trackStateOnSurfaces())) {
                    if (!tsos->type(Trk::TrackStateOnSurface::Alignment)) continue;
                    const Trk::AlignmentEffectsOnTrack* aeot = tsos->alignmentEffectsOnTrack();
                    if (aeot) {
                        nAEOT++;
                        std::set<unsigned int> chIdSet;
                        for (auto id : aeot->vectorOfAffectedTSOS()) {
                            if (!id.is_valid() || !m_idHelperSvc->isMuon(id)) continue;
                            chIdSet.insert(m_idHelperSvc->chamberIndex(id));
                        }
                        std::vector<unsigned int> chIdVec;
                        std::copy(chIdSet.begin(), chIdSet.end(), std::back_inserter(chIdVec));

                        chId.push_back(chIdVec);
                        deltaTrans.push_back(aeot->deltaTranslation());
                        sigmaDeltaTrans.push_back(aeot->sigmaDeltaTranslation());
                        deltaAngle.push_back(aeot->deltaAngle());
                        sigmaDeltaAngle.push_back(aeot->sigmaDeltaAngle());
                    }
                }
            }
        }
    }

    void MuonCreatorTool::addMSIDScatteringAngles(xAOD::Muon& muon) const {
        const xAOD::TrackParticle* tp = muon.primaryTrackParticle();
        if (!tp || tp == muon.trackParticle(xAOD::Muon::InnerDetectorTrackParticle) || !tp->track() || !tp->track()->trackStateOnSurfaces())
            return;

        int nscatter = 0;
        for (const auto* tsos : *(tp->track()->trackStateOnSurfaces())) {
            if (tsos->materialEffectsOnTrack()) {
                const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());
                if (!meot->energyLoss() || !meot->scatteringAngles()) continue;
                if (meot->energyLoss()->deltaE() == 0) {  // artificial scatterer found
                    if (nscatter == 0) {
                        acc_deltaphi_0(muon) = meot->scatteringAngles()->deltaPhi();
                        acc_deltatheta_0(muon) = meot->scatteringAngles()->deltaTheta();
                        acc_sigmadeltaphi_0(muon) = meot->scatteringAngles()->sigmaDeltaPhi();
                        acc_sigmadeltatheta_0(muon) = meot->scatteringAngles()->sigmaDeltaTheta();
                    } else if (nscatter == 1) {
                        acc_deltaphi_1(muon) = meot->scatteringAngles()->deltaPhi();
                        acc_deltatheta_1(muon) = meot->scatteringAngles()->deltaTheta();
                        acc_sigmadeltaphi_1(muon) = meot->scatteringAngles()->sigmaDeltaPhi();
                        acc_sigmadeltatheta_1(muon) = meot->scatteringAngles()->sigmaDeltaTheta();
                    }
                    nscatter++;
                }
            }
            if (nscatter > 1) break;
        }
    }

    void MuonCreatorTool::addMSIDScatteringAngles(const xAOD::TrackParticle* tp) const {
        if (!tp) return;
        static const SG::AuxElement::Decorator<float> dec_deltaphi_1("deltaphi_1");
        static const SG::AuxElement::Decorator<float> dec_deltatheta_1("deltatheta_1");
        static const SG::AuxElement::Decorator<float> dec_sigmadeltaphi_1("sigmadeltaphi_1");
        static const SG::AuxElement::Decorator<float> dec_sigmadeltatheta_1("sigmadeltatheta_1");

        static const SG::AuxElement::Decorator<float> dec_deltaphi_0("deltaphi_0");
        static const SG::AuxElement::Decorator<float> dec_deltatheta_0("deltatheta_0");
        static const SG::AuxElement::Decorator<float> dec_sigmadeltaphi_0("sigmadeltaphi_0");
        static const SG::AuxElement::Decorator<float> dec_sigmadeltatheta_0("sigmadeltatheta_0");

        dec_deltaphi_1(*tp) = -999;
        dec_deltatheta_1(*tp) = -999;
        dec_sigmadeltaphi_1(*tp) = -999;
        dec_sigmadeltatheta_1(*tp) = -999;

        dec_deltaphi_0(*tp) = -999;
        dec_deltatheta_0(*tp) = -999;
        dec_sigmadeltaphi_0(*tp) = -999;
        dec_sigmadeltatheta_0(*tp) = -999;
        int nscatter = 0;
        if (!tp->track() || !tp->track()->trackStateOnSurfaces()) return;
        for (const auto* tsos : *(tp->track()->trackStateOnSurfaces())) {
            if (tsos->materialEffectsOnTrack()) {
                const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());
                if (!meot->energyLoss() || !meot->scatteringAngles()) continue;
                if (meot->energyLoss()->deltaE() == 0) {  // artificial scatterer found
                    if (nscatter == 0) {
                        dec_deltaphi_0(*tp) = meot->scatteringAngles()->deltaPhi();
                        dec_deltatheta_0(*tp) = meot->scatteringAngles()->deltaTheta();
                        dec_sigmadeltaphi_0(*tp) = meot->scatteringAngles()->sigmaDeltaPhi();
                        dec_sigmadeltatheta_0(*tp) = meot->scatteringAngles()->sigmaDeltaTheta();
                    } else if (nscatter == 1) {
                        dec_deltaphi_1(*tp) = meot->scatteringAngles()->deltaPhi();
                        dec_deltatheta_1(*tp) = meot->scatteringAngles()->deltaTheta();
                        dec_sigmadeltaphi_1(*tp) = meot->scatteringAngles()->sigmaDeltaPhi();
                        dec_sigmadeltatheta_1(*tp) = meot->scatteringAngles()->sigmaDeltaTheta();
                    }
                    nscatter++;
                }
            }
            if (nscatter > 1) break;
        }
    }
    void MuonCreatorTool::setP4(xAOD::Muon& muon, const xAOD::TrackParticle& tp) const { muon.setP4(tp.pt(), tp.eta(), tp.phi()); }

}  // namespace MuonCombined
