/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSlimmingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrackSlimmingTool/TrackSlimmingTool.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

Trk::TrackSlimmingTool::TrackSlimmingTool(const std::string& t,
  const std::string& n,
  const IInterface*  p )
  :
AthAlgTool(t,n,p),
  m_keepCaloDeposit(true),
  m_keepOutliers(false),
  m_keepParameters(false),
  m_setPersistificationHints(false),
  m_detID{}
{
  declareInterface<ITrackSlimmingTool>(this);

    //  template for property decalration
  declareProperty("KeepCaloDeposit",m_keepCaloDeposit,
    "If this is set to true, any CaloDeposit with its adjacent MEOT's will be kept on the slimmed track (combined muon property)");
  declareProperty("KeepOutliers",   m_keepOutliers,   "If this is set to true, Outliers will be kept on the slimmed track");
  declareProperty("KeepParameters", m_keepParameters, "If this is set to true, the first and last parameters will be kept on the slimmed track");
  declareProperty("OnlySetPersistificationHints", m_setPersistificationHints, "Only set persistification hints in each track state on surface");
}
Trk::TrackSlimmingTool::~TrackSlimmingTool()
  {}

StatusCode Trk::TrackSlimmingTool::initialize()
{

  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;

  sc = detStore()->retrieve(m_detID, "AtlasID" );
  if (sc.isFailure()) {
    ATH_MSG_FATAL ("Could not get AtlasDetectorID ");
    return sc;
  }
    ATH_MSG_DEBUG ("Found AtlasDetectorID");


  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}


StatusCode Trk::TrackSlimmingTool::finalize()
{
  return StatusCode::SUCCESS;
}

Trk::Track* Trk::TrackSlimmingTool::slim(const Trk::Track& track) const
{
  /*
   * Without persistifications hints we can use the copy
   * method
   */
  if (!m_setPersistificationHints){
   return slimCopy(track).release();
  }
  /*
   * Setting the persistification hints can be non-thread safe
   * as it involves const casting the input rather than creating new.
   */
  const DataVector<const TrackStateOnSurface>* oldTrackStates = track.trackStateOnSurfaces();
  if (oldTrackStates==nullptr)
  {
    ATH_MSG_WARNING("Track has no TSOS vector! Skipping track, returning 0.");
    return nullptr;
  }

  const TrackStateOnSurface* firstValidIDTSOS(nullptr);
  const TrackStateOnSurface* lastValidIDTSOS(nullptr);
  const TrackStateOnSurface* firstValidMSTSOS(nullptr);
  const TrackStateOnSurface* lastValidMSTSOS(nullptr);
  if (m_keepParameters){
    // search last valid TSOS first (as won't be found in later loop)
    findLastValidTSoS(oldTrackStates, lastValidIDTSOS,lastValidMSTSOS);
  }

  // If m_keepParameters is true, then we want to keep the first and last parameters of ID & MS.
  const Trk::MeasurementBase* rot = nullptr;
  const Trk::TrackParameters* parameters = nullptr;
  bool keepParameter=false;
  // looping over all TSOS
  DataVector<const TrackStateOnSurface>::const_iterator itTSoS = oldTrackStates->begin();
  for (   ; itTSoS!=oldTrackStates->end(); ++itTSoS)
  {
    const_cast<TrackStateOnSurface&>((**itTSoS)).setHint(Trk::TrackStateOnSurface::PartialPersistification);
    parameters=nullptr;
    rot=nullptr;
    // if requested: keep calorimeter TSOS with adjacent scatterers (on combined muons)
    if (m_keepCaloDeposit && (**itTSoS).type(TrackStateOnSurface::CaloDeposit)){
        // preceding TSOS (if Scatterer)
      if (itTSoS != oldTrackStates->begin())
      {
        --itTSoS;
        if ((**itTSoS).type(TrackStateOnSurface::Scatterer)){
          const_cast<TrackStateOnSurface&> ((**itTSoS)).resetHint(Trk::TrackStateOnSurface::PartialPersistification);
        }
        ++itTSoS;
      }
      // copy removes CaloEnergy (just keep base EnergyLoss)
      const MaterialEffectsOnTrack* meot  =
        dynamic_cast<const MaterialEffectsOnTrack*>((**itTSoS).materialEffectsOnTrack());
      if (meot && meot->energyLoss())
      {
        const_cast<TrackStateOnSurface&> ((**itTSoS)).setHint(Trk::TrackStateOnSurface::PersistifySlimCaloDeposit);
        const_cast<TrackStateOnSurface&> ((**itTSoS)).setHint(Trk::TrackStateOnSurface::PersistifyTrackParameters);
      }
      // following TSOS (if Scatterer)
      ++itTSoS;
      if (itTSoS != oldTrackStates->end() && (**itTSoS).type(TrackStateOnSurface::Scatterer))
      {
          const_cast<TrackStateOnSurface&> ((**itTSoS)).resetHint(Trk::TrackStateOnSurface::PartialPersistification);
      }
      --itTSoS;
    }

    // We only keep TSOS if they either contain a perigee, OR are a measurement
    if ((*itTSoS)->measurementOnTrack()==nullptr && !(*itTSoS)->type(TrackStateOnSurface::Perigee)) continue;

    keepParameter=keepParameters((*itTSoS),firstValidIDTSOS, lastValidIDTSOS,firstValidMSTSOS,lastValidMSTSOS);

    if (keepParameter) {
      parameters=(*itTSoS)->trackParameters();
    }
    if ( (*itTSoS)->measurementOnTrack()!=nullptr &&
      ( (*itTSoS)->type(TrackStateOnSurface::Measurement) || ( m_keepOutliers && (*itTSoS)->type(TrackStateOnSurface::Outlier) ) ) )
    {
      rot=(*itTSoS)->measurementOnTrack();
    }
    if (rot!=nullptr || parameters!=nullptr) {
      if (rot) {
        const_cast<TrackStateOnSurface&> ((**itTSoS)).setHint(Trk::TrackStateOnSurface::PersistifyMeasurement);
      }
      if (parameters) {
        const_cast<TrackStateOnSurface&> ((**itTSoS)).setHint(Trk::TrackStateOnSurface::PersistifyTrackParameters);
      }
    }
  }
  return nullptr;
}

