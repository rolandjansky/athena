/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODVERTEXCNV_VERTEXCNVALG_H
#define XAODVERTEXCNV_VERTEXCNVALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
namespace xAODMaker {

   /**
    *  @short Algorithm creating xAOD::Vertex from Vertex
    *
    *         This algorithm can be used to translate the Vertexs coming
    *         from an AOD, and create xAOD::Vertex objects out of them
    *         for an output xAOD.
    *
    * @author Ruslan Mashinistov <Ruslan.Mashinistov@cern.ch>
    *
    */
   class VertexCnvAlg : public AthAlgorithm {

   public:
      /// Regular algorithm constructor
      VertexCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// The key of the input Container
      SG::ReadHandle<VxContainer> m_aod;
      
      /// The key for the output xAOD::Container
      SG::WriteHandle<xAOD::VertexContainer> m_xaodout;
      SG::WriteHandle<xAOD::VertexAuxContainer> m_xauxout;      
      
      /// The key for the track particle links Container
      std::string m_TPContainerName;
      /// The key for the neutral particle links Container
      std::string m_NPContainerName;

   }; // class VertexCnvAlg

} // namespace xAODMaker

#endif // XAODVERTEXCNV_VERTEXCNVALG_H
