/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinkTrkSingleJetFilterTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_KINKTRKSINGLEJETMETFILTERTOOL_H
#define DERIVATIONFRAMEWORK_KINKTRKSINGLEJETMETFILTERTOOL_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"


namespace DerivationFramework {
  class KinkTrkSingleJetMetFilterTool : public AthAlgTool, public ISkimmingTool {

  public: 
    /** Constructor with parameters */
    KinkTrkSingleJetMetFilterTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~KinkTrkSingleJetMetFilterTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    bool m_passAll;
    bool m_LeptonVeto;
    std::string m_jetSGKey;
    std::string m_metSGKey;
    std::string m_metTerm;
    std::string m_muonSGKey;
    std::string m_muonIDKey;
    std::string m_electronSGKey;
    std::string m_electronIDKey;
    float m_metCut;
    float m_jetPtCut;
    float m_jetEtaMax;
    float m_leptonPtCut;
    float m_leptonEtaMax;

  }; 
  
}


#endif
