/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "src/InDetTrackCut.h"

#include "AsgTools/Check.h"
#include "xAODTracking/TrackingPrimitives.h"

#ifndef XAOD_ANALYSIS
#include "TrkTrack/Track.h"
#include "VxVertex/Vertex.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/FitQuality.h"
#endif

#include <memory>

namespace {
  // define a local make_unique to use in gcc version < 4.9
#if __cplusplus > 201103L
  using std::make_unique;
#else
  // this version of make_unique is taken from Scott Meyers Effective Modern C++, item 21
  template<typename T, typename... Ts>
  std::unique_ptr<T> make_unique(Ts&&... params)
  {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
  }
#endif
}

InDet::InDetTrackSelectionTool::InDetTrackSelectionTool(const std::string& name, const std::string& cutLevel)
  : asg::AsgTool(name)
  , m_accept( "InDetTrackSelection" )
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
  declareProperty("maxNSiSharedModules", m_maxNSiSharedModules,
		  "Maximum (pixel + SCT/2) shared hits");
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
  if (m_cutLevel != "") {
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
  }
#endif // XAOD_ANALYSIS

  // local helper functions to determine if a maximum cut should be set.
  // we want the user to be able to set a maximum cut to a negative number to unset it from a pre-defined cut level.
  // this will break down if there is ever a legitimate reason to restrict a value to be negative.
  const auto maxDoubleIsSet = [&](Double_t cutValue){return cutValue < LOCAL_MAX_DOUBLE && cutValue >= 0.;};
  const auto maxIntIsSet = [&](Int_t cutValue){return cutValue < LOCAL_MAX_INT && cutValue >= 0;};

  // create the cuts and initialize them (which will create the necessary accessors).
  // tell the user which cuts are recognized.
  if (m_minPt > 0.) {
    ATH_MSG_INFO( "  Minimum Pt: " << m_minPt << " MeV" );
    m_trackCuts["Pt"].push_back(make_unique<MinCut<double, &xAOD::TrackParticle::pt> >(this, m_minPt, "pt"));
  }
  if (maxDoubleIsSet(m_maxAbsEta)) {
    ATH_MSG_INFO( "  Maximum |Eta|: " << m_maxAbsEta );
    m_trackCuts["Eta"].push_back(make_unique< MaxAbsCut<double, &xAOD::TrackParticle::eta> >(this, m_maxAbsEta, "eta"));
  }
  if (m_minP > 0.) {
    ATH_MSG_INFO( "  Minimum P: " << m_minP << " MeV" );
    m_trackCuts["P"].push_back(make_unique< MaxAbsCut<float, &xAOD::TrackParticle::qOverP> >(this, 1./m_minP, "qOverP"));
  }
  if (maxDoubleIsSet(m_maxD0)) {
    ATH_MSG_INFO( "  Maximum d0: " << m_maxD0 << " mm" );
    m_trackCuts["D0"].push_back(make_unique<D0Cut>(this, m_maxD0));
  }
  if (maxDoubleIsSet(m_maxZ0)) {
    ATH_MSG_INFO( "  Maximum z0: " << m_maxZ0 << " mm");
    m_trackCuts["Z0"].push_back(make_unique<Z0Cut>(this, m_maxZ0));
  }
  if (maxDoubleIsSet(m_maxZ0SinTheta)) {
    ATH_MSG_INFO( "  Maximum z0*sin(theta): " << m_maxZ0SinTheta << " mm" );
    m_trackCuts["Z0SinTheta"].push_back(make_unique<Z0SinThetaCut>(this, m_maxZ0SinTheta));
  }
  if (maxDoubleIsSet(m_maxSigmaD0)) {
    ATH_MSG_INFO( "  Maximum uncertainty on d0: " << m_maxSigmaD0 << " mm" );
    m_trackCuts["D0"].push_back(make_unique<D0SigmaCut>(this, m_maxSigmaD0));
  }
  if (maxDoubleIsSet(m_maxSigmaZ0)) {
    ATH_MSG_INFO( "  Maximum uncertainty on z0: " << m_maxSigmaZ0 << " mm" );
    m_trackCuts["Z0"].push_back(make_unique<Z0SigmaCut>(this, m_maxSigmaZ0));
  }
  if (maxDoubleIsSet(m_maxSigmaZ0SinTheta)) {
    ATH_MSG_INFO( "  Maximum uncertainty on z0*sin(theta): "
		  << m_maxSigmaZ0SinTheta << " mm" );
    m_trackCuts["Z0SinTheta"].push_back(make_unique<Z0SinThetaSigmaCut>(this, m_maxSigmaZ0SinTheta));
  }
  if (maxDoubleIsSet(m_maxD0overSigmaD0)) {
    ATH_MSG_INFO( "  Maximum significance on d0: " << m_maxD0overSigmaD0 );
    m_trackCuts["D0"].push_back(make_unique<D0SignificanceCut>(this, m_maxD0overSigmaD0));
  }
  if (maxDoubleIsSet(m_maxZ0overSigmaZ0)) {
    ATH_MSG_INFO( "  Maximum significance on z0: " << m_maxZ0overSigmaZ0 );
    m_trackCuts["Z0"].push_back(make_unique<Z0SignificanceCut>(this, m_maxZ0overSigmaZ0));
  }
  if (maxDoubleIsSet(m_maxZ0SinThetaoverSigmaZ0SinTheta)) {
    ATH_MSG_INFO( "  Maximum significance on z0*sin(theta): "
		  << m_maxZ0SinThetaoverSigmaZ0SinTheta );
    m_trackCuts["Z0SinTheta"].push_back(make_unique<Z0SinThetaSignificanceCut>(this, m_maxZ0SinThetaoverSigmaZ0SinTheta));
  }
  if (m_minNInnermostLayerHits > 0) {
    ATH_MSG_INFO( "  Minimum hits from innermost pixel layer: " << m_minNInnermostLayerHits );
    ATH_MSG_INFO( "    (Track will pass if no hit is expected.)" );
    auto minInnermostLayerHits = make_unique< FuncSummaryValueCut<2> >
      (this, std::array<xAOD::SummaryType,2>({xAOD::numberOfInnermostPixelLayerHits, xAOD::expectInnermostPixelLayerHit}));
    minInnermostLayerHits->setFunction( [=](const std::array<uint8_t, 2> vals)
					{return !vals[1] || vals[0] >= m_minNInnermostLayerHits;} );
    m_trackCuts["InnermostLayersHits"].push_back(std::move(minInnermostLayerHits));
  }
  if (m_minNNextToInnermostLayerHits > 0) {
    ATH_MSG_INFO( "  Minimum hits from next to innermost pixel layer: " << m_minNNextToInnermostLayerHits );
    ATH_MSG_INFO( "    (Track will pass if no hit is expected.)" );
    auto minNextToInnermostLayerHits = make_unique< FuncSummaryValueCut<2> >
      (this, std::array<xAOD::SummaryType,2>({xAOD::numberOfNextToInnermostPixelLayerHits,
	    xAOD::expectNextToInnermostPixelLayerHit}) );
    minNextToInnermostLayerHits->setFunction( [=] (std::array<uint8_t, 2> vals)
					      {return !vals[1] || vals[0] >= m_minNNextToInnermostLayerHits;} );
    m_trackCuts["InnermostLayersHits"].push_back(std::move(minNextToInnermostLayerHits));
  }
  if (m_minNBothInnermostLayersHits > 0) {
    ATH_MSG_INFO( "  Minimum hits from both innermost pixel layers: " << m_minNBothInnermostLayersHits );
    ATH_MSG_INFO( "    (If a layer has no hits but one is not expected, the" );
    ATH_MSG_INFO( "     number of hits in that layer will be taken to be 1.)" );
    if (m_minNBothInnermostLayersHits > 2) {
      ATH_MSG_WARNING( "A value of minNBothInnermostLayersHits above 2 does not make sense." );
      ATH_MSG_WARNING( "  Use 1 for \"or\" or 2 for \"and\"." );
    }
    auto minInnerHits = make_unique< FuncSummaryValueCut<4> >
      (this, std::array<xAOD::SummaryType,4>(
	{xAOD::numberOfInnermostPixelLayerHits, xAOD::numberOfNextToInnermostPixelLayerHits,
	    xAOD::expectInnermostPixelLayerHit, xAOD::expectNextToInnermostPixelLayerHit}) );
    minInnerHits->setFunction( [=](const std::array<uint8_t, 4>& vals)
			       {return std::max(vals[0], (uint8_t) !vals[2])
				   + std::max(vals[1], (uint8_t) !vals[3])
				   >= m_minNBothInnermostLayersHits; } );
    m_trackCuts["InnermostLayersHits"].push_back(std::move(minInnerHits));
  }
  if (m_useMinBiasInnermostLayersCut > 0 ) { // less than zero indicates this cut is manually turned off
    ATH_MSG_INFO( "  An innermost layer hit is required if expected, otherwise" );
    ATH_MSG_INFO( "    a next-to-innermost layer hit is required if it is expected." );
    auto minInnerHits = make_unique< FuncSummaryValueCut<4> >
      (this, std::array<xAOD::SummaryType,4>(  
	{xAOD::numberOfInnermostPixelLayerHits, xAOD::numberOfNextToInnermostPixelLayerHits,
	    xAOD::expectInnermostPixelLayerHit, xAOD::expectNextToInnermostPixelLayerHit}) );
    minInnerHits->setFunction( [=](const std::array<uint8_t,4>& vals)
			       {return (vals[2] > 0 && vals[0] >= 1) ||
				   (vals[2] == 0 && (vals[3] == 0 || vals[1] >= 1));} );
    m_trackCuts["InnermostLayersHits"].push_back(std::move(minInnerHits));
  }
  if (maxIntIsSet(m_maxNInnermostLayerSharedHits)) {
    ATH_MSG_INFO( "  Maximum shared hits in innermost pixel layer: " << m_maxNInnermostLayerSharedHits );
    m_trackCuts["InnermostLayersHits"].push_back
      (make_unique<MaxSummaryValueCut>
       (this, m_maxNInnermostLayerSharedHits,
	std::vector<xAOD::SummaryType>({xAOD::numberOfInnermostPixelLayerSharedHits}) ));
  }
  if (m_minNPixelHits > 0) {
    ATH_MSG_INFO( "  Minimum pixel hits: " << m_minNPixelHits );
    m_trackCuts["PixelHits"].push_back
      (make_unique<MinSummaryValueCut>
       (this, m_minNPixelHits, std::vector<xAOD::SummaryType>({xAOD::numberOfPixelHits, xAOD::numberOfPixelDeadSensors}) ));
  }
  if (m_minNPixelHitsPhysical > 0) {
    ATH_MSG_INFO( "  Minimum physical pixel hits (i.e. dead sensors do not count): "
  		  << m_minNPixelHitsPhysical );
    auto minPixelPhysHits = make_unique<MinSummaryValueCut> (this, m_minNPixelHitsPhysical );
    minPixelPhysHits->addSummaryType(xAOD::numberOfPixelHits);
    m_trackCuts["PixelHits"].push_back(std::move(minPixelPhysHits));
  }
  if (maxIntIsSet(m_maxNPixelHoles)) {
    ATH_MSG_INFO( "  Maximum pixel holes: " << m_maxNPixelHoles );
    auto maxPixHoles = make_unique<MaxSummaryValueCut>(this, m_maxNPixelHoles);
    maxPixHoles->addSummaryType(xAOD::numberOfPixelHoles);
    m_trackCuts["PixelHits"].push_back(std::move(maxPixHoles));
  }
  if (maxIntIsSet(m_maxNPixelSharedHits)) {
    ATH_MSG_INFO( "  Maximum pixel shared hits: " << m_maxNPixelSharedHits );
    auto maxPixSharedHits = make_unique<MaxSummaryValueCut>(this, m_maxNPixelSharedHits);
    maxPixSharedHits->addSummaryType(xAOD::numberOfPixelSharedHits);
    m_trackCuts["PixelHits"].push_back(std::move(maxPixSharedHits));
  }
  if (m_minNSctHits > 0) {
    ATH_MSG_INFO( "  Minimum SCT hits: " << m_minNSctHits );
    m_trackCuts["SctHits"].push_back
      (make_unique<MinSummaryValueCut>
       (this, m_minNSctHits, std::vector<xAOD::SummaryType>({xAOD::numberOfSCTHits, xAOD::numberOfSCTDeadSensors}) ));
  }
  if (m_minNSctHitsPhysical > 0) {
    ATH_MSG_INFO( "  Minimum physical SCT hits (i.e. dead sensors do not count): "
  		  << m_minNSctHitsPhysical );
    auto minSctPhysHits = make_unique<MinSummaryValueCut> (this, m_minNSctHitsPhysical );
    minSctPhysHits->addSummaryType(xAOD::numberOfSCTHits);
    m_trackCuts["SctHits"].push_back(std::move(minSctPhysHits));
  }
  if (maxIntIsSet(m_maxNSctHoles)) {
    ATH_MSG_INFO( "  Maximum SCT holes: " << m_maxNSctHoles );
    auto maxSctHoles = make_unique<MaxSummaryValueCut>(this, m_maxNSctHoles);
    maxSctHoles->addSummaryType( xAOD::numberOfSCTHoles );
    m_trackCuts["SctHits"].push_back(std::move(maxSctHoles));
  }
  if (maxIntIsSet(m_maxNSctSharedHits)) {
    ATH_MSG_INFO( "  Maximum SCT shared hits: " << m_maxNSctSharedHits );
    auto maxSctSharedHits = make_unique<MaxSummaryValueCut>(this, m_maxNSctSharedHits);
    maxSctSharedHits->addSummaryType( xAOD::numberOfSCTSharedHits );
    m_trackCuts["SctHits"].push_back(std::move(maxSctSharedHits));
  }
  if (maxIntIsSet(m_maxNSctDoubleHoles)) {
    ATH_MSG_INFO( "  Maximum SCT double holes: " << m_maxNSctDoubleHoles );
    auto maxSctDoubleHoles = make_unique<MaxSummaryValueCut>(this, m_maxNSctDoubleHoles);
    maxSctDoubleHoles->addSummaryType( xAOD::numberOfSCTDoubleHoles );
    m_trackCuts["SctHits"].push_back(std::move(maxSctDoubleHoles));
  }
  if (m_minNSiHits > 0) {
    ATH_MSG_INFO( "  Minimum silicon (pixel + SCT) hits: " << m_minNSiHits );
    auto minSiHits = make_unique<MinSummaryValueCut>(this, m_minNSiHits);
    minSiHits->addSummaryType( xAOD::numberOfPixelHits );
    minSiHits->addSummaryType( xAOD::numberOfSCTHits );
    minSiHits->addSummaryType( xAOD::numberOfPixelDeadSensors );
    minSiHits->addSummaryType( xAOD::numberOfSCTDeadSensors );
    m_trackCuts["SiHits"].push_back(std::move(minSiHits));
  }
  if (m_minNSiHitsPhysical > 0) {
    ATH_MSG_INFO( "  Minimum physical silicon hits (i.e. dead sensors do not count): "
		  << m_minNSiHitsPhysical );
    auto minSiHitsPhys = make_unique<MinSummaryValueCut>(this, m_minNSiHitsPhysical);
    minSiHitsPhys->addSummaryType( xAOD::numberOfPixelHits );
    minSiHitsPhys->addSummaryType( xAOD::numberOfSCTHits );
    m_trackCuts["SiHits"].push_back(std::move(minSiHitsPhys));
  }
  if (maxIntIsSet(m_maxNSiHoles)) {
    ATH_MSG_INFO( "  Maximum silicon holes: " << m_maxNSiHoles );
    auto maxSiHoles = make_unique<MaxSummaryValueCut>(this, m_maxNSiHoles);
    maxSiHoles->addSummaryType( xAOD::numberOfPixelHoles );
    maxSiHoles->addSummaryType( xAOD::numberOfSCTHoles );
    m_trackCuts["SiHits"].push_back(std::move(maxSiHoles));
  }
  if (maxIntIsSet(m_maxNSiSharedHits)) {
    ATH_MSG_INFO( "  Maximum silicon shared hits: " << m_maxNSiSharedHits );
    auto maxSiSharedHits = make_unique<MaxSummaryValueCut>(this, m_maxNSiSharedHits);
    maxSiSharedHits->addSummaryType( xAOD::numberOfPixelSharedHits );
    maxSiSharedHits->addSummaryType( xAOD::numberOfSCTSharedHits );
    m_trackCuts["SiHits"].push_back(std::move(maxSiSharedHits));
  }
  if (maxIntIsSet(m_maxNSiSharedModules)) {
    ATH_MSG_INFO( "  No more than " << m_maxNSiSharedModules << " shared Si module(s)" );
    ATH_MSG_INFO( "    (where a \"shared module\" is 1 shared pixel hit or 2 shared SCT hits)" );
    auto oneSharedModule = make_unique< FuncSummaryValueCut<2> >
      (this, std::array<xAOD::SummaryType, 2>({xAOD::numberOfPixelSharedHits, xAOD::numberOfSCTSharedHits}) );
    oneSharedModule->setFunction( [=] (const std::array<uint8_t, 2> vals) {return vals[0] + vals[1]/2 <= m_maxNSiSharedModules;} );
    m_trackCuts["SiHits"].push_back(std::move(oneSharedModule));
  }
  if (m_minNSiHitsIfSiSharedHits > 0) {
    ATH_MSG_INFO( "  Minimum silicon hits if the track has shared hits: "
		  << m_minNSiHitsIfSiSharedHits );
    auto siHits = make_unique< FuncSummaryValueCut<6> >
      (this, std::array<xAOD::SummaryType, 6>({xAOD::numberOfPixelHits, xAOD::numberOfSCTHits,
	    xAOD::numberOfPixelDeadSensors, xAOD::numberOfSCTDeadSensors,
	    xAOD::numberOfPixelSharedHits, xAOD::numberOfSCTSharedHits}) );
    siHits->setFunction( [=] (const std::array<uint8_t, 6>& vals)
			 {return (vals[4] + vals[5] == 0) ||
			     (vals[0] + vals[1] + vals[2] + vals[3]
			      >= m_minNSiHitsIfSiSharedHits);} );
    m_trackCuts["SiHits"].push_back(std::move(siHits));
  }
  if (maxDoubleIsSet(m_minEtaForStrictNSiHitsCut)
      && m_minNSiHitsAboveEtaCutoff > 0) {
    ATH_MSG_INFO( "  Require " << m_minNSiHitsAboveEtaCutoff
		  << " silicon hits above eta = " << m_minEtaForStrictNSiHitsCut );
    m_trackCuts["SiHits"].push_back(make_unique<MinNSiHitsAboveEta>(this, m_minNSiHitsAboveEtaCutoff,
    								    m_minEtaForStrictNSiHitsCut));
  }
  if (m_useExperimentalInnermostLayersCut) {
    ATH_MSG_INFO( "  Zero pixel holes allowed, except one pix hole is allowed if there is a physical IBL hit and a BLayer hit is expected but there is no BLayer hit." );
    auto pixHoles = make_unique< FuncSummaryValueCut<4> >
      (this, std::array<xAOD::SummaryType, 4>({xAOD::numberOfPixelHoles, xAOD::numberOfInnermostPixelLayerHits, xAOD::expectNextToInnermostPixelLayerHit, xAOD::numberOfNextToInnermostPixelLayerHits}) );
    pixHoles->setFunction( [=](const std::array<uint8_t, 4>& vals)
			   {return (vals[0] == 0) || (vals[0] <= 1 && vals[1] >= 1 && vals[2] && vals[3] == 0);});
    m_trackCuts["PixHits"].push_back(std::move(pixHoles));
  }
