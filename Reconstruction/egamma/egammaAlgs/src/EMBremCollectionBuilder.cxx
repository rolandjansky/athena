/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "EMBremCollectionBuilder.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "AthenaKernel/errorcheck.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//std includes
#include <algorithm>
#include <memory>

EMBremCollectionBuilder::EMBremCollectionBuilder(const std::string& name,
                                                 ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode EMBremCollectionBuilder::initialize() {

  ATH_CHECK(m_selectedTrackParticleContainerKey.initialize());
  ATH_CHECK(m_trackParticleContainerKey.initialize());
  ATH_CHECK(m_OutputTrkPartContainerKey.initialize());
  ATH_CHECK(m_OutputTrackContainerKey.initialize());
  ATH_CHECK(m_TruthParticlesLinkKey.initialize(m_doTruth));

  /* retrieve the track refitter tool*/
  ATH_CHECK(m_trkRefitTool.retrieve());
  /* Get the particle creation tool */
  ATH_CHECK(m_particleCreatorTool.retrieve());
  /* Get the track slimming tool */
  ATH_CHECK(m_slimTool.retrieve());
  /* Get the track summary tool */
  ATH_CHECK(m_summaryTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode EMBremCollectionBuilder::EMBremCollectionBuilder::finalize(){

  ATH_MSG_INFO ("Not refitted due to Silicon Requirements " << m_FailedSiliconRequirFit);
  ATH_MSG_INFO ("Failed Fit Tracks " << m_FailedFitTracks);
  ATH_MSG_INFO ("RefittedTracks " << m_RefittedTracks);

  return StatusCode::SUCCESS;
}

StatusCode
EMBremCollectionBuilder::execute(const EventContext& ctx) const
{
  // Read the inputs

  // all the track particles
  SG::ReadHandle<xAOD::TrackParticleContainer> trackTES(m_trackParticleContainerKey,ctx);
  if(!trackTES.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "
                  << m_trackParticleContainerKey.key());
    return StatusCode::FAILURE;
  }

  // selected track particles
  SG::ReadHandle<xAOD::TrackParticleContainer> selectedTracks(m_selectedTrackParticleContainerKey,ctx);
  if(!selectedTracks.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "
                  << m_selectedTrackParticleContainerKey.key());
    return StatusCode::FAILURE;
  }

  // Create the final containers to be written out

  // track particles
  SG::WriteHandle<xAOD::TrackParticleContainer> finalTrkPartContainer(
    m_OutputTrkPartContainerKey, ctx);

  ATH_CHECK(finalTrkPartContainer.record(
    std::make_unique<xAOD::TrackParticleContainer>(),
    std::make_unique<xAOD::TrackParticleAuxContainer>()));

  xAOD::TrackParticleContainer* cPtrTrkPart = finalTrkPartContainer.ptr();

  // tracks
  SG::WriteHandle<TrackCollection> finalTracks(m_OutputTrackContainerKey, ctx);
  ATH_CHECK(finalTracks.record(std::make_unique<TrackCollection>()));
  TrackCollection* cPtrTracks = finalTracks.ptr();

  // Loop over the selected track-particles
  // Split TRT-alone from silicon ones. For the TRT we can get all the info already
  std::vector<const xAOD::TrackParticle*> siliconTrkTracks;
  siliconTrkTracks.reserve(16);
  std::vector<TrackWithIndex> trtAloneTrkTracks;
  trtAloneTrkTracks.reserve(8);
  for(const xAOD::TrackParticle* track : *selectedTracks){
    const Trk::Track* trktrack{nullptr};
    if (track->trackLink().isValid()){
      trktrack =track->track();
    }
    else{
      ATH_MSG_ERROR("TrackParticle has not Track --  are you running on AOD?");
      return StatusCode::FAILURE;
    }
    int nSiliconHits_trk =0;
    uint8_t dummy(0);
    if (track->summaryValue(dummy,xAOD::numberOfSCTHits)){
      nSiliconHits_trk += dummy;
    }
    if (track->summaryValue(dummy,xAOD::numberOfPixelHits)){
      nSiliconHits_trk += dummy;
    }
    ATH_MSG_DEBUG("Number of Silicon hits "<<nSiliconHits_trk);

    if (nSiliconHits_trk >= m_MinNoSiHits){
      siliconTrkTracks.push_back(track);
    }else{
      //copy Trk::Track and keep also the index of the original TrackParticle
      trtAloneTrkTracks.emplace_back(
        std::make_unique<Trk::Track>(*trktrack),track->index());
    }
  }

  // Store the results of the GSF refit
  std::vector<TrackWithIndex> refitted;  // refit success
  refitted.reserve(siliconTrkTracks.size());
  std::vector<TrackWithIndex> failedfit;  // refit failure

  // Do the GSF refit. Note that the output is a two collections
  // (success and failure) of TrackWithIndex, which means a xAOD::Track
  // and the index to the original track particle.
  // Note that the input is a xAOD::TrackParticle (the link to the xAOD::Track is used)
  ATH_CHECK(refitTracks(ctx,siliconTrkTracks,refitted,failedfit));
  siliconTrkTracks.clear();
  
  // Fill the final collections
  ATH_CHECK(createCollections(ctx,refitted,failedfit,trtAloneTrkTracks,
                    cPtrTracks,cPtrTrkPart,trackTES.ptr()));

  // Update counters
  m_RefittedTracks.fetch_add(refitted.size(), std::memory_order_relaxed);
  m_FailedFitTracks.fetch_add(failedfit.size(), std::memory_order_relaxed);
  m_FailedSiliconRequirFit.fetch_add(trtAloneTrkTracks.size(), std::memory_order_relaxed);
  return StatusCode::SUCCESS;
}

StatusCode
EMBremCollectionBuilder::refitTracks(
  const EventContext& ctx,
  const std::vector<const xAOD::TrackParticle*>& input,
  std::vector<TrackWithIndex>& refitted,
  std::vector<TrackWithIndex>& failedfit) const
{
  for (const xAOD::TrackParticle* in:input){
    const Trk::Track* track =in->track();
    IegammaTrkRefitterTool::Cache cache{};
    StatusCode status = m_trkRefitTool->refitTrack(ctx,
                                                   track,
                                                   cache);
    if (status == StatusCode::SUCCESS){
      ATH_MSG_DEBUG("FIT SUCCESS ");
      //this is new track
      refitted.emplace_back(
        std::move(cache.refittedTrack),in->index()
        );
    }
    else{
      ATH_MSG_DEBUG("FIT FAILED ");
      //This is copy ctor
      failedfit.emplace_back(
        std::make_unique<Trk::Track>(*track),in->index()
        );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
EMBremCollectionBuilder::createCollections(
  const EventContext& ctx,
  std::vector<TrackWithIndex>& refitted,
  std::vector<TrackWithIndex>& failedfit,
  std::vector<TrackWithIndex>& trtAlone,
  TrackCollection* finalTracks,
  xAOD::TrackParticleContainer* finalTrkPartContainer,
  const xAOD::TrackParticleContainer* AllTracks) const
{
  /*
   * Refitted are new tracks (not copied/cloned)
   * so need to update the summary
   */
  for (auto& trk : refitted) {
    updateGSFTrack(ctx, trk, AllTracks);
  }

  for (auto& trk : refitted) {
    ATH_CHECK(
      createNew(ctx, trk, finalTracks, finalTrkPartContainer, AllTracks));
  }

  for (auto& trk : failedfit) {
    ATH_CHECK(
      createNew(ctx, trk, finalTracks, finalTrkPartContainer, AllTracks));
  }

  for (auto& trk : trtAlone) {
    ATH_CHECK(
      createNew(ctx, trk, finalTracks, finalTrkPartContainer, AllTracks));
  }
  return StatusCode::SUCCESS;
}

StatusCode
EMBremCollectionBuilder::createNew(
  const EventContext& ctx,
  TrackWithIndex& trk_info,
  TrackCollection* finalTracks,
  xAOD::TrackParticleContainer* finalTrkPartContainer,
  const xAOD::TrackParticleContainer* AllTracks) const
{

  size_t origIndex = trk_info.origIndex;
  const xAOD::TrackParticle* original = AllTracks->at(origIndex);
  /*
   * Create TrackParticle it should be now owned by finalTrkPartContainer
   */
  xAOD::TrackParticle* aParticle = m_particleCreatorTool->createParticle(
    ctx, *(trk_info.track), finalTrkPartContainer, nullptr, xAOD::electron);

  if (!aParticle) {
    ATH_MSG_WARNING(
      "Could not create TrackParticle!!! for Track: " << *(trk_info.track));
    return StatusCode::SUCCESS;
  }

  // Add an element link back to original Track Particle collection
  static const SG::AuxElement::Accessor<
    ElementLink<xAOD::TrackParticleContainer>>
    tP("originalTrackParticle");
  ElementLink<xAOD::TrackParticleContainer> linkToOriginal(*AllTracks,
                                                           origIndex,ctx);
  tP(*aParticle) = linkToOriginal;

  if (m_doTruth) {
    // Add Truth decorations. Copy from the original.
    static const SG::AuxElement::Accessor<
      ElementLink<xAOD::TruthParticleContainer>>
      tPL("truthParticleLink");
    if (tPL.isAvailable(*original)) {
      ElementLink<xAOD::TruthParticleContainer> linkToTruth = tPL(*original);
      tPL(*aParticle) = linkToTruth;
    }
    static const SG::AuxElement::Accessor<float> tMP("truthMatchProbability");
    if (tMP.isAvailable(*original)) {
      float originalProbability = tMP(*original);
      tMP(*aParticle) = originalProbability;
    }
    static const SG::AuxElement::Accessor<int> tT("truthType");
    if (tT.isAvailable(*original)) {
      int truthType = tT(*original);
      tT(*aParticle) = truthType;
    }
    static const SG::AuxElement::Accessor<int> tO("truthOrigin");
    if (tO.isAvailable(*original)) {
      int truthOrigin = tO(*original);
      tO(*aParticle) = truthOrigin;
    }
  } // End truth

  /*
   * Add qoverP from the last measurement
   */
  float QoverPLast(0);
  auto rtsos = trk_info.track->trackStateOnSurfaces()->rbegin();
  for (; rtsos != trk_info.track->trackStateOnSurfaces()->rend(); ++rtsos) {
    if ((*rtsos)->type(Trk::TrackStateOnSurface::Measurement) &&
        (*rtsos)->trackParameters() != nullptr &&
        (*rtsos)->measurementOnTrack() != nullptr &&
        !(*rtsos)->measurementOnTrack()->type(
          Trk::MeasurementBaseType::PseudoMeasurementOnTrack)) {
      QoverPLast = (*rtsos)->trackParameters()->parameters()[Trk::qOverP];
      break;
    }
  }

  static const SG::AuxElement::Accessor<float> QoverPLM("QoverPLM");
  QoverPLM(*aParticle) = QoverPLast;

  // Now slim the Trk::Track for writing to disk
  m_slimTool->slimTrack(*(trk_info.track));
  finalTracks->push_back(std::move(trk_info.track));
  ElementLink<TrackCollection> trackLink(*finalTracks,finalTracks->size()-1,ctx);
  aParticle->setTrackLink( trackLink );
  return StatusCode::SUCCESS;
}

void
EMBremCollectionBuilder::updateGSFTrack(
  const EventContext& ctx,
  const TrackWithIndex& trk_info,
  const xAOD::TrackParticleContainer* AllTracks) const
{

  //update the summary of the non-const track without hole search
  m_summaryTool->updateTrackSummary(ctx,*(trk_info.track));
  //Get the summary so as to add info to it
  Trk::TrackSummary* summary = trk_info.track->trackSummary();

  size_t origIndex = trk_info.origIndex;
  const xAOD::TrackParticle* original = AllTracks->at(origIndex);

  uint8_t dummy(0);
  if (m_doPix) {
    //copy over dead sensors
    uint8_t deadPixel= original->summaryValue(dummy,xAOD::numberOfPixelDeadSensors)?dummy:0;
    summary->update(Trk::numberOfPixelDeadSensors,deadPixel);

    int nPixHitsRefitted = summary->get(Trk::numberOfPixelHits);
    int nPixOutliersRefitted = summary->get(Trk::numberOfPixelOutliers);
    int nPixHitsOriginal = original->summaryValue(dummy,xAOD::numberOfPixelHits) ? dummy:-1;
    int nPixHolesOriginal = original->summaryValue(dummy,xAOD::numberOfPixelHoles)? dummy:-1;

    int nPixOutliersOriginal = original->summaryValue(dummy,xAOD::numberOfPixelOutliers)? dummy:-1;
    summary->update(Trk::numberOfPixelHoles,
                    nPixHitsOriginal + nPixHolesOriginal +
                      nPixOutliersOriginal - nPixOutliersRefitted -
                      nPixHitsRefitted);
  }
  if (m_doSCT) {
    uint8_t deadSCT= original->summaryValue(dummy,xAOD::numberOfSCTDeadSensors)?dummy:0;
    summary->update(Trk::numberOfSCTDeadSensors,deadSCT);

    uint8_t SCTDoubleHoles = original->summaryValue(dummy,xAOD::numberOfSCTDoubleHoles )?dummy:0;
    summary->update(Trk::numberOfSCTDoubleHoles , SCTDoubleHoles);

    int nSCTHitsRefitted = summary->get(Trk::numberOfSCTHits);
    int nSCTOutliersRefitted = summary->get(Trk::numberOfSCTOutliers);
    int nSCTHitsOriginal = original->summaryValue(dummy,xAOD::numberOfSCTHits) ? dummy:-1;
    int nSCTHolesOriginal = original->summaryValue(dummy,xAOD::numberOfSCTHoles) ? dummy:-1;
    int nSCTOutliersOriginal = original->summaryValue(dummy,xAOD::numberOfSCTOutliers) ? dummy:-1;

    summary->update(Trk::numberOfSCTHoles,
                    nSCTHitsOriginal + nSCTHolesOriginal +
                      nSCTOutliersOriginal - nSCTOutliersRefitted -
                      nSCTHitsRefitted);
  }
  int nTRTHitsRefitted = summary->get(Trk::numberOfTRTHits);
  int nTRTOutliersRefitted = summary->get(Trk::numberOfTRTOutliers);
  int nTRTHitsOriginal = original->summaryValue(dummy,xAOD::numberOfTRTHits) ? dummy:-1;
  int nTRTHolesOriginal = original->summaryValue(dummy,xAOD::numberOfTRTHoles) ? dummy:-1;
  int nTRTOutliersOriginal = original->summaryValue(dummy,xAOD::numberOfTRTOutliers) ? dummy:-1;

  summary->update(Trk::numberOfTRTHoles,
                  nTRTHitsOriginal + nTRTHolesOriginal + nTRTOutliersOriginal -
                    nTRTOutliersRefitted - nTRTHitsRefitted);
}
