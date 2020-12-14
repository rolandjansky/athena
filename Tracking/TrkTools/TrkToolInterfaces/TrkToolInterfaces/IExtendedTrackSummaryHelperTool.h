/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEXTENDEDTRACKSUMMARYHELPERTOOL
#define IEXTENDEDTRACKSUMMARYHELPERTOOL

#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <bitset>


class Identifier;

namespace Trk {

  class PRDtoTrackMap;

  static const InterfaceID IID_IExtendedTrackSummaryHelperTool("Trk::IExtendedTrackSummaryHelperTool", 1, 0);

/** @class ITrackSummaryHelperTool
  @brief Interface for structuring the summary creation into sub-detector
    specific tools.

    This tool is aimed to be implemented once in the Inner Detector and
    once in the Muon Spectrometer, allowing each implementation to access
    detector-specific information while a master tool can decide at run-time
    which implementation to call, depending on the type of hit on the track.

    @author Edward Moyse, Martin Siebel <http://consult.cern.ch/xwho>
*/

  class IExtendedTrackSummaryHelperTool : virtual public ITrackSummaryHelperTool
  {
  public:
    static const InterfaceID& interfaceID();
    using ITrackSummaryHelperTool::addDetailedTrackSummary;
    using ITrackSummaryHelperTool::analyse;

    /* Expand/Extend the interface , with methods  using the EventContext
     * and Trk::PRDtoTrackMap.
     */

     /*
     * For now due to client compatibility 
     * we provide a default  implementations
     * in terms of the the older interface
     */

    virtual void analyse(
      const EventContext& ctx,
      const Trk::Track& track,
      const Trk::PRDtoTrackMap* prd_to_track_map,
      const RIO_OnTrack* rot,
      const TrackStateOnSurface* tsos,
      std::vector<int>& information,
      std::bitset<Trk::numberOfDetectorTypes>& hitPattern) const
    {
      (void)ctx;
      (void) prd_to_track_map;
      analyse(track, rot, tsos, information, hitPattern);
    };

    virtual void analyse(
      const EventContext& ctx,
      const Trk::Track& track,
      const Trk::PRDtoTrackMap* prd_to_track_map,
      const CompetingRIOsOnTrack* crot,
      const TrackStateOnSurface* tsos,
      std::vector<int>& information,
      std::bitset<Trk::numberOfDetectorTypes>& hitPattern) const
    {
      (void)ctx;
      (void)prd_to_track_map;
      analyse(track, crot, tsos, information, hitPattern);
    }

    virtual void addDetailedTrackSummary(const EventContext& ctx,
                                         const Track& track,
                                         Trk::TrackSummary& summary) const
    {
      (void)ctx;
      addDetailedTrackSummary(track,summary);
    };

    virtual void updateExpectedHitInfo(const EventContext& ctx, 
                                       const Trk::Track& track,
                                       Trk::TrackSummary& summary) const{

      (void)ctx;
      updateExpectedHitInfo(track,summary);
    }
 
    virtual void updateSharedHitCount(
      const Trk::Track&,
      const Trk::PRDtoTrackMap*,
      Trk::TrackSummary&) const {};

   virtual void updateAdditionalInfo(Trk::TrackSummary&,
                                      std::vector<float>&,
                                      float&,
                                      int&,
                                      int&) const {};

    /*
     * Implement the ITrackSummaryHelperTool part
     * of the interface for  the methods with the same
     * name as the method above.
     */
    virtual void analyse(
      const Trk::Track& track,
      const RIO_OnTrack* rot,
      const TrackStateOnSurface* tsos,
      std::vector<int>& information,
      std::bitset<Trk::numberOfDetectorTypes>& hitPattern) const override
    {
      analyse(Gaudi::Hive::currentContext(),
              track,
              nullptr,
              rot,
              tsos,
              information,
              hitPattern);
    }

    virtual void analyse(
      const Trk::Track& track,
      const CompetingRIOsOnTrack* crot,
      const TrackStateOnSurface* tsos,
      std::vector<int>& information,
      std::bitset<Trk::numberOfDetectorTypes>& hitPattern) const override
    {
      analyse(Gaudi::Hive::currentContext(),
              track,
              nullptr,
              crot,
              tsos,
              information,
              hitPattern);
    }

    virtual void addDetailedTrackSummary(
      const Track& track,
      Trk::TrackSummary& summary) const override
    {
      addDetailedTrackSummary(Gaudi::Hive::currentContext(), track, summary);
    }
  
     virtual void updateExpectedHitInfo(const Trk::Track& track,
                                       Trk::TrackSummary& summary) const override{

      updateExpectedHitInfo(Gaudi::Hive::currentContext(),track,summary);
    }
  

  };

  inline const InterfaceID& Trk::IExtendedTrackSummaryHelperTool::interfaceID()
  {
    return IID_ITrackSummaryHelperTool;
  }

}
#endif
