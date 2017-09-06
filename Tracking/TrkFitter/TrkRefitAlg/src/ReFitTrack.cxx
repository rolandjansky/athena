/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"

#include <vector>

// Constructor with parameters:
Trk::ReFitTrack::ReFitTrack(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_TrackName("Tracks"),
  m_NewTrackName("ReFitted_Tracks"),
  m_runOutlier(false),
  m_matEffects(3),
  m_ParticleHypothesis(Trk::pion),
  m_fitRIO_OnTrack(false),
  m_tracks(0),
  m_ITrackFitter("TrkKalmanFitter/KalmanFitter"),
  m_ITrackFitterTRT(""),
  m_trkSummaryTool(""),
  m_assoTool(""),
  m_trkSelectorTool(""),
  m_constrainFitMode(0),
  m_vxContainerName("VxPrimaryCandidate"),
  m_iBeamCondSvc("BeamCondSvc", name),
  m_extrapolator("Trk::Extrapolator/InDetExtrapolator"),
  m_usetrackhypo(false)
{  

  // Get parameter values from jobOptions file
  declareProperty("TrackName",            m_TrackName,    "collection name for tracks to be refitted");
  declareProperty("NewTrackName",         m_NewTrackName, "collection name for output tracks");
  declareProperty("FitterTool",           m_ITrackFitter, "ToolHandle for track fitter implementation");
  declareProperty("FitterToolTRT",        m_ITrackFitterTRT, "ToolHandle for TRT track fitter implementation");
  declareProperty("SummaryTool" ,         m_trkSummaryTool, "ToolHandle for track summary tool");
  declareProperty("AssoTool",             m_assoTool, "ToolHandle for PRD association tool");
  declareProperty("TrackSelectionTool",   m_trkSelectorTool, "ToolHandle for track selection tool");
  declareProperty("runOutlier",           m_runOutlier,   "switch to control outlier finding in track fit");
  declareProperty("matEffects",           m_matEffects,   "type of material interaction in extrapolation");
  declareProperty("fitRIO_OnTrack",       m_fitRIO_OnTrack,"obsolete!");
  declareProperty("useParticleHypothesisFromTrack", m_usetrackhypo,"fit with particle hypothesis from original track");

  // constrained fitting
  declareProperty("ConstrainFit",         m_constrainFitMode,"mode switch if/how the track is constrained to the BS/Vx");
  declareProperty("VertexCollection",     m_vxContainerName,"Source for vertex to use for constraining tracks");
  declareProperty("BeamConditionsSvc",    m_iBeamCondSvc,"Beam spot service in case track is constrained to the BS");
  declareProperty("Extrapolator",         m_extrapolator, "Extrapolator needed for coherent measurement frame.");
  

}

// Initialize method:
StatusCode Trk::ReFitTrack::initialize()
{
  msg(MSG::INFO) << "ReFitTrack::initialize()" << endmsg;


  if (m_fitRIO_OnTrack) {
    msg(MSG::WARNING) << "fitRIO_OnTrack flag: This flag is now obsolete! " << endmsg;
    msg(MSG::INFO) << "fitRIO_OnTrack flag: In the past its effect was to decompose"
		   << " a track into hits before fitting, which misses outliers etc." << endmsg;
    msg(MSG::INFO) << "fitRIO_OnTrack flag: instead, please configure the active "
		   << "track fitter to do the decomposition you want." << endmsg;
  }

  if (m_ITrackFitter.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool "<<m_ITrackFitter.typeAndName()<<endmsg;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved general fitter " << m_ITrackFitter.typeAndName() << endmsg;
  
  if (m_ITrackFitterTRT.name()!=""){
    if (m_ITrackFitterTRT.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_ITrackFitterTRT.typeAndName()<<endmsg;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Retrieved fitter for TRT-only tracks " << m_ITrackFitterTRT.typeAndName() << endmsg;
  }

  if (m_trkSummaryTool.name()!=""){
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
  if (m_constrainFitMode && !m_iBeamCondSvc.empty() ) {
    if ( m_iBeamCondSvc.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Failed to retrieve service " << m_iBeamCondSvc );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_VERBOSE( "Retrieved service " << m_iBeamCondSvc);
  }
  
  // extrapolator
  if (m_constrainFitMode && m_extrapolator.retrieve().isFailure()){
      ATH_MSG_FATAL( "Failed to retrieve " << m_extrapolator );
      return StatusCode::FAILURE;
  }
  
  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_ParticleHypothesis = particleSwitch.particle[m_matEffects];

  ATH_CHECK( m_TrackName.initialize() );
  ATH_CHECK( m_vxContainerName.initialize() );
  ATH_CHECK( m_NewTrackName.initialize() );

  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode Trk::ReFitTrack::execute() 
{
  ATH_MSG_DEBUG ("ReFitTrack::execute()");
  StatusCode sc;

  // clean up association tool
  m_assoTool->reset();

  SG::ReadHandle<TrackCollection> tracks (m_TrackName);

  if (!tracks.isValid()){
    msg(MSG::ERROR) <<"Track collection named " << m_TrackName.key() 
		    << " not found, exit ReFitTrack." << endmsg;
    return StatusCode::SUCCESS;
  }
  else{ 
    ATH_MSG_DEBUG ("Tracks collection '" << m_TrackName.key() << "' retrieved from EventStore.");
  }
  
  // constrainVx
  std::unique_ptr<const Trk::PerigeeSurface> constrainSf;
  std::unique_ptr<const Trk::RecVertex>      cleanup_constrainVx; 
  const Trk::RecVertex*      constrainVx = 0; 
  //const VxContainer* 	     vxContainer = 0;

  ATH_MSG_DEBUG("Track fit configured with constrain option : " << m_constrainFitMode );

  // constrain fit to be done
  if (m_constrainFitMode > 0){
      if ( m_constrainFitMode > 1 ){ 
        // get vertex position and uncertainties from BeamCondSvc
        constrainVx = new Trk::RecVertex(m_iBeamCondSvc->beamVtx());   
        cleanup_constrainVx.reset( constrainVx );
        ATH_MSG_DEBUG("Track fit with BeamSpot constraint (x/y/z)  = " 
            << constrainVx->position().x() << ", " 
            << constrainVx->position().y() << ", " 
            << constrainVx->position().z());
      } else if (m_constrainFitMode ==1){
	  SG::ReadHandle<VxContainer> vxContainer (m_vxContainerName);
	  ATH_MSG_DEBUG("Track fit with vertex from collection '" << m_vxContainerName.key() << "'.");
	  if (!vxContainer.isValid())
	    ATH_MSG_WARNING("Track fit configured to use vertex constraint, but '" << m_vxContainerName.key() << "' could not be retrieved.");
          else if (vxContainer->size() == 1 ) {
	    ATH_MSG_WARNING("Track fit configured to use vertex constraint, but '" << m_vxContainerName.key() << "' contains only dummy vertex.");
          } else {
              // only refit to the 'signal' vertex
              constrainVx = &((*vxContainer)[0]->recVertex());
              ATH_MSG_DEBUG("Track fit with Vertex constraint (x/y/z)  = " 
                  << constrainVx->position().x() << ", " 
                  << constrainVx->position().y() << ", " 
                  << constrainVx->position().z());              
          }
      }
      // constrain surface (either BS of VTX)
      if (constrainVx) {
        constrainSf.reset( new Trk::PerigeeSurface(constrainVx->position()));
      }
  }
    
  ParticleHypothesis hypo=m_ParticleHypothesis;

  // create new collection of tracks to write in storegate
  auto newtracks = std::make_unique<TrackCollection>();
  SG::WriteHandle<TrackCollection> outputtracks (m_NewTrackName);

  // loop over tracks
  for (TrackCollection::const_iterator itr  = (*tracks).begin(); itr < (*tracks).end(); itr++){

    ATH_MSG_VERBOSE ("input track:" << **itr);

    if (m_usetrackhypo) hypo=(**itr).info().particleHypothesis();
    Track* newtrack=0;
    bool trtonly=false;
    
    bool passedSelection = true; 
    // check whether the track passes the selection
    if (*itr && !m_trkSelectorTool.empty()){         
      passedSelection = m_trkSelectorTool->decision(**itr);
    }
   
    if (m_ITrackFitterTRT.name()!="" && m_trkSummaryTool.name()!=""){
      const Trk::TrackSummary *summary = m_trkSummaryTool->createSummaryNoHoleSearch(**itr);
      if ( (**itr).measurementsOnTrack()->size() - summary->get(numberOfTRTHits)<3 )
        trtonly=true;
      delete summary;
    }

    if (passedSelection) {

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
         // extrapolate the track to the vertex -- for consistent Measurement frame
         std::unique_ptr<const Trk::TrackParameters> tp( m_extrapolator->extrapolate(**itr,*constrainSf,Trk::anyDirection) );
         const Trk::Perigee* tpConstrainedSf = dynamic_cast<const Trk::Perigee*>(tp.get());
         // create the vertex/beamsptOnTrack
         std::unique_ptr<Trk::VertexOnTrack> bsvxOnTrack( tpConstrainedSf ? new Trk::VertexOnTrack(*constrainVx,*tpConstrainedSf) : 0 );
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
         newtrack = (trtonly ? m_ITrackFitterTRT : m_ITrackFitter)->fit(vec, *origPerigee, m_runOutlier,hypo);
         // now delete the vsvx
        
      } else 
        newtrack = (trtonly ? m_ITrackFitterTRT : m_ITrackFitter)->fit(**itr,m_runOutlier,hypo);
    }

    if (newtrack) newtracks->push_back(newtrack);

    if (msgLvl(MSG::DEBUG)) {
      if (newtrack==0) ATH_MSG_DEBUG ("Refit Failed");
      else {
      
        ATH_MSG_VERBOSE ("re-fitted track:" << *newtrack);
        const Trk::Perigee *aMeasPer=
          dynamic_cast<const Trk::Perigee*>(newtrack->perigeeParameters () );
        if (aMeasPer==0){
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
  }

  // recreate the summaries on the final track collection with correct PRD tool
  TrackCollection::const_iterator t  = newtracks->begin();
  TrackCollection::const_iterator te = newtracks->end  ();
  for ( ; t!=te; ++t) {
    if((m_assoTool->addPRDs(**t)).isFailure()) {ATH_MSG_WARNING("Failed to add PRDs to map");}
  }
  // now recalculate the summary ... the usual nasty const cast is needed here
  t  = newtracks->begin();
  for ( ; t!=te; ++t) {
    Trk::Track& nonConstTrack = const_cast<Trk::Track&>(**t);
    m_trkSummaryTool->updateTrack(nonConstTrack);
  }

  ATH_CHECK(outputtracks.record(std::move(newtracks)));

  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode Trk::ReFitTrack::finalize() 
{
  msg(MSG::INFO) << "ReFitTrack::finalize()" << endmsg;
  return StatusCode::SUCCESS;
}
