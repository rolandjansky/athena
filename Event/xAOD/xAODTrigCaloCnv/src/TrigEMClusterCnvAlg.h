// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterCnvAlg.h 592334 2014-04-10 11:01:56Z krasznaa $
#ifndef XAODTRIGCALOCNV_TRIGEMCLUSTERCNVALG_H
#define XAODTRIGCALOCNV_TRIGEMCLUSTERCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigCaloCnv/ITrigEMClusterCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for creating xAOD::TrigEMCluster from an AOD
    *
    *         This algorithm can be used to create an xAOD TrigEMCluster
    *         out of the object found in an AOD file.
    *
    * @author Arantxa Ruiz Martinez <aranzazu.ruiz.martinez@cern.ch>
    *
    * $Revision: 592334 $
    * $Date: 2014-04-10 13:01:56 +0200 (Thu, 10 Apr 2014) $
    */
   class TrigEMClusterCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      TrigEMClusterCnvAlg( const std::string& name, ISvcLocator* svcLoc );

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
      ToolHandle< ITrigEMClusterCnvTool > m_cnvTool;

   }; // class TrigEMClusterCnvAlg

} // namespace xAODMaker

#endif // XAODTRIGCALOCNV_TRIGEMCLUSTERCNVALG_H
