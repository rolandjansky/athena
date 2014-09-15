// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef XAODEGAMMACNV_PHOTONCNVALG_H
#define XAODEGAMMACNV_PHOTONCNVALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODEgammaCnv/IPhotonCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm creating xAOD::Photons from Analysis::Photons
    *
    *         This algorithm can be used to translate the Photons coming
    *         from an AOD, and create xAOD::Photon objects out of them
    *         for an output xAOD.
    *
    *
    */
   class PhotonCnvAlg : public AthAlgorithm {

   public:
      /// Regular algorithm constructor
      PhotonCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// The key of the input PhotonContainer
      std::string m_aodContainerName;
      /// The key for the output xAOD::PhotonContainer
      std::string m_xaodContainerName;

     /** @brief Tools to perform electron identification*/
     ToolHandle<IPhotonCnvTool>                 m_cnvTool;

   }; // class PhotonCnvAlg

} // namespace xAODMaker

#endif // XAODEGAMMACNV_PHOTONCNVALG_H