void
Trk::TrackSlimmingTool::slimTrack(Trk::Track& track) const
{
  const DataVector<const TrackStateOnSurface>* oldTrackStates =
    track.trackStateOnSurfaces();
  if (oldTrackStates == nullptr) {
    ATH_MSG_WARNING("Track has no TSOS vector! Skipping track, returning 0.");
  }
  // create vector for new TSOS (the ones which are kept)
  DataVector<const TrackStateOnSurface>* trackStates =
    new DataVector<const TrackStateOnSurface>;
  const TrackStateOnSurface* firstValidIDTSOS(nullptr);
  const TrackStateOnSurface* lastValidIDTSOS(nullptr);
  const TrackStateOnSurface* firstValidMSTSOS(nullptr);
  const TrackStateOnSurface* lastValidMSTSOS(nullptr);
  if (m_keepParameters) {
    // search last valid TSOS first (as won't be found in later loop)
    findLastValidTSoS(oldTrackStates, lastValidIDTSOS, lastValidMSTSOS);
  }

  // If m_keepParameters is true, then we want to keep the first and last
  // parameters of ID & MS.
  const Trk::MeasurementBase* rot = nullptr;
  const Trk::TrackParameters* parameters = nullptr;
  bool keepParameter = false;
  std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>
    typePattern(0);

  DataVector<const TrackStateOnSurface>::const_iterator itTSoS =
    oldTrackStates->begin();
  for (; itTSoS != oldTrackStates->end(); ++itTSoS) {
    parameters = nullptr;
    rot = nullptr;
    // if requested: keep calorimeter TSOS with adjacent scatterers (on combined
    // muons)
    if (m_keepCaloDeposit &&
        (**itTSoS).type(TrackStateOnSurface::CaloDeposit)) {
      // preceding TSOS (if Scatterer)
      if (itTSoS != oldTrackStates->begin()) {
        --itTSoS;
        if ((**itTSoS).type(TrackStateOnSurface::Scatterer)) {
          trackStates->push_back((**itTSoS).clone());
        }
        ++itTSoS;
      }
      // copy removes CaloEnergy (just keep base EnergyLoss)
      const MaterialEffectsOnTrack* meot =
        dynamic_cast<const MaterialEffectsOnTrack*>(
          (**itTSoS).materialEffectsOnTrack());
      if (meot && meot->energyLoss()) {
        trackStates->push_back(new TrackStateOnSurface(
          nullptr,
          (**itTSoS).trackParameters()->clone(),
          nullptr,
          new MaterialEffectsOnTrack(meot->thicknessInX0(),
                                     nullptr,
                                     new EnergyLoss(*meot->energyLoss()),
                                     meot->associatedSurface()),
          (**itTSoS).types()));
      }
      // following TSOS (if Scatterer)
      ++itTSoS;
      if (itTSoS != oldTrackStates->end() &&
          (**itTSoS).type(TrackStateOnSurface::Scatterer)) {
        trackStates->push_back((**itTSoS).clone());
      }
      --itTSoS;
    }
    // We only keep TSOS if they either contain a perigee, OR are a measurement
    if ((*itTSoS)->measurementOnTrack() == nullptr &&
        !(*itTSoS)->type(TrackStateOnSurface::Perigee)) {
      continue;
    }
    typePattern.reset();
    keepParameter = keepParameters((*itTSoS),
                                   firstValidIDTSOS,
                                   lastValidIDTSOS,
                                   firstValidMSTSOS,
                                   lastValidMSTSOS);

    if (keepParameter) {
      parameters = (*itTSoS)
                     ->trackParameters()
                     ->clone(); // make sure we add a new parameter by cloning
      if ((*itTSoS)->type(TrackStateOnSurface::Perigee)) {
        typePattern.set(TrackStateOnSurface::Perigee);
      }
    }

    if ((*itTSoS)->measurementOnTrack() != nullptr &&
        ((*itTSoS)->type(TrackStateOnSurface::Measurement) ||
         (m_keepOutliers && (*itTSoS)->type(TrackStateOnSurface::Outlier)))) {
      if ((*itTSoS)->type(TrackStateOnSurface::Measurement)) {
        typePattern.set(TrackStateOnSurface::Measurement);
      }
      if ((*itTSoS)->type(TrackStateOnSurface::Outlier)) {
        typePattern.set(TrackStateOnSurface::Outlier);
      }
      rot = (*itTSoS)->measurementOnTrack()->clone();
    }

    Trk::TrackStateOnSurface* newTSOS = nullptr;
    if (rot != nullptr || parameters != nullptr) {
      newTSOS = new Trk::TrackStateOnSurface(
        rot, parameters, nullptr, nullptr, typePattern);
      trackStates->push_back(newTSOS);
    }
  }
  track.setTrackStateOnSurfaces(trackStates);
  track.info().setTrackProperties(TrackInfo::SlimmedTrack);
  //The above resets also the caches.
}

