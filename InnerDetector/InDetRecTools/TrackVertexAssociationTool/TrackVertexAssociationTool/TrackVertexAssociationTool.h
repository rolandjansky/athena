/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKVERTEXASSOCIATIONTOOL_H
#define TRACKVERTEXASSOCIATIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"
#include "AsgDataHandles/ReadHandleKey.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexContainer.h"
#include "TrackVertexAssociationTool/TrackVertexAssociationMap.h"

#include <string>

namespace CP {

  /// General track-vertex association tool
  ///
  /// Supported properties (set via setProperty):
  ///
  /// - WorkingPoint: Working Point to operate on.
  /// - d0_cut: Cut on d0. Not applied if set to -1.
  /// - use_d0sig: flag to cut on d0sig instead of d0.
  /// - d0sig_cut: Cut on \f$ |d_{0}^{BL} \mathrm{significance}| \f$.  Not applied if set to -1.
  /// - dzSinTheta_cut: Cut on \f$ |\Delta z_{0}^{BL} \sin \theta| \f$ (in mm).  Not applied if set to -1.
  /// - doUsedInFit: Control whether to allow for a MatchStatus of UsedInFit.
  /// - requirePriVtx: Control whether a vertex must be VxType::PriVtx in order for a track (not UsedInFit) to be uniquely matched to it
  ///
  /// To apply the recommendations, please visit
  /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackingCPRecsEarly2018#Track_to_Vertex_Association_Tool
  class TrackVertexAssociationTool : public asg::AsgTool, virtual public ITrackVertexAssociationTool
  {
    // MatchCode for the matching
    enum MatchStatus{
      UsedInFit=1, // Track used in the vertex fitting process and doUsedInFit = true
      Matched=2,   // Track matched to the vertex by IP cuts alone
      UnMatch=0};  // Track not matched to the vertex

    ASG_TOOL_CLASS(TrackVertexAssociationTool, ITrackVertexAssociationTool)
    public:
      TrackVertexAssociationTool(const std::string& name);

      virtual StatusCode initialize() override;

      virtual bool isCompatible( const xAOD::TrackParticle &trk, const xAOD::Vertex &vx ) const override;

      virtual xAOD::TrackVertexAssociationMap getMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list, std::vector< const xAOD::Vertex *> &vx_list) const override;
      virtual xAOD::TrackVertexAssociationMap getMatchMap( const xAOD::TrackParticleContainer &trkCont, const xAOD::VertexContainer &vxCont ) const override;

      virtual ElementLink< xAOD::VertexContainer> getUniqueMatchVertexLink(const xAOD::TrackParticle &, const xAOD::VertexContainer &vxCont) const override;
      virtual const xAOD::Vertex* getUniqueMatchVertex( const xAOD::TrackParticle &trk, std::vector< const xAOD::Vertex *> &vx_list) const override;

      virtual xAOD::TrackVertexAssociationMap getUniqueMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list , std::vector< const xAOD::Vertex *> &vx_list) const override;
      virtual xAOD::TrackVertexAssociationMap getUniqueMatchMap( const xAOD::TrackParticleContainer &trkCont , const xAOD::VertexContainer &vxCont) const override;

    private:
      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo { this, "EventInfo", "EventInfo", "EventInfo key" };

      /// Checks if a track-vertex pair passes the cuts.  Returns
      /// deltaz * sin theta of the pair in `dzSinTheta` if successful.
      MatchStatus isMatch(const xAOD::TrackParticle &trk, const xAOD::Vertex &vx,
                   float &dzSinTheta) const;

      template <typename T, typename U>
      xAOD::TrackVertexAssociationMap getMatchMapInternal(T &trk_list,
                                                      U &vx_list) const;

      template <typename T, typename U>
      xAOD::TrackVertexAssociationMap getUniqueMatchMapInternal(T &trk_list,
                                                            U &vx_list) const;

      template <typename T>
      const xAOD::Vertex *getUniqueMatchVertexInternal(const xAOD::TrackParticle &trk,
                                                   T &vx_list) const;

      // check whether TrackParticle is used in the fit of a given vertex
      bool trackParticleUsedInVertexFit(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx) const;

      // check whether TrackParticle is used in the fit of ANY vertex
      bool trackParticleUsedInVertexFit(const xAOD::TrackParticle& trk) const;

      // Working Point to operate on
      std::string m_wp;

      // Cut on d0
      float m_d0_cut;
      // flag to cut on d0sig instead of d0
      bool m_use_d0sig;
      // Cut on d0 significance
      float m_d0sig_cut;

      // Cut on dz*sin theta
      float m_dzSinTheta_cut;

      // Control whether to allow for a MatchStatus of UsedInFit
      bool m_doUsedInFit;

      // Control whether a vertex must be VxType::PriVtx in order for a track (not UsedInFit) to be uniquely matched to it
      bool m_requirePriVtx;

      SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey { this, "VertexContainer", "PrimaryVertices",
                                                       "container for reconstructed primary vertices" };

  };

} // namespace CP

#endif // TRACKVERTEXASSOCIATIONTOOL_H
