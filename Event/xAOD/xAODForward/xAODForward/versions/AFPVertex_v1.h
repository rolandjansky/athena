/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPVertex_v1.h
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-12-08
 * 
 * @brief  Header file for the AFPVertex class
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPVERTEX_V1_H
#define XAODFORWARD_VERSIONS_AFPVERTEX_V1_H

// general includes
#include<vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// needed for forward declaration
#include "AthContainers/DataVector.h"

#include "xAODForward/versions/AFPProton_v1.h"
#include "xAODForward/versions/AFPProtonContainer_v1.h"
#include "xAODForward/versions/AFPToFTrack_v1.h"
#include "xAODForward/versions/AFPToFTrackContainer_v1.h"

namespace xAOD {
  // forward declaration
  class AFPProton_v1;
  typedef DataVector< AFPProton_v1 > AFPProtonContainer_v1;

  class AFPToFTrack_v1;
  typedef DataVector< AFPToFTrack_v1 > AFPToFTrackContainer_v1;

  /**
   * @brief Class representing a vertex reconstructed in AFP.
   * 
   * This class provides access to the information about vertices that
   * were reconstructed using AFP information.
   */
  class AFPVertex_v1 : public SG::AuxElement
  {
  public:

    /// Type of a link to the proton
    typedef ElementLink< AFPProtonContainer_v1 > AFPProtonLink_t;

    /// Type of a link to the tofTrack
    typedef ElementLink< AFPToFTrackContainer_v1 > AFPToFTrackLink_t;

    /** 
     * @brief Vertex position .
     *
     * @copydetail xAOD::AFPVertex_v1::position()
     */
    float position() const;

    /** 
     * @brief Set vertex position.
     *
     * @copydetails xAOD::AFPVertex_v1::position()
     * 
     * @param newPosition vertex
     */
    void setPosition (float newPosition);
  
    /** 
     * @brief Vector of links to protons that were used to reconstruct vertex
     * 
     * This method provides access to the protons that were used to
     * reconstruct the vertex via ElementLink object.
     *
     * @note 
     * * It can be checked if the link is active using method ElementLink::isValid()
     * * A pointer to the xAOD::AFPProton object can be retrieved using asterisk operator e.g.
     *   @code{.cpp}
     *   xAOD::AFPProton* proton = *(proton().at(0));
     *   @endcode
     * 
     * @return 
     */
    const std::vector<AFPProtonLink_t>& protons() const;

    /** 
     * @brief Set vector of links to protons used for vertex reconstruction.
     * 
     * @param newProtonsVector vector of links to protons used for vertex reconstruction
     */

    void setProtons( const std::vector<AFPProtonLink_t>& newProtonsVector );


    /** 
     * @brief Add a link to a proton used to reconstruct the vertex.
     *
     * A new link to the proton is added to the existing vector of links
     * to the protons used to reconstruct the vertex.
     * 
     * @param newProton link to the proton used to reconstruct the vertex
     */
    void addProton( const AFPProtonLink_t& newProton);


    /**
     * @brief Vector of links to ToFTracks that were used to reconstruct vertex
     *
     * This method provides access to the ToFTracks that were used to
     * reconstruct the vertex via ElementLink object.
     *
     * @note
     * * It can be checked if the link is active using method ElementLink::isValid()
     * * A pointer to the xAOD::AFPToFTrack object can be retrieved using asterisk operator e.g.
     *   @code{.cpp}
     *   xAOD::AFPToFTrack* tofTrack = *(tofTrack().at(0));
     *   @endcode
     *
     * @return
     */
    const std::vector<AFPToFTrackLink_t>& tofTracks() const;

    /**
     * @brief Set vector of links to ToFTracks used for vertex reconstruction.
     *
     * @param newToFTracksVector vector of links to ToFTracks used for vertex reconstruction
     */
    void setToFTracks( const std::vector<AFPToFTrackLink_t>& newToFTracksVector );

    /**
     * @brief Add a link to a ToFTrack used to reconstruct the vertex.
     *   
     * A new link to the ToFTrack is added to the existing vector of links
     * to the ToFTrackss used to reconstruct the vertex.
     * 
     * @param newToFTrack link to the ToFTrack used to reconstruct the vertex
     */
    void addToFTrack( const AFPToFTrackLink_t& newToFTrack);


    /** 
     * Distance of the proton to the selected ToFTracks.
     *
     * This value gives information about how well the proton  trajectory
     * matches on the selected ToFTracks.
     * 
     * return  distance of the track  to the selected ToFTrack
     */    
    float distA() const;
    float distC() const;

    /** 
     * @brief Set @f$\chi^2@f$ value of the prootn fit to the selected ToFTracks.
     * 
     * @param newFChi2 @f$\chi^2@f$ value of the proton fit to the selected ToFTrackss
     */
    void setDistA (float newDistA);	
    void setDistC (float newDistC);

    /** 
     * @brief Identification number of the algorithm used to reconstruct the vertex.
     * 
     * The following coding is used.
     *
     * | ID  | Algorithm              | Comments |
     * | :-: | ---------------------- | -------- |
     * | 0   | Basic algorithm |          |
     * 
     * @return identification number of the algorithm used to reconstruct the track
     */
    int algID() const;

    /** 
     * @brief Set reconstruction algorithm identification number.
     *
     * @copydetails xAOD::AFPVertex_v1::algID()
     * 
     * @param newIAlgID identification number of the algorithm used to reconstruct the vertex
     */
    void setAlgID (int newIAlgID);

    /// Function making sure that the object is ready for persistification i.e. saving
    void toPersistent();
  };

}

// Declare the inheritance of the type to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPVertex_v1, SG::AuxElement );

     
#endif
     
