/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGElectronAmbiguityTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGELECTRONAMBIGUITYTOOL_H
#define DERIVATIONFRAMEWORK_EGELECTRONAMBIGUITYTOOL_H

#include <string>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODEgamma/Electron.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

namespace DerivationFramework {

  class EGElectronAmbiguityTool : public AthAlgTool, public IAugmentationTool {
    
    public: 
      EGElectronAmbiguityTool(const std::string& t, const std::string& n, const IInterface* p);

      virtual StatusCode addBranches() const;

  private:
      std::string m_containerName, m_VtxContainerName, m_tpContainerName;

      StatusCode decorateSimple(const xAOD::Electron *ele, const xAOD::Vertex *pvtx) const;
      void helix(const xAOD::TrackParticle *trkP, const xAOD::Vertex *pvtx, std::vector<double>& h) const;

      bool m_isMC;

      // cuts to select the electron to run on
      double m_elepTCut;
      std::string m_idCut;

      // cuts to select the other track 
      unsigned int m_nSiCut;
      double m_dctCut, m_sepCut, m_dzCut;

      // cuts to define the various types :
      // ambi = -1 : no other track, 0 : other track exists but no good gamma reco, 1 : gamma*, 2 : material conversion
      double m_rvECCut, m_meeAtVtxECCut,m_meeICCut;

  }; 
}

#endif // DERIVATIONFRAMEWORK_EGCONVERSIONINFOTOOL_H
