/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_TRACKMEASUREMENTVALIDATION_V1_H
#define XAODTRACKING_VERSIONS_TRACKMEASUREMENTVALIDATION_V1_H

// System include(s):
#include <vector>

// Core include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Class describing a TrackMeasurementValidation.
  ///
  /// @author Niels Van Eldik <niels.van.eldik@cern.ch>
  /// @author Edward Moyse <Edward.Moyse@cern.ch>
  /// @author Heberth Torres <htorres@cern.ch>
  /// @nosubgrouping
  ///
  class TrackMeasurementValidation_v1 : public SG::AuxElement {

  public:
    
    /// Default constructor
    TrackMeasurementValidation_v1();

    /// @name Identification 
    /// SOME INFO SHOULD GO HERE
    /// @{
    /// Returns the identifier
    uint64_t identifier() const;
    /// Returns the list of RDO identifiers
    const std::vector< uint64_t >& rdoIdentifierList() const;
    /// Sets the identifier
    void setIdentifier(uint64_t identifier);
    /// Sets the list of RDO identifiers
    void setRdoIdentifierList(const std::vector< uint64_t >& rdoIdentifierList);
    /// @}

    /// @name Local position functions
    /// Returns the local position
    /// @{
    /// Returns the x position
    float localX() const;
    /// Returns the y position
    float localY() const;
    /// Returns the x position error
    float localXError() const;
    /// Returns the y position error
    float localYError() const;
    /// Returns the xy position correlation
    float localXYCorrelation() const;
    /// Sets the local position
    void setLocalPosition(float localX, float localY);
    /// Sets the local position error
    void setLocalPositionError(float localXError, float localYError, float localXYCorrelation);
    /// @}

    /// @name Global position functions
    /// Returns the global position
    /// @{
    /// Returns the x position
    float globalX() const;
    /// Returns the y position
    float globalY() const;
    /// Returns the z position
    float globalZ() const;
    /// Sets the global position
    void setGlobalPosition(float globalX, float globalY, float globalZ);
    /// @}


  }; // end of the TrackMeasurementValidation_v1 class definitions

} // end of the xAOD namespace

#endif // XAODTRACKING_VERSIONS_TRACKMEASUREMENTVALIDATION_V1_H
