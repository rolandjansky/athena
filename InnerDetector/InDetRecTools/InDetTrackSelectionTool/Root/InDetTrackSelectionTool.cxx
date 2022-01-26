/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#include "AsgMessaging/Check.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "InDetAccessor.h"

#ifndef XAOD_ANALYSIS
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkTrack/Track.h"
#include "VxVertex/Vertex.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/FitQuality.h"
#endif

#include <memory>
#include <array>

using namespace InDetAccessor;

namespace {
   template <unsigned int n_summary_types>
   std::array<xAOD::SummaryType, n_summary_types> summaryArray( std::array<xAOD::SummaryType, n_summary_types> summary_types) {return summary_types; }

   template <class Trk_Helper, unsigned int n_summary_types>
   class  MinTRTHitsCut {
   public:
      MinTRTHitsCut( double maxTrtEtaAcceptance,
                     double maxEtaForTrtHitCuts,
                     int min_n_hits,
                     std::array<xAOD::SummaryType, n_summary_types> summary_types)
         : m_maxTrtEtaAcceptance(maxTrtEtaAcceptance),
           m_maxEtaForTrtHitCuts(maxEtaForTrtHitCuts),
           m_minNHits(min_n_hits),
           m_summaryTypes(summary_types)
      {}

      uint8_t nHits(Trk_Helper helper, asg::AsgMessaging &msgHelper) const {
         return getSummarySum(helper,msgHelper,m_summaryTypes);
      }
      bool operator()(Trk_Helper helper, asg::AsgMessaging &msgHelper) {
         double absEta = std::abs( helper.eta( msgHelper) );
         return      (absEta <= m_maxTrtEtaAcceptance || absEta > m_maxEtaForTrtHitCuts) || nHits(helper,msgHelper) >=m_minNHits;
      }
   private:
      double m_maxTrtEtaAcceptance;
      double m_maxEtaForTrtHitCuts;
      int m_minNHits;
      std::array<xAOD::SummaryType, n_summary_types> m_summaryTypes;
   };

   // return the first bin whose value is lower or equal than the given value
   // the bins have to be in ascending order;
   template <typename T>
   unsigned int findBin(const std::vector<T> &bins, T value) {
      if (bins.empty() || bins[0]>value) return bins.size();
      unsigned int bin_i=bins.size();
      while ( value < bins[--bin_i]) {};
      return bin_i;
   }
   // return true if the given bins are in ascending order
   template <typename T>
   bool checkOrder(const std::vector<T> &bins) {
      if (bins.empty()) return true;
      for (unsigned int bin_i=1; bin_i<bins.size(); ++bin_i) {
         if (bins[bin_i-1]>=bins[bin_i]) return false;
      }
      return true;
   }
}

InDet::InDetTrackSelectionTool::InDetTrackSelectionTool(const std::string& name, const std::string& cutLevel)
  : asg::AsgTool(name)
  , m_acceptInfo( "InDetTrackSelection" )
  , m_cutLevel(cutLevel)
#ifndef XAOD_ANALYSIS
  , m_trackSumTool("Trk::TrackSummaryTool/TrackSummaryTool", this)
  , m_extrapolator("Trk::Extrapolator/Extrapolator", this)
#endif // XAOD_ANALYSIS
{

  // set the cut selection: default is "no cut"
  setCutLevelPrivate(InDet::CutLevel::NoCut);

#ifndef XAOD_STANDALONE
  declareInterface<IInDetTrackSelectionTool>(this);
#endif

  declareProperty("CutLevel", m_cutLevel);

  declareProperty("minPt", m_minPt, "Minimum transverse momentum");
  declareProperty("minP", m_minP, "Minimum momentum");
  declareProperty("maxAbsEta", m_maxAbsEta, "Maximum magnitude of pseudorapidity");
  declareProperty("maxD0", m_maxD0, "Maximum transverse separation");
  declareProperty("maxZ0", m_maxZ0, "Maximum longitudinal separation");
  declareProperty("maxZ0SinTheta", m_maxZ0SinTheta, "Maximum |z0|*sin(theta)");
  declareProperty("maxSigmaD0", m_maxSigmaD0, "Maximum error on d0");
  declareProperty("maxSigmaZ0", m_maxSigmaZ0, "Maximum error on z0");
  declareProperty("maxSigmaZ0SinTheta", m_maxSigmaZ0SinTheta, "Maximum error on z0*sin(theta)");
  declareProperty("maxD0overSigmaD0", m_maxD0overSigmaD0, "Significance cut on |d0|");
  declareProperty("maxZ0overSigmaZ0", m_maxZ0overSigmaZ0, "Significance cut on |z0|");
  declareProperty("maxZ0SinThetaoverSigmaZ0SinTheta",
		  m_maxZ0SinThetaoverSigmaZ0SinTheta,
		  "Significance cut on |z0*sin(theta)|");
  declareProperty("minNInnermostLayerHits", m_minNInnermostLayerHits,
		  "Required hits on the innermost pixel layer");
  declareProperty("minNNextToInnermostLayerHits", m_minNNextToInnermostLayerHits,
		  "Required hits on the next to innermost pixel layer");
  declareProperty("minNBothInnermostLayersHits", m_minNBothInnermostLayersHits,
		  "Required hits on two innermost pixel layers");
  declareProperty("maxNInnermostLayerSharedHits", m_maxNInnermostLayerSharedHits,
		  "Maximum shared hits in innermost pixel layer");
  declareProperty("useMinBiasInnermostLayersCut", m_useMinBiasInnermostLayersCut,
		  "IBL hit if expected, otherwise next layer hit if expected");
  declareProperty("minNSiHits", m_minNSiHits, "Minimum silicon (pixel + SCT) hits");
  declareProperty("maxNSiSharedHits", m_maxNSiSharedHits,
		  "Maximum silicon (pixel + SCT) sensors shared with other track");
  declareProperty("minNSiHitsIfSiSharedHits", m_minNSiHitsIfSiSharedHits,
		  "Minimum number of silicon hits if there are any shared silicon hits");
  declareProperty("maxNSiHoles", m_maxNSiHoles, "Maximum silicon (pixel + SCT) holes");
  declareProperty("minNPixelHits", m_minNPixelHits, "Required pixel hits");
  declareProperty("maxNPixelSharedHits", m_maxNPixelSharedHits,
		  "Maximum pixels shared with other tracks");
  declareProperty("maxNPixelHoles", m_maxNPixelHoles,
		  "Maximum number of missed layers in pixel");
  declareProperty("minEtaForStrictNSiHitsCut", m_minEtaForStrictNSiHitsCut,
		  "Eta cutoff for strict silicon hits cut");
  declareProperty("minNSiHitsAboveEtaCutoff", m_minNSiHitsAboveEtaCutoff,
		  "Minimum silicon hits at large pseudorapidity");
  declareProperty("maxOneSharedModule", m_maxOneSharedModule,
		  "Allow only 1 shared pixel hit or 2 shared SCT hits, not both");
  declareProperty("useEtaDependentMaxChiSq", m_useEtaDependentMaxChiSq,
		  "Whether or not to use the eta-dependent chi squared per degree of freedom cut");
  declareProperty("minNSiHitsPhysical", m_minNSiHitsPhysical,
		  "Minimum physical silicon hits (i.e. dead sensors do not count)");
  declareProperty("minNPixelHitsPhysical", m_minNPixelHitsPhysical,
		  "Minimum physical pixel hits");
  declareProperty("minNSctHitsPhysical", m_minNSctHitsPhysical,
		  "Minimum physical SCT hits");
  declareProperty("minNSctHits", m_minNSctHits, "Minimum SCT hits");
  declareProperty("maxNSctSharedHits", m_maxNSctSharedHits, "Maximum SCT hits shared with other track");
  declareProperty("maxNSctHoles", m_maxNSctHoles, "Maximum SCT holes");
  declareProperty("maxNSctDoubleHoles", m_maxNSctDoubleHoles, "Maximum SCT double holes");
  declareProperty("maxTrtEtaAcceptance", m_maxTrtEtaAcceptance,
		  "Maximum eta that ignores TRT hit cuts");
  declareProperty("maxEtaForTrtHitCuts", m_maxEtaForTrtHitCuts, "Eta above which TRT hit cuts are not applied.");
  declareProperty("minNTrtHits", m_minNTrtHits, "Minimum TRT hits");
  declareProperty("minNTrtHitsPlusOutliers", m_minNTrtHitsPlusOutliers,
		  "Minimum TRT hits including outliers");
  declareProperty("minNTrtHighThresholdHits", m_minNTrtHighThresholdHits,
		  "Minimum high E TRT hits");
  declareProperty("minNTrtHighThresholdHitsPlusOutliers",
		  m_minNTrtHighThresholdHitsPlusOutliers,
		  "Minimum high E TRT hits including outliers");
  declareProperty("maxTrtHighEFraction", m_maxTrtHighEFraction,
		  "Maximum TRT hits that are above high energy threshold");
  declareProperty("maxTrtHighEFractionWithOutliers", m_maxTrtHighEFractionWithOutliers,
		  "Maximum TRT hits that are above high energy threshold including outliers");
  declareProperty("maxTrtOutlierFraction", m_maxTrtOutlierFraction,
		  "Maximum fraction of TRT outliers over TRT hits plus outliers");
  declareProperty("maxChiSq", m_maxChiSq, "Maximum chi squared");
  declareProperty("minProb", m_minProb, "Minimum p(chi^2, Ndof)");
  declareProperty("maxChiSqperNdf", m_maxChiSqperNdf,
		  "Maximum chi squared per degree of freedom");
  declareProperty("minPtForProbCut", m_minPtForProbCut,
		  "Minimum pt for chi-sq probability cut");
  declareProperty("minProbAbovePtCutoff", m_minProbAbovePtCutoff,
		  "Minimum chi-sq probability above a pt cutoff");
  declareProperty("minNUsedHitsdEdx", m_minNUsedHitsdEdx,
		  "Minimum hits used for dEdx");
  declareProperty("minNOverflowHitsdEdx", m_minNOverflowHitsdEdx,
		  "Minimum overflow hits in IBL for dEdx");
  declareProperty("minEProbabilityHT", m_minEProbabilityHT,
		  "Minimum High Threshold electron probability");
  declareProperty("eProbHTonlyForXe", m_eProbHTonlyForXe,
		  "Flag whether to apply the eProbabilityHT cut only when all TRT hits are Xenon");

  declareProperty("vecEtaCutoffsForSiHitsCut", m_vecEtaCutoffsForSiHitsCut,
		  "Minimum eta cutoffs for each Silicon hit cut");
  declareProperty("vecMinNSiHitsAboveEta", m_vecMinNSiHitsAboveEta, "Minimum Silicon hits above each eta cutoff");
  declareProperty("vecEtaCutoffsForPtCut", m_vecEtaCutoffsForPtCut,
                  "Minimum eta cutoffs for each pT cut");
  declareProperty("vecMinPtAboveEta", m_vecMinPtAboveEta, "Minimum transverse momentum above each eta cutoff");


  declareProperty("vecPtCutoffsForSctHitsCut", m_vecPtCutoffsForSctHitsCut,
		  "Minimum pt cutoffs for each SCT hits");
  declareProperty("vecMinNSctHitsAbovePt", m_vecMinNSctHitsAbovePt, "Minimum SCT hits above each pt cutoff");
  declareProperty("useExperimentalInnermostLayersCut", m_useExperimentalInnermostLayersCut, "Use the experimental cut on pixel holes");
#ifndef XAOD_ANALYSIS
  declareProperty("minNSiHitsMod", m_minNSiHitsMod);
  declareProperty("minNSiHitsModTop", m_minNSiHitsModTop);
  declareProperty("minNSiHitsModBottom", m_minNSiHitsModBottom);  

  declareProperty("UseTrkTrackTools", m_initTrkTools);
  declareProperty("TrackSummaryTool", m_trackSumTool);
  declareProperty("Extrapolator", m_extrapolator);
#endif

}

