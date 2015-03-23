// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionCnvTool.h 582803 2014-02-12 15:58:23Z krasznaa $
#ifndef XAODTRIGGERCNV_TRIGDECISIONCNVTOOL_H
#define XAODTRIGGERCNV_TRIGDECISIONCNVTOOL_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
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

namespace xAODMaker {

   /**
    *  @short Implementation of the ITrigDecisionCnvTool interface
    *
    *         This tool copies the contents of the "old" TrigDec::TrigDecision
    *         object into an xAOD::TrigDecision one.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 582803 $
    * $Date: 2014-02-12 16:58:23 +0100 (Wed, 12 Feb 2014) $
    */
   class TrigDecisionCnvTool : public AthAlgTool,
                               public virtual ITrigDecisionCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigDecisionCnvTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function filling an xAOD::TrigDecision object with information
      virtual StatusCode convert( const TrigDec::TrigDecision* aod,
                                  xAOD::TrigDecision* xaod );

   private:
      /// Function setting the status of one bit in a bitset
      static StatusCode setBit( std::vector< uint32_t >& bitset,
                                uint32_t chainId, bool value = true );

      /// Connection to the TrigDecisionTool
      ToolHandle< Trig::ITrigDecisionTool > m_tdt;
      /// Connection to the HLT configuration
      ServiceHandle< TrigConf::IHLTConfigSvc > m_trigconf;

   }; // class TrigDecisionCnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGDECISIONCNVTOOL_H
