/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/ITRT_ElectronPidTool.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkParameters/TrackParameters.h"
#include "Identifier/Identifier.h"

#include "GaudiKernel/EventContext.h"
#include <initializer_list>
#include <algorithm> //std::min/max
#include <cmath> //std::sqrt

namespace {
template<class Indexable, class IntegerArray>
void
setTheseElements(Indexable& container,
                 const IntegerArray& indices,
                 const typename Indexable::value_type& toValue)
{
  for (const auto idx : indices) {
    container[idx] = toValue;
  }
}
  //
template<class Indexable>
void
setTheseElements(Indexable& container,
                 const std::initializer_list<size_t>& indices,
                 const typename Indexable::value_type& toValue)
{
  for (const auto idx : indices) {
    container[idx] = toValue;
  }
}
}


Trk::TrackSummaryTool::TrackSummaryTool(const std::string& t,
  const std::string& n,
  const IInterface*  p )
  :
  base_class(t,n,p),
  m_detID{}{
  declareInterface<ITrackSummaryTool>(this);
}


Trk::TrackSummaryTool::~TrackSummaryTool(){
}


StatusCode
Trk::TrackSummaryTool::initialize(){
  ATH_CHECK( detStore()->retrieve(m_detID, "AtlasID" ));
  if (m_idTool.empty() && m_muonTool.empty()) {
      ATH_MSG_WARNING ("Could get neither InDetHelperTool nor MuonHelperTool.");
  }
  if (not m_idTool.empty()) ATH_CHECK(m_idTool.retrieve());
  if (not m_eProbabilityTool.empty()) ATH_CHECK(m_eProbabilityTool.retrieve());
  if (not m_dedxtool.empty()) ATH_CHECK(m_dedxtool.retrieve());
  if (not m_muonTool.empty()) ATH_CHECK(m_muonTool.retrieve());
  auto bool2Txt = [](const bool flag)->std::string {return flag?"ON":"OFF";};
  const std::string indetOnOff=bool2Txt(m_doHolesInDet);
  ATH_MSG_INFO ("Search for InDet holes using external tool turned "<<indetOnOff);
  const std::string muonOnOff=bool2Txt(m_doHolesMuon);
  ATH_MSG_INFO ("Search for Muon holes using external tool turned "<<muonOnOff);
  ATH_MSG_DEBUG ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}


StatusCode
Trk::TrackSummaryTool::finalize(){
  return StatusCode::SUCCESS;
}

/*
 * Method that creates a new summary for the track.
 * First calculates a summary according to job options.
 * If one is present it uses it as starting point (cloning it).
 * But adds relavant info.
 * And then sets the Track Summary for the non-const
 * track.
 */
void
Trk::TrackSummaryTool::computeAndReplaceTrackSummary(
  const EventContext& ctx,
  Trk::Track& track,
  const Trk::PRDtoTrackMap* pPrdToTrackMap,
  bool suppress_hole_search) const
{
  track.setTrackSummary(createSummary(ctx,
                                      track,
                                      pPrdToTrackMap,
                                      m_doHolesInDet & !suppress_hole_search,
                                      m_doHolesMuon & !suppress_hole_search));
}

std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summary(const EventContext& ctx,
                               const Track& track) const
{
  return createSummary(ctx, track, nullptr, m_doHolesInDet, m_doHolesMuon);
}

std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summary(const EventContext& ctx,
                               const Track& track,
                               const Trk::PRDtoTrackMap* pPrdToTrackMap) const
{
  return createSummary(
    ctx, track, pPrdToTrackMap, m_doHolesInDet, m_doHolesMuon);
}

std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summaryNoHoleSearch(const EventContext& ctx,
                                           const Track& track) const
{
  return createSummary(ctx, track, nullptr, false, false);
}

std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summaryNoHoleSearch(
  const EventContext& ctx,
  const Track& track,
  const Trk::PRDtoTrackMap* pPrdToTrackMap) const{
  return createSummary(ctx,track, pPrdToTrackMap, false, false);
}

