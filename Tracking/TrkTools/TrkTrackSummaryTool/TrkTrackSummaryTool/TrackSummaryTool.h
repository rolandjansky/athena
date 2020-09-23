/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACKSUMMARYTOOL_H
#define TRKTRACKSUMMARYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "TRT_ElectronPidTools/ITRT_ToT_dEdx.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryHelperTool.h"
#include "TrkToolInterfaces/IPixelToTPIDTool.h"
#include "TrkToolInterfaces/ITRT_ElectronPidTool.h"

#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include <vector>

class AtlasDetectorID;
class Identifier;
class ITRT_ToT_dEdx;

namespace Trk {
class ITRT_ElectronPidTool;
class ITrackHoleSearchTool;
class IPixelToTPIDTool;

/** @enum flag the methods for the probability calculation */

enum TRT_ElectronPidProbability
{
  Combined = 0,
  HighThreshold = 1,
  TimeOverThreshold = 2,
  Bremsstrahlung = 3
};

class IExtendedTrackSummaryHelperTool;
class RIO_OnTrack;
class TrackStateOnSurface;
class MeasurementBase;

class TrackSummaryTool : public extends<AthAlgTool, IExtendedTrackSummaryTool>
{

public:
  TrackSummaryTool(const std::string&, const std::string&, const IInterface*);
  virtual ~TrackSummaryTool();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

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
    Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map,
    bool suppress_hole_search = false) const override;

  /** Same behavious as
   * IExtendedTrackSummaryTool:computeAndReplaceTrackSummary
   * but without the need to pass
   * Trk::PRDtoTrackMap
   * Does hole search
   */
  virtual void updateTrack(Track& track) const override
  {
   computeAndReplaceTrackSummary(track,nullptr,false /*DO NOT suppress hole search*/);
  }
  
  /* Start from a copy of the existing input track summary if there,
   * otherwise start from a new one. Fill it and return it.
   * Does not modify the const track.
   */
  virtual std::unique_ptr<Trk::TrackSummary> summary(
    const Track& track) const override;

  /* Start from a copy of the existing input track summary if there,
   * otherwise start from a new one. Fill it and return it.
   * but without doing the hole search.
   * Does not modify the const track.
   */
  virtual std::unique_ptr<Trk::TrackSummary> summaryNoHoleSearch(
    const Track& track) const override;

  /* Start from a copy of the existing input track summary if there,
   * otherwise start from a new one. Fill it and return it.
   * Does not modify the const track.
   */
  virtual std::unique_ptr<Trk::TrackSummary> summary(
    const Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map) const override;
  
  /* Start from a copy of the existing input track summary if there,
   * otherwise start from a new one. Fill it and return it.
   * but without doing the hole search.
   * Does not modify the const track.
   */
  virtual std::unique_ptr<Trk::TrackSummary> summaryNoHoleSearch(
    const Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map) const override;

  /** method which can be used to update the summary of a track
   * it, without doing shared hit/ or hole search. 
   * If a summary is present it is  modified in place.
   * Otherwise a new one is created and filled.
   */
  virtual void updateTrackSummary(Track& track) const override
  {
    /*suppress hole search*/
    UpdateSummary(track, nullptr, true);
    /*Needed for expected B-Layer*/
    m_idTool->updateExpectedHitInfo(track, *track.trackSummary());
  }

 
  /** method to update the shared hit content only, this is optimised for track
   * collection merging. */
  virtual void updateSharedHitCount(
    Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map) const override
  {
    if (!track.trackSummary()) {
      computeAndReplaceTrackSummary(
        track, prd_to_track_map, false /*DO NOT suppress hole search*/);
    } else {
      updateSharedHitCount(track, prd_to_track_map, *track.trackSummary());
    }
  }

  /** method to update additional information (PID,shared hits, dEdX), this is
   * optimised for track collection merging.
   */
  virtual void updateAdditionalInfo(
    Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map) const override
  {
    if (!track.trackSummary()) {
      computeAndReplaceTrackSummary(
        track, prd_to_track_map, false /*DO NOT suppress hole search*/);
    } else {
      updateAdditionalInfo(
        track,
        prd_to_track_map,
        *track.trackSummary(),
        true); // @TODO set to false; true for backward compatibility
    }
  }

