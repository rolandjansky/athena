/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGERCNV_TRIGDECISIONCNVTOOL_H
#define XAODTRIGGERCNV_TRIGDECISIONCNVTOOL_H

// System include(s):
#include <vector>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Trigger include(s):
#include "TrigDecisionInterface/ITrigDecisionTool.h"
#include "TrigConfInterfaces/IHLTConfigSvc.h"

// Local include(s):
#include "xAODTriggerCnv/ITrigDecisionCnvTool.h"
#include "EventInfo/TriggerInfo.h"

namespace xAODMaker {

   /**
    *  @short Implementation of the ITrigDecisionCnvTool interface
    *
    *         This tool copies the contents of the "old" TrigDec::TrigDecision
    *         object into an xAOD::TrigDecision one.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    */
   class TrigDecisionCnvTool : public AthAlgTool,
                               public virtual ITrigDecisionCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigDecisionCnvTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      virtual ~TrigDecisionCnvTool();

      /// Function initialising the tool
      virtual StatusCode initialize() override;

      /// Function filling an xAOD::TrigDecision object with information
      virtual StatusCode convert( const TrigDec::TrigDecision* aod,
                                  xAOD::TrigDecision* xaod,
                                  const TriggerInfo* level1TriggerInfo ) const final;

   private:
      /// Function setting the status of one bit in a bitset
      static StatusCode setBit( std::vector< uint32_t >& bitset,
                                uint32_t chainId, bool value = true );

      /// Connection to the TrigDecisionTool
      PublicToolHandle< Trig::ITrigDecisionTool > m_tdt{this, "TrigDecisionTool",
                                                        "Trig::TrigDecisionTool/TrigDecisionTool"};
      /// Connection to the HLT configuration
      ServiceHandle< TrigConf::IHLTConfigSvc > m_trigconf{this, "TrigConfigSvc",
                                                         "TrigConf::xAODConfigSvc/xAODConfigSvc"};

   }; // class TrigDecisionCnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGDECISIONCNVTOOL_H
