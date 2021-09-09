/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TriggerSkimmingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//
// Direct skimming via TrigDecisionTool
// Only intended for cases where the triggers in question cannot be
// handled by ExpressionEvaluation (dashes, dots)

#include "DerivationFrameworkTools/TriggerSkimmingTool.h"

#include <vector>
#include <string>



namespace DerivationFramework {

  TriggerSkimmingTool::TriggerSkimmingTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" )
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty( "TrigDecisionTool", m_trigDec );
    declareProperty("TriggerListAND", m_triggerListAND);
    declareProperty("TriggerListOR", m_triggerListOR);
  }

  StatusCode TriggerSkimmingTool::initialize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerSkimmingTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  bool TriggerSkimmingTool::eventPassesFilter() const
  {
    std::vector<std::string>::const_iterator strItr;
    unsigned int cntrAND(0), cntrOR(0);
    for (strItr=m_triggerListAND.begin(); strItr!=m_triggerListAND.end(); ++strItr) {
        if (m_trigDec->isPassed(*strItr)) ++cntrAND;
    }
    for (strItr=m_triggerListOR.begin(); strItr!=m_triggerListOR.end(); ++strItr) {
        if (m_trigDec->isPassed(*strItr)) ++cntrOR;
    }
    bool passAND(false);
    bool passOR(false);
    if (cntrAND==m_triggerListAND.size() && !m_triggerListAND.empty()) passAND=true;
    if (cntrOR > 0 && !m_triggerListOR.empty()) passOR=true; 

    bool pass(false);
    pass = passAND || passOR;
    if (m_triggerListAND.empty()) pass = passOR;
    if (m_triggerListOR.empty()) pass = passAND;
    return pass;
  }  

}
