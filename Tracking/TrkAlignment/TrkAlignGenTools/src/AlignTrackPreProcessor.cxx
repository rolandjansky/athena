/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAlignGenTools/AlignTrackPreProcessor.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterInterfaces/IGlobalTrackFitter.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

#include "TrkAlignEvent/AlignTrack.h"

namespace Trk {

  //________________________________________________________________________
  AlignTrackPreProcessor::AlignTrackPreProcessor(const std::string & type,
                                                 const std::string & name,
                                                 const IInterface  * parent)
    : AthAlgTool(type,name,parent)
    , m_trackFitterTool("Trk::GlobalChi2Fitter/MCTBFitter")
    , m_SLTrackFitterTool("")
    , m_trackSelectorTool("")
    , m_runOutlierRemoval(false)
    , m_particleHypothesis(Trk::nonInteracting)
    , m_useSingleFitter(false)
  {
    declareInterface<IAlignTrackPreProcessor>(this);

    declareProperty("RefitTracks", m_refitTracks = true);

    declareProperty("TrackFitterTool",   m_trackFitterTool);
    declareProperty("SLTrackFitterTool", m_SLTrackFitterTool);
    declareProperty("UseSingleFitter",   m_useSingleFitter);

    declareProperty("StoreFitMatricesAfterRefit", m_storeFitMatricesAfterRefit = true);

    declareProperty("TrackSelectorTool", m_trackSelectorTool);
    declareProperty("SelectTracks", m_selectTracks = false);

    declareProperty("ParticleHypothesis", m_particleHypothesis);
    declareProperty("RunOutlierRemoval",  m_runOutlierRemoval = false);

    m_logStream = 0;
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
      msg(MSG::FATAL) << "Could not get " << m_trackFitterTool << endreq;
      return StatusCode::FAILURE;
    }

    if (!m_useSingleFitter) {
      if (m_SLTrackFitterTool.retrieve().isSuccess())
        ATH_MSG_INFO("Retrieved " << m_SLTrackFitterTool);
      else {
        msg(MSG::FATAL) << "Could not get " << m_SLTrackFitterTool << endreq;
        return StatusCode::FAILURE;
      }
    }

    if(m_selectTracks) {
      if(m_trackSelectorTool.empty()) {
        msg(MSG::FATAL) << "TrackSelectorTool not specified : " << m_trackSelectorTool << endreq;
        return StatusCode::FAILURE;
      }
      else if(m_trackSelectorTool.retrieve().isFailure())
      {
        msg(MSG::FATAL) << "Could not get " << m_trackSelectorTool << endreq;
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Retrieved " << m_trackSelectorTool);
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

    if (!tracks || tracks->size()==0)
      return 0;

    // the output collection of AlignTracks
    // we define it as collection of Tracks but fill AlignTracks inside
    DataVector<Track> * newTracks = new DataVector<Track>;

    int itrk(0);
    // loop over tracks and create AlignTracks
    TrackCollection::const_iterator it     = tracks->begin();
    TrackCollection::const_iterator it_end = tracks->end();

    for ( ; it != it_end ; ++it, ++itrk) {

      ATH_MSG_DEBUG("Processing track "<<itrk);
      Track * origTrack = *it;
      Track * newTrack = *it;
      AlignTrack* at;

      // check whether the track passes selection
      if (m_selectTracks)
        if(!m_trackSelectorTool->decision(*origTrack)) {
          ATH_MSG_DEBUG("Track did not pass the selection.");
          continue;
        }

      // refit track
      if (m_refitTracks) {

        ToolHandle<Trk::IGlobalTrackFitter> fitter=m_trackFitterTool;
        if (!m_useSingleFitter && AlignTrack::isSLTrack(origTrack) )
          fitter = m_SLTrackFitterTool;

        newTrack=fitter->fit(*origTrack,m_runOutlierRemoval,ParticleHypothesis(m_particleHypothesis));
        if (!newTrack) {
          ATH_MSG_DEBUG("Track refit yielded no track. Skipping the track.");
          continue;
        }

        at = new AlignTrack(*newTrack);

        if (msgLvl(MSG::DEBUG) && !msgLvl(MSG::VERBOSE)) {
          msg(MSG::DEBUG)<<"before refit: "<<endreq;
          AlignTrack::dumpLessTrackInfo(*origTrack,msg(MSG::DEBUG));
          msg(MSG::DEBUG)<<"after refit: "<<endreq;
          AlignTrack::dumpLessTrackInfo(*newTrack,msg(MSG::DEBUG));
          msg(MSG::DEBUG)<<endreq;
        }

        // store fit matrices
        if (m_storeFitMatricesAfterRefit) {
          at->setFullCovarianceMatrix(fitter->FullCovarianceMatrix());
          at->setDerivativeMatrix(fitter->DerivMatrix());
        }

        // delete newTrack since it's copied in AlignTrack
        delete newTrack;
      }

      else
        at=new AlignTrack(*newTrack);

      newTracks->push_back(at);

    }

    if (newTracks->size()==0) {
      delete newTracks;
      return 0;
    }

    return newTracks;
  }

}