// Method to create a new summary
std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::createSummary(const EventContext& ctx,
                                     const Track& track,
                                     const Trk::PRDtoTrackMap* pPrdToTrackMap,
                                     bool doHolesInDet,
                                     bool doHolesMuon) const
{
  std::unique_ptr<Trk::TrackSummary> ts;

  // first check if track has summary already and then clone it.
  if (track.trackSummary() != nullptr) {
    ATH_MSG_DEBUG(
      "Return cached summary for author : " << track.info().dumpInfo());
    ts = std::make_unique<Trk::TrackSummary>(*(track.trackSummary()));
  } else {
    ts = std::make_unique<Trk::TrackSummary>();
  }
  // fill the summary
  fillSummary(ctx, *ts, track, pPrdToTrackMap, doHolesInDet, doHolesMuon);
  return ts;
}

// Method filling the summary with information
void
Trk::TrackSummaryTool::fillSummary(const EventContext& ctx,
                                   Trk::TrackSummary& ts,
                                   const Trk::Track& track,
                                   const Trk::PRDtoTrackMap* prdToTrackMap,
                                   bool doHolesInDet,
                                   bool doHolesMuon) const
{
  std::vector<int>& information = ts.m_information;
  information.resize(std::min(information.size(),
                              static_cast<size_t>(numberOfTrackSummaryTypes)));

  // Troels.Petersen@cern.ch:
  std::vector<float> eProbability = Trk::eProbabilityDefault;;
  float dedx = -1;
  int nHitsUsed_dEdx = -1;
  int nOverflowHits_dEdx = -1;
  // Now set values to 0 for the ones we evaluate
  constexpr int toZero{0};
  if (!m_idTool.empty()) {
    if (m_pixelExists) {
      constexpr size_t numberOfPixelCounters{14};
      constexpr std::array<size_t, numberOfPixelCounters> atPixelIndices{
        numberOfContribPixelLayers,
        numberOfInnermostPixelLayerHits, numberOfInnermostPixelLayerOutliers,
        numberOfNextToInnermostPixelLayerHits, numberOfNextToInnermostPixelLayerOutliers,
        numberOfPixelHits, numberOfPixelOutliers,
        numberOfGangedPixels, numberOfGangedFlaggedFakes,
        numberOfPixelSpoiltHits, numberOfGangedFlaggedFakes,
        numberOfPixelSplitHits, numberOfInnermostLayerSplitHits, numberOfNextToInnermostLayerSplitHits
      };
      setTheseElements(information, atPixelIndices, toZero);
      if (track.info().trackFitter() != TrackInfo::Unknown && !m_dedxtool.empty()) {
        dedx = m_dedxtool->dEdx(ctx,track, nHitsUsed_dEdx, nOverflowHits_dEdx);
      }
      information[Trk::numberOfDBMHits] = 0;
    }
    constexpr size_t numberOfSctOrTrtCounters{11};
    constexpr std::array<size_t, numberOfSctOrTrtCounters> atSctOrTrtIndices{
      numberOfSCTHits, numberOfSCTSpoiltHits, numberOfSCTOutliers,
      numberOfTRTHits, numberOfTRTXenonHits, numberOfTRTHighThresholdHits, numberOfTRTHighThresholdHitsTotal,
      numberOfTRTOutliers, numberOfTRTHighThresholdOutliers, numberOfTRTTubeHits, numberOfTRTSharedHits
    };
    setTheseElements(information, atSctOrTrtIndices, toZero);
    if (!m_eProbabilityTool.empty()) {
      eProbability = m_eProbabilityTool->electronProbability(ctx,track);
      information[Trk::numberOfTRTHitsUsedFordEdx] = static_cast<int>(
        eProbability[Trk::eProbabilityNumberOfTRTHitsUsedFordEdx]);
    }
  }
  if (m_doSharedHits) {
    information[numberOfSCTSharedHits] = 0;
    if (m_pixelExists) {
      setTheseElements(information,
                       { numberOfInnermostPixelLayerSharedHits,
                         numberOfNextToInnermostPixelLayerSharedHits,
                         numberOfPixelSharedHits },
                       toZero);
    }
  }
  if (!m_muonTool.empty()) {
    constexpr size_t numberOfMuonRelatedCounters{15};
    constexpr std::array<size_t, numberOfMuonRelatedCounters> atMuonIndices{
      numberOfMdtHits,
      numberOfTgcPhiHits, numberOfTgcEtaHits,
      numberOfCscPhiHits, numberOfCscEtaHits, numberOfCscUnspoiltEtaHits,
      numberOfRpcPhiHits, numberOfRpcEtaHits,
      Trk::numberOfMdtHoles,
      Trk::numberOfTgcEtaHoles, Trk::numberOfTgcPhiHoles,
      Trk::numberOfCscEtaHoles, Trk::numberOfCscPhiHoles,
      Trk::numberOfRpcEtaHoles, Trk::numberOfRpcPhiHoles,
    };
    // New Small Wheel
    constexpr size_t numberOfNswRelatedCounters{6};
    constexpr std::array<size_t, numberOfNswRelatedCounters> atNswIndices{
      Trk::numberOfStgcEtaHits, Trk::numberOfStgcPhiHits, Trk::numberOfMmHits,
      Trk::numberOfStgcEtaHoles, Trk::numberOfStgcPhiHoles, Trk::numberOfMmHoles
    };
    setTheseElements(information, atMuonIndices, toZero);
    setTheseElements(information, atNswIndices, toZero);
  }

  std::bitset<numberOfDetectorTypes> hitPattern;
  ATH_MSG_DEBUG ("Produce summary for: "<<track.info().dumpInfo());
  if (track.trackStateOnSurfaces()) {
    information[Trk::numberOfOutliersOnTrack] = 0;
    processTrackStates(ctx,
                       track,
                       prdToTrackMap,
                       track.trackStateOnSurfaces(),
                       information,
                       hitPattern,
                       doHolesInDet,
                       doHolesMuon);
  } else {
    ATH_MSG_WARNING("Null pointer to TSoS found on Track (author = "
                    << track.info().dumpInfo()
                    << "). This should never happen! ");
  }

  bool holeSearchDone = (
    information[Trk::numberOfPixelHoles] != -1 &&
    information[Trk::numberOfSCTHoles] != -1 &&
    information[Trk::numberOfSCTDoubleHoles] != -1 &&
    information[Trk::numberOfPixelDeadSensors] != -1 &&
    information[Trk::numberOfSCTDeadSensors] != -1
  );

  if ((doHolesInDet || doHolesMuon) && (!holeSearchDone || m_alwaysRecomputeHoles.value())){
    if (m_pixelExists){
      information [numberOfPixelHoles] = 0;
    }
    information [numberOfSCTHoles]       = 0;
    information [numberOfSCTDoubleHoles] = 0;
    searchHolesStepWise(track,information, doHolesInDet, doHolesMuon);
  }

  ts.m_eProbability = eProbability;
  ts.m_idHitPattern = hitPattern.to_ulong();
  ts.m_dedx = dedx;
  ts.m_nhitsdedx = nHitsUsed_dEdx;
  ts.m_nhitsoverflowdedx = nOverflowHits_dEdx;

  // add detailed summary for indet
  if( m_addInDetDetailedSummary && !m_idTool.empty() ){
    m_idTool->addDetailedTrackSummary(ctx,track,ts);
  }
  // add detailed summary for muons
  if( m_addMuonDetailedSummary && !m_muonTool.empty() ){
    m_muonTool->addDetailedTrackSummary(ctx,track,ts);
  }
}

