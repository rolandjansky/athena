/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// RpvEgammaIDTool.h  (c) ATLAS Detector software
// Straight copy of DerivationFrameworkEGamma/EGammaPassSelectionWrapper.h
// because DerivationFrameworkEGamma is not in AtlasProduction
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGAMMAPASSSELECTONWRAPPER_H
#define DERIVATIONFRAMEWORK_EGAMMAPASSSELECTONWRAPPER_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODEgamma/EgammaContainer.h"
#include "StoreGate/ReadHandleKey.h"

namespace DerivationFramework {

  /** @class RpvEgammaIDTool
 
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class RpvEgammaIDTool : public AthAlgTool, public IAugmentationTool {
   
  public:
    /** Constructor with parameters */
    RpvEgammaIDTool( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~RpvEgammaIDTool();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    std::vector<std::string> m_qualFlags;
    SG::ReadHandleKey<xAOD::EgammaContainer> m_collName { this, "CollectionName", "ElectronCollection", ""};
    std::string m_sgPrefix;

  };
 
}

#endif
