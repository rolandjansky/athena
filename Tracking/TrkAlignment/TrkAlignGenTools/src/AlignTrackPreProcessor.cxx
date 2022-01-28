/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAlignGenTools/AlignTrackPreProcessor.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//Need to update the code to TrackParticles? The track Parameters of the track particles will be calculated wrt the new Beam Spot position.

//#include "xAODTracking/TrackParticle.h"
//#include "xAODTracking/TrackParticleContainer.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterInterfaces/IGlobalTrackFitter.h"

//Old Interface
//#include "TrkToolInterfaces/ITrackSelectorTool.h"




#include "TrkAlignEvent/AlignTrack.h"
#include "EventPrimitives/EventPrimitives.h"

namespace Trk {

  //________________________________________________________________________
  AlignTrackPreProcessor::AlignTrackPreProcessor(const std::string & type,
                                                 const std::string & name,
                                                 const IInterface  * parent)
    : AthAlgTool(type,name,parent)
    , m_trackFitterTool("Trk::GlobalChi2Fitter/InDetTrackFitter")
    , m_SLTrackFitterTool("")
    , m_trackSelectorTool("")
    , m_runOutlierRemoval(false)
    , m_particleHypothesis(Trk::nonInteracting)
    , m_useSingleFitter(false)
    , m_selectHits(false)
    , m_fixMomentum(false)
  {
    declareInterface<IAlignTrackPreProcessor>(this);

    declareProperty("RefitTracks", m_refitTracks = true);

    declareProperty("TrackFitterTool",   m_trackFitterTool);
    declareProperty("SLTrackFitterTool", m_SLTrackFitterTool);
    declareProperty("UseSingleFitter",   m_useSingleFitter = false);

    declareProperty("StoreFitMatricesAfterRefit", m_storeFitMatricesAfterRefit = true);

    declareProperty("TrackSelectorTool", m_trackSelectorTool);
    declareProperty("SelectTracks", m_selectTracks = false);

    declareProperty("ParticleHypothesis", m_particleHypothesis);
    declareProperty("RunOutlierRemoval",  m_runOutlierRemoval = false);

    declareProperty("HitQualityTool", m_hitQualityTool);
    declareProperty("SelectHits", m_selectHits);
    declareProperty("FixMomentum", m_fixMomentum);

    m_logStream = nullptr;
  }

  //________________________________________________________________________
  AlignTrackPreProcessor::~AlignTrackPreProcessor()
  {

  }