// we must define the destructor in order to use forward-declaration with unique_ptr
InDet::InDetTrackSelectionTool::~InDetTrackSelectionTool() = default;

/// Initializes the selection tool, setting up the cut objects as well as any additional tools it needs.
/// 
/// In athena, this is called automatically after the user has set the properties and so
/// it must set the cut level without overwriting any additional cuts that have been set.
/// 
/// In standalone root, the cut levels can be set in a more or less intuitive way. After setting
/// the cut levels and providing any additional configuration, this initialize() function must
/// be called explicitly.
/// 
StatusCode InDet::InDetTrackSelectionTool::initialize() {
  // Make sure we haven't already been initialized - this would be a sign of improper usage.
  if (m_isInitialized) {
    ATH_MSG_ERROR( "Tool has already been initialized. This is illegitimate." );
    ATH_MSG_ERROR( "This call to initialize() will do nothing." );
    return StatusCode::SUCCESS;
  }

  // Greet the user:
  ATH_MSG_INFO( "Initializing track selection tool." );
  ATH_CHECK( asg::AsgTool::initialize() );

  // if the CutLevel string is set to something recognizable,
  // then do a soft set on the cuts (i.e. not overwriting those already set)
  if (!m_cutLevel.empty()) {
    std::unordered_map<std::string, InDet::CutLevel>::const_iterator it_mapCutLevel = s_mapCutLevel.find(m_cutLevel);
    if ( it_mapCutLevel == s_mapCutLevel.end() ) {
      ATH_MSG_ERROR( "The string \"" << m_cutLevel << "\" is not recognized as a cut level. No cuts will be changed." );
      ATH_MSG_ERROR( "Cut level options are:" );
      for (const auto& opt : s_mapCutLevel) {
	ATH_MSG_ERROR( "\t" << opt.first );
      }
    } else {
      ATH_MSG_INFO( "Cut level set to \"" << it_mapCutLevel->first << "\"." );
      ATH_MSG_INFO( "This will not overwrite other cuts that have been set.");
      setCutLevelPrivate( it_mapCutLevel->second, false );
    }
  }

#ifndef XAOD_ANALYSIS
  if (m_initTrkTools) {
    m_trackSumToolAvailable = false;
    if (!m_trackSumTool.empty()) {
      ATH_CHECK( m_trackSumTool.retrieve() );
      ATH_MSG_INFO( "Track summary tool retrieved." );
      m_trackSumToolAvailable = true;
    }
    ATH_CHECK( m_extrapolator.retrieve() );
    ATH_MSG_INFO( "Retrieved tool " << m_extrapolator );
  } else {
    m_extrapolator.disable();
    m_trackSumTool.disable();
  }
#endif // XAOD_ANALYSIS

  // Need messaging helper for cut functions
  m_msgHelper = std::make_unique<asg::AsgMessaging>(this) ;

  // setup cut functions for xAOD::TrackParticles
  ATH_CHECK( setupCuts<1>(m_trackParticleCuts) );
#ifndef XAOD_ANALYSIS
  // setup cut functions for Trk::Tracks
  ATH_CHECK( setupCuts<0>(m_trkTrackCuts) );
#endif

  for (const auto& cutFamily : m_trackParticleCuts) {
    const std::string& cutFamilyName = cutFamily.first;
    // lock(m_mutex) is not needed because this is inside of non-const initialize method.
    m_numTracksPassedCuts.push_back(0);
    if (m_acceptInfo.addCut( cutFamilyName, "Selection of tracks according to " + cutFamilyName ) < 0) {
      ATH_MSG_ERROR( "Failed to add cut family " << cutFamilyName << " because the TAccept object is full." );
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("Adding cut family " << cutFamilyName);
  }

  m_isInitialized = true;

  return StatusCode::SUCCESS;
}

namespace {
  template <typename T>
  inline T sqr(T a) { return a *a;}
}
template <int VERBOSE, class Trk_Helper>
StatusCode InDet::InDetTrackSelectionTool::setupCuts(std::map< std::string, std::vector< std::function<bool(Trk_Helper helper, asg::AsgMessaging &msgHelper)> > > &trackCuts) {
   // track parameter cuts
  if (m_minPt > 0.) {
     if (VERBOSE>0) ATH_MSG_INFO( "  Minimum Pt: " << m_minPt << " MeV" );
     trackCuts["Pt"].push_back( [minPt = m_minPt](Trk_Helper helper, asg::AsgMessaging &msgHelper) -> bool { return helper.pt(msgHelper) >= minPt; } );
  }
  if (maxDoubleIsSet(m_maxAbsEta)) {
     if (VERBOSE>0) ATH_MSG_INFO( "  Maximum |Eta|: " << m_maxAbsEta );
     trackCuts["Eta"].push_back( [maxAbsEta = m_maxAbsEta](Trk_Helper helper, asg::AsgMessaging &msgHelper) { return std::abs(helper.eta(msgHelper)) <= maxAbsEta; } );
  }
  if (m_minP > 0.) {
     if (VERBOSE>0) ATH_MSG_INFO( "  Minimum P: " << m_minP << " MeV" );
     trackCuts["P"].push_back( [maxInvP = 1./m_minP](Trk_Helper helper, asg::AsgMessaging &msgHelper) { return std::abs(helper.qOverP(msgHelper)) <= maxInvP; } );
  }
  if (maxDoubleIsSet(m_maxD0)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum d0: " << m_maxD0 << " mm" );
    trackCuts["D0"].push_back( [maxD0 = m_maxD0](Trk_Helper helper, asg::AsgMessaging &msgHelper) { return std::abs(helper.d0(msgHelper)) <= maxD0; } );
  }
  if (maxDoubleIsSet(m_maxZ0)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum z0: " << m_maxZ0 << " mm");
    trackCuts["Z0"].push_back( [maxZ0 = m_maxZ0](Trk_Helper helper, asg::AsgMessaging &msgHelper) { return std::abs(helper.z0(msgHelper)) <= maxZ0; } );
  }
  if (maxDoubleIsSet(m_maxZ0SinTheta)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum z0*sin(theta): " << m_maxZ0SinTheta << " mm" );
    trackCuts["Z0SinTheta"].push_back([maxZ0SinTheta = m_maxZ0SinTheta](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return std::abs( helper.z0(msgHelper) * std::sin(helper.theta(msgHelper))) <= maxZ0SinTheta;
    });
  }
  if (maxDoubleIsSet(m_maxSigmaD0)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum uncertainty on d0: " << m_maxSigmaD0 << " mm" );
    trackCuts["D0"].push_back([maxSigmaD0Squared = sqr(m_maxSigmaD0)](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return getDefiningParametersCov(helper,msgHelper, InDetAccessor::d0,InDetAccessor::d0) <= maxSigmaD0Squared;
    });
  }
  if (maxDoubleIsSet(m_maxSigmaZ0)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum uncertainty on z0: " << m_maxSigmaZ0 << " mm" );
    trackCuts["Z0"].push_back([maxSigmaZ0Squared = sqr(m_maxSigmaZ0)](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return getDefiningParametersCov(helper,msgHelper, InDetAccessor::z0,InDetAccessor::z0) <= maxSigmaZ0Squared;
    });
  }
  if (maxDoubleIsSet(m_maxSigmaZ0SinTheta)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum uncertainty on z0*sin(theta): "
                                 << m_maxSigmaZ0SinTheta << " mm" );
    trackCuts["Z0SinTheta"].push_back([maxSigmaZ0SinThetaSquared = sqr(m_maxSigmaZ0SinTheta)](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       double theta =    helper.theta(msgHelper);
       double sinTheta = std::sin(theta);
       double cosTheta = std::cos(theta);
       double z0 =       helper.z0(msgHelper);

       return (  sqr(z0)*sqr(cosTheta)     * getDefiningParametersCov(helper,msgHelper, InDetAccessor::theta,InDetAccessor::theta)
               + 2*z0   *sinTheta*cosTheta * getDefiningParametersCov(helper,msgHelper, InDetAccessor::theta,InDetAccessor::z0)
               + sqr(sinTheta)             * getDefiningParametersCov(helper,msgHelper, InDetAccessor::z0,   InDetAccessor::z0) ) <= maxSigmaZ0SinThetaSquared;
    });
  }
  if (maxDoubleIsSet(m_maxD0overSigmaD0)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum significance on d0: " << m_maxD0overSigmaD0 );
    trackCuts["D0"].push_back([maxD0overSigmaD0Squared = sqr(m_maxD0overSigmaD0)](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return sqr(helper.d0(msgHelper)) <= maxD0overSigmaD0Squared * getDefiningParametersCov(helper,msgHelper, InDetAccessor::d0,InDetAccessor::d0);
    });
  }
  if (maxDoubleIsSet(m_maxZ0overSigmaZ0)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum significance on z0: " << m_maxZ0overSigmaZ0 );
    trackCuts["Z0"].push_back([maxZ0overSigmaZ0Squared = sqr(m_maxZ0overSigmaZ0)](Trk_Helper helper, asg::AsgMessaging &msgHelper) -> bool {
       return sqr(helper.z0(msgHelper)) <= maxZ0overSigmaZ0Squared * getDefiningParametersCov(helper,msgHelper, InDetAccessor::z0,InDetAccessor::z0);
    });
  }
  if (maxDoubleIsSet(m_maxZ0SinThetaoverSigmaZ0SinTheta)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum significance on z0*sin(theta): "
                                 << m_maxZ0SinThetaoverSigmaZ0SinTheta );
    trackCuts["Z0SinTheta"].push_back([maxZ0SinThetaoverSigmaZ0SinThetaSquared = sqr(m_maxZ0SinThetaoverSigmaZ0SinTheta)](Trk_Helper helper, asg::AsgMessaging &msgHelper) {

       double theta =    helper.theta(msgHelper);
       double sinTheta = std::sin(theta);
       double cosTheta = std::cos(theta);
       double z0 =       helper.z0(msgHelper);

       return sqr(z0*sinTheta) <=
          maxZ0SinThetaoverSigmaZ0SinThetaSquared * (  sqr(z0)*sqr(cosTheta)        * getDefiningParametersCov(helper,msgHelper, InDetAccessor::theta,InDetAccessor::theta)
                                                     + 2*z0   *sinTheta*cosTheta    * getDefiningParametersCov(helper,msgHelper, InDetAccessor::theta,InDetAccessor::z0)
                                                     + sqr(sinTheta)                * getDefiningParametersCov(helper,msgHelper, InDetAccessor::z0,   InDetAccessor::z0));
    });
  }

  // hit cuts
  if (m_minNInnermostLayerHits > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum hits from innermost pixel layer: " << m_minNInnermostLayerHits );
    if (VERBOSE>0) ATH_MSG_INFO( "    (Track will pass if no hit is expected.)" );
    trackCuts["InnermostLayersHits"].push_back([minNInnermostLayerHits = m_minNInnermostLayerHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return (   getSummary(helper, msgHelper, xAOD::numberOfInnermostPixelLayerHits) >= minNInnermostLayerHits
               || getSummary(helper, msgHelper, xAOD::expectInnermostPixelLayerHit)    == 0);
    });
  }
  if (m_minNNextToInnermostLayerHits > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum hits from next to innermost pixel layer: " << m_minNNextToInnermostLayerHits );
    if (VERBOSE>0) ATH_MSG_INFO( "    (Track will pass if no hit is expected.)" );
    trackCuts["InnermostLayersHits"].push_back([minNNextToInnermostLayerHits = m_minNNextToInnermostLayerHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return (   getSummary(helper, msgHelper, xAOD::numberOfNextToInnermostPixelLayerHits) >= minNNextToInnermostLayerHits
               || getSummary(helper, msgHelper, xAOD::expectNextToInnermostPixelLayerHit)    == 0);
    });
  }
  if (m_minNBothInnermostLayersHits > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum hits from both innermost pixel layers: " << m_minNBothInnermostLayersHits );
    if (VERBOSE>0) ATH_MSG_INFO( "    (If a layer has no hits but one is not expected, the" );
    if (VERBOSE>0) ATH_MSG_INFO( "     number of hits in that layer will be taken to be 1.)" );
    if (m_minNBothInnermostLayersHits > 2) {
      if (VERBOSE>0) ATH_MSG_WARNING( "A value of minNBothInnermostLayersHits above 2 does not make sense." );
      if (VERBOSE>0) ATH_MSG_WARNING( "  Use 1 for \"or\" or 2 for \"and\"." );
    }
    trackCuts["InnermostLayersHits"].push_back([minNBothInnermostLayersHits = m_minNBothInnermostLayersHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return (   std::max(  getSummary(helper, msgHelper, xAOD::numberOfInnermostPixelLayerHits),
                             static_cast<uint8_t>( !getSummary(helper, msgHelper, xAOD::expectInnermostPixelLayerHit) ))
                 +std::max(  getSummary(helper, msgHelper, xAOD::numberOfNextToInnermostPixelLayerHits),
                             static_cast<uint8_t>( !getSummary(helper, msgHelper, xAOD::expectNextToInnermostPixelLayerHit )))
                  >= minNBothInnermostLayersHits);
    });
  }
  if (m_useMinBiasInnermostLayersCut > 0 ) { // less than zero indicates this cut is manually turned off
    if (VERBOSE>0) ATH_MSG_INFO( "  An innermost layer hit is required if expected, otherwise" );
    if (VERBOSE>0) ATH_MSG_INFO( "    a next-to-innermost layer hit is required if it is expected." );
    trackCuts["InnermostLayersHits"].push_back([](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       uint8_t expected_innermost_pixel_layer_hit = getSummary(helper, msgHelper, xAOD::expectInnermostPixelLayerHit);
       return    (expected_innermost_pixel_layer_hit >  0  && getSummary(helper, msgHelper, xAOD::numberOfInnermostPixelLayerHits)>=1)
              || (expected_innermost_pixel_layer_hit == 0  && (   getSummary(helper, msgHelper, xAOD::expectNextToInnermostPixelLayerHit)==0
                                                               || getSummary(helper, msgHelper, xAOD::numberOfNextToInnermostPixelLayerHits)>=1));
    });
  }
  if (maxIntIsSet(m_maxNInnermostLayerSharedHits)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum shared hits in innermost pixel layer: " << m_maxNInnermostLayerSharedHits );
    trackCuts["InnermostLayersHits"].push_back([maxNInnermostLayerSharedHits = m_maxNInnermostLayerSharedHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return getSummary(helper, msgHelper, xAOD::numberOfInnermostPixelLayerSharedHits) <= maxNInnermostLayerSharedHits;
    });
  }
  if (m_minNPixelHits > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum pixel hits: " << m_minNPixelHits );
    trackCuts["PixelHits"].push_back( [minNPixelHits = m_minNPixelHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummarySum<2,Trk_Helper>(helper, msgHelper, {xAOD::numberOfPixelHits,xAOD::numberOfPixelDeadSensors}) >= minNPixelHits;
    });
  }
  if (m_minNPixelHitsPhysical > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum physical pixel hits (i.e. dead sensors do not count): "
                                 << m_minNPixelHitsPhysical );
    trackCuts["PixelHits"].push_back([minNPixelHitsPhysical = m_minNPixelHitsPhysical](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummary(helper, msgHelper, xAOD::numberOfPixelHits) >= minNPixelHitsPhysical;
    });
  }
  if (maxIntIsSet(m_maxNPixelHoles)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum pixel holes: " << m_maxNPixelHoles );
    trackCuts["PixelHits"].push_back([maxNPixelHoles = m_maxNPixelHoles](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummary(helper, msgHelper, xAOD::numberOfPixelHoles) <= maxNPixelHoles;
    });
  }
  if (maxIntIsSet(m_maxNPixelSharedHits)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum pixel shared hits: " << m_maxNPixelSharedHits );
    trackCuts["PixelHits"].push_back([maxNPixelSharedHits = m_maxNPixelSharedHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummary(helper, msgHelper, xAOD::numberOfPixelSharedHits) <= maxNPixelSharedHits;
    });
  }
  if (m_minNSctHits > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum SCT hits: " << m_minNSctHits );
    trackCuts["SctHits"].push_back( [minNSctHits = m_minNSctHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummarySum<2,Trk_Helper>(helper, msgHelper, {xAOD::numberOfSCTHits, xAOD::numberOfSCTDeadSensors}) >= minNSctHits;
    });
  }
  if (m_minNSctHitsPhysical > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum physical SCT hits (i.e. dead sensors do not count): "
                                 << m_minNSctHitsPhysical );
    trackCuts["SctHits"].push_back([minNSctHitsPhysical = m_minNSctHitsPhysical](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummary(helper, msgHelper, xAOD::numberOfSCTHits) >= minNSctHitsPhysical;
    });
  }
  if (maxIntIsSet(m_maxNSctHoles)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum SCT holes: " << m_maxNSctHoles );
    trackCuts["SctHits"].push_back([maxNSctHoles = m_maxNSctHoles](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummary(helper, msgHelper, xAOD::numberOfSCTHoles) <= maxNSctHoles;
    });
  }
  if (maxIntIsSet(m_maxNSctSharedHits)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum SCT shared hits: " << m_maxNSctSharedHits );
    trackCuts["SctHits"].push_back([maxNSctSharedHits = m_maxNSctSharedHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummary(helper, msgHelper, xAOD::numberOfSCTSharedHits) <= maxNSctSharedHits;
    });
  }
  if (maxIntIsSet(m_maxNSctDoubleHoles)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum SCT double holes: " << m_maxNSctDoubleHoles );
    trackCuts["SctHits"].push_back([maxNSctDoubleHoles = m_maxNSctDoubleHoles](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummary(helper, msgHelper, xAOD::numberOfSCTDoubleHoles) <= maxNSctDoubleHoles ;
    });
  }
  if (m_minNSiHits > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum silicon (pixel + SCT) hits: " << m_minNSiHits );
    trackCuts["SiHits"].push_back([minNSiHits = m_minNSiHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return getSummarySum<4,Trk_Helper>(helper, msgHelper, {xAOD::numberOfPixelHits,
                                                              xAOD::numberOfSCTHits,
                                                              xAOD::numberOfPixelDeadSensors,
                                                              xAOD::numberOfSCTDeadSensors} )    >= minNSiHits ;
    });
  }
  if (m_minNSiHitsPhysical > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum physical silicon hits (i.e. dead sensors do not count): "
                                 << m_minNSiHitsPhysical );
    trackCuts["SiHits"].push_back([minNSiHitsPhysical = m_minNSiHitsPhysical](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummarySum<2,Trk_Helper>(helper, msgHelper,{xAOD::numberOfPixelHits, xAOD::numberOfSCTHits} )   >= minNSiHitsPhysical ;
    });
  }
  if (maxIntIsSet(m_maxNSiHoles)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum silicon holes: " << m_maxNSiHoles );
    trackCuts["SiHits"].push_back([maxNSiHoles = m_maxNSiHoles](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummarySum<2,Trk_Helper>(helper, msgHelper,{xAOD::numberOfPixelHoles, xAOD::numberOfSCTHoles} )   <= maxNSiHoles ;
    });
  }
  if (maxIntIsSet(m_maxNSiSharedHits)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum silicon shared hits: " << m_maxNSiSharedHits );
    trackCuts["SiHits"].push_back([maxNSiSharedHits = m_maxNSiSharedHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummarySum<2,Trk_Helper>(helper, msgHelper,{xAOD::numberOfPixelSharedHits, xAOD::numberOfSCTSharedHits} )   <= maxNSiSharedHits ;
    });
  }
  if (m_maxOneSharedModule) {
    if (VERBOSE>0) ATH_MSG_INFO( "  No more than one shared module:" );
    if (VERBOSE>0) ATH_MSG_INFO( "    i.e. max 1 shared pixel hit or" );
    if (VERBOSE>0) ATH_MSG_INFO( "    2 shared SCT hits, and not both." );
    trackCuts["SiHits"].push_back([](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummary(helper, msgHelper,xAOD::numberOfPixelSharedHits )
              + getSummary(helper, msgHelper,xAOD::numberOfSCTSharedHits )/2  <= 1  ;
    });
  }
  if (m_minNSiHitsIfSiSharedHits > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum silicon hits if the track has shared hits: "
                                 << m_minNSiHitsIfSiSharedHits );
    trackCuts["SiHits"].push_back([minNSiHitsIfSiSharedHits = m_minNSiHitsIfSiSharedHits](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return    getSummarySum<2,Trk_Helper>(helper, msgHelper,{xAOD::numberOfPixelSharedHits, xAOD::numberOfSCTSharedHits} ) == 0
              || getSummarySum<4,Trk_Helper>(helper, msgHelper,{xAOD::numberOfPixelHits,
                                                                xAOD::numberOfSCTHits,
                                                                xAOD::numberOfPixelDeadSensors,
                                                                xAOD::numberOfSCTDeadSensors}) >=minNSiHitsIfSiSharedHits;
    });
  }
  if (maxDoubleIsSet(m_minEtaForStrictNSiHitsCut)
      && m_minNSiHitsAboveEtaCutoff > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Require " << m_minNSiHitsAboveEtaCutoff
                                 << " silicon hits above eta = " << m_minEtaForStrictNSiHitsCut );
    trackCuts["SiHits"].push_back([minEtaForStrictNSiHitsCut = m_minEtaForStrictNSiHitsCut,
                                   minNSiHitsAboveEtaCutoff  = m_minNSiHitsAboveEtaCutoff](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return    std::abs(helper.eta(msgHelper)) <= minEtaForStrictNSiHitsCut
              || getSummarySum<4,Trk_Helper>(helper, msgHelper,{xAOD::numberOfPixelHits,
                                                                xAOD::numberOfSCTHits,
                                                                xAOD::numberOfPixelDeadSensors,
                                                                xAOD::numberOfSCTDeadSensors})   >= minNSiHitsAboveEtaCutoff;
    });
  }
  if (m_useExperimentalInnermostLayersCut) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Zero pixel holes allowed, except one pix hole is allowed if there is a physical IBL hit and a BLayer hit is expected but there is no BLayer hit." );
    trackCuts["PixHits"].push_back([](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       uint8_t pixel_holes = getSummary(helper, msgHelper,xAOD::numberOfPixelHoles);
       return  pixel_holes == 0 || ( pixel_holes<=1 && getSummary(helper, msgHelper,xAOD::numberOfInnermostPixelLayerHits)       >= 1
                                                    && getSummary(helper, msgHelper,xAOD::expectNextToInnermostPixelLayerHit)    > 0
                                                    && getSummary(helper, msgHelper,xAOD::numberOfNextToInnermostPixelLayerHits) == 0 );
    });
  }
