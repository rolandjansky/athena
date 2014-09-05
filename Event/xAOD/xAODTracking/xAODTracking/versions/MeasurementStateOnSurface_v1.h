/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_MEASUREMENTSTATEONSURFACE_V1_H
#define XAODTRACKING_VERSIONS_MEASUREMENTSTATEONSURFACE_V1_H

// System include(s):
#include <vector>

// Core include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTracking/PrepRawDataContainer.h"

namespace xAOD {

  /// Class describing a MeasurementStateOnSurface.
  ///
  /// @author Niels Van Eldik <niels.van.eldik@cern.ch>
  /// @author Anthony Morley <anthony.morley@cern.ch>
  /// @author Edward Moyse <Edward.Moyse@cern.ch>
  /// @author Heberth Torres <htorres@cern.ch>
  /// @nosubgrouping
  ///
  class MeasurementStateOnSurface_v1 : public SG::AuxElement {

  public:

    /// Default constructor
    MeasurementStateOnSurface_v1();

    /// @name Identification 
    /// @{
    /// Returns the type (measurement, outlier, hole)
    char type() const;
    /// Sets the type (measurement, outlier, hole)
    void setType(char type);
    /// Returns the detector element identifier
    uint64_t detElementId() const;
    /// Sets the detector element identifier
    void setDetElementId(uint64_t detElementId);
    /// Returns the detector type
    char detType() const;
    /// Sets the detector type
    void setDetType(char detType);
    /// @}


    /// @name Local info functions
    /// Returns the local position and angles
    /// @{
    /// Returns the x position
    float localX() const;
    /// Returns the y position
    float localY() const;
    /// Sets the local position
    void setLocalPosition(float localX, float localY);
    /// Returns the theta position
    float localTheta() const;
    /// Returns the phi position
    float localPhi() const;
    /// Sets the local angles
    void setLocalAngles(float localTheta, float localPhi);

    /// Returns the unbiased x residual
    float unbiasedResidualX() const;
    /// Returns the unbiased x residual
    float unbiasedResidualY() const;
    /// Sets the unbiased residual
    void setUnbiasedResidual(float unbiasedResidualX, float unbiasedResidualY);

    /// Returns the unbiased x pull
    float unbiasedPullX() const;
    /// Returns the unbiased x pull
    float unbiasedPullY() const;
    /// Sets the unbiased pull
    void setUnbiasedPull(float unbiasedPullX, float unbiasedPullY);

    /// Returns the biased x residual
    float biasedResidualX() const;
    /// Returns the biased x residual
    float biasedResidualY() const;
    /// Sets the biased residual
    void setBiasedResidual(float biasedResidualX, float biasedResidualY);

    /// Returns the biased x pull
    float biasedPullX() const;
    /// Returns the biased x pull
    float biasedPullY() const;
    /// Sets the biased pull
    void setBiasedPull(float biasedPullX, float biasedPullY);
    /// @}

    /// @name
    /// @{
    /// Returns the link to the PrepRawDataContainer
    ElementLink<xAOD::PrepRawDataContainer> prepRawDataLink() const;
    /// sets the link to the PrepRawDataContainer
    void setPrepRawDataLink(ElementLink<xAOD::PrepRawDataContainer> prepRawDataLink);
    /*     /// Returns the link to the TrackParticle */
    /*     ElementLink<xAOD::TrackParticleContainer> trackParticleLink() const; */
    /*     /// Sets the link to the TrackParticle */
    /*     void setTrackParticleLink(ElementLink<xAOD::TrackParticleContainer> trackParticleLink); */
    /// @}

  }; // end of the MeasurementStateOnSurface_v1 class definitions

} // end of the xAOD namespace

#endif // XAODTRACKING_VERSIONS_MEASUREMENTSTATEONSURFACE_V1_H
