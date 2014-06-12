/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NTUPSkimmingToolExample.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_NTUPSKIMMINGTOOLEXAMPLE_H
#define DERIVATIONFRAMEWORK_NTUPSKIMMINGTOOLEXAMPLE_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

namespace DerivationFramework {

  /** @class NTUPSkimmingToolExample
  
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class NTUPSkimmingToolExample : public AthAlgTool, public ISkimmingTool {
    
  public: 
    /** Constructor with parameters */
    NTUPSkimmingToolExample( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~NTUPSkimmingToolExample();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    std::string m_mu_n_name;
    std::string m_mu_isComb_name;
    std::string m_mu_pt_name;
    std::vector<float>* mu_pt;
    std::vector<int >* mu_isComb;
    unsigned int m_nMuons;
    double m_muonPtCut;

  }; 
  
}


#endif
