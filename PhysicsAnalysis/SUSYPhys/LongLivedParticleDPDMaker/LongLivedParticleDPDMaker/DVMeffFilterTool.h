/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    std::string m_metSGKey;
    std::string m_jetSGKey; 
    double m_MeffCut;
    double m_METoverMeffCutMin;
    double m_METoverMeffCutMax;
    double m_jetPtCut;
    double m_jetEtaCut;
    double m_METCut;

  }; 
  
}


#endif