void
Trk::TrackSummaryTool::updateSharedHitCount(
  const Track& track,
  const Trk::PRDtoTrackMap* prdToTrackMap,
  TrackSummary& summary) const
{
  // first check if track has no summary - then it is recreated
  if (m_idTool){
    m_idTool->updateSharedHitCount(track, prdToTrackMap, summary);
  }
}

void
Trk::TrackSummaryTool::updateAdditionalInfo(const Track& track,
                                            TrackSummary& summary,
                                            bool initialiseToZero) const
{
  std::vector<float> eProbability = Trk::eProbabilityDefault;
  if (!m_eProbabilityTool.empty()) {
    eProbability = m_eProbabilityTool->electronProbability(track);
    int nHits = eProbability[Trk::eProbabilityNumberOfTRTHitsUsedFordEdx];
    if (!summary.update(Trk::numberOfTRTHitsUsedFordEdx, static_cast<uint8_t>(std::max(nHits,0)) )) {
      ATH_MSG_WARNING("Attempt to update numberOfTRTHitsUsedFordEdx but this summary information is "
                      "already set. numberOfTRTHitsUsedFordEdx is:" << summary.get(numberOfTRTHitsUsedFordEdx)
                      << " =?= should:" << nHits );
    }
  }
  const int initialValue(initialiseToZero ? 0 : -1);
  float dedx=             initialValue;
  int nHitsUsed_dEdx=     initialValue;
  int nOverflowHits_dEdx= initialValue;
  if (track.info().trackFitter() != TrackInfo::Unknown && !m_dedxtool.empty()) {
    dedx = m_dedxtool->dEdx(track, nHitsUsed_dEdx, nOverflowHits_dEdx);
  }
  if (m_idTool){
    m_idTool->updateAdditionalInfo(summary, eProbability,dedx, nHitsUsed_dEdx,nOverflowHits_dEdx);
    m_idTool->updateExpectedHitInfo(track, summary);
    if (m_addInDetDetailedSummary) m_idTool->addDetailedTrackSummary(track,summary);
  } else {
    ATH_MSG_INFO("No updates attempted, as the SummaryHelperTool is not defined.");
  }
}

