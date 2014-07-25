// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigMinBiasCNV_TrigSpacePointCountsCNVALG_H
#define xAODTrigMinBiasCNV_TrigSpacePointCountsCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigMinBiasCnv/ITrigSpacePointCountsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for creating xAOD::TrigSpacePointCountsContainer from an AOD
    *
    *         This algorithm can be used to create an xAOD muon RoI container
    *         out of the objects found in an AOD file.
    *
    */
   class TrigSpacePointCountsCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      TrigSpacePointCountsCnvAlg( const std::string& name, ISvcLocator* svcLoc );

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
      ToolHandle< ITrigSpacePointCountsCnvTool > m_cnvTool;

   }; // class TrigSpacePointCountsCnvAlg

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_TrigSpacePointCountsCNVALG_H
