/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACK_LINKTOTRACK_H
#define TRKTRACK_LINKTOTRACK_H

#include "AthLinks/ElementLink.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackLink/ITrackLink.h"

namespace Trk {
/** @brief AUTO - An Undocumented Tracking Object */

class LinkToTrack
  : public ITrackLink
  , public ElementLink<TrackCollection>
{

public:
  /** default constructor (needed for persistency) */
  LinkToTrack();

  /** constructor with ElementLink */
  LinkToTrack(ElementLink<TrackCollection>& link);

  /** default destructor */
  virtual ~LinkToTrack() = default;

  /** return the track parameters of the track (to which the EL< TrackCollection
   * > points) */
  virtual const TrackParameters* parameters() const override final;

  /** return the neutral parameters of the NeutralParticle) */
  virtual const NeutralParameters* neutralParameters() const override final { return nullptr; }

  /** method to clone the LinkToTrack object */
  virtual LinkToTrack* clone() const override final;

  virtual ITrackLinkType type() const override final {
    return ToTrack;
  }

  // This is here to make sure that the TrackCollection typedef
  // will make it into the dictionary.
  typedef TrackCollection TrackCollectionForDict;
}; // end of class definitions
} // end of namespace definitions

#endif // TRKTRACK_LINKTOTRACK_H
