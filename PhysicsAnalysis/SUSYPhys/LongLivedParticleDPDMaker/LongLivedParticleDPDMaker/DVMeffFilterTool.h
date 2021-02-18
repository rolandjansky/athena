/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolExample.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_DVMEFFFILTERTOOL_H
#define DERIVATIONFRAMEWORK_DVMEFFFILTERTOOL_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"

namespace DerivationFramework {

  /** @class DVMeffFilterTool
  
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class DVMeffFilterTool : public AthAlgTool, public ISkimmingTool {
    
  public: 
    /** Constructor with parameters */
    DVMeffFilterTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~DVMeffFilterTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    mutable std::atomic<unsigned int> m_ntot;
    mutable std::atomic<unsigned int> m_npass;
    SG::ReadHandleKey<xAOD::MissingETContainer> m_metSGKey { this, "MetContainerKey", "MET_Calo", ""};
    SG::ReadHandleKey<xAOD::JetContainer> m_jetSGKey { this, "JetContainerKey", "AntiKt4LCTopoJets", ""};
    double m_MeffCut;
    double m_METoverMeffCutMin;
    double m_METoverMeffCutMax;
    double m_jetPtCut;
    double m_jetEtaCut;
    double m_METCut;

  }; 
  
}


#endif
