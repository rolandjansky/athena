/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPTrackRecoAlgID.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-10-07
 * 
 * @brief  Definitions of identification numbers of track reconstruction algorithms
 * 
 */

#ifndef XAODFORWARD_AFPTRACKRECOALGID_H
#define XAODFORWARD_AFPTRACKRECOALGID_H

namespace xAOD
{
  
/**
 * @brief Helper class to store ID numbers of track reconstruction algorithms.
 *
 * This class contains public static const variables which can be used
 * instead of raw numbers for refering to algorithms used for
 * track reconstruction. Enum is not used to allow adding new numbers
 * without the need of modifying the header file.
 *
 * An example of usage
 * @code{.cpp}
 * xAOD::AFPTrack* track;
 * if (track->algID() == xAOD::AFPTrackRecoAlgID::basicKalman) {
 * // do something if basic kalman algorithm is used
 * }
 * @endcode
 * 
 * Algorithms indexing follows the scheme
 *
 * | ID  | Algorithm                                                  | Comments |
 * | :-: | ---------------------------------------------------------- | -------- |
 * | 0   | Basic Kalman algorithm (AFP_LocReco/AFPSiDBasicKalmanTool) |          |
 * | 1   | Linear Regression algorithm (AFP_LocRecoAFPSiDLinRegTool)  |          |
 * 
 */
  class AFPTrackRecoAlgID
  {
  public:
    static constexpr int basicKalman{0}; ///< basic Kalman algorithm id=0
    static constexpr int linReg{1}; ///< linear regression algorithm id=1
  };

}
#endif // XAODFORWARD_AFPTRACKRECOALGID_H

