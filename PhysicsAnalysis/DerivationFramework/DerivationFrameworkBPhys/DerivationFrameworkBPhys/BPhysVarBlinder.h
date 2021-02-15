/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///
/// @file   BPhysVarBlinder.h
/// @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
///
/// @brief  Vertex variable(s) blinding tool
///
#ifndef DERIVATIONFRAMEWORK_BPhysVarBlinder_H
#define DERIVATIONFRAMEWORK_BPhysVarBlinder_H

#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "DerivationFrameworkBPhys/CfAthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

///
/// forward declarations
///
namespace xAOD {
  class BPhysBlindingTool;
}
namespace DerivationFramework {

  ///
  /// @class  BPhysVarBlinder
  /// @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
  ///
  /// @ brief Vertex variable(s) blinding tool
  ///
  /// This is an AthAlgTool wrapper around the BPhysBlindingTool
  /// from the package BPhysTools.
  ///
  /// Job options:
  /// - BlindingTool   : ToolHandle for xAOD::BPhysBlindingTool
  /// - EnableBlinding : Switch to easily en-/disable this tool 
  ///
  /// For an example configuration using this tool see BPHY8.py.
  ///
  class BPhysVarBlinder : public CfAthAlgTool, public IAugmentationTool {

  public: 
    ///
    /// @brief Constructor
    ///
    BPhysVarBlinder(const std::string& t, const std::string& n,
                    const IInterface* p);
    ///
    /// @brief Initialization
    /// 
    StatusCode initialize();
    ///
    /// @brief Finalization
    ///
    StatusCode finalize();
    ///
    /// @brief Perform blinding per event (if enabled)
    ///
    virtual StatusCode addBranches() const;
      
  private:
    ///
    /// @name Job options
    /// @{
    ///
    /// @brief ToolHandle for blinding tool
    ToolHandle<xAOD::BPhysBlindingTool> m_blindingTool;
    ///
    /// @brief Switch for enabling blinding 
    bool   m_enableBlinding;
    /// @}
    
  }; 
}
#endif // DERIVATIONFRAMEWORK_BPhysVarBlinder_H
