// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRNNOutputCnvAlg.h  $
#ifndef XAODTRIGRINGERCNV_TRIGRNNOUTPUTCNVALG_H
#define XAODTRIGRINGERCNV_TRIGRNNOUTPUTCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigRingerCnv/ITrigRNNOutputCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for creating xAOD::RingerRNNOutput from an AOD
    *
    *         This algorithm can be used to create an xAOD RingerRNNOutput
    *         out of the object found in an AOD file.
    *
    * @author joao victor da fonseca pinto <joao.victor.da.fonseca.pinto@cern.ch>
    *
    * $Revision:  $
    * $Date:  $
    */
   class TrigRNNOutputCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      TrigRNNOutputCnvAlg( const std::string& name, ISvcLocator* svcLoc );

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
      ToolHandle< ITrigRNNOutputCnvTool > m_cnvTool;

   }; // class RingerRNNOutputCnvAlg

} // namespace xAODMaker

#endif // XAOD
