// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionCnvAlg.h 578517 2014-01-15 13:48:04Z krasznaa $
#ifndef XAODTRIGGERCNV_TRIGDECISIONCNVALG_H
#define XAODTRIGGERCNV_TRIGDECISIONCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

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
   class TrigDecisionCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      TrigDecisionCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key of the input object
      std::string m_aodKey;
      /// StoreGate key for the output object
      std::string m_xaodKey;

      /// Handle to the converter tool
      ToolHandle< ITrigDecisionCnvTool > m_cnvTool;

   }; // class TrigDecisionCnvAlg

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGDECISIONCNVALG_H
