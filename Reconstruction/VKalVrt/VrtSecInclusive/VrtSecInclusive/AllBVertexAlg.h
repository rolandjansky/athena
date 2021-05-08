/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AllBVertexAlg.h, (c) ATLAS Detector software
// author: Vadim Kostyukhin (vadim.kostyukhin@cern.ch)
///////////////////////////////////////////////////////////////////

/**
 * Reconstruct all B vertices in events, including soft ones
*/

#ifndef VRTSECINCLUSIVE_AllBVertexAlg_H
#define VRTSECINCLUSIVE_AllBVertexAlg_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "VrtSecInclusive/IVrtInclusive.h"


namespace Rec {

  class AllBVertexAlg : public AthAlgorithm {
    public: 

      AllBVertexAlg( const std::string& name, ISvcLocator* pSvcLocator );

      StatusCode initialize() override;
      StatusCode finalize() override;
      StatusCode execute() override;

    private:

      std::string m_bvertexContainerName;
      std::string m_trContainerName;
      std::string m_pvContainerName;

      ToolHandle < Rec::IVrtInclusive > m_bvertextool;
      double vrtVrtDist(const xAOD::Vertex & primVrt, const xAOD::Vertex & secVrt) const;

  };
}

#endif // VRTSECINCLUSIVE_AllBVertexAlg_H