  /** Update the shared hit count of the given track summary.
   * @param summary the summary to be updated i.e. a copy of the track summary
   * of the given track.
   * @param track the track which corresponds to the given track summary and is
   * used to compute the numbers of shared hits.
   * @param prd_to_track_map an optional PRD-to-track map which is used to
   * compute the shared hits otherwise the association tool will be used. The
   * method will update the shared information in the given summary. The track
   * will not be modified i.e. the shared hit count of the summary owned by the
   * track will not be updated.
   */
  virtual void updateSharedHitCount(const Track& track,
                                    const Trk::PRDtoTrackMap* prd_to_track_map,
                                    TrackSummary& summary) const override;

  /** method to update the shared hit content only, this is optimised for track
   * collection merging. */
  virtual void updateSharedHitCount(Track& track) const override
  {
    if (!track.trackSummary()) {
      computeAndReplaceTrackSummary(
        track, nullptr, false /*DO NOT suppress hole search*/);
    } else {
      updateSharedHitCount(track, nullptr, *track.trackSummary());
    }
  }

  /** Update the shared hit counts, expected hit, PID information and eventually
   * the detailed track summaries.
   * @param track the track corresponding to the summary which is updated, which
   * is not actively changed.
   * @param prd-to-track an optional  PRD-to-track map to compute shared hits or
   * nullptr.
   * @param summary the summary which is updated.
   * Will update the shared hit, expected hit, PID information and eventually
   * the detailed track summaries. If no PRD-to-track map is given the helper
   * tools will rely on a PRD association tool to privide the shared hit
   * information. The method will not update the track itself unless the given
   * summary is owned by the track.
   */
  virtual void updateAdditionalInfo(const Track& track,
                                    const Trk::PRDtoTrackMap* prd_to_track_map,
                                    TrackSummary& summary) const override
  {
    updateAdditionalInfo(track, prd_to_track_map, summary, false);
  }

  /** method to update additional information (PID,shared hits, dEdX), this is
   * optimised for track collection merging.
   */
  virtual void updateAdditionalInfo(Track& track) const override
  {
    if (!track.trackSummary()) {
      computeAndReplaceTrackSummary(
        track, nullptr, false /*DO NOT suppress hole search*/);
    } else {
      updateAdditionalInfo(
        track,
        nullptr,
        *track.trackSummary(),
        true); // @TODO set to false; true for backward compatibility
    }
  }

private:

  /*
   * Fill the summary info for a Track*/
  void fillSummary(Trk::TrackSummary& ts,
                   const Trk::Track& track,
                   const Trk::PRDtoTrackMap *prd_to_track_map,
                   bool doHolesInDet,
                   bool doHolesMuon) const;

  /*
   * If a summary is there Update it with the required info.
   * If not there create a new one with the required info.
   */
  void UpdateSummary(Track& track,
                     const Trk::PRDtoTrackMap* prd_to_track_map,
                     bool suppress_hole_search) const
  {
    if (!track.trackSummary()) {
      track.setTrackSummary(std::make_unique<Trk::TrackSummary>());
    }
    fillSummary(*(track.trackSummary()),
                track,
                prd_to_track_map,
                m_doHolesInDet && !suppress_hole_search,
                m_doHolesMuon && !suppress_hole_search);
  }

  void updateAdditionalInfo(const Track& track,
                            const Trk::PRDtoTrackMap* prd_to_track_map,
                            TrackSummary& summary,
                            bool initialise_to_zero) const;

  /** use this method to update a track. this means a tracksummary is created
  for this track but not returned. the summary can then be obtained from the
  track. Because it is taken from the track the ownership stays with the track
  */
  void updateTrack(Track& track,
                   const Trk::PRDtoTrackMap* prd_to_track_map) const;

  std::unique_ptr<Trk::TrackSummary> createSummary(
    const Track& track,
    const Trk::PRDtoTrackMap* prd_to_track_map,
    bool doHolesInDet,
    bool doHolesMuon) const;

