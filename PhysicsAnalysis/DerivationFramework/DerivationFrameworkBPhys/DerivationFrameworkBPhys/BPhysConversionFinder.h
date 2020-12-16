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

        StatusCode doCascadeFit(const xAOD::Vertex * diMuonVertex, const xAOD::Vertex * convVertex, const double diMuonMassConstraint, TLorentzVector & fitMom, float & chiSq) const;

        std::string m_diMuonCollectionToCheck;
        std::vector<std::string> m_passFlagsToCheck;

        ToolHandle <Trk::V0Tools> m_v0Tools;
        ToolHandle <Trk::IVertexFitter> m_vertexFitter;
        ToolHandle <InDet::VertexPointEstimator> m_vertexEstimator;
        ToolHandle <Trk::ITrkDistanceFinder> m_distanceTool;
        ToolHandle <InDet::ConversionPostSelector> m_postSelector;
        ToolHandle <Trk::TrkVKalVrtFitter > m_cascadeFitter;

        std::string m_inputTrackParticleContainerName;
        std::string m_conversionContainerName;

        float m_maxDistBetweenTracks;
        float m_maxDeltaCotTheta;

        bool m_requireDeltaM;
        float m_maxDeltaM;

  };
}

#endif // DERIVATIONFRAMEWORK_BPhysConversionFinder_H
