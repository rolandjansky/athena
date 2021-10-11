/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSlimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKTRACKSLIMMINGTOOL_H
#define TRKTRACKSLIMMINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "TrkTrack/Track.h"
#include <memory>
class AtlasDetectorID;
class Identifier;

namespace Trk {
class TrackStateOnSurface;

/** @class TrackSlimmingTool

  A tool to produce 'slimmed' Tracks from a reference track.

  By default, this slimmed track will include all the measurements, and the
  Perigee parameters (currently these are assumed to exist)

  @author  Edward Moyse <Edward.Moysecern.ch>
  @author  Christos Anastopoulos Athena MT modifications

*/
class TrackSlimmingTool
  : virtual public ITrackSlimmingTool
  , public AthAlgTool
{
public:
  TrackSlimmingTool(const std::string&, const std::string&, const IInterface*);

  /** default destructor */
  virtual ~TrackSlimmingTool();

  /** standard Athena-Algorithm method */
  virtual StatusCode initialize() override;

  /** standard Athena-Algorithm method */
  virtual StatusCode finalize() override;

  /**This method 'skims' interesting information from the passed track.
   * @param track A const reference to the track to be skimmed
   * For compatibility reasons in can do two different things
   * depending on the value of m_setPersistificationHints.
   *
   * When m_setPersistificationHints = False
   * @return A 'slimmed' version of 'track'.
   * This is equivalent to calling the slimCopy below
   *
   * When m_setPersistificationHints = True
   * it sets persistification hints
   * @return nullptr
   * The later behaviour can be nom thread-safe as it
   * modifies the const TrackStateOnSurfaces attached
   * to the const Trk::Track reference.
   *
   * If you need MT safety 
   * A. If the Trk::Track is not const consider the slimTrack method
   * B. If it is const consider using slimCopy
   */
  Trk::Track* slim
  ATLAS_NOT_THREAD_SAFE(const Trk::Track& track) const override final;

  /**
   * This method always creates a std::unique_ptr<Trk::Track> with information
   * removed
   * @param track A const reference to the track to be skimmed. It will not be
   * modified in any way.
   * @return A 'slimmed' version of 'track', where exactly what information is
   * copied depends on how the tool is configured
   */
  std::unique_ptr<Trk::Track> slimCopy(
    const Trk::Track& track) const override final;

  /**
   * Slim/skim a non const Track. (m_setPersistificationHints is not used)
   * @param track A reference to the track to be skimmed.
   * It will be modified.
   */
  void slimTrack(Trk::Track& track) const override final;

private:
  /** any CaloDeposit with its adjacent MEOT's will be kept on the slimmed track
   * (combined muon property) */
  bool m_keepCaloDeposit;

  /** If true, Outliers will be kept on the slimmed track*/
  bool m_keepOutliers;

  /** If true, the first and last parameters of ID & MS subsystems will be kept
   * on the slimmed track*/
  bool m_keepParameters;

  /** Do not create slimmed tracks but only set hints for the persistifier to
   * drop information while writing.
   */
  bool m_setPersistificationHints;

  /**atlas id helper*/
  const AtlasDetectorID* m_detID;

  void checkForValidMeas(const Trk::TrackStateOnSurface* tsos,
                         bool& isIDmeas,
                         bool& isMSmeas) const;

  void findLastValidTSoS(
    const DataVector<const Trk::TrackStateOnSurface>* oldTrackStates,
    const Trk::TrackStateOnSurface*& lastValidIDTSOS,
    const TrackStateOnSurface*& lastValidMSTSOS) const;

  bool keepParameters(const Trk::TrackStateOnSurface* TSoS,
                      const TrackStateOnSurface*& firstValidIDTSOS,
                      const TrackStateOnSurface*& lastValidIDTSOS,
                      const TrackStateOnSurface*& firstValidMSTSOS,
                      const TrackStateOnSurface*& lastValidMSTSOS) const;
};
} // end of namespace

#endif
