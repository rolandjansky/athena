// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IBPhysBlindingTool.h
 * @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
 *
 * @brief  Interface for dual-use tool for (un-)blinding of float values.
 */

#ifndef BPHYSTOOLS_IBPHYSBLINDINGTOOL_H
#define BPHYSTOOLS_IBPHYSBLINDINGTOOL_H

// Framework includes
#include "AsgTools/IAsgTool.h"

// System include(s):
#include <string>
#include <vector>

// EDM includes
#include "xAODTracking/VertexContainer.h"

namespace xAOD {
  ///
  /// Interface for dual-use tool for blinding and unblinding
  /// certain float values provided as variables in a container. 
  ///
  /// @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
  ///
  ///
  class IBPhysBlindingTool : virtual public asg::IAsgTool {

  public:
    /// Declare the correct interface for Athena
    ASG_TOOL_INTERFACE( xAOD::IBPhysBlindingTool )

    /// @ brief Function finalizing the tool
    virtual StatusCode finalize() = 0;

    ///
    /// @name Methods to be called by user classes
    /// @{
    ///
    /// @brief Simply blind one positive float value
    virtual float doBlind(const float& val) = 0;

    ///
    /// @brief Simply unblind one positive float value
    virtual float doUnblind(const float& val) = 0;
    
    ///
    /// @brief Simply blind one (positive) float value with corretions
    virtual float doBlind(const float& val, const bool& negativeSign,
                          const float& offset, const float& factor) = 0;

    ///
    /// @brief Simply unblind one (positive) float value with corrections
    virtual float doUnblind(const float& val, const bool& negativeSign,
                            const float& offset, const float& factor) = 0;

    /// 
    /// @brief Perform blinding of requested variables
    virtual StatusCode doBlind() = 0;

    /// 
    /// @brief Perform unblinding of requested variables
    virtual StatusCode doUnblind() = 0;
    
    /// @}

  }; // class IBPhysBlindingTool

} // namespace xAOD

#endif // BPHYSTOOLS_IBPHYSBLINDINGTOOL_H
