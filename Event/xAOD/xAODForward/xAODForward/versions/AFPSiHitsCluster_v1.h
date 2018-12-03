/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHitsCluster_v1.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-12
 * 
 * @brief  Header file for AFPSiHitsCluster_v1 class representing a cluster of hits in AFP tracking detector plate.
 * 
 */

#ifndef XAODFORWARD_VERSIONS_AFPSIHITSCLUSTER_V1_H
#define XAODFORWARD_VERSIONS_AFPSIHITSCLUSTER_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// needed for forward declaration
#include "AthContainers/DataVector.h"

// helper classes decoding AFP information
#include "xAODForward/AFPStationID.h"
#include "xAODForward/AFPPixelLayerID.h"

// xAOD representing hits
#include "xAODForward/versions/AFPSiHit_v2.h"
#include "xAODForward/versions/AFPSiHitContainer_v2.h"

namespace xAOD {

  /// Class representing a cluster of AFP pixel hits.
  class AFPSiHitsCluster_v1 : public SG::AuxElement
  {
  public:
    /// Type of a link to the hit
    typedef ElementLink< AFPSiHitContainer_v2 > AFPHitLink_t;

    /**
     * @brief Index of the station with pixels cluster
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
     * @copydetails xAOD::AFPSiHitsCluster_v1::stationID()
     *
     * @param stationID index of the station with the hit
     */
    void setStationID(int stationID);

    /** 
     * @brief Index of the silicon layer with the cluster.
     * 
     * It is advised to use class xAOD::AFPPixelLayerID instead of
     * integers to process this information. In this class the
     * numbering scheme is explained.
     */    
    int pixelLayerID() const;

    /** 
     * @brief Method setting index of the detector layer with the cluster.
     *
     * @copydetails xAOD::AFPSiHitsCluster_v1::pixelLayerID()
     * 
     * @param layerID index of the pixel layer with the cluster
     */
    void setPixelLayerID(int layerID);


    /// Cluster position along X axis in station local coordinate system.
    float xLocal() const;

    /// Set cluster position along X axis in station local coordinate system.
    void setXLocal (float newXLocal);

    /// Uncertainty of cluster position along X axis in station local coordinate system.
    float xLocalErr() const;

    /// Set uncertainty of cluster position along X axis in station local coordinate system.
    void setXLocalErr (float newXLocalErr);

    /// Cluster position along Y axis in station local coordinate system.
    float yLocal() const;

    /// Set cluster position along Y axis in station local coordinate system.
    void setYLocal (float newYLocal);
  
    /// Uncertainty of cluster position along Y axis in station local coordinate system.
    float yLocalErr() const;

    /// Set uncertainty of cluster position along Y axis in station local coordinate system.
    void setYLocalErr (float newYLocalErr);
  
    /// Cluster position along Z axis in station local coordinate system.
    float zLocal() const;

    /// Set cluster position along Z axis in station local coordinate system.
    void setZLocal (float newZLocal);

    /// Uncertainty of cluster position along Z axis in station local coordinate system.
    float zLocalErr() const;

    /// Set uncertainty of cluster position along Z axis in station local coordinate system.
    void setZLocalErr (float newZLocalErr);

    /// @brief Total charge of the cluster.
    ///
    ///The method of calculation depends on used algorithm.
    float depositedCharge() const;

    /// @brief Method setting total charge of the cluster.
    ///
    /// @param charge value of the cluster charge
    void setDepositedCharge(float charge);


    /// Number of fired pixels (hits) in cluster.
    int nHits() const;

    /// Set number of fired pixels (hits) in the cluster.
    void setNHits (int nPixels);

    /** 
     * @brief Vector of links to pixels that were used to construct the cluster.
     * 
     * This method provides access to the hits (pixels) that were used to
     * reconstruct the cluster via ElementLink object.
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
    const std::vector<AFPHitLink_t>& hitsLinks() const;

    /** 
     * @brief Set vector of links to pixels used to construct the cluster.
     * 
     * @param newHitsVector vector of links to pixels used to construct the cluster.
     */
    void setHitsLinks( const std::vector<AFPHitLink_t>& newHitsVector );

    /** 
     * @brief Add a link to a pixel in this cluster.
     *
     * A new link to the hit is added to the existing vector of links
     * to the pixels in this cluster.
     * 
     * @param newHit link to the pixel used to construct this cluster.
     */
    void addHitLink( const AFPHitLink_t& newHit);

    /** 
     * @brief Identification number of the algorithm used to cluster pixels.
     * 
     * The following coding is used.
     *
     * | ID  | Algorithm              | Comments |
     * | :-: | ---------------------- | -------- |
     * | 0   | no clustering - one hit per clusetr |          |
     */
    int clusterAlgID() const;

    /** 
     * @brief Set ID number of the clustering algorithm.
     *
     * @copydetails xAOD::AFPCluster_v1::clusterAlgID()
     * 
     * @param newIAlgID identification number of the algorithm used to cluster hits
     */
    void setClusterAlgID (int newIAlgID);

    /// Function making sure that the object is ready for persistification i.e. saving
    void toPersistent();
  };

} // namespace xAOD

// Declare the inheritance of the type to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPSiHitsCluster_v1, SG::AuxElement );

#endif // XAODFORWARD_VERSIONS_AFPSIHITSCLUSTER_V1_H

