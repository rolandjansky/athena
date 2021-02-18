/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Trackless jets
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_DVTRACKLESSJETFILTERTOOL_H
#define DERIVATIONFRAMEWORK_DVTRACKLESSJETFILTERTOOL_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/ReadHandleKey.h"

namespace DerivationFramework {

  /** @class DVTracklessJetFilterTool
  
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class DVTracklessJetFilterTool : public AthAlgTool, public ISkimmingTool {
    
  public: 
    /** Constructor with parameters */
    DVTracklessJetFilterTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~DVTracklessJetFilterTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    mutable std::atomic<unsigned int> m_ntot;
    mutable std::atomic<unsigned int> m_npass;
    SG::ReadHandleKey<xAOD::JetContainer> m_jetSGKey
      { this, "JetContainerKey", "AntiKt4EMTopoJets", ""};

    double m_ptCut;
    double m_etaCut;
    double m_sumPtTrkCut;
    int m_nJetsRequired;
    
  }; 
  
}


#endif