#ifndef XAOD_ANALYSIS
  if (m_minNSiHitsMod > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum modified Si hits (2*pix + sct) (does not include dead sensors)= "
                                 << m_minNSiHitsMod );
    trackCuts["SiHits"].push_back([minNSiHitsMod = m_minNSiHitsMod](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return   getSummarySum<3,Trk_Helper>(helper, msgHelper,{xAOD::numberOfPixelHits,
                                                               xAOD::numberOfPixelHits, // pixel hits count twice in this definition
                                                               xAOD::numberOfSCTHits})   >= minNSiHitsMod;
    });
  }
  if (m_minNSiHitsModTop > 0 || m_minNSiHitsModBottom > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum modified Si hits in top half = "
                                 << m_minNSiHitsModTop );
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum modified Si hits in bottom half = "
                                 << m_minNSiHitsModBottom );
    trackCuts["SiHits"].push_back([minNSiHitsModTop    = m_minNSiHitsModTop,
                                   minNSiHitsModBottom = m_minNSiHitsModBottom](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       auto [top,bottom] = getSiHitsTopBottom(helper, msgHelper);
       return top  >= minNSiHitsModTop && bottom >= minNSiHitsModBottom;
    });
  }
#endif
  if (m_maxEtaForTrtHitCuts > 0. && m_maxTrtEtaAcceptance < m_maxEtaForTrtHitCuts) {
    ATH_MSG_INFO( "  -- TRT hit cuts applied above eta = " << m_maxTrtEtaAcceptance
		  << " and below eta = " << m_maxEtaForTrtHitCuts << " --" );
    if (m_minNTrtHits > 0) {
      ATH_MSG_INFO( "    Minimum TRT hits outside eta acceptance: " << m_minNTrtHits );
      trackCuts["TrtHits"].push_back( MinTRTHitsCut<Trk_Helper,1>( m_maxTrtEtaAcceptance, m_maxEtaForTrtHitCuts, m_minNTrtHits,
                                                                   {xAOD::numberOfTRTHits} ));
    }
    if (m_minNTrtHitsPlusOutliers > 0) {
      ATH_MSG_INFO( "    Minimum TRT hits outside eta acceptance including outliers: " << m_minNTrtHitsPlusOutliers );
      trackCuts["TrtHits"].push_back( MinTRTHitsCut<Trk_Helper,2>( m_maxTrtEtaAcceptance,m_maxEtaForTrtHitCuts, m_minNTrtHitsPlusOutliers,
                                                                   {xAOD::numberOfTRTHits, xAOD::numberOfTRTOutliers} ));
    }
    if (m_minNTrtHighThresholdHits > 0) {
      ATH_MSG_INFO( "    Minimum TRT hits outside eta acceptance above high energy threshold: "
		    << m_minNTrtHighThresholdHits );
      trackCuts["TrtHits"].push_back( MinTRTHitsCut<Trk_Helper,1>( m_maxTrtEtaAcceptance,m_maxEtaForTrtHitCuts, m_minNTrtHighThresholdHits,
                                                                   {xAOD::numberOfTRTHighThresholdHits} ));
    }
    if (m_minNTrtHighThresholdHitsPlusOutliers > 0) {
      ATH_MSG_INFO( "    Minimum TRT hits outside eta acceptance above high energy threshold including outliers: "
		    << m_minNTrtHighThresholdHitsPlusOutliers );
      trackCuts["TrtHits"].push_back( MinTRTHitsCut<Trk_Helper,2>( m_maxTrtEtaAcceptance,m_maxEtaForTrtHitCuts, m_minNTrtHighThresholdHitsPlusOutliers,
                                                                   {xAOD::numberOfTRTHighThresholdHits, xAOD::numberOfTRTHighThresholdOutliers} ));
    }
    if (maxDoubleIsSet(m_maxTrtHighEFraction)) { // I think this condition could be instead that it is between 0 and 1
      ATH_MSG_INFO( "    Maximum ratio of high threshold to regular TRT hits outside eta acceptance: "
		    << m_maxTrtHighEFraction);
      trackCuts["TrtHits"].push_back([maxTrtEtaAcceptance = m_maxTrtEtaAcceptance,
                                      maxEtaForTrtHitCuts = m_maxEtaForTrtHitCuts,
                                      maxTrtHighEFraction = m_maxTrtHighEFraction](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
         double absEta = std::abs( helper.eta( msgHelper) );
         return      (absEta <= maxTrtEtaAcceptance || absEta > maxEtaForTrtHitCuts)
                ||                            getSummary(helper, msgHelper,xAOD::numberOfTRTHighThresholdHits )
                     <= maxTrtHighEFraction * getSummary(helper, msgHelper,xAOD::numberOfTRTHits );
      });
    }
    if (maxDoubleIsSet(m_maxTrtHighEFractionWithOutliers)) {
      ATH_MSG_INFO( "    Maximum ratio of high threshold to regular TRT hits above eta acceptance including outliers: "
		    << m_maxTrtHighEFractionWithOutliers);
      trackCuts["TrtHits"].push_back([maxTrtEtaAcceptance             = m_maxTrtEtaAcceptance,
                                      maxEtaForTrtHitCuts             = m_maxEtaForTrtHitCuts,
                                      maxTrtHighEFractionWithOutliers = m_maxTrtHighEFractionWithOutliers](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
         double absEta = std::abs( helper.eta( msgHelper) );
         return      (absEta <= maxTrtEtaAcceptance || absEta > maxEtaForTrtHitCuts)
            ||                                      (  getSummarySum<2,Trk_Helper>(helper, msgHelper,{xAOD::numberOfTRTHighThresholdHits, xAOD::numberOfTRTHighThresholdOutliers} ))
               <= maxTrtHighEFractionWithOutliers * (  getSummarySum<2,Trk_Helper>(helper, msgHelper,{xAOD::numberOfTRTHits,              xAOD::numberOfTRTOutliers} ));
      });
    }
    if (m_maxTrtOutlierFraction < 1. && m_maxTrtOutlierFraction >= 0.) {
      ATH_MSG_INFO( "    Maximum fraction of TRT hits that are outliers: " << m_maxTrtOutlierFraction );
      trackCuts["TrtHits"].push_back([maxTrtEtaAcceptance   = m_maxTrtEtaAcceptance,
                                      maxEtaForTrtHitCuts   = m_maxEtaForTrtHitCuts,
                                      maxTrtOutlierFraction = m_maxTrtOutlierFraction](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
         double absEta = std::abs( helper.eta( msgHelper) );
         uint8_t trt_outliers = getSummary(helper, msgHelper,xAOD::numberOfTRTOutliers );
         return     ( absEta <= maxTrtEtaAcceptance || absEta > maxEtaForTrtHitCuts)
                 || trt_outliers <= maxTrtOutlierFraction  * (  trt_outliers + getSummary(helper, msgHelper,xAOD::numberOfTRTHits) );
      });
    }
  }

  // fit quality cuts
  if (m_useEtaDependentMaxChiSq) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Using pre-defined eta-dependent maximum chi squared (no longer recommended)." );
    trackCuts["FitQuality"].push_back([](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       double eta = helper.eta(msgHelper);
       double fit_chi_square = getFitChiSquare(helper,msgHelper);
       double fit_ndof = getFitNDoF(helper,msgHelper);
       if (std::abs( eta) < 1.9) {
          return fit_chi_square <= fit_ndof * ( 4.4 + 0.32*sqr(eta) );
       }
       else {
          return fit_chi_square <= fit_ndof * ( 26.9 - 19.6978*std::abs(eta) + 4.4534*sqr(eta) );
       }
    });
  }
  if (maxDoubleIsSet(m_maxChiSq)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum chi squared: " << m_maxChiSq );
    trackCuts["FitQuality"].push_back([maxChiSq = m_maxChiSq](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return getFitChiSquare(helper,msgHelper) <= maxChiSq;
    });
  }
  if (maxDoubleIsSet(m_maxChiSqperNdf)) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Maximum chi squared per degree of freedom: " << m_maxChiSqperNdf );
    trackCuts["FitQuality"].push_back([maxChiSqperNdf = m_maxChiSqperNdf](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return getFitChiSquare(helper,msgHelper)  <= maxChiSqperNdf * getFitNDoF(helper,msgHelper);
    });
  }
  if (m_minProb > 0.) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum chi squared probability: " << m_minProb );
    trackCuts["FitQuality"].push_back([minProb = m_minProb](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return TMath::Prob( getFitChiSquare(helper,msgHelper),getFitNDoF(helper,msgHelper))  >= minProb;
    });
  }
  if (maxDoubleIsSet(m_minPtForProbCut) && m_minProbAbovePtCutoff > 0.) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum chi-sq probability of " << m_minProbAbovePtCutoff
                                 << " above pt of " << m_minPtForProbCut*1e-3 << " GeV." );
    trackCuts["FitQuality"].push_back([minPtForProbCut      = m_minPtForProbCut,
                                       minProbAbovePtCutoff = m_minProbAbovePtCutoff](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return    helper.pt(msgHelper) <= minPtForProbCut
              || TMath::Prob( getFitChiSquare(helper,msgHelper),getFitNDoF(helper,msgHelper))  >= minProbAbovePtCutoff;
    });
  }

  // dE/dx cuts
  if (m_minNUsedHitsdEdx > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum used hits for dEdx: " << m_minNUsedHitsdEdx );
    trackCuts["dEdxHits"].push_back([minNUsedHitsdEdx = m_minNUsedHitsdEdx](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return getNumberOfUsedHitsdEdx(helper,msgHelper) >= minNUsedHitsdEdx;
    });
  }
  if (m_minNOverflowHitsdEdx > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum IBL overflow hits for dEdx: " << m_minNOverflowHitsdEdx );
    trackCuts["dEdxHits"].push_back([minNOverflowHitsdEdx = m_minNOverflowHitsdEdx](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       return getNumberOfIBLOverflowsdEdx(helper,msgHelper) >= minNOverflowHitsdEdx;
    });
  }
  if (m_minEProbabilityHT > 0) {
    if (VERBOSE>0) ATH_MSG_INFO( "  Minimum high threshold electron probability: " << m_minEProbabilityHT );
    if (m_eProbHTonlyForXe) {
      if (VERBOSE>0) ATH_MSG_INFO( "    (only applied on tracks where all TRT hits are Xenon)" );
      trackCuts["eProbHT"].push_back([minEProbabilityHT = m_minEProbabilityHT](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
         return     getSummarySum<2,Trk_Helper>(helper, msgHelper, {xAOD::numberOfTRTHits, xAOD::numberOfTRTOutliers})
                  > getSummary(helper, msgHelper, xAOD::numberOfTRTXenonHits)
               || getEProbabilityHT(helper,msgHelper) >= minEProbabilityHT;
      });
    } else {
       trackCuts["eProbHT"].push_back([minEProbabilityHT = m_minEProbabilityHT](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
          return  getEProbabilityHT(helper,msgHelper) >= minEProbabilityHT;
       });
    }
  }

  if (!m_vecEtaCutoffsForSiHitsCut.empty() || !m_vecMinNSiHitsAboveEta.empty()) {
    auto cutSize = m_vecEtaCutoffsForSiHitsCut.size();
    if (cutSize != m_vecMinNSiHitsAboveEta.size()) {
       if (VERBOSE>0) ATH_MSG_ERROR( "Eta cutoffs and Silicon hit cuts must be vectors of the same length." );
       return StatusCode::FAILURE;
    }
    if (VERBOSE>0) {
       for (size_t i_cut=0; i_cut<cutSize-1; ++i_cut) {
          ATH_MSG_INFO( "  for " << m_vecEtaCutoffsForSiHitsCut.at(i_cut)
                        << " < eta < " << m_vecEtaCutoffsForSiHitsCut.at(i_cut+1)
                        << " ,Silicon hits  >= " << m_vecMinNSiHitsAboveEta.at(i_cut) );
       }
    }
    if (!checkOrder(m_vecEtaCutoffsForSiHitsCut)) {
       if (VERBOSE>0) ATH_MSG_ERROR( "Eta values not in ascending order." );
       return StatusCode::FAILURE;
    }
    if (VERBOSE>0) ATH_MSG_INFO( "  for eta > " << m_vecEtaCutoffsForSiHitsCut.at(cutSize-1)
                               << " ,Silicon hits >= " << m_vecMinNSiHitsAboveEta.at(cutSize-1) );

    trackCuts["SiHits"].push_back([p_vecEtaCutoffsForSiHitsCut = &std::as_const(m_vecEtaCutoffsForSiHitsCut),
                                   p_vecMinNSiHitsAboveEta     = &std::as_const(m_vecMinNSiHitsAboveEta)](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       double abs_eta = std::abs(helper.eta(msgHelper));
       unsigned int bin_i = findBin(*p_vecEtaCutoffsForSiHitsCut, abs_eta);
       return    bin_i >= p_vecMinNSiHitsAboveEta->size()
              || abs_eta>5.0
              || getSummarySum<4,Trk_Helper>(helper, msgHelper,{xAOD::numberOfPixelHits,
                                                                xAOD::numberOfSCTHits,
                                                                xAOD::numberOfPixelDeadSensors,
                                                                xAOD::numberOfSCTDeadSensors})   >= (*p_vecMinNSiHitsAboveEta)[bin_i];
    });
  }

  if (!m_vecEtaCutoffsForPtCut.empty() || !m_vecMinPtAboveEta.empty()) {
    auto cutSize = m_vecEtaCutoffsForPtCut.size();
    if (cutSize != m_vecMinPtAboveEta.size()) {
       if (VERBOSE>0) ATH_MSG_ERROR( "Eta cutoffs and pT cuts must be vectors of the same length." );
      return StatusCode::FAILURE;
    }
    if (VERBOSE>0) {
       for (size_t i_cut=0; i_cut<cutSize-1; ++i_cut) {
          ATH_MSG_INFO( "  for " << m_vecEtaCutoffsForPtCut.at(i_cut)
                        << " < eta < " << m_vecEtaCutoffsForPtCut.at(i_cut+1)
                        << " ,transverse momentum >= " << m_vecMinPtAboveEta.at(i_cut) );
       }
    }
    if (!checkOrder(m_vecEtaCutoffsForPtCut)) {
       if (VERBOSE>0) ATH_MSG_ERROR( "Eta values not in ascending order." );
       return StatusCode::FAILURE;
    }
    if (VERBOSE>0) ATH_MSG_INFO( "  for eta > " << m_vecEtaCutoffsForPtCut.at(cutSize-1)
                               << " ,transverse momentum >= " << m_vecMinPtAboveEta.at(cutSize-1) );
    trackCuts["Pt"].push_back([p_vecEtaCutoffsForPtCut = &std::as_const(m_vecEtaCutoffsForPtCut),
                               p_vecMinPtAboveEta = &std::as_const(m_vecMinPtAboveEta)](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       double abs_eta = std::abs(helper.eta(msgHelper));
       unsigned int bin_i = findBin(*p_vecEtaCutoffsForPtCut, abs_eta);
       return  bin_i >= p_vecMinPtAboveEta->size() || abs_eta > 5.0 || helper.pt(msgHelper)  >= (*p_vecMinPtAboveEta)[bin_i];
    });
  }



  if (!m_vecPtCutoffsForSctHitsCut.empty() || !m_vecMinNSctHitsAbovePt.empty()) {
    auto cutSize = m_vecPtCutoffsForSctHitsCut.size();
    if (cutSize != m_vecMinNSctHitsAbovePt.size()) {
       if (VERBOSE>0) ATH_MSG_ERROR( "Pt cutoffs and SCT hit cuts must be vectors of the same length." );
       return StatusCode::FAILURE;
    }
    if (VERBOSE>0) {
       for (size_t i_cut=0; i_cut<cutSize-1; ++i_cut) {
          ATH_MSG_INFO( "  for " << m_vecPtCutoffsForSctHitsCut.at(i_cut)
                        << " < pt < " << m_vecPtCutoffsForSctHitsCut.at(i_cut+1)
                        << " MeV,\tSCT hits >= " << m_vecMinNSctHitsAbovePt.at(i_cut) );
       }
    }
    if (!checkOrder(m_vecPtCutoffsForSctHitsCut)) {
       if (VERBOSE>0) ATH_MSG_ERROR( "Pt values not in ascending order." );
       return StatusCode::FAILURE;
    }
    if (VERBOSE>0) ATH_MSG_INFO( "  for pt > " << m_vecPtCutoffsForSctHitsCut.at(cutSize-1)
                               << " MeV,\t\tSCT hits >= " << m_vecMinNSctHitsAbovePt.at(cutSize-1) );
    trackCuts["SctHits"].push_back([p_vecPtCutoffsForSctHitsCut = &std::as_const(m_vecPtCutoffsForSctHitsCut),
                                    p_vecMinNSctHitsAbovePt     = &std::as_const(m_vecMinNSctHitsAbovePt)](Trk_Helper helper, asg::AsgMessaging &msgHelper) {
       double pt = helper.pt(msgHelper);
       unsigned int bin_i = findBin(*p_vecPtCutoffsForSctHitsCut, pt);
       return     bin_i >= p_vecPtCutoffsForSctHitsCut->size()
               || getSummarySum<2,Trk_Helper>(helper, msgHelper,{xAOD::numberOfSCTHits,
                                                                 xAOD::numberOfSCTDeadSensors}) >= (*p_vecMinNSctHitsAbovePt)[bin_i];
    });
  }
  return StatusCode::SUCCESS;
}

