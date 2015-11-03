// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef XAODFORWARDCNV_MBTSMODULECNVALG_H
#define XAODFORWARDCNV_MBTSMODULECNVALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODForwardCnv/IMBTSModuleCnvTool.h"

namespace xAODMaker {

   /**
    * update for mbts
    *  @short Algorithm creating xAOD::MBTSModules from TileCellContainer
    *
    *         This algorithm can be used to translate the TileCells coming
    *         from an AOD, and create xAOD::MBTSModule objects out of them
    *         for an output xAOD.
    *
    *
    */
   class MBTSModuleCnvAlg : public AthAlgorithm {

   public:
     ///@brief Regular algorithm constructor
     MBTSModuleCnvAlg( const std::string& name, ISvcLocator* svcLoc );
     
      ///@brief Function initialising the algorithm
     virtual StatusCode initialize();
     ///@brief Function executing the algorithm
     virtual StatusCode execute();
     
   private:

     ///@brief  The key of the input MBTSModuleContainer
     std::string m_aodContainerName;

     ///@brief The key for the output xAOD::MBTSModuleContainer
     std::string m_xaodContainerName;

     /** @brief Handle to the converter tool */
     ToolHandle<IMBTSModuleCnvTool>                 m_cnvTool;


  }; // class MBTSModuleCnvAlg

} // namespace xAODMaker

#endif // XAODFORWARDCNV_MBTSMODULECNVALG_H
