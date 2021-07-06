/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NewVrtSecInclusiveAlg.h, (c) ATLAS Detector software
// author: Vadim Kostyukhin (vadim.kostyukhin@cern.ch)
///////////////////////////////////////////////////////////////////

/**
* Example algorithm to run the NewVrtSecInclusive algorithm and save results to StoreGate
*/

#ifndef VKalVrt_NewVrtSecInclusiveAlg_H
#define VKalVrt_NewVrtSecInclusiveAlg_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "NewVrtSecInclusiveTool/IVrtInclusive.h"


namespace Rec {

   class NewVrtSecInclusiveAlg : public AthAlgorithm {
     public: 

       NewVrtSecInclusiveAlg( const std::string& name, ISvcLocator* pSvcLocator );

       StatusCode initialize() override;
       StatusCode execute() override;
       StatusCode finalize() override;

    private:

      std::string m_tpContainerName;
      std::string m_pvContainerName;

      SG::WriteHandle<xAOD::VertexContainer>  m_foundVertices;
      ToolHandle < Rec::IVrtInclusive >       m_bvertextool;
  };
}

#endif