  //________________________________________________________________________
  StatusCode AlignTrackPreProcessor::initialize()
  {
    // get track fitter tools
    if (m_trackFitterTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_trackFitterTool);
    else{
      msg(MSG::FATAL) << "Could not get " << m_trackFitterTool << endmsg;
      return StatusCode::FAILURE;
    }

    if (m_useSingleFitter) {
      
      if (m_SLTrackFitterTool.retrieve().isSuccess())
        ATH_MSG_INFO("Retrieved " << m_SLTrackFitterTool);
      else {
        msg(MSG::FATAL) << "Could not get " << m_SLTrackFitterTool << endmsg;
        return StatusCode::FAILURE;
      }
    }

    if(m_selectTracks) {
      if(m_trackSelectorTool.empty()) {
        msg(MSG::FATAL) << "TrackSelectorTool not specified : " << m_trackSelectorTool << endmsg;
        return StatusCode::FAILURE;
      }
      else if(m_trackSelectorTool.retrieve().isFailure())
      {
        msg(MSG::FATAL) << "Could not get " << m_trackSelectorTool << endmsg;
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Retrieved " << m_trackSelectorTool);
    }

    if (m_selectHits) {
      if(m_hitQualityTool.empty()) {
  msg(MSG::FATAL) << "HitQualityTool not specified : " << m_hitQualityTool << endmsg;
  return StatusCode::FAILURE;
      }
      else if(m_hitQualityTool.retrieve().isFailure())
  {
    msg(MSG::FATAL) << "Could not get " << m_trackSelectorTool << endmsg;
    return StatusCode::FAILURE;
  }
      ATH_MSG_INFO("Retrieved " << m_hitQualityTool);
    }

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode AlignTrackPreProcessor::finalize()
  {
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  DataVector<Track>* AlignTrackPreProcessor::processTrackCollection(const DataVector<Track>* tracks)
  {
    ATH_MSG_DEBUG("AlignTrackPreProcessor::processTrackCollection()");

    if (!tracks || tracks->empty())
      return nullptr;

    // the output collection of AlignTracks
    // we define it as collection of Tracks but fill AlignTracks inside
    DataVector<Track> * newTracks = new DataVector<Track>;

    int itrk(0);
    // loop over tracks and create AlignTracks
    TrackCollection::const_iterator it     = tracks->begin();
    TrackCollection::const_iterator it_end = tracks->end();

    for ( ; it != it_end ; ++it, ++itrk) {

      ATH_MSG_DEBUG(" ** processTrackCollection ** Processing track "<<itrk);
      const Track * origTrack = *it;
      const Track * newTrack = *it;
      AlignTrack* at;

      // check whether the original track passes selection
      if (m_selectTracks) {
        if(!m_trackSelectorTool->accept(*origTrack)) {
          ATH_MSG_VERBOSE(" ** processTrackCollection ** Original track did not pass the selection."<<itrk);
          continue;
        }

        ToolHandle<Trk::IGlobalTrackFitter> fitter=m_trackFitterTool;
        if (!m_useSingleFitter && AlignTrack::isSLTrack(origTrack) )
          fitter = m_SLTrackFitterTool;

        if (m_selectHits) {
          /** select silicon hits by quality. keep all the rest **/
          ATH_MSG_DEBUG(" ** processTrackCollection ** entering the silicon hit selection ");
          newTrack = performSiliconHitSelection(origTrack, fitter);

          if (!newTrack) {
            ATH_MSG_DEBUG(" ** processTrackCollection ** Track refit yielded no track. Skipping the track.");
            continue;
          }

          // check whether the track passes selection
          if(!m_trackSelectorTool->accept(*newTrack)) {
            ATH_MSG_DEBUG(" ** processTrackCollection ** Track did not pass the selection.");
            delete newTrack;
            continue;
          }
        }

        IGlobalTrackFitter::AlignmentCache alignCache;
        // refit track
        if (m_refitTracks &!m_selectHits) {

          newTrack=fitter->alignmentFit(alignCache,*origTrack,m_runOutlierRemoval,ParticleHypothesis(m_particleHypothesis));
          if (!newTrack) {
            ATH_MSG_DEBUG("Track refit yielded no track. Skipping the track.");
            continue;
          }
          // check that the refitted track satisfies the aligntrack selection
          if(!m_trackSelectorTool->accept(*newTrack)) {
            ATH_MSG_DEBUG("New track did not pass the selection.");
            delete newTrack;
            continue;
          }
        }

        at = new AlignTrack(*newTrack);

        if (msgLvl(MSG::DEBUG) && !msgLvl(MSG::VERBOSE)) {
          msg(MSG::DEBUG)<<"before refit: "<<endmsg;
          AlignTrack::dumpLessTrackInfo(*origTrack,msg(MSG::DEBUG));
          msg(MSG::DEBUG)<<"after refit: "<<endmsg;
          AlignTrack::dumpLessTrackInfo(*newTrack,msg(MSG::DEBUG));
          msg(MSG::DEBUG)<<endmsg;
        }

        // store fit matrices
        if (m_storeFitMatricesAfterRefit) {
          at->setFullCovarianceMatrix(alignCache.m_fullCovarianceMatrix);
          at->setDerivativeMatrix(alignCache.m_derivMatrix);
        }
        if (m_fixMomentum)
          {
             at->AlignTrack::setRefitQovP(false);
          }
        // delete newTrack since it's copied in AlignTrack
        delete newTrack;
      }
      else { // in case no selection is performed, keep all tracks
        at=new AlignTrack(*newTrack);
      }
  	if (m_fixMomentum)
 	   {
  	     at->AlignTrack::setRefitQovP(false);
 	   }

      newTracks->push_back(at);
    }


    if (newTracks->empty()) {
      delete newTracks;
      return nullptr;
    }

    return newTracks;
  }

   //________________________________________________________________________
  Track * AlignTrackPreProcessor::performSiliconHitSelection(const Track * inputTrack, const ToolHandle<Trk::IGlobalTrackFitter> & fitter)
  {
    /** select silicon hits by quality. keep all the rest **/
    ATH_MSG_DEBUG(" -- performSiliconHitSelection -- before removing bad Silicon hits, this track has "<< inputTrack->trackStateOnSurfaces()->size()<< " tsos");
    Track * newTrack;

    std::vector<const Trk::MeasurementBase*> selectedMeasurementSet;

    // loop on track hits
    int nhits = 0;
    for (const Trk::TrackStateOnSurface* tsos : *inputTrack->trackStateOnSurfaces())
    {
      nhits++;
      if (m_hitQualityTool->isGoodSiHit(tsos)) {
        selectedMeasurementSet.push_back( tsos->measurementOnTrack() );
      }
      else {
        ATH_MSG_DEBUG(" -- performSiliconHitSelection -- hit # "<< nhits << " status = BAD HIT ");
      }
    }
    ATH_MSG_DEBUG(" -- performSiliconHitSelection -- after removing bad Silicon hits, the selected measurement collection has "<< selectedMeasurementSet.size()<< " elements");

    newTrack = (fitter->fit(Gaudi::Hive::currentContext(),
                            selectedMeasurementSet,
                            *inputTrack->perigeeParameters(),
                            m_runOutlierRemoval,ParticleHypothesis(m_particleHypothesis))).release();

    return newTrack;
  }
}
