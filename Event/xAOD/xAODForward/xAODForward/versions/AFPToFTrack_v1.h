/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPToFTrack_v1.h
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-11-11
 * 
 * @brief  Header file for the AFPToFTrack class
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPTOFTRACK_V1_H
#define XAODFORWARD_VERSIONS_AFPTOFTRACK_V1_H

// EDM include(s):

#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// needed for forward declaration

#include "AthContainers/DataVector.h"

// general includes

#include<vector>

namespace xAOD {
  // forward declaration
  class AFPToFHit_v1;
  typedef DataVector< AFPToFHit_v1 > AFPToFHitContainer_v1;

  /**
   * @brief Class representing a track reconstructed in AFP ToF.
   * 
   * This class provides access to the information about tracks that
   * were reconstructed using AFP ToF information.
   */
  class AFPToFTrack_v1 : public SG::AuxElement
  {
  public:
    /// Type of a link to the hit
    typedef ElementLink< AFPToFHitContainer_v1 > AFPToFHitLink_t;

    /** 
     * @brief Index of the station where track was reconstructed.
     *
     * @copydetails xAOD::AFPToFHit_v1::stationID()
     */  
    int stationID() const;

    /**
     * @brief Set index of the station where track was reconstructed.
     * 
     * @copydetails xAOD::AFPToFHit_v1::stationID()
     *
     * @param stationID index of the station where track was reconstructed
     *
     * @ingroup setters
     */
    void setStationID (int stationID);

    /** 
     * @brief Track local time.
     *
     */
    float trainTime() const;

    /** 
     * @brief Set track time.
     *
     * @copydetails xAPD::AFPToFTrack_v1::TrainTime()
     * 
     * @param newTrainTime
     */
    void setTrainTime (float newTrainTime);
  
    /** 
     * @brief Track train ID
     *
     * @copydetail xAOD::AFPToFTrack_v1::TrainID()
     */
    int trainID() const;

    /** 
     * @brief Set track train id(position).
     *
     * @copydetails xAOD::AFPToFTrack_v1::TrainID()
     * 
     * @param newTrainID track train ID(position)
     */
    void setTrainID (int newTrainID);
  
    /** 
     * @brief Track train size.
     *
     * @copydetails xAOD::AFPToFTrack_v1::TrainSize()
     */
    int trainSize() const;

    /** 
     * @brief Set track train size.
     *
     * @copydetails xAOD::AFPToFTrack_v1::TrainSize()
     * 
     * @param newTrainSize track train size
     */
    void setTrainSize (int newTrainSize);

    /** 
     * @brief number of saturated bars in the train
     * 
     */
    int trainNSat() const;

    /** 
     * @brief Set slope of the reconstructed track along X axis in local coordinate system.
     * 
     * @copydetails xAOD::AFPToFTrack_v1::TrainNSat()
     * 
     * @param newTrainNSat number of saturated bars in the train
     */
    void setTrainNSat (int newTrainNSat);
  

    /** 
     * @brief Vector of links to bars that were used to reconstruct the track.
     * 
     * This method provides access to the bar that were used to
     * reconstruct the track via ElementLink object.
     *
     * @note 
     * * It can be checked if the link is active using method ElementLink::isValid()
     * * A pointer to the xAOD::AFPToFHit object can be retrieved using asterisk operator e.g.
     *   @code{.cpp}
     *   xAOD::AFPToFHit* hit = *(hits().at(0));
     *   @endcode
     * 
     * @return 
     */
    const std::vector<AFPToFHitLink_t>& hits() const;

    /** 
     * @brief Set vector of links to bars used for track reconstruction.
     * 
     * @param newHitsVector vector of links to bars used for track reconstruction
     */
    void setHits( const std::vector<AFPToFHitLink_t>& newHitsVector );

    /** 
     * @brief Add a link to a bar used to reconstruct the track.
     *
     * A new link to the hit is added to the existing vector of links
     * to the bars used to reconstruct the track.
     * 
     * @param newHit link to the pixel used to reconstruct the track
     */
    void addBar( const AFPToFHitLink_t& newHit);

    /** 
     * @brief Identification number of the algorithm used to reconstruct the track.
     * 
     * The following coding is used.
     *
     * | ID  | Algorithm              | Comments |
     * | :-: | ---------------------- | -------- |
     * | 0   | Basic algorithm        |          |
     * 
     * @return identification number of the algorithm used to reconstruct the track
     */
    int algID() const;

    /** 
     * @brief Set reconstruction algorithm identification number.
     *
     * @copydetails xAOD::AFPToFTrack_v1::algID()
     * 
     * @param newIAlgID identification number of the algorithm used to reconstruct the track
     */
    void setAlgID (int newIAlgID);

    /// Function making sure that the object is ready for persistification i.e. saving
    void toPersistent();
  };

}

// Declare the inheritance of the type to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPToFTrack_v1, SG::AuxElement );

     
#endif
     
