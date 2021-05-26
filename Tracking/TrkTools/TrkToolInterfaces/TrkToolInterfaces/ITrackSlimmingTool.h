/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackSlimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKSLIMMINGTOOL_H
#define ITRKTRACKSLIMMINGTOOL_H

#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/IAlgTool.h"

namespace Trk

{
class Track;

static const InterfaceID IID_ITrackSlimmingTool("Trk::ITrackSlimmingTool",
                                                1,
                                                0);

/** @brief Interface for constructing 'slimmed' Tracks from complete tracks.

    @author edward.moyse@cern.ch
*/
class ITrackSlimmingTool : virtual public IAlgTool
{
public:
  static const InterfaceID& interfaceID();
  /**This method 'skims' interesting information from the passed track.
   * @param track A const reference to the track to be skimmed
   *
   * For compatibility reasons in can do two different things
   * depending on so called setPersistificationHints.
   *
   * When setPersistificationHints is not to be used
   * @return A 'slimmed' version of 'track'.
   * This is equivalent to calling the slimCopy below
   *
   * When asetPersistificationHints = True
   * it sets persistification hints
   * @return nullptr
   * The later behaviour can be not thread-safe as it
   * modifies the const TrackStateOnSurfaces attached
   * to the Trk::Track.
   */
  virtual Trk::Track* slim
  ATLAS_NOT_THREAD_SAFE(const Trk::Track& track) const = 0;
  /**
   * This method always creates a std::unique_ptr<Trk::Track*> with information
   * removed
   * @param track A const reference to the track to be skimmed. It will not be
   * modified in any way.
   * @return A 'slimmed' version of 'track', where exactly what information is
   * copied depends on how the tool is configured
   */
  virtual std::unique_ptr<Trk::Track> slimCopy(
    const Trk::Track& track) const = 0;
  /**
   * Slim/skim a non const Track.
   * @param track A reference to the track to be skimmed.
   * It will be modified.
   */
  virtual void slimTrack(Trk::Track& track) const = 0;
};

inline const InterfaceID&
Trk::ITrackSlimmingTool::interfaceID()
{
  return IID_ITrackSlimmingTool;
}

} // end of namespace

#endif
