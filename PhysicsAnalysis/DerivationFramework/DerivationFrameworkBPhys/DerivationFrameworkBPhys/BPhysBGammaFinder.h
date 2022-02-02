/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// BPhysBGammaFinder header file
//
// Tatiana Lyubushkina <tatiana.lyubushkina@cern.ch>

// ----------------------------------------------------------------------------
// ****************************************************************************
#ifndef DERIVATIONFRAMEWORK_BPHYSBGAMMAFINDER_H
#define DERIVATIONFRAMEWORK_BPHYSBGAMMAFINDER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h" //ParticleMasses struct

#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "xAODBPhys/BPhysHelper.h"

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
}

namespace DerivationFramework {
	
class BPhysPVTools;

class BPhysBGammaFinder : public AthAlgTool, public IAugmentationTool {

    public:

        BPhysBGammaFinder(const std::string& t, const std::string& n, const IInterface* p);

        StatusCode initialize() override;
        StatusCode finalize() override;

        virtual StatusCode addBranches() const override;
        TVector3 trackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const;

    private:

        std::vector<std::string> m_BVertexCollectionsToCheck;
        std::vector<std::string> m_passFlagsToCheck;

        ToolHandle <Trk::V0Tools> m_v0Tools;
        ToolHandle <Trk::IVertexFitter> m_vertexFitter;
        ToolHandle <InDet::VertexPointEstimator> m_vertexEstimator;

        std::string m_inputTrackParticleContainerName;
        std::string m_inputLowPtTrackContainerName;
        std::string m_conversionContainerName;

        float m_maxDistBetweenTracks;
        float m_maxDeltaCotTheta;

        bool m_requireDeltaQ;
        bool m_use_low_pT;
        float m_maxDeltaQ;
        float m_Chi2Cut;
        float m_maxGammaMass;
        
        std::unique_ptr< BPhysPVTools > m_BPhysPVTools;

  };
}

#endif // DERIVATIONFRAMEWORK_BPhysBGammaFinder_H
