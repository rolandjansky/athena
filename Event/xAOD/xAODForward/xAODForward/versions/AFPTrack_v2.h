/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPTrack_v2.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-26
 * 
 * @brief  Header file for the AFPTrack class
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPTRACK_V2_H
#define XAODFORWARD_VERSIONS_AFPTRACK_V2_H

// general includes
#include<vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

#include "xAODForward/versions/AFPSiHitsCluster_v1.h"
#include "xAODForward/versions/AFPSiHitsClusterContainer_v1.h"

namespace xAOD {

  /**
   * @brief Class representing a track reconstructed in AFP.
   * 
   * This class provides access to the information about tracks that
   * were reconstructed using AFP information.
   */
  class AFPTrack_v2 : public SG::AuxElement
  {
  public:
    /// Type of a link to the cluster
    typedef ElementLink< AFPSiHitsClusterContainer_v1 > AFPClusterLink_t;

    /** 
     * @brief Index of the station where track was reconstructed.
     *
     * @copydetails xAOD::AFPSiHit_v2::stationID()
     */  
    int stationID() const;

    /**
     * @brief Set index of the station where track was reconstructed.
     * 
     * @copydetails xAOD::AFPSiHit_v2::stationID()
     *
     * @param stationID index of the station where track was reconstructed
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
     * @copydetails xAPD::AFPTrack_v2::xLocal()
     * 
     * @param newXLocal track coordinate along X axis in station local coordinate system
     */
    void setXLocal (float newXLocal);
  
    /** 
     * @brief Track position along Y axis in station local coordinate system.
     *
     * @copydetail xAOD::AFPTrack_v2::xLocal()
     */
    float yLocal() const;

    /** 
     * @brief Set track coordinate along Y axis in station local coordinate system.
     *
     * @copydetails xAOD::AFPTrack_v2::yLocal()
     * 
     * @param newYLocal track position along Y axis in station local coordinate system
     */
    void setYLocal (float newYLocal);
  
    /** 
     * @brief Track position along Z axis in station local coordinate system.
     *
     * @copydetails xAOD::AFPTrack_v2::xLocal()
     */
    float zLocal() const;

    /** 
     * @brief Set track coordinate along Z axis in station local coordinate system.
     *
     * @copydetails xAOD::AFPTrack_v2::zLocal()
     * 
     * @param newYLocal track coordinate along Z axis in station local coordinate system
     */
    void setZLocal (float newZLocal);

    /** 
     * @brief Slope of the reconstructed track along X axis in local coordinate system.
     * 
     * Difference between X position of the first and last cluster used to
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
     * @copydetails xAOD::AFPTrack_v2::xSlope()
     * 
     * @param newXSlope slope of the track along X axis in local coordinate system
     */
    void setXSlope (float newXSlope);
  

    /** 
     * @brief Slope of the reconstructed track along Y axis in local coordinate system.
     * 
     * Difference between Y position of the first and last cluster used to
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
     * @copydetails xAOD::AFPTrack_v2::ySlope()
     * 
     * @param newYSlope slope of the track along Y axis in local coordinate system
     */
    void setYSlope (float newYSlope);
  
    /** 
     * @brief Number of empty layers that the track passes through.
     * 
     * Number of layers that are expected to be hit, because track
     * goes through them, but which are considered to be not fired.
     *
     * @return Number of empty pixels that the track passes through
     */
    int nHoles() const;

    /** 
     * @brief Set number of empty pixels that the track passes through.
     * 
     * @copydetails xAOD::AFPTrack_v2::nHoles()
     *
     * @param nHoles number of empty pixels that the track passes through
     */
    void setNHoles (int nHoles);

    /** 
     * @brief Number of clusters used to reconstruct the track.
     * 
     * @return number of clusters used to reconstruct the track.
     */
    int nClusters() const;

    /** 
     * @brief Set number of clusters used to reconstruct the track.
     * 
     * @param nClusters number of clusters used to reconstruct the track
     */
    void setNClusters (int nClusters);

    /** 
     * @brief Vector of links to clusters that were used to reconstruct the track.
     * 
     * This method provides access to the clusters that were used to
     * reconstruct the track via ElementLink object.
     *
     * @note 
     * * It can be checked if the link is active using method ElementLink::isValid()
     * * A pointer to the xAOD::AFPSiHitsCluster object can be retrieved using asterisk operator e.g.
     *   @code{.cpp}
     *   xAOD::AFPSiHitsCluster* cluster = *(clusters().at(0));
     *   @endcode
     * 
     * @return 
     */
    const std::vector<AFPClusterLink_t>& clusters() const;

    /** 
     * @brief Set vector of links to clusters used for track reconstruction.
     * 
     * @param newClustersVector vector of links to clusters used for track reconstruction
     */
    void setClusters( const std::vector<AFPClusterLink_t>& newClustersVector );

    /** 
     * @brief Add a link to a cluster used to reconstruct the track.
     *
     * A new link to the cluster is added to the existing vector of links
     * to the clusters used to reconstruct the track.
     * 
     * @param newCluster link to the cluster used to reconstruct the track
     */
    void addCluster( const AFPClusterLink_t& newCluster);

    /** 
     * @brief @f$\chi^2@f$ value of the track fit to the selected clusters.
     *
     * This value gives information about how well the fitted track
     * lays on the selected clusters.
     * 
     * @return @f$\chi^2@f$ value of the track fit to the selected clusters
     */    
    float chi2() const;

    /** 
     * @brief Set @f$\chi^2@f$ value of the track fit to the selected clusters.
     * 
     * @param newFChi2 @f$\chi^2@f$ value of the track fit to the selected clusters
     */
    void setChi2 (float newFChi2);	

    /** 
     * @brief Identification number of the algorithm used to reconstruct the track.
     * 
     * It is advised to use class xAOD::AFPTrackRecoAlgID instead of
     * integers to process this information. In this class the
     * numbering scheme is explained.
     * 
     * @return identification number of the algorithm used to reconstruct the track (see xAOD::AFPTrackRecoAlgID )
     */
    int algID() const;

    /** 
     * @brief Set reconstruction algorithm identification number.
     *
     * @copydetails xAOD::AFPTrack_v2::algID()
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
SG_BASE( xAOD::AFPTrack_v2, SG::AuxElement );

     
#endif
     