#ifndef XAOD_ANALYSIS
  if (m_minNSiHitsMod > 0) {
    ATH_MSG_INFO( "  Minimum modified Si hits (2*pix + sct) (does not include dead sensors)= "
		  << m_minNSiHitsMod );
    auto siHits = make_unique<MinSummaryValueCut>(this, m_minNSiHitsMod);
    siHits->addSummaryType(xAOD::numberOfPixelHits);
    siHits->addSummaryType(xAOD::numberOfPixelHits); // pixel hits count twice in this definition
    siHits->addSummaryType(xAOD::numberOfSCTHits);
    m_trackCuts["SiHits"].push_back(std::move(siHits));
  }
  if (m_minNSiHitsModTop > 0 || m_minNSiHitsModBottom > 0) {
    ATH_MSG_INFO( "  Minimum modified Si hits in top half = "
		  << m_minNSiHitsModTop );
    ATH_MSG_INFO( "  Minimum modified Si hits in bottom half = "
		  << m_minNSiHitsModBottom );
    m_trackCuts["SiHits"].push_back(make_unique<MinSiHitsModTopBottomCut>
				    (this, m_minNSiHitsModTop, m_minNSiHitsModTop));
  }
#endif
  if (m_maxEtaForTrtHitCuts > 0. && m_maxTrtEtaAcceptance < m_maxEtaForTrtHitCuts) {
    ATH_MSG_INFO( "  -- TRT hit cuts applied above eta = " << m_maxTrtEtaAcceptance
		  << " and below eta = " << m_maxEtaForTrtHitCuts << " --" );
    if (m_minNTrtHits > 0) {
      ATH_MSG_INFO( "    Minimum TRT hits outside eta acceptance: " << m_minNTrtHits );
      auto minTrtHits = make_unique<MinTrtHitCut>(this, m_minNTrtHits, m_maxTrtEtaAcceptance,
						  m_maxEtaForTrtHitCuts);
      minTrtHits->addSummaryType( xAOD::numberOfTRTHits );
      m_trackCuts["TrtHits"].push_back(std::move(minTrtHits));
    }
    if (m_minNTrtHitsPlusOutliers > 0) {
      ATH_MSG_INFO( "    Minimum TRT hits outside eta acceptance including outliers: " << m_minNTrtHitsPlusOutliers );
      auto minTrtHits = make_unique<MinTrtHitCut>(this, m_minNTrtHitsPlusOutliers, m_maxTrtEtaAcceptance,
						  m_maxEtaForTrtHitCuts);
      minTrtHits->addSummaryType( xAOD::numberOfTRTHits );
      minTrtHits->addSummaryType( xAOD::numberOfTRTOutliers );
      m_trackCuts["TrtHits"].push_back(std::move(minTrtHits));
    }
    if (m_minNTrtHighThresholdHits > 0) {
      ATH_MSG_INFO( "    Minimum TRT hits outside eta acceptance above high energy threshold: "
		    << m_minNTrtHighThresholdHits );
      auto minTrtHits = make_unique<MinTrtHitCut>(this, m_minNTrtHighThresholdHits,
						  m_maxTrtEtaAcceptance, m_maxEtaForTrtHitCuts);
      minTrtHits->addSummaryType( xAOD::numberOfTRTHighThresholdHits );
      m_trackCuts["TrtHits"].push_back(std::move(minTrtHits));
    }
    if (m_minNTrtHighThresholdHitsPlusOutliers > 0) {
      ATH_MSG_INFO( "    Minimum TRT hits outside eta acceptance above high energy threshold including outliers: "
		    << m_minNTrtHighThresholdHitsPlusOutliers );
      auto minTrtHits = make_unique<MinTrtHitCut>(this, m_minNTrtHighThresholdHitsPlusOutliers,
						  m_maxTrtEtaAcceptance, m_maxEtaForTrtHitCuts);
      minTrtHits->addSummaryType( xAOD::numberOfTRTHighThresholdHits );
      minTrtHits->addSummaryType( xAOD::numberOfTRTHighThresholdOutliers );
      m_trackCuts["TrtHits"].push_back(std::move(minTrtHits));
    }
    if (maxDoubleIsSet(m_maxTrtHighEFraction)) { // I think this condition could be instead that it is between 0 and 1
      ATH_MSG_INFO( "    Maximum ratio of high threshold to regular TRT hits outside eta acceptance: "
		    << m_maxTrtHighEFraction);
      auto maxTrtRatio = make_unique<MaxTrtHitRatioCut>(this, m_maxTrtHighEFraction,
							m_maxTrtEtaAcceptance,
							m_maxEtaForTrtHitCuts);
      maxTrtRatio->addSummaryTypeNumerator( xAOD::numberOfTRTHighThresholdHits );
      maxTrtRatio->addSummaryTypeDenominator( xAOD::numberOfTRTHits );
      m_trackCuts["TrtHits"].push_back(std::move(maxTrtRatio));
    }
    if (maxDoubleIsSet(m_maxTrtHighEFractionWithOutliers)) {
      ATH_MSG_INFO( "    Maximum ratio of high threshold to regular TRT hits above eta acceptance including outliers: "
		    << m_maxTrtHighEFractionWithOutliers);
      auto maxTrtRatio = make_unique<MaxTrtHitRatioCut>(this, m_maxTrtHighEFractionWithOutliers,
							m_maxTrtEtaAcceptance,
							m_maxEtaForTrtHitCuts);
      maxTrtRatio->addSummaryTypeNumerator( xAOD::numberOfTRTHighThresholdHits );
      maxTrtRatio->addSummaryTypeNumerator( xAOD::numberOfTRTHighThresholdOutliers );
      maxTrtRatio->addSummaryTypeDenominator( xAOD::numberOfTRTHits );
      maxTrtRatio->addSummaryTypeDenominator( xAOD::numberOfTRTOutliers );
      m_trackCuts["TrtHits"].push_back(std::move(maxTrtRatio));
    }
    if (m_maxTrtOutlierFraction < 1. && m_maxTrtOutlierFraction >= 0.) {
      ATH_MSG_INFO( "    Maximum fraction of TRT hits that are outliers: " << m_maxTrtOutlierFraction );
      auto maxTrtRatio = make_unique<MaxTrtHitRatioCut>(this, m_maxTrtOutlierFraction,
							m_maxTrtEtaAcceptance,
							m_maxEtaForTrtHitCuts);
      maxTrtRatio->addSummaryTypeNumerator( xAOD::numberOfTRTOutliers );
      maxTrtRatio->addSummaryTypeDenominator( xAOD::numberOfTRTHits );
      maxTrtRatio->addSummaryTypeDenominator( xAOD::numberOfTRTOutliers );
      m_trackCuts["TrtHits"].push_back(std::move(maxTrtRatio));
    }
  }
  if (m_useEtaDependentMaxChiSq) {
    ATH_MSG_INFO( "  Using pre-defined eta-dependent maximum chi squared (no longer recommended)." );
    m_trackCuts["FitQuality"].push_back(make_unique<EtaDependentChiSqCut>(this));
  }
  if (maxDoubleIsSet(m_maxChiSq)) {
    ATH_MSG_INFO( "  Maximum chi squared: " << m_maxChiSq );
    m_trackCuts["FitQuality"].push_back(make_unique<MaxChiSq>(this, m_maxChiSq));
  }
  if (maxDoubleIsSet(m_maxChiSqperNdf)) {
    ATH_MSG_INFO( "  Maximum chi squared per degree of freedom: " << m_maxChiSqperNdf );
    m_trackCuts["FitQuality"].push_back(make_unique<MaxChiSqPerNdf>(this, m_maxChiSqperNdf));
  }
  if (m_minProb > 0.) {
    ATH_MSG_INFO( "  Minimum chi squared probability: " << m_minProb );
    m_trackCuts["FitQuality"].push_back(make_unique<MinProb>(this, m_minProb));
  }
  if (maxDoubleIsSet(m_minPtForProbCut) && m_minProbAbovePtCutoff > 0.) {
    ATH_MSG_INFO( "  Minimum chi-sq probability of " << m_minProbAbovePtCutoff
		  << " above pt of " << m_minPtForProbCut*1e-3 << " GeV." );
    auto lowPtOrAboveProb = make_unique< OrCut<2> >(this);
    lowPtOrAboveProb->Cut(0) = make_unique< MaxCut<double, &xAOD::TrackParticle::pt> >(this, m_minPtForProbCut, "pt");
    lowPtOrAboveProb->Cut(1) = make_unique<MinProb>(this, m_minProbAbovePtCutoff);
    m_trackCuts["FitQuality"].push_back(std::move(lowPtOrAboveProb));
  }
  if (m_minNUsedHitsdEdx > 0) {
    ATH_MSG_INFO( "  Minimum used hits for dEdx: " << m_minNUsedHitsdEdx );
    m_trackCuts["dEdxHits"].push_back(make_unique< MinCut<uint8_t, &xAOD::TrackParticle::numberOfUsedHitsdEdx> >(this, m_minNUsedHitsdEdx, "usedHitsdEdx"));
  }
  if (m_minNOverflowHitsdEdx > 0) {
    ATH_MSG_INFO( "  Minimum IBL overflow hits for dEdx: " << m_minNOverflowHitsdEdx );
    m_trackCuts["dEdxHits"].push_back(make_unique< MinCut<uint8_t, &xAOD::TrackParticle::numberOfIBLOverflowsdEdx> >(this, m_minNOverflowHitsdEdx, "overflowHitsdEdx"));
  }
  if (m_minEProbabilityHT > 0) {
    ATH_MSG_INFO( "  Minimum high threshold electron probability: " << m_minEProbabilityHT );
    if (m_eProbHTonlyForXe) {
      ATH_MSG_INFO( "    (only applied on tracks where all TRT hits are Xenon)" );
      auto notAllXeOrEProb = make_unique< OrCut<2> >(this);
      auto notAllXe = make_unique< FuncSummaryValueCut<3> >
	(this, std::array<xAOD::SummaryType,3>{xAOD::numberOfTRTHits, xAOD::numberOfTRTOutliers, xAOD::numberOfTRTXenonHits});
      notAllXe->setFunction( [=] (const std::array<uint8_t, 3>& vals)
			     {return vals[0] + vals[1] > vals[2];} );
      notAllXeOrEProb->setCut(0, std::move(notAllXe));
      notAllXeOrEProb->Cut(1) = make_unique<MinEProbHTCut>(this, m_minEProbabilityHT);
      m_trackCuts["eProbHT"].push_back(std::move(notAllXeOrEProb));
    } else {
      m_trackCuts["eProbHT"].push_back(make_unique<MinEProbHTCut>(this, m_minEProbabilityHT));
    }
  }

  if (!m_vecEtaCutoffsForSiHitsCut.empty() || !m_vecMinNSiHitsAboveEta.empty()) {
    auto cutSize = m_vecEtaCutoffsForSiHitsCut.size();
    if (cutSize != m_vecMinNSiHitsAboveEta.size()) {
      ATH_MSG_ERROR( "Eta cutoffs and Silicon hit cuts must be vectors of the same length." );
      return StatusCode::FAILURE;
    }
    for (size_t i_cut=0; i_cut<cutSize-1; ++i_cut) {
      ATH_MSG_INFO( "  for " << m_vecEtaCutoffsForSiHitsCut.at(i_cut)
		    << " < eta < " << m_vecEtaCutoffsForSiHitsCut.at(i_cut+1)
		    << " ,Silicon hits  >= " << m_vecMinNSiHitsAboveEta.at(i_cut) );
    }
    ATH_MSG_INFO( "  for eta > " << m_vecEtaCutoffsForSiHitsCut.at(cutSize-1)
		    << " ,Silicon hits >= " << m_vecMinNSiHitsAboveEta.at(cutSize-1) );
    auto siHitCut = make_unique<EtaDependentSiliconHitsCut>
      (this, m_vecEtaCutoffsForSiHitsCut, m_vecMinNSiHitsAboveEta);
    m_trackCuts["SiHits"].push_back(std::move(siHitCut));
  }

  if (!m_vecEtaCutoffsForPtCut.empty() || !m_vecMinPtAboveEta.empty()) {
    auto cutSize = m_vecEtaCutoffsForPtCut.size();
    if (cutSize != m_vecMinPtAboveEta.size()) {
      ATH_MSG_ERROR( "Eta cutoffs and pT cuts must be vectors of the same length." );
      return StatusCode::FAILURE;
    }
    for (size_t i_cut=0; i_cut<cutSize-1; ++i_cut) {
      ATH_MSG_INFO( "  for " << m_vecEtaCutoffsForPtCut.at(i_cut)
                    << " < eta < " << m_vecEtaCutoffsForPtCut.at(i_cut+1)
                    << " ,transverse momentum >= " << m_vecMinPtAboveEta.at(i_cut) );
    }
    ATH_MSG_INFO( "  for eta > " << m_vecEtaCutoffsForPtCut.at(cutSize-1)
		  << " ,transverse momentum >= " << m_vecMinPtAboveEta.at(cutSize-1) );
    auto ptCut = make_unique<EtaDependentPtCut>
      (this, m_vecEtaCutoffsForPtCut, m_vecMinPtAboveEta);
    m_trackCuts["Pt"].push_back(std::move(ptCut));
  }



  if (!m_vecPtCutoffsForSctHitsCut.empty() || !m_vecMinNSctHitsAbovePt.empty()) {
    auto cutSize = m_vecPtCutoffsForSctHitsCut.size();
    if (cutSize != m_vecMinNSctHitsAbovePt.size()) {
      ATH_MSG_ERROR( "Pt cutoffs and SCT hit cuts must be vectors of the same length." );
      return StatusCode::FAILURE;
    }
    for (size_t i_cut=0; i_cut<cutSize-1; ++i_cut) {
      ATH_MSG_INFO( "  for " << m_vecPtCutoffsForSctHitsCut.at(i_cut)
		    << " < pt < " << m_vecPtCutoffsForSctHitsCut.at(i_cut+1)
		    << " MeV,\tSCT hits >= " << m_vecMinNSctHitsAbovePt.at(i_cut) );
    }
    ATH_MSG_INFO( "  for pt > " << m_vecPtCutoffsForSctHitsCut.at(cutSize-1)
		    << " MeV,\t\tSCT hits >= " << m_vecMinNSctHitsAbovePt.at(cutSize-1) );
    auto sctCut = make_unique<PtDependentSctHitsCut>
      (this, m_vecPtCutoffsForSctHitsCut, m_vecMinNSctHitsAbovePt);
    m_trackCuts["SctHits"].push_back(std::move(sctCut));
  }


  // initialize the cuts and set up the TAccept object
  for (const auto& cutFamily : m_trackCuts) {
    for (const auto& cut : cutFamily.second) {
      ATH_CHECK( cut->initialize() );
    }
    const std::string& cutFamilyName = cutFamily.first;
    m_numTracksPassedCuts.push_back(0);
    if (m_accept.addCut( cutFamilyName, "Selection of tracks according to " + cutFamilyName ) < 0) {
      ATH_MSG_ERROR( "Failed to add cut family " << cutFamilyName << " because the TAccept object is full." );
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("Adding cut family " << cutFamilyName);
  }

  m_isInitialized = true;
  
  return StatusCode::SUCCESS;
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
  for (const auto& cutFamily : m_trackCuts) {
    ULong64_t numPassed = m_numTracksPassedCuts.at(m_accept.getCutPosition(cutFamily.first));
    ATH_MSG_INFO( numPassed << " = " << numPassed*100./m_numTracksProcessed << "% passed "
		  << cutFamily.first << " cut." );
  }
  return StatusCode::SUCCESS;
}


