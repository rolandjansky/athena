// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoCnvAlg.h 577605 2014-01-09 16:47:58Z krasznaa $
#ifndef XAODEVENTINFOCNV_EVENTINFOCNVALG_H
#define XAODEVENTINFOCNV_EVENTINFOCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODEventInfoCnv/IEventInfoCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for creating xAOD::EventInfo objects for the xAOD
    *
    *         This algorithm can be used to translate an EventInfo object
    *         from an input (AOD) POOL file into an xAOD::EventInfo object.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 577605 $
    * $Date: 2014-01-09 17:47:58 +0100 (Thu, 09 Jan 2014) $
    */
   class EventInfoCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      EventInfoCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key for the input object
      std::string m_aodKey;
      /// StoreGate key for the output object
      std::string m_xaodKey;

      /// Handle to the converter tool
      ToolHandle< IEventInfoCnvTool > m_cnvTool;

   }; // class EventInfoCnvAlg

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_EVENTINFOCNVALG_H
