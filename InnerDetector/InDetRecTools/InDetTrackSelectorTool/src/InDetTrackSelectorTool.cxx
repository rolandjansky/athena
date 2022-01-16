/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSelectorTool/InDetTrackSelectorTool.h"
// forward declares
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "VxVertex/Vertex.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkTrack/Track.h"
// normal includes
#include "TrkTrackSummary/TrackSummary.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Units/SystemOfUnits.h"

using CLHEP::mm;

namespace InDet
{

//_______________________________________________________________________________
InDetTrackSelectorTool::InDetTrackSelectorTool(const std::string & t, const std::string & n, const IInterface * p)
  : AthAlgTool(t,n,p)
  , m_minPt(500.)
  , m_IPz0Max(10.*mm)
  , m_maxZ0(99999.)
  , m_maxD0(2.*mm)
  , m_maxD0overSigmaD0(3.)
  , m_numberOfPixelHits(2)
  , m_numberOfBLayerHits(1)
  , m_trackSumToolAvailable(false) 
  , m_extrapolator("Trk::Extrapolator")
{
  declareInterface<ITrackSelectorTool>(this);
  declareProperty("minPt",                    m_minPt);
  declareProperty("IPz0Max",                  m_IPz0Max); // cut on |z|*sin(theta)
  declareProperty("maxZ0",                    m_maxZ0);
  declareProperty("maxD0",                    m_maxD0);
  declareProperty("maxD0overSigmaD0",         m_maxD0overSigmaD0);
  declareProperty("numberOfPixelHits",        m_numberOfPixelHits);
  declareProperty("numberOfInnermostPixelLayerHits",       m_numberOfBLayerHits);
  declareProperty("TrackSummaryTool",         m_trackSumTool);
  declareProperty("Extrapolator",             m_extrapolator);
}

//_______________________________________________________________________________
InDetTrackSelectorTool::~InDetTrackSelectorTool()
{}

//_______________________________________________________________________________
StatusCode InDetTrackSelectorTool::initialize()
{
  m_trackSumToolAvailable = false;
  if (!m_trackSumTool.empty())
  {
    if(m_trackSumTool.retrieve().isFailure()) {
      msg(MSG::ERROR)<<" Unable to retrieve "<<m_trackSumTool<<endmsg;
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_INFO("Track summary tool retrieved");
      m_trackSumToolAvailable = true;
    }
  }

  if ( m_extrapolator.retrieve().isFailure() ) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_extrapolator << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool " << m_extrapolator);

  return StatusCode::SUCCESS;
}


//_______________________________________________________________________________
bool InDetTrackSelectorTool::decision(const Trk::Track & track, const Trk::Vertex * vertex) const
{
  // decision based on the track parameters
  if(!decision(track.perigeeParameters(), vertex, track.info().particleHypothesis()))
    return false;

  // number of hits, silicon hits, b-layer
  // first ask track for summary
  std::unique_ptr<Trk::TrackSummary> summaryUniquePtr;
  const Trk::TrackSummary * summary = track.trackSummary();
  if (summary == nullptr && m_trackSumToolAvailable) {
    summaryUniquePtr = m_trackSumTool->summary(track);
    summary = summaryUniquePtr.get();
  }

  if (nullptr==summary) {
   ATH_MSG_DEBUG( "Track preselection: cannot create a track summary. This track will not pass." );
   return false;
  }

  int nPixelHits = summary->get(Trk::numberOfPixelHits);
  int nPixelDead = summary->get(Trk::numberOfPixelDeadSensors);
  if (nPixelDead<0)
    nPixelDead=0;

  int nBLayerHits = summary->get(Trk::numberOfInnermostPixelLayerHits);

  if(nPixelHits+nPixelDead<m_numberOfPixelHits || nBLayerHits<m_numberOfBLayerHits )
    return false;

  // all ok
  return true;
}

//_______________________________________________________________________________
bool InDetTrackSelectorTool::decision(const Trk::TrackParticleBase & track, const Trk::Vertex * vertex) const
{
  if(!decision(&(track.definingParameters()), vertex, Trk::pion))
    return false;

  const Trk::TrackSummary * summary = track.trackSummary();
  if (summary == nullptr) {
    ATH_MSG_INFO( "TrackParticleBase does not have a Track Summary. Rejected." );
    return false;
  }
  int nPixelHits  =  summary->get(Trk::numberOfPixelHits);
  int nPixelDead  =  summary->get(Trk::numberOfPixelDeadSensors);
  if (nPixelDead<0)
    nPixelDead=0;

  int nBLayerHits =  summary->get(Trk::numberOfInnermostPixelLayerHits);

  return !(nPixelHits+nPixelDead<m_numberOfPixelHits || nBLayerHits<m_numberOfBLayerHits);
}

//_______________________________________________________________________________
bool InDetTrackSelectorTool::decision(const Trk::TrackParameters * track, const Trk::Vertex * vertex, const Trk::ParticleHypothesis hyp) const
{
  // checking pointer first
  if(nullptr==track || !track->covariance()) {
    ATH_MSG_WARNING( "Track preselection: Zero pointer to parameterbase* received (most likely a track without perigee). This track will not pass." );
    return false;
  }

  // getting the  perigee parameters of the track
  const Trk::Perigee * perigee(nullptr);
  if(vertex == nullptr)
    perigee = dynamic_cast<const Trk::Perigee *>(track);
  else {
    Trk::PerigeeSurface perigeeSurface(vertex->position());
    perigee = dynamic_cast<const Trk::Perigee *>(m_extrapolator->extrapolate(Gaudi::Hive::currentContext(),
                                                                             *track,
                                                                             perigeeSurface,
                                                                             Trk::anyDirection,true,hyp));
  }

  if(nullptr == perigee || !perigee->covariance() ) {
   ATH_MSG_INFO( "Track preselection: cannot make a measured perigee. This track will not pass." );
   return false;
  }

  AmgVector(5) trackParameters = perigee->parameters();

  // d0 and z0 cuts
  double d0 = trackParameters[Trk::d0];
  if(fabs(d0) > m_maxD0) { if(vertex != nullptr) { delete perigee; } return false; }

  double z0 = trackParameters[Trk::z0];
  if (fabs(z0)*sin(trackParameters[Trk::theta]) > m_IPz0Max)
  { if(vertex != nullptr) { delete perigee; } return false; }
  if (fabs(z0) > m_maxZ0)
  { if(vertex != nullptr) { delete perigee; } return false; }

  // transverse momentum
  double pt = perigee->momentum().perp();
  if(pt<m_minPt) { if(vertex != nullptr) { delete perigee; } return false; }

  // d0 significance
  double d0Significance=fabs(trackParameters[Trk::d0]/sqrt( (*perigee->covariance())(Trk::d0,Trk::d0) ));
  if (d0Significance>m_maxD0overSigmaD0)
  { if(vertex != nullptr) { delete perigee; } return false; }

  if(vertex != nullptr) { delete perigee; }
  return true;
} //end of selection method

} //end of namespace definitions
