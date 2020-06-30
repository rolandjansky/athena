/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKITRACKSUMMARYTOOL_H
#define TRKITRACKSUMMARYTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "CxxUtils/checker_macros.h"

#include <memory>
namespace Trk {

class Track;
class TrackSummary;

static const InterfaceID IID_ITrackSummaryTool("Trk::ITrackSummaryTool", 1, 0);

/** @class ITrackSummaryTool
    @brief Interface for condensing Trk::Track properties and associated
           hits to a (non-fittable) foot print, the Trk::TrackSummary object.

    @author Edward Moyse, Martin Siebel <http://consult.cern.ch/xwho>
*/
class ITrackSummaryTool : virtual public IAlgTool
{
public:
  static const InterfaceID& interfaceID();

  /** create a summary object from passed Track. The summary object belongs to
      you, the user, and so you must take care of deletion of it.
      If the track has a summary already a clone is returned back.
      @param onlyUpdateTrack If false (default) then the summary is cloned and
     added to the track, and a separate summary returned. If true, only update
     track and return nullptr */
  virtual const Trk::TrackSummary* createSummary
  ATLAS_NOT_THREAD_SAFE(const Track& track,
                        bool onlyUpdateTrack = false) const = 0;

  /** create a summary object of passed track without doing the tedious hole
     search. Same comments as for createSummary apply here, of course, too. */
  virtual const Trk::TrackSummary* createSummaryNoHoleSearch
  ATLAS_NOT_THREAD_SAFE(const Track& track) const = 0;

  /** create a summary object from a passed Track.
    If the track has a summary already a clone is returned back.
   */
  virtual std::unique_ptr<Trk::TrackSummary> summary(
    const Track& track) const = 0;

  /** create a summary object of passed track without doing the tedious hole
   search. If the track has a summary already a clone is returned back.*/
  virtual std::unique_ptr<Trk::TrackSummary> summaryNoHoleSearch(
    const Track& track) const = 0;

  /** method which can be used to update the track and add a summary to it.
      This can be used to add a summary to a track and then retrieve it from it
     without the need to clone. */
  virtual void updateTrack(Track& track) const = 0;

  /** method which can be used to update a refitted track and add a summary to
   * it, without doing shard hit/ or hole search. Adds a summary to a track and
   * then retrieve it from it without the need to clone. */
  virtual void updateRefittedTrack(Track& track) const = 0;

  /** method to update the shared hit content only, this is optimised for track
   * collection merging. */
  virtual void updateSharedHitCount(Track& track) const = 0;

  /** method to update additional information (PID,shared hits, dEdX), this is
   * optimised for track collection merging. */
  virtual void updateAdditionalInfo(Track& track) const = 0;
};

inline const InterfaceID& Trk::ITrackSummaryTool::interfaceID()
{ 
	return IID_ITrackSummaryTool; 
}


}
#endif 
