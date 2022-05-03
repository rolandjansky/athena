/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RecToolInterfaces/IHGTD_TrackTimeExtensionTool.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @author
 * @date August, 2021
 * @brief Tracks reconstructed in ITk get extended to the surfaces of HGTD,
 * where compatible hits get selected. In the extension process, the track
 * is not altered. Currently the information from the selected hits in HGTD
 * is retrieved and decorations added to the track. After this, the newly
 * found track states are deleted.
 * This might be changed in the future, where a dedicated object is written out,
 * and a connection (link) between a track and its extension to HGTD stored.
 *
 */

#ifndef IHGTD_TRACKTIMEEXTENSIONTOOL_H
#define IHGTD_TRACKTIMEEXTENSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "HGTD_RecToolInterfaces/IHGTD_ClusterTruthTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "xAODTracking/TrackParticle.h"
#include "AtlasHepMC/GenEvent.h"
#include <memory>

namespace HGTD {

struct ExtensionObject {
  // track states after extrapolation, can be of hole type
  std::array<std::unique_ptr<const Trk::TrackStateOnSurface>, 4> m_hits;
  // truth info about primary hits left by the truth particles
  // is kept independent of them being found during the track extension
  std::array<const HGTD_Cluster*, 4> m_truth_primary_hits;
  std::array<HGTD::ClusterTruthInfo, 4> m_truth_primary_info;
  // point of extrapolation on HGTD layer closest to IP
  float m_extrap_x = 0;
  float m_extrap_y = 0;
};

} // namespace HGTD

class IHGTD_TrackTimeExtensionTool : virtual public IAlgTool {

public:
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IHGTD_TrackTimeExtensionTool, 1, 0);

  /**
   * @brief  Extends a track to (up to) 4 measurements in HGTD.
   *
   * @param [in] track Track built in the inner tracker to be extended to HGTD.
   * @param [in] container Container of HGTD_Cluster objects.
   *
   * @return Returns an object containing the 4 measurements in HGTD (can
   * contain holes or outliers) associated to the track as well as truth
   * information if available.
   */
  virtual HGTD::ExtensionObject
  extendTrackToHGTD(const EventContext& ctx,
                    const xAOD::TrackParticle& track,
                    const HGTD_ClusterContainer* container,
                    const HepMC::GenEvent* hs_event = nullptr,
                    const InDetSimDataCollection* sim_data = nullptr) = 0;
};

#endif // IHGTD_TRACKTIMEEXTENSIONTOOL_H
