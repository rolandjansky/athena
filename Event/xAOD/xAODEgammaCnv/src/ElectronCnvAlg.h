// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef XAODEGAMMACNV_ELECTRONCNVALG_H
#define XAODEGAMMACNV_ELECTRONCNVALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODEgammaCnv/IElectronCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm creating xAOD::Electrons from Analysis::Electrons
    *
    *         This algorithm can be used to translate the Electrons coming
    *         from an AOD, and create xAOD::Electron objects out of them
    *         for an output xAOD.
    *
    *
    */
   class ElectronCnvAlg : public AthAlgorithm {

   public:
     ///@brief Regular algorithm constructor
     ElectronCnvAlg( const std::string& name, ISvcLocator* svcLoc );
     
      ///@brief Function initialising the algorithm
     virtual StatusCode initialize();
     ///@brief Function executing the algorithm
     virtual StatusCode execute();
     
   private:

     ///@brief  The key of the input ElectronContainer
     std::string m_aodContainerName;

     ///@brief The key for the output xAOD::ElectronContainer
     std::string m_xaodContainerName;
     std::string m_xaodFrwdContainerName;


     /** @brief Tools to perform electron identification*/
     ToolHandle<IElectronCnvTool>                 m_cnvTool;


  }; // class ElectronCnvAlg

} // namespace xAODMaker

#endif // XAODEGAMMACNV_ELECTRONCNVALG_H
