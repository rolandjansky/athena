/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolEXOT20.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SKIMMINGTOOLEXOT20_H
#define DERIVATIONFRAMEWORK_SKIMMINGTOOLEXOT20_H 1

#include <string>
#include <vector>
#include <algorithm>

// Gaudi & Athena basics
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trk {
  class IExtrapolator;
}

namespace DerivationFramework {

  class SkimmingToolEXOT20 : public AthAlgTool, public ISkimmingTool {
    
  public: 
    SkimmingToolEXOT20( const std::string& t, const std::string& n, const IInterface* p );
    
    ~SkimmingToolEXOT20();
    
    StatusCode  initialize();
    StatusCode  finalize();
    
    virtual bool eventPassesFilter() const;
   
  private:

    ////////////////
    ///// TOOLS
    ToolHandle<Trig::TrigDecisionTool>    m_trigDecisionTool;
    ToolHandle<Trk::IExtrapolator>        m_extrapolator;

    /////////////////////////
    ///// CUT VALUES/SETTINGS
    std::string                       m_MSTPSGKey;
    std::vector<std::string>          m_triggers;
    double                            m_MinPtMSTP;
    int                               m_MinNumMSTP;
    int                               m_direction;
    int                               m_particleType;

    ////////////////
    ///// FUNCTIONS
    bool  SubcutTrigger() const;
    bool  SubcutSelect() const;
    std::string TriggerVarName(std::string s) const;

    ///////////////
    ///// COUNTERS
    mutable unsigned int              n_tot;
    mutable unsigned int              n_pass;
    mutable unsigned int              n_passTrigger;

    /////////////////////////////
    ///// EVENT LEVEL QUANTITIES
    mutable bool                      e_passTrigger;

  }; 
  
}

#endif 