  /** Return the correct tool, matching the passed Identifier*/
  Trk::IExtendedTrackSummaryHelperTool* getTool(const Identifier& id);
  const Trk::IExtendedTrackSummaryHelperTool* getTool(
    const Identifier& id) const;

  /**tool to decipher ID RoTs*/
  ToolHandle<IExtendedTrackSummaryHelperTool>
    m_idTool{ this, "InDetSummaryHelperTool", "", "" };
  /**tool to calculate electron probabilities*/
  ToolHandle<ITRT_ElectronPidTool> m_eProbabilityTool{ this,
                                                       "TRT_ElectronPidTool",
                                                       "",
                                                       "" };
  /** tool to calculate the TRT_ToT_dEdx.*/
  ToolHandle<ITRT_ToT_dEdx> m_trt_dEdxTool{ this, "TRT_ToT_dEdxTool", "", "" };
  /**tool to calculate dE/dx using pixel clusters*/
  ToolHandle<IPixelToTPIDTool> m_dedxtool{ this, "PixelToTPIDTool", "", "" };
  /**tool to decipher muon RoTs*/
  ToolHandle<IExtendedTrackSummaryHelperTool>
    m_muonTool{ this, "MuonSummaryHelperTool", "", "" };

  /** controls whether holes on track are produced
      Turning this on will (slightly) increase processing time.*/
  Gaudi::Property<bool> m_doHolesMuon{ this, "doHolesMuon", false, "" };
  /** For the InDet it is switched on automatically as soon as the
   * HoleSearchTool is given */
  Gaudi::Property<bool> m_doHolesInDet{ this, "doHolesInDet", false, "" };
  /** controls whether shared hits in Pix+SCT are produced
     Turning this on will increase processing time.*/
  Gaudi::Property<bool> m_doSharedHits{ this, "doSharedHits", false, "" };

  /** controls whether the detailed summary is added for the indet */
  Gaudi::Property<bool> m_addInDetDetailedSummary{ this,
                                                   "AddDetailedInDetSummary",
                                                   true,
                                                   "" };
  /** controls whether the detailed summary is added for the muons */
  Gaudi::Property<bool> m_addMuonDetailedSummary{ this,
                                                  "AddDetailedMuonSummary",
                                                  true,
                                                  "" };
  /** switch to deactivate Pixel info init */
  Gaudi::Property<bool> m_pixelExists{ this, "PixelExists", true, "" };

  /** Only compute TRT dE/dx if there are at least this number of TRT hits or
   * outliers.*/
  Gaudi::Property<int> m_minTRThitsForTRTdEdx{ this,
                                               "minTRThitsForTRTdEdx",
                                               1,
                                               "" };

  /**atlas id helper*/
  const AtlasDetectorID* m_detID;

  /**loops over TrackStatesOnSurface and uses this to produce the summary
     information Fills 'information', 'eProbability', and 'hitPattern'*/
  void processTrackStates(const Track& track,
                          const Trk::PRDtoTrackMap* prd_to_track_map,
                          const DataVector<const TrackStateOnSurface>* tsos,
                          std::vector<int>& information,
                          std::bitset<numberOfDetectorTypes>& hitPattern,
                          bool doHolesInDet,
                          bool doHolesMuon) const;

  void processMeasurement(const Track& track,
                          const Trk::PRDtoTrackMap* prd_to_track_map,
                          const Trk::MeasurementBase* meas,
                          const Trk::TrackStateOnSurface* tsos,
                          std::vector<int>& information,
                          std::bitset<numberOfDetectorTypes>& hitPattern) const;

  /** Extrapolation is performed from one hit to the next, it is checked if
     surfaces in between the extrapolation are left out. The trackParameters of
     the destination hit (instead of the trackParameters of the extrapolation
     step) are then used as starting point for the next extrapolation step. */
  void searchHolesStepWise(const Trk::Track& track,
                           std::vector<int>& information,
                           bool doHolesInDet,
                           bool doHolesMuon) const;
};

}
#endif
