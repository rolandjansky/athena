/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ReFitTrack.cxx
//   Implementation file for class ReFitTrack
///////////////////////////////////////////////////////////////////
// version 1.0 13/05/04 Maria Jose Costa
///////////////////////////////////////////////////////////////////

//SiTBLineFitter includes
#include "TrkRefitAlg/ReFitTrack.h"

// Gaudi includes
#include "GaudiKernel/ListItem.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkVertexOnTrack/VertexOnTrack.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"

#include <memory>

#include <vector>

// Constructor with parameters:
Trk::ReFitTrack::ReFitTrack(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_trackName("Tracks"),
  m_newTrackName("ReFitted_Tracks"),
  m_runOutlier(false),
  m_matEffects(3),
  m_ParticleHypothesis(Trk::pion),
  m_tracks(nullptr),
  m_ITrackFitter("TrkKalmanFitter/KalmanFitter"),
  m_ITrackFitterTRT(""),
  m_trkSummaryTool(""),
  m_trkSelectorTool(""),
  m_constrainFitMode(0),
  m_vxContainerName("VxPrimaryCandidate"),
  m_extrapolator("Trk::Extrapolator/InDetExtrapolator"),
  m_usetrackhypo(false)
{

  // Get parameter values from jobOptions file
  declareProperty("TrackName",            m_trackName,    "collection name for tracks to be refitted");
  declareProperty("NewTrackName",         m_newTrackName, "collection name for output tracks");
  declareProperty("FitterTool",           m_ITrackFitter, "ToolHandle for track fitter implementation");
  declareProperty("FitterToolTRT",        m_ITrackFitterTRT, "ToolHandle for TRT track fitter implementation");
  declareProperty("SummaryTool" ,         m_trkSummaryTool, "ToolHandle for track summary tool");
  declareProperty("TrackSelectionTool",   m_trkSelectorTool, "ToolHandle for track selection tool");
  declareProperty("runOutlier",           m_runOutlier,   "switch to control outlier finding in track fit");
  declareProperty("matEffects",           m_matEffects,   "type of material interaction in extrapolation");
  declareProperty("useParticleHypothesisFromTrack", m_usetrackhypo,"fit with particle hypothesis from original track");

  // constrained fitting
  declareProperty("ConstrainFit",         m_constrainFitMode,"mode switch if/how the track is constrained to the BS/Vx");
  declareProperty("VertexCollection",     m_vxContainerName,"Source for vertex to use for constraining tracks");
  declareProperty("Extrapolator",         m_extrapolator, "Extrapolator needed for coherent measurement frame.");


}

// Initialize method:
StatusCode Trk::ReFitTrack::initialize()
{
  ATH_MSG_INFO("ReFitTrack::initialize()");

  if (m_ITrackFitter.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool "<<m_ITrackFitter.typeAndName()<<endmsg;
    return StatusCode::FAILURE;
  } else
    msg(MSG::INFO) << "Retrieved general fitter " << m_ITrackFitter.typeAndName() << endmsg;

  if (!m_ITrackFitterTRT.name().empty()){
    if (m_ITrackFitterTRT.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_ITrackFitterTRT.typeAndName()<<endmsg;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Retrieved fitter for TRT-only tracks " << m_ITrackFitterTRT.typeAndName() << endmsg;
  }

  if (!m_trkSummaryTool.name().empty()){
    if (m_trkSummaryTool.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkSummaryTool << endmsg;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Retrieved tool " << m_trkSummaryTool.typeAndName() << endmsg;
  }
  if ( m_assoTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_assoTool << endmsg;
    return StatusCode::FAILURE;
  } else ATH_MSG_INFO("Retrieved tool " << m_assoTool);

  if (!m_trkSelectorTool.empty()){
    if (m_trkSelectorTool.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkSelectorTool << ". No Track Selection will be done." << endmsg;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Retrieved tool " << m_trkSelectorTool.typeAndName() << endmsg;
  }

  // beam conditions service
  if (m_constrainFitMode) {
    ATH_CHECK(m_beamSpotKey.initialize());
  }

  // extrapolator
  if (m_constrainFitMode && m_extrapolator.retrieve().isFailure()){
      ATH_MSG_FATAL( "Failed to retrieve " << m_extrapolator );
      return StatusCode::FAILURE;
  }

  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_ParticleHypothesis = particleSwitch.particle[m_matEffects];

  ATH_CHECK( m_trackName.initialize() );
  ATH_CHECK( m_vxContainerName.initialize() );
  ATH_CHECK( m_newTrackName.initialize() );
  ATH_CHECK( m_pixelDetEleCollKey.initialize() );
  ATH_CHECK( m_SCTDetEleCollKey.initialize() );

  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode Trk::ReFitTrack::execute()
{
  ATH_MSG_DEBUG ("ReFitTrack::execute()");
  std::unique_ptr<Trk::PRDtoTrackMap> prd_to_track_map(m_assoTool->createPRDtoTrackMap());

  SG::ReadHandle<TrackCollection> tracks (m_trackName);

  if (!tracks.isValid()){
    msg(MSG::ERROR) <<"Track collection named " << m_trackName.key()
		    << " not found, exit ReFitTrack." << endmsg;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG ("Tracks collection '" << m_trackName.key() << "' retrieved from EventStore.");
  }

  ATH_MSG_DEBUG("Track fit configured with constrain option : " << m_constrainFitMode );

  // Handle the various possible constraints.
  std::unique_ptr<const Trk::PerigeeSurface> constrainSf;
  std::unique_ptr<const Trk::RecVertex>      constrainVx;

  if (m_constrainFitMode > 0){
    constrainVx.reset( setupConstrainedFit() );
  }

  // constrain surface (either BS of VTX)
  if (constrainVx) {
    constrainSf = std::make_unique<Trk::PerigeeSurface>( constrainVx->position());
  }

  ParticleHypothesis hypo=m_ParticleHypothesis;

  // create new collection of tracks to write in storegate
  //  auto newtracks = std::make_unique<TrackCollection>();
  std::vector<std::unique_ptr<Trk::Track> > new_tracks;

  // loop over tracks
  for (TrackCollection::const_iterator itr  = (*tracks).begin(); itr < (*tracks).end(); itr++){

    ATH_MSG_VERBOSE ("input track:" << **itr);

    if (m_usetrackhypo) hypo=(**itr).info().particleHypothesis();
    std::unique_ptr<Trk::Track> newtrack;
    bool trtonly=false;

    bool passedSelection = true;
    // check whether the track passes the selection
    if (*itr && !m_trkSelectorTool.empty()){
      passedSelection = m_trkSelectorTool->decision(**itr);
    }


    if (!m_ITrackFitterTRT.name().empty() && !m_trkSummaryTool.name().empty()){
      ATH_MSG_VERBOSE ("Creating summary");
      // @TODO does not need PRDtoTrackMap
      unsigned int n_trt_hits;
      if ((*itr)->trackSummary()) {
         n_trt_hits = (*itr)->trackSummary()->get(numberOfTRTHits);
      }
      else {
         std::unique_ptr<const Trk::TrackSummary> summary(  m_trkSummaryTool->summaryNoHoleSearch(**itr, prd_to_track_map.get()));
         n_trt_hits = summary->get(numberOfTRTHits);
      }
      if ( (**itr).measurementsOnTrack()->size() - n_trt_hits<3 )
        trtonly=true;
    }

    if (passedSelection) {
      ATH_MSG_VERBOSE ("Passed selection");

       const Trk::Perigee* origPerigee = (*itr)->perigeeParameters();
       if (origPerigee){
           double od0 = origPerigee->parameters()[Trk::d0];
           double oz0 = origPerigee->parameters()[Trk::z0];
           double ophi0 = origPerigee->parameters()[Trk::phi0];
           double otheta = origPerigee->parameters()[Trk::theta];
           double oqOverP = origPerigee->parameters()[Trk::qOverP];
           ATH_MSG_DEBUG ("Original parameters " << od0  << " " << oz0  << " " << ophi0 << " " << otheta << " " << oqOverP);
       }

	  // create a measurement for the beamspot
	  if (constrainVx && origPerigee){

         ATH_MSG_VERBOSE ("Creating measurement for beamspot.");
         // extrapolate the track to the vertex -- for consistent Measurement frame
         std::unique_ptr<const Trk::TrackParameters> tp( m_extrapolator->extrapolate(**itr,*constrainSf,Trk::anyDirection) );
         const Trk::Perigee* tpConstrainedSf = dynamic_cast<const Trk::Perigee*>(tp.get());
         // create the vertex/beamsptOnTrack
         std::unique_ptr<Trk::VertexOnTrack> bsvxOnTrack( tpConstrainedSf ? new Trk::VertexOnTrack(*constrainVx,*tpConstrainedSf) : nullptr );
         std::vector<const MeasurementBase*> vec;
         if (tpConstrainedSf) vec.push_back(bsvxOnTrack.get() );
         // get the measurmentsOnTrack
         const DataVector<const MeasurementBase>* measurementsOnTracks = (**itr).measurementsOnTrack();
         // get the outliersOnTrack, needs sorting
         //!< @todo : include sorting of outliers
         // clone measurements and outliers into the track
         DataVector<const MeasurementBase>::const_iterator measIter = measurementsOnTracks->begin();
         DataVector<const MeasurementBase>::const_iterator measIterEnd = measurementsOnTracks->end();
         for ( ; measIter != measIterEnd; ++measIter)
             vec.push_back((*measIter));
         // if - protect the outliers ...
         // const DataVector<const MeasurementBase>* outliersOnTracks = (**itr).outliersOnTrack();
         // measIter = outliersOnTracks->begin();
         // measIterEnd = outliersOnTracks->end();
         // for ( ; measIter != measIterEnd; ++measIter)
         //    vec.push_back((*measIter));
         // refit with the beamspot / vertex
         newtrack.reset((trtonly ? m_ITrackFitterTRT : m_ITrackFitter)->fit(vec, *origPerigee, m_runOutlier,hypo));
         // now delete the vsvx
      } else {
         newtrack.reset((trtonly ? m_ITrackFitterTRT : m_ITrackFitter)->fit(**itr,m_runOutlier,hypo));
      }
    } // passed selection

    if (msgLvl(MSG::DEBUG)) {
      if (!newtrack) ATH_MSG_DEBUG ("Refit Failed");
      else {

        // ATH_MSG_VERBOSE ("re-fitted track:" << *newtrack);
        const Trk::Perigee *aMeasPer=
          dynamic_cast<const Trk::Perigee*>(newtrack->perigeeParameters () );
        if (aMeasPer==nullptr){
          msg(MSG::ERROR) << "Could not get Trk::MeasuredPerigee" << endmsg;
          continue;
        }
        double d0 = aMeasPer->parameters()[Trk::d0];
        double z0 = aMeasPer->parameters()[Trk::z0];
        double phi0 = aMeasPer->parameters()[Trk::phi0];
        double theta = aMeasPer->parameters()[Trk::theta];
        double qOverP = aMeasPer->parameters()[Trk::qOverP];
        ATH_MSG_DEBUG ("Refitted parameters " << d0  << " " << z0  << " " << phi0 << " " << theta << " " << qOverP);
      }
    }

    if (newtrack) {
      new_tracks.push_back(std::move(newtrack));
    }

  }

  ATH_MSG_VERBOSE ("Add PRDs to assoc tool.");

  // recreate the summaries on the final track collection with correct PRD tool
  for(const std::unique_ptr<Trk::Track> &new_track : new_tracks ) {
    if((m_assoTool->addPRDs(*prd_to_track_map, *new_track)).isFailure()) {ATH_MSG_WARNING("Failed to add PRDs to map");}
  }

  ATH_MSG_VERBOSE ("Recalculate the summary");
  // and copy tracks from vector of non-const tracks to collection of const tracks
  std::unique_ptr<TrackCollection> new_track_collection = std::make_unique<TrackCollection>();
  new_track_collection->reserve(new_tracks.size());
  for(std::unique_ptr<Trk::Track> &new_track : new_tracks ) {
    m_trkSummaryTool->computeAndReplaceTrackSummary(*new_track, prd_to_track_map.get(), false /* DO NOT suppress hole search*/);
    new_track_collection->push_back(std::move(new_track));
  }

  ATH_MSG_VERBOSE ("Save tracks");
  ATH_CHECK(SG::WriteHandle<TrackCollection>(m_newTrackName).record(std::move(new_track_collection)));

  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode Trk::ReFitTrack::finalize()
{
  msg(MSG::INFO) << "ReFitTrack::finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

const Trk::RecVertex* Trk::ReFitTrack::setupConstrainedFit()
{
  // constrainVx
  const Trk::RecVertex*      constrainVx = nullptr;
  if ( m_constrainFitMode == 2 ){
    // Beamspot mode
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
    // get vertex position and uncertainties from BeamCondSvc
    constrainVx = new Trk::RecVertex(beamSpotHandle->beamVtx());
    ATH_MSG_DEBUG("Track fit with BeamSpot constraint (x/y/z)  = "
      << constrainVx->position().x() << ", "
      << constrainVx->position().y() << ", "
      << constrainVx->position().z());
  } else if (m_constrainFitMode ==1){
    // Vertex mode
    SG::ReadHandle<VxContainer> vxContainer (m_vxContainerName);
    ATH_MSG_DEBUG("Track fit with vertex from collection '" << m_vxContainerName.key() << "'.");
    if (!vxContainer.isValid())
      ATH_MSG_WARNING("Track fit configured to use vertex constraint, but '" << m_vxContainerName.key() << "' could not be retrieved.");
    else if (vxContainer->size() == 1 ) {
      ATH_MSG_WARNING("Track fit configured to use vertex constraint, but '" << m_vxContainerName.key() << "' contains only dummy vertex.");
    } else {
      // only refit to the 'signal' vertex
      constrainVx = new Trk::RecVertex( (*vxContainer)[0]->recVertex() ); // Just copy it to simplify ownership.
      ATH_MSG_DEBUG("Track fit with Vertex constraint (x/y/z)  = "
        << constrainVx->position().x() << ", "
        << constrainVx->position().y() << ", "
        << constrainVx->position().z());
    }
  } else {
    ATH_MSG_WARNING("Unknown constraint mode: "<< m_constrainFitMode);
  }
  return constrainVx;
}
