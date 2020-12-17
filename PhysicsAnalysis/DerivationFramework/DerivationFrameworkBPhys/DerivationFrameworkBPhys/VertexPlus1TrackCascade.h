/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VERTEXPLUS1TRACKCASCADE_H
#define VERTEXPLUS1TRACKCASCADE_H
//*********************
// VertexPlus1Cascade header file
//
// Adam Barton <abarton@cern.ch>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include <vector>


namespace Trk {
    class IVertexFitter;
    class ITrackSelectorTool;
    class TrkVKalVrtFitter;
    class IVertexCascadeFitter;
    class VxCascadeInfo;
}

namespace DerivationFramework {

    static const InterfaceID IID_VertexPlus1TrackCascade("VertexPlus1TrackCascade", 1, 0);

    class VertexPlus1TrackCascade : virtual public AthAlgTool
    {
        

        SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainerKey;
        SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackPContainerKey;
        SG::ReadHandleKey<xAOD::MuonContainer>  m_MuonsUsedInJpsiKey;

        std::vector<double> m_massHypothesis;

        std::vector<int> m_massConstraintTracksVtx1;
        std::vector<int> m_massConstraintTracksVtx2;
        double m_Vtx1MassConstraint;
        double m_Vtx2MassConstraint;

        double m_trkThresholdPt;
        double m_trkMaxEta;
//        double m_BThresholdPt;
//        double m_BMassUpper;
//        double m_BMassLower;

        double m_roughMassLower;
        double m_roughMassUpper;
        ToolHandle < Trk::TrkVKalVrtFitter > m_iVertexFitter;
        ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;

    public:
        static const InterfaceID& interfaceID() { return IID_VertexPlus1TrackCascade;}
        VertexPlus1TrackCascade(const std::string& t, const std::string& n, const IInterface*  p);
        ~VertexPlus1TrackCascade();
        StatusCode initialize() override;
        StatusCode finalize() override;
        static double getInvariantMass(const std::vector<const xAOD::TrackParticle*> &Tracks, const std::vector<double> &massHypotheses);
        static bool isContainedIn(const xAOD::TrackParticle* theTrack, const xAOD::MuonContainer* theColl);
        StatusCode performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer ) const;

    };
}


#endif

