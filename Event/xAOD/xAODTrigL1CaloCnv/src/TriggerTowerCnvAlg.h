// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerCnvAlg.h 576327 2013-12-19 16:08:56Z morrisj $
#ifndef XAODTRIGL1CALOCNV_TRIGGERTOWERCNVALG_H
#define XAODTRIGL1CALOCNV_TRIGGERTOWERCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/ITriggerTowerCnvTool.h"

namespace xAODMaker {
   /**
    *  @short Algorithm for creating xAOD::TriggerTower from an ESD
    *
    *         This algorithm can be used to create an xAOD TriggerTower container
    *         out of the objects found in an ESD file.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576327 $
    * $Date: 2013-12-19 17:08:56 +0100 (Thu, 19 Dec 2013) $
    */
   class TriggerTowerCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      TriggerTowerCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key of the input container
      std::string m_esdKey;
      /// StoreGate key for the output container
      std::string m_xaodKey;

      /// Handle to the converter tool
      ToolHandle< ITriggerTowerCnvTool > m_cnvTool;

   }; // class EmTauRoICnvAlg

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_TRIGGERTOWERCNVALG_H
