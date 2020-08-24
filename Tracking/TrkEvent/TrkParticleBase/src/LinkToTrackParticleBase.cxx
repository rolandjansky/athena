/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBase.h"

namespace Trk {

LinkToTrackParticleBase::LinkToTrackParticleBase()
  : ElementLink<TrackParticleBaseCollection>()
{}

LinkToTrackParticleBase::LinkToTrackParticleBase(
  ElementLink<TrackParticleBaseCollection>& link)
  : ElementLink<TrackParticleBaseCollection>(link)
{}

const TrackParameters*
LinkToTrackParticleBase::parameters() const
{
  if (isValid()) {
    const Trk::TrackParticleBase* trk = this->cachedElement();
    if (nullptr != trk) {
      return &(trk->definingParameters());
    }
    return nullptr;
  } // end of validity check
  return nullptr;
} // end of parameters method

Trk::LinkToTrackParticleBase*
Trk::LinkToTrackParticleBase::clone() const
{
  return new LinkToTrackParticleBase(*this);
}

} // end of namespace definitions
