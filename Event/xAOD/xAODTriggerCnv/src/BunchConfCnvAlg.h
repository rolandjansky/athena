// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfCnvAlg.h 583381 2014-02-14 15:58:20Z krasznaa $
#ifndef XAODTRIGGERCNV_BUNCHCONFCNVALG_H
#define XAODTRIGGERCNV_BUNCHCONFCNVALG_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>
#include <set>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingConfProvider.h"

// xAOD include(s):
#include "xAODTrigger/BunchConfContainer.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for writing bunch configuration metadata
    *
    *         This algorithm can be used to write ROOT-readable bunch
    *         configuration metadata into an xAOD file.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 583381 $
    * $Date: 2014-02-14 16:58:20 +0100 (Fri, 14 Feb 2014) $
    */
   class BunchConfCnvAlg : public AthAlgorithm {

   public:
      /// Regular algorithm constructor
      BunchConfCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key for the event object
      std::string m_eventName;
      /// StoreGate key for the configuration object
      std::string m_metaName;

      /// Connection to the bunch configuration provider tool
      ToolHandle< Trig::IBunchCrossingConfProvider > m_bcConf;
      /// Connection to the metadata store
      ServiceHandle< StoreGateSvc > m_metaStore;

      /// The configuration object that is being written
      xAOD::BunchConfContainer* m_bcc;

      /// Configurations that have already been converted
      std::set< uint32_t > m_convertedIDs;

   }; // class BunchConfCnvAlg

} // xAODMaker

#endif // XAODTRIGGERCNV_BUNCHCONFCNVALG_H
