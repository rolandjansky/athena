// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoCnvAlg.h 751296 2016-06-01 08:00:25Z krasznaa $
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
    *
    * $Revision: 751296 $
    * $Date: 2016-06-01 10:00:25 +0200 (Wed, 01 Jun 2016) $
    */
   class EventInfoCnvAlg : public AthReentrantAlgorithm {

   public:
      /// Regular Algorithm constructor
      EventInfoCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize() override;
      /// Function executing the algorithm
      virtual StatusCode execute (const EventContext& ctx) const override;

      /// Function executed at the beginning of the job, once the first event
      /// from the input file is already open
      virtual StatusCode beginRun() override;

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

      /// Do BeginRun processing?
      bool m_doBeginRun;

   }; // class EventInfoCnvAlg

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_EVENTINFOCNVALG_H
