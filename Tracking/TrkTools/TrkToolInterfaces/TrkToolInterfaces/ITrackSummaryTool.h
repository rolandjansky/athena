/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKITRACKSUMMARYTOOL_H
#define TRKITRACKSUMMARYTOOL_H

#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/IAlgTool.h"

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

  /* Start from a copy of the existing input track summary if there,
   * otherwise start from a new one. Fill it and return it.
   * Does not modify the const track.
   */
  virtual std::unique_ptr<Trk::TrackSummary> summary(
    const Track& track) const = 0;
  
  /* Start from a copy of the existing input track summary if there,
   * otherwise start from a new one. Fill it and return it.
   * but without doing the hole search.
   * Does not modify the const track.
   */
  virtual std::unique_ptr<Trk::TrackSummary> summaryNoHoleSearch(
    const Track& track) const = 0;

  /** Same behavious as
   * IExtendedTrackSummaryTool:computeAndReplaceTrackSummary
   * but without the need to pass
   * Trk::PRDtoTrackMap
   * Does hole search
   */
  virtual void updateTrack(Track& track) const = 0;

  /** method which can be used to update the summary of a track
   * it, without doing shared hit/ or hole search. 
   * If a summary is present is modified in place
   * otherwise a new one is created.
   */
  virtual void updateTrackSummary(Track& track) const = 0;

  /** method to update the shared hit content only, this is optimised for track
   * collection merging. */
  virtual void updateSharedHitCount(Track& track) const = 0;

  /** method to update additional information (PID,shared hits, dEdX), this is
   * optimised for track collection merging. */
  virtual void updateAdditionalInfo(Track& track) const = 0;
};

inline const InterfaceID&
Trk::ITrackSummaryTool::interfaceID()
{
  return IID_ITrackSummaryTool;
}

}
#endif
