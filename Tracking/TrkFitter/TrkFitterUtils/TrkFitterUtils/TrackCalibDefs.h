/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackCalibDefs.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKFITTERUTILS_TRACKCALIBDEFS_H
#define TRKFITTERUTILS_TRACKCALIBDEFS_H

namespace Trk {
  namespace TrackState {

    /** @enum CalibrationType
        @brief enum reflecting knowledge what kind of RIO_OnTrackCreation
               was used, and if re-tries to use more/less accurate hits
               should be made. This enum is still too much in development
               to deploy it for the primitives. Plan to do so once the
               ROT creator interfaces are enhanced.

        States 0 and 1 are meant such that tools can try to find a better calibration.
        State  2 says that calibration should not be changed except tubes made from DC
        State  3 says that position/error are highly tuned and can be reverted to
                 Nominal or broad in case of incompatibilities. 

        @author W. Liebig */

    enum CalibrationType {
      CalibrationNotKnown = 0, //!< status which will trigger tools to find out
      Uncalibrated        = 1, //!< known that calibration has not been tried
      TubeHit             = 1, //!< drift circles: ignores drift time, tube center+large error
      BroadCluster        = 1, //!< clusters: combination of channels treated with large error
      Nominal             = 2, //!< uses intrinsic errors
      HighPrecision       = 3, //!< combination of channels or TOF used for aggressive error
      NumberOfCalibrationTypes = 4
    };


  }
}
#endif
