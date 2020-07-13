/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKPARTICLEBASE_LINKTOTRACKPARTICLEBASE_H
#define TRKPARTICLEBASE_LINKTOTRACKPARTICLEBASE_H

#include "AthLinks/ElementLink.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkTrackLink/ITrackLink.h"

namespace Trk {

class LinkToTrackParticleBase
  : public ITrackLink
  , public ElementLink<TrackParticleBaseCollection>
{

public:
  /** default constructor (needed for persistency) */
  LinkToTrackParticleBase();

  /** constructor with ElementLink */
  LinkToTrackParticleBase(ElementLink<TrackParticleBaseCollection>& link);

  /** default destructor */
  virtual ~LinkToTrackParticleBase() = default;

  /** return the track parameters of the track (to which the EL<
   * TrackParticleBaseCollection > points) */
  virtual const TrackParameters* parameters() const override final;

  /** return the neutral parameters of the NeutralParticle) */
  virtual const NeutralParameters* neutralParameters() const override final { return nullptr; }

  /** method to clone the LinkToTrack object */
  virtual LinkToTrackParticleBase* clone() const override final;

  virtual ITrackLinkType type() const override final {
    return ToTrackParticleBase;
  }
}; // end of class definitions
} // end of namespace definitions

#endif // LINKTOTRACKPARTICLEBASE_H
