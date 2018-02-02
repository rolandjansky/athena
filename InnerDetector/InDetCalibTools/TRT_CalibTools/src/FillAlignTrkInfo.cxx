/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************


********************************************************************/

//#include "GaudiKernel/MsgStream.h"
#include "FillAlignTrkInfo.h"
#include "TRT_CalibData/TrackInfo.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "GaudiKernel/ListItem.h"

#include <map>

FillAlignTrkInfo::FillAlignTrkInfo(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_TrackSummaryTool("Trk::TrackSummaryTool/TrackSummaryTool")
{
  declareInterface<IFillAlignTrkInfo>(this);
  declareProperty("TrackSummaryTool",m_TrackSummaryTool);
}

StatusCode FillAlignTrkInfo::initialize()
{
  msg(MSG::INFO) << "initialize()" << endmsg;
  if (m_TrackSummaryTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Cannot get TrackSummaryTool" << endmsg;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "finished initialize()" << endmsg;
  return StatusCode::SUCCESS ;
}

//extern std::map<const Trk::Track*, double> gTrkToTrkT0Map ;

bool FillAlignTrkInfo::fill(const Trk::Track* aTrack, TRT::TrackInfo* output,
                            const ComTime* /* theComTime */, const xAOD::EventInfo& /* eventInfo */,
                            const xAOD::VertexContainer& /* vertices */) {
//bool FillAlignTrkInfo::fill(const Trk::Track* aTrack, TRT::TrackInfo* output) const {
  // track parameters
  const Trk::Perigee* mesp=(aTrack->perigeeParameters());
  if (mesp!=0) {
    (*output)[TRT::Track::d0]=mesp->parameters()[Trk::d0];
    (*output)[TRT::Track::z0]=mesp->parameters()[Trk::z0];
    (*output)[TRT::Track::phi]=mesp->parameters()[Trk::phi];
    // transform back to cot(theta), q/pt representation for alignment ntuple
    (*output)[TRT::Track::cotTheta]=1/tan(mesp->parameters()[Trk::theta]);
    (*output)[TRT::Track::qOverPT]=(mesp->parameters()[Trk::qOverP])/sin(mesp->parameters()[Trk::theta]);
//    std::map<const Trk::Track*, double>::const_iterator it = gTrkToTrkT0Map.find(aTrack) ;
//    (*output)[TRT::Track::t0] = it!=gTrkToTrkT0Map.end() ? it->second : 0 ;
    (*output)[TRT::Track::t0] = 0.0 ;
  } else {
    msg(MSG::FATAL) << " Could not get Trk::Perigee " << endmsg;
    (*output)[TRT::Track::d0]=0.0;
    (*output)[TRT::Track::z0]=0.0;
    (*output)[TRT::Track::phi]=0.0;
    (*output)[TRT::Track::cotTheta]=0.0;
    (*output)[TRT::Track::qOverPT]=0.0;
//    (*output)[TRT::Track::t0] = gTrkToTrkT0Map[aTrack]  ;
    (*output)[TRT::Track::t0] = 0.0  ;
  }
  (*output)[TRT::Track::chiSquare]=aTrack->fitQuality()->chiSquared();
  (*output)[TRT::Track::degreesOfFreedom]=aTrack->fitQuality()->numberDoF();

  // implicit memory allocation in createSummary, need to clean up later 
  const Trk::TrackSummary* summary = m_TrackSummaryTool->createSummary(*aTrack);
  (*output)[TRT::Track::numberOfPixelHits]=summary->get(Trk::numberOfPixelHits) ;
  (*output)[TRT::Track::numberOfSCTHits]=summary->get(Trk::numberOfSCTHits) ;
  (*output)[TRT::Track::numberOfTRTHits]=summary->get(Trk::numberOfTRTHits) ;

  // fix to coverity 118333
  delete summary;

  // All ok
  if (msgLvl(MSG::DEBUG)) msg() << "Track info filled .... " << endmsg;
  return true;
}
