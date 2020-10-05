/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACK_LINKTOXAODNEUTRALPARTICLE_H
#define TRKTRACK_LINKTOXAODNEUTRALPARTICLE_H

#include "AthLinks/ElementLink.h"
#include "TrkTrackLink/ITrackLink.h"

#include "TrkNeutralParameters/NeutralParameters.h"
#include "xAODTracking/NeutralParticleContainer.h"

namespace Trk {
/** @brief Element link to XAOD NeutralParticle.
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

class LinkToXAODNeutralParticle
  : public ITrackLink
  , public ElementLink<xAOD::NeutralParticleContainer>
{

public:
  /** default constructor (needed for persistency) */
  LinkToXAODNeutralParticle();

  /** constructor with ElementLink */
  LinkToXAODNeutralParticle(ElementLink<xAOD::NeutralParticleContainer>& link);

  LinkToXAODNeutralParticle(const LinkToXAODNeutralParticle&) = default;
  LinkToXAODNeutralParticle(LinkToXAODNeutralParticle&&) = default;
  LinkToXAODNeutralParticle& operator=(const LinkToXAODNeutralParticle&) = default;
  LinkToXAODNeutralParticle& operator=(LinkToXAODNeutralParticle&&) = default;
  virtual ~LinkToXAODNeutralParticle() = default;

  /** dummy function to return 0 if TrackParameters are asked for */
  const TrackParameters* parameters() const override final { return nullptr; }

  /** return the neutral parameters of the neutral (to which the EL<
   * NeutralCollection > points) */
  const NeutralParameters* neutralParameters() const override final;

  /** method to clone the LinkToXAODNeutralParticle object */
  LinkToXAODNeutralParticle* clone() const override final;

  virtual ITrackLinkType type() const override final
  {
    return ToxAODNeutralParticle;
  }
}; // end of class definitions
} // end of namespace definitions

#endif // TRKTRACK_LINKTOXAODNEUTRALPARTICLE_H
