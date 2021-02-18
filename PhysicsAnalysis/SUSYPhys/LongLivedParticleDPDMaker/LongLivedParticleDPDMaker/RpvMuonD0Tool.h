/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// RpvMuonD0Tool.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_RPVMUOND0TOOL_H
#define DERIVATIONFRAMEWORK_RPVMUOND0TOOL_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODMuon/MuonContainer.h"
#include "StoreGate/ReadHandleKey.h"

namespace DerivationFramework {

  /** @class RpvMuonD0Tool
 
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author Michael Flowerdew -at- cern.ch
     */
  class RpvMuonD0Tool : public AthAlgTool, public IAugmentationTool {
   
  public: 
    /** Constructor with parameters */
    RpvMuonD0Tool( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~RpvMuonD0Tool();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    SG::ReadHandleKey<xAOD::MuonContainer> m_collName { this, "MuonContainerKey", "Muons", ""};
    std::string m_sgPrefix;

  }; 
 
}

#endif
