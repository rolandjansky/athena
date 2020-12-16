/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DERIVATIONFRAMEWORKBPHYS_CASCADE3PLUS1_H
#define DERIVATIONFRAMEWORKBPHYS_CASCADE3PLUS1_H
//*********************
// Cascade3Plus1 header file
//
// Adam Barton <abarton@cern.ch>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODTracking/TrackParticle.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

namespace Trk {
    class IVertexFitter;
    class TrkVKalVrtFitter;
    class VxTrackAtVertex;
    class ITrackSelectorTool;
    class VxCascadeInfo;
    class V0Tools;
}


namespace DerivationFramework {
    class CascadeTools;
}

namespace DerivationFramework {
    static const InterfaceID IID_Cascade3Plus1("Cascade3Plus1", 1, 0);
class Cascade3Plus1 : virtual public AthAlgTool, public IAugmentationTool
{

public:
    static const InterfaceID& interfaceID() { return IID_Cascade3Plus1;}
    Cascade3Plus1(const std::string& t, const std::string& n, const IInterface*  p);
    virtual ~Cascade3Plus1();
    virtual StatusCode initialize() override;
    virtual StatusCode addBranches() const override;

private:
    static constexpr int s_topoN = 2;

    ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
    ToolHandle < Trk::TrkVKalVrtFitter > m_iVertexFitter;
    ToolHandle < Trk::V0Tools >                      m_V0Tools;
    ToolHandle < DerivationFramework::CascadeTools > m_CascadeTools;
    ToolHandle < Analysis::PrimaryVertexRefitter >   m_pvRefitter;
    ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

    std::unique_ptr<Trk::VxCascadeInfo> CascadeFit(std::array<const xAOD::TrackParticle*, 4> &Track)const;

    std::vector<double> m_trackMasses;
    std::vector<std::string> m_cascadeOutputsKeys;
    double m_2trackmassMin = 979.45;
    double m_2trackmassMax = 1059.45;
    double m_3trackmassMin = 1800.47;
    double m_3trackmassMax = 2168.47;
    double m_4trackmassMin = 5200.0;
    double m_4trackmassMax = 5450.0;
    double m_3tracksMass = 1968.47;
    double m_4tracksMass = 5366.79;
    double m_2tracksMass = 0;

    double m_4trackmassFinalMin = 0;
    double m_4trackmassFinalMax = 0;
    std::string m_hypoName;               //!< name of the mass hypothesis. E.g. Jpsi, Upsi, etc. Will be used as a prefix for decorations
    std::string m_3TrackName;
    int         m_PV_max;
    int         m_DoVertexType;
    size_t      m_PV_minNTracks;
    std::string m_VxPrimaryCandidateName;   //!< Name of primary vertex container
    std::string m_refPVContainerName;
    double      m_Chi2NDFCut=0.;
    float       m_3TrackChi2NDFCut=0.;
    double      m_tauCut = -999999;
    bool        m_refitPV;
    bool        m_3TrackMassConstraint = false;
    bool        m_2TrackMassConstraint = false;
    bool        m_eliminateBad3Tracksfrom4Track = false;
    bool        m_copyAllVertices = false;
    std::bitset<4> m_muonTrackBit{0};
    std::vector<int> m_requireMuonsOnTrack;
    std::string m_3TrackVertexOutput;
    std::unique_ptr<xAOD::Vertex> StandardFit(const std::vector<const xAOD::TrackParticle*> &inputTracks, const xAOD::TrackParticleContainer* importedTrackCollection) const;
    std::vector<double> m_ptCutPerTrack;
    std::array<double, 3> m_ptCutPerVertex;
    const std::vector<const xAOD::TrackParticle*>& ApplyAdditionalCuts(const std::vector<const xAOD::TrackParticle*>&, 
       const std::vector<const xAOD::TrackParticle*>&,
       std::vector<const xAOD::TrackParticle*>&, size_t) const;
};
}

#endif
