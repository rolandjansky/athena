/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// HDBSa0Finder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: M. Marinescu <mihaela.marinescu@cern.ch>
#ifndef DERIVATIONFRAMEWORK_HDBSA0FINDER_H
#define DERIVATIONFRAMEWORK_HDBSA0FINDER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODEgamma/Electron.h" 

#include "TLorentzVector.h"

namespace Trk
{
    class V0Tools;
    class IVertexFitter;
    class TrkVKalVrtFitter;
    class ITrackSelectorTool;
}

namespace InDet
{
    class IInDetTrackSelectionTool; 
    class VertexPointEstimator;
    class TrackPairsSelector;
}

namespace xAOD{
   class BPhysHelper;
}

namespace DerivationFramework {

class HDBSa0Finder : public AthAlgTool, public IAugmentationTool {

    public:

        HDBSa0Finder(const std::string& t, const std::string& n, const IInterface* p);

        StatusCode initialize() override;
        StatusCode finalize() override;

        virtual StatusCode addBranches() const override;


    private:

        ToolHandle <Trk::V0Tools> m_v0Tools;
        ToolHandle <Trk::IVertexFitter> m_iVertexFitter;
        ToolHandle <InDet::VertexPointEstimator> m_vertexEstimator;
        std::string m_inputTrackParticleContainerName;
        std::string m_TrkParticleGSFCollection;
        std::string m_HCandidateContainerName;
        double m_trackPtMin;
        double m_deltaz0PVsinthetaMax;
        double m_deltaz0PVsignificanceMax;
        double m_trkZDeltaZ;
        double m_ditrackMassMax;
        bool m_ZisMuons;
        bool m_ZisElectrons;
        double m_d0significanceMax;
        double m_HcandidateMassMin;
        double m_chi2cut;
        int m_nHitPix;
        int m_nHitSct;
        bool m_onlyTightPTrk;
        bool m_onlyLoosePTrk;
        bool m_onlyLooseTrk;
        std::string m_electronCollectionKey;
        ToolHandle <Trk::ITrackSelectorTool> m_trkSelector;
        double m_eleThresholdPt;
        double m_leptonTrkThresholdPt;
        double m_muThresholdPt;
        double m_ZMassMin;
        double m_ZMassMax;
        double m_ZeeChi2Cut;
        double m_ZmumuChi2Cut;
        std::string m_muonCollectionKey;
        double m_ditrackPtMin;
        double m_leadingTrackPt;
        double m_deltaPhiTracks;
        double m_deltaRTracks;
        double m_chiSqProbMin;
        std::vector<int> m_useGSFTrackIndices;
        
        ToolHandle <InDet::IInDetTrackSelectionTool> m_TrackSelectionToolTightP;
        ToolHandle <InDet::IInDetTrackSelectionTool> m_TrackSelectionToolLooseP;
        ToolHandle <InDet::IInDetTrackSelectionTool> m_TrackSelectionToolLoose;
        Trk::TrkVKalVrtFitter* m_VKVFitter;
               
        std::bitset<4> m_useGSFTrack;
        const double mass_pion = 139.570; //MeV
        const double mass_muon = 105.658; //MeV
        const double mass_electron = 0.5109989461; //MeV
        const xAOD::TrackParticle* trackParticle1{nullptr};
        const xAOD::TrackParticle* trackParticle2{nullptr};
        const xAOD::Electron* el1{nullptr};
        const xAOD::Electron* el2{nullptr};

        static bool sortDitrackPt (std::tuple<const xAOD::TrackParticle*, const xAOD::TrackParticle*, double> i, std::tuple<const xAOD::TrackParticle*, const xAOD::TrackParticle*, double> j) { 
        double ditrackpt_i = std::get<2>(i);
        double ditrackpt_j = std::get<2>(j);
        return (ditrackpt_i > ditrackpt_j); }

  };
}

#endif // DERIVATIONFRAMEWORK_HDBSa0Finder_H
