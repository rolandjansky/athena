/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolExample.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_HVJETMETFILTERTOOL_H
#define DERIVATIONFRAMEWORK_HVJETMETFILTERTOOL_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "xAODMissingET/MissingETContainer.h"
#include "StoreGate/ReadHandleKey.h"

namespace DerivationFramework {

  /** @class HVJetMETFilterTool
  
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class HVJetMETFilterTool : public AthAlgTool, public ISkimmingTool {
    
  public: 
    /** Constructor with parameters */
    HVJetMETFilterTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~HVJetMETFilterTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    mutable std::atomic<unsigned int> m_ntot;
    mutable std::atomic<unsigned int> m_npass;
    SG::ReadHandleKey<xAOD::MissingETContainer> m_metSGKey
      { this, "METContainerKey", "MET_RefFinal", ""};
    std::string m_metTerm;
    double m_metCut;

  }; 
  
}


#endif
