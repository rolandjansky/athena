/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSelectorTool/InDetCosmicTrackSelectorTool.h"

// forward declares
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "VxVertex/Vertex.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"

// normal includes
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "AthContainers/DataVector.h"


namespace InDet
{
  //----------------------------------------------------------------------------
  InDetCosmicTrackSelectorTool::InDetCosmicTrackSelectorTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p), 
    m_trackSumToolAvailable(false),
    m_magFieldSvc("AtlasFieldSvc",n)
  {
    declareInterface<ITrackSelectorTool>(this);
    declareProperty("maxZ0",                     m_maxZ0 = 150.);
    declareProperty("maxD0",                     m_maxD0 = 2.5);
    declareProperty("minPt",                     m_minPt = 0.);
    declareProperty("numberOfPixelHits",         m_numberOfPixelHits = 0);
    declareProperty("numberOfSCTHits",           m_numberOfSCTHits = 0);
    declareProperty("numberOfTRTHits",           m_numberOfTRTHits = 15);
    declareProperty("numberOfSiliconHits",       m_numberOfSiHits = 8);
    declareProperty("numberOfSiliconHitsTop",    m_numberOfSiHitsTop = -1);
    declareProperty("numberOfSiliconHitsBottom", m_numberOfSiHitsBottom = -1);
    declareProperty("TrackSummaryTool",          m_trackSumTool);
    declareProperty("MagFieldSvc", m_magFieldSvc);
  }

  //----------------------------------------------------------------------------
  InDetCosmicTrackSelectorTool::~InDetCosmicTrackSelectorTool()
  {}

  //----------------------------------------------------------------------------
  StatusCode  InDetCosmicTrackSelectorTool::initialize()
  {
    if(AthAlgTool::initialize().isFailure()) {
      msg(MSG::ERROR)<<" Unable to initialize the AlgTool"<<endmsg;
      return StatusCode::FAILURE;
    }

     m_trackSumToolAvailable = false;
     if (!m_trackSumTool.empty()) {
       if(m_trackSumTool.retrieve().isFailure())
       {
         msg(MSG::ERROR)<<" Unable to retrieve "<<m_trackSumTool<<endmsg;
         return StatusCode::FAILURE;
       }
       ATH_MSG_INFO("Track summary tool retrieved");
       m_trackSumToolAvailable = true;
     }

     if (m_magFieldSvc.retrieve().isFailure()) {
       msg(MSG::FATAL) << "Failed to retrieve tool " << m_magFieldSvc << endmsg;
       return StatusCode::FAILURE;
     }
     ATH_MSG_INFO("Retrieved tool "<<m_magFieldSvc);

     return StatusCode::SUCCESS;
  }

  //----------------------------------------------------------------------------
  StatusCode InDetCosmicTrackSelectorTool::finalize()
  {
    ATH_MSG_INFO("Finalize successful");
    return StatusCode::SUCCESS;
  }

  //----------------------------------------------------------------------------
  bool InDetCosmicTrackSelectorTool::decision(const Trk::Track & track, const Trk::Vertex * vertex) const
  {

    // decision based on the track parameters
    if(!decision(track.perigeeParameters(), vertex, track.info().particleHypothesis()))
      return false;

    // number of hits, silicon hits, b-layer
    // first ask track for summary
    const Trk::TrackSummary * summary = track.trackSummary();
    if (summary == 0 && m_trackSumToolAvailable) {
      // ugly but one needs to cast the const away because the method needs to update the track (the tool is a friend of track)
      Trk::Track& nonConstTrack = const_cast<Trk::Track&>(track);
      m_trackSumTool->updateTrack(nonConstTrack);
      // now get it from the track (the track has OWNERSHIP)
      summary = nonConstTrack.trackSummary();
    }

    if (0==summary) {
      ATH_MSG_DEBUG( "Track preselection: cannot create a track summary. This track will not pass." );
      return false;
    }

    int nPixHits = summary->get(Trk::numberOfPixelHits);
    int nSCTHits = summary->get(Trk::numberOfSCTHits);
    int nTRTHits = summary->get(Trk::numberOfTRTHits);
    int nSiHits  = summary->get(Trk::numberOfPixelHits) * 2 + summary->get(Trk::numberOfSCTHits);
    int nSiHitsTop     = getNSiHits(&track,true);
    int nSiHitsBottom  = getNSiHits(&track,false);

    if(nPixHits<m_numberOfPixelHits) {
      ATH_MSG_DEBUG("Track rejected because of numberOfPixelHits "<<nPixHits<<"<"<<m_numberOfPixelHits);
      return false;
    }

    if(nSCTHits<m_numberOfSCTHits) {
      ATH_MSG_DEBUG("Track rejected because of numberOfSCTHits "<<nSCTHits<<"<"<<m_numberOfSCTHits);
      return false;
    }

    if(nTRTHits<m_numberOfTRTHits) {
      ATH_MSG_DEBUG("Track rejected because of numberOfTRTHits "<<nTRTHits<<"<"<<m_numberOfTRTHits);
      return false;
    }

    if(nSiHits<m_numberOfSiHits) {
      ATH_MSG_DEBUG("Track rejected because of numberOfSiHits "<<nSiHits<<"<"<<m_numberOfSiHits);
      return false;
    }

    if(nSiHitsTop<m_numberOfSiHitsTop) {
      ATH_MSG_DEBUG("Track rejected because of nSiHitsTop "<<nSiHitsTop<<"<"<<m_numberOfSiHitsTop);
      return false;
    }

    if(nSiHitsBottom<m_numberOfSiHitsBottom) {
      ATH_MSG_DEBUG("Track rejected because of numberOfSiHitsBottom "<<nSiHitsBottom<<"<"<<m_numberOfSiHitsBottom);
      return false;
    }

    // all ok
    return true;
  }

  //----------------------------------------------------------------------------
  bool InDetCosmicTrackSelectorTool::decision(const Trk::TrackParticleBase & track,const Trk::Vertex * vertex) const
  {
    if(!decision(&(track.definingParameters()), vertex, Trk::pion))
      return false;

    const Trk::TrackSummary * summary = track.trackSummary();
    if (0==summary ) {
      ATH_MSG_DEBUG( "TrackParticleBase does not have a Track Summary. Rejected." );
      return false;
    }
    const Trk::Track *  otrack= track.originalTrack();

    if(otrack==0){
      ATH_MSG_DEBUG(  "TrackParticleBase does not contain the original cosmic track. Rejected." );
      return false;
    }

    int nPixHits = summary->get(Trk::numberOfPixelHits);
    int nSCTHits = summary->get(Trk::numberOfSCTHits);
    int nTRTHits = summary->get(Trk::numberOfTRTHits);
    int nSiHits  = summary->get(Trk::numberOfPixelHits)*2+summary->get(Trk::numberOfSCTHits);
    int nSiHitsTop     = getNSiHits(otrack,true);
    int nSiHitsBottom  = getNSiHits(otrack,false);

    if(nPixHits<m_numberOfPixelHits) {
      ATH_MSG_DEBUG("Track rejected because of numberOfPixelHits "<<nPixHits<<"<"<<m_numberOfPixelHits);
      return false;
    }

    if(nSCTHits<m_numberOfSCTHits) {
      ATH_MSG_DEBUG("Track rejected because of numberOfSCTHits "<<nSCTHits<<"<"<<m_numberOfSCTHits);
      return false;
    }

    if(nTRTHits<m_numberOfTRTHits) {
      ATH_MSG_DEBUG("Track rejected because of numberOfTRTHits "<<nTRTHits<<"<"<<m_numberOfTRTHits);
      return false;
    }

    if(nSiHits<m_numberOfSiHits) {
      ATH_MSG_DEBUG("Track rejected because of numberOfSiHits "<<nSiHits<<"<"<<m_numberOfSiHits);
      return false;
    }

    if(nSiHitsTop<m_numberOfSiHitsTop) {
      ATH_MSG_DEBUG("Track rejected because of nSiHitsTop "<<nSiHitsTop<<"<"<<m_numberOfSiHitsTop);
      return false;
    }

    if(nSiHitsBottom<m_numberOfSiHitsBottom) {
      ATH_MSG_DEBUG("Track rejected because of numberOfSiHitsBottom "<<nSiHitsBottom<<"<"<<m_numberOfSiHitsBottom);
      return false;
    }

    // all ok
    return true;
  }

  //----------------------------------------------------------------------------
  bool InDetCosmicTrackSelectorTool::decision(const Trk::TrackParameters* track, const Trk::Vertex *, const Trk::ParticleHypothesis) const
  {
    // checking pointer first
    if(0==track) {
      ATH_MSG_DEBUG( "Track preselection: Zero pointer to parameterbase* received (most likely a track without perigee). This track will not pass." );
      return false;
    }

    // getting the  perigee parameters of the track
    const Trk::Perigee * perigee(0);
    perigee = dynamic_cast<const Trk::Perigee *>(track);

    if(!perigee || !perigee->covariance()) {
     ATH_MSG_DEBUG( "Track preselection: cannot make a measured perigee. This track will not pass." );
     return false;
    }

    Amg::VectorX trackParameters = perigee->parameters();

    // d0 and z0 cuts
    double d0 = trackParameters[Trk::d0];
    if(fabs(d0) > fabs(m_maxD0)) {
      ATH_MSG_DEBUG("Track rejected because of d0 "<<fabs(d0)<<">"<<m_maxD0);
      return false;
    }

    double z0 = trackParameters[Trk::z0];
    if(fabs(z0) > fabs(m_maxZ0)) {
      ATH_MSG_DEBUG("Track rejected because of z0 "<<fabs(z0)<<">"<<m_maxZ0);
      return false;
    }

    // only check pt if mag. field is on
    if (m_magFieldSvc->solenoidOn()) {
      if (trackParameters[Trk::qOverP] == 0.) {
        ATH_MSG_DEBUG("Track rejected because of qOverP == 0.");
        return false;
      }

      double pt = fabs(1./trackParameters[Trk::qOverP])*sin(trackParameters[Trk::theta]);
      if (pt < m_minPt) {
        ATH_MSG_DEBUG("Track rejected because of pt " << pt << " < " << m_minPt);
        return false;
      }
    }
    return true;
  }

  //----------------------------------------------------------------------------
  int InDetCosmicTrackSelectorTool::getNSiHits(const Trk::Track * track, bool top) const
  {
    int nsilicon = 0;

    //loop over all measurements on Track
    DataVector<const Trk::MeasurementBase>::const_iterator it    = track->measurementsOnTrack()->begin();
    DataVector<const Trk::MeasurementBase>::const_iterator itEnd = track->measurementsOnTrack()->end();
    for ( ; it!=itEnd; ++it) {
      const Trk::RIO_OnTrack *rot = dynamic_cast<const Trk::RIO_OnTrack *>(*it);
      if (!rot)
        continue;
      const InDet::SiClusterOnTrack *siclus = dynamic_cast<const InDet::SiClusterOnTrack *>(rot);
      if(!siclus)
        continue;

      if(top && siclus->globalPosition().y()<0)
        continue;
      if(!top && siclus->globalPosition().y()>0)
        continue;

      const SCT_ClusterOnTrack *sctclus = dynamic_cast<const SCT_ClusterOnTrack *>(siclus);
      if(!sctclus)
        //Pixel hit
        nsilicon += 2;
      else
        nsilicon += 1;
    }
    return nsilicon;
  }

} //end of namespace definitions
