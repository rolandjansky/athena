/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Emerging jets
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EMERGINGJETSFILTERTOOL_H
#define DERIVATIONFRAMEWORK_EMERGINGJETSFILTERTOOL_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

namespace DerivationFramework {

  /** @class EmergingJetsFilterTool
  
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
  **/
  
  class EmergingJetsFilterTool : public AthAlgTool, public ISkimmingTool {
    
  public: 
    /** Constructor with parameters */
    EmergingJetsFilterTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~EmergingJetsFilterTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    ToolHandle< Trig::TrigDecisionTool > m_tdt;
    std::vector< std::string > m_triggers;
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    mutable unsigned int m_nptpass;
    std::string m_jetSGKey;

    double m_ptCut;
    double m_etaCut;
    int m_nJetsRequired;
    
  }; 
  
}


#endif
