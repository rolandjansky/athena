/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#include "xAODTracking/TrackingPrimitives.h"

#ifndef XAOD_ANALYSIS
#include "TrkTrack/Track.h"
#include "VxVertex/Vertex.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/FitQuality.h"
#endif

#include <memory>
#include <limits>

namespace {
  Double_t LOCAL_MAX_DOUBLE = 1.0e16;
  Int_t LOCAL_MAX_INT = std::numeric_limits<Int_t>::max();
}

InDet::InDetTrackSelectionTool::InDetTrackSelectionTool(const std::string& name)
  : asg::AsgTool(name)
  , m_accept( "InDetTrackSelection" )
  , m_cutLevel("")
#ifndef XAOD_ANALYSIS
  , m_initTrkTools(false)
  , m_trackSumTool("Trk::TrackSummaryTool/TrackSummaryTool", this)
  , m_extrapolator("Trk::Extrapolator/Extrapolator", this)
#endif // XAOD_ANALYSIS
{

  // set the cut selection: default is "no cut"
  setCutLevel(InDet::CutLevel::NoCut);

#ifndef XAOD_STANDALONE
  declareInterface<IInDetTrackSelectionTool>(this);
#endif

  // TODO: make object that links cut values, strings, and TrackCuts.
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
  //declareProperty("minNBLayersHits", m_minNBLayersHits,
  //		  "Required B layer hits (including IBL)");
  //declareProperty("maxNBLayersSharedHits", m_maxNBLayersSharedHits,
  //		  "Maximum shared hits in B layers");
  declareProperty("minNInnermostLayerHits", m_minNInnermostLayerHits,
		  "Required hits on the innermost pixel layer");
  declareProperty("minNNextToInnermostLayerHits", m_minNNextToInnermostLayerHits,
		  "Required hits on the next to innermost pixel layer");
  declareProperty("minNBothInnermostLayersHits", m_minNBothInnermostLayersHits,
		  "Required hits on two innermost pixel layers");
  declareProperty("maxNInnermostLayerSharedHits", m_maxNInnermostLayerSharedHits,
		  "Maximum shared hits in innermost pixel layer");
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
  declareProperty("minNSctHits", m_minNSctHits, "Minimum SCT hits");
  declareProperty("maxNSctSharedHits", m_maxNSctSharedHits, "Maximum SCT hits shared with other track");
  declareProperty("maxNSctHoles", m_maxNSctHoles, "Maximum SCT holes");
  declareProperty("maxNSctDoubleHoles", m_maxNSctDoubleHoles, "Maximum SCT double holes");
  declareProperty("maxTrtEtaAcceptance", m_maxTrtEtaAcceptance,
		  "Maximum eta that ignores TRT hit cuts");
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
  declareProperty("maxChiSq", m_maxChiSq, "Maximum chi squared");
  declareProperty("minProb", m_minProb, "Minimum p(chi^2, Ndof)");
  declareProperty("maxChiSqperNdf", m_maxChiSqperNdf,
		  "Maximum chi squared per degree of freedom");
  // TODO: implement pt-dependent SCT cut

  declareProperty("CutLevel", m_cutLevel);
#ifndef XAOD_ANALYSIS
  declareProperty("UseTrkTrackTools", m_initTrkTools);
  declareProperty("TrackSummaryTool", m_trackSumTool);
  declareProperty("Extrapolator", m_extrapolator);
  m_trackSumToolAvailable = false;
#endif
}


/// Initializes the selection tool, setting up the cut objects as well as any additional tools it needs.
/// 
/// In athena, this is called automatically after the user has set the properties and so
/// it must set the cut level without overwriting any additional cuts that have been set.
/// Unfortunately there is not a clean way to unset a cut from a given pre-defined cut level
/// in athena (though this is trivial in standalone root) - the user either select a pre-defined
/// cut level that is less restrictive and then manually add the cuts, or, if it is a cut on a
/// maximum value, set the cut to something less than LOCAL_MAX_[DOUBLE|INT] (defined above)
/// but still greater than the range of phisical values.
/// 
/// In standalone root, the cut levels can be set in a more or less intuitive way. After setting
/// the cut levels and providing any additional configuration, this initialize() function must
/// be called explicitly.
/// 
StatusCode InDet::InDetTrackSelectionTool::initialize() {

  // Greet the user:
  ATH_MSG_INFO( "Initialising Track Selection Tool..." );
  ASG_CHECK( asg::AsgTool::initialize() );

  // if the CutLevel string is set to something recognizable,
  // then do a soft set on the cuts (not overwriting those already
  // set when the tool is created by Athena)
  if (m_cutLevel != "") {
    std::unordered_map<std::string, InDet::CutLevel>::const_iterator it_mapCutLevel = s_mapCutLevel.find(m_cutLevel);
    if ( it_mapCutLevel == s_mapCutLevel.end() ) {
      ATH_MSG_WARNING( "The string \"" << m_cutLevel << "\" is not recognized as a cut level. No cuts will be changed." );
      ATH_MSG_WARNING( "Cut level options are:" );
      for (const auto& opt : s_mapCutLevel) {
	ATH_MSG_WARNING( "\t" << opt.first );
      }
    } else {
      ATH_MSG_INFO( "Cut level set to \"" << it_mapCutLevel->first << "\". This will not overwrite other cuts if they are otherwise set to something other than the default (no cut).");
      setCutLevel( it_mapCutLevel->second, false );
    }
  }

#ifndef XAOD_ANALYSIS
  if (m_initTrkTools) {
    m_trackSumToolAvailable = false;
    if (!m_trackSumTool.empty()) {
      ASG_CHECK( m_trackSumTool.retrieve() );
      ATH_MSG_INFO( "Track summary tool retrieved." );
      m_trackSumToolAvailable = true;
    }
    ASG_CHECK( m_extrapolator.retrieve() );
    ATH_MSG_INFO( "Retrieved tool " << m_extrapolator );
  }
#endif // XAOD_ANALYSIS

  // create the cuts and initialize them (which will create the necessary accessors).
  // tell the user which cuts are recognized.
  if (m_minPt > 0) {
    ATH_MSG_INFO( "  Minimum Pt: " << m_minPt << " MeV" );
    std::unique_ptr<PtCut> pt(new PtCut(this));
    pt->setMinValue( m_minPt );
    ASG_CHECK( pt->initialize() );
    m_trackCuts["Pt"].push_back(std::move(pt));
  }
  if (m_maxAbsEta < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum |Eta|: " << m_maxAbsEta );
    std::unique_ptr<EtaCut> eta(new EtaCut(this));
    eta->setMaxValue( m_maxAbsEta );
    ASG_CHECK( eta->initialize() );
    m_trackCuts["Eta"].push_back(std::move(eta));
  }
  if (m_minP > 0) {
    ATH_MSG_INFO( "  Minimum P: " << m_minP << " MeV" );
    std::unique_ptr<PCut> p(new PCut(this));
    p->setMinValue( m_minP );
    ASG_CHECK( p->initialize() );
    m_trackCuts["P"].push_back(std::move(p));
  }
  if (m_maxD0 < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum d0: " << m_maxD0 << " mm" );
    std::unique_ptr<D0Cut> d0(new D0Cut(this));
    d0->setMaxValue( m_maxD0 );
    ASG_CHECK( d0->initialize() );
    m_trackCuts["D0"].push_back(std::move(d0));
  }
  if (m_maxZ0 < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum z0: " << m_maxZ0 << " mm");
    std::unique_ptr<Z0Cut> z0(new Z0Cut(this));
    z0->setMaxValue( m_maxZ0 );
    ASG_CHECK( z0->initialize() );
    m_trackCuts["Z0"].push_back(std::move(z0));
  }
  if (m_maxZ0SinTheta < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum z0*sin(theta): " << m_maxZ0SinTheta << " mm" );
    std::unique_ptr<Z0SinThetaCut> z0SinTheta(new Z0SinThetaCut(this));
    z0SinTheta->setMaxValue( m_maxZ0SinTheta );
    ASG_CHECK( z0SinTheta->initialize() );
    m_trackCuts["Z0SinTheta"].push_back(std::move(z0SinTheta));
  }
  if (m_maxSigmaD0 < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum uncertainty on d0: " << m_maxSigmaD0 << " mm" );
    std::unique_ptr<D0SigmaCut> d0(new D0SigmaCut(this));
    d0->setMaxValue( m_maxSigmaD0 );
    ASG_CHECK( d0->initialize() );
    m_trackCuts["D0"].push_back(std::move(d0));
  }
  if (m_maxSigmaZ0 < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum uncertainty on z0: " << m_maxSigmaZ0 << " mm" );
    std::unique_ptr<Z0SigmaCut> z0(new Z0SigmaCut(this));
    z0->setMaxValue( m_maxSigmaZ0 );
    ASG_CHECK( z0->initialize() );
    m_trackCuts["Z0"].push_back(std::move(z0));
  }
  if (m_maxSigmaZ0SinTheta < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum uncertainty on z0*sin(theta): "
  		     << m_maxSigmaZ0SinTheta << " mm" );
    std::unique_ptr<Z0SinThetaSigmaCut> z0SinTheta(new Z0SinThetaSigmaCut(this));
    z0SinTheta->setMaxValue( m_maxSigmaZ0SinTheta );
    ASG_CHECK( z0SinTheta->initialize() );
    m_trackCuts["Z0SinTheta"].push_back(std::move(z0SinTheta));
  }
  if (m_maxD0overSigmaD0 < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum significance on d0: " << m_maxD0overSigmaD0 );
    std::unique_ptr<D0SignificanceCut> d0(new D0SignificanceCut(this));
    d0->setMaxValue( m_maxD0overSigmaD0 );
    ASG_CHECK( d0->initialize() );
    m_trackCuts["D0"].push_back(std::move(d0));
  }
  if (m_maxZ0overSigmaZ0 < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum significance on z0: " << m_maxZ0overSigmaZ0 );
    std::unique_ptr<Z0SignificanceCut> z0(new Z0SignificanceCut(this));
    z0->setMaxValue( m_maxZ0overSigmaZ0 );
    ASG_CHECK( z0->initialize() );
    m_trackCuts["Z0"].push_back(std::move(z0));
  }
  if (m_maxZ0SinThetaoverSigmaZ0SinTheta < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum significance on z0*sin(theta): "
  		     << m_maxZ0SinThetaoverSigmaZ0SinTheta );
    std::unique_ptr<Z0SinThetaSignificanceCut> z0SinTheta(new Z0SinThetaSignificanceCut(this));
    z0SinTheta->setMaxValue( m_maxZ0SinThetaoverSigmaZ0SinTheta );
    ASG_CHECK( z0SinTheta->initialize() );
    m_trackCuts["Z0SinTheta"].push_back(std::move(z0SinTheta));
  }
  if (m_minNInnermostLayerHits > 0) {
    ATH_MSG_INFO( "  Minimum hits from innermost pixel layer: " << m_minNInnermostLayerHits );
    std::unique_ptr<MinSummaryValueCut> minInnermostLayerHits(new MinSummaryValueCut(this));
    minInnermostLayerHits->setMinValue(m_minNInnermostLayerHits);
    minInnermostLayerHits->addSummaryType( xAOD::numberOfInnermostPixelLayerHits );
    ASG_CHECK( minInnermostLayerHits->initialize() );
    m_trackCuts["InnermostLayersHits"].push_back(std::move(minInnermostLayerHits));
  }
  if (m_minNNextToInnermostLayerHits > 0) {
    ATH_MSG_INFO( "  Minimum hits from next to innermost pixel layer: " << m_minNNextToInnermostLayerHits );
    std::unique_ptr<MinSummaryValueCut> minNextToInnermostLayerHits(new MinSummaryValueCut(this));
    minNextToInnermostLayerHits->setMinValue(m_minNNextToInnermostLayerHits);
    minNextToInnermostLayerHits->addSummaryType( xAOD::numberOfNextToInnermostPixelLayerHits );
    ASG_CHECK( minNextToInnermostLayerHits->initialize() );
    m_trackCuts["InnermostLayersHits"].push_back(std::move(minNextToInnermostLayerHits));
  }
  if (m_minNBothInnermostLayersHits > 0) {
    ATH_MSG_INFO( "  Minimum hits from both innermost pixel layers: " << m_minNBothInnermostLayersHits );
    std::unique_ptr<MinSummaryValueCut> minBothInnermostLayersHits(new MinSummaryValueCut(this));
    minBothInnermostLayersHits->setMinValue(m_minNBothInnermostLayersHits);
    minBothInnermostLayersHits->addSummaryType( xAOD::numberOfInnermostPixelLayerHits );
    minBothInnermostLayersHits->addSummaryType( xAOD::numberOfNextToInnermostPixelLayerHits );
    ASG_CHECK( minBothInnermostLayersHits->initialize() );
    m_trackCuts["InnermostLayersHits"].push_back(std::move(minBothInnermostLayersHits));
  }
  if (m_maxNInnermostLayerSharedHits < LOCAL_MAX_INT) {
    ATH_MSG_INFO( "  Maximum shared hits in innermost pixel layer: " << m_maxNInnermostLayerSharedHits );
    std::unique_ptr<MaxSummaryValueCut> maxInnermostShared(new MaxSummaryValueCut(this));
    maxInnermostShared->setMaxValue(m_maxNInnermostLayerSharedHits);
    maxInnermostShared->addSummaryType( xAOD::numberOfInnermostPixelLayerSharedHits );
    ASG_CHECK( maxInnermostShared->initialize() );
    m_trackCuts["InnermostLayersHits"].push_back(std::move(maxInnermostShared));
  }
  if (m_minNPixelHits > 0) {
    ATH_MSG_INFO( "  Minimum pixel hits: " << m_minNPixelHits );
    std::unique_ptr<MinSummaryValueCut> minPixHits(new MinSummaryValueCut(this));
    minPixHits->setMinValue(m_minNPixelHits);
    minPixHits->addSummaryType( xAOD::numberOfPixelHits );
    minPixHits->addSummaryType( xAOD::numberOfPixelDeadSensors );
    ASG_CHECK( minPixHits->initialize() );
    m_trackCuts["PixelHits"].push_back(std::move(minPixHits));
  }
  if (m_minNPixelHitsPhysical > 0) {
    ATH_MSG_INFO( "  Minimum physical pixel hits (i.e. dead sensors do not count): "
  		  << m_minNPixelHitsPhysical );
    std::unique_ptr<MinSummaryValueCut> minPixelPhysHits(new MinSummaryValueCut(this));
    minPixelPhysHits->setMinValue(m_minNPixelHitsPhysical);
    minPixelPhysHits->addSummaryType( xAOD::numberOfPixelHits );
    ASG_CHECK( minPixelPhysHits->initialize() );
    m_trackCuts["PixelHits"].push_back(std::move(minPixelPhysHits));
  }
  if (m_maxNPixelHoles < LOCAL_MAX_INT) {
    ATH_MSG_INFO( "  Maximum pixel holes: " << m_maxNPixelHoles );
    std::unique_ptr<MaxSummaryValueCut> maxPixHoles(new MaxSummaryValueCut(this));
    maxPixHoles->setMaxValue(m_maxNPixelHoles);
    maxPixHoles->addSummaryType( xAOD::numberOfPixelHoles );
    ASG_CHECK( maxPixHoles->initialize() );
    m_trackCuts["PixelHits"].push_back(std::move(maxPixHoles));
  }
  if (m_maxNPixelSharedHits < LOCAL_MAX_INT) {
    ATH_MSG_INFO( "  Maximum pixel shared hits: " << m_maxNPixelSharedHits );
    std::unique_ptr<MaxSummaryValueCut> maxPixSharedHits(new MaxSummaryValueCut(this));
    maxPixSharedHits->setMaxValue(m_maxNPixelSharedHits);
    maxPixSharedHits->addSummaryType( xAOD::numberOfPixelSharedHits );
    ASG_CHECK( maxPixSharedHits->initialize() );
    m_trackCuts["PixelHits"].push_back(std::move(maxPixSharedHits));
  }
  if (m_minNSctHits > 0) {
    ATH_MSG_INFO( "  Minimum SCT hits: " << m_minNSctHits );
    std::unique_ptr<MinSummaryValueCut> minSctHits(new MinSummaryValueCut(this));
    minSctHits->setMinValue(m_minNSctHits);
    minSctHits->addSummaryType( xAOD::numberOfSCTHits );
    minSctHits->addSummaryType( xAOD::numberOfSCTDeadSensors );
    ASG_CHECK( minSctHits->initialize() );
    m_trackCuts["SctHits"].push_back(std::move(minSctHits));
  }
  if (m_maxNSctHoles < LOCAL_MAX_INT) {
    ATH_MSG_INFO( "  Maximum SCT holes: " << m_maxNSctHoles );
    std::unique_ptr<MaxSummaryValueCut> maxSctHoles(new MaxSummaryValueCut(this));
    maxSctHoles->setMaxValue(m_maxNSctHoles);
    maxSctHoles->addSummaryType( xAOD::numberOfSCTHoles );
    ASG_CHECK( maxSctHoles->initialize() );
    m_trackCuts["SctHits"].push_back(std::move(maxSctHoles));
  }
  if (m_maxNSctSharedHits < LOCAL_MAX_INT) {
    ATH_MSG_INFO( "  Maximum SCT shared hits: " << m_maxNSctSharedHits );
    std::unique_ptr<MaxSummaryValueCut> maxSctSharedHits(new MaxSummaryValueCut(this));
    maxSctSharedHits->setMaxValue(m_maxNSctSharedHits);
    maxSctSharedHits->addSummaryType( xAOD::numberOfSCTSharedHits );
    ASG_CHECK( maxSctSharedHits->initialize() );
    m_trackCuts["SctHits"].push_back(std::move(maxSctSharedHits));
  }
  if (m_maxNSctDoubleHoles < LOCAL_MAX_INT) {
    ATH_MSG_INFO( "  Maximum SCT double holes: " << m_maxNSctDoubleHoles );
    std::unique_ptr<MaxSummaryValueCut> maxSctDoubleHoles(new MaxSummaryValueCut(this));
    maxSctDoubleHoles->setMaxValue(m_maxNSctDoubleHoles);
    maxSctDoubleHoles->addSummaryType( xAOD::numberOfSCTDoubleHoles );
    ASG_CHECK( maxSctDoubleHoles->initialize() );
    m_trackCuts["SctHits"].push_back(std::move(maxSctDoubleHoles));
  }
  if (m_minNSiHits > 0) {
    ATH_MSG_INFO( "  Minimum silicon (pixel + SCT) hits: " << m_minNSiHits );
    std::unique_ptr<MinSummaryValueCut> minSiHits(new MinSummaryValueCut(this));
    minSiHits->setMinValue(m_minNSiHits);
    minSiHits->addSummaryType( xAOD::numberOfPixelHits );
    minSiHits->addSummaryType( xAOD::numberOfSCTHits );
    minSiHits->addSummaryType( xAOD::numberOfPixelDeadSensors );
    minSiHits->addSummaryType( xAOD::numberOfSCTDeadSensors );
    ASG_CHECK( minSiHits->initialize() );
    m_trackCuts["SiHits"].push_back(std::move(minSiHits));
  }
  if (m_minNSiHitsPhysical > 0) {
    ATH_MSG_INFO( "  Minimum physical silicon hits (i.e. dead sensors do not count): "
  		  << m_minNSiHitsPhysical );
    std::unique_ptr<MinSummaryValueCut> minSiHitsPhys(new MinSummaryValueCut(this));
    minSiHitsPhys->setMinValue(m_minNSiHitsPhysical);
    minSiHitsPhys->addSummaryType( xAOD::numberOfPixelHits );
    minSiHitsPhys->addSummaryType( xAOD::numberOfSCTHits );
    ASG_CHECK( minSiHitsPhys->initialize() );
    m_trackCuts["SiHits"].push_back(std::move(minSiHitsPhys));
  }
  if (m_maxNSiHoles < LOCAL_MAX_INT) {
    ATH_MSG_INFO( "  Maximum silicon holes: " << m_maxNSiHoles );
    std::unique_ptr<MaxSummaryValueCut> maxSiHoles(new MaxSummaryValueCut(this));
    maxSiHoles->setMaxValue(m_maxNSiHoles);
    maxSiHoles->addSummaryType( xAOD::numberOfPixelHoles );
    maxSiHoles->addSummaryType( xAOD::numberOfSCTHoles );
    ASG_CHECK( maxSiHoles->initialize() );
    m_trackCuts["SiHits"].push_back(std::move(maxSiHoles));
  }
  if (m_maxNSiSharedHits < LOCAL_MAX_INT) {
    ATH_MSG_INFO( "  Maximum silicon shared hits: " << m_maxNSiSharedHits );
    std::unique_ptr<MaxSummaryValueCut> maxSiSharedHits(new MaxSummaryValueCut(this));
    maxSiSharedHits->setMaxValue(m_maxNSiSharedHits);
    maxSiSharedHits->addSummaryType( xAOD::numberOfPixelSharedHits );
    maxSiSharedHits->addSummaryType( xAOD::numberOfSCTSharedHits );
    ASG_CHECK( maxSiSharedHits->initialize() );
    m_trackCuts["SiHits"].push_back(std::move(maxSiSharedHits));
  }
  if (m_maxOneSharedModule) {
    ATH_MSG_INFO( "  No more than one shared module: i.e. max 1 shared pixel hit or two shared SCT hits, and not both." );
    std::unique_ptr<MaxOneSharedModule> oneSharedModule(new MaxOneSharedModule(this));
    ASG_CHECK( oneSharedModule->initialize() );
    m_trackCuts["SiHits"].push_back(std::move(oneSharedModule));
  }
  if (m_minNSiHitsIfSiSharedHits > 0) {
    ATH_MSG_INFO( "  Minimum silicon hits if the track has shared hits: "
		     << m_minNSiHitsIfSiSharedHits );
    std::unique_ptr<MinNSiHitsIfSiSharedHits> siHits(new MinNSiHitsIfSiSharedHits(this));
    siHits->setMinSiHits( m_minNSiHitsIfSiSharedHits );
    ASG_CHECK( siHits->initialize() );
    m_trackCuts["SiHits"].push_back(std::move(siHits));
  }
  if (m_minEtaForStrictNSiHitsCut < LOCAL_MAX_DOUBLE
      && m_minNSiHitsAboveEtaCutoff > 0) {
    ATH_MSG_INFO( "  Require " << m_minNSiHitsAboveEtaCutoff
		     << " silicon hits above eta = " << m_minEtaForStrictNSiHitsCut );
    std::unique_ptr<MinNSiHitsAboveEta> siHits(new MinNSiHitsAboveEta(this));
    siHits->setMinSiHits( m_minNSiHitsAboveEtaCutoff );
    siHits->setEtaCutoff( m_minEtaForStrictNSiHitsCut );
    ASG_CHECK( siHits->initialize() );
    m_trackCuts["SiHits"].push_back(std::move(siHits));
  }
  if (m_maxTrtEtaAcceptance < LOCAL_MAX_DOUBLE) {
    if (m_minNTrtHits > 0) {
      ATH_MSG_INFO( "  Minimum TRT hits above eta acceptance: " << m_minNTrtHits );
      std::unique_ptr<MinTrtHitCut> minTrtHits(new MinTrtHitCut(this));
      minTrtHits->setMinValue(m_minNTrtHits);
      minTrtHits->addSummaryType( xAOD::numberOfTRTHits );
      ASG_CHECK( minTrtHits->initialize() );
      m_trackCuts["TrtHits"].push_back(std::move(minTrtHits));
    }
    if (m_minNTrtHitsPlusOutliers > 0) {
      ATH_MSG_INFO( "  Minimum TRT hits above eta acceptance including outliers: " << m_minNTrtHitsPlusOutliers );
      std::unique_ptr<MinTrtHitCut> minTrtHits(new MinTrtHitCut(this));
      minTrtHits->setMinValue(m_minNTrtHitsPlusOutliers);
      minTrtHits->addSummaryType( xAOD::numberOfTRTHits );
      minTrtHits->addSummaryType( xAOD::numberOfTRTOutliers );
      ASG_CHECK( minTrtHits->initialize() );
      m_trackCuts["TrtHits"].push_back(std::move(minTrtHits));
    }
    if (m_minNTrtHighThresholdHits > 0) {
      ATH_MSG_INFO( "  Minimum TRT hits above eta acceptance above high energy threshold: "
		    << m_minNTrtHighThresholdHits );
      std::unique_ptr<MinTrtHitCut> minTrtHits(new MinTrtHitCut(this));
      minTrtHits->setMinValue(m_minNTrtHighThresholdHits);
      minTrtHits->addSummaryType( xAOD::numberOfTRTHighThresholdHits );
      ASG_CHECK( minTrtHits->initialize() );
      m_trackCuts["TrtHits"].push_back(std::move(minTrtHits));
    }
    if (m_minNTrtHighThresholdHitsPlusOutliers > 0) {
      ATH_MSG_INFO( "  Minimum TRT hits above eta acceptance above high energy threshold including outliers: "
		    << m_minNTrtHighThresholdHitsPlusOutliers );
      std::unique_ptr<MinTrtHitCut> minTrtHits(new MinTrtHitCut(this));
      minTrtHits->setMinValue(m_minNTrtHighThresholdHitsPlusOutliers);
      minTrtHits->addSummaryType( xAOD::numberOfTRTHighThresholdHits );
      minTrtHits->addSummaryType( xAOD::numberOfTRTHighThresholdOutliers );
      ASG_CHECK( minTrtHits->initialize() );
      m_trackCuts["TrtHits"].push_back(std::move(minTrtHits));
    }
    if (m_maxTrtHighEFraction < LOCAL_MAX_DOUBLE) {
      ATH_MSG_INFO( "  Maximum ratio of high threshold to regular TRT hits above eta acceptance: " << m_maxTrtHighEFraction);
      std::unique_ptr<MaxTrtHitRatioCut> maxTrtRatio(new MaxTrtHitRatioCut(this));
      maxTrtRatio->setMaxEtaAcceptance(m_maxTrtEtaAcceptance);
      maxTrtRatio->setMaxValue(m_maxTrtHighEFraction);
      maxTrtRatio->addSummaryTypeNumerator( xAOD::numberOfTRTHighThresholdHits );
      maxTrtRatio->addSummaryTypeDenominator( xAOD::numberOfTRTHits );
      ASG_CHECK( maxTrtRatio->initialize() );
      m_trackCuts["TrtHits"].push_back(std::move(maxTrtRatio));
    }
    if (m_maxTrtHighEFractionWithOutliers < LOCAL_MAX_DOUBLE) {
      ATH_MSG_INFO( "  Maximum ratio of high threshold to regular TRT hits above eta acceptance including outliers: " << m_maxTrtHighEFractionWithOutliers);
      std::unique_ptr<MaxTrtHitRatioCut> maxTrtRatio(new MaxTrtHitRatioCut(this));
      maxTrtRatio->setMaxEtaAcceptance(m_maxTrtEtaAcceptance);
      maxTrtRatio->setMaxValue(m_maxTrtHighEFraction);
      maxTrtRatio->addSummaryTypeNumerator( xAOD::numberOfTRTHighThresholdHits );
      maxTrtRatio->addSummaryTypeNumerator( xAOD::numberOfTRTHighThresholdOutliers );
      maxTrtRatio->addSummaryTypeDenominator( xAOD::numberOfTRTHits );
      maxTrtRatio->addSummaryTypeDenominator( xAOD::numberOfTRTOutliers );
      ASG_CHECK( maxTrtRatio->initialize() );
      m_trackCuts["TrtHits"].push_back(std::move(maxTrtRatio));
    }
  }
  if (m_useEtaDependentMaxChiSq) {
    ATH_MSG_INFO( "  Using pre-defined eta-dependent maximum chi squared." );
    std::unique_ptr<EtaDependentChiSqCut> chi(new EtaDependentChiSqCut(this));
    ASG_CHECK( chi->initialize() );
    m_trackCuts["FitQuality"].push_back(std::move(chi));
  }
  if (m_maxChiSq < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum chi squared: " << m_maxChiSq );
    std::unique_ptr<MaxChiSq> chi(new MaxChiSq(this));
    chi->setMaxValue(m_maxChiSq);
    ASG_CHECK( chi->initialize() );
    m_trackCuts["FitQuality"].push_back(std::move(chi));
  }
  if (m_maxChiSqperNdf < LOCAL_MAX_DOUBLE) {
    ATH_MSG_INFO( "  Maximum chi squared per degree of freedom: " << m_maxChiSqperNdf );
    std::unique_ptr<MaxChiSqPerNdf> chi(new MaxChiSqPerNdf(this));
    chi->setMaxValue(m_maxChiSqperNdf);
    ASG_CHECK( chi->initialize() );
    m_trackCuts["FitQuality"].push_back(std::move(chi));
  }
  if (m_minProb > 0) {
    ATH_MSG_INFO( "  Minimum chi squared probability: " << m_minProb );
    std::unique_ptr<MinProb> prob(new MinProb(this));
    prob->setMinValue(m_minProb);
    ASG_CHECK( prob->initialize() );
    m_trackCuts["FitQuality"].push_back(std::move(prob));
  }

  // Set up the TAccept object:
  for (const auto& cutFamily : m_trackCuts) {
    // TODO: this function returns the cut position. This could be stored so that
    // the cuts can be set by number and not a string lookup.
    m_accept.addCut( cutFamily.first, "Selection of tracks according to " + cutFamily.first );
    ATH_MSG_INFO("Adding cut family " << cutFamily.first);
  }

  // Return gracefully:
  return StatusCode::SUCCESS;
}
  
