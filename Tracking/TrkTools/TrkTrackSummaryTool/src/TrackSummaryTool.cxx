/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "Identifier/Identifier.h"

#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkParameters/TrackParameters.h"

#include <cassert>
#include <vector>
#include <algorithm>


Trk::TrackSummaryTool::TrackSummaryTool(const std::string& t,
  const std::string& n,
  const IInterface*  p )
  :
  base_class(t,n,p),
  m_detID{}
{
  declareInterface<ITrackSummaryTool>(this);
}


Trk::TrackSummaryTool::~TrackSummaryTool()
{
}


StatusCode
  Trk::TrackSummaryTool::initialize()
{
    // StatusCode sc=StatusCode::SUCCESS;
    //StatusCode sc = AlgTool::initialize();
    //if (sc.isFailure()) return sc;

    ATH_CHECK( detStore()->retrieve(m_detID, "AtlasID" ));

    if (m_idTool.empty() && m_muonTool.empty()) {
        ATH_MSG_ERROR ("Could get neither InDetHelperTool nor MuonHelperTool. Must abort.");
        return StatusCode::FAILURE;
    }

    if ( !m_idTool.empty() && m_idTool.retrieve().isFailure() ) {
        ATH_MSG_ERROR ("Failed to retrieve InDet helper tool "<< m_idTool);
        ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
        return StatusCode::FAILURE;
    }
        if ( !m_idTool.empty()) msg(MSG::INFO) << "Retrieved tool " << m_idTool << endmsg;


  // Troels.Petersen@cern.ch:
    if ( !m_eProbabilityTool.empty() && m_eProbabilityTool.retrieve().isFailure() )
    {
        ATH_MSG_ERROR ("Failed to retrieve electron probability tool " << m_eProbabilityTool);
        ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
        return StatusCode::FAILURE;
    }
       if ( !m_eProbabilityTool.empty()) msg(MSG::INFO) << "Retrieved tool " << m_eProbabilityTool << endmsg;


    if (!m_trt_dEdxTool.empty()) {
      ATH_CHECK( m_trt_dEdxTool.retrieve() );
    }


    if ( !m_dedxtool.empty() && m_dedxtool.retrieve().isFailure() )
    {
        ATH_MSG_ERROR ("Failed to retrieve pixel dEdx tool " << m_dedxtool);
        ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
        return StatusCode::FAILURE;
    }
       if ( !m_dedxtool.empty()) msg(MSG::INFO) << "Retrieved tool " << m_dedxtool << endmsg;


    if ( !m_muonTool.empty() && m_muonTool.retrieve().isFailure() )
    {
        ATH_MSG_ERROR ("Failed to retrieve Muon helper tool "<< m_muonTool);
        ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
    }
    else {
        if ( !m_muonTool.empty()) msg(MSG::INFO) << "Retrieved tool " << m_muonTool<<endmsg;
    }

    if (m_doHolesInDet)
        ATH_MSG_INFO ("Search for InDet holes using external tool turned ON");
    else
        ATH_MSG_INFO ("Search for InDet holes using external tool turned OFF");

    if (m_doHolesMuon)
        ATH_MSG_INFO ("Search for Muon holes using external tool turned ON");
    else
        ATH_MSG_INFO ("Search for Muon holes using external tool turned OFF");

    ATH_MSG_INFO ("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}


StatusCode
  Trk::TrackSummaryTool::finalize()
{
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
  Trk::Track& track,
  const Trk::PRDtoTrackMap* prd_to_track_map,
  bool suppress_hole_search) const
{
  track.setTrackSummary(createSummary(track,
                                      prd_to_track_map,
                                      m_doHolesInDet & !suppress_hole_search,
                                      m_doHolesMuon & !suppress_hole_search));
}

std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summary(const Track& track) const
{
  return createSummary(track, nullptr, m_doHolesInDet, m_doHolesMuon);
}

std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summary(const Track& track,
                               const Trk::PRDtoTrackMap* prd_to_track_map) const
{
  return createSummary(track, prd_to_track_map, m_doHolesInDet, m_doHolesMuon);
}

std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summaryNoHoleSearch(const Track& track) const
{
  return createSummary(track, nullptr, false, false);
}

std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summaryNoHoleSearch(
  const Track& track,
  const Trk::PRDtoTrackMap* prd_to_track_map) const
{
  return createSummary(track, prd_to_track_map, false, false);
}

//Method to create a new summary
std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::createSummary( const Track& track,
                                      const Trk::PRDtoTrackMap *prd_to_track_map,
                                      bool doHolesInDet,
                                      bool doHolesMuon) const
{
  std::unique_ptr<Trk::TrackSummary> ts;

  // first check if track has summary already and then clone it.
  if (track.trackSummary()!=nullptr) {
    ATH_MSG_DEBUG ("Return cached summary for author : "<<track.info().dumpInfo());
    ts = std::make_unique<Trk::TrackSummary>(*(track.trackSummary()));
  } else {
    ts = std::make_unique<Trk::TrackSummary>();
  }
  //fill the summary
  fillSummary(*ts, track, prd_to_track_map, doHolesInDet, doHolesMuon);
  return ts;
}

// Method filling the summary with information
void
Trk::TrackSummaryTool::fillSummary(Trk::TrackSummary& ts,
                                   const Trk::Track& track,
                                   const Trk::PRDtoTrackMap *prd_to_track_map,
                                   bool doHolesInDet,
                                   bool doHolesMuon) const {

std::vector<int>& information = ts.m_information;
information.resize(std::min(information.size(),
                            static_cast<size_t>(numberOfTrackSummaryTypes)));

// Troels.Petersen@cern.ch:
unsigned int numberOfeProbabilityTypes = Trk::numberOfeProbabilityTypes + 1;
std::vector<float> eProbability(numberOfeProbabilityTypes, 0.5);

  float dedx = -1;
  int nhitsuseddedx = -1;
  int noverflowhitsdedx = -1;

  // Now set values to 0 for the ones we evaluate
  if (!m_idTool.empty()) {
    if (m_pixelExists) {
      information[numberOfContribPixelLayers] = 0;
      information[numberOfInnermostPixelLayerHits] = 0;
      information[numberOfInnermostPixelLayerOutliers] = 0;
      information[numberOfNextToInnermostPixelLayerHits] = 0;
      information[numberOfNextToInnermostPixelLayerOutliers] = 0;
      information[numberOfPixelHits] = 0;
      information[numberOfPixelOutliers] = 0;
      information[numberOfGangedPixels] = 0;
      information[numberOfGangedFlaggedFakes] = 0;
      information[numberOfPixelSpoiltHits] = 0;
      information[numberOfGangedFlaggedFakes] = 0;
      information[numberOfPixelSplitHits] = 0;
      information[numberOfInnermostLayerSplitHits] = 0;
      information[numberOfNextToInnermostLayerSplitHits] = 0;
      if (track.info().trackFitter() != TrackInfo::Unknown &&
          !m_dedxtool.empty()) {
        dedx = m_dedxtool->dEdx(track, nhitsuseddedx, noverflowhitsdedx);
      }
      information[Trk::numberOfDBMHits] = 0;
    }
    information[numberOfSCTHits] = 0;
    information[numberOfSCTSpoiltHits] = 0;
    information[numberOfSCTOutliers] = 0;
    information[numberOfTRTHits] = 0;
    information[numberOfTRTXenonHits] = 0;
    information[numberOfTRTHighThresholdHits] = 0;
    information[numberOfTRTHighThresholdHitsTotal] = 0;
    information[numberOfTRTOutliers] = 0;
    information[numberOfTRTHighThresholdOutliers] = 0;
    information[numberOfTRTTubeHits] = 0;
    information[numberOfTRTSharedHits] = 0;

    // Troels.Petersen@cern.ch:
    if (!m_eProbabilityTool.empty()) {
      eProbability = m_eProbabilityTool->electronProbability(track);
    }
  }

  if (m_doSharedHits) {
    information [numberOfSCTSharedHits]      = 0;
    if (m_pixelExists) {
      information [numberOfInnermostPixelLayerSharedHits] = 0;
      information [numberOfNextToInnermostPixelLayerSharedHits] = 0;
      information [numberOfPixelSharedHits]  = 0;
    }
  }
  if (!m_muonTool.empty()) {
    information [numberOfMdtHits]    = 0;
    information [numberOfTgcPhiHits] = 0;
    information [numberOfTgcEtaHits] = 0;
    information [numberOfCscPhiHits] = 0;
    information [numberOfCscEtaHits] = 0;
    information [numberOfCscUnspoiltEtaHits] = 0;
    information [numberOfRpcPhiHits] = 0;
    information [numberOfRpcEtaHits] = 0;
    information[Trk::numberOfMdtHoles] =0;// no matter what, we either use an external tool or count holes on track, so set zero
    information[Trk::numberOfCscEtaHoles] =0;
    information[Trk::numberOfCscPhiHoles] =0;
    information[Trk::numberOfRpcEtaHoles] =0;
    information[Trk::numberOfRpcPhiHoles] =0;
    information[Trk::numberOfTgcEtaHoles] =0;
    information[Trk::numberOfTgcPhiHoles] =0;
    // New Small Wheel
    information[Trk::numberOfStgcEtaHits] =0;
    information[Trk::numberOfStgcPhiHits] =0;
    information[Trk::numberOfMmHits] =0;
    information[Trk::numberOfStgcEtaHoles] =0;
    information[Trk::numberOfStgcPhiHoles] =0;
    information[Trk::numberOfMmHoles] =0;
  }

  std::bitset<numberOfDetectorTypes> hitPattern;

  ATH_MSG_DEBUG ("Produce summary for: "<<track.info().dumpInfo());

  if (track.trackStateOnSurfaces()!=nullptr)
  {
    information[Trk::numberOfOutliersOnTrack] = 0;
    processTrackStates(track,prd_to_track_map, track.trackStateOnSurfaces(), information, hitPattern,
                       doHolesInDet, doHolesMuon);
  }else{
    ATH_MSG_WARNING ("Null pointer to TSoS found on Track (author = "
      <<track.info().dumpInfo()<<"). This should never happen! ");
  }

  if (doHolesInDet || doHolesMuon)
  {
    if (m_pixelExists)
    {
      information [numberOfPixelHoles] = 0;
    }
    information [numberOfSCTHoles]       = 0;
    information [numberOfSCTDoubleHoles] = 0;
    searchHolesStepWise(track,information, doHolesInDet, doHolesMuon);
  }

  if (!m_trt_dEdxTool.empty()) {
    if (information[Trk::numberOfTRTHits]+information[Trk::numberOfTRTOutliers]>=m_minTRThitsForTRTdEdx) {
      int nhits = static_cast<int>( m_trt_dEdxTool->usedHits(&track) );
      double fvalue = (nhits>0 ? m_trt_dEdxTool->dEdx(&track) : 0.0);
      eProbability.push_back(fvalue);
      information[ numberOfTRTHitsUsedFordEdx] = static_cast<uint8_t>(std::max(nhits,0));
    }
    else {
      information[ numberOfTRTHitsUsedFordEdx]=0;
      eProbability.push_back(0.0);
    }
  }
  else {
    eProbability.push_back(0.0);
  }

  ts.m_eProbability = eProbability;
  ts.m_idHitPattern = hitPattern.to_ulong();
  ts.m_dedx = dedx;
  ts.m_nhitsdedx = nhitsuseddedx;
  ts.m_nhitsoverflowdedx = noverflowhitsdedx;

  // add detailed summary for indet
  if( m_addInDetDetailedSummary && !m_idTool.empty() ){
    m_idTool->addDetailedTrackSummary(track,ts);
  }
  // add detailed summary for muons
  if( m_addMuonDetailedSummary && !m_muonTool.empty() ){
    m_muonTool->addDetailedTrackSummary(track,ts);
  }
  // move this part to VERBOSE
  ATH_MSG_VERBOSE ( ts << endmsg << "Finished!");
}

void Trk::TrackSummaryTool::updateSharedHitCount(const Track& track, const Trk::PRDtoTrackMap *prd_to_track_map,TrackSummary &summary) const
{
  // first check if track has no summary - then it is recreated
  m_idTool->updateSharedHitCount(track, prd_to_track_map, summary);
}

void Trk::TrackSummaryTool::updateAdditionalInfo(const Track& track, const Trk::PRDtoTrackMap *prd_to_track_map, TrackSummary &summary, bool initialise_to_zero) const
{
  unsigned int numberOfeProbabilityTypes = Trk::numberOfeProbabilityTypes+1;
  std::vector<float> eProbability(numberOfeProbabilityTypes,0.5);
  if ( !m_eProbabilityTool.empty() ) eProbability = m_eProbabilityTool->electronProbability(track);

  if (!m_trt_dEdxTool.empty()) {
    if (summary.get(Trk::numberOfTRTHits)+summary.get(Trk::numberOfTRTOutliers)>=m_minTRThitsForTRTdEdx) {
      int nhits = static_cast<int>( m_trt_dEdxTool->usedHits(&track) );
      double fvalue = (nhits>0 ? m_trt_dEdxTool->dEdx(&track) : 0.0);
      eProbability.push_back(fvalue);
      if (!summary.update(Trk::numberOfTRTHitsUsedFordEdx, static_cast<uint8_t>(std::max(nhits,0)) )) {
        ATH_MSG_WARNING( "Attempt to update numberOfTRTHitsUsedFordEdx but this summary information is "
                         "already set. numberOfTRTHitsUsedFordEdx is:" << summary.get(numberOfTRTHitsUsedFordEdx)
                         << " =?= should:" << nhits );
      }
    }
    else {
      eProbability.push_back(0.0);
      if (!summary.update(Trk::numberOfTRTHitsUsedFordEdx, 0) ) {
        ATH_MSG_WARNING( "Attempt to update numberOfTRTHitsUsedFordEdx but this summary information is "
                         "already set. numberOfTRTHitsUsedFordEdx is:" << summary.get(numberOfTRTHitsUsedFordEdx)
                         << " =?= should:" << 0 );
      }
    }
  }

  float dedx=            (initialise_to_zero ? 0 : -1);
  int nhitsuseddedx=     (initialise_to_zero ? 0 : -1);
  int noverflowhitsdedx= (initialise_to_zero ? 0 : -1);

  if (track.info().trackFitter() != TrackInfo::Unknown && !m_dedxtool.empty()) {
    dedx = m_dedxtool->dEdx(track, nhitsuseddedx, noverflowhitsdedx);
  }

  m_idTool->updateAdditionalInfo(summary, eProbability,dedx, nhitsuseddedx,noverflowhitsdedx);

  m_idTool->updateSharedHitCount(track, prd_to_track_map, summary);

  m_idTool->updateExpectedHitInfo(track, summary);

  if (m_addInDetDetailedSummary) m_idTool->addDetailedTrackSummary(track,summary);
}


/*
 * Then the internal helpers
 */

void Trk::TrackSummaryTool::processTrackStates(const Track& track,
                                               const Trk::PRDtoTrackMap *prd_to_track_map,
					       const DataVector<const TrackStateOnSurface>* tsos,
					       std::vector<int>& information,
					       std::bitset<numberOfDetectorTypes>& hitPattern,
                                               bool doHolesInDet,
                                               bool doHolesMuon) const
{
  ATH_MSG_DEBUG ("Starting to process " << tsos->size() << " track states");

  int measCounter = 0;
  int cntAddChi2 = 0;
  float chi2Sum = 0;
  float chi2Sum2 = 0;
  DataVector<const TrackStateOnSurface>::const_iterator it = tsos->begin();
  DataVector<const TrackStateOnSurface>::const_iterator itEnd = tsos->end();
  for ( ; it!=itEnd; ++it){
    if ((*it)->type(Trk::TrackStateOnSurface::Measurement) || (*it)->type(Trk::TrackStateOnSurface::Outlier)){
      ++measCounter;
      const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();
      if (!measurement) {
        ATH_MSG_WARNING ("measurementOnTrack == Null for a TrackStateOnSurface "
          << "of type Measurement or Outlier");
      } else {
        if ((*it)->type(Trk::TrackStateOnSurface::Outlier)) ++information[Trk::numberOfOutliersOnTrack]; // increment outlier counter
        ATH_MSG_VERBOSE ("analysing TSoS " << measCounter << " of type " << (*it)->dumpType() );
        processMeasurement(track, prd_to_track_map, measurement, *it, information, hitPattern);
      } // if have measurement pointer
    } // if type measurement, scatterer or outlier

    if ((*it)->type(Trk::TrackStateOnSurface::Measurement) &&
      (*it)->fitQualityOnSurface() &&
      (*it)->fitQualityOnSurface()->numberDoF()>0 )
    {
      ++cntAddChi2;
      if ((*it)->fitQualityOnSurface()->chiSquared() > 1.e5) {// limit unphysical values and protect against FPE
        chi2Sum += 1.e5;
        chi2Sum2 += 1.e10;
        ATH_MSG_DEBUG ("TSOS has unphysical chi2: "<< (*it)->fitQualityOnSurface()->chiSquared());
      } else {
        float chi2add =(*it)->fitQualityOnSurface()->chiSquared()/
          (*it)->fitQualityOnSurface()->numberDoF();
        chi2Sum+=chi2add;
        chi2Sum2+=chi2add*chi2add;
      }
    }

    if ( (*it)->type(Trk::TrackStateOnSurface::Hole) && (*it)->trackParameters() ){
      if (!doHolesInDet || !doHolesMuon ){ // no dedicated hole search via extrapolation, but take what might be on the track already.
        if ( (*it)->trackParameters()->associatedSurface().associatedDetectorElement()!=nullptr ) {
          const Identifier& id = (*it)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
          if ( !doHolesInDet && m_detID->is_pixel( id ) ) ++information[Trk::numberOfPixelHoles];
          if ( !doHolesInDet && m_detID->is_sct( id ) )    ++information[Trk::numberOfSCTHoles];
          if ( !doHolesMuon && m_detID->is_mdt( id ) )    ++information[Trk::numberOfMdtHoles];
        }
      }
    }
  } // end loop

  float varChi2 = 0;
  if (cntAddChi2>0) varChi2=chi2Sum2/cntAddChi2 - (chi2Sum/cntAddChi2) *(chi2Sum/cntAddChi2) ;
  if (varChi2>0 && varChi2<1.e13) information[Trk::standardDeviationOfChi2OS] = int(sqrt(varChi2)*100);
}

void Trk::TrackSummaryTool::processMeasurement(const Track& track,
                                               const Trk::PRDtoTrackMap *prd_to_track_map,
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
      tool->analyse(track,prd_to_track_map, rot,tsos,information, hitPattern);
    }
  } else {
    //check if the measurement type is CompetingRIOsOnTrack
    //
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
        tool->analyse(track,prd_to_track_map, compROT,tsos,information, hitPattern);
      }
    }
  }
}

