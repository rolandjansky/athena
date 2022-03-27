/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RecToolInterfaces/IHGTD_TOFcorrectionTool.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @author
 * @date August, 2021
 * @brief In order to compare the times associated to different tracks, the time
 *        measured in HGTD needs to be corrected for time of flight as well as
 *        for limitations to the detector intrinsic calibration. Uncertainties
 *        on impact parameters, the point of extrapolation etc. can also be pro-
 *        pagated to the time resolution of a hit.
 */

#ifndef IHGTD_TOFCORRECTIONTOOL_H
#define IHGTD_TOFCORRECTIONTOOL_H

#include "GaudiKernel/AlgTool.h"

namespace Trk {
class Track;
}

class HGTD_Cluster;

class IHGTD_TOFcorrectionTool : virtual public IAlgTool {

public:
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IHGTD_TOFcorrectionTool, 1, 0);

  /**
   * @brief   Corrects and calibrates the measured time of arrival (TOA) and its
   * resolution using information from the track. This includes e.g. time of
   * flight corrections necessary to compare tracks traveling into different
   * regions of HGTD.
   *
   * @param [in] track Track Needed to retrieve the impact parameter
   * @param [in] time_of_arrival TOA measured in HGTD.
   * @param [in] time_of_arrival_err Resolution of the TOA.
   *
   * @return Returns calibrated time of arrival and its resolution.
   */
  virtual std::pair<float, float> correctTimeAndResolution(
      const Trk::Track& track, const HGTD_Cluster& cluster,
      const float time_of_arrival, const float time_of_arrival_err) const = 0;
};

#endif // IHGTD_TOFCORRECTIONTOOL_H