/*
 * Then the internal helpers
 */
void
Trk::TrackSummaryTool::processTrackStates(
  const EventContext& ctx,
  const Track& track,
  const Trk::PRDtoTrackMap* prdToTrackMap,
  const DataVector<const TrackStateOnSurface>* tsos,
  std::vector<int>& information,
  std::bitset<numberOfDetectorTypes>& hitPattern,
  bool doHolesInDet,
  bool doHolesMuon) const
{
  ATH_MSG_DEBUG("Starting to process " << tsos->size() << " track states");
  int measCounter = 0;
  int cntAddChi2 = 0;
  float chi2Sum = 0;
  float chi2Sum2 = 0;
  DataVector<const TrackStateOnSurface>::const_iterator it = tsos->begin();
  DataVector<const TrackStateOnSurface>::const_iterator itEnd = tsos->end();
  for ( ; it!=itEnd; ++it){
    const auto & trackState = **it;
    const auto isMeasurement = trackState.type(Trk::TrackStateOnSurface::Measurement);
    const auto isOutlier = trackState.type(Trk::TrackStateOnSurface::Outlier);
    if (isMeasurement or isOutlier){
      ++measCounter;
      const Trk::MeasurementBase *measurement = trackState.measurementOnTrack();
      if (!measurement) {
        ATH_MSG_WARNING ("measurementOnTrack == null for a TrackStateOnSurface of type Measurement or Outlier");
      } else {
        if (isOutlier) ++information[Trk::numberOfOutliersOnTrack]; // increment outlier counter
        ATH_MSG_VERBOSE ("analysing TSoS " << measCounter << " of type " << trackState.dumpType() );
        processMeasurement(ctx,track, prdToTrackMap, measurement, *it, information, hitPattern);
      } // if have measurement pointer
    } // if type measurement, scatterer or outlier

    if (isMeasurement){
      if (const auto pFitQuality{trackState.fitQualityOnSurface()}; pFitQuality and pFitQuality->numberDoF()>0){
        ++cntAddChi2;
        if (const auto & chiSq{pFitQuality->chiSquared()}; chiSq > 1.e5) {// limit unphysical values and protect against FPE
          chi2Sum += 1.e5;
          chi2Sum2 += 1.e10;
          ATH_MSG_DEBUG ("TSOS has unphysical chi2: "<< chiSq);
        } else {
          const float chi2add = chiSq/pFitQuality->numberDoF();
          chi2Sum+=chi2add;
          chi2Sum2+=chi2add*chi2add;
        }
      }
    }

    if ( trackState.type(Trk::TrackStateOnSurface::Hole) && trackState.trackParameters() ){
      if (!doHolesInDet || !doHolesMuon ){ // no dedicated hole search via extrapolation, but take what might be on the track already.
        if ( trackState.trackParameters()->associatedSurface().associatedDetectorElement()!=nullptr ) {
          const Identifier& id = trackState.trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
          if ( !doHolesInDet && m_detID->is_pixel( id ) ) ++information[Trk::numberOfPixelHoles];
          if ( !doHolesInDet && m_detID->is_sct( id ) )    ++information[Trk::numberOfSCTHoles];
          if ( !doHolesMuon && m_detID->is_mdt( id ) )    ++information[Trk::numberOfMdtHoles];
        }
      }
    }
  } // end loop

  float varChi2 = 0;
  if (cntAddChi2>0){
    const auto inverseCount{1./cntAddChi2};
    varChi2 = (chi2Sum2 - (chi2Sum * chi2Sum * inverseCount)) * inverseCount;
  }
  if (varChi2>0 && varChi2<1.e13) information[Trk::standardDeviationOfChi2OS] = int(std::sqrt(varChi2)*100);
}

