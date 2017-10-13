/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPTrack_v1.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2016-07-14
 * 
 * @brief  Header file for the AFPTrack class
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPTRACK_V1_H
#define XAODFORWARD_VERSIONS_AFPTRACK_V1_H

// general includes
#include<vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// needed for forward declaration
#include "AthContainers/DataVector.h"

namespace xAOD {
  // forward declaration
  class AFPSiHit_v1;
  typedef DataVector< AFPSiHit_v1 > AFPSiHitContainer_v1;

  /**
   * @brief Class representing a track reconstructed in AFP.
   * 
   * This class provides access to the information about tracks that
   * were reconstructed using AFP information.
   */
  class AFPTrack_v1 : public SG::AuxElement
  {
  public:
    /// Type of a link to the hit
    typedef ElementLink< AFPSiHitContainer_v1 > AFPHitLink_t;

    /** 
     * @brief Index of the station where track was reconstructed.
     *
     * @copydetails xAOD::AFPSiHit_v1::stationID()
     */  
    int stationID() const;

    /**
     * @brief Set index of the station where track was reconstructed.
     * 
     * @copydetails xAOD::AFPSiHit_v1::stationID()
     *
     * @param stationID index of the station where track was reconstructed
     *
     * @ingroup setters
     */
    void setStationID (int stationID);

    /** 
     * @brief Track position along X axis in station local coordinate system.
     *
     * This is the position of the beginning of the track i.e. closest
     * to the interaction point. Usually in the first pixel plane.
     */
    float xLocal() const;

    /** 
     * @brief Set track position along X axis in station local coordinate system.
     *
     * @copydetails xAPD::AFPTrack_v1::xLocal()
     * 
     * @param newXLocal track coordinate along X axis in station local coordinate system
     */
    void setXLocal (float newXLocal);
  
    /** 
     * @brief Track position along Y axis in station local coordinate system.
     *
     * @copydetail xAOD::AFPTrack_v1::xLocal()
     */
    float yLocal() const;

    /** 
     * @brief Set track coordinate along Y axis in station local coordinate system.
     *
     * @copydetails xAOD::AFPTrack_v1::yLocal()
     * 
     * @param newYLocal track position along Y axis in station local coordinate system
     */
    void setYLocal (float newYLocal);
  
    /** 
     * @brief Track position along Z axis in station local coordinate system.
     *
     * @copydetails xAOD::AFPTrack_v1::xLocal()
     */
    float zLocal() const;

    /** 
     * @brief Set track coordinate along Z axis in station local coordinate system.
     *
     * @copydetails xAOD::AFPTrack_v1::zLocal()
     * 
     * @param newYLocal track coordinate along Z axis in station local coordinate system
     */
    void setZLocal (float newZLocal);

    /** 
     * @brief Slope of the reconstructed track along X axis in local coordinate system.
     * 
     * Difference between X position of the first and last hit used to
     * reconstruct track divided by their distance in Z direction.
     * @f[
     *   \textrm{xSlope} = \frac{x_{\textrm{end}} - x_{\textrm{begin}}}
     *                          {z_{\textrm{end}} - z_{\textrm{begin}}}
     * @f]
     */
    float xSlope() const;

    /** 
     * @brief Set slope of the reconstructed track along X axis in local coordinate system.
     * 
     * @copydetails xAOD::AFPTrack_v1::xSlope()
     * 
     * @param newXSlope slope of the track along X axis in local coordinate system
     */
    void setXSlope (float newXSlope);
  

    /** 
     * @brief Slope of the reconstructed track along Y axis in local coordinate system.
     * 
     * Difference between Y position of the first and last hit used to
     * reconstruct track divided by their distance in Z direction.
     * @f[
     *   \textrm{ySlope} = \frac{y_{\textrm{end}} - y_{\textrm{begin}}}
     *                          {z_{\textrm{end}} - z_{\textrm{begin}}}
     * @f]
     */
    float ySlope() const;

    /** 
     * @brief Set slope of the reconstructed track along Y axis in local coordinate system.
     * 
     * @copydetails xAOD::AFPTrack_v1::ySlope()
     * 
     * @param newYSlope slope of the track along Y axis in local coordinate system
     */
    void setYSlope (float newYSlope);
  
    // float zSlope() const;
    // void setZSlope (float newZSlope);

    /** 
     * @brief Number of empty pixels that the track passes through.
     * 
     * Number of pixels that are expected to be hit, because track
     * goes through them, but which are considered to be not fired.
     *
     * @return Number of empty pixels that the track passes through
     */
    int nHoles() const;


    /** 
     * @brief Set number of empty pixels that the track passes through.
     * 
     * @copydetails xAOD::AFPTrack_v1::nHoles()
     *
     * @param nHoles number of empty pixels that the track passes through
     */
    void setNHoles (int nHoles);


    /** 
     * @brief Number of pixels used to reconstruct the track.
     * 
     * @return number of pixels used to reconstruct the track.
     */
    int nHits() const;

    /** 
     * @brief Set number of pixels used to reconstruct the track.
     * 
     * @param nHits number of pixels used to reconstruct the track
     */
    void setNHits (int nHits);

    /** 
     * @brief Vector of links to pixels that were used to reconstruct the track.
     * 
     * This method provides access to the pixels that were used to
     * reconstruct the track via ElementLink object.
     *
     * @note 
     * * It can be checked if the link is active using method ElementLink::isValid()
     * * A pointer to the xAOD::AFPSiHit object can be retrieved using asterisk operator e.g.
     *   @code{.cpp}
     *   xAOD::AFPSiHit* hit = *(hits().at(0));
     *   @endcode
     * 
     * @return 
     */
    const std::vector<AFPHitLink_t>& hits() const;

    /** 
     * @brief Set vector of links to pixels used for track reconstruction.
     * 
     * @param newHitsVector vector of links to pixels used for track reconstruction
     */
    void setHits( const std::vector<AFPHitLink_t>& newHitsVector );

    /** 
     * @brief Add a link to a pixel used to reconstruct the track.
     *
     * A new link to the hit is added to the existing vector of links
     * to the pixels used to reconstruct the track.
     * 
     * @param newHit link to the pixel used to reconstruct the track
     */
    void addHit( const AFPHitLink_t& newHit);

    /** 
     * @brief @f$\chi^2@f$ value of the track fit to the selected pixels.
     *
     * This value gives information about how well the fitted track
     * lays on the selected pixels.
     * 
     * @return @f$\chi^2@f$ value of the track fit to the selected pixels
     */    
    float chi2() const;

    /** 
     * @brief Set @f$\chi^2@f$ value of the track fit to the selected pixels.
     * 
     * @param newFChi2 @f$\chi^2@f$ value of the track fit to the selected pixels
     */
    void setChi2 (float newFChi2);	

    /** 
     * @brief Identification number of the algorithm used to reconstruct the track.
     * 
     * The following coding is used.
     *
     * | ID  | Algorithm              | Comments |
     * | :-: | ---------------------- | -------- |
     * | 0   | Basic Kalman algorithm |          |
     * 
     * @return identification number of the algorithm used to reconstruct the track
     */
    int algID() const;

    /** 
     * @brief Set reconstruction algorithm identification number.
     *
     * @copydetails xAOD::AFPTrack_v1::algID()
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
SG_BASE( xAOD::AFPTrack_v1, SG::AuxElement );

     
#endif
     
