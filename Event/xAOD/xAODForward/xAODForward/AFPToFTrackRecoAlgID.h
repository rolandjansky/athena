/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPToFTrackRecoAlgID.h
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-11-01
 * 
 * @brief  Definitions of identification numbers of track reconstruction algorithms
 * 
 */

#ifndef XAODFORWARD_AFPTOFTRACKRECOALGID_H
#define XAODFORWARD_AFPTOFTRACKRECOALGID_H

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
 * xAOD::AFPToFTrack* track;
 * if (track->algID() == xAOD::AFPToFTrackRecoAlgID::basic) {
 * // do something if basic algorithm is used
 * }
 * @endcode
 * 
 * Algorithms indexing follows the scheme
 *
 * | ID  | Algorithm                                                  | Comments |
 * | :-: | ---------------------------------------------------------- | -------- |
 * | 0   | Basic algorithm (AFP_LocReco/AFPTDBasicTool)               |          |
 * 
 */
  class AFPToFTrackRecoAlgID
  {
  public:
    static const int basic; ///< basic  algorithm id=0
  };

}
#endif // XAODFORWARD_AFPTOFTRACKRECOALGID_H