template <class Trk_Helper>
asg::AcceptData InDet::InDetTrackSelectionTool::accept(Trk_Helper helper,
                                                       const std::map< std::string, std::vector< std::function<bool(Trk_Helper helper, asg::AsgMessaging &msgHelper)> > > &trackCuts) const {
  if (!m_isInitialized) {
    if (!m_warnInit) {
      ATH_MSG_WARNING( "Tool is not initialized! Calling accept() will not be very helpful." );
      m_warnInit = true;
    }
  }

  asg::AcceptData acceptData(&m_acceptInfo);
  bool passAll = true;

  // loop over all cuts
  UShort_t cutFamilyIndex = 0;
  for ( const auto& cutFamily : trackCuts ) {
    bool pass = true;

    for ( const auto& cut : cutFamily.second ) {
      if (! cut(helper, *m_msgHelper) ) {
	pass = false;
	break;
      }
    }

    // @TODO really always run through all cuts even if passed is false ?
    passAll &= pass;
    acceptData.setCutResult( cutFamilyIndex, pass );
    cutFamilyIndex++;
  }

  {
     // lock access to m_numTracksPassedCuts
     std::lock_guard<std::mutex> lock(m_mutex);
     for (unsigned int idx=0; idx < trackCuts.size(); ++idx) {
        assert(idx<m_numTracksPassedCuts.size());
        m_numTracksPassedCuts[idx] += acceptData.getCutResult(idx);
     }
  }
  if (passAll) m_numTracksPassed++;
  m_numTracksProcessed++;

  return acceptData;

}


