/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPStationID.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-13
 * 
 * @brief  Definitions of AFP stations identification numbers
 * 
 */

#ifndef XAODFORWARD_AFPSTATIONID_H
#define XAODFORWARD_AFPSTATIONID_H

namespace xAOD
{
  
/**
 * @brief Helper class to store ID numbers of AFP stations
 *
 * This class contains public static const variables which can be used
 * instead of raw numbers for refering to station ID. Enum is not used
 * to allow adding new numbers without the need of modifying the
 * header file.
 *
 * An example of usage
 * @code{.cpp}
 * xAOD::AFPSiHit* hit;
 * if (hit->stationID() == xAOD::AFPStationID::farA) {
 * // do something with station at z = 217 m
 * }
 * @endcode
 * 
 * Stations indexing follows the scheme
 *
 * | Station ID | variable name | Station Name        | Station Position |
 * | :--------: | ------------- | ------------------- | ---------------- |
 * |    0       | farA          | far station A side  | z = 217 m        |
 * |    1       | nearA         | near station A side | z = 205 m        |
 * |    2       | nearC         | near station C side | z = -205 m       |
 * |    3       | farC          | far station C side  | z = -217 m       |
 * 
 */
  class AFPStationID
  {
  public:
    static const int farA; ///< station with at Z = 217 m
    static const int nearA; ///< station with at Z = 205 m
    static const int nearC; ///< station with at Z = -205 m
    static const int farC; ///< station with at Z = -217 m
  };

}
#endif // XAODFORWARD_AFPSTATIONID_H

