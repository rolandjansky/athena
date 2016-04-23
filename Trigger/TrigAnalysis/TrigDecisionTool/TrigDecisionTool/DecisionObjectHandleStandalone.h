// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TrigDecisionTool_DecisionObjectHandleStandalone_h
#define TrigDecisionTool_DecisionObjectHandleStandalone_h

// System include(s):
#include <string>

// ASG include(s):
#include "AsgTools/AsgToolsConf.h"

#include "AsgTools/AsgMessaging.h"

// xAOD include(s):
#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigNavigation.h"

// Local include(s):
#include "TrigDecisionTool/DecisionObjectHandle.h"
#include "TrigDecisionTool/EventPtrDef.h"


namespace Trig {

   /**
    * @brief Decision invalidator for ROOT (really doing the job)
    */
   class DecisionObjectHandleStandalone :
    public DecisionObjectHandle< xAOD::TrigDecision, xAOD::TrigNavigation >{

   public:
      /// Constructor with arguments
      DecisionObjectHandleStandalone( EventPtr_t sg, const std::string& deckey,
                                      const std::string& navikey );

      /// Reset the object at the end of an event
      virtual void reset();
      /// Retrieve the decision object
      virtual xAOD::TrigDecision const * getDecision() const;
      /// Retrieve the navigation object
      virtual xAOD::TrigNavigation const * getNavigation() const;

   private:
      /// Pointer to the event store
      EventPtr_t m_sg;
      /// Key of the trigger decision object in the event
      const std::string m_deckey;
      /// Key of the trigger navigation object in the event
      const std::string m_navikey;
      /// Cached pointer to the trigger decision object
      mutable xAOD::TrigDecision const * m_decision;
      /// Cached pointer to the trigger navigation object
      mutable xAOD::TrigNavigation const * m_navigation;

   }; // class DecisionObjectHandleStandalone

} // namespace Trig

#endif // TrigDecisionTool_DecisionObjectHandleStandalone_h
