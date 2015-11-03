// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef XAODFORWARDCNV_FORWARDEVENTINFOCNVALG_H
#define XAODFORWARDCNV_FORWARDEVENTINFOCNVALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODForwardCnv/IForwardEventInfoCnvTool.h"

namespace xAODMaker {

   /**
    * update for mbts
    *  @short Algorithm creating xAOD::ForwardEventInfos from TileCellContainer
    *
    *         This algorithm can be used to translate the TileCells coming
    *         from an AOD, and create xAOD::ForwardEventInfo objects out of them
    *         for an output xAOD.
    *
    *
    */
   class ForwardEventInfoCnvAlg : public AthAlgorithm {

   public:
     ///@brief Regular algorithm constructor
     ForwardEventInfoCnvAlg( const std::string& name, ISvcLocator* svcLoc );
     
      ///@brief Function initialising the algorithm
     virtual StatusCode initialize();
     ///@brief Function executing the algorithm
     virtual StatusCode execute();
     
   private:

     ///@brief  The key of the input ForwardEventInfoContainer
     std::string m_aodContainerName;

     ///@brief The key for the output xAOD::ForwardEventInfoContainer
     std::string m_xaodContainerName;

     /** @brief Handle to the converter tool */
     ToolHandle<IForwardEventInfoCnvTool>                 m_cnvTool;


  }; // class ForwardEventInfoCnvAlg

} // namespace xAODMaker

#endif // XAODFORWARDCNV_FORWARDEVENTINFOCNVALG_H