Trk::IExtendedTrackSummaryHelperTool*
Trk::TrackSummaryTool::getTool(const Identifier& id)
{
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
Trk::TrackSummaryTool::getTool(const Identifier& id) const
{
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

void Trk::TrackSummaryTool::searchHolesStepWise( const Trk::Track& track,
                                                 std::vector<int>& information,
                                                 bool doHolesInDet,
                                                 bool doHolesMuon) const
{

  ATH_MSG_VERBOSE ("Entering Trk::TrackSummaryTool::searchHolesStepWise");
// -------- obtain hits in Pixel and SCT only
  if (track.trackStateOnSurfaces()==nullptr)
  {
    ATH_MSG_DEBUG ("No trackStatesOnSurface!!!!");
    information [numberOfPixelHoles]           = -1;
    information [numberOfPixelDeadSensors]     = -1;
    information [numberOfSCTHoles]             = -1;
    information [numberOfSCTDoubleHoles]       = -1;
    information [numberOfSCTDeadSensors]       = -1;
    information [numberOfTRTHoles]             = -1;
    information [numberOfTRTDeadStraws]        = -1;
    // NOTE: Eta holes was used twice instead of Phi holes
    information [numberOfCscEtaHoles]          = -1;
    information [numberOfCscPhiHoles]          = -1;
    information [numberOfRpcEtaHoles]          = -1;
    information [numberOfRpcPhiHoles]          = -1;
    information [numberOfTgcEtaHoles]          = -1;
    information [numberOfTgcPhiHoles]          = -1;
    // New Small Wheel
    information [numberOfStgcEtaHoles]         = -1;
    information [numberOfStgcPhiHoles]         = -1;
    information [numberOfMmHoles]              = -1;
    return;
  }


    if (doHolesInDet)
    {
      // -------- perform the InDet hole search
      if (m_pixelExists) {
        information [numberOfPixelHoles]           = 0;
        information [numberOfPixelDeadSensors]     = 0;
      }
      information [numberOfSCTHoles]             = 0;
      information [numberOfSCTDoubleHoles]       = 0;
      information [numberOfSCTDeadSensors]       = 0;
      // ME : revert to take the summary helper, this is a temporary thing for 16.0.X
      m_idTool->searchForHoles(track,information,Trk::pion);
    }
    if (!m_muonTool.empty() && doHolesMuon)
    {
      // now do Muon hole search. It works completely differently to the above,
      // so we need to make this all a bit more general
      // and probably more efficient. But this hopefully works for now! EJWM
      information [numberOfMdtHoles]             = 0;
      information [numberOfCscEtaHoles]          = 0;
      information [numberOfCscPhiHoles]          = 0;
      information [numberOfRpcEtaHoles]          = 0;
      information [numberOfRpcPhiHoles]          = 0;
      information [numberOfTgcEtaHoles]          = 0;
      information [numberOfTgcPhiHoles]          = 0;
      // New Small Wheel
      information [numberOfStgcEtaHoles]         = 0;
      information [numberOfStgcPhiHoles]         = 0;
      information [numberOfMmHoles] = 0;
      m_muonTool->searchForHoles(track,information,Trk::muon) ;
    }

  }

