/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPPixelLayerID.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-13
 * 
 * @brief  Definitions of silicon layers (tracking) identification numbers in an AFP station.
 * 
 */

#ifndef XAODFORWARD_AFPPIXELLAYERID_H
#define XAODFORWARD_AFPPIXELLAYERID_H

namespace xAOD
{

  
/**
 * @brief Helper class to store ID numbers of silicon layers (tracking) in an AFP station.
 *
 * This class contains public static const variables which can be used
 * instead of raw numbers for refering to silicon layers ID. Enum is not used
 * to allow adding new numbers without the need of modifying the
 * header file.
 *
 * An example of usage
 * @code{.cpp}
 * xAOD::AFPSiHit* hit;
 * if (hit->pixelLayerID() == xAOD::AFPPixelLayerID::firstFromIP) {
 * // do something with station at z = 217 m
 * }
 * @endcode
 *
 * The layer closest to the interaction point has index 0, the layer
 * furthest from the interaction point has index 3.
 *
 * <table>
 * <tr>
 *  <th colspan="4">Far Station A Side (ID = 0) </th>
 *  <td align="center"> </td>
 *  <th colspan="4">Near Station A Side (ID = 1) </th>
 *  <td align="center"> </td>
 *  <th colspan="4">Near Station C Side (ID = 2) </th>
 *  <td align="center"> </td>
 *  <th colspan="4">Far Station C Side (ID = 3) </th>
 * </tr>
 * <tr>
 *  <td align="center">fourthFromIP = 3</td><td align="center">thirdFromIP = 2</td><td align="center">secondFromIP = 1</td><td align="center">firstFromIP = 0</td> <th> </th>  <td align="center">fourthFromIP = 3</td><td align="center">thirdFromIP = 2</td><td align="center">secondFromIP = 1</td><td align="center">firstFromIP = 0</td>
 *  <th>INTERACTION POINT </th>
 *  <td align="center">firstFromIP = 0</td><td align="center">secondFromIP = 1</td><td align="center">thirdFromIP = 2</td><td align="center">fourthFromIP = 3</td> <th> </th>  <td align="center">firstFromIP = 0</td><td align="center">secondFromIP = 1</td><td align="center">thirdFromIP = 2</td><td align="center">fourthFromIP = 3</td>
 * </tr>
 * </table>
 */
  class AFPPixelLayerID
  {
  public:
    static const int firstFromIP; ///< the silicon layer closest to the interaction point
    static const int secondFromIP; ///< the second silicon layer counting from the interaction point
    static const int thirdFromIP; ///< the third silicon layer counting from the interaction point
    static const int fourthFromIP; ///< the fourth (last) silicon layer counting from the interaction point
  };

}
#endif //  XAODFORWARD_AFPPIXELLAYERID_H

