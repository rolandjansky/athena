/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
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
 * - FIXME: retrieveClusters should be removed from the interface and called
 * once at the start of the event in the derived tool!!!
 */

#ifndef IHGTD_TRACKTIMEEXTENSIONTOOL_H
#define IHGTD_TRACKTIMEEXTENSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include <memory>

class MsgStream;

namespace HGTD {

static const InterfaceID
    IID_IHGTD_TrackExtensionTool("HGTD::IHGTD_TrackTimeExtensionTool", 1, 0);

class IHGTD_TrackTimeExtensionTool : virtual public IAlgTool {

public:
  static const InterfaceID& interfaceID();

  /**
   * @brief  Extends a track to (up to) 4 measurements in HGTD.
   *
   * @param [in] track Track built in the inner tracker to be extended to HGTD.
   * @param [in] container Container of HGTD_Cluster objects.
   *
   * @return Returns 4 measurements in HGTD (can contain holes or outliers)
   * associated to the track.
   */
  virtual std::array<std::unique_ptr<const Trk::TrackStateOnSurface>, 4>
  extendTrackToHGTD(const Trk::Track& track,
                    const HGTD::HGTD_ClusterContainer* container) = 0;
};

/** Inline methods **/

inline const InterfaceID& IHGTD_TrackTimeExtensionTool::interfaceID() {
  return IID_IHGTD_TrackExtensionTool;
}

} // namespace HGTD

#endif // IHGTD_TRACKTIMEEXTENSIONTOOL_H