std::unique_ptr<Trk::Track>
Trk::TrackSlimmingTool::slimCopy(const Trk::Track& track) const
{
  const DataVector<const TrackStateOnSurface>* oldTrackStates =
    track.trackStateOnSurfaces();
  if (oldTrackStates == nullptr) {
    ATH_MSG_WARNING("Track has no TSOS vector! Skipping track, returning 0.");
    return nullptr;
  }
  //Make a copy of the input
  std::unique_ptr<Trk::Track> newTrack = std::make_unique<Trk::Track>(track);
  slimTrack(*newTrack);
  return newTrack;
}

void Trk::TrackSlimmingTool::checkForValidMeas(const Trk::TrackStateOnSurface* tsos, bool& isIDmeas, bool& isMSmeas) const {
  if (tsos->measurementOnTrack()!=nullptr){
    bool isPseudo = (dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tsos->measurementOnTrack())!=nullptr);
    // Handle horrible cROTs
    const Trk::CompetingRIOsOnTrack* cROT = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(tsos->measurementOnTrack());
    Identifier id;
    if (cROT) {
      id = cROT->rioOnTrack( cROT->indexOfMaxAssignProb() ).identify();
    } else {
      id = tsos->measurementOnTrack()->associatedSurface().associatedDetectorElementIdentifier();
    }
    isIDmeas =  !isPseudo && m_detID->is_indet(id );
    isMSmeas = tsos->measurementOnTrack()!=nullptr && !isPseudo && m_detID->is_muon(id );
  }
}

void Trk::TrackSlimmingTool::findLastValidTSoS(const DataVector<const Trk::TrackStateOnSurface>* oldTrackStates,
                                               const Trk::TrackStateOnSurface*& lastValidIDTSOS,
                                               const TrackStateOnSurface*& lastValidMSTSOS) const{

  for ( DataVector<const TrackStateOnSurface>::const_reverse_iterator rItTSoS = oldTrackStates->rbegin();
        rItTSoS != oldTrackStates->rend(); ++rItTSoS){
    if ( (*rItTSoS)->type(TrackStateOnSurface::Measurement) &&
         (*rItTSoS)->trackParameters()!=nullptr && (*rItTSoS)->measurementOnTrack()!=nullptr &&
         !(*rItTSoS)->measurementOnTrack()->type(Trk::MeasurementBaseType::PseudoMeasurementOnTrack)){

      if (m_detID->is_indet((*rItTSoS)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier())) {
        lastValidIDTSOS = (*rItTSoS);
        break;
      } if(m_detID->is_muon((*rItTSoS)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier())){
        lastValidMSTSOS = (*rItTSoS);
        break;
      }
    }
  }
}

bool Trk::TrackSlimmingTool::keepParameters(const Trk::TrackStateOnSurface* TSoS,
                                            const TrackStateOnSurface*& firstValidIDTSOS,
                                            const TrackStateOnSurface*& lastValidIDTSOS,
                                            const TrackStateOnSurface*& firstValidMSTSOS,
                                            const TrackStateOnSurface*& lastValidMSTSOS) const{

  if ( TSoS->trackParameters() != nullptr && TSoS->type(TrackStateOnSurface::Perigee) ) {
    return true;
  }
  // Now do checks for first/last ID/MS measurement (isIDmeas and isMSmeas)
  if (m_keepParameters){
    bool isIDmeas = false;
    bool isMSmeas = false;
    checkForValidMeas(TSoS, isIDmeas, isMSmeas);
    // entering ID?
    if (isIDmeas && !firstValidIDTSOS && TSoS->type(TrackStateOnSurface::Measurement) ) {
      firstValidIDTSOS=TSoS;
      if ( TSoS->trackParameters() != nullptr) return true;
    }
    // entering MS?
    if (isMSmeas && !firstValidMSTSOS ) {
        firstValidMSTSOS=TSoS;
        if ( TSoS->trackParameters() != nullptr) return true;
    }
    // Is this the last TSOS on the track?
    if (lastValidIDTSOS==TSoS || lastValidMSTSOS == TSoS){
      if ( TSoS->trackParameters() != nullptr) return true;
    }
  }
  return false;
}
