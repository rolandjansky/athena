/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauPVTrkSelectionTool.h
// author: e.bouhova@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TAUPVTRKSELECTIONTOOL_H
#define DERIVATIONFRAMEWORK_TAUPVTRKSELECTIONTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TauAnalysisTools/ITauTruthTrackMatchingTool.h"

/**
 * tool for selecting tracks associated to tau candidates (xAOD::TauJetContainer)
 * for the purpose of PV refit, returns a xAOD::TrackParticleContainer
 * if UseTrueTracks = true, use truth matched tracks only
*/

namespace DerivationFramework {

  class TauPVTrkSelectionTool : public AthAlgTool, public IAugmentationTool {
    public: 
      TauPVTrkSelectionTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:

      std::string m_tauPVTracksContainerName;
      std::string m_tauContainerName;

      bool m_useTruth;
      double m_maxDeltaR;
      double m_minPt;

      ToolHandle < TauAnalysisTools::ITauTruthTrackMatchingTool > m_T3MT;

      StatusCode select(const xAOD::TrackParticleContainer*& tauPVTracks) const;

  }; 
}

#endif // DERIVATIONFRAMEWORK_TAUPVTRKSELECTIONTOOL_H
