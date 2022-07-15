/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKIEXTENDEDTRACKSUMMARYTOOL_H
#define TRKIEXTENDEDTRACKSUMMARYTOOL_H

#include "ITrackSummaryTool.h"

namespace Trk {

class PRDtoTrackMap;

static const InterfaceID
  IID_IExtendedTrackSummaryTool("Trk::IExtendedTrackSummaryTool", 1, 0);

/** @class IExtendedTrackSummaryTool
    @brief Interface for condensing Trk::Track properties and associated
           hits to a (non-fittable) foot print, the Trk::TrackSummary object.

    @author Edward Moyse, Martin Siebel <http://consult.cern.ch/xwho>
*/
class IExtendedTrackSummaryTool : virtual public ITrackSummaryTool
{
public:
  static const InterfaceID& interfaceID();

  using ITrackSummaryTool::summary;
  using ITrackSummaryTool::summaryNoHoleSearch;
  using ITrackSummaryTool::updateAdditionalInfo;
  using ITrackSummaryTool::updateSharedHitCount;
  using ITrackSummaryTool::updateTrackSummary;

  /** Compute track summary and replace the summary in given track.
   * @param track the track whose track summary is replaced with a newly
   * computed one
   * @param prd_to_track_map a PRD to track association map to compute shared
   * hits or a nullptr
   * @param suppress_hole_search do not perform the hole search independent of
   * the settings of the ID and muon hole search properties. Will recompute the
   * track summary for the given track, delete the old track summary of the
   * track if there is already one and set the new one. If a valid PRD to track
   * map is given the number of shared hits is computed otherwise not. The hole
   * search is performed according to the settings of the ID and muon hole
   * search properties unless the suppress_hole_search argument is true.
   */
  virtual void computeAndReplaceTrackSummary(
    const EventContext& ctx,
    Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map,
    bool suppress_hole_search = false) const = 0;

  void computeAndReplaceTrackSummary(Track& track,
                                     const Trk::PRDtoTrackMap* prd_to_track_map,
                                     bool suppress_hole_search = false) const;

  /* Start from a copy of the existing input track summary if there,
   * otherwise start from a new one. Fill it and return it.
   * Does not modify the const track.
   */
  virtual std::unique_ptr<Trk::TrackSummary> summary(
    const EventContext& ctx,
    const Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map) const = 0;

  std::unique_ptr<Trk::TrackSummary> summary(
    const Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map) const;

  /** method which can be used to update the summary of a track.
   * If a summary is present is modified in place
   * otherwise a new one is created.
   */
  virtual void updateTrackSummary(const EventContext& ctx,
                                  Track& track,
                                  const Trk::PRDtoTrackMap* prd_to_track_map,
                                  bool suppress_hole_search = false) const = 0;

  void updateTrackSummary(Track& track,
                          const Trk::PRDtoTrackMap* prd_to_track_map,
                          bool suppress_hole_search = false) const;

  /* Start from a copy of the existing input track summary if there,
   * otherwise start from a new one. Fill it and return it.
   * but without doing the hole search.
   * Does not modify the const track.
   */
  virtual std::unique_ptr<Trk::TrackSummary> summaryNoHoleSearch(
    const EventContext& ctx,
    const Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map) const = 0;

  std::unique_ptr<Trk::TrackSummary> summaryNoHoleSearch(
    const Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map) const;

  /** Update the shared hit count of the given track summary.
   * @param track the track which corresponds to the given track summary and is
   * used to compute the numbers of shared hits.
   * @param prd_to_track_map an optional PRD-to-track map which is used to
   * compute the shared hits otherwise the association tool will be used.
   * @param summary the summary to be updated i.e. a copy of the track summary
   * of the given track. The method will update the shared information in the
   * given summary. The track will not be modified i.e. the shared hit count of
   * the summary owned by the track will not be updated.
   */
  virtual void updateSharedHitCount(const Track& track,
                                    const Trk::PRDtoTrackMap* prd_to_track_map,
                                    TrackSummary& summary) const = 0;

  /** method to update the shared hit content only, this is optimised for track
   * collection merging. */
  virtual void updateSharedHitCount(
    Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map) const = 0;

  virtual void updateAdditionalInfo(Track& track) const = 0;
};
}
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.icc"
#endif
