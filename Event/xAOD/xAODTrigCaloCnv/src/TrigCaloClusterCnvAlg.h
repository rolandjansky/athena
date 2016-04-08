// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterCnvAlg.h 592334 2014-04-10 11:01:56Z krasznaa $
#ifndef XAODTRIGCALOCNV_TRIGCALOCLUSTERCNVALG_H
#define XAODTRIGCALOCNV_TRIGCALOCLUSTERCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigCaloCnv/ITrigCaloClusterCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for creating xAOD::TrigCaloCluster from an AOD
    *
    *         This algorithm can be used to create an xAOD TrigCaloCluster
    *         out of the object found in an AOD file.
    *
    * @author Arantxa Ruiz Martinez <aranzazu.ruiz.martinez@cern.ch>
    *
    * $Revision: 592334 $
    * $Date: 2014-04-10 13:01:56 +0200 (Thu, 10 Apr 2014) $
    */
   class TrigCaloClusterCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      TrigCaloClusterCnvAlg( const std::string& name, ISvcLocator* svcLoc );

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
      ToolHandle< ITrigCaloClusterCnvTool > m_cnvTool;

   }; // class TrigCaloClusterCnvAlg

} // namespace xAODMaker

#endif // XAODTRIGCALOCNV_TRIGCALOCLUSTERCNVALG_H
