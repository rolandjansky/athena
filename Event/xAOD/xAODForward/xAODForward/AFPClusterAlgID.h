/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPClusterAlgID.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-14
 * 
 * @brief  Definitions of identification numbers of pixel clustering algorithms
 * 
 */

#ifndef XAODFORWARD_AFPCLUSTERALGID_H
#define XAODFORWARD_AFPCLUSTERALGID_H

namespace xAOD
{
  
/**
 * @brief Helper class to store ID numbers of pixel clustering algorithm.
 *
 * This class contains public static const variables which can be used
 * instead of raw numbers for refering to algorithms used for
 * clustering pixels. Enum is not used to allow adding new numbers
 * without the need of modifying the header file.
 *
 * An example of usage
 * @code{.cpp}
 * xAOD::AFPSiHitsCluter* cluster;
 * if (cluster->clusterAlgID() == xAOD::AFPClusterAlgID::singleHit) {
 * // do something if clustering single hit algorithm was used
 * }
 * @endcode
 * 
 * Algorithms indexing follows the scheme
 *
 * | Station ID | variable name    | Algorithm Name      | Comment |
 * | :--------: | ---------------- | ------------------- | ---------------- |
 * |    0       | singleHit        | Single hit          | No clustering - each hit creates a separate cluster |
 * |    1       | nearestNeighbour | Nearest neighbour   | Pairs of adjacent pixels are combined in a cluster |
 * 
 */
  class AFPClusterAlgID
  {
  public:
    static const int singleHit; ///< Single hit algorithm
    static const int nearestNeighbour; ///< Nearest neighbour algorithm
  };

}
#endif // XAODFORWARD_AFPCLUSTERALGID_H

