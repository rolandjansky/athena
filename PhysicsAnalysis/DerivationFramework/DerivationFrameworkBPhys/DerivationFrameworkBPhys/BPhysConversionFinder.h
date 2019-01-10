/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// BPhysConversionFinder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: A. Chisholm <andrew.chisholm@cern.ch>
#ifndef DERIVATIONFRAMEWORK_BPHYSCONVERSIONFINDER_H
#define DERIVATIONFRAMEWORK_BPHYSCONVERSIONFINDER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "InDetConversionFinderTools/ConversionPostSelector.h"
#include "TrkVertexSeedFinderUtils/ITrkDistanceFinder.h"

#include "TLorentzVector.h"

namespace Trk
{
    class V0Tools;
    class IVertexFitter;
    class TrkVKalVrtFitter;
}

namespace InDet
{
    class VertexPointEstimator;
    class TrackPairsSelector;
    class ConversionPostSelector;
}

namespace DerivationFramework {

class BPhysConversionFinder : public AthAlgTool, public IAugmentationTool {

    public:

        BPhysConversionFinder(const std::string& t, const std::string& n, const IInterface* p);

        StatusCode initialize() override;
        StatusCode finalize() override;

        virtual StatusCode addBranches() const override;

    private:

        StatusCode DoCascadeFit(const xAOD::Vertex * DiMuonVertex, const xAOD::Vertex * ConvVertex, const double DiMuonMassConstraint, TLorentzVector & FitMom, float & ChiSq) const;

        std::string m_DiMuonCollectionToCheck;
        std::vector<std::string> m_PassFlagsToCheck;

        ToolHandle <Trk::V0Tools> m_V0Tools;
        ToolHandle <Trk::IVertexFitter> m_VertexFitter;
        ToolHandle <InDet::VertexPointEstimator> m_VertexEstimator;
        ToolHandle <Trk::ITrkDistanceFinder> m_DistanceTool;
        ToolHandle <InDet::ConversionPostSelector> m_PostSelector;
        ToolHandle <Trk::TrkVKalVrtFitter > m_CascadeFitter;

        std::string m_InputTrackParticleContainerName;
        std::string m_ConversionContainerName;
        std::string m_CascadeParticleContainerName;

        float m_MaxDistBetweenTracks;
        float m_MaxDeltaCotTheta;

        bool m_RequireDeltaM;
        float m_MaxDeltaM;

  };
}

#endif // DERIVATIONFRAMEWORK_BPhysConversionFinder_H
