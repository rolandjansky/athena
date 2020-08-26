/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACKLINK_ITRACKLINK_H
#define TRKTRACKLINK_ITRACKLINK_H

/**
 * @class Trk::ITrackLink
 * An abstract class which is meant
 * to represent an element link to
 * the Trk::Track or Trk::TrackParticleBase
 *
 *  The use of abstract class is required to
 *  break the TrackParticleBase<-->VxVertex
 *  circular dependence.
 *
 *  Kirill Prokofiev, June 2007
 */

#include "TrkNeutralParameters/NeutralParameters.h" // no forward declare for now because needed in cast
#include "TrkParameters/TrackParameters.h" // no forward declare for now because needed in cast

namespace Trk {
class ITrackLink
{

public:
  enum ITrackLinkType
  {
    ToxAODTrackParticle = 0,
    ToxAODNeutralParticle = 1,
    ToTrack = 2,
    ToTrackParticleBase = 3
  };

  ITrackLink() = default;
  ITrackLink(const ITrackLink&) = default;
  ITrackLink(ITrackLink&&) = default;
  ITrackLink& operator=(const ITrackLink&) = default;
  ITrackLink& operator=(ITrackLink&&) = default;
  virtual ~ITrackLink() = default;

  /** clone methods for copy constructors */
  virtual ITrackLink* clone() const = 0;

  /** return the track parameters of the track or TrackParticleBase) */
  virtual const TrackParameters* parameters() const = 0;

  /** return the neutral parameters of the NeutralParticle) */
  virtual const NeutralParameters* neutralParameters() const = 0;

  /** return the type */
  virtual ITrackLinkType type() const = 0;

}; // end of class definitions

} // end of namespace definitions

#endif // TRKTRACKLINK_ITRACKLINK_H
