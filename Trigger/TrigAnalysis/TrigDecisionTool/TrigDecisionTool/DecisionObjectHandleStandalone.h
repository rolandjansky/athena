// -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TrigDecisionTool_DecisionObjectHandleStandalone_h
#define TrigDecisionTool_DecisionObjectHandleStandalone_h

// System include(s):
#include <string>

// ASG include(s):
#include "AsgTools/AsgMessaging.h"

// xAOD include(s):
#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigNavigation.h"

// Local include(s):
#include "TrigDecisionTool/DecisionObjectHandle.h"
#include "TrigDecisionTool/EventPtrDef.h"

#include "AsgDataHandles/ReadHandleKey.h"


namespace Trig {

   /**
    * @brief Decision invalidator for ROOT (really doing the job)
    */
   class DecisionObjectHandleStandalone :
    public DecisionObjectHandle< xAOD::TrigDecision, xAOD::TrigNavigation >{

   public:
      /// Constructor with arguments
      DecisionObjectHandleStandalone( SG::ReadHandleKey<xAOD::TrigDecision>* deckey,
                                      SG::ReadHandleKey<xAOD::TrigNavigation>* navikey );

      /// Reset the object at the end of an event
      virtual void reset(bool hard = false);
      /// Retrieve the decision object
      virtual xAOD::TrigDecision const * getDecision() const override;
      /// Retrieve the navigation object
      virtual xAOD::TrigNavigation const * getNavigation() const override;

   private:
      /// Key of the trigger decision object in the event
      SG::ReadHandleKey<xAOD::TrigDecision>* m_deckey;
      /// Key of the trigger navigation object in the event
      SG::ReadHandleKey<xAOD::TrigNavigation>* m_navikey;
      /// Cached pointer to the trigger decision object
      mutable xAOD::TrigDecision const * m_decision;
      /// Cached pointer to the trigger navigation object
      mutable xAOD::TrigNavigation const * m_navigation;

   }; // class DecisionObjectHandleStandalone

} // namespace Trig

#endif // TrigDecisionTool_DecisionObjectHandleStandalone_h
