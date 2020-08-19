/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACK_LINKTOXAODTRACKPARTICLE_H
#define TRKTRACK_LINKTOXAODTRACKPARTICLE_H

#include "AthLinks/ElementLink.h"
#include "TrkTrackLink/ITrackLink.h"

#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Trk {
/** @brief Element link to XAOD TrackParticle.
 *  An EDM object to reduce dependencies among
 * Tracking/InDet/XAOD domains. This is NOT an
 * analysis class. This link keeps the EDM of
 * different domains together and makes it interchangeable.
 * WARNING: Do not use this class unless you understand
 * how the ITrackLink functions.
 *
 * A hack to the EDM which will be possibly removed at later stages
 * Kirill.Prokofiev@cern.ch
 *
 * February 2014
 */

class LinkToXAODTrackParticle
  : public ITrackLink
  , public ElementLink<xAOD::TrackParticleContainer>
{

public:
  /** default constructor (needed for persistency) */
  LinkToXAODTrackParticle();

  /** constructor with ElementLink */
  LinkToXAODTrackParticle(ElementLink<xAOD::TrackParticleContainer>& link);

  LinkToXAODTrackParticle(const LinkToXAODTrackParticle&) = default;
  LinkToXAODTrackParticle(LinkToXAODTrackParticle&&) = default;
  LinkToXAODTrackParticle& operator=(const LinkToXAODTrackParticle&) = default;
  LinkToXAODTrackParticle& operator=(LinkToXAODTrackParticle&&) = default;
  virtual ~LinkToXAODTrackParticle() = default;

  /** return the track parameters of the track (to which the EL< TrackCollection
   * > points) */
  virtual const TrackParameters* parameters() const override final;

  /** return the neutral parameters of the NeutralParticle) */
  virtual const NeutralParameters* neutralParameters() const override final
  {
    return nullptr;
  }

  /** method to clone the LinkToXAODTrackParticle object */
  virtual LinkToXAODTrackParticle* clone() const override final;

  virtual ITrackLinkType type() const override final
  {
    return ToxAODTrackParticle;
  }

}; // end of class definitions
} // end of namespace definitions

#endif // TRKTRACK_LINKTOXAODTRACKPARTICLE_H
