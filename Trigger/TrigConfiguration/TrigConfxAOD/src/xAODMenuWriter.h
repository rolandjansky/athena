// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMenuWriter.h 589118 2014-03-24 16:20:19Z krasznaa $
#ifndef TRIGCONFXAOD_XAODMENUWRITER_H
#define TRIGCONFXAOD_XAODMENUWRITER_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>
#include <set>
#include <map> // For std::pair...

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger include(s):
#include "TrigConfInterfaces/ITrigConfigSvc.h"

// EDM include(s):
#include "xAODTrigger/TriggerMenuContainer.h"

namespace TrigConf {

   /**
    *  @short Algorithm used to write the light-weight xAOD configuration
    *
    *         This algorithm needs to be scheduled in jobs that write xAOD
    *         files in Athena in order to write the trigger configuration
    *         into the output file.
    *
    *         It puts a tiny amount of information into each event that is
    *         later used to find the correct configuration of the events,
    *         and also assembles the configuration metadata that is written
    *         into the metadata TTree of the xAOD file at the end of the job.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 589118 $
    * $Date: 2014-03-24 17:20:19 +0100 (Mon, 24 Mar 2014) $
    */
   class xAODMenuWriter : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      xAODMenuWriter( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();

      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key for the event object
      std::string m_eventName;
      /// StoreGate key for the configuration object
      std::string m_metaName;
      /// Switch for overwriting existing event-wise objects
      bool m_overwriteEventObj;

      /// Connection to the trigger configuration service
      ServiceHandle< TrigConf::ITrigConfigSvc > m_trigConf;
      /// Connection to the metadata store
      ServiceHandle< StoreGateSvc > m_metaStore;

      /// The configuration object that we are writing
      xAOD::TriggerMenuContainer* m_tmc;

      /// Trigger configuration key type (used just internally)
      typedef std::pair< uint32_t, std::pair< uint32_t, uint32_t > > TrigKey_t;

      /// Trigger configuration keys that are already converted
      std::set< TrigKey_t > m_convertedKeys;

   }; // class xAODMenuWriter

} // namespace TrigConf

#endif // TRIGCONFXAOD_XAODMENUWRITER_H
