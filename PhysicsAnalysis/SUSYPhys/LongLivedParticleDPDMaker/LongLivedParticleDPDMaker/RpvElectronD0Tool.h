/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// RpvElectronD0Tool.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_RPVELECTROND0TOOL_H
#define DERIVATIONFRAMEWORK_RPVELECTROND0TOOL_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "StoreGate/ReadHandleKey.h"

namespace DerivationFramework {

  /** @class RpvElectronD0Tool
 
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author Michael Flowerdew -at- cern.ch
     */
  class RpvElectronD0Tool : public AthAlgTool, public IAugmentationTool {
   
  public: 
    /** Constructor with parameters */
    RpvElectronD0Tool( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~RpvElectronD0Tool();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    SG::ReadHandleKey<xAOD::ElectronContainer> m_collName { this, "ElectronContainerKey", "Electrons", ""};
    std::string m_sgPrefix;

  }; 
 
}

#endif