StatusCode InDet::InDetTrackSelectionTool::finalize()
{
  ATH_MSG_INFO("Finalizing track selection tool.");
  if (!m_isInitialized) {
    ATH_MSG_ERROR( "You are attempting to finalize a tool that has not been initialized()." );
  }
  if (m_numTracksProcessed == 0) {
    ATH_MSG_INFO( "No tracks processed in selection tool." );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_INFO( m_numTracksPassed << " / " << m_numTracksProcessed << " = "
		<< m_numTracksPassed*100./m_numTracksProcessed << "% passed all cuts." );
  for (const auto& cutFamily : m_trackParticleCuts) {
    // lock(m_mutex) is not needed because this is inside of non-const finalize method.
    ULong64_t numPassed = m_numTracksPassedCuts.at(m_acceptInfo.getCutPosition(cutFamily.first));
    ATH_MSG_INFO( numPassed << " = " << numPassed*100./m_numTracksProcessed << "% passed "
		  << cutFamily.first << " cut." );
  }

  return StatusCode::SUCCESS;
}


/// Accessor function to the tool's Root::TAccept object, which stores the result of
/// the last accept(...) call. The TAccept object itself is expensive to copy,
/// and so should be accessed by const reference.
/// 
const asg::AcceptInfo& InDet::InDetTrackSelectionTool::getAcceptInfo() const
{  
  return m_acceptInfo;
}


/// Function that returns the result of the cut through the xAOD::IParticle interface.
/// 
/// This is included to implement the AsgSelectionTool interface. It is not the recommended
/// way of using the tool. If the IParticle is an xAOD::TrackParticle, then it will return
/// the result of the explicitly xAOD::TrackParticle version below, with no vertex provided.
/// 
/// @param p An xAOD::IParticle pointer. If it is of type xAOD::TrackParticle, then it
///          will be cast as such and funnelled into the corresponding function. If it
///          is of some different type, then the particle will not pass.
/// 
asg::AcceptData
InDet::InDetTrackSelectionTool::accept( const xAOD::IParticle* p ) const
{
  
  asg::AcceptData acceptData(&m_acceptInfo); 
  // Check if this is a track:
  if( p->type() != xAOD::Type::TrackParticle ) {
    ATH_MSG_ERROR( "accept(...) Function received a non-track" );
    return acceptData;
  }
    
  // Cast it to a track (we have already checked its type so we do not have to dynamic_cast):
  const xAOD::TrackParticle* trk = static_cast< const xAOD::TrackParticle* >( p );
    
  // Let the specific function do the work:
  return accept( *trk, nullptr );
}


/// Function that returns the result of the cuts applied to a track in the xAOD framework.
/// 
/// The workhorse function of the tool that checks whether or not a track passes,
/// and also can provide some information about why the track failed when it does.
/// First it accesses all the data it needs from the track (but does not retrieve
/// info it won't need to check) by looping over its InDet::TrackAccessors. It then
/// loops over its cut families, each of which is composed of one or more
/// InDet::TrackCuts, storing whether or not the track passes all the corresponding
/// cuts. This design allows functionality to be added to the tool without increasing
/// overhead in the main loop for users who don't need it. This eliminates the need
/// for several multiple versions of the tool.
/// 
/// @param trk The xAOD::TrackParticle in question.
/// @param vtx An optional pointer to a xAOD::Vertex that can be provided if the user
///            wishes to cut on z0 with respect to the vertex's position. Note that
///            this does not affect d0 cuts. If a vertex is not provided then the z0
///            cuts are performed with respect to the beamspot. More information can
///            be found at the InDetTrackingDC14 TWiki.
/// @returns A asg::AcceptData object that can be treated as a boolean for a simple
///          pass/fail. It does store more detailed information about which cuts
///          passed and which failed, by organizing the results into cut families.
///          For example, TAccept::getCutResult("Z0SinTheta") will be true if the
///          track passed cuts on Z0*Sin(theta), its uncertainty, and its
///          significance.
/// 
asg::AcceptData InDet::InDetTrackSelectionTool::accept( const xAOD::TrackParticle& trk,
                                                        const xAOD::Vertex* vtx ) const
{
  asg::AcceptData acceptData(&m_acceptInfo);
  if (m_trackParticleCuts.empty()) {
     m_numTracksPassed++;
     m_numTracksProcessed++;
     return acceptData;
  }

  InDetAccessor::TrackParticleHelper track_helper(trk,vtx);
  return accept( track_helper, m_trackParticleCuts);
}

#ifndef XAOD_ANALYSIS
/// Function that returns the result of the cuts to a Trk::Track.
///
/// This version of accept() is included to allow the tool to be used
/// in full-blown athena on Trk::Tracks. It is supposed to have identical
/// functionality to the user as the xAOD version above, but of course it
/// does need to access track information differently.
/// 
asg::AcceptData
InDet::InDetTrackSelectionTool::accept( const Trk::Track& track,
					const Trk::Vertex* vertex ) const
{
  if (!m_isInitialized) ATH_MSG_WARNING( "Tool is not initialized! Calling accept() will not be very helpful." );

  asg::AcceptData acceptData(&m_acceptInfo);
  if (m_trkTrackCuts.empty()) {
     m_numTracksPassed++;
     m_numTracksProcessed++;
     return acceptData;
  }

  const Trk::TrackParameters* perigee = track.perigeeParameters();

  if ( perigee == nullptr || !perigee->covariance() ) {
    ATH_MSG_WARNING( "Track preselection: Zero pointer to parameterbase* received (most likely a track without perigee). This track will not pass any cuts." );
    return acceptData;
  }

  std::unique_ptr<const Trk::TrackParameters> paramsAtVertex;
  if (vertex) {
    Trk::PerigeeSurface perigeeSurface(vertex->position());
    paramsAtVertex =
      m_extrapolator->extrapolate(Gaudi::Hive::currentContext(),
                                  *perigee,
                                  perigeeSurface,
                                  Trk::anyDirection,
                                  true,
                                  track.info().particleHypothesis());
    perigee = paramsAtVertex.get();
  }

  if ( perigee == nullptr || !perigee->covariance() ) {
    ATH_MSG_INFO( "Track preselection: cannot make a measured perigee. This track will not pass any cuts." );
    if (!m_initTrkTools)
      ATH_MSG_INFO( "The user should set \"UseTrkTrackTools\" to true if they want the extrapolation tool to try to get a perigee." );
    return acceptData;
  }


  std::unique_ptr<Trk::TrackSummary> cleanup_summary;
  const Trk::TrackSummary* summary = track.trackSummary();
  if (summary == nullptr && m_trackSumToolAvailable) {
     cleanup_summary = m_trackSumTool->summary(Gaudi::Hive::currentContext(),track);
     summary = cleanup_summary.get();
  }
  if (summary == nullptr) {
    ATH_MSG_INFO( "Track preselection: cannot get a track summary. This track will not pass any cuts." );
    if (!m_initTrkTools)
      ATH_MSG_INFO( "The Trk::Track tools were not set to be initialized. The user should set the property \"UseTrkTrackTools\" to true if they wish to use the summary tool." );
    return acceptData;
  }

  InDetAccessor::TrkTrackHelper track_helper(track,*summary, perigee);
  acceptData = accept( track_helper, m_trkTrackCuts);
  return acceptData;
}

#endif // XAOD_ANALYSIS

/// Function to set the cut level within standalone ROOT
/// 
/// This function can be used to set the cut selection of the tool to a
/// pre-defined level, as definied in the twiki for
/// InDetTrackingPerformanceGuidelines. It is left public for use in
/// standalone ROOT. In athena, this should be set through the jobOptions
/// via the "CutLevel" property instead.
///
/// @param level The CutLevel enumeration that picks the preset level
///              to set the cuts to.
/// @param overwrite A boolean (default true) that indicates whether
///                  to force an overwrite of each cut. If false, this
///                  function does not change the cut levels if they
///                  have been altered from their default (no cut) value.
///
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
void InDet::InDetTrackSelectionTool::setCutLevel(InDet::CutLevel level, Bool_t overwrite )
{
#ifndef XAOD_STANDALONE
  ATH_MSG_WARNING( "InDetTrackSelectionTool::setCutLevel() is not designed to be called manually in Athena." );
  ATH_MSG_WARNING( "It may not behave as intended. Instead, configure it in the job options through the CutLevel property." );
#endif // XAOD_STANDALONE
  if (!m_cutLevel.empty()) {
    ATH_MSG_WARNING( "Cut level already set to " << m_cutLevel << ".  Calling setCutLevel() is not expected." );
  }
  setCutLevelPrivate(level, overwrite);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

void InDet::InDetTrackSelectionTool::setCutLevelPrivate(InDet::CutLevel level, Bool_t overwrite)
{
  switch (level) {
  case CutLevel::NoCut :
    if (m_isInitialized) {
      // this check is in here so that it only happens once per call to setCutLevel,
      // but will still warn if only the private version is called somehow.
      ATH_MSG_WARNING( "Trying to set cut level while the tool is already initialized." );
      ATH_MSG_WARNING( "This will almost certainly not exhibit intended behavior." );
    }
    if (overwrite) {
      // minimum cuts will default to -1, so if a user wishes to remove a cut
      //   from a preset level they can do so by setting the minimum to zero.
      // maximum cuts can be removed from a preset level by setting them negative.
      m_minPt = -1.; // in MeV
      m_minP = -1.;
      m_maxAbsEta = LOCAL_MAX_DOUBLE;
      m_maxZ0SinTheta = LOCAL_MAX_DOUBLE; // in mm
      m_maxZ0 = LOCAL_MAX_DOUBLE;
      m_maxD0 = LOCAL_MAX_DOUBLE;
      m_maxSigmaD0 = LOCAL_MAX_DOUBLE;
      m_maxSigmaZ0 = LOCAL_MAX_DOUBLE;
      m_maxSigmaZ0SinTheta = LOCAL_MAX_DOUBLE;
      m_maxD0overSigmaD0 = LOCAL_MAX_DOUBLE;
      m_maxZ0overSigmaZ0 = LOCAL_MAX_DOUBLE;
      m_maxZ0SinThetaoverSigmaZ0SinTheta = LOCAL_MAX_DOUBLE;
      m_minNInnermostLayerHits = -1;
      m_minNNextToInnermostLayerHits = -1;
      m_minNBothInnermostLayersHits = -1;
      m_maxNInnermostLayerSharedHits = LOCAL_MAX_INT;
      m_useMinBiasInnermostLayersCut = 0;
      m_minNPixelHits = -1;
      m_minNPixelHitsPhysical = -1;
      m_maxNPixelSharedHits = LOCAL_MAX_INT;
      m_maxNPixelHoles = LOCAL_MAX_INT;
      m_minNSctHits = -1;
      m_minNSctHitsPhysical = -1;
      m_maxNSctHoles = LOCAL_MAX_INT;
      m_maxNSctSharedHits = LOCAL_MAX_INT;
      m_maxNSctDoubleHoles = LOCAL_MAX_INT;
      m_minNSiHits = -1;
      m_minNSiHitsPhysical = -1;
      m_maxNSiSharedHits = LOCAL_MAX_INT;
      m_minNSiHitsIfSiSharedHits = -1;
      m_maxNSiHoles = LOCAL_MAX_INT;
      m_minEtaForStrictNSiHitsCut = LOCAL_MAX_DOUBLE;
      m_minNSiHitsAboveEtaCutoff = -1;
      m_maxOneSharedModule = false;
      m_maxTrtEtaAcceptance = LOCAL_MAX_DOUBLE;
      m_maxEtaForTrtHitCuts = -1.; // this is really a minimum eta above which cuts are not applied
      m_minNTrtHits = -1;
      m_minNTrtHitsPlusOutliers = -1;
      m_minNTrtHighThresholdHits = -1;
      m_minNTrtHighThresholdHitsPlusOutliers = -1;
      m_maxTrtHighEFraction = LOCAL_MAX_DOUBLE;
      m_maxTrtHighEFractionWithOutliers = LOCAL_MAX_DOUBLE;
      m_maxTrtOutlierFraction = LOCAL_MAX_DOUBLE;
      m_maxChiSq = LOCAL_MAX_DOUBLE;
      m_minProb = -1.;
      m_maxChiSqperNdf = LOCAL_MAX_DOUBLE;
      m_minPtForProbCut = LOCAL_MAX_DOUBLE;
      m_minProbAbovePtCutoff = -1.;
      m_useEtaDependentMaxChiSq = false;
      m_minNUsedHitsdEdx = -1;
      m_minNOverflowHitsdEdx = -1;
      m_minEProbabilityHT = -1.;
      m_eProbHTonlyForXe = false;
#ifndef XAOD_ANALYSIS
      m_minNSiHitsMod = -1;
      m_minNSiHitsModTop = -1;
      m_minNSiHitsModBottom = -1;
#endif
      m_vecEtaCutoffsForSiHitsCut.clear();
      m_vecMinNSiHitsAboveEta.clear();
      m_vecEtaCutoffsForPtCut.clear();
      m_vecMinPtAboveEta.clear();
      m_vecPtCutoffsForSctHitsCut.clear();
      m_vecMinNSctHitsAbovePt.clear();
    }
    break;
  case CutLevel::Loose :
    setCutLevelPrivate(CutLevel::NoCut, overwrite); // if hard overwrite, reset all cuts first. will do nothing if !overwrite
    // change the cuts if a hard overwrite is asked for or if the cuts are unset
    if (overwrite || m_maxAbsEta >= LOCAL_MAX_DOUBLE) m_maxAbsEta = 2.5;
    if (overwrite || m_minNSiHits < 0) m_minNSiHits = 7;
    m_maxOneSharedModule = true;
    if (overwrite || m_maxNSiHoles >= LOCAL_MAX_INT) m_maxNSiHoles = 2;
    if (overwrite || m_maxNPixelHoles >= LOCAL_MAX_INT) m_maxNPixelHoles = 1;
    break;
  case CutLevel::LoosePrimary :
    setCutLevelPrivate(CutLevel::NoCut, overwrite); // implement loose cuts first
    if (overwrite || m_maxAbsEta >= LOCAL_MAX_DOUBLE) m_maxAbsEta = 2.5;
    if (overwrite || m_minNSiHits < 0) m_minNSiHits = 7;
    m_maxOneSharedModule = true;
    if (overwrite || m_maxNSiHoles >= LOCAL_MAX_INT) m_maxNSiHoles = 2;
    if (overwrite || m_maxNPixelHoles >= LOCAL_MAX_INT) m_maxNPixelHoles = 1;
    if (overwrite || m_minNSiHitsIfSiSharedHits < 0) m_minNSiHitsIfSiSharedHits = 10;
    break;
  case CutLevel::TightPrimary :
    setCutLevelPrivate(CutLevel::NoCut, overwrite);
    if (overwrite || m_maxAbsEta >= LOCAL_MAX_DOUBLE) m_maxAbsEta = 2.5;
    if (overwrite || m_minNSiHits < 0) m_minNSiHits = 9;
    m_maxOneSharedModule = true;
    if (overwrite || m_maxNSiHoles >= LOCAL_MAX_INT) m_maxNSiHoles = 2;
    if (overwrite || m_maxNPixelHoles >= LOCAL_MAX_INT) m_maxNPixelHoles = 0;
    if (overwrite || m_minEtaForStrictNSiHitsCut >= LOCAL_MAX_DOUBLE) m_minEtaForStrictNSiHitsCut = 1.65;
    if (overwrite || m_minNSiHitsAboveEtaCutoff < 0) m_minNSiHitsAboveEtaCutoff = 11;
    if (overwrite || m_minNBothInnermostLayersHits < 0) m_minNBothInnermostLayersHits = 1;
    break;
  case CutLevel::LooseMuon :
    setCutLevelPrivate(CutLevel::NoCut, overwrite); // reset cuts unless we are doing a soft set
    if (overwrite || m_minNPixelHits < 0) m_minNPixelHits = 1;
    if (overwrite || m_minNSctHits < 0) m_minNSctHits = 5;
    if (overwrite || m_maxNSiHoles >= LOCAL_MAX_INT) m_maxNSiHoles = 2;
    if (overwrite || m_maxTrtEtaAcceptance >= LOCAL_MAX_DOUBLE ) m_maxTrtEtaAcceptance = 0.1;
    if (overwrite || m_maxEtaForTrtHitCuts < 0.) m_maxEtaForTrtHitCuts = 1.9;
    if (overwrite || m_minNTrtHitsPlusOutliers < 0) m_minNTrtHitsPlusOutliers = 6;
    if (overwrite || m_maxTrtOutlierFraction >= LOCAL_MAX_DOUBLE) m_maxTrtOutlierFraction = 0.9;
    break;
  case CutLevel::LooseElectron :
    setCutLevelPrivate(CutLevel::NoCut, overwrite);
    if (overwrite || m_minNSiHits < 0) m_minNSiHits = 7;
    if (overwrite || m_minNPixelHits < 0) m_minNPixelHits = 1;
    break;
  case CutLevel::LooseTau :
    setCutLevelPrivate(CutLevel::NoCut, overwrite);
    if (overwrite || m_minPt < 0.0) m_minPt = 1000.0;
    if (overwrite || m_minNSiHits < 0) m_minNSiHits = 7;
    if (overwrite || m_minNPixelHits < 0) m_minNPixelHits = 2;
    if (overwrite || m_maxD0 >= LOCAL_MAX_DOUBLE) m_maxD0 = 1.0;
    if (overwrite || m_maxZ0 >= LOCAL_MAX_DOUBLE) m_maxZ0 = 1.5;
    break;
  case CutLevel::MinBias :
    setCutLevelPrivate(CutLevel::NoCut, overwrite);
    if (overwrite || m_useMinBiasInnermostLayersCut >= 0) m_useMinBiasInnermostLayersCut = 1; // if this is less than 0, it is turned off
    if (overwrite || m_minNPixelHits < 0) m_minNPixelHits = 1;
    if (overwrite || m_minNSctHits < 0) m_minNSctHits = 6;
    if (overwrite || m_minProbAbovePtCutoff < 0.) {
      m_minPtForProbCut = 10000.;
      m_minProbAbovePtCutoff = .01;
    }
    if (overwrite || m_maxD0 >= LOCAL_MAX_DOUBLE) m_maxD0 = 1.5;
    if (overwrite || m_maxZ0SinTheta >= LOCAL_MAX_DOUBLE) m_maxZ0SinTheta = 1.5;
    if (overwrite || m_maxAbsEta >= LOCAL_MAX_DOUBLE) m_maxAbsEta = 2.5;
    if (overwrite || m_minPt < 0.) m_minPt = 500.0;
    break;
  case CutLevel::HILoose:
    // HILoose is similar to MinBias, but not identical
    setCutLevelPrivate(CutLevel::NoCut, overwrite);
    if (overwrite || m_maxAbsEta >= LOCAL_MAX_DOUBLE) m_maxAbsEta = 2.5;
    if (overwrite || m_useMinBiasInnermostLayersCut >= 0) m_useMinBiasInnermostLayersCut = 1;
    if (overwrite || m_minNPixelHits < 0) m_minNPixelHits = 1;
    if (overwrite || (m_vecPtCutoffsForSctHitsCut.empty()
		      && m_vecMinNSctHitsAbovePt.empty())) {
      m_vecPtCutoffsForSctHitsCut = {0.0, 300.0, 400.0};
      m_vecMinNSctHitsAbovePt = {2, 4, 6};
    }
    if (overwrite || m_maxD0 >= LOCAL_MAX_DOUBLE) m_maxD0 = 1.5;
    if (overwrite || m_maxZ0SinTheta >= LOCAL_MAX_DOUBLE) m_maxZ0SinTheta = 1.5;
    break;
  case CutLevel::HITight:
    setCutLevelPrivate(CutLevel::NoCut, overwrite);
    // HITight is like HILoose but we require 8 SCT hits and 2 pixel hits
    setCutLevelPrivate(CutLevel::NoCut, overwrite);
    if (overwrite || m_maxAbsEta >= LOCAL_MAX_DOUBLE) m_maxAbsEta = 2.5;
    if (overwrite || m_useMinBiasInnermostLayersCut >= 0) m_useMinBiasInnermostLayersCut = 1;
    if (overwrite || m_minNPixelHits < 0) m_minNPixelHits = 2;
    if (overwrite || (m_vecPtCutoffsForSctHitsCut.empty()
		      && m_vecMinNSctHitsAbovePt.empty())) {
      m_vecPtCutoffsForSctHitsCut = {0.0, 300.0, 400.0};
      m_vecMinNSctHitsAbovePt = {4, 6, 8};
    }
    if (overwrite || m_maxD0 >= LOCAL_MAX_DOUBLE) m_maxD0 = 1.0;
    if (overwrite || m_maxZ0SinTheta >= LOCAL_MAX_DOUBLE) m_maxZ0SinTheta = 1.0;
    if (overwrite || m_maxChiSqperNdf >= LOCAL_MAX_DOUBLE) m_maxChiSqperNdf = 6.0;
    break;
  default:
    ATH_MSG_ERROR("CutLevel not recognized. Cut selection will remain unchanged.");
    break;
  }      
}

// initialize the map from strings to cut levels
const std::unordered_map<std::string, InDet::CutLevel>
InDet::InDetTrackSelectionTool::s_mapCutLevel =
  {
    {"NoCut", InDet::CutLevel::NoCut},
    {"Loose", InDet::CutLevel::Loose},
    {"LoosePrimary", InDet::CutLevel::LoosePrimary},
    {"TightPrimary", InDet::CutLevel::TightPrimary},
    {"LooseMuon", InDet::CutLevel::LooseMuon},
    {"LooseElectron", InDet::CutLevel::LooseElectron},
    {"LooseTau", InDet::CutLevel::LooseTau},
    {"MinBias", InDet::CutLevel::MinBias},
    {"HILoose", InDet::CutLevel::HILoose},
    {"HITight", InDet::CutLevel::HITight}
  };
