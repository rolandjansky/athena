/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Emerging jets
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EMERGINGJETSHIGHPTFILTERTOOL_H
#define DERIVATIONFRAMEWORK_EMERGINGJETSHIGHPTFILTERTOOL_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/ReadHandleKey.h"

namespace DerivationFramework {

  /** @class EmergingJetsHighPtFilterTool
  
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
  **/
  
  class EmergingJetsHighPtFilterTool : public AthAlgTool, public ISkimmingTool {
    
  public: 
    /** Constructor with parameters */
    EmergingJetsHighPtFilterTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~EmergingJetsHighPtFilterTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    ToolHandle< Trig::TrigDecisionTool > m_tdt;
    std::vector< std::string > m_triggers;
    mutable std::atomic<unsigned int> m_ntot;
    mutable std::atomic<unsigned int> m_npass;
    mutable std::atomic<unsigned int> m_nptpass;
    mutable std::atomic<unsigned int> m_nhighptpass;
    SG::ReadHandleKey<xAOD::JetContainer> m_jetSGKey { this, "JetContainerKey", "AntiKt4EMTopoJets", ""};

    double m_ptCut;
    double m_highPtCut;
    double m_etaCut;
    int m_nJetsRequired;
    int m_nHighPtJetsRequired;
    
  }; 
  
}


#endif
