/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHit_v2.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-26
 * 
 * @brief  Header file for AFPSiHit_v2 class representing a hit in AFP track detector.
 * 
 */

#ifndef XAODFORWARD_VERSIONS_AFPSIHIT_V2_H
#define XAODFORWARD_VERSIONS_AFPSIHIT_V2_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// needed for forward declaration
#include "AthContainers/DataVector.h"


namespace xAOD {

  /**
   * @brief Class representing a hit in silicon detector.
   * 
   * This class provides information about a single hit in AFP silicon
   * detector.
   */
  class AFPSiHit_v2 : public SG::AuxElement
  {
  public:
    // needed by athenaRoot 
    AFPSiHit_v2();

    /** 
     * @brief Charge deposited in the pixel.
     * 
     * Charge deposited in the pixel (amplitude of the signal)
     * calculated based on the time over threshold. Time over
     * threshold should be proportional to the deposited charge, but
     * there may be small deviations.
     * 
     * @return charge deposited in the pixel
     */
    float depositedCharge() const;

    /** 
     * @brief Method setting value of the charge deposited in the pixel.
     * 
     * @param charge value of the charge deposited in the pixel
     */
    void setDepositedCharge(float charge);

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
     */
    float timeOverThreshold() const;

    /** 
     * @brief Method setting value of time over threshold.
     * 
     * @param timeOverThreshold value of the time over threshold for a pixel
     */
    void setTimeOverThreshold(float timeOverThreshold);

    /**
     * @brief Index of the station with pixel hit 
     * 
     * It is advised to use class xAOD::AFPStationID instead of
     * integers to process this information. In this class the
     * numbering scheme is explained.
     *
     * @return index of the station (see class xAOD::AFPStationID )
     */
    int stationID() const;

    /**
     * @brief Set index of the station with pixel hit 
     * 
     * @copydetails xAOD::AFPSiHit_v2::stationID()
     *
     * @param stationID index of the station with the hit
     */
    void setStationID(int stationID);

    /** 
     * @brief Index of the layer of pixels, i.e. the silicon detector, with hit.
     * 
     * It is advised to use class xAOD::AFPPixelLayerID instead of
     * integers to process this information. In this class the
     * numbering scheme is explained.
     *
     * @return index of the pixels layer with hit (see class xAOD::AFPPixelLayerID )
     */    
    int pixelLayerID() const;

    /** 
     * @brief Method setting index of the pixel layer with hit.
     *
     * The index of the layer should be set according to the convention  in pixelLayerID()
     * 
     * @param layerID index of the pixel layer with hit
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
     */
    void setPixelVertID(const int verticalID);

    /// Function making sure that the object is ready for persistification i.e. saving
    void toPersistent();
  };


} // namespace xAOD

// Declare the inheritance of the type to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPSiHit_v2, SG::AuxElement );

#endif // XAODFORWARD_VERSIONS_AFPSIHIT_V2_H

