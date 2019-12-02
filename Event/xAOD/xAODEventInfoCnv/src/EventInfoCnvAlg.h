/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEVENTINFOCNV_EVENTINFOCNVALG_H
#define XAODEVENTINFOCNV_EVENTINFOCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODCnvInterfaces/IEventInfoCnvTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "EventInfo/EventInfo.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for creating xAOD::EventInfo objects for the xAOD
    *
    *         This algorithm can be used to translate an EventInfo object
    *         from an input (AOD) POOL file into an xAOD::EventInfo object.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    */
   class EventInfoCnvAlg : public AthReentrantAlgorithm {

   public:
      /// Regular Algorithm constructor
      EventInfoCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize() override;
      /// Function executing the algorithm
      virtual StatusCode execute (const EventContext& ctx) const override;

   private:
      /// Key for the input object
      /// If blank, we do a keyless retrieve from SG instead!
      SG::ReadHandleKey<EventInfo> m_aodKey;
      /// Key for the output object
      SG::WriteHandleKey<xAOD::EventInfo> m_xaodKey;

      /// For pileup.
      SG::WriteHandleKey<xAOD::EventInfoContainer> m_pileupKey;

      /// Handle to the converter tool
      ToolHandle< IEventInfoCnvTool > m_cnvTool;

   }; // class EventInfoCnvAlg

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_EVENTINFOCNVALG_H