void
Trk::TrackSummaryTool::processMeasurement(
  const EventContext& ctx,
  const Track& track,
  const Trk::PRDtoTrackMap* prdToTrackMap,
  const Trk::MeasurementBase* meas,
  const Trk::TrackStateOnSurface* tsos,
  std::vector<int>& information,
  std::bitset<numberOfDetectorTypes>& hitPattern) const
{
  // Check if the measurement type is RIO on Track (ROT)
  const RIO_OnTrack* rot = nullptr;
  if (meas->type(Trk::MeasurementBaseType::RIO_OnTrack)) {
    rot = static_cast<const RIO_OnTrack*>(meas);
  }
  if ( rot ){
    // have RIO_OnTrack
    const Trk::IExtendedTrackSummaryHelperTool* tool = getTool(rot->identify());
    if (tool==nullptr){
      ATH_MSG_WARNING("Cannot find tool to match ROT. Skipping.");
    } else {
      tool->analyse(ctx,track,prdToTrackMap, rot,tsos,information, hitPattern);
    }
  } else {
    //check if the measurement type is CompetingRIOsOnTrack
    const Trk::CompetingRIOsOnTrack *compROT =nullptr;
    if ( meas->type(Trk::MeasurementBaseType::CompetingRIOsOnTrack)) {
      compROT = static_cast<const CompetingRIOsOnTrack *>(meas);
    }

    if (compROT) {
      // if this works we have a CompetingRIOsOnTrack.
      rot = &compROT->rioOnTrack(0); // get 1st rot
      const Trk::IExtendedTrackSummaryHelperTool* tool = getTool(rot->identify()); // Use 'main' ROT to get detector type
      if (tool==nullptr){
        ATH_MSG_WARNING("Cannot find tool to match cROT. Skipping.");
      } else {
        tool->analyse(ctx,track,prdToTrackMap, compROT,tsos,information, hitPattern);
      }
    }
  }
}

