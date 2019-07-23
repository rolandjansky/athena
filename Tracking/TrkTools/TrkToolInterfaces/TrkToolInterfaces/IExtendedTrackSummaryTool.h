/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/



#ifndef TRKIEXTENDEDTRACKSUMMARYTOOL_H
#define TRKIEXTENDEDTRACKSUMMARYTOOL_H

// @TODO remove once interface without PRDtoTrackMap argument is retired.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include "ITrackSummaryTool.h"

namespace Trk {

class PRDtoTrackMap;

static const InterfaceID IID_IExtendedTrackSummaryTool("Trk::IExtendedTrackSummaryTool", 1, 0);

/** @class IExtendedTrackSummaryTool
    @brief Interface for condensing Trk::Track properties and associated
           hits to a (non-fittable) foot print, the Trk::TrackSummary object.

    @author Edward Moyse, Martin Siebel <http://consult.cern.ch/xwho>
*/
class IExtendedTrackSummaryTool : virtual public ITrackSummaryTool {
  public:
  static const InterfaceID& interfaceID( ) ;

  /** create a summary object from passed Track. The summary object belongs to
      you, the user, and so you must take care of deletion of it.*/
  virtual const TrackSummary* createSummary ATLAS_NOT_THREAD_SAFE ( const Track& track,
                                                                    const Trk::PRDtoTrackMap *prd_to_track_map,
                                                                    bool onlyUpdateTrack=false ) const = 0;

  /** create a summary object of passed track without doing the tedious hole search.
      Same comments as for createSummary apply here, of course, too. */
  virtual const Trk::TrackSummary* createSummaryNoHoleSearch ATLAS_NOT_THREAD_SAFE (const Track& track,
                                                                                    const Trk::PRDtoTrackMap *prd_to_track_map) const = 0;

  /** method which can be used to update the track and add a summary to it.
   *  this can be used to add a summary to a track and then retrieve it from it without the need to clone. */
  virtual void updateTrack(Track& track, const Trk::PRDtoTrackMap *prd_to_track_map) const = 0;

  /** method which can be used to update the track and add a summary to it,without doing the tedious hole search.
      This can be used to add a summary to a track and then retrieve it from it without the need to clone. */
  virtual void updateTrackNoHoleSearch(Track& track, const Trk::PRDtoTrackMap *prd_to_track_map) const = 0;

  /** method to update the shared hit content only, this is optimised for track collection merging. */
  virtual void updateSharedHitCount(Track& track, const Trk::PRDtoTrackMap *prd_to_track_map) const = 0;


  virtual void updateAdditionalInfo(Track& track, const Trk::PRDtoTrackMap *prd_to_track_map) const = 0;

};

inline const InterfaceID& Trk::IExtendedTrackSummaryTool::interfaceID()
{
        return IID_IExtendedTrackSummaryTool;
}


}
#pragma GCC diagnostic pop
#endif