StatusCode InDet::InDetTrackSelectionTool::finalize()
{
  ATH_MSG_INFO("Finalizing track selection tool.");
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
  // Reset the result:
  m_accept.clear();

  // access all the track properties we will need
  for ( auto& accessor : m_trackAccessors ) {
    if(!accessor.second->access( trk, vtx ).isSuccess()) {
      ATH_MSG_WARNING("Track access for " << accessor.first << " unsuccessful.");
    }
  }
  // loop over all cuts
  for ( const auto& cutFamily : m_trackCuts ) {
    bool pass = true;
    for ( const auto& cut : cutFamily.second ) {
      if (! cut->result() ) {
	pass = false;
	break;
      }
    }
    // TODO: access by number of cut family, not by string lookup
    m_accept.setCutResult( cutFamily.first, pass );
  }

  // Return the result:
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


  // access all the track properties we will need
  for ( auto& accessor : m_trackAccessors ) {
    if(!accessor.second->access( track, perigee, summary ).isSuccess()) {
      ATH_MSG_WARNING("Track access for " << accessor.first << " unsuccessful.");
    }
  }
  for ( const auto& cutFamily : m_trackCuts ) {
    bool pass = true;
    for ( const auto& cut : cutFamily.second ) {
      if (! cut->result() ) {
	pass = false;
	break;
      }
    }
    m_accept.setCutResult( cutFamily.first, pass );
  }
  
 return m_accept;
}

