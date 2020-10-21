/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSummaryHelperTool/InDetTrackSummaryHelperTool.h"

// forward declares
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
// normal includes
#include "Identifier/Identifier.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/InDetTrackSummary.h"

#include <cassert>

//==========================================================================
InDet::InDetTrackSummaryHelperTool::InDetTrackSummaryHelperTool(const std::string& t,
                                                                const std::string& n,
                                                                const IInterface*  p)
:
  base_class(t, n, p)
{
}

//==========================================================================
namespace {
   std::string_view stripStoreName(const std::string &name) {
      std::string::size_type pos = name.find("+");
      pos =  (pos!=std::string::npos ? pos + 1 : 0);
      return std::string_view( &(name.c_str()[pos]),name.size()-pos);
   }
}

StatusCode InDet::InDetTrackSummaryHelperTool::initialize()
{
  if (m_usePixel) {
    if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) {
      ATH_MSG_ERROR("Could not get PixelID helper !");
      return StatusCode::FAILURE;
    }
  }

  if (m_useSCT) {
    if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) {
      ATH_MSG_ERROR("Could not get SCT_ID helper !");
      return StatusCode::FAILURE;
    }
  }

  if (m_useTRT) {
    if (detStore()->retrieve(m_trtId, "TRT_ID").isFailure()) {
      ATH_MSG_ERROR("Could not get TRT_ID helper !");
      return StatusCode::FAILURE;
    }
  }

  ATH_CHECK( m_assoTool.retrieve( DisableTool{!m_doSharedHits || m_assoTool.empty()} ));
  ATH_CHECK( m_pixeldedxtool.retrieve( DisableTool{m_pixeldedxtool.empty() }));
  ATH_CHECK( m_holeSearchTool.retrieve( DisableTool{m_holeSearchTool.empty() }));
  ATH_CHECK( m_testBLayerTool.retrieve( DisableTool{m_testBLayerTool.empty() }));
  ATH_CHECK( m_TRTStrawSummaryTool.retrieve( DisableTool{not m_useTRT or m_TRTStrawSummaryTool.empty() }));

  ATH_CHECK(m_clusterSplitProbContainer.initialize( !m_clusterSplitProbContainer.key().empty()));

  if (!m_renounce.empty()) {
     for (Gaudi::DataHandle* input_handle : inputHandles()) {
        std::string_view base_name(stripStoreName(input_handle->objKey()));
        for (const std::string &renounce_input : m_renounce) {
           if (base_name==renounce_input) {
              renounce(*input_handle);
              ATH_MSG_INFO("Renounce : " << name() << " . " << input_handle->objKey() );
           }
        }
     }
  }

  ATH_MSG_INFO("initialize() successful in " << name());

  return StatusCode::SUCCESS;
}


namespace {
  bool isShared(const Trk::PRDtoTrackMap *prd_to_track_map,
                                            const PublicToolHandle<Trk::IPRD_AssociationTool> &asso_tool,
                                            const Trk::PrepRawData& prd) {
    if (prd_to_track_map) {
      return prd_to_track_map->isShared(prd);
    }
    else {
      if (!asso_tool.isEnabled()) {
        throw std::logic_error("Shared hits to be computed but no PRDtoTrack provided "
                               " (and no association tool configured (deprecated))");
      }
      return asso_tool->isShared(prd);
    }
  }
}