/// Accessor function to the tool's Root::TAccept object, which stores the result of
/// the last accept(...) call. The TAccept object itself is expensive to copy,
/// and so should be accessed by const reference.
/// 
const Root::TAccept& InDet::InDetTrackSelectionTool::getTAccept() const
{  
  return m_accept;
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
const Root::TAccept&
InDet::InDetTrackSelectionTool::accept( const xAOD::IParticle* p ) const
{
  // Reset the result:
  m_accept.clear();
    
  // Check if this is a track:
  if( p->type() != xAOD::Type::TrackParticle ) {
    ATH_MSG_ERROR( "accept(...) Function received a non-track" );
    return m_accept;
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
/// @returns A Root::TAccept object that can be treated as a boolean for a simple
///          pass/fail. It does store more detailed information about which cuts
///          passed and which failed, by organizing the results into cut families.
///          For example, TAccept::getCutResult("Z0SinTheta") will be true if the
///          track passed cuts on Z0*Sin(theta), its uncertainty, and its
///          significance.
/// 
const Root::TAccept& InDet::InDetTrackSelectionTool::accept( const xAOD::TrackParticle& trk,
							     const xAOD::Vertex* vtx ) const
{
  if (!m_isInitialized) {
    if (!m_warnInit) {
      ATH_MSG_WARNING( "Tool is not initialized! Calling accept() will not be very helpful." );
      m_warnInit = true;
    }
  }

  // Reset the result:
  m_accept.clear();

  bool passAll = true;
  // access all the track properties we will need
  for ( auto& accessor : m_trackAccessors ) {
    if(!accessor.second->access( trk, vtx ).isSuccess()) {
      ATH_MSG_WARNING("Track access for " << accessor.first << " unsuccessful.");
    }
  }
  // loop over all cuts
  UShort_t cutFamilyIndex = 0;
  for ( const auto& cutFamily : m_trackCuts ) {
    bool pass = true;
    //    const std::string& cutFamilyName = cutFamily.first;
    //const auto& cut1 = cutFamily.second;
    //    ATH_MSG_INFO("cut/value "<<cutFamilyName<<": ");

    for ( const auto& cut : cutFamily.second ) {
      if (! cut->result() ) {
	pass = false;
	passAll = false;
	break;
      }
    }
    m_accept.setCutResult( cutFamilyIndex, pass );
    if (pass) m_numTracksPassedCuts.at(cutFamilyIndex)++; // number of tracks that pass each cut family
    cutFamilyIndex++;
  }

  if (passAll) m_numTracksPassed++;
  m_numTracksProcessed++;
  
  return m_accept;
}

#ifndef XAOD_ANALYSIS
/// Function that returns the result of the cuts to a Trk::Track.
///
/// This version of accept() is included to allow the tool to be used
/// in full-blown athena on Trk::Tracks. It is supposed to have identical
/// functionality to the user as the xAOD version above, but of course it
/// does need to access track information differently.
/// 
const Root::TAccept&
InDet::InDetTrackSelectionTool::accept( const Trk::Track& track,
					const Trk::Vertex* vertex ) const
{
  if (!m_isInitialized) ATH_MSG_WARNING( "Tool is not initialized! Calling accept() will not be very helpful." );

  m_accept.clear();

  const Trk::TrackParameters* perigee = track.perigeeParameters();

  if ( perigee == nullptr || !perigee->covariance() ) {
    ATH_MSG_WARNING( "Track preselection: Zero pointer to parameterbase* received (most likely a track without perigee). This track will not pass any cuts." );
    return m_accept;
  }

  std::unique_ptr<const Trk::TrackParameters> paramsAtVertex;
  if (vertex) {
    Trk::PerigeeSurface perigeeSurface(vertex->position());
    paramsAtVertex.reset(m_extrapolator->extrapolate(*perigee,perigeeSurface,
						     Trk::anyDirection,true,
						     track.info().particleHypothesis()) );
    perigee = paramsAtVertex.get();
  }

  if ( perigee == nullptr || !perigee->covariance() ) {
    ATH_MSG_INFO( "Track preselection: cannot make a measured perigee. This track will not pass any cuts." );
    if (!m_initTrkTools)
      ATH_MSG_INFO( "The user should set \"UseTrkTrackTools\" to true if they want the extrapolation tool to try to get a perigee." );
    return m_accept;
  }

  const Trk::TrackSummary* summary = track.trackSummary();
  if (summary == nullptr && m_trackSumToolAvailable) {
    // unfortunately the const must be cast away because the tool needs
    // to update the track to make a summary (it is a friend of the track)
    Trk::Track& nonConstTrack = const_cast<Trk::Track&>(track);
    m_trackSumTool->updateTrack(nonConstTrack);
    // now get the summary from the track (the track has ownership)
    summary = nonConstTrack.trackSummary();
  }
  if (summary == nullptr) {
    ATH_MSG_INFO( "Track preselection: cannot get a track summary. This track will not pass any cuts." );
    if (!m_initTrkTools)
      ATH_MSG_INFO( "The Trk::Track tools were not set to be initialized. The user should set the property \"UseTrkTrackTools\" to true if they wish to use the summary tool." );
    return m_accept;
  }

  bool passAll = true;
  // access all the track properties we will need
  for ( auto& accessor : m_trackAccessors ) {
    if(!accessor.second->access( track, perigee, summary ).isSuccess()) {
      ATH_MSG_WARNING("Track access for " << accessor.first << " unsuccessful.");
    }
  }
  // for faster lookup in setCutResult we will keep track of the index explicitly
  UShort_t cutFamilyIndex = 0;
  for ( const auto& cutFamily : m_trackCuts ) {
    bool pass = true;
    for ( const auto& cut : cutFamily.second ) {
      if (! cut->result() ) {
	pass = false;
	passAll = false;
	break;
      }
    }
    m_accept.setCutResult( cutFamilyIndex, pass );
    if (pass)
      m_numTracksPassedCuts.at(cutFamilyIndex)++; // increment the number of tracks that passed this cut family
    cutFamilyIndex++;
  }
  
  if (passAll)
    m_numTracksPassed++;
  m_numTracksProcessed++;

  return m_accept;
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
void InDet::InDetTrackSelectionTool::setCutLevel(InDet::CutLevel level, Bool_t overwrite )
{
#ifndef XAOD_STANDALONE
  ATH_MSG_WARNING( "InDetTrackSelectionTool::setCutLevel() is not designed to be called manually in Athena." );
  ATH_MSG_WARNING( "It may not behave as intended. Instead, configure it in the job options through the CutLevel property." );
#endif // XAOD_STANDALONE
  if (m_cutLevel != "") {
    ATH_MSG_WARNING( "Cut level already set to " << m_cutLevel << ".  Calling setCutLevel() is not expected." );
  }
  setCutLevelPrivate(level, overwrite);
}

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
      m_maxNSiSharedModules = LOCAL_MAX_INT;
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
    if (overwrite || m_maxNSiSharedModules >= LOCAL_MAX_INT) m_maxNSiSharedModules = 1;
    if (overwrite || m_maxNSiHoles >= LOCAL_MAX_INT) m_maxNSiHoles = 2;
    if (overwrite || m_maxNPixelHoles >= LOCAL_MAX_INT) m_maxNPixelHoles = 1;
    break;
  case CutLevel::LoosePrimary :
    setCutLevelPrivate(CutLevel::NoCut, overwrite); // implement loose cuts first
    if (overwrite || m_maxAbsEta >= LOCAL_MAX_DOUBLE) m_maxAbsEta = 2.5;
    if (overwrite || m_minNSiHits < 0) m_minNSiHits = 7;
    if (overwrite || m_maxNSiSharedModules >= LOCAL_MAX_INT) m_maxNSiSharedModules = 1;
    if (overwrite || m_maxNSiHoles >= LOCAL_MAX_INT) m_maxNSiHoles = 2;
    if (overwrite || m_maxNPixelHoles >= LOCAL_MAX_INT) m_maxNPixelHoles = 1;
    if (overwrite || m_minNSiHitsIfSiSharedHits < 0) m_minNSiHitsIfSiSharedHits = 10;
    break;
  case CutLevel::TightPrimary :
    setCutLevelPrivate(CutLevel::NoCut, overwrite);
    if (overwrite || m_maxAbsEta >= LOCAL_MAX_DOUBLE) m_maxAbsEta = 2.5;
    if (overwrite || m_minNSiHits < 0) m_minNSiHits = 9;
    if (overwrite || m_maxNSiSharedModules >= LOCAL_MAX_INT) m_maxNSiSharedModules = 1;
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