#endif // XAOD_STANDALONE or XAOD_ANALYSIS

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
  // current cut levels are not necessarily final.
  switch (level) {
  case CutLevel::NoCut :
    if (overwrite) {
      m_minPt = 0.; // in MeV
      m_minP = 0.;
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
      m_minNInnermostLayerHits = 0;
      m_minNNextToInnermostLayerHits = 0;
      m_minNBothInnermostLayersHits = 0;
      m_maxNInnermostLayerSharedHits = LOCAL_MAX_INT;
      m_minNPixelHits = 0;
      m_minNPixelHitsPhysical = 0;
      m_maxNPixelSharedHits = LOCAL_MAX_INT;
      m_maxNPixelHoles = LOCAL_MAX_INT;
      m_minNSctHits = 0;
      m_maxNSctHoles = LOCAL_MAX_INT;
      m_maxNSctSharedHits = LOCAL_MAX_INT;
      m_maxNSctDoubleHoles = LOCAL_MAX_INT;
      m_minNSiHits = 0;
      m_minNSiHitsPhysical = 0;
      m_maxNSiSharedHits = LOCAL_MAX_INT;
      m_minNSiHitsIfSiSharedHits = 0;
      m_maxNSiHoles = LOCAL_MAX_INT;
      m_minEtaForStrictNSiHitsCut = LOCAL_MAX_DOUBLE;
      m_minNSiHitsAboveEtaCutoff = 0;
      m_maxOneSharedModule = false;
      m_maxTrtEtaAcceptance = LOCAL_MAX_DOUBLE;
      m_minNTrtHits = 0;
      m_minNTrtHitsPlusOutliers = 0;
      m_minNTrtHighThresholdHits = 0;
      m_minNTrtHighThresholdHitsPlusOutliers = 0;
      m_maxTrtHighEFraction = LOCAL_MAX_DOUBLE;
      m_maxTrtHighEFractionWithOutliers = LOCAL_MAX_DOUBLE;
      m_maxChiSq = LOCAL_MAX_DOUBLE;
      m_minProb = 0.;
      m_maxChiSqperNdf = LOCAL_MAX_DOUBLE;
      m_useEtaDependentMaxChiSq = false;
    }
    break;
  case CutLevel::Loose :
    if (overwrite)
      setCutLevel(CutLevel::NoCut, overwrite); // if hard overwrite, reset all cuts first
    // change the cuts if a hard overwrite is asked for or if the cuts are unset
    if (overwrite || m_minPt <= 0)
      m_minPt = 400.;
    if (overwrite || m_maxAbsEta >= LOCAL_MAX_DOUBLE)
      m_maxAbsEta = 2.5;
    if (overwrite || m_minNSiHits <= 0)
      m_minNSiHits = 7;
    m_maxOneSharedModule = true;
    if (overwrite || m_maxNSiHoles >= LOCAL_MAX_INT)
      m_maxNSiHoles = 2;
    if (overwrite || m_maxNPixelHoles >= LOCAL_MAX_INT)
      m_maxNPixelHoles = 1;
    break;
  case CutLevel::LoosePrimary :
    setCutLevel(CutLevel::Loose, overwrite); // implement loose cuts first
    if (overwrite || m_minNSiHitsIfSiSharedHits <= 0)
      m_minNSiHitsIfSiSharedHits = 10;
    m_useEtaDependentMaxChiSq = true; // should get set regardless of "overwrite"
    break;
  case CutLevel::TightPrimary :
    setCutLevel(CutLevel::Loose, overwrite); // implement loose cuts first
    if (overwrite || m_minNSiHits <= 0)
      m_minNSiHits = 9;
    if (overwrite || m_minEtaForStrictNSiHitsCut >= LOCAL_MAX_DOUBLE)
      m_minEtaForStrictNSiHitsCut = 1.65;
    if (overwrite || m_minNSiHitsAboveEtaCutoff <= 0)
      m_minNSiHitsAboveEtaCutoff = 11;
    if (overwrite || m_minNBothInnermostLayersHits <= 0)
      m_minNBothInnermostLayersHits = 1;
    if (overwrite || m_maxNPixelHoles >= LOCAL_MAX_INT)
      m_maxNPixelHoles = 0;
    break;
  default:
    ATH_MSG_WARNING("CutLevel not recognized. Cut selection will remain unchanged.");
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
    {"TightPrimary", InDet::CutLevel::TightPrimary}
  };