//==========================================================================
void InDet::InDetTrackSummaryHelperTool::analyse(const EventContext& ctx,
                                                 const Trk::Track& track,
                                                 const Trk::PRDtoTrackMap *prd_to_track_map,
                                                 const Trk::RIO_OnTrack* rot,
                                                 const Trk::TrackStateOnSurface* tsos,
                                                 std::vector<int>& information,
                                                 std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const
{
  const Identifier& id = rot->identify();
  bool  isOutlier      = tsos->type(Trk::TrackStateOnSurface::Outlier);
  bool  ispatterntrack = (track.info().trackFitter()==Trk::TrackInfo::Unknown);

  if ( m_usePixel and m_pixelId->is_pixel(id) ) {

    if (isOutlier and not ispatterntrack ) { // ME: outliers on pattern tracks may be reintegrated by fitter, so count them as hits
      information[Trk::numberOfPixelOutliers]++;
      if (m_pixelId->layer_disk(id)==0 and m_pixelId->is_barrel(id)){
        information[Trk::numberOfInnermostPixelLayerOutliers]++;
      }
      if (m_pixelId->layer_disk(id)==1 and m_pixelId->is_barrel(id)){
        information[Trk::numberOfNextToInnermostPixelLayerOutliers]++;
      }
    } else {
      bool hitIsSplit(false);
      if (m_pixelId->is_dbm(id)) {
        int offset = static_cast<int> (Trk::DBM0); //get int value of first DBM layer
        offset    += m_pixelId->layer_disk(id);
        hitPattern.set(offset);
        information[Trk::numberOfDBMHits]++;
      } else {
        information[Trk::numberOfPixelHits]++;
        if (m_pixelId->layer_disk(id) == 0 and m_pixelId->is_barrel(id))
          information[Trk::numberOfInnermostPixelLayerHits]++;
        if (m_pixelId->layer_disk(id) == 1 and m_pixelId->is_barrel(id))
          information[Trk::numberOfNextToInnermostPixelLayerHits]++;
        // check to see if there's an ambiguity with the ganged cluster.
        const PixelClusterOnTrack* pix = nullptr;
        if (rot->rioType(Trk::RIO_OnTrackType::PixelCluster)) {
          pix = static_cast<const PixelClusterOnTrack*>(rot);
        }
        if ( not pix ) {
          ATH_MSG_ERROR("Could not cast pixel RoT to PixelClusterOnTrack!");
        } else {
          const InDet::PixelCluster* pixPrd = pix->prepRawData();
          const Trk::ClusterSplitProbabilityContainer::ProbabilityInfo &splitProb = getClusterSplittingProbability(pixPrd);
          if ( pixPrd and splitProb.isSplit() ) { information[Trk::numberOfPixelSplitHits]++; hitIsSplit=true; }
          if (pixPrd and m_pixelId->is_barrel(id) and
              m_pixelId->layer_disk(id) == 0 and splitProb.isSplit())
            information[Trk::numberOfInnermostLayerSplitHits]++;
          if (pixPrd and m_pixelId->is_barrel(id) and
              m_pixelId->layer_disk(id) == 1 and splitProb.isSplit())
            information[Trk::numberOfNextToInnermostLayerSplitHits]++;
          if ( pix->isBroadCluster() ) information[Trk::numberOfPixelSpoiltHits]++;
          if ( pix->hasClusterAmbiguity() ) {
            information[Trk::numberOfGangedPixels]++;
            if (pix->isFake()) information[Trk::numberOfGangedFlaggedFakes]++;
          }
        }

        if ( ( m_pixelId->is_barrel(id) ) ) {
          int offset = m_pixelId->layer_disk(id);
          if (not hitPattern.test(offset)) information[Trk::numberOfContribPixelLayers]++;
          hitPattern.set(offset); // assumes numbered consecutively
        } else {
          int offset = static_cast<int> (Trk::pixelEndCap0); //get int value of first pixel endcap disc
          offset    += m_pixelId->layer_disk(id);
          if (not hitPattern.test(offset)) information[Trk::numberOfContribPixelLayers]++;
          hitPattern.set(offset); // assumes numbered consecutively
        }
      }

      if (m_doSharedHits && !isOutlier) {
        // If we are running the TIDE ambi don't count split hits as shared
        if ( not (m_runningTIDE_Ambi and hitIsSplit) ) {
          // used in more than one track ?
          if ( isShared(prd_to_track_map, m_assoTool, *(rot->prepRawData())) ) {
            ATH_MSG_DEBUG("shared Pixel hit found");
            information[Trk::numberOfPixelSharedHits]++;
            if ( (m_pixelId->is_blayer(id) ) ) {
              ATH_MSG_DEBUG("--> shared Pixel hit is in b-layer");
              information[Trk::numberOfBLayerSharedHits]++;
            }
            if ( (m_pixelId->is_barrel(id) and m_pixelId->layer_disk(id)==0) ) {
              ATH_MSG_DEBUG("--> shared Pixel hit is in innermost layer");
              information[Trk::numberOfInnermostPixelLayerSharedHits]++;
            }
            if ( (m_pixelId->is_barrel(id) and m_pixelId->layer_disk(id)==1) ) {
              ATH_MSG_DEBUG("--> shared Pixel hit is in next to innermost layer");
              information[Trk::numberOfNextToInnermostPixelLayerSharedHits]++;
            }
          }
        }
      }
    }

  } else if (m_useSCT and m_sctId->is_sct(id) ) {
    if (isOutlier and not ispatterntrack ) { // ME: outliers on pattern tracks may be reintegrated by fitter, so count them as hits
      information[Trk::numberOfSCTOutliers]++;

    } else {
      information[Trk::numberOfSCTHits]++;

      const InDet::SCT_ClusterOnTrack* sctclus = nullptr;
      if (rot->rioType(Trk::RIO_OnTrackType::SCTCluster)) {
        sctclus = static_cast<const InDet::SCT_ClusterOnTrack*>(rot);
      }
      if ( not sctclus ) {
        ATH_MSG_ERROR("Could not cast SCT RoT to SCT_ClusterOnTrack!");
      } else {
        if (sctclus->isBroadCluster()) information[Trk::numberOfSCTSpoiltHits]++;
      }

      if ( (m_sctId->is_barrel(id) ) ) {
        int offset = static_cast<int>(Trk::sctBarrel0);
        hitPattern.set( offset + m_sctId->layer_disk(id) ); // assumes numbered consecutively
      } else {
        int offset = static_cast<int>(Trk::sctEndCap0); //get int value of first sct endcap disc
        hitPattern.set( offset + m_sctId->layer_disk(id) ); // assumes numbered consecutively
      }

      if (m_doSharedHits && !isOutlier) {
        if ( isShared(prd_to_track_map, m_assoTool, *(rot->prepRawData())) ) {
          ATH_MSG_DEBUG("shared SCT hit found");
          information[Trk::numberOfSCTSharedHits]++;
        }
      }
    }
  } else if (m_useTRT and m_trtId->is_trt(id) ) {
    bool isArgonStraw   = false;
    bool isKryptonStraw = false;
    if (not m_TRTStrawSummaryTool.empty()) {
      int statusHT = m_TRTStrawSummaryTool->getStatusHT(id,ctx);
      if ( statusHT == TRTCond::StrawStatus::Argon or
           statusHT == TRTCond::StrawStatus::Dead  or
           statusHT == TRTCond::StrawStatus::EmulateArgon ) {
        isArgonStraw = true;
      }
      if ( statusHT == TRTCond::StrawStatus::Krypton or
           statusHT == TRTCond::StrawStatus::EmulateKrypton ) {
        isKryptonStraw = true;
      }
    }
    if ( not isArgonStraw and not isKryptonStraw ) {
      information[Trk::numberOfTRTXenonHits]++;
    }

    if (isOutlier and not ispatterntrack) {
      // ME: outliers on pattern tracks may be
      // reintegrated by fitter, so count them as hits
      information[Trk::numberOfTRTOutliers]++;

      const InDet::TRT_DriftCircleOnTrack* trtDriftCircle = nullptr;
      if (rot->rioType(Trk::RIO_OnTrackType::TRT_DriftCircle)) {
        trtDriftCircle = static_cast<const InDet::TRT_DriftCircleOnTrack*>(rot);
      }
      if (not trtDriftCircle) {
        ATH_MSG_ERROR("Could not cast TRT RoT to TRT_DriftCircleOnTracknot ");
      } else {
        if (trtDriftCircle->highLevel() == true and not isArgonStraw and
            not isKryptonStraw)
          information[Trk::numberOfTRTHighThresholdOutliers]++;
      }
    } else {
      information[Trk::numberOfTRTHits]++;
      double error2=rot->localCovariance()(0,0);
      if (error2>1) information[Trk::numberOfTRTTubeHits]++;

      const InDet::TRT_DriftCircleOnTrack* trtDriftCircle = nullptr;
      if (rot->rioType(Trk::RIO_OnTrackType::TRT_DriftCircle)) {
        trtDriftCircle = static_cast<const InDet::TRT_DriftCircleOnTrack*>(rot);
      }
      if (not trtDriftCircle) {
        ATH_MSG_ERROR("Could not cast TRT RoT to TRT_DriftCircleOnTracknot ");
      } else {
        if (trtDriftCircle->highLevel() == true) {
          if (not isArgonStraw and not isKryptonStraw)
            information[Trk::numberOfTRTHighThresholdHits]++;
          assert(Trk::numberOfTRTHighThresholdHitsTotal < information.size());
          information[Trk::numberOfTRTHighThresholdHitsTotal]++;
        }
      }
    }

    if (m_doSharedHitsTRT && !isOutlier) {
       // used in more than one track ?
      assert(information[Trk::numberOfTRTSharedHits]>=0);
      if ( isShared(prd_to_track_map, m_assoTool, *(rot->prepRawData())) ) {
          ATH_MSG_DEBUG("shared TRT hit found");
          information[Trk::numberOfTRTSharedHits]++;
       }
    }
  }
  return;
}

void InDet::InDetTrackSummaryHelperTool::analyse(const EventContext& ctx,
                                                 const Trk::Track& track,
                                                 const Trk::PRDtoTrackMap *prd_to_track_map,
                                                 const Trk::CompetingRIOsOnTrack* crot,
                                                 const Trk::TrackStateOnSurface* tsos,
                                                 std::vector<int>& information,
                                                 std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const
{
  // re-produce prior behaviour (i.e. just take most probable ROT)
  analyse(ctx,
          track,
          prd_to_track_map,
          &crot->rioOnTrack(crot->indexOfMaxAssignProb()),
          tsos,
          information,
          hitPattern);
}

void InDet::InDetTrackSummaryHelperTool::searchForHoles(const Trk::Track& track,
                                                        std::vector<int>& information,
                                                        const Trk::ParticleHypothesis partHyp) const
{
  ATH_MSG_DEBUG("Do hole search within HELPER, PLEASE FIX THIS AFTER 16.0.X");
  m_holeSearchTool->countHoles(track, information, partHyp);

  // this is a hack, we need to run the TestBLayer Tool somewhere

  if (m_usePixel and not m_testBLayerTool.empty()) {

    if (information[Trk::numberOfContribPixelLayers] == 0) {
      ATH_MSG_DEBUG("No pxiels on track, so wo do not expect a B-Layer hit !");
      information[Trk::expectInnermostPixelLayerHit] = 0;
      information[Trk::expectNextToInnermostPixelLayerHit] = 0;
    } else{
      //innermost layer block
      if (information[Trk::numberOfInnermostPixelLayerHits] > 0){
        information[Trk::expectInnermostPixelLayerHit] = 1;
      } else {
        if (m_testBLayerTool->expectHitInInnermostPixelLayer(&track) ) {
          ATH_MSG_DEBUG("expect Pixel Layer 0 hit !");
          information[Trk::expectInnermostPixelLayerHit] = 1;
        } else {
          ATH_MSG_DEBUG("do not expect Pixel Layer 0 hit !");
          information[Trk::expectInnermostPixelLayerHit] = 0;
        }
      }

      //next to innermost block
      if (information[Trk::numberOfNextToInnermostPixelLayerHits] > 0){
        information[Trk::expectNextToInnermostPixelLayerHit] = 1;
      } else {
        if (m_testBLayerTool->expectHitInNextToInnermostPixelLayer(&track) ) {
          ATH_MSG_DEBUG("expect Pixel Layer 1 hit !");
          information[Trk::expectNextToInnermostPixelLayerHit] = 1;
        } else {
          ATH_MSG_DEBUG("do not expect Pixel Layer 1 hit !");
          information[Trk::expectNextToInnermostPixelLayerHit] = 0;
        }
      }
    }
  }

  return;
}


void InDet::InDetTrackSummaryHelperTool::updateSharedHitCount(const Trk::Track &track,
                                                              const Trk::PRDtoTrackMap *prd_to_track_map,
                                                              Trk::TrackSummary& summary) const {
  // loop over track states on surface and take pixel / sct to update the shared hit count
  summary.m_information[Trk::numberOfPixelSharedHits]  = 0;
  summary.m_information[Trk::numberOfInnermostPixelLayerSharedHits] = 0;
  summary.m_information[Trk::numberOfNextToInnermostPixelLayerSharedHits] = 0;
  summary.m_information[Trk::numberOfSCTSharedHits]    = 0;
  summary.m_information[Trk::numberOfTRTSharedHits]    = 0;
  if ( m_runningTIDE_Ambi ) {
    summary.m_information[Trk::numberOfPixelSplitHits]   = 0;
    summary.m_information[Trk::numberOfInnermostLayerSplitHits]   = 0;
    summary.m_information[Trk::numberOfNextToInnermostLayerSplitHits]   = 0;
  }

  const DataVector<const Trk::MeasurementBase>* measurements = track.measurementsOnTrack();
  if (measurements){
    for (const auto& ms : *measurements){
      // check if it's a rot
      const Trk::RIO_OnTrack* rot = nullptr;
      if (ms->type(Trk::MeasurementBaseType::RIO_OnTrack)) {
        rot = static_cast<const Trk::RIO_OnTrack*>(ms);
      }
      if (rot){
        const Identifier& id = rot->identify();
        if ( m_doSharedHits and m_usePixel and m_pixelId->is_pixel(id) ) {
          // check if shared
          bool hitIsSplit(false);
          if (m_runningTIDE_Ambi) {
            const PixelClusterOnTrack* pix =nullptr;
            if (rot->rioType(Trk::RIO_OnTrackType::PixelCluster)) {
              pix = static_cast<const PixelClusterOnTrack*>(rot);
            }
            if (pix) {
              const InDet::PixelCluster* pixPrd = pix->prepRawData();
              const Trk::ClusterSplitProbabilityContainer::ProbabilityInfo &splitProb = getClusterSplittingProbability(pixPrd);
              if (pixPrd and splitProb.isSplit()) {
                summary.m_information[Trk::numberOfPixelSplitHits]++;
                hitIsSplit=true;
                if ( m_pixelId->is_barrel(id) and m_pixelId->layer_disk(id)==0) summary.m_information[Trk::numberOfInnermostLayerSplitHits]++;
                if ( m_pixelId->is_barrel(id) and m_pixelId->layer_disk(id)==1) summary.m_information[Trk::numberOfNextToInnermostLayerSplitHits]++;
              }
            }
          }
          // If we are running the TIDE ambi don't count split hits as shared
          if ( not (m_runningTIDE_Ambi and hitIsSplit) ){
            if (isShared(prd_to_track_map, m_assoTool, *(rot->prepRawData()))) {
              ATH_MSG_DEBUG("shared Pixel hit found");
              summary.m_information[Trk::numberOfPixelSharedHits]++;
              if ( (m_pixelId->is_barrel(id) and m_pixelId->layer_disk(id)==0) ) {
                ATH_MSG_DEBUG("--> shared Pixel hit is in Innermost Pixel layer");
                summary.m_information[Trk::numberOfInnermostPixelLayerSharedHits]++;
              } else if ( (m_pixelId->is_barrel(id) and m_pixelId->layer_disk(id)==1) ) {
                ATH_MSG_DEBUG("--> shared Pixel hit is in Next To Innermost Pixel layer");
                summary.m_information[Trk::numberOfNextToInnermostPixelLayerSharedHits]++;
              }
            }
          }
        } else if ( m_doSharedHits and m_useSCT and m_sctId->is_sct(id) ){
          // used in more than one track ?
          if ( isShared(prd_to_track_map, m_assoTool, *(rot->prepRawData())) ) {
            ATH_MSG_DEBUG("shared SCT hit found");
            summary.m_information[Trk::numberOfSCTSharedHits]++;
          }
        }
        if (m_doSharedHitsTRT and m_useTRT and m_trtId->is_trt(id)) {
          // used in more than one track ?
          if ( isShared(prd_to_track_map, m_assoTool, *(rot->prepRawData())) ) {
            ATH_MSG_DEBUG("shared TRT hit found");
            summary.m_information[Trk::numberOfTRTSharedHits]++;
          }
        }
      }
    }
  }
  return;
}

void  InDet::InDetTrackSummaryHelperTool::updateExpectedHitInfo(const Trk::Track &track, Trk::TrackSummary& summary) const{

  if (m_usePixel and not m_testBLayerTool.empty() ) {

    if ( summary.m_information[Trk::numberOfContribPixelLayers] == 0 ) {
      ATH_MSG_DEBUG("No pxiels on track, so wo do not expect a B-Layer hit !");
      summary.m_information[Trk::expectInnermostPixelLayerHit] = 0;
      summary.m_information[Trk::expectNextToInnermostPixelLayerHit] = 0;
    } else{
      //innermost layer block
      if (summary.m_information[Trk::numberOfInnermostPixelLayerHits] > 0){
        ATH_MSG_DEBUG("Innermost pixel Layer hit on track, so we expect a innermost pixel layer hit !");
        summary.m_information[Trk::expectInnermostPixelLayerHit] = 1;
      } else {
        if (m_testBLayerTool->expectHitInInnermostPixelLayer(&track) ) {
          ATH_MSG_DEBUG("expect Pixel Layer 0 hit !");
          summary.m_information[Trk::expectInnermostPixelLayerHit] = 1;
        } else {
          ATH_MSG_DEBUG("do not expect Pixel Layer 0 hit !");
          summary.m_information[Trk::expectInnermostPixelLayerHit] = 0;
        }
      }

      //next to innermost block
      if (summary.m_information[Trk::numberOfNextToInnermostPixelLayerHits] > 0){
        summary.m_information[Trk::expectNextToInnermostPixelLayerHit] = 1;
      } else {
        if (m_testBLayerTool->expectHitInNextToInnermostPixelLayer(&track) ) {
          ATH_MSG_DEBUG("expect Pixel Layer 1 hit !");
          summary.m_information[Trk::expectNextToInnermostPixelLayerHit] = 1;
        } else {
          ATH_MSG_DEBUG("do not expect Pixel Layer 1 hit !");
          summary.m_information[Trk::expectNextToInnermostPixelLayerHit] = 0;
        }
      }
    }
  }

  return;
}


void InDet::InDetTrackSummaryHelperTool::updateAdditionalInfo(Trk::TrackSummary& summary, std::vector<float>& eprob,
                                                              float& dedx, int& nclus, int& noverflowclus) const {
  summary.m_eProbability = eprob;
  summary.m_dedx = dedx;
  summary.m_nhitsdedx =  nclus;
  summary.m_nhitsoverflowdedx = noverflowclus;

  return;
}

void  InDet::InDetTrackSummaryHelperTool::addDetailedTrackSummary(const Trk::Track& track, Trk::TrackSummary& summary) const {
  if (summary.m_indetTrackSummary and not m_overwriteidsummary) {
    ATH_MSG_DEBUG("TrackSummary already has detailed indet track summary, not adding a new one");
    return;
  }
  if (not m_usePixel) {
    ATH_MSG_DEBUG("Pixels are off, so no need for an detailed indet track summary");
    return;
  }
  ATH_MSG_DEBUG("Adding detailed indet track summary");
  delete summary.m_indetTrackSummary;
  Trk::InDetTrackSummary* indetTrackSummary = new Trk::InDetTrackSummary();
  Trk::InDetTrackSummary& trackSummary      = *indetTrackSummary;
  if (m_usePixel and not m_pixeldedxtool.empty() and (track.perigeeParameters() or not track.trackParameters()->empty())) {
    const Trk::TrackParameters* par = track.perigeeParameters();
    if (par==nullptr) par = *track.trackParameters()->begin();
    double p      = (par->parameters()[Trk::qOverP]!=0) ? 1./par->parameters()[Trk::qOverP] : 1.e15;
    double dedx   = summary.getPixeldEdx();
    int ngoodhits = summary.numberOfUsedHitsdEdx();
    if (ngoodhits>0 and dedx>0 and track.info().trackFitter()!=Trk::TrackInfo::Unknown and p<1.e15) {
      trackSummary.m_likelihoodspixeldedx = m_pixeldedxtool->getLikelihoods(dedx, p, ngoodhits);
      trackSummary.m_massdedx             = m_pixeldedxtool->getMass(dedx, p, ngoodhits);
    }
  }
  summary.m_indetTrackSummary = indetTrackSummary;
}

// ====================================================================================================================

StatusCode InDet::InDetTrackSummaryHelperTool::finalize() {
  return StatusCode::SUCCESS;
}
