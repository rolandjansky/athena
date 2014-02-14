// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoICnvAlg.h 575635 2013-12-16 10:58:41Z krasznaa $
#ifndef XAODTRIGGERCNV_ENERGYSUMROICNVALG_H
#define XAODTRIGGERCNV_ENERGYSUMROICNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTriggerCnv/IEnergySumRoICnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for creating xAOD::EnergySumRoI from an AOD
    *
    *         This algorithm can be used to create an xAOD energy-sum RoI
    *         out of the object found in an AOD file.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 575635 $
    * $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $
    */
   class EnergySumRoICnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      EnergySumRoICnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key of the input container
      std::string m_aodKey;
      /// StoreGate key for the output container
      std::string m_xaodKey;

      /// Handle to the converter tool
      ToolHandle< IEnergySumRoICnvTool > m_cnvTool;

   }; // class EnergySumRoICnvAlg

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_ENERGYSUMROICNVALG_H
