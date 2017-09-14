/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHit_v1.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2016-07-13
 * 
 * @brief  Header file for AFPSiHit_v1 class representing a hit in AFP track detector.
 * 
 */

#ifndef XAODFORWARD_VERSIONS_AFPSIHIT_V1_H
#define XAODFORWARD_VERSIONS_AFPSIHIT_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// needed for forward declaration
#include "AthContainers/DataVector.h"


namespace xAOD {

  // forward declaration
  class AFPTrack_v1;
  typedef DataVector< AFPTrack_v1 > AFPTrackContainer_v1;

  /**
   * @brief Class representing a hit in silicon detector.
   * 
   * This class provides information about a single hit in AFP silicon
   * detector.
   */
  class AFPSiHit_v1 : public SG::AuxElement
  //  class AFPSiHit_v1 : public  SG::AuxElement
  {
  public:
    /// Type of a link to the track
    typedef ElementLink< AFPTrackContainer_v1 > AFPTrackLink_t;

    
    // needed by athenaRoot 
    AFPSiHit_v1();

    /** 
     * @brief Charge deposited in the pixel.
     * 
     * Charge deposited in the pixel (amplitude of the signal)
     * calculated based on the time over threshold. Time over
     * threshold should be proportional to the deposited charge, but
     * there may be small deviations.
     * 
     * @return charge deposited in the pixel
     *
     * @ingroup getters
     */
    float depositedCharge() const;

    /** 
     * @brief Method setting value of the charge deposited in the pixel.
     * 
     * @param charge value of the charge deposited in the pixel
     *
     * @ingroup setters
     */
    void setDepositedCharge(float charge);                      // amplitude of the signal for given pixel

    /** 
     * @brief Time over threshold of signal for a pixel.
     *
     * The length of the signal is proportional to the charge
     * deposited in the pixel. The transformation is configured during
     * the detector running and requires calibration. This is basic
     * quantity which is used to reconstruct charge deposited in the
     * pixel.
     * 
     * @return time over threshold of the signal
     *
     * @ingroup getters
     */
    float timeOverThreshold() const;

    /** 
     * @brief Method setting value of time over threshold.
     * 
     * @param timeOverThreshold value of the time over threshold for a pixel
     *
     * @ingroup setters
     */
    void setTimeOverThreshold(float timeOverThreshold);                      // amplitude of the signal for given pixel

    
    // float discriminator() const;                      // Constant fraction discriminator time for given pixel
    // void setDiscriminator(float discriminator);                      // Constant fraction discriminator time for given pixel


    /**
     * @brief Index of the station with pixel hit 
     * 
     * Stations indexing follows the scheme
     *
     * | Station ID | Station Name        | Station Position |
     * | :--------: | ------------------- | ---------------- |
     * |    0       | far station A side  | z = 217 m        |
     * |    1       | near station A side | z = 205 m        |
     * |    2       | near station C side | z = -205 m       |
     * |    3       | far station C side  | z = -217 m       |
     * 
     * @return index of the station
     *
     * @ingroup getters
     */
    int stationID() const;

    /**
     * @brief Set index of the station with pixel hit 
     * 
     * @copydetails xAOD::AFPSiHit_v1::stationID()
     *
     * @param stationID index of the station with the hit
     *
     * @ingroup setters
     */
    void setStationID(int stationID);

    /** 
     * @brief Index of the layer of pixels, i.e. the silicon detector, with hit.
     * 
     * Index of the pixel layer where the hit was recorded. The layer
     * closest to the interaction point has index 0, the layer furthest
     * away from the interaction point has index 3.
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
     *  <td align="center">3</td><td align="center">2</td><td align="center">1</td><td align="center">0</td> <th> </th>  <td align="center">3</td><td align="center">2</td><td align="center">1</td><td align="center">0</td>
     *  <th>INTERACTION POINT </th>
     *  <td align="center">0</td><td align="center">1</td><td align="center">2</td><td align="center">3</td> <th> </th>  <td align="center">0</td><td align="center">1</td><td align="center">2</td><td align="center">3</td>
     * </tr>
     * </table>
     *
     * @return index of the pixels layer with hit
     *
     * @ingroup getters
     */    
    int pixelLayerID() const;

    /** 
     * @brief Method setting index of the pixel layer with hit.
     *
     * The index of the layer should be set according to the convention  in pixelLayerID()
     * 
     * @param layerID index of the pixel layer with hit
     *
     * @ingroup setters
     */
    void setPixelLayerID(int layerID);
    
    /** 
     * @brief Index of the pixel row in chip coordinate system.
     *
     * Index of the row in chip coordinate system. There are 336 rows
     * numbered from 1. Row number 1 is closest to the LHC centre,
     * while row 336 is the outermost.
     * 
     * @note Rows in chip coordinate system are vertical,
     * because the chips are mounted rotated by 90 degrees.
     * 
     * @return index of the pixel row which is in vertical direction
     *
     * @ingroup getters
     */
    int pixelRowIDChip() const;

    /** 
     * @brief Set pixel row index in the chip coordinate system.
     *
     * Index of the row in chip coordinate system can have value
     * between 1 and 336. Row number 1 is closest to the LHC centre,
     * while row 336 is the outermost.
     *
     * @note Rows in chip coordinate system are vertical,
     * because the chips are mounted rotated by 90 degrees.
     * 
     * @param rowID index of pixel row in chip coordinate system to be assigned to the hit
     *
     * @ingroup setters
     */
    void setPixelRowIDChip(int rowID);

    
    /** 
     * @brief Index of the pixel column in chip coordinate system.
     *
     * Index of the column in chip coordinate system. There are 80
     * columns numbered from 1. Column number 1 is the bottom one,
     * while column 80 is the topmost one.
     * 
     * @note Columns in chip coordinate system are horizontal,
     * because the chips are mounted rotated by 90 degrees.
     * 
     * @return index of the pixel column which is in horizontal direction
     *
     * @ingroup getters
     */
    int pixelColIDChip() const;

    /** 
     * @brief Set pixel column index in the chip coordinate system.
     *
     * Index of the column in chip coordinate system can have value
     * between 1 and 80. Column number 1 is the bottom one, while
     * column 80 is the topmost one.
     *
     * @note Columns in chip coordinate system are horizontal,
     * because the chips are mounted rotated by 90 degrees.
     * 
     * @param colID index of pixel column in chip coordinate system to be assigned to the hit
     *
     * @ingroup setters
     */
    void setPixelColIDChip(int colID);


    /** 
     * @brief Index of the pixel along X axis in LHC coordinate system.
     *
     * There are 336 pixels in the horizontal direction numbered from
     * 0. The numbering is according to the LHC coordinate system
     * i.e. pixel 0 is the outermost pixel and pixel 335 is the
     * innermost (closest to the LHC centre).
     *
     * This method transforms index from the chip coordinate system to
     * the LHC coordinate system.
     * 
     * @return index of the pixel along X axis in LHC coordinate system
     *
     * @ingroup getters
     */
    int pixelHorizID() const;

    /** 
     * @brief Set index of the pixel along X axis in LHC coordinate system.
     *
     * There are 336 pixels in the horizontal direction numbered from
     * 0. The numbering is according to the LHC coordinate system
     * i.e. pixel 0 is the outermost pixel and pixel 335 is the inner
     * most (closest to the LHC centre).
     *
     * This method transforms input from the LHC coordinate system to
     * the chip coordinate system and saves it.
     *
     * @param horizontalID index of pixel along X axis in the LHC coordinate system
     *
     * @ingroup setters
     */
    void setPixelHorizID(const int horizontalID);

    
    /** 
     * @brief Index of the pixel along Y axis in LHC coordinate system.
     *
     * There are 80 pixels in the vertical direction numbered from
     * 0. The numbering is according to the LHC coordinate system
     * i.e. pixel 0 is the lowest and pixel 79 is the topmost.
     *
     * This method transforms index from the chip coordinate system to
     * the LHC coordinate system.
     * 
     * @return index of the pixel along Y axis in LHC coordinate system
     *
     * @ingroup getters
     */
    int pixelVertID() const;

    /** 
     * @brief Set index of the pixel along Y axis in LHC coordinate system.
     *
     * There are 80 pixels in the horizontal direction numbered from
     * 0. The numbering is according to the LHC coordinate system
     * i.e. pixel 0 is the lowest and pixel 79 is the topmost.
     *
     * This method transforms input from the LHC coordinate system to
     * the chip coordinate system and saves it.
     *
     * @param verticalID index of pixel along Y axis in the LHC coordinate system
     *
     * @ingroup setters
     */
    void setPixelVertID(const int verticalID);

    /** 
     * @brief Vector of links to tracks for which reconstruction this hit was used.
     * 
     * This method provides access to the tracks that use this
     * hit. The access is via ElementLink object.
     *
     * @note 
     * * It can be checked if the link is active using method ElementLink::isValid()
     * * A pointer to the xAOD::AFPTrack object can be retrieved using asterisk operator e.g.
     *   @code{.cpp}
     *   xAOD::AFPTrack* track = *(tracks().at(0));
     *   @endcode
     * 
     * @return 
     */
    const std::vector<AFPTrackLink_t>& tracksLinks() const;

    /** 
     * @brief Set vector of links to tracks that use this hit.
     * 
     * @param tracksVector vector of links to tracks
     */
    void setTracksLinks( const std::vector<AFPTrackLink_t>& tracksVector );

    /** 
     * @brief Add a link to a track that uses this hit in reconstruction.
     *
     * A new link to the track is added to the existing vector of links
     * to the tracks which use this hit in reconstruction.
     * 
     * @param newTrack link to the track
     */
    void addTrackLink( const AFPTrackLink_t& newTrack);

    /// Function making sure that the object is ready for persistification i.e. saving
    void toPersistent();
  };


} // namespace xAOD

// Declare the inheritance of the type to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPSiHit_v1, SG::AuxElement );

#endif // XAODFORWARD_VERSIONS_AFPSIHIT_V1_H