Trk::IExtendedTrackSummaryHelperTool*
Trk::TrackSummaryTool::getTool(const Identifier& id){
  if (m_detID->is_indet(id)){
    if (!m_idTool.empty()){
      return &*m_idTool;
    }
    ATH_MSG_WARNING("getTool: Identifier is from ID but have no ID tool");
  } else if(m_detID->is_muon(id)) {
    if (!m_muonTool.empty()) {
      return &*m_muonTool;
    }
    ATH_MSG_WARNING("getTool: Identifier is from Muon but have no Muon tool");
  } else {
    ATH_MSG_WARNING("getTool: Identifier is of unknown type! id: "<<id.getString());
  }
  return nullptr;
}

const Trk::IExtendedTrackSummaryHelperTool*
Trk::TrackSummaryTool::getTool(const Identifier& id) const{
  if (m_detID->is_indet(id)){
    if (!m_idTool.empty()){
      return &*m_idTool;
    }
    ATH_MSG_WARNING("getTool: Identifier is from ID but have no ID tool");
  } else if(m_detID->is_muon(id)) {
    if (!m_muonTool.empty()) {
      return &*m_muonTool;
    }
    ATH_MSG_WARNING("getTool: Identifier is from Muon but have no Muon tool");
  } else {
    ATH_MSG_WARNING("getTool: Identifier is of unknown type! id: "<<id.getString());
  }
  return nullptr;
}

void
Trk::TrackSummaryTool::searchHolesStepWise(const Trk::Track& track,
                                           std::vector<int>& information,
                                           bool doHolesInDet,
                                           bool doHolesMuon) const
{
  ATH_MSG_VERBOSE("Entering Trk::TrackSummaryTool::searchHolesStepWise");
  // -------- obtain hits in Pixel and SCT only
  if (track.trackStateOnSurfaces()==nullptr){
    ATH_MSG_DEBUG ("No trackStatesOnSurface!!!!");
    constexpr int toMinusOne{-1};
    constexpr std::array<size_t, 16> atIndices{
      numberOfPixelHoles, numberOfPixelDeadSensors,
      numberOfSCTHoles, numberOfSCTDoubleHoles, numberOfSCTDeadSensors,
      numberOfTRTHoles, numberOfTRTDeadStraws,
      numberOfCscEtaHoles, numberOfCscPhiHoles,
      numberOfRpcEtaHoles, numberOfRpcPhiHoles,
      numberOfTgcEtaHoles, numberOfTgcPhiHoles,
      //new small wheels
      numberOfStgcEtaHoles, numberOfStgcPhiHoles,
      numberOfMmHoles
    };
    setTheseElements(information, atIndices, toMinusOne);
    return;
  }

  constexpr int toZero{0};
  if (doHolesInDet){
    // -------- perform the InDet hole search
    if (m_pixelExists) {
      setTheseElements(information, {numberOfPixelHoles, numberOfPixelDeadSensors}, toZero);
    }
    setTheseElements(information, {numberOfSCTHoles, numberOfSCTDoubleHoles, numberOfSCTDeadSensors}, toZero );
    // ME : revert to take the summary helper, this is a temporary thing for 16.0.X
    m_idTool->searchForHoles(track,information,Trk::pion);
  }
  if (!m_muonTool.empty() && doHolesMuon){
    // now do Muon hole search. It works completely differently to the above,
    // so we need to make this all a bit more general
    // and probably more efficient. But this hopefully works for now! EJWM
    constexpr size_t numberOfRelatedMuonCounters{10};
    constexpr std::array<size_t, numberOfRelatedMuonCounters> atMuonIndices{
      numberOfMdtHoles,
      numberOfCscEtaHoles, numberOfCscPhiHoles,
      numberOfRpcEtaHoles, numberOfRpcPhiHoles,
      numberOfTgcEtaHoles, numberOfTgcPhiHoles,
      //new small wheels
      numberOfStgcEtaHoles, numberOfStgcPhiHoles,
      numberOfMmHoles
    };
    setTheseElements(information, atMuonIndices, toZero);
    m_muonTool->searchForHoles(track,information,Trk::muon) ;
  }
}

