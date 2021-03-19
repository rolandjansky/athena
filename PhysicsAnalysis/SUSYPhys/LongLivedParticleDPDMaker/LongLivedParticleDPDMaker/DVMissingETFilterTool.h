/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolExample.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_DVMISSINGETFILTERTOOL_H
#define DERIVATIONFRAMEWORK_DVMISSINGETFILTERTOOL_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/ReadHandleKey.h"

namespace DerivationFramework {

  /** @class DVMissingETFilterTool
  
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class DVMissingETFilterTool : public AthAlgTool, public ISkimmingTool {
    
  public: 
    /** Constructor with parameters */
    DVMissingETFilterTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~DVMissingETFilterTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    mutable std::atomic<unsigned int> m_ntot;
    mutable std::atomic<unsigned int> m_npass;
    SG::ReadHandleKey<xAOD::MissingETContainer> m_metSGKey { this, "METContainerKey", "MET_LocHadTopo", ""};

    double m_metCut;
    double m_jetPtCut;
    bool m_applyDeltaPhiCut;
    double m_deltaPhiCut;
    SG::ReadHandleKey<xAOD::JetContainer> m_jetSGKey 
        { this, "JetContainerKey", "AntiKt4LCTopoJets", ""}; // only needed if we want to cut on dphi(jet, MET)

  }; 
  
}


#endif
