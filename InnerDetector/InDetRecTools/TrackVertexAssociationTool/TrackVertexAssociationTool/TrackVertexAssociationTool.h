/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKVERTEXASSOCIATIONTOOL_H
#define TRACKVERTEXASSOCIATIONTOOL_H

// Includes from this package
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/TrackVertexAssociationMap.h"

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/CurrentContext.h"
#include "AthLinks/ElementLink.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/ReadDecorHandle.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"

// STL includes
#include <string>
#include <vector>

namespace CP {

  /// General track-vertex association tool
  ///
  /// Supported properties (set via setProperty):
  ///
  /// - WorkingPoint: Working Point to operate on.
  /// - d0_cut: Cut on d0. Not applied if set to -1.
  /// - use_d0sig: flag to cut on d0sig instead of d0.
  /// - d0sig_cut: Cut on d0Sig. Not applied if set to -1.
  /// - dzSinTheta_cut: Cut on |dz*sinTheta| (in mm).  Not applied if set to -1.
  /// - doUsedInFit: Control whether to allow for a MatchStatus of UsedInFit.
  /// - requirePriVtx: Control whether a vertex must be VxType::PriVtx in order for a track (not UsedInFit) to be uniquely matched to it.
  /// - HardScatterLinkDeco: The decoration name of the ElementLink to the hardscatter vertex (found on xAOD::EventInfo)
  ///
  /// To apply the recommendations, please visit
  /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackingCPRecsEarly2018#Track_to_Vertex_Association_Tool

  class TrackVertexAssociationTool
    : public asg::AsgTool, virtual public ITrackVertexAssociationTool
  {

    ASG_TOOL_CLASS(TrackVertexAssociationTool, ITrackVertexAssociationTool)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /// @name Public helper class for applying the difference TTVA WPs

    typedef SG::ReadDecorHandle<xAOD::TrackParticleContainer, std::vector<ElementLink<xAOD::VertexContainer>>> AMVFVerticesRDH;
    typedef SG::ReadDecorHandle<xAOD::TrackParticleContainer, std::vector<float>> AMVFWeightsRDH;
    class WorkingPoint {
    public:
      virtual ~WorkingPoint() = default;
      virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo* evt,
        const AMVFVerticesRDH& vtxDeco, const AMVFWeightsRDH& wgtDeco) const = 0;
    };

    /// @}

    TrackVertexAssociationTool(const std::string& name);

    virtual StatusCode initialize() override;

    virtual bool isCompatible(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx) const override;
    virtual bool isCompatible(const xAOD::TrackParticle& trk) const override;

    virtual xAOD::TrackVertexAssociationMap getMatchMap(std::vector<const xAOD::TrackParticle*>& trk_list, std::vector<const xAOD::Vertex*>& vx_list) const override;
    virtual xAOD::TrackVertexAssociationMap getMatchMap(const xAOD::TrackParticleContainer& trkCont, const xAOD::VertexContainer& vxCont) const override;

    virtual ElementLink<xAOD::VertexContainer> getUniqueMatchVertexLink(const xAOD::TrackParticle& trk, const xAOD::VertexContainer& vxCont) const override;
    virtual const xAOD::Vertex* getUniqueMatchVertex(const xAOD::TrackParticle& trk, std::vector<const xAOD::Vertex*>& vx_list) const override;

    virtual xAOD::TrackVertexAssociationMap getUniqueMatchMap(std::vector<const xAOD::TrackParticle*>& trk_list, std::vector<const xAOD::Vertex*>& vx_list) const override;
    virtual xAOD::TrackVertexAssociationMap getUniqueMatchMap(const xAOD::TrackParticleContainer& trkCont , const xAOD::VertexContainer& vxCont) const override;

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
  private:

    bool isMatch(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo* evtInfo = nullptr) const;

    template <typename T, typename U>
    xAOD::TrackVertexAssociationMap getMatchMapInternal(T& trk_list, U& vx_list) const;

    template <typename T, typename U>
    xAOD::TrackVertexAssociationMap getUniqueMatchMapInternal(T& trk_list, U& vx_list) const;

    template <typename T>
    const xAOD::Vertex* getUniqueMatchVertexInternal(const xAOD::TrackParticle& trk, T& vx_list) const;

    /// @name The properties that can be defined via the python job options
    /// @{

    /// Working Point to operate on.
    std::string m_wp;

    /// Cut on d0. Not applied if set to -1.
    float m_d0_cut;

    /// Flag to cut on d0sig instead of d0.
    bool m_use_d0sig;

    /// Cut on d0Sig. Not applied if set to -1.
    float m_d0sig_cut;

    /// Cut on |dz*sinTheta| (in mm).  Not applied if set to -1.
    float m_dzSinTheta_cut;

    /// Flag to cut on d0sig instead of d0.
    bool m_doUsedInFit;

    /// Flag to cut on d0sig instead of d0.
    bool m_requirePriVtx;

    /// The decoration name of the ElementLink to the hardscatter vertex (found on xAOD::EventInfo)
    std::string m_hardScatterDeco;

    /// xAOD::TrackParticleContaier name (for building the accessors below)
    std::string m_trkContName;

    /// AMVF vertices decoration name
    std::string m_vtxDecoName;

    /// AMVF weights decoration name
    std::string m_wgtDecoName;

    /// @}

  private:

    /// @name Truly private internal data members
    /// @{

    /// EventInfo key
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo {this, "EventInfo", "EventInfo", "EventInfo key"};

    /// AMVF vertices decoration key
    SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> m_vtxDecoKey;

    /// AMVF weights decoration key
    SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> m_wgtDecoKey;

    /// Hardscatter vertex link key
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_hardScatterDecoKey;

    /// Stored WorkingPoint class
    std::unique_ptr<WorkingPoint> m_applicator;

    /// @}

  };

} // namespace CP

#endif // TRACKVERTEXASSOCIATIONTOOL_H
