/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/LinkToTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

namespace Trk {
LinkToTrack::LinkToTrack()
  : ElementLink<TrackCollection>()
{}

LinkToTrack::LinkToTrack(ElementLink<TrackCollection>& link)
  : ElementLink<TrackCollection>(link)
{}

const TrackParameters*
LinkToTrack::parameters() const
{
  if (isValid()) {
    const Trk::Track* trk = this->cachedElement();
    if (nullptr != trk) {
      return trk->perigeeParameters();
    }
    return nullptr;
  }
  return nullptr;
} // end of parameters method

Trk::LinkToTrack*
Trk::LinkToTrack::clone() const
{
  return new LinkToTrack(*this);
}

} // end of namespace definitions
