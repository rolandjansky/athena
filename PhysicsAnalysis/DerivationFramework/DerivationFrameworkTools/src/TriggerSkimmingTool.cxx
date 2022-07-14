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
    AthAlgTool(t,n,p) {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
   
  }

  StatusCode TriggerSkimmingTool::initialize()
  {
    ATH_CHECK(m_trigDec.retrieve());
    return StatusCode::SUCCESS;
  }



  bool TriggerSkimmingTool::eventPassesFilter() const
  {
    
    if (msgLvl(MSG::VERBOSE)){
       const Trig::ChainGroup* Chain = m_trigDec->getChainGroup(".*");
       const std::vector<std::string> fired_triggers = Chain->getListOfTriggers();
       for (const std::string& fired : fired_triggers) {
         if (m_trigDec->isPassed(fired)) ATH_MSG_VERBOSE("Fired trigger "<<fired);
      }
    }
   
    unsigned int cntrAND{0}, cntrOR{0};
    for (const std::string& trig_and : m_triggerListAND) {
        ATH_MSG_DEBUG("AND - Trigger "<<trig_and<<" passed "<<m_trigDec->isPassed(trig_and));        
        cntrAND+=(m_trigDec->isPassed(trig_and));
    }
    for (const std::string& trig_or : m_triggerListOR) {
        ATH_MSG_DEBUG("OR - Trigger "<<trig_or<<" passed "<<m_trigDec->isPassed(trig_or));
        cntrOR +=(m_trigDec->isPassed(trig_or));
    }
    bool passAND = cntrAND==m_triggerListAND.size() && !m_triggerListAND.empty();
    bool passOR = cntrOR > 0;   
    return passAND || passOR;    
  }  

}
