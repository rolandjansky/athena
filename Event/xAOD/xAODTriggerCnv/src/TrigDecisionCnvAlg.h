// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionCnvAlg.h 578517 2014-01-15 13:48:04Z krasznaa $
#ifndef XAODTRIGGERCNV_TRIGDECISIONCNVALG_H
#define XAODTRIGGERCNV_TRIGDECISIONCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "TrigDecisionEvent/TrigDecision.h"
#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigDecisionAuxInfo.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"

// Local include(s):
#include "xAODTriggerCnv/ITrigDecisionCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for creating xAOD::TrigDecision objects
    *
    *         This algorithm can be used to translate existing
    *         TrigDec::TrigDecision objects into xAOD::TrigDecision ones.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 578517 $
    * $Date: 2014-01-15 14:48:04 +0100 (Wed, 15 Jan 2014) $
    */
   class TrigDecisionCnvAlg : public AthReentrantAlgorithm {

   public:
      /// Regular Algorithm constructor
      TrigDecisionCnvAlg( const std::string& name, ISvcLocator* svcLoc );
      virtual ~TrigDecisionCnvAlg();

      /// Function initialising the algorithm
      virtual StatusCode initialize() override;
      /// Function executing the algorithm
      virtual StatusCode execute(const EventContext& ctx) const override;

   private:

      SG::ReadHandleKey<EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo", "StoreGate key of the legacy event info object"};
      SG::ReadHandleKey<TrigDec::TrigDecision> m_aodKey{this, "AODKey", "TrigDecision", "StoreGate key of the input object"};
      SG::WriteHandleKey<xAOD::TrigDecision> m_xaodKey{this, "xAODKey", "xTrigDecision", "StoreGate key for the output object"};

      ToolHandle< ITrigDecisionCnvTool > m_cnvTool{this, "CnvTool", "xAODMaker::TrigDecisionCnvTool/TrigDecisionCnvTool", "Handle to the converter tool"};

   }; // class TrigDecisionCnvAlg

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGDECISIONCNVALG_H
