/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauPVRefitTool.h
// author: e.bouhova@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TAUPVREFITTOOL_H
#define DERIVATIONFRAMEWORK_TAUPVREFITTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "GaudiKernel/ToolHandle.h"

/**
 * refit any primary vertex in the PV collection that contains any
 * of the TrackParticles provided via a xAOD::TrackParticleContainer
 * set links to original PV container and decorate PV container with
 * links to the refitted PV container
*/

namespace Analysis { class PrimaryVertexRefitter; }

namespace DerivationFramework {

  class TauPVRefitTool : public AthAlgTool, public IAugmentationTool {
    public: 
      TauPVRefitTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:

      std::string m_linkName;
      std::string m_reflinkName;
      std::string m_pvrefContainerName;
      std::string m_pvContainerName;
      std::string m_tauTrkContainerName;

      ToolHandle < Analysis::PrimaryVertexRefitter > m_pvrefitter;

      StatusCode refit(xAOD::VertexContainer*& TauRefittedPrimaryVertices, xAOD::VertexAuxContainer*& TauRefittedPrimaryVerticesAux) const;

  }; 
}

#endif // DERIVATIONFRAMEWORK_TAUPVREFITTOOL_H
